/* unlinkd SUPPORT */
/* lang=C++20 */

/* subroutine to try and invoke the UNLINK daemon */
/* version %I% last-modified %G% */

#define	CF_SETRUID	0		/* set real UID to EUID */

/* revision history:

	= 1998-05-14, David A­D­ Morano
	This subroutine was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

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
	<0		some error (system-return)

******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<sys/wait.h>
#include	<stdlib.h>
#include	<unistd.h>
#include	<ctime>
#include	<cstring>
#include	<netdb.h>
#include	<usystem.h>
#include	<vecstr.h>
#include	<spawnproc.h>
#include	<exitcodes.h>
#include	<localmisc.h>

#include	"rmermsg.h"


/* local defines */

#ifndef	VARPRLOCAL
#define	VARPRLOCAL	"LOCAL"
#endif

#ifndef	VARPRPCS
#define	VARPRPCS	"PCS"
#endif

#ifndef	VARPATH
#define	VARPATH		"PATH"
#endif

#define	SUBINFO		struct subinfo

#define	DEFDELAY	30

#define	PROG_RMER	"rmer"

#define	PROG_UNLINKD	"unlinkd"

#define	DEFEXECPATH	"/usr/xpg4/bin:/usr/bin:/usr/extra/bin"


/* external subroutines */

extern int	mkpath1(char *,cchar *) ;
extern int	mkpath2(char *,cchar *,cchar *) ;
extern int	getnodedomain(char *,char *) ;
extern int	mkpr(char *,int,cchar *,cchar *) ;
extern int	prgetprogpath(cchar *,char *,cchar *,int) ;
extern int	msleep(int) ;

extern char	*strwcpy(char *,cchar *,int) ;


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
	struct subinfo_args	arg ;
	struct subinfo_flags	f ;
	time_t			daytime ;
} ;


/* forward references */

static int	subinfo_start(SUBINFO *,cchar *,int) ;
static int	subinfo_finish(SUBINFO *) ;
static int	subinfo_fork(SUBINFO *) ;
static int	subinfo_daemon(SUBINFO *) ;
static int	subinfo_rmer(SUBINFO *) ;


/* local variables */

static constexpr int	(*scheds[])(SUBINFO *) = {
	subinfo_rmer,
	subinfo_fork,
	subinfo_daemon,
	NULL
} ;


/* exported variables */


/* exported subroutines */

int unlinkd(cchar *fname,int delay) noex {
	SUBINFO		si, *sip = &si ;
	USTAT		sb ;
	int		rs = SR_OK ;
	int		rs1 ;
	int		i = 0 ;

	if (fname == NULL)
	    return SR_FAULT ;

	if (fname[0] == '\0')
	    return SR_INVALID ;

	if (u_stat(fname,&sb) >= 0) {
	    if ((rs = subinfo_start(sip,fname,delay)) >= 0) {

	        for (i = 0 ; scheds[i] != NULL ; i += 1) {
	            rs = (*scheds[i])(sip) ;
	            if (rs >= 0) break ;
	        } /* end for */

	        subinfo_finish(sip) ;
	    } /* end if (subinfo) */
	} /* end if (stat; file exists) */

	return rs ;
}
/* end subroutine (unlinkd) */


/* local subroutines */

static int subinfo_start(SUBINFO *sip,cc *fname,int delay) noex {
	int	rs = SR_OK ;
	if (delay <= 0) delay = DEFDELAY ;
	memclear(sip) ;
	sip->daytime = time(NULL) ;
	sip->arg.fname = fname ;
	sip->arg.delay = delay ;
	return rs ;
}
/* end subroutine (subinfo_start) */

static int subinfo_finish(SUBINFO *sip) noex {
	sip->daytime = 0 ;
	return SR_OK ;
}
/* end subroutine (subinfo_finish) */

static int subinfo_fork(SUBINFO *sip) noex {
	USTAT		sb ;
	pid_t	pid ;
	time_t	ti_expire ;
	int	rs = SR_OK ;
	int	rs1 ;
	int	i ;

	rs = uc_fork() ;
	pid = rs ;
	if (rs < 0)
	    goto ret0 ;

	if (pid == 0) {

#if	CF_SETRUID
	{
	    uid_t	uid, euid ;
	    uid = getuid() ;
	    euid = geteuid() ;
	    if (euid != uid) {
	        u_setreuid(euid,-1) ;
	    }
	}
#endif /* CF_SETRUID */

/* the child continues on from here */

	    for (i = 0 ; i < NOFILE ; i += 1)
	        u_close(i) ;

	    u_setsid() ;

	    ti_expire = (sip->daytime + sip->arg.delay) ;
	    rs1 = SR_OK ;
	    while (ti_expire > sip->daytime) {

	        uc_safesleep(1) ;

	        sip->daytime = time(NULL) ;

	        rs1 = u_stat(sip->arg.fname,&sb) ;
	        if (rs1 < 0)
	            break ;

	    } /* end for */

	    if ((rs1 >= 0) && (sip->arg.fname != NULL))
	        u_unlink(sip->arg.fname) ;

	    uc_exit(EX_OK) ;

	} /* end if (we got a child off) */

ret0:
	return rs ;
}
/* end subroutine (subinfo_fork) */

static int subinfo_daemon(SUBINFO *sip) noex {
	int	rs = SR_NOSYS ;
	return rs ;
}
/* end subroutine (subinfo_daemon) */

static int subinfo_rmer(SUBINFO *sip) noex {
	struct spawnproc	pg ;
	struct rmermsg_fname	m0 ;
	pid_t	pid ;
	int	rs = SR_OK ;
	int	rs1 ;
	int	fd ;
	int	sv ;
	int	m0_size = sizeof(struct rmermsg_fname) ;
	int	ipcbuflen ;
	int	len ;
	int	cs = 0 ;
	int	opt = 0 ;
	int	i ;

	cchar	*pn = PROG_RMER ;
	cchar	*av[10 + 1] ;

	char	dname[MAXHOSTNAMELEN + 1] ;
	char	pr[MAXPATHLEN + 1] ;
	char	progfname[MAXPATHLEN + 1] ;
	char	*ipcbuf = NULL ;


	rs1 = getnodedomain(NULL,dname) ;
	if (rs1 < 0)
	    dname[0] = '\0' ;

	rs1 = mkpr(pr,MAXPATHLEN,VARPRLOCAL,dname) ;

	if (rs1 >= 0) {
	    rs1 = prgetprogpath(pr,progfname,pn,-1) ;

	    if (rs1 == 0)
	        rs = mkpath1(progfname,pn) ;
	}

	if ((rs >= 0) && (rs1 < 0)) {

	    rs = findfilepath(VARPATH,progfname,pn,X_OK) ;

	}

	if (rs < 0)
	    goto ret0 ;

/* allocate IPC buffer */

	ipcbuflen = m0_size ;
	rs = uc_malloc(ipcbuflen,&ipcbuf) ;
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
	av[i++] = NULL ;

	memset(&pg,0,sizeof(struct spawnproc)) ;
	pg.opts |= SPAWNPROC_OIGNINTR ;
	pg.opts |= SPAWNPROC_OSETPGRP ;
	pg.disp[0] = SPAWNPROC_DOPEN ;
	pg.disp[1] = SPAWNPROC_DCLOSE ;
	pg.disp[2] = SPAWNPROC_DCLOSE ;

	rs = spawnproc(&pg,progfname,av,environ) ;
	pid = rs ;

	if (rs < 0)
	    goto ret1 ;

	fd = pg.fd[0] ;
	if (fd >= 0) {

	    if ((rs = rmermsg_fname(&m0,0,ipcbuf,ipcbuflen)) >= 0) {
	        len = rs ;
	        rs = uc_writen(fd,ipcbuf,len) ;
	    }
	    u_close(fd) ;

	} else {
	    rs = SR_NOSYS ;
	}

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
		if (rs >= 0)
		    rs = rs1 ;
		break ;
	    }

	} /* end for */

ret1:
	if (ipcbuf != NULL)
	    uc_free(ipcbuf) ;

ret0:
	return rs ;
}
/* end subroutine (subinfo_rmer) */


