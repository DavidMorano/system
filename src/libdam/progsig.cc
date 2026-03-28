/* progsig SUPPORT (XXX INCOMPLETE TODO) */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* program signal handling */
/* version %I% last-modified %G% */

#define	CF_SIGTSTP	1		/* Solaris® bug in |sigpending(2)| */

/* revision history:

	= 2000-05-14, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2000 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	progsig

	Description:
	Manage process signals.

	Deveopment:
	This is derived from LIBKSH.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<poll.h>
#include	<ctime>
#include	<csignal>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<clanguage.h>
#include	<usysbase.h>
#include	<usyscalls.h>
#include	<uclibmem.h>
#include	<ucatexit.h>
#include	<ucatfork.h>
#include	<ucfork.h>
#include	<getbufsize.h>
#include	<aflag.hh>
#include	<upt.h>
#include	<ptm.h>
#include	<ptc.h>
#include	<sighand.h>
#include	<raqhand.h>
#include	<sockaddress.h>
#include	<listenusd.h>
#include	<strwcpy.h>
#include	<mkpathx.h>
#include	<mkdirs.h>
#include	<permx.h>
#include	<ctdec.h>
#include	<sesmsg.hh>		/* "sesion message" */
#include	<msgdata.h>
#include	<mkchar.h>
#include	<localmisc.h>

#include	"progsig.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |memclear(3u)| */

/* local defines */

#ifndef	POLL_INTMULT
#define	POLL_INTMULT	1000		/* poll-time multiplier */
#endif

#ifndef	MSGBUFLEN
#define	MSGBUFLEN	2048
#endif

#ifndef	CMSGBUFLEN
#define	CMSGBUFLEN	256
#endif

#ifndef	SYMNAMELEN
#define	SYMNAMELEN	60
#endif

#define	TO_LOCKENV	10

#define	PROGSIG_SCOPE	PTHREAD_SCOPE_PROCESS
#define	PROGSIG_SESDN	"/var/tmp/sessions"

#define	SN		struct snote	/* store-note */


/* imported namespaces */


/* local typedefs */

#ifndef	TYPEDEF_TWORKER
#define	TYPEDEF_TWORKER
extern "C" {
    typedef	int (*tworker_f)(void *) noex ;
}
#endif

extern "C" {
    typedef	int (*cmdsub_f)(int,cchar **,cchar **,void *) noex ;
    typedef	int (*func_caller)(int,cchar **,void *) noex ;
}


/* external subroutines */

extern "C" {
    typedef int (*subcmd_f)(int,cchar **,cchar **,void *) noex ;
}

extern "C" {
    int		progsig_init(void) noex ;
    int		progsig_fini(void) noex ;
    int		progsig_initenviron(void *) noex ;
    int		progsig_callcmd(cchar *,int,cchar **,cchar **,void *) noex ;
    int 	progsig_callfunc(subcmd_f,int,cchar **,cchar **,void *) noex ;
    int		progsig_initmemalloc(int) noex ;
}

extern "C" {
    extern int uc_stat(cchar *,ustat *) noex ;
    extern int uc_minmod(cchar *,mode_t) noex ;
    extern int uc_unlink(cchar *) noex ;
    extern int uc_close(int) ;
    extern int uc_closeonexec(int,int) noex ;
} /* end extern */


/* external variables */

extern char	**environ ;


/* local structures */

struct progsig_fl {
	uint		dummy:1 ;
} ; /* end struct */

struct progsig {
	ptm		mx ;		/* mutex data */
	ptc		cn ;		/* condition variable */
	sighand		sm ;
	sockaddress	servaddr ;	/* server address */
	raqhand		mq ;		/* message queue */
	cchar		*reqfname ;
	cchar		**envv ;
	progsig_fl	fl ;
	pthread_t	tid ;
	pid_t		pid ;
	volatile int	f_init ;
	volatile int	f_initdone ;
	volatile int	f_running ;
	volatile int	f_capture ;
	volatile int	f_exiting ;
	volatile int	waiters ;
	sig_atomic_t	f_sigquit ;
	sig_atomic_t	f_sigterm ;
	sig_atomic_t	f_sigintr ;
	sig_atomic_t	f_sigwich ;
	sig_atomic_t	f_sigsusp ;
	sig_atomic_t	f_sigchild ;
	int		f_mq ;
	int		runmode ;
	int		serial ;
	int		sfd ;
	int		cdefs ;		/* defualt count */
	int		servlen ;	/* serv-addr length */
} ; /* end struct */

struct snote { /* store-note */
	time_t		stime ;
	cchar		*dbuf ;
	cchar		*user ;
	char		*a ;
	int		type ;
	int		dlen ;
} ; /* end struct */

enum cmds {
	cmd_session,
	cmd_overlast
} ; /* end enum (cmds) */


/* forward references */

extern "C" {
    static void	progsig_atforkbefore() noex ;
    static void	progsig_atforkafter() noex ;
    static void	progsig_exit() noex ;
    static void	progsig_sighand(int,siginfo_t *,void *) noex ;
}

local int	progsig_mainstuff(progsig *) noex ;

local int	progsig_begin(progsig *) noex ;
local int	progsig_end(progsig *) noex ;
local int	progsig_runbegin(progsig *) noex ;
local int	progsig_runner(progsig *) noex ;
local int	progsig_runend(progsig *) noex ;
local int	progsig_entfins(progsig *) noex ;
local int	progsig_mq(progsig *) noex ;
local int	progsig_mkreqfname(progsig *,char *,cchar *) noex ;
local int	progsig_worker(progsig *) noex ;
local int	progsig_workecho(progsig *,msgdata *) noex ;
local int	progsig_workbiff(progsig *,msgdata *) noex ;
local int	progsig_workbiffer(progsig *,SESMSG_BIFF *) noex ;
local int	progsig_workgen(progsig *,msgdata *) noex ;
local int	progsig_workgener(progsig *,SESMSG_GEN *) noex ;
local int	progsig_workdef(progsig *,msgdata *) noex ;
local int	progsig_msgenter(progsig *,SN *) noex ;
local int	progsig_reqopen(progsig *) noex ;
local int	progsig_reqopener(progsig *,cchar *) noex ;
local int	progsig_reqsend(progsig *,msgdata *,int) noex ;
local int	progsig_reqrecv(progsig *,msgdata *) noex ;
local int	progsig_reqclose(progsig *) noex ;
local int	progsig_poll(progsig *) noex ;
local int	progsig_cmdsend(progsig *,int) noex ;
local int	progsig_capbegin(progsig *,int) noex ;
local int	progsig_capend(progsig *) noex ;
local int	progsig_sigbegin(progsig *) noex ;
local int	progsig_sigend(progsig *) noex ;

local int snote_start(SN *,int,time_t,cc *,cc *,int) noex ;
local int snote_finish(SN *) noex ;

local int	libmalstrw(cchar *,int,cchar **) noex ;
local int	sdir(cchar *,int) noex ;
local int	mksdir(cchar *,mode_t) noex ;
local int	mksdname(char *,cchar *,pid_t) noex ;


/* local variables */

static progsig		progsig_data ;

constexpr int		sigblocks[] = {
	SIGUSR1,
	SIGUSR2,
	SIGHUP,
	SIGCHLD,
	0
} ; /* end array */

constexpr int		sigigns[] = {
	SIGPIPE,
	SIGPOLL,
	SIGXFSZ,
	0
} ; /* end array */

constexpr int		sigints[] = {
	SIGQUIT,
	SIGTERM,
	SIGINT,
	SIGWINCH,
	SIGCHLD,
	SIGTSTP,
	0
} ; /* end array */


/* exported variables */


/* exported subroutines */

int progsig_init(void) noex {
	progsig		*uip = &progsig_data ;
	int		rs = SR_OK ;
	if (! uip->f_init) {
	    ptm *mxp = &uip->mx ;
	    uip->f_init = true ;
	    if ((rs = mxp->create) >= 0) {
	        ptc *cnp = &uip->cn ;
	        if ((rs = cnp->create) >= 0) {
	            void_f	b = progsig_atforkbefore ;
	            void_f	a = progsig_atforkafter ;
	            if ((rs = uc_atforkrec(b,a,a)) >= 0) {
	                if ((rs = uc_atexit(progsig_exit)) >= 0) {
			    uip->pid = getpid() ;
			    uip->sfd = -1 ;
		            rs = 1 ;
	    	            uip->f_initdone = true ;
		        }
		        if (rs < 0) {
		            uc_atforkexp(b,a,a) ;
			}
	            } /* end if (uc_atfork) */
	            if (rs < 0) {
	                cnp->destroy() ;
		    }
	        } /* end if (ptc_create) */
		if (rs < 0) {
		    mxp->destroy() ;
		}
	    } /* end if (ptm_create) */
	    if (rs < 0) {
	        uip->f_init = false ;
	    }
	} else {
	    while (! uip->f_initdone) msleep(1) ;
	}
	return rs ;
}
/* end subroutine (progsig_init) */

int progsig_fini(void) noex {
	progsig		*uip = &progsig_data ;
	int		rs = SR_NXIO ;
	int		rs1 ;
	if (uip->f_initdone) {
	    uip->f_initdone = false ;
	    rs = SR_OK ;
	    {
	        rs1 = progsig_runend(uip) ;
		if (rs >= 0) rs = rs1 ;
	    }
	    {
		rs1 = progsig_end(uip) ;
		if (rs >= 0) rs = rs1 ;
	    }
	    {
	        void_f	b = progsig_atforkbefore ;
	        void_f	a = progsig_atforkafter ;
	        rs1 = uc_atforkexp(b,a,a) ;
		if (rs >= 0) rs = rs1 ;
	    }
	    {
	        ptc *cnp = &uip->cn ;
	        rs1 = cnp->destroy ;
		if (rs >= 0) rs = rs1 ;
	    }
	    {
	        ptm *mxp = &uip->mx ;
	        rs1 = mxp->destroy ;
		if (rs >= 0) rs = rs1 ;
	    }
	    uip->f_init = false ;
	    uip->f_initdone = false ;
	} /* end if (atexit registered) */
	return rs ;
}
/* end subroutine (progsig_fini) */

int progsig_mainbegin(cchar **envv) noex {
	int		rs ;
	if ((rs = progsig_init()) >= 0) {
	    progsig	*uip = &progsig_data ;
	    uip->envv = envv ;
	    if ((rs = progsig_sigbegin(uip)) >= 0) {
		rs = progsig_mainstuff(uip) ;
		if (rs < 0) {
	    	    progsig_sigend(uip) ;
		}
	    } /* end if (progsig_sigbegin) */
	} /* end if (progsig_init) */
	return rs ;
}
/* end subroutine (progsig_mainbegin) */

int progsig_mainend(void) noex {
	progsig		*uip = &progsig_data ;
	int		rs = SR_OK ;
	int		rs1 ;
	if (uip->f_running) {
	    rs1 = progsig_runend(uip) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (running) */
	{
	    rs1 = progsig_sigend(uip) ;
	    if (rs >= 0) rs = rs1 ;
	}
	uip->envv = nullptr ;
	return rs ;
}
/* end subroutine (progsig_mainend) */

int progsig_adm(int cmd) noex {
	progsig		*kip = &progsig_data ;
	int		rs = SR_OK ;
	switch (cmd) {
	case cmd_session:
	    rs = progsig_runbegin(kip) ;
	    break ;
	default:
	    rs = SR_INVALID ;
	    break ;
	} /* end switch */
	return rs ;
}
/* end subroutine (progsig_runmode) */

int progsig_runmode(void) noex {
	progsig		*kip = &progsig_data ;
	return kip->runmode ;
}
/* end subroutine (progsig_runmode) */

int progsig_serial() noex {
	progsig		*kip = &progsig_data ;
	int		s = kip->serial ;
	return s ;
}
/* end subroutine (progsig_serial) */

int progsig_sigquit(void) noex {
	progsig		*kip = &progsig_data ;
	int		rs = SR_OK ;
	if (kip->f_sigquit) {
	    kip->f_sigquit = 0 ;
	    rs = SR_QUIT ;
	}
	return rs ;
}
/* end subroutine (progsig_sigquit) */

int progsig_sigterm() noex {
	progsig		*kip = &progsig_data ;
	int		rs = SR_OK ;
	if (kip->f_sigterm) {
	    kip->f_sigterm = 0 ;
	    rs = SR_EXIT ;
	}
	return rs ;
}
/* end subroutine (progsig_sigterm) */

int progsig_sigintr() noex {
	progsig		*kip = &progsig_data ;
	int		rs = SR_OK ;
	if (kip->f_sigintr) {
	    kip->f_sigintr = 0 ;
	    rs = SR_INTR ;
	}
	return rs ;
}
/* end subroutine (progsig_sigintr) */

int progsig_issig(int sn) noex {
	progsig		*kip = &progsig_data ;
	int		rs = SR_OK ;
	int		f = false ;
	switch (sn) {
	case SIGQUIT:
	    f = kip->f_sigquit ;
	    if (f) kip->f_sigquit = 0 ;
	    break ;
	case SIGTERM:
	    f = kip->f_sigterm ;
	    if (f) kip->f_sigterm = 0 ;
	    break ;
	case SIGINT:
	    f = kip->f_sigintr ;
	    if (f) kip->f_sigintr = 0 ;
	    break ;
	case SIGWINCH:
	    f = kip->f_sigwich ;
	    if (f) kip->f_sigwich = 0 ;
	    break ;
	case SIGTSTP:
	    f = kip->f_sigsusp ;
	    if (f) kip->f_sigsusp = 0 ;
	    break ;
	case SIGCHLD:
	    f = kip->f_sigchild ;
	    if (f) kip->f_sigchild = 0 ;
	    break ;
	default:
	    rs = SR_INVALID ;
	    break ;
	} /* end switch */
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (progsig_issig) */

int progsig_noteread(progsig_note *rp,int ni) noex {
	int		rs ;
	int		rs1 ;
	int		rc = 0 ;
	if (rp == nullptr) return SR_FAULT ;
	memclear(rp) ;
	if (ni < 0) return SR_INVALID ;
	if ((rs = progsig_init()) >= 0) {
	    progsig	*uip = &progsig_data ;
	    if ((rs = progsig_capbegin(uip,-1)) >= 0) {
	        if ((rs = progsig_mq(uip)) >= 0) {
		    void	*vp{} ;
		    if ((rs = raqhand_acc(&uip->mq,ni,&vp)) >= 0) {
			if (vp) {
		    	    SN	*ep = (SN *) vp ;
			    rp->stime = ep->stime ;
			    rp->type = ep->type ;
			    rp->dlen = ep->dlen ;
			    strwcpy(rp->dbuf,ep->dbuf,SESMSG_NBUFLEN) ;
			    strwcpy(rp->user,ep->user,SESMSG_USERLEN) ;
			    rc = +1 ;
			} /* end if (non-null) */
		    } /* end if (raqhand_acc) */
		} /* end if (progsig_mq) */
	        rs1 = progsig_capend(uip) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (capture) */
	} /* end if (progsig_init) */
	return (rs >= 0) ? rc : rs ;
}
/* end subroutine (progsig_noteread) */

int progsig_notedel(int ni) noex {
	int		rs ;
	int		rs1 ;
	int		rc = 0 ;
	if (ni < 0) return SR_INVALID ;
	if ((rs = progsig_init()) >= 0) {
	    progsig	*uip = &progsig_data ;
	    if ((rs = progsig_capbegin(uip,-1)) >= 0) {
	        if ((rs = progsig_mq(uip)) >= 0) {
		    rs = raqhand_del(&uip->mq,ni) ;
		    rc = rs ;
		} /* end if (progsig_mq) */
	        rs1 = progsig_capend(uip) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (capture) */
	} /* end if (progsig_init) */
	return (rs >= 0) ? rc : rs ;
}
/* end subroutine (progsig_notedel) */


/* local subroutines */

local int progsig_mainstuff(progsig *uip) noex {
	int		rs = SR_OK ;
	if (uip == nullptr) return SR_FAULT ;
	return rs ;
}
/* end subroutine (progsig_mainstuff) */

local int progsig_mq(progsig *uip) noex {
	int		rs = SR_OK ;
	if (! uip->f_mq) {
	    rs = progsig_begin(uip) ;
	}
	return rs ;
}
/* end subroutine (progsig_mq) */

local int progsig_begin(progsig *uip) noex {
	int		rs = SR_OK ;
	if (! uip->f_mq) {
	    cint	n = PROGSIG_NENTS ;
	    if ((rs = raqhand_start(&uip->mq,n,0)) >= 0) {
	        uip->f_mq = true ;
	    }
	}
	return rs ;
}
/* end subroutine (progsig_begin) */

local int progsig_end(progsig *uip) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (uip->f_mq) {
	    {
	        rs1 = progsig_entfins(uip) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = raqhand_finish(&uip->mq) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    uip->f_mq = false ;
	} /* end if */
	return rs ;
}
/* end subroutine (progsig_end) */

local int progsig_entfins(progsig *uip) noex {
	raqhand		*qlp = &uip->mq ;
	int		rs = SR_OK ;
	int		rs1 ;
	void		*vp{} ;
	for (int i = 0 ; raqhand_get(qlp,i,&vp) >= 0 ; i += 1) {
	    if (vp) {
		SN	*ep = (SN *) vp ;
		{
		    rs1 = snote_finish(ep) ;
		    if (rs >= 0) rs = rs1 ;
		}
		{
		    rs1 = lm_free(ep) ;
		    if (rs >= 0) rs = rs1 ;
		}
	    } /* end if (non-null) */
	} /* end for */
	return rs ;
}
/* end subroutine (progsig_entfins) */

local int progsig_runbegin(progsig *uip) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	int		f = false ;
	if (! uip->f_running) {
	    if ((rs = progsig_capbegin(uip,-1)) >= 0) {
		if (! uip->f_running) {
		    if ((rs = progsig_reqopen(uip)) >= 0) {
		        rs = progsig_runner(uip) ;
		        f = rs ;
		    } /* end if (progsig_reqopen) */
		} /* end if (not running) */
		rs1 = progsig_capend(uip) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (capture) */
	} /* end if (not-running) */
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (progsig_runbegin) */

local int progsig_runner(progsig *uip) noex {
	int		rs ;
	int		rs1 ;
	int		f = false ;
	if (pta ta ; (rs = pta_create(&ta)) >= 0) {
	    cint	scope = PROGSIG_SCOPE ;
	    if ((rs = pta_setscope(&ta,scope)) >= 0) {
		pthread_t	tid ;
		tworker_f	wt = (tworker_f) progsig_worker ;
		if ((rs = uptcreate(&tid,&ta,wt,uip)) >= 0) {
		    uip->f_running = true ;
		    uip->tid = tid ;
		    f = true ;
		} /* end if (pthread-create) */
	    } /* end if (pta-setscope) */
	    rs1 = pta_destroy(&ta) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (pta) */

	return (rs >= 0) ? f : rs ;
}
/* end subroutine (progsig_runner) */

local int progsig_runend(progsig *uip) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (uip->f_running) {
	    cint	cmd = sesmsgtype_exit ;
	    if ((rs = progsig_cmdsend(uip,cmd)) >= 0) {
	 	pthread_t	tid = uip->tid ;
		int		trs ;
		if ((rs = uptjoin(tid,&trs)) >= 0) {
		    uip->f_running = false ;
		    rs = trs ;
		}
	        rs1 = progsig_reqclose(uip) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (progsig_cmdsend) */
	} /* end if (running) */
	return rs ;
}
/* end subroutine (progsig_runend) */

/* it always takes a good bit of code to make this part look easy! */
local int progsig_worker(progsig *uip) noex {
	msgdata		m ;
	int		rs = SR_OK ;
	while ((rs = progsig_reqrecv(uip,&m)) > 0) {
	    switch (rs) {
	    case sesmsgtype_echo:
		rs = progsig_workecho(uip,&m) ;
		break ;
	    case sesmsgtype_gen:
		rs = progsig_workgen(uip,&m) ;
		break ;
	    case sesmsgtype_biff:
		rs = progsig_workbiff(uip,&m) ;
		break ;
		default:
		rs = progsig_workdef(uip,&m) ;
		break ;
	    } /* end switch */
	    if (rs < 0) break ;
	} /* end while (looping on commands) */
	uip->f_exiting = true ;
	return rs ;
}
/* end subroutine (progsig_worker) */

local int progsig_workecho(progsig *uip,msgdata *mip) noex {
	int		rs ;
	if ((rs = msgdata_conpass(mip,false)) >= 0) {
	    rs = progsig_reqsend(uip,mip,0) ;
	} /* end if (msgdata_conpass) */
	return rs ;
}
/* end subroutine (progsig_workecho) */

local int progsig_workgen(progsig *uip,msgdata *mip) noex {
	int		rs ;
	int		rs1 ;
	int		rv = 0 ; /* return-value */
	if ((rs = progsig_capbegin(uip,-1)) >= 0) {
	    if ((rs = progsig_mq(uip)) >= 0) {
		SESMSG_GEN	m2 ;
		if ((rs = sesmsger_gen(&m2,1,mip->mbuf,mip->mlen)) >= 0) {
		    rs = progsig_workgener(uip,&m2) ;
		    rv = rs ;
		} /* end if (sesmsger_gen) */
	    } /* end if (progsig_mq) */
	    rs1 = progsig_capend(uip) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (capture) */
	return (rs >= 0) ? rv : rs ;
}
/* end subroutine (progsig_workgen) */

local int progsig_workgener(progsig *uip,SESMSG_GEN *mp) noex {
	cint		esz = szof(SN) ;
	int		rs ;
	int		rv = 0 ; /* return-value */
	if (SN *ep ; (rs = lm_mall(esz,&ep)) >= 0) {
	    time_t	st = mp->stime ;
	    cint	mt = mp->msgtype ;
	    cint	nlen = lenstr(mp->nbuf) ;
	    cchar	*nbuf = mp->nbuf ;
	    cchar	*un = mp->user ;
	    if ((rs = snote_start(ep,mt,st,un,nbuf,nlen)) >= 0) {
		rs = progsig_msgenter(uip,ep) ;
		rv = rs ;
	        if (rs < 0) {
		    snote_finish(ep) ;
		}
	    } /* end if (snote_start) */
	    if (rs < 0) {
		lm_free(ep) ;
	    }
	} /* end if (m-a) */
	return (rs >= 0) ? rv : rs ;
}
/* end subroutine (progsig_workgener) */

local int progsig_workbiff(progsig *uip,msgdata *mip) noex {
	int		rs ;
	int		rs1 ;
	int		rv = 0 ; /* return-value */
	if ((rs = progsig_capbegin(uip,-1)) >= 0) {
	    if ((rs = progsig_mq(uip)) >= 0) {
		SESMSG_BIFF	m3 ;
		if ((rs = sesmsger_biff(&m3,1,mip->mbuf,mip->mlen)) >= 0) {
		    rs = progsig_workbiffer(uip,&m3) ;
		    rv = rs ;
		} /* end if (sesmsger_biff) */
	    } /* end if (progsig_mq) */
	    rs1 = progsig_capend(uip) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (capture) */
	return (rs >= 0) ? rv : rs ;
}
/* end subroutine (progsig_workbiff) */

local int progsig_workbiffer(progsig *uip,SESMSG_BIFF *mp) noex {
	cint		esz = szof(SN) ;
	int		rs ;
	int		rv = 0 ; /* return-value */
	if (void *vp ; (rs = lm_mall(esz,&vp)) >= 0) {
	    SN		*ep = resumelife<SN>(vp) ;
	    time_t	st = mp->stime ;
	    cint	mt = mp->msgtype ;
	    cint	nlen = lenstr(mp->nbuf) ;
	    cchar	*un = mp->user ;
	    cchar	*nbuf = mp->nbuf ;
	    if ((rs = snote_start(ep,mt,st,un,nbuf,nlen)) >= 0) {
		rs = progsig_msgenter(uip,ep) ;
		rv = rs ;
	        if (rs < 0) {
		    snote_finish(ep) ;
		}
	    } /* end if (snote_start) */
	    if (rs < 0) {
		lm_free(ep) ;
	    }
	} /* end if (m-a) */
	return (rs >= 0) ? rv : rs ;
}
/* end subroutine (progsig_workbiffer) */

local int progsig_workdef(progsig *uip,msgdata *mip) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (mip) {
	    ptm *mxp = &uip->mx ;
	    if ((rs = mxp->lockbegin) >= 0) {
		{
	            uip->cdefs += 1 ;
		}
	        rs1 = mxp->lockend ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (mutex) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (progsig_workdef) */

local int progsig_msgenter(progsig *uip,SN *ep) noex {
	raqhand		*qlp = &uip->mq ;
	cint		ors = SR_OVERFLOW ;
	int		rs ;
	if ((rs = raqhand_ins(qlp,ep)) == ors) {
	    if (void *dum ; (rs = raqhand_rem(qlp,&dum)) >= 0) {
		rs = raqhand_ins(qlp,ep) ;
	    }
	}
	return rs ;
}
/* end subroutine (progsig_msgenter) */

local int progsig_reqopen(progsig *uip) noex {
    	cmode		dm = (W_OK|X_OK) ;
	int		rs ;
	int		rs1 ;
	int		rv = 0 ;
	cchar		*dname = PROGSIG_SESDN ;
	if ((rs = sdir(dname,dm)) >= 0) {
	    if ((rs = getbufsize(bufsize_mp)) >= 0) {
	        cint	sz = ((rs + 1) * 2) ;
	        cint	maxpath = rs ;
	        int	ai = 0 ;
	        if (char *a ; (rs = lm_mall(sz,&a)) >= 0) {
	            const pid_t	sid = getsid(0) ;
	            char	*sbuf = (a + ((maxpath + 1) * ai++)) ;
	            if ((rs = mksdname(sbuf,dname,sid)) >= 0) {
		        if (uip->reqfname == nullptr) {
		            char	*pbuf = (a + ((maxpath + 1) * ai++)) ;
	                    if ((rs = progsig_mkreqfname(uip,pbuf,sbuf)) >= 0) {
			        rs = progsig_reqopener(uip,pbuf) ;
				rv = rs ;
		            } /* end if (progsig_mkreqfname) */
		        } /* end if (reqfname) */
	            } /* end if (mksdname) */
		    rs1 = lm_free(a) ;
		    if (rs >= 0) rs = rs1 ;
		} /* end if (m-a-f) */
	    } /* end if (getbufsize) */
	} /* end if (sdir) */
	return (rs >= 0) ? rv : rs ;
}
/* end subroutine (progsig_reqopen) */

local int progsig_reqopener(progsig *uip,cchar *pbuf) noex {
	cint		lo = 0 ;
	int		rs ;
	cmode		om = 0666 ;
	if ((rs = listenusd(pbuf,om,lo)) >= 0) {
	    cint	fd = rs ;
	    if ((rs = uc_closeonexec(fd,true)) >= 0) {
		sockaddress	*sap = &uip->servaddr ;
		cint	af = AF_UNIX ;
		cchar		*rf = pbuf ;
		if ((rs = sockaddress_start(sap,af,rf,0,0)) >= 0) {
		    uip->servlen = rs ;
		    uip->sfd = fd ;
		}
	    }
	    if (rs < 0) {
		uc_close(fd) ;
	    }
	} /* end if (listenusd) */
	return rs ;
}
/* end subroutine (progsig_reqopener) */

local int progsig_reqclose(progsig *uip) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (uip->sfd >= 0) {
	    rs1 = u_close(uip->sfd) ;
	    if (rs >= 0) rs = rs1 ;
	    uip->sfd = -1 ;
	    {
		sockaddress	*sap = &uip->servaddr ;
	        rs1 = sockaddress_finish(sap) ;
		if (rs >= 0) rs = rs1 ;
	    }
	    if (uip->reqfname != nullptr) {
		if (uip->reqfname[0] != '\0') {
		    uc_unlink(uip->reqfname) ;
		}
		void *vp = voidp(uip->reqfname) ;
		rs1 = lm_free(vp) ;
		if (rs >= 0) rs = rs1 ;
		uip->reqfname = nullptr ;
	    } /* end if (reqfname) */
	} /* end if (server-open) */
	return rs ;
}
/* end subroutine (progsig_reqclose) */

local int progsig_reqsend(progsig *uip,msgdata *mip,int dlen) noex {
	cint		fd = uip->sfd ;
	return msgdata_send(mip,fd,dlen,0) ;
}
/* end subroutine (progsig_reqsend) */

local int progsig_reqrecv(progsig *uip,msgdata *mip) noex {
	POLLFD		fds[1] = {} ;
	cint		fd = uip->sfd ;
	cint		mto = (5 * POLL_INTMULT) ;
	cint		nfds = 1 ;
	int		rs ;
	int		rc = 0 ;
	fds[0].fd = fd ;
	fds[0].events = (POLLIN | POLLPRI | POLLERR) ;
	fds[0].revents = 0 ;
	while ((rs = u_poll(fds,nfds,mto)) >= 0) {
	    bool	f = false ;
	    if (rs > 0) {
		cint	re = fds[0].revents ;
		if (re & (POLLIN|POLLPRI)) {
		    if ((rs = msgdata_recv(mip,fd)) >= 0) {
			f = true ;
	    	        if (rs > 0) {
	        	    rc = MKCHAR(mip->mbuf[0]) ;
	    	        } else {
	        	    rc = sesmsgtype_invalid ;
			}
	            } /* end if (msgdata_recv) */
		} else if (re & POLLERR) {
		    rs = SR_IO ;
		}
	    } else if (rs == SR_INTR) {
		rs = SR_OK ;
	    }
	    if (f) break ;
	    if (rs >= 0) {
		rs = progsig_poll(uip) ;
	    }
	    if (rs < 0) break ;
	} /* end while (polling) */
	return (rs >= 0) ? rc : rs ;
}
/* end subroutine (progsig_reqrecv) */

local int progsig_poll(progsig *uip) noex {
	int		rs = SR_FAULT ;
	if (uip) {
	    rs = SR_OK ;
	}
	return rs ;
}
/* end subroutine (progsig_poll) */

local int progsig_cmdsend(progsig *uip,int cmd) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	int		f = false ;
	if (uip->f_running && uip->reqfname) {
	    f = true ;
	    switch (cmd) {
	    case sesmsgtype_exit:
		{
	    	    if (msgdata m ; (rs = msgdata_init(&m,0)) >= 0) {
		        SESMSG_EXIT	m0{} ;
			cint	mlen = MSGBUFLEN ;
			cint	sal = uip->servlen ;
			cvoid	*sap = &uip->servaddr ;
			msgdata_setaddr(&m,sap,sal) ;
		        if ((rs = sesmsger_exit(&m0,0,m.mbuf,mlen)) >= 0) {
			    m.mlen = rs ;
	    	            rs = progsig_reqsend(uip,&m,0) ;
			} /* end if (sesmsger_exit) */
			rs1 = msgdata_fini(&m) ;
			if (rs >= 0) rs = rs1 ;
		    } /* end if (msgdata) */
		} /* end block */
		break ;
	    default:
		rs = SR_INVALID ;
		break ;
	    } /* end switch */
	} /* end if (running) */
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (progsig_cmdsend) */

static void progsig_atforkbefore() noex {
	progsig		*uip = &progsig_data ;
	{
	    ptm *mxp = &uip->mx ;
	    mxp->lockbegin() ;
	}
}
/* end subroutine (progsig_atforkbefore) */

static void progsig_atforkafter() noex {
	progsig		*uip = &progsig_data ;
	{
	    ptm *mxp = &uip->mx ;
	    mxp->lockend() ;
	}
}
/* end subroutine (progsig_atforkafter) */

static void progsig_exit() noex {
	(void) progsig_fini() ;
}
/* end subroutine (progsig_atforkafter) */

static void progsig_sighand(int sn,siginfo_t *sip,void *vcp) noex {
	progsig		*kip = &progsig_data ;
	(void) sip ;
	(void) vcp ;
	switch (sn) {
	case SIGQUIT:
	    kip->f_sigquit = true ;
	    break ;
	case SIGTERM:
	    kip->f_sigterm = true ;
	    break ;
	case SIGINT:
	    kip->f_sigintr = true ;
	    break ;
	case SIGWINCH:
	    kip->f_sigwich = true ;
	    break ;
	case SIGTSTP:
	    kip->f_sigsusp = true ;
	    break ;
	case SIGCHLD:
	    kip->f_sigchild = true ;
	    break ;
	} /* end switch */
}
/* end subroutine (progsig_sighand) */

local int progsig_mkreqfname(progsig *uip,char *sbuf,cchar *dname) noex {
	const uint	uv = uint(uip->pid) ;
	cint		dlen = DIGBUFLEN ;
	int		rs ;
	char		dbuf[DIGBUFLEN+1] = { 'p' } ;
	if ((rs = ctdecui((dbuf+1),(dlen-1),uv)) >= 0) {
	    if ((rs = mkpath2(sbuf,dname,dbuf)) >= 0) {
		if (uip->reqfname == nullptr) {
		    cchar	*cp ;
		    if ((rs = libmalstrw(sbuf,rs,&cp)) >= 0) {
			uip->reqfname = cp ;
		    }
		}
	    } /* end if (mkpath) */
	} /* end if (ctdecui) */
	return rs ;
}
/* end subroutine (progsig_mkreqfname) */

local int progsig_capbegin(progsig *uip,int to) noex {
	ptm		*mxp = &uip->mx ;
	int		rs ;
	int		rs1 ;
	if ((rs = mxp->lockbegin(to)) >= 0) {
	    {
	        ptc *cnp = &uip->cn ;
	        uip->waiters += 1 ;
	        while ((rs >= 0) && uip->f_capture) { /* busy */
	            rs = cnp->wait(mxp,to) ;
	        } /* end while */
	        if (rs >= 0) {
	            uip->f_capture = true ;
	        }
	        uip->waiters -= 1 ;
	    } /* end block */
	    rs1 = mxp->lockend ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (ptm) */
	return rs ;
}
/* end subroutine (progsig_capbegin) */

local int progsig_capend(progsig *uip) noex {
	ptm		*mxp = &uip->mx ;
	int		rs ;
	int		rs1 ;
	if ((rs = mxp->lockbegin) >= 0) {
	    {
	        ptc *cnp = &uip->cn ;
	        uip->f_capture = false ;
	        if (uip->waiters > 0) {
	            rs = cnp->signal ;
	        }
	    } /* end block */
	    rs1 = mxp->lockend ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (ptm) */
	return rs ;
}
/* end subroutine (progsig_capend) */

local int progsig_sigbegin(progsig *kip) noex {
	int		rs ;
	auto		sh = progsig_sighand ;
	kip->f_sigterm = 0 ;
	kip->f_sigintr = 0 ;
	rs = sighand_start(&kip->sm,sigblocks,sigigns,sigints,sh) ;
	return rs ;
}
/* end subroutine (progsig_sigbegin) */

local int progsig_sigend(progsig *kip) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (kip) {
	    rs = SR_OK ;
	    {
	        rs1 = sighand_finish(&kip->sm) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (progsig_sigend) */

local int snote_start(SN *ep,int mt,time_t st,cc *un,cc *mdp,int mdl) noex {
	int		rs = SR_FAULT ;
	if (un && mdp) {
	    int		sz = 0 ;
	    ep->stime = st ;
	    ep->type = mt ;
	    if (mdl < 0) mdl = lenstr(mdp) ;
	    sz += (mdl + 1) ;
	    sz += (lenstr(un) + 1) ;
	    if (char *bp ; (rs = lm_mall(sz,&bp)) >= 0) {
	        ep->a = bp ;
	        ep->user = bp ;
	        bp = (strwcpy(bp,un,-1) + 1) ;
	        ep->dbuf = bp ;
	        bp = (strwcpy(bp,mdp,mdl) + 1) ;
	        ep->dlen = mdl ;
	    } /* end if (m-a) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (snote_start) */

local int snote_finish(SN *ep) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (ep->a != nullptr) {
	    rs1 = lm_free(ep->a) ;
	    if (rs >= 0) rs = rs1 ;
	    ep->a = nullptr ;
	    ep->user = nullptr ;
	    ep->dbuf = nullptr ;
	    ep->dlen = 0 ;
	}
	ep->stime = 0 ;
	ep->type = 0 ;
	return rs ;
}
/* end subroutine (snote_finish) */

local int libmalstrw(cchar *sp,int sl,cchar **rpp) noex {
	int		rs = SR_FAULT ;
	if (rpp) {
	    if (sl < 0) sl = lenstr(sp) ;
	    if (char *bp ; (rs = lm_mall((sl+1),&bp)) >= 0) {
	        *rpp = bp ;
	        strwcpy(bp,sp,sl) ;
	    } /* end if (m-a) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (libmalstrw) */

local int sdir(cchar *dname,int am) noex {
	cint		nrs = SR_NOTFOUND ;
	int		rs ;
	int		f = false ;
	cmode		dm = 0777 ;
	if (ustat sb ; (rs = uc_stat(dname,&sb)) == nrs) {
	    f = true ;
	    rs = mksdir(dname,dm) ;
	} else {
	    rs = perm(dname,-1,-1,nullptr,am) ;
	} /* end if (stat) */
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (sdir) */

local int mksdir(cchar *dname,mode_t dm) noex {
	int		rs ;
	if ((rs = mkdirs(dname,dm)) >= 0) {
	    rs = uc_minmod(dname,dm) ;
	}
	return rs ;
}
/* end if (mksdir) */

local int mksdname(char *rbuf,cchar *dname,pid_t sid) noex {
	const uint	uv = uint(sid) ;
	cint		dlen = DIGBUFLEN ;
	int		rs ;
	char		dbuf[DIGBUFLEN+1] = { 's' } ;
	if ((rs = ctdecui((dbuf+1),(dlen-1),uv)) >= 0) {
	    if ((rs = mkpath2(rbuf,dname,dbuf)) >= 0) {
		cmode	dm = 0777 ;
		if ((rs = mkdirs(rbuf,dm)) >= 0) {
		    rs = uc_minmod(rbuf,dm) ;
		}
	    } /* end if (mkpath) */
	} /* end if (ctdecui) */
	return rs ;
}
/* end subroutine (mksdname) */


