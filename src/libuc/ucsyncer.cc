/* ucsyncer SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* interface components for UNIX® library-3c */
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
	>=0		OK
	<0		error (system-return)

	Notes:
	We need to carefully watch out for what happens after a
	|fork(2)|.  Although data structures survice a |fork(2)|,
	running threads besides the one that that forked do not!
	Therein lie the potential problems.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>		/* POSIX® */
#include	<sys/stat.h>		/* POSIX® */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<ucfork.h>		/* LIBUC */
#include	<ucatfork.h>		/* LIBUC */
#include	<ucatexit.h>		/* LIBUC */
#include	<sigblocker.h>		/* LIBUC */
#include	<ptm.h>			/* LIBU */
#include	<ptc.h>			/* LIBU */
#include	<pta.h>			/* LIBU */
#include	<upt.h>			/* LIBU */
#include	<localmisc.h>		/* LIBU */


/* local defines */

#define	US		ucsyncer
#define	US_SCOPE	PTHREAD_SCOPE_SYSTEM


/* local typedefs */


/* external subroutines */

extern "C" {
    int		ucsyncer_init() noex ;
    int		ucsyncer_fini() noex ;
} /* end extern (C) */


/* external variables */


/* local structures */

struct ucsyncer {
	ptm		mx ;		/* data mutex */
	ptc		cn ;		/* condition variable */
	pid_t		pid ;
	pthread_t	tid ;
	volatile int	f_void ;
	volatile int	f_init ;
	volatile int	f_initdone ;
	volatile int	f_running ;
	volatile int	f_cmd ;
	volatile int	f_syncing ;
	volatile int	f_exiting ;
	volatile int	waiters ;
	int		cmd ;
	int		count ;
} ; /* end struct */

enum cmds {
	cmd_exit,
	cmd_sync,
	cmd_overlast
} ; /* end enum */


/* forward references */

local int	ucsyncer_sendsync	(US *) noex ;
local int	ucsyncer_run		(US *) noex ;
local int	ucsyncer_runcheck	(US *) noex ;
local int	ucsyncer_runner		(US *) noex ;
local int	ucsyncer_worker		(US *) noex ;
local int	ucsyncer_worksync	(US *) noex ;
local int	ucsyncer_cmdsend	(US *,int) noex ;
local int	ucsyncer_cmdrecv	(US *) noex ;
local int	ucsyncer_waitdone	(US *) noex ;

extern "C" {
    local void	ucsyncer_atforkbefore() noex ;
    local void	ucsyncer_atforkparent() noex ;
    local void	ucsyncer_atforkchild() noex ;
    local void	ucsyncer_exit() noex ;
} /* end extern (C) */


/* local variables */

static US		ucsyncer_data ;


/* exported variables */


/* exported subroutines */

int ucsyncer_init() noex {
	US	*uip = &ucsyncer_data ;
	int		rs = SR_NXIO ;
	int		f = false ;
	if (! uip->f_void) {
	    rs = SR_OK ;
	    if (! uip->f_init) {
		ptm *mxp = &uip->mx ;
	        uip->f_init = true ;
	        if ((rs = mxp->create) >= 0) {
		    ptc *cnp = &uip->cn ;
	            if ((rs = cnp->create) >= 0) {
	    	        void_f	b = ucsyncer_atforkbefore ;
	    	        void_f	ap = ucsyncer_atforkparent ;
	    	        void_f	ac = ucsyncer_atforkchild ;
	                if ((rs = uc_atforkrec(b,ap,ac)) >= 0) {
	                    if ((rs = uc_atexit(ucsyncer_exit)) >= 0) {
	    	                uip->f_initdone = true ;
			        uip->pid = getpid() ;
			        f = true ;
		            }
		            if (rs < 0) {
		                uc_atforkexp(b,ap,ac) ;
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
	        } /* end if (error) */
	    } else {
	        while ((rs >= 0) && uip->f_init && (! uip->f_initdone)) {
		    rs = msleep(1) ;
		    if (rs == SR_INTR) rs = SR_OK ;
	        } /* end while */
	        if ((rs >= 0) && (! uip->f_init)) rs = SR_LOCKFAIL ;
	    } /* end if */
	} /* end if (not voided) */
	return (rs >= 0) ? f : rs ;
} /* end subroutine (ucsyncer_init) */

int ucsyncer_fini() noex {
	US	*uip = &ucsyncer_data ;
	int		rs = SR_OK ;
	int		rs1 ;
	if (uip->f_initdone) {
	    uip->f_initdone = false ;
	    {
	        rs1 = ucsyncer_waitdone(uip) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
	        void_f	b = ucsyncer_atforkbefore ;
	        void_f	ap = ucsyncer_atforkparent ;
	        void_f	ac = ucsyncer_atforkchild ;
	        rs1 = uc_atforkexp(b,ap,ac) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
		ptc *cnp = &uip->cn ;
	        rs1 = cnp->destroy ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
		ptm *mxp = &uip->mx ;
	        rs1 = mxp->destroy() ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    uip->f_initdone = false ;
	    uip->f_init = false ;
	} /* end if (atexit registered) */
	return rs ;
} /* end subroutine (ucsyncer_fini) */

int uc_syncer(int w) noex {
	US	*uip = &ucsyncer_data ;
	int		rs = SR_INVALID ;
	if (w >= 0) {
	    rs = SR_OK ;
	    switch (w) {
	    case 0:
	        sync() ;
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
} /* end subroutine (uc_syncer) */


/* local subroutines */

local int ucsyncer_sendsync(US *uip) noex {
    	cnullptr	np{} ;
	int		rs = SR_OK ;
	int		rs1 ;
	int		c = 0 ;
	if (! uip->f_syncing) {
	    if (sigblocker b ; (rs = b.start(np)) >= 0) {
	        if ((rs = ucsyncer_init()) >= 0) {
		    if ((rs = ucsyncer_run(uip)) >= 0) {
		        const int	cmd = cmd_sync ;
		        rs = ucsyncer_cmdsend(uip,cmd) ;
		        c = uip->count ;
		    }
	        } /* end if (init) */
	        rs1 = b.finish ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (sigblock) */
	} /* end if (syncing not in progress) */
	return (rs >= 0) ? c : rs ;
} /* end subroutine (ucsyncer_sendsync) */

local int ucsyncer_run(US *uip) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	int		f = false ;
	if (! uip->f_running) {
	    if ((rs = uc_forklockbegin(-1)) >= 0) { /* multi */
		ptm *mxp = &uip->mx ;
	        if ((rs = mxp->lockbegin) >= 0) { /* single */
		    if (! uip->f_running) {
		        rs = ucsyncer_runner(uip) ;
		        f = rs ;
		    } else {
			const pid_t	pid = getpid() ;
			if (pid != uip->pid) {
			    uip->f_running = false ;
			    uip->f_exiting = false ;
			    uip->pid = pid ;
			    rs = ucsyncer_runner(uip) ;
			    f = rs ;
			} /* end if */
		    } /* end if (not running) */
	            rs1 = mxp->lockend ;
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
} /* end subroutine (ucsyncer_run) */

local int ucsyncer_runcheck(US *uip) noex {
	const pid_t	pid = getpid() ;
	int		rs = SR_OK ;
	int		f = false ;
	if (pid != uip->pid) {
	    uip->f_running = false ;
	    uip->f_exiting = false ;
	    uip->pid = pid ;
	    rs = ucsyncer_run(uip) ;
	    f = rs ;
	}
	return (rs >= 0) ? f : rs ;
} /* end subroutine (ucsyncer_runcheck) */

local int ucsyncer_runner(US *uip) noex {
	int		rs ;
	int		rs1 ;
	int		f = false ;
	if (pta ta ; (rs = pta_create(&ta)) >= 0) {
	    const int	scope = US_SCOPE ;
	    if ((rs = pta_setscope(&ta,scope)) >= 0) {
		pthread_t	tid ;
		uptsub_f	tsub = uptsub_f(ucsyncer_worker) ;
		if ((rs = uptcreate(&tid,&ta,tsub,uip)) >= 0) {
		    uip->f_running = true ;
		    uip->tid = tid ;
		    f = true ;
		} /* end if (pthread-create) */
	    } /* end if (pta-setscope) */
	    rs1 = pta_destroy(&ta) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (pta) */
	return (rs >= 0) ? f : rs ;
} /* end subroutine (ucsyncer_runner) */

local int ucsyncer_worker(US *uip) noex {
	int		rs ;
	while ((rs = ucsyncer_cmdrecv(uip)) > 0) {
	    switch (rs) {
	    case cmd_sync:
		rs = ucsyncer_worksync(uip) ;
		break ;
	    } /* end switch */
	    if (rs < 0) break ;
	} /* end while (looping on commands) */
	uip->f_exiting = true ;
	return rs ;
} /* end subroutine (ucsyncer_worker) */

local int ucsyncer_worksync(US *uip) noex {
	int		rs = SR_OK ;
	uip->f_syncing = true ;
	sync() ;
	uip->count += 1 ;
	uip->f_syncing = false ;
	return rs ;
} /* end subroutine (ucsyncer_worksync) */

local int ucsyncer_cmdsend(US *uip,int cmd) noex {
	int		rs ;
	int		rs1 ;
	int		to = 5 ;
	ptm *mxp = &uip->mx ;
	if ((rs = mxp->lockbegin(to)) >= 0) {
	    if (! uip->f_exiting) {
		ptc *cnp = &uip->cn ;
	        uip->waiters += 1 ;
	        while ((rs >= 0) && uip->f_cmd) {
		    rs = cnp->wait(mxp,to) ;
	        } /* end while */
	        if (rs >= 0) {
	            uip->cmd = cmd ;
	            uip->f_cmd = true ;
		    if (uip->waiters > 1) {
	                rs = cnp->signal ;
		    }
	        } /* end if (ok) */
	        uip->waiters -= 1 ;
	    } else {
		rs = SR_HANGUP ;
	    }
	    rs1 = mxp->lockend ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (mutex-section) */
	return rs ;
} /* end subroutine (ucsyncer_cmdsend) */

local int ucsyncer_cmdrecv(US *uip) noex {
	int		rs ;
	int		rs1 ;
	int		to = 1 ;
	int		cmd = 0 ;
	ptm *mxp = &uip->mx ;
	if ((rs = mxp->lockbegin(to)) >= 0) {
	    {
	        ptc *cnp = &uip->cn ;
	        uip->waiters += 1 ;
	        to = -1 ;
	        while ((rs >= 0) && (! uip->f_cmd)) {
		    rs = cnp->wait(mxp,to) ;
	        } /* end while */
	        if (rs >= 0) {
	            cmd = uip->cmd ;
	            uip->f_cmd = false ;
		    if (uip->waiters > 1) {
	                rs = cnp->signal ;
		    }
	        } /* end if (ok) */
	        uip->waiters -= 1 ;
	    } /* end block */
	    rs1 = mxp->lockend ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (mutex-section) */
	return (rs >= 0) ? cmd : rs ;
} /* end subroutine (ucsyncer_cmdrecv) */

local int ucsyncer_waitdone(US *uip) noex {
	int		rs = SR_OK ;
	if (uip->f_running) {
	    const pid_t	pid = getpid() ;
	    if (pid == uip->pid) {
	        const int	cmd = cmd_exit ;
	        if ((rs = ucsyncer_cmdsend(uip,cmd)) >= 0) {
	 	    pthread_t	tid = uip->tid ;
		    int		trs ;
		    if ((rs = uptjoin(tid,&trs)) >= 0) {
		        uip->f_running = false ;
		        rs = trs ;
		    } else if (rs == SR_SRCH) {
		        uip->f_running = false ;
		        rs = SR_OK ;
		    }
	        } /* end if (ucsyncer_sendsync) */
	    } else {
		uip->f_running = false ;
	    }
	} /* end if (running) */
	return rs ;
} /* end subroutine (ucsyncer_waitdone) */

local void ucsyncer_atforkbefore() noex {
	US	*uip = &ucsyncer_data ;
	{
	    ptm *mxp = &uip->mx ;
	    mxp->lockbegin() ;
	}
} /* end subroutine (ucsyncer_atforkbefore) */

local void ucsyncer_atforkparent() noex {
	US	*uip = &ucsyncer_data ;
	{
	    ptm *mxp = &uip->mx ;
	    mxp->lockend() ;
	}
} /* end subroutine (ucsyncer_atforkparent) */

local void ucsyncer_atforkchild() noex {
	US	*uip = &ucsyncer_data ;
	{
	    ptm *mxp = &uip->mx ;
	    uip->f_running = false ;
	    uip->f_exiting = false ;
	    uip->pid = getpid() ;
	    mxp->lockend() ;
	}
} /* end subroutine (ucsyncer_atforkchild) */

local void ucsyncer_exit() noex {
	(void) ucsyncer_fini() ;
} /* end subroutine (ucsyncer_exit) */


