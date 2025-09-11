/* ucopendialer (open-dialer-service) */
/* charset=ISO8859-1 */
/* lang=C20 */

/* interface component for UNIX® library-3c */
/* open a dialer */
/* version %I% last-modified %G% */

#define	CF_GETEXECNAME	1		/* use 'getexecname(3c)' */

/* revision history:

	= 1998-07-10, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	uc_opendialer

	Description:
	Dialer services are represented as files with names of the form:
		<dialer>¥<svc>[­<arg(s)>
	These sorts of file names are often actually stored in the
	filesystem as symbolic links.

	Synopsis:
	int uc_opendialer(prn,svc,of,om,argv,envv,to)
	cchar	prn[] ;
	cchar	svc[] ;
	int		of ;
	mode_t		om ;
	cchar	**argv[] ;
	cchar	**envv[] ;
	int		to ;

	Arguments:
	prn		facility name
	svc		service name
	of		open-flags
	om		open-mode
	argv		array of arguments
	envv		attay of environment
	to		time-out

	Returns:
	>=0		file-descriptor
	<0		error (system-return)

	Dialer services are implemented with loadable shared-object
	files. Each service has a file of the same name as the
	service name itself. The file is a shared-object with a
	global symbol of a callable subroutine with the name
	'opendialer_<svc>' where <svc> is the service name. The
	subroutine looks like:

	int opendialer_<svc>(pr,prn,of,om,argv,envv,to)
	cchar	*pr ;
	cchar	*prn ;
	int		of ;
	mode_t		om ;
	cchar	*argv[] ;
	cchar	*envv[] ;

	Multiple services can be actually implemented in the same
	shared-object.  But the actual file of that object should
	be linked to other files, each with the filename of a service
	to be implemented. These links are required because this
	code only searches for services by searching for files with
	the names of the services.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/stat.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<dlfcn.h>
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<getnodedomain.h>
#include	<ids.h>
#include	<sncpyx.h>
#include	<mkx.h>
#include	<mkpr.h>
#include	<mkpathx.h>
#include	<permx.h>
#include	<isnot.h>
#include	<localmisc.h>

import libutil ;

/* local defines */

#define	SI		subinfo
#define	SI_FL		subinfo_flags

#define	SVCDNAME	"lib/opendialers"
#define	SVCSYMPREFIX	"opendialer_"

#ifndef	SVCLEN
#define	SVCLEN		MAXNAMELEN
#endif


/* imported namespaces */


/* local typedefs */

typedef mainv		mv ;
typedef mode_t		m_t ;


/* external subroutines */


/* external variables */

extern char	**environ ;


/* local structures */

extern "C" {
    typedef int (*subdialer_t)(cc *,cc *,cc *,int,mode_t,mainv,mainv,int) noex ;
}

struct subinfo_flags {
	uint		dummy:1 ;
} ;

struct subinfo {
	ids		id ;
	SI_FL		fl ;
	cchar		*prn ;
	cchar		*svc ;
	char		*dialsym ;	/* memory-allocated */
	mainv		argv ;
	mainv		envv ;
	int		of ;
	int		to ;
	int		fd ;
	mode_t		om ;
} ;


/* forward references */

static int	subinfo_start(SI *,cchar *,cchar *,
			int,mode_t,mainv,mainv,int) noex ;
static int	subinfo_finish(SI *) noex ;
static int	subinfo_search(SI *) noex ;
static int	subinfo_exts(SI *,cchar *,cchar *,char *) noex ;
static int	subinfo_searchlib(SI *,cchar *,cchar *) noex ;
static int	subinfo_searchcall(SI *,cchar *,subdialer_t) noex ;
static int	subinfo_idbegin(SI *) noex ;
static int	subinfo_idend(SI *) noex ;


/* local variables */

static cpcchar		prns[] = {
	"extra",
	"preroot",
	nullptr
} ;

static cpcchar		soexts[] = {
	"so",
	"o",
	"",
	nullptr
} ;


/* exported variables */


/* exported subroutines */

int uc_opendialer(cc *prn,cc *svc,int of,m_t om,mv argv,mv envv,int to) noex {
	SI		si, *sip = &si ;
	int		rs ;
	int		rs1 ;
	int		fd = -1 ;

	if ((prn == nullptr) && (svc == nullptr)) return SR_FAULT ;

	if ((prn[0] == '\0') && (svc[0] == '\0')) return SR_INVALID ;

	if ((rs = subinfo_start(&si,prn,svc,of,om,argv,envv,to)) >= 0) {
	    if ((rs = subinfo_search(&si)) > 0) { /* >0 means found */
		fd = sip->fd ;
	    } else if (rs == 0) {
		rs = SR_NOENT ;
	    }
	    rs1 = subinfo_finish(&si) ;
	    if (rs >= 0) rs = rs1 ;
	    if ((rs < 0) && (fd >= 0)) u_close(fd) ;
	} /* end if (subinfo) */

	return (rs >= 0) ? fd : rs ;
}
/* end subroutine (uc_opendialer) */


/* local subroutines */

static int subinfo_start(SI *sip,cc *prn,cc *svc,int of,mode_t om,
		mainv argv,mainv envv,int to) noex {
	int		rs = SR_FAULT ;
	if (sip) {
	   if ((rs = memclear(sip)) >= 0) {
	       sip->prn = prn ;
	       sip->svc = svc ;
	       sip->argv = argv ;
	       sip->envv = (envv) ? envv : ((cchar **) environ) ;
	       sip->of = of ;
	       sip->om = om ;
	       sip->to = to ;
	       sip->fd = -1 ;
	       {
		   const int	maxname = MAXNAMELEN ;
	           cchar	*prefix = SVCSYMPREFIX ;
	           char	dialsym[maxname +1] ;
	           if ((rs = sncpy2(dialsym,maxname,prefix,sip->prn)) >= 0) {
		       auto	mall = lm_strw ;
		       cchar	*sp = dialsym ;
	               if (cchar *cp ; (rs = mall(sp,rs,&cp)) >= 0) {
	                   sip->dialsym = const_cast<char *>(cp) ;
		       }
	           }
	        } /* end block */
	    } /* end if (memclear) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (subinfo_start) */

static int subinfo_finish(SI *sip) noex {
	int		rs = SR_OK ;
	int		rs1 ;

	if (sip->dialsym != nullptr) {
	    rs1 = lm_free(sip->dialsym) ;
	    if (rs >= 0) rs = rs1 ;
	    sip->dialsym = nullptr ;
	}

	return rs ;
}
/* end subroutine (subinfo_finish) */

static int subinfo_search(SI *sip) noex {
	cint	plen = MAXPATHLEN ;
	int		rs ;
	int		rs1 ;
	int		f = false ;

	if ((rs = subinfo_idbegin(sip)) >= 0) {
	    cint	size = ((plen+1)*3) ;
	    char	*abuf ;
	    if ((rs = lm_mall(size,&abuf)) >= 0) {
	        char	dn[MAXHOSTNAMELEN+1] ;
	        char	*pdn = (abuf+(0*(plen+1))) ;
	        char	*sdn = (abuf+(1*(plen+1))) ;
	        char	*sfn = (abuf+(2*(plen+1))) ;
	        if ((rs = getnodedomain(nullptr,dn)) >= 0) {
		    USTAT	sb ;
	            for (int i = 0 ; prns[i] != nullptr ; i += 1) {
	                if ((rs = mkpr(pdn,plen,prns[i],dn)) > 0) {
		            if ((rs = mkpath2(sdn,pdn,SVCDNAME)) >= 0) {
	        		if ((rs = u_stat(sdn,&sb)) >= 0) {
				    if (S_ISDIR(sb.st_mode)) {
	                                rs = subinfo_exts(sip,pdn,sdn,sfn) ;
	                                f = rs ;
				    } /* end if */
				} else if (isNotPresent(rs)) {
		    		    rs = SR_OK ;
				}
			    } /* end if (mkpath) */
	                } /* end if (mkpr) */
			if (f) break ;
			if (rs < 0) break ;
	            } /* end for (prns) */

	        } /* end if */
	        rs1 = lm_free(abuf) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (ma-a) */
	    rs1 = subinfo_idend(sip) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (subinfo-id) */

	if ((rs < 0) && (sip->fd >= 0)) {
	    u_close(sip->fd) ;
	    sip->fd = -1 ;
	}

	return (rs >= 0) ? f : rs ;
}
/* end subroutine (subinfo_search) */

static int subinfo_exts(SI *sip,cchar *pr,cchar *sdn,char *sfn) noex {
	USTAT		sb ;
	cint		am = (R_OK|X_OK) ;
	int		rs = SR_OK ;
	int		f = false ;
	cchar		*prn = sip->prn ;
	for (int i = 0 ; soexts[i] != nullptr ; i += 1) {
	    if ((rs = mksofname(sfn,sdn,prn,soexts[i])) >= 0) {
	        if ((rs = u_stat(sfn,&sb)) >= 0) {
		    if (S_ISREG(sb.st_mode)) {
	                if ((rs = permid(&sip->id,&sb,am)) >= 0) {
			    rs = subinfo_searchlib(sip,pr,sfn) ;
			    f = rs ;
			} else if (rs == SR_ACCESS) {
			    rs = SR_OK ;
			}
		    } /* end if (regular file) */
		} else if (isNotPresent(rs)) {
		    rs = SR_OK ;
		}
	    } /* end if (mksofname) */
	    if (f) break ;
	    if (rs < 0) break ;
	} /* end for (soexts) */
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (subinfo_exts) */

static int subinfo_searchlib(SI *sip,cchar *pr,cchar *sfn) noex {
	cint		dlmode = RTLD_LAZY ;
	int		rs = SR_OK ;
	int		f = false ;
	void		*sop ;
	if ((sop = dlopen(sfn,dlmode)) != nullptr) {
	    subdialer_t	symp ;
	    if ((symp = (subdialer_t) dlsym(sop,sip->dialsym)) != nullptr) {
		rs = subinfo_searchcall(sip,pr,symp) ;
		f = rs ;
	    } /* end if (dlsym) */
	    dlclose(sop) ;
	} /* end if (dlopen) */
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (subinfo_searchlib) */

static int subinfo_searchcall(SI *sip,cchar *pr,subdialer_t symp) noex {
	mode_t		om = sip->om ;
	int		rs ;
	int		of = sip->of ;
	int		to = sip->to ;
	int		f = false ;
	cchar	*prn = sip->prn ;
	cchar	*svc = sip->svc ;
	mainv		argv = sip->argv ;
	mainv		envv = sip->envv ;

	if ((rs = (*symp)(pr,prn,svc,of,om,argv,envv,to)) >= 0) {
	    sip->fd = rs ;
	    f = true ;
	} /* end if (call) */

	return (rs >= 0) ? f : rs ;
}
/* end subroutine (subinfo_searchcall) */

static int subinfo_idbegin(SI *sip) noex {
	int		rs = ids_load(&sip->id) ;
	return rs ;
}
/* end subroutine (subinfo_idbegin) */

static int subinfo_idend(SI *sip) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (sip) {
	    rs = SR_OK ;
	    {
	        rs1 = ids_release(&sip->id) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	}
	return rs ;
}
/* end subroutine (subinfo_idend) */


