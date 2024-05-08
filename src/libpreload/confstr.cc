/* confstr SUPPORT */
/* lang=C++20 */

/* Configuration-String UNIX® System interposer */
/* version %I% last-modified %G% */


/* revision history:

	= 2000-05-14, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2000 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This is a version of |confstr(3c)| that is preloaded to over-ride
	the standard UNIX® system version.

	Q. Is this multi-thread safe?
	A. Since it is a knock-off of an existing UNIX® system LIBC (3c)
	   subroutine that is already multi-thread safe -- then of course
	   it is!

	Q. Is this muc slower than the default system version?
	A. No, not really.

	Q. How are we smarter than the default system version?
	A. Let me count the ways:
		+ value optionally from environment
		+ value optionally from a configuration file
		+ customizable built-in compiled default
		+ value is cached w/ a Time-To-live (TTL)!

	Q. Why are you so smart?
	A. I do not know.


*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<unistd.h>
#include	<dlfcn.h>
#include	<cerrno>
#include	<cstddef>
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<buffer.h>
#include	<vecstr.h>
#include	<sfx.h>
#include	<vstrkeycmpx.h>
#include	<isnot.h>
#include	<localmisc.h>

#include	"preload.h"


/* local defines */

#define	CONFSTR_CSPATH	_CS_PATH
#define	CONFSTR_BUFLEN	(10*MAXPATHLEN)
#define	CONFSTR_TTL	(2*3600)	/* Time-To-Live two (2) hours */
#define	CONFSTR_FNAME	"/etc/default/confstr"
#define	CONFSTR_SBIN	"/usr/sbin"

#ifndef	VARCSPATH
#define	VARCSPATH	"CSPATH"
#endif

#ifndef	UIDSYS
#define	UIDSYS		100
#endif


/* external subroutines */


/* local structures */


/* forward references */

static size_t	confstr_cspath(char *,size_t) ;
static size_t	confstr_next(int,char *,size_t) ;

static int	confstr_var(buffer *) ;
static int	confstr_file(buffer *) ;
static int	confstr_def(buffer *) ;
static int	confstr_defsbin(buffer *) ;
static int	confstr_defer(buffer *,int,cchar *) ;


/* local variables */

static int	(*confstrs[])(buffer *) = {
	confstr_var,
	confstr_file,
	confstr_def,
	nullptr
} ;

static cchar	*comps[] = {
	"/usr/preroot/bin",
	"/usr/xpg4/bin",
	"/usr/ccs/bin",
	"/usr/bin",
	"/usr/SUNWspro/bin",
	"/usr/extra/bin",
	nullptr
} ;


/* exported variables */


/* exported subroutines */

size_t confstr(int name,char *rbuf,size_t len) noex {
	size_t		rc = 0 ;
	if (name == CONFSTR_CSPATH) {
	    rc = confstr_cspath(rbuf,len) ;
	} else {
	    rc = confstr_next(name,rbuf,len) ;
	} /* end if */
	return rc ;
}
/* end subroutine (confstr) */


/* local subroutines */

extern "C" {
    typedef	size_t (*func_confstr)(int,char *,size_t) noex ;
}

static size_t confstr_next(int name,char *rbuf,size_t len) noex {
	size_t		rc = 0 ;
	void		*sp = dlsym(RTLD_NEXT,"confstr") ;
	if (sp) {
	    func_confstr	func = (func_confstr) sp ;
	    rc = (*func)(name,rbuf,len) ;
	}
	return rc ;
}
/* end subroutine (confstr_next) */

static size_t confstr_cspath(char *ubuf,size_t len) noex {
	cint		rlen = CONFSTR_BUFLEN ;
	int		rs ;
	int		rs1 ;
	size_t		rc = 0 ;
	char		*rbuf ;
	if ((rs = uc_malloc((rlen+1),&rbuf)) >= 0) {
	    cint	di = PRELOAD_DCSPATH ;
	    if ((rs = preload_get(di,rbuf,rlen)) == 0) {
		buffer	b ;
		cint		blen = MAXNAMELEN ;
		if ((rs = buffer_start(&b,blen)) >= 0) {
		    for (int i = 0 ; confstrs[i] != nullptr ; i += 1) {
			rs = (*confstrs[i])(&b) ;
			if (rs != 0) break ;
		    } /* end for */
		    if (rs >= 0) {
			rs = confstr_defsbin(&b) ;
		    }
		    if (rs >= 0) {
	    		cchar	*bp ;
	   		if ((rs = buffer_get(&b,&bp)) >= 0) {
			    cint	ttl = CONFSTR_TTL ;
			    if ((rs = preload_set(di,bp,rs,ttl)) >= 0) {
	    		        rc = (strlcpy(ubuf,bp,len)+1) ;
			    } /* end if (preload_set) */
			} /* end if (buffer_get) */
		    } /* end if (positive response) */
	    	    rs1 = buffer_finish(&b) ;
	    	    if (rs >= 0) rs = rs1 ;
		} /* end if (buffer) */
	    } else {
		rc = (strlcpy(ubuf,rbuf,len)+1) ;
	    } /* end if (cache) */
	    rs1 = uc_free(rbuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a) */
	if (rs < 0) errno = (- rs) ;
	return (rs >= 0) ? rc : 0 ;
}
/* end subroutine (confstr_cspath) */

static int confstr_var(buffer *bdp) noex {
	int		rs = SR_OK ;
	cchar		*vp ;
	if ((vp = getenv(VARCSPATH)) != nullptr) {
	    if (vp[0] != '\0') {
	        rs = buffer_strw(bdp,vp,-1) ;
	    }
	}
	return rs ;
}
/* end subroutine (confstr_var) */

static int confstr_file(buffer *bdp) noex {
	vecstr		env ;
	int		rs ;
	int		rs1 ;
	int		rl = 0 ;
	if ((rs = vecstr_start(&env,0,0)) >= 0) {
	    cchar	*fname = CONFSTR_FNAME ;
	    if ((rs = vecstr_envfile(&env,fname)) >= 0) {
		cchar	*var = VARCSPATH ;
		cchar	*ep ;
		if ((rs = vecstr_finder(&env,var,vstrkeycmp,&ep)) >= 0) {
		    if (ep != nullptr) {
		        cchar	*tp = strchr(ep,'=') ;
			if ((tp != nullptr) && (tp[1] != '\0')) {
			    rs = buffer_strw(bdp,(tp+1),-1) ;
			    rl = rs ;
			} /* end if (have proper ENV variable) */
		    } /* end if (non-null) */
		} /* end if (vecstr_finder) */
	    } /* end if (vecstr_envfile) */
	    if (isNotPresent(rs)) rs = SR_OK ;
	    rs1 = vecstr_finish(&env) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (vecstr) */
	return (rs >= 0) ? rl : rs ;
}
/* end subroutine (confstr_file) */

static int confstr_def(buffer *bdp) noex {
	int		rs = SR_OK ;
	int		rl = 0 ;
	for (int i = 0 ; (rs >= 0) && comps[i] ; i += 1) {
	    USTAT	sb ;
	    cchar	*dname = comps[i] ;
	    if ((rs = u_stat(dname,&sb)) >= 0) {
		if (S_ISDIR(sb.st_mode)) {
		    rs = confstr_defer(bdp,i,dname) ;
		    rl += rs ;
		} /* end if (directory) */
	    } else if (isNotPresent(rs))
		rs = SR_OK ;
	} /* end for */
	return (rs >= 0) ? rl : rs ;
}
/* end subroutine (confstr_def) */

/* super El-Cheapo (but correct) way to append a path-component to the end! */
static int confstr_defsbin(buffer *bdp) noex {
	const uid_t	uid = getuid() ;
	int		rs = SR_OK ;
	int		rl = 0 ;
	if (uid < UIDSYS) {
	    cchar	*bp ;
	    if ((rs = buffer_get(bdp,&bp)) >= 0) {
		int	bl = rs ;
	        cchar	*dname = CONFSTR_SBIN ;
	        cchar	*tp ;
	        if (sfsub(bp,bl,dname,nullptr) < 0) {
		    int	f = ((bl > 0) && (bp[bl-1] != ':')) ;
		    rs = confstr_defer(bdp,f,dname) ;
		    rl += rs ;
		} /* end if (sfsub) */
		if (rs >= 0) {
	            if ((rs = buffer_get(bdp,&bp)) >= 0) {
		        bl = rs ;
		        if ((bl > 0) && (bp[bl-1] != ':')) {
		            rs = buffer_char(bdp,':') ;
		            rl += rs ;
		        } /* end if (adding) */
		    } /* end if (buffer_get) */
		} /* end if (ok) */
	    } /* end if (buffer_get) */
	} /* end if (system user) */
	return (rs >= 0) ? rl : rs ;
}
/* end subroutine (confstr_defsbin) */

static int confstr_defer(buffer *bdp,int i,cchar *dname) noex {
	int		rs = SR_OK ;
	int		rl = 0 ;
	if (i > 0) {
	    rs = buffer_char(bdp,':') ;
	    rl += rs ;
	}
	if (rs >= 0) {
	    rs = buffer_strw(bdp,dname,-1) ;
	    rl += rs ;
	}
	return (rs >= 0) ? rl : rs ;
}
/* end subroutine (confstr_defer) */


