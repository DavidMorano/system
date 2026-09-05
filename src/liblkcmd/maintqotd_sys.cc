/* maintqotd_sys SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* SYSTEM dialer for MAINTQOTD */
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
	maintqotd_sys

	Description:
	This subroutine is called for the "system" dialer for MAINTQOTD.

	Synopsis:
	int maintqotd_sys(MAINTQOTD *sip,cchar *qfname,cchar *sep)

	Arguments:
	sip		pointer to local state
	qfname		QOTD-file name
	sep		source entry pointer

	Returns:
	>=0		resulting FD
	<0		some error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* POSIX® */
#include	<sys/param.h>		/* POSIX® */
#include	<sys/stat.h>		/* POSIX® */
#include	<unistd.h>		/* POSIX® */
#include	<fcntl.h>		/* POSIX® */
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
#include	<snwcpy.h>		/* LIBUC */
#include	<strx.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */
#include	<libdebug.h>		/* LIBDEBUG |DEBUGPRINTF(3debug)| */
#include	<systems.h>
#include	<sysdialer.h>
#include	<cm.h>
#include	<maintqotd.h>

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| */

/* local defines */

#define	CHECKER		struct checker

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
	cchar		**envv ;	/* remains NULL */
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
local int checker_proglog(CHECKER *,int) noex ;

#ifdef	COMMENT
local int mksfname(char *,cchar *,cchar *,cchar *) noex ;
#endif /* COMMENT */


/* local variables */


/* exported variables */


/* exported subroutines */

int maintqotd_sys(MAINTQOTD *sip,cchar *qfname,cchar *sep) noex {
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
	        if ((rs = checker_argbegin(&c,sep)) >= 0) {

	            rs = checker_progrun(&c,qfname) ;
	            fd = rs ;

#if	CF_DEBUG
	            DEBUGPRINTF("checker_progrun() rs=%d\n",rs) ;
#endif
	            rs1 = checker_argend(&c) ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (arg) */
	    rs1 = checker_finish(&c) ;
	    if (rs >= 0) rs = rs1 ;
	    if ((rs < 0) && (fd >= 0)) {
	        u_close(fd) ;
	        fd = -1 ;
	    }
	} /* end if (checker) */

#if	CF_DEBUG
	DEBUGPRINTF("ret rs=%d fd=%u\n",rs,fd) ;
#endif

	return (rs >= 0) ? fd : rs ;
} /* end subroutine (maintqotd_sys) */


/* local subroutines */

local int procsystems(PROGINFO *pip,void *ofp,cchar *sfname) noex {
	LOCINFO		*lip = pip->lip ;
	SYSDIALER	d ;
	int		rs ;
	int		rs1 ;
	if ((rs = sysdialer_start(&d,pip->pr,nullptr,nullptr)) >= 0) {
	    CM_ARGS	ca{} ;
	    systems	sysdb ;
	    int		al ;
	    cchar	*ap ;
	    ca.pr = pip->pr ;
	    ca.prn = pip->rootname ;
	    ca.searchname = pip->searchname ;
	    ca.nodename = pip->nodename ;
	    ca.domainname = pip->domainname ;
	    ca.username = pip->username ;
	    ca.sp = &sysdb ;
	    ca.dp = &d ;
	    ca.timeout = lip->to ;
	    ca.options = (SYSDIALER_MFULL | SYSDIALER_MCO) ;

/* do it */

	    if ((rs = systems_open(&sysdb,sfname)) >= 0) {

#if	CF_DEBUG
	            DEBUGPRINTF("systems_open() rs=%d\n",rs) ;
#endif

	        if (sfname == nullptr) {
	            rs = loadsysfiles(pip,&sysdb) ;
	        } /* end if (loadfiles) */

#if	CF_DEBUG && 0
	        {
	            systems_cur		cur ;
	            systems_ent	*sep ;
	            DEBUGPRINTF("sysnames: \n") ;
	            systems_curbegin(&sysdb,&cur) ;
	            while (systems_curenum(&sysdb,&cur,&sep) >= 0) {
	                DEBUGPRINTF("sysname=%s\n",sep->sysname) ;
	            }
	            systems_curend(&sysdb,&cur) ;
	        }
#endif /* CF_DEBUG */

	        if (rs >= 0) {
	            int	i = 0 ;
	            while (rs >= 0) {
	                al = locinfo_argenum(lip,i++,&ap) ;
	                if (al == SR_NOTFOUND) break ;
	                rs = al ;

	                if (rs >= 0) {
	                    if (ap == nullptr) continue ;
	                    rs = procsystem(pip,ofp,&ca,ap) ;
	                }

	            } /* end while */
	        } /* end if */
	        rs1 = systems_close(&sysdb) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (systems) */
	    rs1 = sysdialer_finish(&d) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (sysdialer) */
	return rs ;
} /* end subroutine (procsystems) */

local int procsystem(pip,ofp,cap,ap)
PROGINFO	*pip ;
void		*ofp ;
CM_ARGS		*cap ;
cchar	*ap ;
{
	struct locinfo	*lip = pip->lip ;
	struct query	q ;
	CM		con ;
	const int	llen = LINEBUFLEN ;
	const int	clen = LINEBUFLEN ;
	int		rs ;
	int		to ;
	int		ll ;
	int		cl ;
	int		ql = -1 ;
	int		ropts = 0 ;
	int		wlen = 0 ;
	int		f_long ;
	cchar	**av ;
	cchar	*qp ;
	char		lbuf[LINEBUFLEN + 1], *lp = lbuf ;
	char		cbuf[LINEBUFLEN+1] ;

#if	CF_DEBUG
	{
	    DEBUGPRINTF("ap=>%s<\n",ap) ;
	    DEBUGPRINTF("svc=%s\n",lip->svcspec) ;
	}
#endif

	f_long = lip->fl.longer ;
	av = lip->av ;
	to = lip->to ;

#if	CF_DEBUG
	{
	    DEBUGPRINTF("av=%p\n",av) ;
	    if (av != nullptr) {
	        int	i ;
	        for (i = 0 ; av[i] != nullptr ; i += 1)
	            DEBUGPRINTF("a[%u]=%s\n",i,av[i]) ;
	    }
	}
#endif /* CF_DBEBUGS */

	rs = query_parse(&q,ap) ;
	if (rs < 0) goto ret0 ;

#if	CF_DEBUG
	{
	    DEBUGPRINTF("hpart=>%s<\n",q.hpart) ;
	    DEBUGPRINTF("upart=>%s<\n",q.upart) ;
	}
#endif

	rs = mkfingerquery(lbuf,llen,f_long,q.upart,av) ;
	ql = rs ;
	if (rs < 0) goto ret0 ;
	qp = lbuf ;

#if	CF_DEBUG
	{
	    DEBUGPRINTF("mkfingerquery() rs=%d\n",rs) ;
	    DEBUGPRINTF("q=>%r<\n",qp,ql) ;
	}
#endif /* CF_DEBUG */

	if ((rs = cm_open(&con,cap,q.hpart,lip->svcspec,nullptr)) >= 0) {
	    cchar	*tmpdname = pip->tmpdname ;

#if	CF_DEBUG
	        DEBUGPRINTF("cm_open() rs=%d\n",rs) ;
#endif

/* debug information (if requested) */

	    if (pip->debuglevel > 0) {
	        CM_INFO	ci ;

	        rs = cm_info(&con,&ci) ;

#if	CF_DEBUG
	            DEBUGPRINTF("cm_info() rs=%d\n",rs) ;
#endif

	        if (rs >= 0)
	            shio_printf(pip->efp,"%s: selected dialer=%s\n",
	                pip->progname,ci.dname) ;

	        if (pip->open.logprog)
	            logfile_printf(&pip->lh,"dialer=%s",
	                ((rs >= 0) ? ci.dname : "*")) ;

	    } /* end if (dialer information) */

/* write the query */

	    if (rs >= 0) {
	        if (ql < 0) ql = strlen(qp) ;
	        rs = cm_write(&con,qp,ql) ;
	    }

#if	CF_SHUTDOWN
	    if ((rs >= 0) && lip->fl.shutdown)
	        cm_shutdown(&con,SHUT_WR) ;
#endif /* CF_SHUTDOWN */

/* read the response */

	    if ((rs >= 0) && ((rs = opentmp(tmpdname,O_RDWR,0664)) >= 0)) {
	        int	fd = rs ;

	        while (rs >= 0) {
	            rs = cm_reade(&con,lbuf,llen,to,ropts) ;
	            ll = rs ;
#if	CF_DEBUG
	                DEBUGPRINTF("cm_reade() rs=%d\n",
	                    rs) ;
#endif
	            if (rs <= 0) break ;
	            rs = u_write(fd,lbuf,ll) ;
	        } /* end while */

	        if (rs >= 0) rs = u_rewind(fd) ;

	        if (rs >= 0) {
	            cint	opts = 0 ;
	            if (filer b ; (rs = filer_start(&b,fd,0z,512,opts)) >= 0) {

	                while (rs >= 0) {
	                    rs = filer_readlns(&b,lbuf,llen,to,nullptr) ;
	                    ll = rs ;
	                    if (rs <= 0) break ;

	                    if ((ll > 0) && (lp[ll-1] == '\n')) ll -= 1 ;

	                    if (if_exit) rs = SR_EXIT ;
	                    if ((rs >= 0) && if_int) rs = SR_INTR ;

	                    if (rs >= 0) {
	                        rs = snwcpyclean(cbuf,clen,'¿',lp,ll) ;
	                        cl = rs ;
	                        if (lip->open.outer && (cl > 0)) {
	                            rs = locinfo_termoutprint(lip,ofp,cbuf,cl) ;
	                            wlen += rs ;
	                        } else {
	                            rs = shio_printline(ofp,cbuf,cl) ;
	                            wlen += rs ;
	                        }
	                    }

	                } /* end while (reading lines) */

	                filer_finish(&b) ;
	            } /* end if (filer) */

	        } /* end if */

	        u_close(fd) ;
	    } /* end if (opentmp) */

	    cm_close(&con) ;
	} /* end if (cm) */

ret0:
	return (rs >= 0) ? wlen : rs ;
} /* end subroutine (procsystem) */

local int loadsysfiles(proginfo *pip,systems *sdbp) noex {
	SCHEDVAR	sf ;
	int		rs ;
	int		rs1 ;
	int		i, j ;
	int		n = 0 ;
	char		tmpfname[MAXPATHLEN + 1] ;

	if ((rs = schedvar_start(&sf)) >= 0) {

	    schedvar_add(&sf,"p",pip->pr,-1) ;

	    schedvar_add(&sf,"n",pip->searchname,-1) ;

	    for (j = 0 ; j < 2 ; j += 1) {

	        if (j == 0) {
	            schedvar_add(&sf,"f",SYSFNAME1,-1) ;
	        } else {
	            schedvar_add(&sf,"f",SYSFNAME2,-1) ;
		}

	        for (i = 0 ; sysfiles[i] != nullptr ; i += 1) {

	            rs = schedvar_expand(&sf,tmpfname,MAXPATHLEN,
	                sysfiles[i],-1) ;

	            rs1 = SR_NOENT ;
	            if (rs >= 0)
	                rs1 = u_access(tmpfname,R_OK) ;

	            if (rs1 >= 0) {

	                n += 1 ;
	                rs = systems_fileadd(sdbp,tmpfname) ;

#if	CF_DEBUG
	                {
	                    DEBUGPRINTF("systems_fileadd() rs=%d\n",rs) ;
	                    DEBUGPRINTF("fname=%s\n", tmpfname) ;
	                }
#endif /* CF_DEBUG */

	            } /* end if */

	            if (rs < 0) break ;
	        } /* end for */

	        if (rs < 0) break ;
	    } /* end for */

	    schedvar_finish(&sf) ;
	} /* end if (schedvar) */

	return (rs >= 0) ? n : rs ;
} /* end subroutine (loadsysfiles) */

local int checker_start(CHECKER *chp,MAINTQORD *sip,cchar *sep) noex {
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

local int checker_argbegin(CHECKER *chp,cchar *ap) noex {
	int		rs ;
	int		avsize ;
	int		size = 0 ;
	int		na = 0 ;
	cchar	*sp = ap ;
	cchar	*tp ;
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

	rs = prgetprogpath(chp->pr,rbuf,pp,pl) ;

#if	CF_DEBUG
	DEBUGPRINTF("prgetprogpath() rs=%d rbuf=%s\n",
	    rs,rbuf) ;
#endif

	if (rs >= 0) {
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
	        cchar	*ap = av[0] ;
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
	                }
	                if (rs >= 0) {
	                    int	i ;
#if	CF_BACKGROUND
	                    spawner_sigignores(&s) ;
	                    spawner_setsid(&s) ;
#endif
	                    for (i = 0 ; i < 3 ; i += 1)
	                        spawner_fdclose(&s,i) ;
	                    spawner_fdnull(&s,O_RDONLY) ;
	                    spawner_fddup(&s,fd) ;
	                    spawner_fdnull(&s,O_WRONLY) ;
	                    if ((rs = spawner_run(&s)) >= 0) {
				int	cs = 0 ;

	                	rs1 = spawner_wait(&s,&cs,0) ;
	                	if (rs >= 0) rs = rs1 ;

#if	CF_DEBUG
	                        DEBUGPRINTF("w rs=%d cs=%d\n", rs,cs) ;
#endif

				if (rs >= 0) {
				    rs = checker_proglog(chp,cs) ;
				}

	                        if (rs >= 0) u_rewind(fd) ;
			    } /* end if (spawner_run) */

#if	CF_DEBUG
	                    DEBUGPRINTF("spawner_run() rs=%d\n", rs) ;
	                    debugoutput("checker_progrun: ¬",fd) ;
#endif

	                } /* end if */
	                spawner_finish(&s) ;
	            } /* end if (spawner) */
	            if ((rs < 0) && (fd >= 0)) {
	                u_close(fd) ;
	                fd = -1 ;
	            }
	        } /* end if (piper) */
	    } else
	        rs = SR_NOENT ;
	} else
	    rs = SR_NOENT ;

#if	CF_DEBUG
	DEBUGPRINTF("ret rs=%d fd=%u\n",rs,fd) ;
#endif

	return (rs >= 0) ? fd : rs ;
} /* end subroutine (checker_progrun) */

local int checker_proglog(CHECKER *chp,int cs) noex {
	MAINTQOTD	*sip = chp->sip ;
	int		rs = SR_OK ;

	if (sip->open.logsub) {
	   LOGFILE	*lhp = sip->logsub ;
	    if (WIFEXITED(cs)) {
		int	ex = WEXITSTATUS(cs) ;
		logfile_printf(lhp,"program exited normally ex=%u",ex) ;
	    } else if (WIFSIGNALED(cs)) {
		int	sig = WTERMSIG(cs) ;
		cchar	*ss ;
		char		sigbuf[20+1] ;
		if ((ss = strabbrsig(sig)) == nullptr) {
		     ctdeci(sigbuf,20,sig) ;
		     ss = sigbuf ;
		}
		logfile_printf(lhp,"program exited w/ sig=%s",ss) ;
	    } else {
		logfile_printf(lhp,"program exited weirdly cs=\\x%08x",cs) ;
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


