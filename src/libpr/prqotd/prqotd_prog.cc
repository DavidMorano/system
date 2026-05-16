/* prqotd_prog SUPPORT */
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
	subinfo_prog

	Description:
	This subroutine is called for the "prog" dialer for MAINTQOTD.

	Synopsis:
	int subinfo_prog(subinfo *sip,cchar *qfname,cchar *sep) noex

	Arguments:
	sip		pointer to local state
	qfname		QOTD-file name
	sep		source entry pointer

	Returns:
	>=0		resulting FD
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* system types |pid_t| */
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>		/* |strchr(3c)| */
#include	<utility>		/* |pair(3c++)| */
#include	<clanguage.h>
#include	<usysbase.h>
#include	<usyscalls.h>
#include	<uclibmem.h>
#include	<ucopen.h>
#include	<ucdesc.h>
#include	<ucfileop.h>
#include	<vecstr.h>
#include	<ascii.h>
#include	<spawner.h>
#include	<filer.h>
#include	<logfile.h>
#include	<ctdec.h>
#include	<sfx.h>			/* |sfbasename(3uc)| */
#include	<strn.h>		/* |strnwcpy(3uc)| */
#include	<strx.h>
#include	<strwcpy.h>
#include	<ischarx.h>		/* |ispm(3uc)| */
#include	<localmisc.h>
#include	<libpr.h>		/* LIBPR */
#include	<libdebug.h>		/* LIBDEBUG */

#include	"prqotd.h"
#include	"prqotd_util.hh"
#include	"prqotd_subinfo.hh"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| */

/* local defines */

#define	SUB		subinfo
#define	CK		checker

#ifndef	CF_DEBUG
#define	CF_DEBUG	0		/* non-switchable debug print-outs */
#endif
#ifndef	CF_BACKGROUND
#define	CF_BACKGROUND	0		/* put program in background */
#endif


/* imported namespaces */

using std::pair ;			/* type */
using prqotd::var ;			/* type */
using prqotd::subinfo ;			/* type */
using prqotd::config ;			/* type */
using prqotd::config_start ;		/* subroutine */
using prqotd::config_finish ;		/* subroutine */
using prqotd::config_read ;		/* subroutine */
using prqotd::config_check ;		/* subroutine */
using prqotd::getdefmjd ;		/* subroutine */
using prqotd::mkqfname ;		/* subroutine */
using prqotd::mkourname ;		/* subroutine */
using libuc::libmem ;			/* variable */


/* local typedefs */

typedef config *	configp ;
typedef logfile *	logfilep ;
typedef vecstr *	vecstrp ;


/* external subroutines */


/* external variables */


/* local structures */

struct checker {
	vecstr		*slp ;		/* store-list-pointer */
	SUB		*sip ;		/* caller supplied */
	mainv		envv ;		/* <- type |mainv| is desired */
	ccharpp		argv ;		/* <- type |ccharpp| */
	cchar		*pr ;
	cchar		*sep ;
	cchar		*progfname ;
	char		*a ;		/* allocated */
	int		intcheck ;	/* interval-check */
	int		an ;
} ; /* end struct (checker) */


/* forward references */

local int checker_start		(CK *,SUB *,cchar *) noex ;
local int checker_finish	(CK *) noex ;
local int checker_setentry	(CK *,cchar **,cchar *,int) noex ;
local int checker_argbegin	(CK *,cchar *) noex ;
local int checker_argend	(CK *) noex ;
local int checker_findprog	(CK *,char *,cchar *,int) noex ;
local int checker_progrun	(CK *,cchar *) noex ;
local int checker_proglog	(CK *,int,pid_t,int) noex ;

#ifdef	COMMENT
local int mksfname(char *,cchar *,cchar *,cchar *) noex ;
#endif /* COMMENT */


/* local variables */

cbool		f_debug		= CF_DEBUG ;
cbool		f_background	= CF_BACKGROUND	;


/* exported variables */


/* exported subroutines */

namespace ptqotd {
    int subinfo_prog(SUB *sip,cchar *qfname,cchar *sep) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		fd = -1 ;
	DEBUGPRINTF("ent qfn=%s\n",qfname) ;
	if (sip && qfname && sep) ylikely {
	    if (char *rbuf ; (rs = lm_mp(&rbuf)) >= 0) ylikely {
	        if (CK c ; (rs = checker_start(&c,sip,sep)) >= 0) ylikely {
	            int		pl = -1 ; /* used-multiple */
	            cchar	*pp = sep ;
	            cchar	*ap = sep ;
	            if (cchar *tp = strchr(pp,CH_FS) ; tp) {
	                pl = intconv(tp - pp) ;
	                ap = (tp + 1) ;
	            } /* end if */
	            if ((rs = checker_findprog(&c,rbuf,pp,pl)) > 0) ylikely {
	                if ((rs = checker_argbegin(&c,ap)) >= 0) ylikely {
		            {
	                        rs = checker_progrun(&c,qfname) ;
	                        fd = rs ;
	                        DEBUGPRINTF("_progrun() rs=%d\n",rs) ;
		            }
	                    rs1 = checker_argend(&c) ;
	                    if (rs >= 0) rs = rs1 ;
	                } /* end if (arg) */
	            } /* end if (findprog) */
	            DEBUGPRINTF("findprog-out rs=%d\n",rs) ;
	            rs1 = checker_finish(&c) ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (checker) */
	        rs1 = lm_free(rbuf) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (m-a-f) */
	    if ((rs < 0) && (fd >= 0)) nlikely {
		uc_unlink(qfname) ;
	        u_close(fd) ;
	        fd = -1 ;
	    } /* end if (error) */
	} /* end if (non-null) */
	DEBUGPRINTF("ret rs=%d fd=%d\n",rs,fd) ;
	return (rs >= 0) ? fd : rs ;
    } /* end subroutine (maintqotd_prog) */
} /* end namespace (subinfo_prog) */


/* local subroutines */

local int checker_start(CK *chp,SUB *sip,cchar *sep) noex {
    	cint		vsz = szof(vecstr) ;
	int		rs ;
	memclear(chp) ;
	chp->pr = sip->pr ;
	chp->sip = sip ;
	chp->sep = sep ;
	chp->envv = var.envv ;
	if (vecstr *slp ; (rs = lm_mall(vsz,&slp)) >= 0) ylikely {
	    chp->slp = slp ;
	    {
	        rs = slp->start(1,0) ;
	    }
	    if (rs < 0) nlikely {
	       lm_free(slp) ;
	       chp->slp = nullptr ;
	    } /* end if (error) */
	} /* end if (memory-acquire) */
	DEBUGPRINTF("ret rs=%d\n",rs) ;
	return rs ;
} /* end subroutine (checker_start) */

local int checker_finish(CK *chp) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	DEBUGPRINTF("ent\n") ;
	if (chp->slp) {
	    vecstr *slp = chp->slp ;
	    {
	        rs1 = slp->finish ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (finish) */
	    if (chp->a) { /* <- should never happen */
		rs1 = lm_free(chp->a) ;
	        if (rs >= 0) rs = rs1 ;
		chp->a = nullptr ;
	    } /* end if (memory-release) */
	    {
		rs1 = lm_free(chp->slp) ;
	        if (rs >= 0) rs = rs1 ;
		chp->slp = nullptr ;
	    } /* end if (memory-release) */
	} /* end if */
	DEBUGPRINTF("ret rs=%d\n",rs) ;
	return rs ;
} /* end subroutine (checker_finish) */

local int checker_setentry(CK *chp,cchar **epp,cchar *vap,int val) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		len = 0 ; /* return-value */
	DEBUGPRINTF("ent\n") ;
	if (chp && epp) {
	    vecstr	*slp = chp->slp ;
	    int		oi = -1 ;
	    rs = SR_OK ;
	    if (*epp) {
	        oi = vecstr_findaddr(slp,*epp) ;
	    }
	    if (vap) {
	        len = lenstr(vap,val) ;
	        rs = vecstr_store(slp,vap,len,epp) ;
	    } else if (epp) {
	        *epp = nullptr ;
	    }
	    if ((rs >= 0) && (oi >= 0)) {
	        rs1 = vecstr_del(slp,oi) ;
		if (rs >= 0) rs = rs1 ;
	    }
	} /* end if (non-null) */
	DEBUGPRINTF("ret rs=%d len=%d\n",rs,len) ;
	return (rs >= 0) ? len : rs ;
} /* end subroutine (checker_setentry) */

#ifdef	COMMENT
namespace {
    struct argsizer {
	int	na ;
	int	sz ;
	argsizer(cchar *sp,int chx) noex {
    	    cnullptr	np{} ;
	    for (cchar *tp ; (tp = strchr(sp,chx)) != np ; ) {
	        na += 1 ;
	        sz += intconv((tp - sp) + 1) ;
	        sp = (tp + 1) ;
	    } /* end for */
	    if (sp[0] != '\0') {
	        na += 1 ;
	        sz += (lenstr(sp) + 1) ;
	    } /* end if */
	    sz += ((na + 1) * szof(cchar **)) ;
	} ; /* end ctor */
    } ; /* end struct (argsizer) */
} /* end namespace */
#endif /* COMMENT */

namespace {
    struct argloader {
	ccharpp		av ;
	argloader(char *a,cchar *sp,int na,int chx) noex {
	    cnullptr	np{} ;
	    cint	avsz = (na + 1) * szof(cchar **) ;
	    char	*bp = a ;
	    int		c = 1 ;
	    av = ccharpp(a) ;
	    bp += avsz ;
	    for (cchar *tp ; (tp = strchr(sp,chx)) != np ; ) {
		cint tl = intconv(tp - sp) ;
	        av[c++] = bp ;
		bp = (strwcpy(bp,sp,tl) + 1) ;
		sp = (tp + 1) ;
	    } /* end for */
	    if (sp[0] != '\0') {
	        av[c++] = sp ;
	        bp = (strwcpy(bp,sp,-1) + 1) ;
	    } /* end if */
	    av[c] = nullptr ;
	} /* end ctor */
    } ; /* end struct (argloader) */
} /* end namespace */

local pair<int,int> argsz(cchar *sp,int chx) noex {
    	cnullptr	np{} ;
    	pair<int,int>	rv{} ;
	int	sz = 0 ; /* return-value */
	int	na = 0 ;
	for (cchar *tp ; (tp = strchr(sp,chx)) != np ; ) {
	    na += 1 ;
	    sz += intconv((tp - sp) + 1) ;
	    sp = (tp + 1) ;
	} /* end for */
	if (sp[0] != '\0') {
	    na += 1 ;
	    sz += (lenstr(sp) + 1) ;
	} /* end if */
	rv.first = na ;
	rv.second = ((na + 1) * szof(cchar **)) ;
	return rv ;
} /* end subroutine (argsz) */

local int checker_argbegin(CK *chp,cchar *ap) noex {
    	cauto		[na, sz] = argsz(ap,CH_FS) ;
	int		rs ;
	DEBUGPRINTF("ent\n") ;
	if (char *a ; (rs = lm_mall(sz,&a)) >= 0) ylikely {
	    const argloader	ald(a,ap,na,CH_FS) ;
	    chp->a = a ;
	    chp->argv = ald.av ;
	} /* end if (memory-acquire) */
	DEBUGPRINTF("ret rs=%d na=%d\n",rs,na) ;
	return (rs >= 0) ? na : rs ;
} /* end subroutine (checker_argbegin) */

local int checker_argend(CK *chp) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (chp->a) {
	    rs1 = lm_free(chp->a) ;
	    if (rs >= 0) rs = rs1 ;
	    chp->a = nullptr ;
	    chp->argv = nullptr ;
	} /* end if (memory-release) */
	return rs ;
} /* end subroutine (checker_argend) */

local int checker_findprog(CK *chp,char *rbuf,cchar *pp,int pl) noex {
	int		rs = SR_OK ;
	int		rl = 0 ;
	DEBUGPRINTF("ent\n") ;
	rbuf[0] = '\0' ;
	if (pp[0] != '/') {
	    if (ispm(pp[0]) || (pp[0] == '\0')) {
	        pp = PRQOTD_PROG ;
	        pl = -1 ;
	    }
	} /* end if */
	if ((rs = prgetprogpath(chp->pr,rbuf,pp,pl)) >= 0) ylikely {
	    cchar	**vpp = &chp->progfname ;
	    rl = (rs > 0) ? rs : lenstr(rbuf) ;
	    rs = checker_setentry(chp,vpp,rbuf,rl) ;
	} /* end if */
	if (rs >= 0) ylikely {
	    SUB	*sip = chp->sip ;
	    if (sip->open.logsub) {
		logfile	*lhp = logfilep(sip->lfp) ;
		logfile_printf(lhp,"svc=prog") ;
		logfile_printf(lhp,"pf=%s",rbuf) ;
	    } /* end if (logging) */
	} /* end if (ok) */
	DEBUGPRINTF("ret rs=%d rl=%d\n",rs,rl) ;
	return (rs >= 0) ? rl : rs ;
} /* end subroutine (checker_findprog) */

namespace {
    struct progrunner {
	CK	*chp ;
	cchar	*qfname ;
	char	*argz{} ;		/* allocated */
	int	maxname ;
	progrunner(CK *p,cchar *q) noex : chp(p), qfname(q) { 
	    maxname = var.maxnamelen ;
	} ;
	int proc() noex ;
	operator int () noex ;
    } ; /* end struct (progrunner) */
} /* end namespace */

local int checker_progrun(CK *chp,cchar *qfname) noex {
    	int		rs = SR_FAULT ;
	int		fd = -1 ;
	if (chp) ylikely {
	    rs = SR_NOENT ;
	    if (chp->progfname) ylikely {
		progrunner ro(chp,qfname) ;
		rs = ro ;
		fd = rs ;
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? fd : rs ;
} /* end subroutine (checker_progrun) */

progrunner::operator int () noex {
    	int		rs ;
	int		rs1 ;
	int		fd = -1 ;
	if ((rs = lm_mn(&argz)) >= 0) ylikely {
	    {
	        rs = proc() ;
	        fd = rs ;
	    }
	    rs1 = lm_free(argz) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return (rs >= 0) ? fd : rs ;
} /* end method (progrunner::operator) */

int progrunner::proc() noex {
	int		rs = SR_NOENT ;
	int		rs1 ;
	int		fd = -1 ;
	DEBUGPRINTF("ent\n") ;
        cchar       *pf = chp->progfname ;
        mainv       ev = chp->envv ;
        ccharp      *av = chp->argv ; /* <- type |ccharpp| */
        cchar *zp ;
        if (int zl ; (zl = sfbasename(pf,-1,&zp)) > 0) ylikely {
            cint    of = (O_RDWR|O_CREAT|O_TRUNC) ;
            cmode   om = 0664 ;
            cchar   *ap = av[0] ;
            if (ap) {
                if (ispm(ap[0]) || (ap[0] == '\0')) {
		    int		blen = maxname ;
                    char    *bp = argz ;
                    if (ap[0] == '-') {
			*bp++ = '-' ;
			blen -= 1 ;
		    }
                    bp = strnwcpy(bp,blen,zp,zl) ;
                    av[0] = argz ;
                } /* end if (special) */
            } /* end if */
            if ((rs = u_open(qfname,of,om)) >= 0) ylikely {
                spawner s ;
                fd = rs ;
                if ((rs = spawner_start(&s,pf,av,ev)) >= 0) ylikely {
                    cchar   *varpr = PRQOTD_PRNAME ;
                    if (getourenv(ev,varpr) == nullptr) {
                        rs = spawner_envset(&s,varpr,chp->pr,-1) ;
                    }
                    if (rs >= 0) ylikely {
                        if_constexpr (f_background) {
                            spawner_sigignores(&s) ;
                            spawner_setsid(&s) ;
                        } /* end if_constexpr (f_background) */
                        for (int i = 0 ; i < 3 ; i += 1) {
                            spawner_fdclose(&s,i) ;
                        } /* end for */
                        spawner_fdnull(&s,O_RDONLY) ;
                        spawner_fddup(&s,fd) ;
                        spawner_fdnull(&s,O_WRONLY) ;
                        if ((rs = spawner_run(&s)) >= 0) ylikely {
                            con pid_t       pid = rs ;
                            int             cs = 0 ;
                            {
                                rs1 = spawner_wait(&s,&cs,0) ;
                                if (rs >= 0) rs = rs1 ;
                            }
                            DEBUGPRINTF("w rs=%d cs=%d\n",rs,cs) ;
                            if (rs >= 0) ylikely {
                                rs = checker_proglog(chp,fd,pid,cs) ;
                            }
                            if (rs >= 0) u_rewind(fd) ;
                        } /* end if (spawner_run) */
                        DEBUGPRINTF("spawner_run() rs=%d\n", rs) ;
                    } /* end if (ok) */
                    DEBUGPRINTF("ok-out rs=%d\n",rs) ;
                    rs1 = spawner_finish(&s) ;
                    if (rs >= 0) rs = rs1 ;
                } /* end if (spawner) */
		DEBUGPRINTF("spawner-out rs=%d\n",rs) ;
                if (rs < 0) nlikely {
                    uc_unlink(qfname) ;
                    u_close(fd) ;
                    fd = -1 ;
                } /* end if (error) */
            } /* end if (u_open) */
            DEBUGPRINTF("open-out rs=%d\n",rs) ;
        } /* end if (sfbasename) */
	DEBUGPRINTF("ret rs=%d fd=%d\n",rs,fd) ;
	return (rs >= 0) ? fd : rs ;
} /* end method (progrunner::proc) */

local int checker_proglog(CK *chp,int fd,pid_t pid,int cs) noex {
	SUB		*sip = chp->sip ;
	int		rs = SR_OK ;
	if (sip->open.logsub) {
	    logfile	*lhp = logfilep(sip->lfp) ;
	    con uint	v = pid ;
	    cchar	*fmt ;
	    if (WIFEXITED(cs)) {
		cint	ex = WEXITSTATUS(cs) ;
		fmt = "program (%u) exited normally ex=%u" ;
		logfile_printf(lhp,fmt,v,ex) ;
	    } else if (WIFSIGNALED(cs)) {
		cint	sig = WTERMSIG(cs) ;
		cchar	*ss ;
		char	sigbuf[20+1] ;
		if ((ss = strabbrsig(sig)) == nullptr) {
		     rs = ctdeci(sigbuf,20,sig) ;
		     ss = sigbuf ;
		} /* end if */
		if (rs >= 0) {
		    fmt = "program (%u) exited w/ sig=%s" ;
		    logfile_printf(lhp,fmt,v,ss) ;
		} /* end if (ok) */
	    } else {
		fmt = "program (%u) exited weirdly cs=\\x%08x" ;
		logfile_printf(lhp,fmt,v,cs) ;
	    } /* end if */
	    if (rs >= 0) ylikely {
		if ((rs = uc_fsize(fd)) >= 0) {
		    logfile_printf(lhp,"quote size=%u",rs) ;
		}
	    } /* end if (ok) */
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
	int		rs1 ;
	int		wlen = 0 ;
	debugprintf("%r\n",ids,strlinelen(ids,80,60)) ;
	sleep(2) ;
	if ((rs = uc_fsize(fd)) >= 0) {
	    debugprintf("%r fsize=%u\n",ids,strlinelen(ids,80,60),rs) ;
	    if (char *lbuf ; (rs = lm_ml(&lbuf)) >= 0) {
		cint llen = rs ;
	        if (filer b ; (rs = filer_start(&b,fd,0z,0,0)) >= 0) {
	            while ((rs = filer_readln(&b,lbuf,llen,-1)) > 0) {
	                debugprintf("o> %r\n",lbuf,strlinelen(lbuf,rs,70)) ;
	            } /* end while */
	            rs1 = filer_finish(&b) ;
		    if (rs >= 0) rs = rs1 ;
	        } /* end if (filer) */
	        rs1 = lm_free(lbuf) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (m-a-f) */
	} /* end if (fsize) */
	if (rs >= 0) u_rewind(fd) ;
	return (rs >= 0) ? wlen : rs ;
} /* end subroutine */
#endif /* CF_DEBUG */


