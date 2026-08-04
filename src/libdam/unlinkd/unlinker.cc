/* unlinker SUPPORT */
/* charset=ISO8859-1 */
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
#include	<sys/types.h>		/* POSIX® */
#include	<sys/param.h>		/* POSIX® */
#include	<sys/stat.h>		/* POSIX® */
#include	<sys/wait.h>		/* POSIX® */
#include	<unistd.h>		/* POSIX® */
#include	<netdb.h>		/* POSIX® */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstring>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<usysdata.h>		/* LIBU */
#include	<ucmem.h>		/* LIBUC */
#include	<getnodename.h>		/* LIBUC */
#include	<subinfo.h>		/* LIBUC */
#include	<spawnproc.h>		/* LIBUC */
#include	<mkpath.h>		/* LIBUC */
#include	<mkpr.h>		/* LIBUC */
#include	<exitcodes.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU |NOFILE| */

#include	"unlinkd.h"
#include	"rmermsg.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |memclear(3u)| */

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

#define	SI		subinfo
#define	SI_FL		subinfo_fl
#define	SI_ARGS		subinfo_args


/* imported namespaces */

using libuc::mem ;		/* variable */


/* local typedefs */


/* external subroutines */

extern "C" {
    extern int	pcsgetprogpath(cchar *,char *,cchar *,int) noex ;
}


/* external variables */


/* local structures */

enum subinfomems {
    subinfomem_start,
    subinfomem_finish,
    subinfomem_overlast
} ; /* end enum (subinfomems) */

namespace {
    struct vars {
	uint		sysid ;
	operator int () noex ;
    } ; /* end struct (vars) */
} /* end namespace */

namespace {
    struct subinfo ;
    struct subinfo_co {
	subinfo		*op = nullptr ;
	int		w = -1 ;
	void operator () (subinfo *p,int m) noex {
	    op = p ;
	    w = m ;
	} ;
	operator int () noex ;
	int operator () () noex { 
	    return operator int () ;
	} ;
    } ; /* end struct (subinfo_co) */
    struct subinfo_args {
	cchar		*fname ;
	uint		delay ;
    } ; /* end struct */
    struct subinfo_fl {
	uint		here:1 ;
    } ; /* end struct */
    struct subinfo {
	friend		subinfo_co ;
	subinfo_co	start ;
	subinfo_co	finish ;
	SI_ARGS		arg ;
	mainv		envv ;
	time_t		daytime ;
	SI_FL		fl ;
	subinfo() noex {
	    start	(this,subinfomem_start) ;
	    finish	(this,subinfomem_start) ;
	} ; /* end ctor */
    private:
	int ifini	() noex ;
	int istart	() noex ;
    } ; /* end struct */
} /* end namespace */

typedef int (*subinfo_f)(subinfo *) noex ;


/* forward references */

local int	subinfo_start	(SI *,cchar *,int) noex ;
local int	subinfo_finish	(SI *) noex ;
local int	subinfo_fork	(SI *) noex ;
local int	subinfo_daemon	(SI *) noex ;
local int	subinfo_rmer	(SI *) noex ;


/* local variables */

constexpr subinfo_f	scheds[] = {
	subinfo_rmer,
	subinfo_fork,
	subinfo_daemon
} ; /* end array */

static vars	var ;
cbool		f_setruid = CF_SETRUID ;


/* exported variables */


/* exported subroutines */

int unlinkd(cchar *fname,int delay) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (fname) ylikely {
	    rs = SR_INVALID ;
	    if (fname[0]) ylikely {
	       if (ustat sb ; (rs = u_stat(fname,&sb)) >= 0) ylikely {
	           if (subinfo si ; (rs = subinfo_start(&si,fname,delay)) >= 0) {
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
} /* end subroutine (unlinkd) */


/* local subroutines */

local int subinfo_start(SI *sip,cchar *fname,int delay) noex {
	int		rs = SR_FAULT ;
	if (delay <= 0) delay = DEFDELAY ;
	if (sip && fname) ylikely {
	    memclear(sip) ;
	    if (mainv ev ; (rs = u_getenvon(&ev)) >= 0) ylikely {
		sip->envv = ev ;
	        sip->daytime = getustime ;
	        sip->arg.fname = fname ;
	        sip->arg.delay = delay ;
	    } /* end if (u_getenviron) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (subinfo_start) */

local int subinfo_finish(SI *sip) noex {
    	int		rs = SR_FAULT ;
	if (sip) ylikely {
	    sip->daytime = 0 ;
	    rs = SR_OK ;
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (subinfo_finish) */

local int subinfo_fork(SI *sip) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = u_fork()) == 0) ylikely {
	    ustat	sb ;
	    time_t	ti_expire ;
	    pid_t	pid = rs ;

	if_constexpr (f_setruid) {
	    uid_t	uid = getuid() ;
	    uid_t	euid = geteuid() ;
	    if (euid != uid) {
	        u_setreuid(euid,-1) ;
	    }
	}

	/* the child continues on from here */

	    for (int i = 0 ; i < NOFILE ; i += 1) {
	        u_close(i) ;
	    }

	    u_setsid() ;

	    ti_expire = (sip->daytime + sip->arg.delay) ;
	    rs1 = SR_OK ;
	    while (ti_expire > sip->daytime) {
	        uc_safesleep(1) ;
	        sip->daytime = time(nullptr) ;
	        rs1 = u_stat(sip->arg.fname,&sb) ;
	        if (rs1 < 0) break ;
	    } /* end for */
	    if ((rs1 >= 0) && (sip->arg.fname != nullptr)) {
	        u_unlink(sip->arg.fname) ;
	    }
	    uc_exit(EX_OK) ;
	} /* end if (we got a child off) */

	return rs ;
} /* end subroutine (subinfo_fork) */

local int subinfo_daemon(SI *sip) noex {
	int		rs = SR_NOSYS ;
	if (sip == nullptr) return SR_FAULT ;
	return rs ;
} /* end subroutine (subinfo_daemon) */

local int subinfo_rmer(SI *sip) noex {
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
	rs = spawnproc(&pg,progfname,av,op->envv) ;
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
} /* end subroutine (subinfo_rmer) */

vars::operator int () noex {
    	int		rs ;
	if (ulong hid ; (rs = u_gethostid(&hid)) >= 0) {
	    sysid = conv<uint>(hid) ;
	} /* end if (u_gethostid) */
	return rs ;
} /* end method (vars::operator) */


