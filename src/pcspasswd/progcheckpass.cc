/* progcheckpass SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* check a password for validity */
/* version %I% last-modified %G% */

#define	CF_DEBUG	0		/* compile-time debugging */

/* revision history:

	= 1996-03-01, David A­D­ Morano
        The program was written from scratch to do what the previous program by
        the same name did.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	checkpass

	Description:
	This subroutine will take as input a username and a password
	and it will return a boolean about whether or not the
	password is valid on the current system.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* POSIX® */
#include	<sys/param.h>		/* POSIX® */
#include	<sys/stat.h>		/* POSIX® */
#include	<sys/wait.h>		/* POSIX® */
#include	<unistd.h>		/* POSIX® */
#include	<shadow.h>		/* POSIX® */
#include	<pwd.h>			/* POSIX® */
#include	<grp.h>			/* POSIX® */
#include	<csignal>		/* CSTD */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstring>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<getax.h>		/* LIBUC */
#include	<bufsizeget.h>		/* LIBUC */
#include	<pwfile.h>		/* LIBUC */
#include	<vecstr.h>		/* LIBUC */
#include	<des.h>			/* LIBUC */
#include	<spawnproc.h>		/* LIBUC */
#include	<filer.h>		/* LIBUC */
#include	<sncpy.h>		/* LIBUC */
#include	<sfx.h>			/* LIBUC */
#include	<onc.h>			/* LIBUC */
#include	<localmisc.h>		/* LIBU */
#include	<libdebug.h>		/* LIBDEBUG |DEBUGPRINTF(3debug)| */
#include	<bfile.h>		/* LIBB */
#include	<passwdok.h>		/* LIBDAM */

#include	"proginfo.hh"
#include	"progchechpass.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| */

/* local defines */

#ifndef	BUFLEN
#define	BUFLEN		MAXPATHLEN
#endif

#ifndef	RDBUFLEN
#define	RDBUFLEN	80
#endif

#ifndef	PROG_NISMATCH
#define	PROG_NISMATCH	"/usr/bin/nismatch"
#endif

#define	TO_READ		5

#ifndef	PI
#define	PI		proginfo
#endif

#define	PD		passdata

#ifndef	CF_DEBUG
#define	CF_DEBUG	0		/* compile-time debugging */
#endif


/* external subroutines */

extern "C" {
    extern int	passwdok(cchar *,cchar *) noex ;
} /* end extern (C) */


/* external variables */


/* local structures */

struct passdata {
	cchar		*username ;
	char		*passbuf ;
} ; /* end struct */


/* forward references */

local int	try_start	(PD *,char *,cchar *) noex ;
local int	try_pwd		(PD *) noex ;
local int	try_spw		(PD *) noex ;
local int	try_nisplus	(PD *) noex ;
local int	try_finish	(PD *) noex ;

local int	readpass(int,char *,int) noex ;


/* local variables */

constexpr cpcchar	badpasswds[] = {
	"x",
	"*NP*",
	"NP",
	nullptr
} ; /* end array */

constexpr int	(*tries[])(PD *) = {
	try_pwd,
	try_spw,
	try_nisplus,
	nullptr
} ; /* end array */

cbool		f_debug		= CF_DEBUG ;


/* exported variables */


/* exported subroutines */

int progcheckpass(PI *pip,cc *username,cc *password,int f_useshadow) noex {
	PD	pd, *pdp = &pd ;
	int		rs ;
	int		rs1 = 0 ;
	int		i ;
	int		passlen = 0 ;
	int		f_valid = false ;
	char		passbuf[PASSWDLEN + 1] ;

	if (username == nullptr) return SR_FAULT ;
	if (password == nullptr) return SR_FAULT ;

	if (username[0] == '\0') return SR_INVALID ;

	if (pip->debuglevel > 0) {
	    bprintf(pip->efp,"%s: f_useshadow=%u\n",
	        pip->progname,f_useshadow) ;
	}

	if ((rs = try_start(pdp,passbuf,username)) >= 0) {

	for (i = 0 ; tries[i] != nullptr ; i += 1) {

	    rs1 = (*tries[i])(pdp) ;
	    passlen = rs1 ;

	    if ((rs1 >= 0) || (rs1 == SR_NOMEM))
	        break ;

	} /* end for */

	if ((rs >= 0) && (rs1 == SR_NOMEM))
	    rs = SR_NOMEM ;

	try_finish(pdp) ;
	} /* end if */

	if ((pip->debuglevel > 0) && (rs >= 0))
	    bprintf(pip->efp,"%s: system password=>%s<\n",
	        pip->progname,passbuf) ;

/* check if the password is OK */

	if ((rs >= 0) && (passlen > 0)) {
	    rs1 = passwdok(password,passbuf) ;
	    f_valid = (rs1 > 0) ;
	}

ret0:

#if	CF_DEBUG
	if (DEBUGLEVEL(3))
	    DEBUGPRINTF("ret rs=%d f_valid=%u\n",
	        rs,f_valid) ;
#endif

	return (rs >= 0) ? f_valid : rs ;
} /* end subroutine (progcheckpass) */


/* local subroutines */

local int try_start(PD *pdp,char *passbuf,cchar *username) noex {
	if (pdp == nullptr) return SR_FAULT ;
	if (username == nullptr) return SR_FAULT ;
	passbuf[0] = '\0' ;
	memclear(pdp) ;
	pdp->username = username ;
	pdp->passbuf = passbuf ;
	return SR_OK ;
} /* end subroutine (try_start) */

local int try_finish(PD *pdp) noex {
	if (pdp == nullptr) return SR_FAULT ;
	memclear(pdp) ;
	return SR_OK ;
} /* end subroutine (try_finish) */

local int try_pwd(PD *pdp) noex {
	PASSWD		pw ;
	cint	pwlen = bufsizeget(bufsize_pw) ;
	int		rs ;
	char		*pwbuf ;

	pdp->passbuf[0] = '\0' ;
	if ((rs = uc_malloc((pwlen+1),&pwbuf)) >= 0) {
	    cchar	*un = pdp->username ;
	    if ((rs = getpw_name(&pw,pwbuf,pwlen,un)) >= 0) {
	        rs = SR_NOEXIST ;
	        if (matstr(badpasswds,pw.pw_passwd,-1) < 0) {
	            rs = sncpy1(pdp->passbuf,PASSWDLEN,pw.pw_passwd) ;
		}
	    }
	    uc_free(pwbuf) ;
	} /* end if (memory-allocation) */

	return rs ;
} /* end subroutine (try_pwd) */

local int try_spw(PD *pdp) noex {
	SPWD		sp ;
	cint		splen = bufsizeget(bufsize_sp) ;
	int		rs ;
	char		*spbuf ;

	pdp->passbuf[0] = '\0' ;
	if ((rs = uc_malloc((splen+1),&spbuf)) >= 0) {
	    cchar	*un = pdp->username ;
	    if ((rs = getsp_name(&sp,spbuf,splen,un)) >= 0) {
	        rs = SR_NOEXIST ;
	        if (matstr(badpasswds,sp.sp_pwdp,-1) < 0) {
	            rs = sncpy1(pdp->passbuf,PASSWDLEN,sp.sp_pwdp) ;
		}
	    }
	    uc_free(spbuf) ;
	} /* end if (memory-allocation) */

	return rs ;
} /* end subroutine (try_spw) */

local int try_nisplus(PD *pdp) noex {
	spawnproc	ps{} ;
	vecstr		args ;
	pid_t		cpid ;
	int		rs ;
	int		len ;
	int		cstat ;
	cchar	*progfname = PROG_NISMATCH ;
	cchar	**av ;

#if	CF_DEBUG
	DEBUGPRINTF("ent\n") ;
#endif

	rs = havenis() ;
	if (rs < 0)
	    goto ret0 ;

	pdp->passbuf[0] = '\0' ;
	rs = vecstr_start(&args,5,0) ;
	if (rs < 0)
	    goto ret0 ;

	rs = vecstr_add(&args,"nismatch",-1) ;
	if (rs >= 0) {
	    rs = vecstr_add(&args,pdp->username,-1) ;
	}
	if (rs >= 0) {
	    rs = vecstr_add(&args,"passwd.org_dir",-1) ;
	}
	if (rs < 0) goto ret1 ;

/* try to execute NISMATCH */

	vecstr_getvec(&args,&av) ;

	memclear(&ps) ;
	ps.disp[0] = SPAWNPROC_DCLOSE ;
	ps.disp[1] = SPAWNPROC_DCREATE ;
	ps.disp[2] = SPAWNPROC_DCLOSE ;
	if ((rs = spawnproc(&ps,progfname,av,nullptr)) >= 0) {
	    cint	fd = ps.fd[1] ;
	    int		cex ;
	    cpid = rs ;

	    rs = readpass(fd,pdp->passbuf,PASSWDLEN) ;
	    len = rs ;

	    u_close(fd) ;

	    u_waitpid(cpid,&cstat,WUNTRACED) ;

	    if (rs >= 0) {
	        if (WIFEXITED(cstat)) {
		    cex = WEXITSTATUS(cstat) ;
		    if (cex > 0) {
		        rs = SR_NOEXIST ;
		    }
		} else {
		    rs = SR_NOEXIST ;
		}
	    } /* end if (ok) */
	} /* end if */

	if ((rs >= 0) && (len > 0) &&
	    (matstr(badpasswds,pdp->passbuf,len) >= 0)) {
	    rs = SR_NOEXIST ;
	}

ret1:
	vecstr_finish(&args) ;

ret0:

#if	CF_DEBUG
	DEBUGPRINTF("ret rs=%d\n",rs) ;
#endif

	return rs ;
} /* end subroutine (try_nisplus) */

local int readpass(int fd,char *outbuf,int outlen) noex {
	int		rs ;
	int		len ;
	int		cl = -1 ;
	cchar	*tp ;
	cchar	*cp = nullptr ;
	char		rdbuf[RDBUFLEN + 1] ;

	if (outbuf == nullptr) return SR_FAULT ;

	if (fd < 0) {
	    rs = SR_BADF ;
	    goto ret0 ;
	}

	filer	b ;
	rs = filer_start(&b,fd,0,-1,0) ;
	if (rs < 0)
	    goto ret0 ;

	rs = filer_readln(&b,rdbuf,RDBUFLEN,TO_READ) ;
	len = rs ;
	if (rs >= 0) {
	        if (rdbuf[len - 1] == '\n') {
	            len -= 1 ;
		}
	        rdbuf[len] = '\0' ;
	        cp = nullptr ;
	        cl = -1 ;
	        if ((tp = strchr(rdbuf,':')) != nullptr) {
		    cp = (tp + 1) ;
		    cl = -1 ;
	            if ((tp = strchr(cp,':')) != nullptr) {
			cl = (tp - cp) ;
		    }
		}

	} /* end if (reading line) */

	filer_finish(&b) ;

	if (rs >= 0) {
	    rs = SR_NOEXIST ;
	    if ((cp != nullptr) && (cl >= 0)) {
		rs = snwcpy(outbuf,outlen,cp,cl) ;
	    }
	}

ret0:
	return rs ;
} /* end subroutine (readpass) */



