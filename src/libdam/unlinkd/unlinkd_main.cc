/* unlinkd_main SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* subroutine to try and invoke the UNLINK daemon */
/* version %I% last-modified %G% */

#define	CF_SETRUID	0		/* set real UID to EUID */

/* revision history:

	= 1998-05-14, David A-D- Morano
	This code was originally written.

*/

/* Copyright © 1998 David A-D- Morano.  All rights reserved. */

/******************************************************************************

	Name:
	unlinkd

	Description:
	This subroutine calls the UNLINKD program to delete (unlink)
	files.

	Synopsis:
	int unlinkd(cchar *filename,int delay) noex

	Arguments:
	filename	filename to unlink
	delay		time to wait before the unlink in seconds

	Returns:
	>=0		OK
	<0		error (system-return)

******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<sys/wait.h>
#include	<unistd.h>
#include	<cstddef>		/* |nullptr_t(3c++)| */
#include	<cstdlib>
#include	<cstring>
#include	<netdb.h>
#include	<usystem.h>
#include	<uvariables.hh>
#include	<getnodename.h>
#include	<vecstr.h>
#include	<spawnproc.h>
#include	<mkpath.h>
#include	<mkpr.h>
#include	<exitcodes.h>
#include	<localmisc.h>		/* |NOFILE| */

#include	"rmermsg.h"


/* local defines */

#ifndef	VARPRLOCAL
#define	VARPRLOCAL	"LOCAL"
#endif

#ifndef	VARPRPCS
#define	VARPRPCS	"PCS"
#endif

#define	DEFDELAY	30

#define	PROG_RMER	"rmer"
#define	PROG_UNLINKD	"unlinkd"

#define	DEFEXECPATH	"/usr/xpg4/bin:/usr/bin:/usr/extra/bin"

#define	SUBINFO		struct subinfo
#define	SUBINFO_ARGS	struct subinfo_args
#define	SUBINFO_FL	struct subinfo_flags


/* imported subroutines */


/* local typedefs */


/* external subroutines */

extern "C" {
    extern int	pcsgetprogpath(cchar *,char *,cchar *,int) noex ;
}


/* external variables */

extern cchar	**environ ;	/* from system at invocation */


/* local structures */

struct subinfo_args {
	cchar		*fname ;
	uint		delay ;
} ;

struct subinfo_flags {
	uint		here:1 ;
} ;

struct subinfo {
	SUBINFO_ARGS	arg ;
	SUBINFO_FL	f ;
	time_t		daytime ;
} ;

typedef int (*subinfo_f)(subinfo *) noex ;

cbool		f_setruid = CF_SETRUID;


/* forward references */

static int	subinfo_start(ßubinfo *,cchar *,int) noex ;
static int	subinfo_finish(ßubinfo *) noex ;
static int	subinfo_fork(ßubinfo *) noex ;
static int	subinfo_daemon(ßubinfo *) noex ;
static int	subinfo_rmer(ßubinfo *) noex ;


/* local variables */

constexpr subinfo_f	scheds[] = {
	subinfo_rmer,
	subinfo_fork,
	subinfo_daemon
} ;


/* exported variables */


/* exported subroutines */

int unlinkd(cchar *fname,int delay) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (fname) {
	    rs = SR_INVALID ;
	    if (fname[0]) {
	       if (USTAT sb ; (rs = u_stat(fname,&sb)) >= 0) {
	           if (subinfo si ; (rs = subinfo_start(si,fname,delay)) >= 0) {
		       for (cauto &fun : scheds) {
	    	           rs = (*fun)(&si) ;
	    	           if (rs >= 0) break ;
	               } /* end for */
	               rs1 = subinfo_finish(&si) ;
		       if (rs >= 0) rs = rs1 ;
	           } /* end if (subinfo) */
	       } else if (isNotPresent(rs)) {
	           rs = SR_OK ;
	       } /* end if (uc_stat) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (unlinkd) */


/* local subroutines */

static int subinfo_start(ßubinfo *sip,cchar *fname,int delay) noex {
	int		rs = SR_FAULT ;
	if (delay <= 0) delay = DEFDELAY ;
	if (sip && fname) {
	    rs = memclear(sip) ;
	    sip->daytime = getustime ;
	    sip->arg.fname = fname ;
	    sip->arg.delay = delay ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (subinfo_start) */

static int subinfo_finish(ßubinfo *sip) noex {
    	int		rs = SR_FAULT ;
	if (sip) {
	    sip->daytime = 0 ;
	    rs = SR_OK ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (subinfo_finish) */

static int subinfo_fork(ßubinfo *sip) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = u_fork()) == 0) {
	    USTAT	sb ;
	    time_t	ti_expire ;
	    pid_t	pid = rs ;
	    int		i ;

	if_constexpr (f_setruid) {
	    uid_t	uid = getuid() ;
	    uid_t	euid = geteuid() ;
	    if (euid != uid) {
	        u_setreuid(euid,-1) ;
	    }
	}

	/* the child continues on from here */

	    for (i = 0 ; i < NOFILE ; i += 1) {
	        u_close(i) ;
	    }

	    u_setsid() ;

	    ti_expire = (sip->daytime + sip->arg.delay) ;
	    rs1 = SR_OK ;
	    while (ti_expire > sip->daytime) {

	        uc_safesleep(1) ;

	        sip->daytime = time(nullptr) ;

	        rs1 = u_stat(sip->arg.fname,&sb) ;
	        if (rs1 < 0)
	            break ;

	    } /* end for */

	    if ((rs1 >= 0) && (sip->arg.fname != nullptr)) {
	        u_unlink(sip->arg.fname) ;
	    }

	    uc_exit(EX_OK) ;
	} /* end if (we got a child off) */

	return rs ;
}
/* end subroutine (subinfo_fork) */

static int subinfo_daemon(ßubinfo *sip) noex {
	int		rs = SR_NOSYS ;
	if (sip == nullptr) return SR_FAULT ;
	return rs ;
}
/* end subroutine (subinfo_daemon) */

static int subinfo_rmer(ßubinfo *sip) noex {
	spawnproc	pg{} ;
	rmermsg_fname	m0{} ;
	pid_t		pid ;

	int		rs = SR_OK ;
	int		rs1 ;
	int		fd ;
	int		sv ;
	int		m0_size = szof(struct rmermsg_fname) ;
	int		ipclen ;
	int		len ;
	int		cs = 0 ;
	int		opt = 0 ;
	int		i ;

	cchar	*pn = PROG_RMER ;
	cchar	*av[10 + 1] ;

	char		dname[MAXHOSTNAMELEN + 1] ;
	char		pr[MAXPATHLEN + 1] ;
	char		progfname[MAXPATHLEN + 1] ;
	char		*ipcbuf = nullptr ;


	rs1 = getnodedomain(nullptr,dname) ;
	if (rs1 < 0)
	    dname[0] = '\0' ;

	rs1 = mkpr(pr,MAXPATHLEN,VARPRLOCAL,dname) ;

	if (rs1 >= 0) {
	    rs1 = pcsgetprogpath(pr,progfname,pn,-1) ;

	    if (rs1 == 0)
	        rs = mkpath1(progfname,pn) ;
	}

	if ((rs >= 0) && (rs1 < 0)) {
	    cchar	*pvname = varname.path ;
	    rs = findfilepath(pvname,progfname,pn,X_OK) ;
	}

	if (rs < 0)
	    goto ret0 ;

/* allocate IPC buffer */

	ipclen = m0_size ;
	rs = uc_malloc(ipclen,&ipcbuf) ;
	if (rs < 0)
	    goto ret0 ;

/* load the message we are sending */

	memset(&m0,0,m0_size) ;
	m0.delay = sip->arg.delay ;

	rs = mkpath1(m0.fname,sip->arg.fname) ;
	if (rs < 0)
	    goto ret1 ;

/* prepare arguments for the spawned program */

	i = 0 ;
	av[i++] = "RMER" ;
	av[i++] = nullptr ;

	pg.disp[0] = SPAWNPROC_DOPEN ;
	pg.disp[1] = SPAWNPROC_DCLOSE ;
	pg.disp[2] = SPAWNPROC_DCLOSE ;
	pg.opts |= SPAWNPROC_OIGNINTR ;
	pg.opts |= SPAWNPROC_OSETPGRP ;
	rs = spawnproc(&pg,progfname,av,environ) ;
	pid = rs ;

	if (rs < 0)
	    goto ret1 ;

	fd = pg.fd[0] ;
	if (fd >= 0) {
	    if ((rs = rmermsg_fname(&m0,0,ipcbuf,ipclen)) >= 0) {
	        len = rs ;
	        rs = uc_writen(fd,ipcbuf,len) ;
	    }
	    u_close(fd) ;
	} else
	    rs = SR_NOSYS ;

/* wait for the spawned program to exit */

	opt = WNOHANG ;
	for (i = 0 ; i < 100 ; i += 1) {

	    rs1 = u_waitpid(pid,&cs,opt) ;

	    if (rs1 == 0) {
		sv = (i < 5) ? 10 : 100 ;
		msleep(sv) ;
	    }

	    if (rs1 > 0)
		break ;

	    if ((rs1 < 0) && (rs1 != SR_INTR)) {
		if (rs >= 0) rs = rs1 ;
		break ;
	    }

	} /* end for */

ret1:
	if (ipcbuf != nullptr) {
	    uc_free(ipcbuf) ;
	}

ret0:
	return rs ;
}
/* end subroutine (subinfo_rmer) */


