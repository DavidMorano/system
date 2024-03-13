/* progsig SUPPORT */
/* lang=C++20 */

/* program signal handling */
/* version %I% last-modified %G% */

#define	CF_SIGTSTP	1		/* Solaris� bug in |sigpending(2)| */

/* revision history:

	= 2000-05-14, David A�D� Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright � 2000 David A�D� Morano.  All rights reserved. */

/*******************************************************************************

	Manage process signals.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<poll.h>
#include	<csignal>
#include	<cstring>
#include	<usystem.h>
#include	<upt.h>
#include	<ptm.h>
#include	<ptc.h>
#include	<sighand.h>
#include	<sockaddress.h>
#include	<raqhand.h>
#include	<exitcodes.h>
#include	<localmisc.h>

#include	"sesmsg.h"
#include	"msgdata.h"
#include	"progsig.h"


/* local defines */

#ifndef	POLLINTMULT
#define	POLLINTMULT	1000		/* poll-time multiplier */
#endif

#ifndef	MSGHDR
#define	MSGHDR		srtuct msghdr
#endif

#ifndef	MSGBUFLEN
#define	MSGBUFLEN	2048
#endif

#ifndef	CMSGBUFLEN
#define	CMSGBUFLEN	256
#endif

#ifndef	DIGBUFLEN
#define	DIGBUFLEN	40		/* can hold int128_t in decimal */
#endif

#define	NDF		"progsig.deb"

#ifndef	SYMNAMELEN
#define	SYMNAMELEN	60
#endif

#define	TO_LOCKENV	10

#define	PROGSIG		struct progsig
#define	PROGSIG_FL	struct progsig_flags
#define	PROGSIG_SCOPE	PTHREAD_SCOPE_PROCESS
#define	PROGSIG_SESDN	"/var/tmp/sessions"

#define	STORENOTE	struct storenote


/* imported namespaces */


/* local typedefs */


/* external subroutines */

extern "C" {
    int		progsig_init(void) noex ;
    int		progsig_fini(void) noex ;
    int		progsig_initenviron(void *) noex ;
    int		progsig_callcmd(cchar *,int,cchar **,cchar **,void *) noex ;
    int 	progsig_callfunc(subcmd_t,int,cchar **,cchar **,void *) noex ;
}
extern "C" {
    int		progsig_initmemalloc(int) noex ;
}

/* external variables */

extern char	**environ ;


/* local structures */

extern "C" {
    typedef int (*subcmd_t)(int,cchar **,cchar **,void *) noex ;
}

#ifndef	TYPEDEF_TWORKER
#define	TYPEDEF_TWORKER
extern "C" {
    typedef	int (*tworker)(void *) noex ;
}
#endif

extern "C" {
    typedef	int (*cmdsub_t)(int,cchar **,cchar **,void *) noex ;
    typedef	int (*func_caller)(int,cchar **,void *) noex ;
}

struct progsig_flags {
	uint		dummy:1 ;
} ;

struct progsig {
	ptm		m ;		/* mutex data */
	ptm		menv ;		/* mutex environment */
	ptc		c ;		/* condition variable */
	SIGHAND		sm ;
	SOCKADDRESS	servaddr ;	/* server address */
	RAQHAND		mq ;		/* message queue */
	PROGSIG_FL	f ;
	cchar		*reqfname ;
	cchar		**envv ;
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
} ;

struct storenote {
	time_t		stime ;
	cchar		*dbuf ;
	cchar		*user ;
	char		*a ;
	int		type ;
	int		dlen ;
} ;

enum cmds {
	cmd_session,
	cmd_overlast
} ;


/* forward references */

extern "C" {
    static void	progsig_atforkbefore() noex ;
    static void	progsig_atforkafter() noex ;
    static void	progsig_sighand(int,siginfo_t *,void *) noex ;
}

static int	progsig_mainstuff(PROGSIG *) noex ;

static int	progsig_begin(PROGSIG *) noex ;
static int	progsig_end(PROGSIG *) noex ;
static int	progsig_runbegin(PROGSIG *) noex ;
static int	progsig_runner(PROGSIG *) noex ;
static int	progsig_runend(PROGSIG *) noex ;
static int	progsig_entfins(PROGSIG *) noex ;
static int	progsig_mq(PROGSIG *) noex ;
static int	progsig_mkreqfname(PROGSIG *,char *,cchar *) noex ;
static int	progsig_worker(PROGSIG *) noex ;
static int	progsig_workecho(PROGSIG *,MSGDATA *) noex ;
static int	progsig_workbiff(PROGSIG *,MSGDATA *) noex ;
static int	progsig_workbiffer(PROGSIG *,SESMSG_BIFF *) noex ;
static int	progsig_workgen(PROGSIG *,MSGDATA *) noex ;
static int	progsig_workgener(PROGSIG *,SESMSG_GEN *) noex ;
static int	progsig_workdef(PROGSIG *,MSGDATA *) noex ;
static int	progsig_msgenter(PROGSIG *,STORENOTE *) noex ;
static int	progsig_reqopen(PROGSIG *) noex ;
static int	progsig_reqopener(PROGSIG *,cchar *) noex ;
static int	progsig_reqsend(PROGSIG *,MSGDATA *,int) noex ;
static int	progsig_reqrecv(PROGSIG *,MSGDATA *) noex ;
static int	progsig_reqclose(PROGSIG *) noex ;
static int	progsig_poll(PROGSIG *) noex ;
static int	progsig_cmdsend(PROGSIG *,int) noex ;
static int	progsig_capbegin(PROGSIG *,int) noex ;
static int	progsig_capend(PROGSIG *) noex ;
static int	progsig_sigbegin(PROGSIG *) noex ;
static int	progsig_sigend(PROGSIG *) noex ;

static int storenote_start(STORENOTE *,int,time_t,cchar *,cchar *,int) noex ;
static int storenote_finish(STORENOTE *) noex ;

static int	mallocstrw(cchar *,int,cchar **) noex ;
static int	sdir(cchar *,int) noex ;
static int	mksdir(cchar *,mode_t) noex ;
static int	mksdname(char *,cchar *,pid_t) noex ;


/* local variables */

static PROGSIG		progsig_data ;

static constexpr int	sigblocks[] = {
	SIGUSR1,
	SIGUSR2,
	SIGHUP,
	SIGCHLD,
	0
} ;

static constexpr int	sigigns[] = {
	SIGPIPE,
	SIGPOLL,
#if	defined(SIGXFSZ)
	SIGXFSZ,
#endif
	0
} ;

static constexpr int	sigints[] = {
	SIGQUIT,
	SIGTERM,
	SIGINT,
	SIGWINCH,
	SIGCHLD,
#if	CF_SIGTSTP /* causes a hang in |sigpending(2)| in Solaris� UNIX� */
	SIGTSTP,
#endif
	0
} ;


/* exported subroutines */

int progsig_init(void) noex {
	PROGSIG		*uip = &progsig_data ;
	int		rs = SR_OK ;
	if (! uip->f_init) {
	    uip->f_init = true ;
	    if ((rs = ptm_create(&uip->m,nullptr)) >= 0) {
	        if ((rs = ptc_create(&uip->c,nullptr)) >= 0) {
	            void	(*b)() = progsig_atforkbefore ;
	            void	(*a)() = progsig_atforkafter ;
	            if ((rs = uc_atfork(b,a,a)) >= 0) {
	                if ((rs = uc_atexit(progsig_fini)) >= 0) {
			    uip->pid = getpid() ;
			    uip->sfd = -1 ;
		            rs = 1 ;
	    	            uip->f_initdone = true ;
		        }
		        if (rs < 0) {
		            uc_atforkexpunge(b,a,a) ;
			}
	            } /* end if (uc_atfork) */
	            if (rs < 0) {
	                ptc_destroy(&uip->c) ;
		    }
	        } /* end if (ptc_create) */
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

void progsig_fini(void) noex {
	struct progsig	*uip = &progsig_data ;
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
	        rs1 = uc_atforkexpunge(b,a,a) ;
		if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = ptc_destroy(&uip->c) ;
		if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = ptm_destroy(&uip->m) ;
		if (rs >= 0) rs = rs1 ;
	    }
	    uip->f_init = false ;
	    uip->f_initdone = false ;
	} /* end if (atexit registered) */
}
/* end subroutine (progsig_fini) */


int progsig_mainbegin(cchar **envv)
{
	int		rs ;

	if ((rs = progsig_init()) >= 0) {
	    PROGSIG	*uip = &progsig_data ;
	    uip->envv = envv ;
	    if ((rs = progsig_sigbegin(uip)) >= 0) {
		rs = progsig_mainstuff(uip) ;
		if (rs < 0)
	    	    progsig_sigend(uip) ;
	    } /* end if (progsig_sigbegin) */
	} /* end if (progsig_init) */

	return rs ;
}
/* end subroutine (progsig_mainbegin) */


int progsig_mainend(void)
{
	PROGSIG		*uip = &progsig_data ;
	int		rs = SR_OK ;
	int		rs1 ;

	if (uip->f_running) {
	    rs1 = progsig_runend(uip) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (running) */

	rs1 = progsig_sigend(uip) ;
	if (rs >= 0) rs = rs1 ;

	uip->envv = nullptr ;
	return rs ;
}
/* end subroutine (progsig_mainend) */


int progsig_adm(int cmd)
{
	PROGSIG		*kip = &progsig_data ;
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


int progsig_runmode(void)
{
	PROGSIG		*kip = &progsig_data ;
	return kip->runmode ;
}
/* end subroutine (progsig_runmode) */


int progsig_serial(void)
{
	PROGSIG		*kip = &progsig_data ;
	int		s = kip->serial ;
	return s ;
}
/* end subroutine (progsig_serial) */


int progsig_sigquit(void)
{
	PROGSIG		*kip = &progsig_data ;
	int		rs = SR_OK ;
	if (kip->f_sigquit) {
	    kip->f_sigquit = 0 ;
	    rs = SR_QUIT ;
	}
	return rs ;
}
/* end subroutine (progsig_sigquit) */


int progsig_sigterm(void)
{
	PROGSIG		*kip = &progsig_data ;
	int		rs = SR_OK ;
	if (kip->f_sigterm) {
	    kip->f_sigterm = 0 ;
	    rs = SR_EXIT ;
	}
	return rs ;
}
/* end subroutine (progsig_sigterm) */


int progsig_sigintr(void)
{
	PROGSIG		*kip = &progsig_data ;
	int		rs = SR_OK ;
	if (kip->f_sigintr) {
	    kip->f_sigintr = 0 ;
	    rs = SR_INTR ;
	}
	return rs ;
}
/* end subroutine (progsig_sigintr) */


int progsig_issig(int sn)
{
	PROGSIG		*kip = &progsig_data ;
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


int progsig_noteread(PROGSIG_NOTE *rp,int ni)
{
	int		rs ;
	int		rs1 ;
	int		rc = 0 ;
	if (rp == nullptr) return SR_FAULT ;
	memset(rp,0,sizeof(PROGSIG_NOTE)) ;
	if (ni < 0) return SR_INVALID ;
	if ((rs = progsig_init()) >= 0) {
	    PROGSIG	*uip = &progsig_data ;
	    if ((rs = progsig_capbegin(uip,-1)) >= 0) {
	        if ((rs = progsig_mq(uip)) >= 0) {
		    STORENOTE	*ep ;
		    if ((rs = raqhand_acc(&uip->mq,ni,&ep)) >= 0) {
			if (ep != nullptr) {
			    rp->stime = ep->stime ;
			    rp->type = ep->type ;
			    rp->dlen = ep->dlen ;
			    strwcpy(rp->dbuf,ep->dbuf,SESMSG_NBUFLEN) ;
			    strwcpy(rp->user,ep->user,SESMSG_USERLEN) ;
			    rc = 1 ;
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


int progsig_notedel(int ni)
{
	int		rs ;
	int		rs1 ;
	int		rc = 0 ;
	if (ni < 0) return SR_INVALID ;
	if ((rs = progsig_init()) >= 0) {
	    PROGSIG	*uip = &progsig_data ;
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


static int progsig_mainstuff(PROGSIG *uip)
{
	int		rs = SR_OK ;
	if (uip == nullptr) return SR_FAULT ;
	return rs ;
}
/* end subroutine (progsig_mainstuff) */


static int progsig_mq(PROGSIG *uip)
{
	int		rs = SR_OK ;
	if (! uip->f_mq) {
	    rs = progsig_begin(uip) ;
	}
	return rs ;
}
/* end subroutine (progsig_mq) */


static int progsig_begin(PROGSIG *uip)
{
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


static int progsig_end(PROGSIG *uip)
{
	int		rs = SR_OK ;
	int		rs1 ;
	if (uip->f_mq) {
	    rs1 = progsig_entfins(uip) ;
	    if (rs >= 0) rs = rs1 ;
	    uip->f_mq = false ;
	    rs1 = raqhand_finish(&uip->mq) ;
	    if (rs >= 0) rs = rs1 ;
	}
	return rs ;
}
/* end subroutine (progsig_end) */


static int progsig_entfins(PROGSIG *uip)
{
	RAQHAND		*qlp = &uip->mq ;
	STORENOTE	*ep ;
	int		rs = SR_OK ;
	int		rs1 ;
	int		i ;
	for (i = 0 ; raqhand_get(qlp,i,&ep) >= 0 ; i += 1) {
	    if (ep != nullptr) {
		rs1 = storenote_finish(ep) ;
		if (rs >= 0) rs = rs1 ;
		rs1 = uc_libfree(ep) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (non-null) */
	} /* end for */
	return rs ;
}
/* end subroutine (progsig_entfins) */


static int progsig_runbegin(PROGSIG *uip)
{
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


static int progsig_runner(PROGSIG *uip)
{
	PTA		ta ;
	int		rs ;
	int		rs1 ;
	int		f = false ;

	if ((rs = pta_create(&ta)) >= 0) {
	    cint	scope = PROGSIG_SCOPE ;
	    if ((rs = pta_setscope(&ta,scope)) >= 0) {
		pthread_t	tid ;
		tworker		wt = (tworker) progsig_worker ;
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


static int progsig_runend(PROGSIG *uip)
{
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
static int progsig_worker(PROGSIG *uip)
{
	MSGDATA		m ;
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


static int progsig_workecho(PROGSIG *uip,MSGDATA *mip)
{
	int		rs ;
	if ((rs = msgdata_conpass(mip,false)) >= 0) {
	    rs = progsig_reqsend(uip,mip,0) ;
	} /* end if (msgdata_conpass) */
	return rs ;
}
/* end subroutine (progsig_workecho) */


static int progsig_workgen(PROGSIG *uip,MSGDATA *mip)
{
	int		rs ;
	int		rs1 ;
	if ((rs = progsig_capbegin(uip,-1)) >= 0) {
	    if ((rs = progsig_mq(uip)) >= 0) {
		SESMSG_GEN	m2 ;
		if ((rs = sesmsg_gen(&m2,1,mip->mbuf,mip->mlen)) >= 0) {
		    rs = progsig_workgener(uip,&m2) ;
		} /* end if (sesmsg_gen) */
	    } /* end if (progsig_mq) */
	    rs1 = progsig_capend(uip) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (capture) */
	return rs ;
}
/* end subroutine (progsig_workgen) */


static int progsig_workgener(PROGSIG *uip,SESMSG_GEN *mp)
{
	STORENOTE	*ep ;
	cint	esize = sizeof(STORENOTE) ;
	int		rs ;
	if ((rs = uc_libmalloc(esize,&ep)) >= 0) {
	    time_t	st = mp->stime ;
	    cint	mt = mp->msgtype ;
	    cint	nlen = strlen(mp->nbuf) ;
	    cchar	*nbuf = mp->nbuf ;
	    cchar	*un = mp->user ;
	    if ((rs = storenote_start(ep,mt,st,un,nbuf,nlen)) >= 0) {
		rs = progsig_msgenter(uip,ep) ;
	        if (rs < 0) {
		    storenote_finish(ep) ;
		}
	    } /* end if (storenote_start) */
	    if (rs < 0) {
		uc_libfree(ep) ;
	    }
	} /* end if (m-a) */
	return rs ;
}
/* end subroutine (progsig_workgener) */


static int progsig_workbiff(PROGSIG *uip,MSGDATA *mip)
{
	int		rs ;
	int		rs1 ;
	if ((rs = progsig_capbegin(uip,-1)) >= 0) {
	    if ((rs = progsig_mq(uip)) >= 0) {
		SESMSG_BIFF	m3 ;
		if ((rs = sesmsg_biff(&m3,1,mip->mbuf,mip->mlen)) >= 0) {
		    rs = progsig_workbiffer(uip,&m3) ;
		} /* end if (sesmsg_biff) */
	    } /* end if (progsig_mq) */
	    rs1 = progsig_capend(uip) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (capture) */
	return rs ;
}
/* end subroutine (progsig_workbiff) */


static int progsig_workbiffer(PROGSIG *uip,SESMSG_BIFF *mp)
{
	STORENOTE	*ep ;
	cint	esize = sizeof(STORENOTE) ;
	int		rs ;
	if ((rs = uc_libmalloc(esize,&ep)) >= 0) {
	    time_t	st = mp->stime ;
	    cint	mt = mp->msgtype ;
	    cint	nlen = strlen(mp->nbuf) ;
	    cchar	*un = mp->user ;
	    cchar	*nbuf = mp->nbuf ;
	    if ((rs = storenote_start(ep,mt,st,un,nbuf,nlen)) >= 0) {
		rs = progsig_msgenter(uip,ep) ;
	        if (rs < 0)
		    storenote_finish(ep) ;
	    } /* end if (storenote_start) */
	    if (rs < 0)
		uc_libfree(ep) ;
	} /* end if (m-a) */
	return rs ;
}
/* end subroutine (progsig_workbiffer) */


static int progsig_workdef(PROGSIG *uip,MSGDATA *mip)
{
	int		rs ;
	if (mip == nullptr) return SR_FAULT ;
	if ((rs = ptm_lock(&uip->m)) >= 0) {
	    uip->cdefs += 1 ;
	    ptm_unlock(&uip->m) ;
	} /* end if (mutex) */
	return rs ;
}
/* end subroutine (progsig_workdef) */


static int progsig_msgenter(PROGSIG *uip,STORENOTE *ep)
{
	RAQHAND		*qlp = &uip->mq ;
	cint	ors = SR_OVERFLOW ;
	int		rs ;
	if ((rs = raqhand_ins(qlp,ep)) == ors) {
	    void	*dum ;
	    if ((rs = raqhand_rem(qlp,&dum)) >= 0) {
		rs = raqhand_ins(qlp,ep) ;
	    }
	}
	return rs ;
}
/* end subroutine (progsig_msgenter) */


static int progsig_reqopen(PROGSIG *uip)
{
	int		rs ;
	cchar		*dname = PROGSIG_SESDN ;

	if ((rs = sdir(dname,(W_OK|X_OK))) >= 0) {
	    pid_t	sid = getsid(0) ;
	    char	sbuf[MAXPATHLEN+1] ;
	    if ((rs = mksdname(sbuf,dname,sid)) >= 0) {
		if (uip->reqfname == nullptr) {
		    char	pbuf[MAXPATHLEN+1] ;
	            if ((rs = progsig_mkreqfname(uip,pbuf,sbuf)) >= 0) {
			rs = progsig_reqopener(uip,pbuf) ;
		    } /* end if (progsig_mkreqfname) */
		} /* end if (reqfname) */
	    } /* end if (mksdname) */
	} /* end if (sdir) */

	return rs ;
}
/* end subroutine (progsig_reqopen) */


static int progsig_reqopener(PROGSIG *uip,cchar *pbuf)
{
	cmode	om = 0666 ;
	cint	lo = 0 ;
	int		rs ;
	if ((rs = listenusd(pbuf,om,lo)) >= 0) {
	    int	fd = rs ;
	    if ((rs = uc_closeonexec(fd,true)) >= 0) {
		SOCKADDRESS	*sap = &uip->servaddr ;
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


static int progsig_reqclose(PROGSIG *uip)
{
	int		rs = SR_OK ;
	int		rs1 ;

	if (uip->sfd >= 0) {
	    rs1 = u_close(uip->sfd) ;
	    if (rs >= 0) rs = rs1 ;
	    uip->sfd = -1 ;
	    {
		SOCKADDRESS	*sap = &uip->servaddr ;
	        rs1 = sockaddress_finish(sap) ;
		if (rs >= 0) rs = rs1 ;
	    }
	    if (uip->reqfname != nullptr) {
		if (uip->reqfname[0] != '\0') {
		    uc_unlink(uip->reqfname) ;
		}
		rs1 = uc_free(uip->reqfname) ;
		if (rs >= 0) rs = rs1 ;
		uip->reqfname = nullptr ;
	    } /* end if (reqfname) */
	} /* end if (server-open) */

	return rs ;
}
/* end subroutine (progsig_reqclose) */


static int progsig_reqsend(PROGSIG *uip,MSGDATA *mip,int clen)
{
	cint	fd = uip->sfd ;
	return msgdata_send(mip,fd,clen) ;
}
/* end subroutine (progsig_reqsend) */


static int progsig_reqrecv(PROGSIG *uip,MSGDATA *mip)
{
	struct pollfd	fds[1] ;
	cint	fd = uip->sfd ;
	cint	mto = (5*POLLINTMULT) ;
	cint	nfds = 1 ;
	int		size ;
	int		rs ;
	int		rc = 0 ;

	size = (nfds * sizeof(struct pollfd)) ;
	memset(fds,0,size) ;
	fds[0].fd = fd ;
	fds[0].events = (POLLIN | POLLPRI | POLLERR) ;
	fds[0].revents = 0 ;

	while ((rs = u_poll(fds,nfds,mto)) >= 0) {
	    int	f = false ;
	    if (rs > 0) {
		cint	re = fds[0].revents ;
		if (re & (POLLIN|POLLPRI)) {
		    if ((rs = msgdata_recv(mip,fd)) >= 0) {
			f = true ;
	    	        if (rs > 0) {
	        	    rc = MKCHAR(mip->mbuf[0]) ;
	    	        } else
	        	    rc = sesmsgtype_invalid ;
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


static int progsig_poll(PROGSIG *uip)
{
	int		rs = SR_OK ;

	if (uip == nullptr) return SR_FAULT ;

	return rs ;
}
/* end subroutine (progsig_poll) */


static int progsig_cmdsend(PROGSIG *uip,int cmd)
{
	int		rs = SR_OK ;
	int		f = false ;
	if (uip->f_running && (uip->reqfname != nullptr)) {
	    f = true ;
	    switch (cmd) {
	    case sesmsgtype_exit:
		{
	    	    MSGDATA	m ;
		    if ((rs = msgdata_init(&m,0)) >= 0) {
		        SESMSG_EXIT	m0 ;
			cint	mlen = MSGBUFLEN ;
			cint	sal = uip->servlen ;
			const void	*sap = &uip->servaddr ;
			msgdata_setaddr(&m,sap,sal) ;
			memset(&m0,0,sizeof(SESMSG_EXIT)) ;
		        if ((rs = sesmsg_exit(&m0,0,m.mbuf,mlen)) >= 0) {
			    m.mlen = rs ;
	    	            rs = progsig_reqsend(uip,&m,0) ;
			} /* end if (sesmsg_exit) */
		    } /* end if (init) */
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


static void progsig_atforkbefore()
{
	PROGSIG		*uip = &progsig_data ;
	ptm_lock(&uip->m) ;
}
/* end subroutine (progsig_atforkbefore) */


static void progsig_atforkafter()
{
	PROGSIG		*uip = &progsig_data ;
	ptm_unlock(&uip->m) ;
}
/* end subroutine (progsig_atforkafter) */


/* ARGSUSED */
static void kshlib_sighand(int sn,siginfo_t *sip,void *vcp)
{
	PROGSIG		*kip = &progsig_data ;
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


static int progsig_mkreqfname(PROGSIG *uip,char *sbuf,cchar *dname)
{
	const uint	uv = (uint) uip->pid ;
	cint	dlen = DIGBUFLEN ;
	int		rs ;
	char		dbuf[DIGBUFLEN+1] = { 'p' } ;

	if ((rs = ctdecui((dbuf+1),(dlen-1),uv)) >= 0) {
	    if ((rs = mkpath2(sbuf,dname,dbuf)) >= 0) {
		if (uip->reqfname == nullptr) {
		    cchar	*cp ;
		    if ((rs = mallocstrw(sbuf,rs,&cp)) >= 0) {
			uip->reqfname = cp ;
		    }
		}
	    } /* end if (mkpath) */
	} /* end if (ctdecui) */

	return rs ;
}
/* end subroutine (progsig_mkreqfname) */


static int progsig_capbegin(PROGSIG *uip,int to)
{
	int		rs ;
	int		rs1 ;

	if ((rs = ptm_lockto(&uip->m,to)) >= 0) {
	    uip->waiters += 1 ;

	    while ((rs >= 0) && uip->f_capture) { /* busy */
	        rs = ptc_waiter(&uip->c,&uip->m,to) ;
	    } /* end while */

	    if (rs >= 0) {
	        uip->f_capture = true ;
	    }

	    uip->waiters -= 1 ;
	    rs1 = ptm_unlock(&uip->m) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (ptm) */

	return rs ;
}
/* end subroutine (progsig_capbegin) */


static int progsig_capend(PROGSIG *uip)
{
	int		rs ;
	int		rs1 ;

	if ((rs = ptm_lock(&uip->m)) >= 0) {

	    uip->f_capture = false ;
	    if (uip->waiters > 0) {
	        rs = ptc_signal(&uip->c) ;
	    }

	    rs1 = ptm_unlock(&uip->m) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (ptm) */

	return rs ;
}
/* end subroutine (progsig_capend) */


static int progsig_sigbegin(PROGSIG *kip)
{
	int		rs ;
	void		(*sh)(int) = progsig_sighand ;
	kip->f_sigterm = 0 ;
	kip->f_sigintr = 0 ;
	rs = sighand_start(&kip->sm,sigblocks,sigigns,sigints,sh) ;
	return rs ;
}
/* end subroutine (progsig_sigbegin) */


static int progsig_sigend(PROGSIG *kip)
{
	int		rs = SR_OK ;
	int		rs1 ;
	rs1 = sighand_finish(&kip->sm) ;
	if (rs >= 0) rs = rs1 ;
	return rs ;
}
/* end subroutine (progsig_sigend) */


static int storenote_start(ep,mt,st,un,mdp,mdl)
STORENOTE	*ep ;
int		mt ;
time_t		st ;
cchar		*un ;
cchar		*mdp ;
int		mdl ;
{
	int		rs ;
	int		size = 0 ;
	char		*bp ;
	if (un == nullptr) return SR_FAULT ;
	if (mdp == nullptr) return SR_FAULT ;
	ep->stime = st ;
	ep->type = mt ;
	if (mdl < 0) mdl = strlen(mdp) ;
	size += (mdl+1) ;
	size += (strlen(un)+1) ;
	if ((rs = uc_malloc(size,&bp)) >= 0) {
	    ep->a = bp ;
	    ep->user = bp ;
	    bp = (strwcpy(bp,un,-1)+1) ;
	    ep->dbuf = bp ;
	    bp = (strwcpy(bp,mdp,mdl)+1) ;
	    ep->dlen = mdl ;
	} /* end if (m-a) */
	return rs ;
}
/* end subroutine (storenote_start) */

static int storenote_finish(STORENOTE *ep) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (ep->a != nullptr) {
	    rs1 = uc_libfree(ep->a) ;
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
/* end subroutine (storenote_finish) */

static int mallocstrw(cchar *sp,int sl,cchar **rpp) noex {
	int		rs ;
	char		*bp ;
	if (rpp == nullptr) return SR_FAULT ;
	if (sl < 0) sl = strlen(sp) ;
	if ((rs = uc_libmalloc((sl+1),&bp)) >= 0) {
	    *rpp = bp ;
	    strwcpy(bp,sp,sl) ;
	} /* end if (m-a) */
	return rs ;
}
/* end subroutine (mallocstrw) */

static int sdir(cchar *dname,int am) noex {
	struct ustat	sb ;
	cmode	dm = 0777 ;
	cint	nrs = SR_NOTFOUND ;
	int		rs ;
	int		f = false ;

	if ((rs = uc_stat(dname,&sb)) == nrs) {
	    f = true ;
	    rs = mksdir(dname,dm) ;
	} else {
	    rs = perm(dname,-1,-1,nullptr,am) ;
	} /* end if (stat) */

	return (rs >= 0) ? f : rs ;
}
/* end subroutine (sdir) */

static int mksdir(cchar *dname,mode_t dm) noex {
	int		rs ;
	if ((rs = mkdirs(dname,dm)) >= 0) {
	    rs = uc_minmod(dname,dm) ;
	}
	return rs ;
}
/* end if (mksdir) */

static int mksdname(char *rbuf,cchar *dname,pid_t sid) noex {
	const uint	uv = (uint) sid ;
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


