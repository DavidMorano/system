/* maintqotd_prog SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* PROGRAM dialer for MAINTQOTD */
/* version %I% last-modified %G% */

#define	CF_DEBUG	0		/* non-switchable debug print-outs */
#define	CF_BACKGROUND	1		/* put program in background */

/* revision history:

	= 1998-06-01, David A­D­ Morano
	This code was originally written.


*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:

	Description:
	This subroutine is called for the "prog" dialer for MAINTQOTD.

	Synopsis:
	int maintqotd_prog(MAINTQOTD *sip,cchar *qfname,cchar *sep) noex

	Arguments:
	sip		pointer to local state
	qfname		QOTD-file name
	sep		source entry pointer

	Returns:
	>=0		resulting FD
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* POSIX® */
#include	<sys/param.h>		/* POSIX® */
#include	<sys/stat.h>		/* POSIX® */
#include	<unistd.h>		/* POSIX® */
#include	<fcntl.h>		/* POSIX® */
#include	<ctime>			/* CSTD */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstring>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<ascii.h>		/* LIBU */
#include	<vecstr.h>		/* LIBUC */
#include	<spawner.h>		/* LIBUC */
#include	<filer.h>		/* LIBUC */
#include	<logfile.h>		/* LIBUC */
#include	<strx.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */
#include	<libdebug.h>		/* LIBDEBUG |DEBUGPRINTF(3debug)| */

#include	"maintqotd.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| */

/* local defines */

#define	CHECKER		checker

#ifndef	VBUFLEN
#define	VBUFLEN		(6 * MAXPATHLEN)
#endif

#ifndef	CF_DEBUG
#define	CF_DEBUG	0		/* non-switchable debug print-outs */
#endif


/* external subroutines */


/* external variables */

extern cchar	**environ ;


/* local structures */

struct checker {
	vecstr		stores ;
	MAINTQOTD	*sip ;
	cchar		**envv ;	/* remains nullptr */
	cchar		*pr ;
	cchar		*sep ;
	cchar		*progfname ;
	cchar		**argv ;
	cchar		*a ;		/* allocation */
	int		intcheck ;	/* interval-check */
	int		an ;
} ; /* end struct (checker) */


/* forward references */

local int checker_start(CHECKER *,MAINTQOTD *,cchar *) noex ;
local int checker_finish(CHECKER *) noex ;
local int checker_setentry(CHECKER *,cchar **,cchar *,int) noex ;
local int checker_argbegin(CHECKER *,cchar *) noex ;
local int checker_argend(CHECKER *) noex ;
local int checker_findprog(CHECKER *,char *,cchar *,int) noex ;
local int checker_progrun(CHECKER *,cchar *) noex ;
local int checker_proglog(CHECKER *,int,pid_t,int) noex ;

#ifdef	COMMENT
local int mksfname(char *,cchar *,cchar *,cchar *) noex ;
#endif /* COMMENT */


/* local variables */

cbool		f_debug		= CF_DEBUG ;


/* exported variables */


/* exported subroutines */

int maintqotd_prog(MAINTQOTD *sip,cchar qfname[],cchar *sep) noex {
	CHECKER		c ;
	int		rs ;
	int		rs1 ;
	int		fd = -1 ;

#if	CF_DEBUG
	DEBUGPRINTF("ent\n") ;
#endif

	if (qfname == nullptr) return SR_FAULT ;
	if (sep == nullptr) return SR_FAULT ;

	if ((rs = checker_start(&c,sip,sep)) >= 0) {
	    int		pl = -1 ;
	    cchar	*pp = sep ;
	    cchar	*ap = sep ;
	    cchar	*tp ;
	    char	rbuf[MAXPATHLEN+1] ;
	    if ((tp = strchr(pp,CH_FS)) != nullptr) {
	        pl = (tp-pp) ;
	        ap = (tp+1) ;
	    }
	    if ((rs = checker_findprog(&c,rbuf,pp,pl)) > 0) {
	        if ((rs = checker_argbegin(&c,ap)) >= 0) {

	            rs = checker_progrun(&c,qfname) ;
	            fd = rs ;

#if	CF_DEBUG
	            DEBUGPRINTF("_progrun() rs=%d\n",rs) ;
#endif
	            rs1 = checker_argend(&c) ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (arg) */
	    } /* end if (findprog) */
#if	CF_DEBUG
	    DEBUGPRINTF("findprog-out rs=%d\n",rs) ;
#endif

	    rs1 = checker_finish(&c) ;
	    if (rs >= 0) rs = rs1 ;
	    if ((rs < 0) && (fd >= 0)) {
		uc_unlink(qfname) ;
	        u_close(fd) ;
	        fd = -1 ;
	    }
	} /* end if (checker) */

#if	CF_DEBUG
	DEBUGPRINTF("ret rs=%d fd=%u\n",rs,fd) ;
#endif

	return (rs >= 0) ? fd : rs ;
} /* end subroutine (maintqotd_prog) */


/* local subroutines */

local int checker_start(CHECKER *chp,MAINTQOTD *sip,cchar *sep) noex {
	int		rs ;

	memset(chp,0,sizeof(CHECKER)) ;
	chp->pr = sip->pr ;
	chp->sip = sip ;
	chp->sep = sep ;
	chp->envv = environ ;

	rs = vecstr_start(&chp->stores,1,0) ;

#if	CF_DEBUG
	DEBUGPRINTF("ret rs=%d\n",rs) ;
#endif

	return rs ;
} /* end subroutine (checker_start) */

local int checker_finish(CHECKER *chp) noex {
	int		rs = SR_OK ;
	int		rs1 ;

#if	CF_DEBUG
	DEBUGPRINTF("ent\n") ;
#endif

	rs1 = vecstr_finish(&chp->stores) ;
	if (rs >= 0) rs = rs1 ;

#if	CF_DEBUG
	DEBUGPRINTF("ret rs=%d\n",rs) ;
#endif

	return rs ;
} /* end subroutine (checker_finish) */

int checker_setentry(CHECKER *chp,cchar **epp,cchar *vp,int vl) noex {
	vecstr		*slp = &chp->stores ;
	int		rs = SR_OK ;
	int		oi = -1 ;
	int		len = 0 ;

	if (chp == nullptr) return SR_FAULT ;
	if (epp == nullptr) return SR_INVALID ;

	if (*epp != nullptr)
	    oi = vecstr_findaddr(slp,*epp) ;

	if (vp != nullptr) {
	    len = strnlen(vp,vl) ;
	    rs = vecstr_store(slp,vp,len,epp) ;
	} else if (epp != nullptr)
	    *epp = nullptr ;

	if ((rs >= 0) && (oi >= 0))
	    vecstr_del(slp,oi) ;

	return (rs >= 0) ? len : rs ;
} /* end subroutine (checker_setentry) */


local int checker_argbegin(CHECKER *chp,cchar *ap)
{
	int		rs ;
	int		avsize ;
	int		size = 0 ;
	int		na = 0 ;
	cchar		*sp = ap ;
	cchar		*tp ;
	void		*a ;

	while ((tp = strchr(sp,CH_FS)) != nullptr) {
	    na += 1 ;
	    size += ((tp-sp)+1) ;
	    sp = (tp+1) ;
	} /* end while */
	if (sp[0] != '\0') {
	    na += 1 ;
	    size += (strlen(sp) + 1) ;
	}

	avsize = ((na+1) * sizeof(cchar **)) ;
	size += avsize ;

	chp->an = na ;
	if ((rs = uc_malloc(size,&a)) >= 0) {
	    int		c = 1 ;
	    cchar	**argv = (cchar **) a ;
	    char	*bp = a ;
	    chp->a = a ;
	    chp->argv = argv ;
	    sp = ap ;
	    bp += avsize ;
	    while ((tp = strchr(sp,CH_FS)) != nullptr) {
	        argv[c++] = sp ;
	        bp = (strwcpy(bp,sp,(tp-sp)) + 1) ;
	    }
	    if (sp[0] != '\0') {
	        argv[c++] = sp ;
	        bp = (strwcpy(bp,sp,-1) + 1) ;
	    }
	    argv[c] = nullptr ;
	} /* end if (memory-allocation) */

#if	CF_DEBUG
	DEBUGPRINTF("ret rs=%d n=%u\n",rs,na) ;
#endif

	return (rs >= 0) ? na : rs ;
} /* end subroutine (checker_argbegin) */

local int checker_argend(CHECKER *chp) noex {
	int		rs = SR_OK ;
	int		rs1 ;

	if (chp->a != nullptr) {
	    rs1 = uc_free(chp->a) ;
	    if (rs >= 0) rs = rs1 ;
	    chp->a = nullptr ;
	    chp->argv= nullptr ;
	}

	return rs ;
} /* end subroutine (checker_argend) */

local int checker_findprog(CHECKER *chp,char *rbuf,cchar *pp,int pl) noex {
	int		rs = SR_OK ;
	int		rl = 0 ;

#if	CF_DEBUG
	DEBUGPRINTF("ent pn=%r\n",pp,pl) ;
#endif

	rbuf[0] = '\0' ;
	if (pp[0] != '/') {
	    if ((pp[0] == '\0') || (pp[0] == '-') || (pp[0] == '+')) {
	        pp = MAINTQOTD_PROGMKQOTD ;
	        pl = -1 ;
	    }
	}

#if	CF_DEBUG
	DEBUGPRINTF("pn=%r\n",pp,pl) ;
#endif

	if ((rs = prgetprogpath(chp->pr,rbuf,pp,pl)) >= 0) {
	    cchar	**vpp = &chp->progfname ;
	    rl = (rs > 0) ? rs : strlen(rbuf) ;
	    rs = checker_setentry(chp,vpp,rbuf,rl) ;
	} /* end if */

	if (rs >= 0) {
	    MAINTQOTD	*sip = chp->sip ;
	    if (sip->open.logsub) {
		LOGFILE	*lhp = sip->logsub ;
		logfile_printf(lhp,"svc=prog") ;
		logfile_printf(lhp,"pf=%s",rbuf) ;
	    }
	}

#if	CF_DEBUG
	DEBUGPRINTF("ret rbuf=%s\n",rbuf) ;
	DEBUGPRINTF("ret rs=%d rl=%u\n",rs,rl) ;
#endif

	return (rs >= 0) ? rl : rs ;
} /* end subroutine (checker_findprog) */

local int checker_progrun(CHECKER *chp,cchar *qfname) noex {
	SPAWNER		s ;
	int		rs = SR_OK ;
	int		rs1 ;
	int		fd = -1 ;

#if	CF_DEBUG
	DEBUGPRINTF("ent\n") ;
#endif

	if (chp == nullptr) return SR_FAULT ;

	if (chp->progfname != nullptr) {
	    int		zl ;
	    cchar	*pf = chp->progfname ;
	    cchar	**ev = chp->envv ;
	    cchar	**av = chp->argv ;
	    cchar	*zp ;
	    if ((zl = sfbasename(pf,-1,&zp)) > 0) {
		const mode_t	om = 0664 ;
		const int	of = (O_RDWR|O_CREAT|O_TRUNC) ;
	        cchar		*ap = av[0] ;
	        char		argz[MAXNAMELEN+1] ;
	        if (ap != nullptr) {
	            if ((ap[0] == '+') || (ap[0] == '-') || (ap[0] == '\0')) {
	                char	*bp = argz ;
	                if (ap[0] == '-') *bp++ = '-' ;
	                bp = strnwcpy(bp,(MAXNAMELEN-1),zp,zl) ;
	                av[0] = argz ;
	            }
	        }
	        if ((rs = u_open(qfname,of,om)) >= 0) {
	            fd = rs ;
	            if ((rs = spawner_start(&s,pf,av,ev)) >= 0) {
	                cchar	*varpr = MAINTQOTD_PRNAME ;
	                if (getourenv(ev,varpr) == nullptr) {
	                    rs = spawner_envset(&s,varpr,chp->pr,-1) ;
#if	CF_DEBUG
	    	    DEBUGPRINTF("spawner_envset() rs=%d\n",
			rs) ;
#endif
	                }
#if	CF_DEBUG
	    	    DEBUGPRINTF("about rs=%d\n",rs) ;
#endif
	                if (rs >= 0) {
	                    int	i ;
#if	CF_BACKGROUND
	                    spawner_sigignores(&s) ;
	                    spawner_setsid(&s) ;
#endif
	                    for (i = 0 ; i < 3 ; i += 1) {
	                        spawner_fdclose(&s,i) ;
			    }
	                    spawner_fdnull(&s,O_RDONLY) ;
	                    spawner_fddup(&s,fd) ;
	                    spawner_fdnull(&s,O_WRONLY) ;
	                    if ((rs = spawner_run(&s)) >= 0) {
				pid_t	pid = rs ;
				int	cs = 0 ;

	                	rs1 = spawner_wait(&s,&cs,0) ;
	                	if (rs >= 0) rs = rs1 ;

#if	CF_DEBUG
	                        DEBUGPRINTF("w rs=%d cs=%d\n",
				    rs,cs) ;
#endif

				if (rs >= 0) {
				    rs = checker_proglog(chp,fd,pid,cs) ;
				}

	                        if (rs >= 0) u_rewind(fd) ;
			    } /* end if (spawner_run) */

#if	CF_DEBUG
	                    DEBUGPRINTF("spawner_run() rs=%d\n", rs) ;
	                    debugoutput("checker_progrun: =",fd) ;
#endif

	                } /* end if (ok) */
#if	CF_DEBUG
	    	    DEBUGPRINTF("ok-out rs=%d\n",rs) ;
#endif

	                rs1 = spawner_finish(&s) ;
			if (rs >= 0) rs = rs1 ;
	            } /* end if (spawner) */
#if	CF_DEBUG
	    	    DEBUGPRINTF("spawner-out rs=%d\n",rs) ;
#endif
	            if (rs < 0) {
			uc_unlink(qfname) ;
	                u_close(fd) ;
	                fd = -1 ;
	            }
	        } /* end if (u_open) */
#if	CF_DEBUG
	    	    DEBUGPRINTF("open-out rs=%d\n",rs) ;
#endif
	    } else
	        rs = SR_NOENT ;
	} else
	    rs = SR_NOENT ;

#if	CF_DEBUG
	DEBUGPRINTF("ret rs=%d fd=%u\n",rs,fd) ;
#endif

	return (rs >= 0) ? fd : rs ;
} /* end subroutine (checker_progrun) */

local int checker_proglog(CHECKER *chp,int fd,pid_t pid,int cs) noex {
	MAINTQOTD	*sip = chp->sip ;
	int		rs = SR_OK ;

	if (sip->open.logsub) {
	    LOGFILE	*lhp = sip->logsub ;
	    uint	v = pid ;
	    cchar	*fmt ;
	    if (WIFEXITED(cs)) {
		int	ex = WEXITSTATUS(cs) ;
		fmt = "program (%u) exited normally ex=%u" ;
		logfile_printf(lhp,fmt,v,ex) ;
	    } else if (WIFSIGNALED(cs)) {
		int	sig = WTERMSIG(cs) ;
		cchar	*ss ;
		char	sigbuf[20+1] ;
		if ((ss = strabbrsig(sig)) == nullptr) {
		     rs = ctdeci(sigbuf,20,sig) ;
		     ss = sigbuf ;
		}
		if (rs >= 0) {
		    fmt = "program (%u) exited w/ sig=%s" ;
		    logfile_printf(lhp,fmt,v,ss) ;
		}
	    } else {
		fmt = "program (%u) exited weirdly cs=\\x%08x" ;
		logfile_printf(lhp,fmt,v,cs) ;
	    }
	    if (rs >= 0) {
		if ((rs = uc_fsize(fd)) >= 0) {
		    logfile_printf(lhp,"quote size=%u",rs) ;
		}
	    }
	} /* end if (logging) */
	return rs ;
} /* end subroutine (checker_proglog) */

#ifdef	COMMENT
local int mksfname(char *rbuf,cc *pr,cc *sdname,cc *sname) noex {
	int		rs ;
	if (sdname[0] != '/') {
	    rs = mkpath3(rbuf,pr,sdname,sname) ;
	} else {
	    rs = mkpath2(rbuf,sdname,sname) ;
	}
	return rs ;
} /* end subroutine (mksfname) */
#endif /* COMMENT */

#if	CF_DEBUG
local int debugoutput(cchar *ids,int fd) noex {
	int		rs ;
	int		wlen = 0 ;
	DEBUGPRINTF("%r\n",ids,strlinelen(ids,80,60)) ;
	sleep(2) ;
	if ((rs = uc_fsize(fd)) >= 0) {
	    DEBUGPRINTF("%r fsize=%u\n",ids,strlinelen(ids,80,60),rs) ;
	    if (filer b ; (rs = filer_start(&b,fd,0z,0,0)) >= 0) {
	        cint	llen = LINEBUFLEN ;
	        char	lbuf[LINEBUFLEN+1] ;
	        while ((rs = filer_readln(&b,lbuf,llen,-1)) > 0) {
	            DEBUGPRINTF("o> %r\n",
	                lbuf,strlinelen(lbuf,rs,70)) ;
	        } /* end while */
	        filer_finish(&b) ;
	    } /* end if (filer) */
	} /* end if (fsize) */
	if (rs >= 0) u_rewind(fd) ;
	return (rs >= 0) ? wlen : rs ;
} /* end subroutine */
#endif /* CF_DEBUG */


