/* unlinkd_main SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* subroutine to try and invoke the UNLINK daemon */
/* version %I% last-modified %G% */

#define	CF_DEBUGS	0		/* non-switchable debug print-outs */
#define	CF_SETRUID	0		/* set real UID to EUID */

/* revision history:

	= 1998-05-14, Dave Morano
	This code was originally written.

*/

/* Copyright © 1998 David A-D- Morano.  All rights reserved. */

/******************************************************************************

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
	<0		some error

******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<sys/wait.h>
#include	<unistd.h>
#include	<netdb.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<clanguage.h>
#include	<usysbase.h>
#include	<usyscalls.h>
#include	<vecstr.h>
#include	<mkx.h>
#include	<mkpathx.h>
#include	<spawnproc.h>
#include	<exitcodes.h>
#include	<localmisc.h>
#include	<libpcs.h>		/* LIBPCS */

#include	"rmermsg.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| */

/* local defines */

#ifndef	NOFILE
#define	NOFILE		20
#endif

#ifndef	VARPRLOCAL
#define	VARPRLOCAL	"LOCAL"
#endif

#ifndef	VARPRPCS
#define	VARPRPCS	"PCS"
#endif

#ifndef	VARPATH
#define	VARPATH		"PATH"
#endif

#define	DEFDELAY	30

#if	CF_DEBUGS
#define	PROG_RMER	"/home/dam/src/rmer/rmer.x"
#else
#define	PROG_RMER	"rmer"
#endif

#define	PROG_UNLINKD	"unlinkd"

#define	DEFEXECPATH	"/usr/xpg4/bin:/usr/bin:/usr/extra/bin"

#define	SI		subinfo
#define	SI_ARGS		subinfo_args
#define	SI_FL		subinfo_flags


/* external subroutines */

extern int	findfilepath(cchar *,cchar *,int,char *) noex ;


/* external variables */


/* local structures */

struct subinfo_args {
	cchar		*fname ;
	uint		delay ;
} ; /* end struct */

struct subinfo_flags {
	uint		here:1 ;
} ; /* end struct */

struct subinfo {
	SI_ARGS		arg ;
	SI_FL		fl ;
	time_t		daytime ;
} ; /* end struct */


/* forward references */

local int	subinfo_start(SI *,cchar *,int) noex ;
local int	subinfo_finish(SI *) noex ;
local int	subinfo_fork(SI *) noex ;
local int	subinfo_daemon(SI *) noex ;
local int	subinfo_rmer(SI *) noex ;


/* local variables */

constexpr int	(*scheds[])(SI *) = {
	subinfo_rmer,
	subinfo_fork,
	subinfo_daemon,
	nullptr
} ; /* end array */


/* exported variables */


/* exported subroutines */

int unlinkd(cchar *fname,int delay) noex {
	ustat	sb ;
	int		rs ;
	int		rs1 ;

	if (fname == nullptr) return SR_FAULT ;

	if (fname[0] == '\0') return SR_INVALID ;

#if	CF_DEBUGS
	debugprintf("unlinkd: f=%s d=%d\n",fname,delay) ;
#endif

	if ((rs = u_stat(fname,&sb)) >= 0) {
	    SI	si, *sip = &si ;
	    if ((rs = subinfo_start(sip,fname,delay)) >= 0) {
		int	i ;
		for (i = 0 ; scheds[i] != nullptr ; i += 1) {
	    	    rs = (*scheds[i])(sip) ;
	    	    if (rs >= 0) break ;
	        } /* end for */
	        subinfo_finish(sip) ;
	    } /* end if (subinfo) */
	} else if (isNotPresent(rs)) {
	    rs = SR_OK ;
	} /* end if (uc_stat) */

#if	CF_DEBUGS
	debugprintf("unlinkd: ret rs=%d\n",rs) ;
#endif

	return rs ;
}
/* end subroutine (unlinkd) */


/* local subroutines */

local int subinfo_start(SI *sip,cchar *fname,int delay) noex {
	int		rs = SR_OK ;

	if (delay <= 0)
	    delay = DEFDELAY ;

	memclear(sip) ;
	sip->daytime = time(nullptr) ;

	sip->arg.fname = fname ;
	sip->arg.delay = delay ;
	return rs ;
}
/* end subroutine (subinfo_start) */

local int subinfo_finish(SI *sip) noex {
	sip->daytime = 0 ;
	return SR_OK ;
}
/* end subroutine (subinfo_finish) */

local int subinfo_fork(SI *sip) noex {
	int		rs ;
	int		rs1 ;

	if ((rs = u_fork()) == 0) {
	    ustat	sb ;
	    time_t	ti_expire ;
	    pid_t	pid = rs ;

#if	CF_SETRUID
	{
	    uid_t	uid = getuid() ;
	    uid_t	euid = geteuid() ;
	    if (euid != uid) {
	        u_setreuid(euid,-1) ;
	    }
	}
#endif /* CF_SETRUID */

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
}
/* end subroutine (subinfo_fork) */

local int subinfo_daemon(SI *sip) noex {
	int		rs = SR_NOSYS ;
	if (sip == nullptr) return SR_FAULT ;
	return rs ;
}
/* end subroutine (subinfo_daemon) */

local int subinfo_rmer(SI *sip) noex {
	struct spawnproc	pg{} ;
	struct rmermsg_fname	m0{} ;
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

#if	CF_DEBUGS
	int		fd_err ;
#endif

	cchar	*pn = PROG_RMER ;
	cchar	*av[10 + 1] ;

	char		dname[MAXHOSTNAMELEN + 1] ;
	char		pr[MAXPATHLEN + 1] ;
	char		progfname[MAXPATHLEN + 1] ;
	char		*ipcbuf = nullptr ;


	rs1 = getnodedomain(nullptr,dname) ;
	if (rs1 < 0)
	    dname[0] = '\0' ;

#if	CF_DEBUGS
	debugprintf("unlinkd/subinfo_rmer: getnodedomain() rs=%d\n",rs1) ;
#endif

	rs1 = mkpr(pr,MAXPATHLEN,VARPRLOCAL,dname) ;

#if	CF_DEBUGS
	debugprintf("unlinkd/subinfo_rmer: mkpr() rs=%d\n",rs1) ;
#endif

	if (rs1 >= 0) {
	    rs1 = pcsgetprogpath(pr,progfname,pn,-1) ;

#if	CF_DEBUGS
	debugprintf("unlinkd/subinfo_rmer: pcsgetprogpath() rs=%d\n",rs1) ;
#endif

	    if (rs1 == 0)
	        rs = mkpath1(progfname,pn) ;
	}

	if ((rs >= 0) && (rs1 < 0)) {
	    rs = findfilepath(VARPATH,pn,X_OK,progfname) ;
#if	CF_DEBUGS
	debugprintf("unlinkd/subinfo_rmer: findfilepath() rs=%d\n",rs) ;
#endif
	}

#if	CF_DEBUGS
	debugprintf("unlinkd/subinfo_rmer: progfname=%s\n",progfname) ;
#endif

	if (rs < 0)
	    goto ret0 ;

/* allocate IPC buffer */

	ipclen = m0_size ;
	rs = uc_malloc(ipclen,&ipcbuf) ;
	if (rs < 0)
	    goto ret0 ;

/* load the message we are sending */

	m0.delay = sip->arg.delay ;
	rs = mkpath1(m0.fname,sip->arg.fname) ;
	if (rs < 0)
	    goto ret1 ;

/* prepare arguments for the spawned program */

	i = 0 ;

#if	CF_DEBUGS
	av[i++] = "RMERd" ;
#else
	av[i++] = "RMER" ;
#endif

#if	CF_DEBUGS
	fd_err = uc_open("rmer.e",(O_WRONLY | O_CREAT | O_TRUNC),0666) ;
	av[i++] = "-D=5" ;
#endif

	av[i++] = nullptr ;
	pg.disp[0] = SPAWNPROC_DOPEN ;
	pg.disp[1] = SPAWNPROC_DCLOSE ;

#if	CF_DEBUGS
	pg.disp[2] = SPAWNPROC_DDUP ;
	pg.fd[2] = fd_err ;
#else
	pg.disp[2] = SPAWNPROC_DCLOSE ;
#endif

	if (mainv ev ; (rs = u_getenviron(&ev)) >= 0) {
#if	CF_DEBUGS
	{
	vecstr envs ;
	vecstr_start(&envs,10,VECSTR_OCOMPACT) ;
	    for (int i = 0 ; ev[i] ; i += 1) {
		    vecstr_add(&envs,ev[i],-1) ;
	    }
	vecstr_add(&envs,"RMER_DEBUGFILE=rmer.d",-1) ;
	vecstr_getvec(&envs,&ev) ;
	rs = spawnproc(&pg,progfname,av,ev) ;
	pid = rs ;
	vecstr_finish(&envs) ;
	}
#else
	rs = spawnproc(&pg,progfname,av,ev) ;
	pid = rs ;
#endif /* CF_DEBUGS */
	} /* end if (u_getenviron) */

#if	CF_DEBUGS
	debugprintf("unlinkd/subinfo_rmer: spawnproc() rs=%d\n",rs) ;
	if (fd_err >= 0)
	u_close(fd_err) ;
#endif

	if (rs < 0)
	    goto ret1 ;

	fd = pg.fd[0] ;
	if (fd >= 0) {
	    if ((rs = rmermsg_fname(&m0,0,ipcbuf,ipclen)) >= 0) {
	        len = rs ;
	        rs = uc_writen(fd,ipcbuf,len) ;
	    }
	    u_close(fd) ;
	} else {
	    rs = SR_NOSYS ;
	}

/* wait for the spawned program to exit */

	opt = WNOHANG ;
	for (int i = 0 ; i < 100 ; i += 1) {
	    rs1 = u_waitpid(pid,&cs,opt) ;
	    if (rs1 == 0) {
		sv = (i < 5) ? 10 : 100 ;
		msleep(sv) ;
	    }
	    if (rs1 > 0) break ;
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


