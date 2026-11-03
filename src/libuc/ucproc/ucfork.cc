/* ucfork SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* interface components for UNIX® library-3c */
/* special handling for |fork(2)| */
/* version %I% last-modified %G% */

#define	CF_DEBUG	0		/* debugging */

/* revision history:

	= 1998-04-10, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Mame:
	uc_fork() noex
	uc_forklockbeg(int) noex
	uc_forklockend() noex

	Description:
	These are subroutines implement a feature where there is a
	read-write lock protecting the call to the system |fork(2)|
	subroutine. This is useful for linrary-based user-level
	emulated kernel calls that have statically allocated
	mutex-locks within them.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>		/* POSIX® */
#include	<unistd.h>		/* POSIX® */
#include	<csignal>		/* CSTD */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<timewatch.hh>		/* LIBU */
#include	<sigblocker.h>		/* LIBU */
#include	<ucatexit.h>		/* LIBUC */
#include	<ucgetpid.h>		/* LIBUC */
#include	<lockrw.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */
#include	<dprint.hh>		/* LIBU |DPRINTF(3u)| */

#include	"ucfork.h"


/* local defines */

#ifndef	CF_DEBUG
#define	CF_DEBUG	0		/* debugging */
#endif


/* imported namespaces */


/* local typedefs */


/* external subroutines */

extern "C" {
    int		ucfork_init() noex ;
    int		ucfork_fini() noex ;
} /* end extern (C) */


/* external variables */


/* local structures */

enum forkermems {
	forkermem_init,
	forkermem_fini,
	forkermem_clear,
	forkermem_sysfork,
	forkermem_lockend,
	forkermem_overlast
} ; /* end enum (forkermems) */

namespace {
    struct forker_fl {
	uint		lock:1 ;
    } ; /* end struct (forker_fl) */
    struct forker ;
    struct forker_co {
	forker		*op = nullptr ;
	int		w = -1 ;
	void operator () (forker *p,int m) noex {
	    op = p ;
	    w = m ;
	} ; /* end */
	operator int () noex ;
	int operator () () noex { 
	    return operator int () ;
	} ; /* end */
    } ; /* end struct (forker_co) */
    struct forker {
	friend		forker_co ;
	lockrw		lock ;		/* critical-section lock */
	aflag		fvoid ;
	aflag		finit ;
	aflag		finitdone ;
	forker_co	init ;
	forker_co	fini ;
	forker_co	clear ;
	forker_co	sysfork ;
	forker_co	lockend ;
	forker_fl	fl ;
	forker() noex {
	    init	(this,forkermem_init) ;
	    fini	(this,forkermem_fini) ;
	    clear	(this,forkermem_clear) ;
	    sysfork	(this,forkermem_sysfork) ;
	    lockend	(this,forkermem_lockend) ;
	} ; /* end ctor */
	int lockbeg	(int) noex ;
	destruct forker	() noex ;
    private:
	int pinit	() noex ;
	int pfini	() noex ;
	int pclear	() noex ;
	int psysfork	() noex ;
	int plockend	() noex ;
    } ; /* end struct (forker) */
} /* end namespace */


/* forward references */

extern "C" {
    local void	ucfork_exit() noex ;
} /* end extern (C) */


/* local variables */

static forker		ucfork_data ;
cint			to_lockwr	= 4 ;
cbool			f_debug		= CF_DEBUG ;


/* exported variables */


/* exported subroutines */

int ucfork_init() noex {
	return ucfork_data.init() ;
} /* end subroutine */

int ucfork_fini() noex {
	return ucfork_data.fini() ;
} /* end subroutine */

int uc_fork() noex {
	return ucfork_data.sysfork() ;
} /* end subroutine */

int uc_forklockbeg(int to) noex {
	return ucfork_data.lockbeg(to) ;
} /* end subroutine */

int uc_forklockend() noex {
	return ucfork_data.lockend() ;
} /* end subroutine */


/* local subroutines */

int forker::pinit() noex {
	int		rs = SR_NXIO ;
	int		f = false ;
	DPRINTF("ent fvoid=%u\n",uint(fvoid)) ;
	if (! fvoid) {
	    cint	to = utimeout[uto_busy] ;
	    rs = SR_OK ;
	    if (! finit.testandset) {
	        if ((rs = lock.create) >= 0) {
		    fl.lock = true ;
	            if ((rs = uc_atexit(ucfork_exit)) >= 0) {
	                f = true ;
	                finitdone.set() ;
	            } /* end */
	            if (rs < 0) {
	                lock.destroy() ;
	            } /* end if (error) */
	        } /* end if (lockrw-create) */
	        if (rs < 0) {
	            finit.clear() ;
		} /* end if (error) */
	    } else if (! finitdone) {
	        timewatch tw(to) ;
	        cauto lamb = [this] () -> int {
	            int rsl = SR_OK ;
	            if (! finit) {
		        rsl = SR_LOCKFAIL ;
	            } else if (finitdone) {
		        rsl = 1 ;
	            } /* end */
	            return rsl ;
	        } ; /* end lambda */
	        rs = tw(lamb) ;
	    } /* end if (initialization) */
	} /* end if (not voided) */
	DPRINTF("ret rs=%d f=%u\n",rs,f) ;
	return (rs >= 0) ? f : rs ;
} /* end method (forker::init) */

int forker::pfini() noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (finitdone) {
	    fvoid.set() ;
	    finitdone.clear() ;
	    if (fl.lock) {
		fl.lock = false ;
	        rs1 = lock.destroy ;
		if (rs >= 0) rs = rs1 ;
	    } /* end */
	    finit.clear() ;
	} /* end if (atexit registered) */
	return rs ;
} /* end method (forker::fini) */

int forker::pclear() noex {
    	int		rs = SR_OK ;
	int		rs1 ;
	if (fl.lock) {
	    fl.lock = false ;
	    rs1 = lock.destroy ;
	    if (rs >= 0) rs = rs1 ;
	} /* end */
	fvoid		= false ;
	finit		= false ;
	finitdone	= false ;
	fl = {} ;
	return rs ;
} /* end method (forker::clear) */

int forker::psysfork() noex {
	int		rs ;
	int		rs1 ;
	int		pid = 0 ; /* return-value */
	DPRINTF("ent\n") ;
	if (sigblocker b ; (rs = b.start) >= 0) {
	    if ((rs = init) >= 0) {
	        DPRINTF("lock.lockwr\n") ;
	        if ((rs = lock.lockwr(to_lockwr)) >= 0) {
	            DPRINTF("lock.lockwr() rs=%d\n",rs) ;
	            DPRINTF("-> u_fork\n") ;
	            if ((rs = u_fork()) == 0) { /* child */
	        DPRINTF("child\n") ;
	                clear() ;
	        DPRINTF("fini-after\n") ;
	        DPRINTF("uc_setpid\n") ;
			uc_setpid(0) ;
	        DPRINTF("uc_setpid-after\n") ;
	        DPRINTF("init\n") ;
	                if ((rs = init) > 0) {
	        	    DPRINTF("init() rs=%d\n",rs) ;
	                    rs = 0 ;
	                }
	        DPRINTF("init-out rs=%d\n",rs) ;
	            } else if (rs > 0) {
	        DPRINTF("parent rs=%d\n",rs) ;
			pid = rs ;
	                lock.lockun() ;
	            } /* end */
	        } /* end if (lockrw) */
	        DPRINTF("lock.lockwr-out rs=%d\n",rs) ;
	    } /* end if (ucfork_init) */
	    rs1 = b.finish ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (sigblocker) */
	DPRINTF("ret rs=%d pid=%d\n",rs,pid) ;
	return (rs >= 0) ? pid : rs ;
} /* end mrthod (forker::sysfork) */

int forker::lockbeg(int to) noex {
	int		rs ;
	int		rs1 ;
	int		rv = 0 ; /* return-value */
	DPRINTF("ent\n") ;
	if (sigblocker b ; (rs = b.start) >= 0) {
	    if ((rs = init()) >= 0) {
	        rs = lock.lockrd(to) ;
		rv = rs ;
	    } /* end if (ucfork_init) */
	    rs1 = b.finish ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (sigblocker) */
	DPRINTF("ret rs=%d rv=%u\n",rs,rv) ;
	return (rs >= 0) ? rv : rs ;
} /* end subroutine (forker::lockbeg) */

int forker::plockend() noex {
	int		rs ;
	int		rs1 ;
	int		rv = 0 ; /* return-value */
	DPRINTF("ent\n") ;
	if (sigblocker b ; (rs = b.start) >= 0) {
	    if (finit) {
	        rs = lock.lockun ;
		rv = rs ;
	    } else {
	        rs = SR_NOANODE ;
	    } /* end */
	    rs1 = b.finish ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (sigblocker) */
	DPRINTF("ret rs=%d rv=%u\n",rs,rv) ;
	return (rs >= 0) ? rv : rs ;
} /* end subroutine (forker::lockend) */

forker::destruct forker() noex {
	if (cint rs = fini() ; rs < 0) {
	    ulogerror("ucfork",rs,"dtor-fini") ;
	} /* end */
} /* end dtor (forker::destruct) */

forker_co::operator int () noex {
	int		rs = SR_BUGCHECK ;
	if (op) ylikely {
	    switch (w) {
	    case forkermem_init:
	        rs = op->pinit() ;
	        break ;
	    case forkermem_fini:
	        rs = op->pfini() ;
	        break ;
	    case forkermem_clear:
	        rs = op->pclear() ;
	        break ;
	    case forkermem_sysfork:
	        rs = op->psysfork() ;
	        break ;
	    case forkermem_lockend:
	        rs = op->plockend() ;
	        break ;
	    } /* end switch */
	} /* end if (non-null) */
	return rs ;
} /* end method (forker_co::operator) */

local void ucfork_exit() noex {
	ucfork_data.fvoid = true ;
} /* end subroutine */


