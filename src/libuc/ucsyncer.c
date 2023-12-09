/* uc_syncer */
/* lang=C20 */

/* UNIX® file-system synchronization */
/* version %I% last-modified %G% */


/* revision history:

	= 2000-05-14, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2000 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	uc_syncer

	Description:
	This subroutine proforms a file-system synchronization
	(possibly asynchronously if specified).

	Synopsis:
	int uc_syncer(int w) noex

	Arguments:
	w		which type:
				0=synchronous
				1=parallel

	Returns:
	<0		error
	>=0		OK

	Notes:
	We need to carefully watch out for what happens after a
	|fork(2)|.  Although data structures survice a |fork(2)|,
	running threads besides the one that that forked do not!
	Therein lie the potential problems.

*******************************************************************************/

#include	<envstandards.h>
#include	<sys/types.h>
#include	<sys/stat.h>
#include	<string.h>
#include	<usystem.h>
#include	<usupport.h>
#include	<sigblocker.h>
#include	<ptm.h>
#include	<ptc.h>
#include	<pta.h>
#include	<upt.h>
#include	<localmisc.h>


/* local defines */

#define	UCSYNCER	struct ucsyncer
#define	UCSYNCER_SCOPE	PTHREAD_SCOPE_SYSTEM

#define	NDF		"ucsyncer.deb"


/* local typedefs */


/* external subroutines */


/* local structures */

#ifndef	TYPEDEF_TWORKER
#define	TYPEDEF_TWORKER	1
typedef	int (*tworker)(void *) ;
#endif

struct ucsyncer {
	ptm		m ;		/* data mutex */
	ptc		c ;		/* condition variable */
	pid_t		pid ;
	pthread_t	tid ;
	volatile int	f_init ;
	volatile int	f_initdone ;
	volatile int	f_running ;
	volatile int	f_cmd ;
	volatile int	f_syncing ;
	volatile int	f_exiting ;
	volatile int	waiters ;
	int		cmd ;
	int		count ;
} ;

enum cmds {
	cmd_exit,
	cmd_sync,
	cmd_overlast
} ;


/* forward references */

int		ucsyncer_init() noex ;
void		ucsyncer_fini() noex ;

static int	ucsyncer_sendsync(UCSYNCER *) noex ;
static int	ucsyncer_run(UCSYNCER *) noex ;
static int	ucsyncer_runcheck(UCSYNCER *) noex ;
static int	ucsyncer_runner(UCSYNCER *) noex ;
static int	ucsyncer_worker(UCSYNCER *) noex ;
static int	ucsyncer_worksync(UCSYNCER *) noex ;
static int	ucsyncer_cmdsend(UCSYNCER *,int) noex ;
static int	ucsyncer_cmdrecv(UCSYNCER *) noex ;
static int	ucsyncer_waitdone(UCSYNCER *) noex ;
static void	ucsyncer_atforkbefore() noex ;
static void	ucsyncer_atforkparent() noex ;
static void	ucsyncer_atforkchild() noex ;


/* local variables */

static UCSYNCER		ucsyncer_data ; /* zero-initialized */


/* exported subroutines */

int ucsyncer_init() noex {
	UCSYNCER	*uip = &ucsyncer_data ;
	int		rs = SR_OK ;
	int		f = FALSE ;
	if (! uip->f_init) {
	    uip->f_init = TRUE ;
	    if ((rs = ptm_create(&uip->m,NULL)) >= 0) {
	        if ((rs = ptc_create(&uip->c,NULL)) >= 0) {
	    	    void	(*b)() = ucsyncer_atforkbefore ;
	    	    void	(*ap)() = ucsyncer_atforkparent ;
	    	    void	(*ac)() = ucsyncer_atforkchild ;
	            if ((rs = uc_atfork(b,ap,ac)) >= 0) {
	                if ((rs = uc_atexit(ucsyncer_fini)) >= 0) {
	    	            uip->f_initdone = TRUE ;
			    uip->pid = getpid() ;
			    f = TRUE ;
		        }
		        if (rs < 0)
		            uc_atforkexpunge(b,ap,ac) ;
	            } /* end if (uc_atfork) */
	            if (rs < 0)
	                ptc_destroy(&uip->c) ;
	        } /* end if (ptc_create) */
		if (rs < 0)
		    ptm_destroy(&uip->m) ;
	    } /* end if (ptm_create) */
	    if (rs < 0)
	        uip->f_init = FALSE ;
	} else {
	    while ((rs >= 0) && uip->f_init && (! uip->f_initdone)) {
		rs = msleep(1) ;
		if (rs == SR_INTR) rs = SR_OK ;
	    }
	    if ((rs >= 0) && (! uip->f_init)) rs = SR_LOCKLOST ;
	}
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (ucsyncer_init) */

void ucsyncer_fini() noex {
	UCSYNCER	*uip = &ucsyncer_data ;
	if (uip->f_initdone) {
	    uip->f_initdone = FALSE ;
	    ucsyncer_waitdone(uip) ;
	    {
	        void	(*b)() = ucsyncer_atforkbefore ;
	        void	(*ap)() = ucsyncer_atforkparent ;
	        void	(*ac)() = ucsyncer_atforkchild ;
	        uc_atforkexpunge(b,ap,ac) ;
	    }
	    ptc_destroy(&uip->c) ;
	    ptm_destroy(&uip->m) ;
	    memset(uip,0,sizeof(UCSYNCER)) ;
	} /* end if (atexit registered) */
}
/* end subroutine (ucsyncer_fini) */

int uc_syncer(int w) noex {
	UCSYNCER	*uip = &ucsyncer_data ;
	int		rs = SR_INVALID ;
	if (w >= 0) {
	    switch (w) {
	    case 0:
	        rs = u_sync() ;
	        break ;
	    case 1:
	        rs = ucsyncer_sendsync(uip) ;
	        break ;
	    default:
	        rs = SR_NOSYS ;
	        break ;
	    } /* end switch */
	} /* end if (valid) */
	return rs ;
}
/* end subroutine (uc_syncer) */


/* local subroutines */

static int ucsyncer_sendsync(UCSYNCER *uip) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	int		c = 0 ;
	if (! uip->f_syncing) {
	    SIGBLOCKER	b ;
	    if ((rs = sigblocker_start(&b,NULL)) >= 0) {
	        if ((rs = ucsyncer_init()) >= 0) {
		    if ((rs = ucsyncer_run(uip)) >= 0) {
		        const int	cmd = cmd_sync ;
		        rs = ucsyncer_cmdsend(uip,cmd) ;
		        c = uip->count ;
		    }
	        } /* end if (init) */
	        rs1 = sigblocker_finish(&b) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (sigblock) */
	} /* end if (syncing not in progress) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (ucsyncer_sendsync) */

static int ucsyncer_run(UCSYNCER *uip) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	int		f = FALSE ;
	if (! uip->f_running) {
	    if ((rs = uc_forklockbegin(-1)) >= 0) { /* multi */
	        if ((rs = ptm_lock(&uip->m)) >= 0) { /* single */
		    if (! uip->f_running) {
		        rs = ucsyncer_runner(uip) ;
		        f = rs ;
		    } else {
			const pid_t	pid = getpid() ;
			if (pid != uip->pid) {
			    uip->f_running = FALSE ;
			    uip->f_exiting = FALSE ;
			    uip->pid = pid ;
			    rs = ucsyncer_runner(uip) ;
			    f = rs ;
			}
		    } /* end if (not running) */
	            rs1 = ptm_unlock(&uip->m) ;
		    if (rs >= 0) rs = rs1 ;
	        } /* end if (mutex) */
	        rs1 = uc_forklockend() ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (forklock) */
	} else {
	    rs = ucsyncer_runcheck(uip) ;
	    f = rs ;
	} /* end if (not-running) */
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (ucsyncer_run) */

static int ucsyncer_runcheck(UCSYNCER *uip) noex {
	const pid_t	pid = getpid() ;
	int		rs = SR_OK ;
	int		f = FALSE ;
	if (pid != uip->pid) {
	    uip->f_running = FALSE ;
	    uip->f_exiting = FALSE ;
	    uip->pid = pid ;
	    rs = ucsyncer_run(uip) ;
	    f = rs ;
	}
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (ucsyncer_runcheck) */

static int ucsyncer_runner(UCSYNCER *uip) noex {
	PTA		ta ;
	int		rs ;
	int		rs1 ;
	int		f = FALSE ;
	if ((rs = pta_create(&ta)) >= 0) {
	    const int	scope = UCSYNCER_SCOPE ;
	    if ((rs = pta_setscope(&ta,scope)) >= 0) {
		pthread_t	tid ;
		tworker		wt = (tworker) ucsyncer_worker ;
		if ((rs = uptcreate(&tid,&ta,wt,uip)) >= 0) {
		    uip->f_running = TRUE ;
		    uip->tid = tid ;
		    f = TRUE ;
		} /* end if (pthread-create) */
	    } /* end if (pta-setscope) */
	    rs1 = pta_destroy(&ta) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (pta) */
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (ucsyncer_runner) */

static int ucsyncer_worker(UCSYNCER *uip) noex {
	int		rs ;
	while ((rs = ucsyncer_cmdrecv(uip)) > 0) {
	    switch (rs) {
	    case cmd_sync:
		rs = ucsyncer_worksync(uip) ;
		break ;
	    } /* end switch */
	    if (rs < 0) break ;
	} /* end while (looping on commands) */
	uip->f_exiting = TRUE ;
	return rs ;
}
/* end subroutine (ucsyncer_worker) */

static int ucsyncer_worksync(UCSYNCER *uip) noex {
	int		rs ;
	uip->f_syncing = TRUE ;
	if ((rs = u_sync()) >= 0) {
	    uip->count += 1 ;
	}
	uip->f_syncing = FALSE ;
	return rs ;
}
/* end subroutine (ucsyncer_worksync) */

static int ucsyncer_cmdsend(UCSYNCER *uip,int cmd) noex {
	int		rs ;
	int		rs1 ;
	int		to = 5 ;
	if ((rs = ptm_lockto(&uip->m,to)) >= 0) {
	    if (! uip->f_exiting) {
	        uip->waiters += 1 ;
	        while ((rs >= 0) && uip->f_cmd) {
		    rs = ptc_waiter(&uip->c,&uip->m,to) ;
	        } /* end while */
	        if (rs >= 0) {
	            uip->cmd = cmd ;
	            uip->f_cmd = TRUE ;
		    if (uip->waiters > 1) {
	                rs = ptc_signal(&uip->c) ;
		    }
	        }
	        uip->waiters -= 1 ;
	    } else {
		rs = SR_HANGUP ;
	    }
	    rs1 = ptm_unlock(&uip->m) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (mutex-section) */
	return rs ;
}
/* end subroutine (ucsyncer_cmdsend) */

static int ucsyncer_cmdrecv(UCSYNCER *uip) noex {
	int		rs ;
	int		rs1 ;
	int		to = 1 ;
	int		cmd = 0 ;
	if ((rs = ptm_lockto(&uip->m,to)) >= 0) {
	    uip->waiters += 1 ;
	    to = -1 ;
	    while ((rs >= 0) && (! uip->f_cmd)) {
		rs = ptc_waiter(&uip->c,&uip->m,to) ;
	    } /* end while */
	    if (rs >= 0) {
	        cmd = uip->cmd ;
	        uip->f_cmd = FALSE ;
		if (uip->waiters > 1) {
	            rs = ptc_signal(&uip->c) ;
		}
	    }
	    uip->waiters -= 1 ;
	    rs1 = ptm_unlock(&uip->m) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (mutex-section) */
	return (rs >= 0) ? cmd : rs ;
}
/* end subroutine (ucsyncer_cmdrecv) */

static int ucsyncer_waitdone(UCSYNCER *uip) noex {
	int		rs = SR_OK ;
	if (uip->f_running) {
	    const pid_t	pid = getpid() ;
	    if (pid == uip->pid) {
	        const int	cmd = cmd_exit ;
	        if ((rs = ucsyncer_cmdsend(uip,cmd)) >= 0) {
	 	    pthread_t	tid = uip->tid ;
		    int		trs ;
		    if ((rs = uptjoin(tid,&trs)) >= 0) {
		        uip->f_running = FALSE ;
		        rs = trs ;
		    } else if (rs == SR_SRCH) {
		        uip->f_running = FALSE ;
		        rs = SR_OK ;
		    }
	        } /* end if (ucsyncer_sendsync) */
	    } else {
		uip->f_running = FALSE ;
	    }
	} /* end if (running) */
	return rs ;
}
/* end subroutine (ucsyncer_waitdone) */

static void ucsyncer_atforkbefore() noex {
	UCSYNCER	*uip = &ucsyncer_data ;
	ptm_lock(&uip->m) ;
}
/* end subroutine (ucsyncer_atforkbefore) */

static void ucsyncer_atforkparent() noex {
	UCSYNCER	*uip = &ucsyncer_data ;
	ptm_unlock(&uip->m) ;
}
/* end subroutine (ucsyncer_atforkparent) */

static void ucsyncer_atforkchild() noex {
	UCSYNCER	*uip = &ucsyncer_data ;
	uip->f_running = FALSE ;
	uip->f_exiting = FALSE ;
	uip->pid = getpid() ;
	ptm_unlock(&uip->m) ;
}
/* end subroutine (ucsyncer_atforkchild) */


