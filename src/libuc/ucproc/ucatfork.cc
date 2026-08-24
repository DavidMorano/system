/* ucatfork SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* interface components for UNIX® library-3c */
/* an enhanced UNIX®-like |pthread_atfork(3pthread)| subroutine */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-10, David A­D­ Morano
	This is being written to add an "unregister" feature to the
	'atfork' capability that came with POSIX threads.  In the
	past we always had pure reentrant subroutines without hidden
	locks associated with them.  But more recently we have
	started to dabble with hidden locks.  The problem is that
	there is no way to put these hidden locks into a library
	that also gets unloaded (unmapped) after it is done being
	used.  The standard |pthread_atfork(3pthread)| does not
	have an "unregister" feature associated with it.  This is
	what we are creating here.

	= 2018-09-28, David A-D- Morano
	Small refactor (mostly pretty-up).
	
*/

/* Copyright © 1998,2018 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Group:
	ucatfork

	Names:
	uc_atforkrec
	uc_atforkexp
	ucatfork_init
	ucatfork_fini
	ucatfork_trackbegin

	Description:
	UNIX® operations realted to |fork(2)|.
	We are attempting to add an "unregister" feature to the
	|pthread_atfork(3pthread)| facility.  We need to create a
	whole new interface for this.  This new interface will consist
	of:

	+ uc_atforkrec(3uc)
	+ uc_atforkexp(3uc)

	We suffered a lot when first learning that
	|pthread_atfork(3pthread)| does not get its registered
	subroutines removed at module unload time (as though using
	something like |dlclose(3dl)|) on all OSes (even POSIX-compliant
	OSes).  So we attempt here to provide something that does
	the un-registering at module unload time.  The failure by
	some OSes to call and remove at-fork handlers on module
	unload was just a failure by those OSes.  And I will not
	name names on some of the supposedly more responsible OSes
	which made this failure (Slowlaris!).  This should have
	been in the original POSIX specification on at-fork handlers,
	but was somehow not mentioned.
	Enjoy.

  	Synopsis:
	int uc_atforkrec(void_f b,void_f p,void_f c) noex
	int uc_atforkexp(void_f b,void_f p,void_f c) noex
	int ucatfork_init() noex
	int ucatfork_fini() noex
	int ucatfork_trackbegin() noex

	Arguments:
	b		handler becore
	p		handler parent
	c		handler child

	Returns:
	>=0		OK
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>		/* POSIX® */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include        <timewatch.hh>		/* LIBU */
#include	<sigblocker.h>		/* LIBU */
#include	<ptm.h>			/* LIBU */
#include	<uclibmem.h>		/* LIBUC */
#include	<ucfork.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */

#include	"ucatfork.h"

#pragma		GCC dependency		"mod/usysbasic.ccm"

import usysbasic ;			/* |uatexit(3u)| + |uatfork(3u)| */

/* local defines */

#define	UCAF		ucatfork_head
#define	UCAF_ENT	ucatfork_ent
#define	UCAF_LIST	ucatfork_list


/* imported namespaces */


/* local typedefs */


/* external subroutines */

extern "C" {
    extern int uc_atforkrec(void_f,void_f,void_f) noex ;
    extern int uc_atforkexp(void_f,void_f,void_f) noex ;
    extern int ucatfork_init() noex ;
    extern int ucatfork_fini() noex ;
    extern int ucatfork_trackbegin() noex ;
} /* end extern (C) */


/* local structures */

namespace {
    struct ucatfork_ent {
	ucatfork_ent	*next ;
	ucatfork_ent	*prev ;
	void_f		sub_before ;
	void_f		sub_parent ;
	void_f		sub_child ;
    } ; /* end struct */
    struct ucatfork_list {
	ucatfork_ent	*head ;
	ucatfork_ent	*tail ;
    } ; /* end struct */
    struct ucatfork_head {
	ptm		mx ;		/* data mutex */
	ucatfork_list	hlist ;		/* memory allocations */
	aflag		fvoid ;
	aflag		finit ;
	aflag		finitdone ;
	aflag		ftrack ;
	int		init() noex ;
	int		fini() noex ;
	int		record(void_f,void_f,void_f) noex ;
	int		expunge(void_f,void_f,void_f) noex ;
	int		trackbegin() noex ;
	int		trackend() noex ;
	void		atforkbefore() noex ;
	void		atforkparent() noex ;
	void		atforkchild() noex ;
	destruct ucatfork_head() noex {
	    if (cint rs = fini() ; rs < 0) {
		ulogerror("ucatfork",rs,"dtor-fini") ;
	    }
	} ; /* end dtor (ucatfork) */
    } ; /* end struct (ucatfork) */
} /* end namespace */


/* forward references */

extern "C" {
    local void ucatfork_atforkbefore() noex ;
    local void ucatfork_atforkparent() noex ;
    local void ucatfork_atforkchild() noex ;
    local void ucatfork_exit() noex ;
} /* end extern (C) */

local int	list_add(UCAF_LIST *,UCAF_ENT *) noex ;
local int	list_rem(UCAF_LIST *,UCAF_ENT *) noex ;

local int	entry_load(UCAF_ENT *,void_f,void_f,void_f) noex ;
local int	entry_match(UCAF_ENT *,void_f,void_f,void_f) noex ;


/* local variables */

static ucatfork_head		ucatfork_data ;


/* exported variables */


/* exported subroutines */

int uc_atforkrec(void_f b,void_f p,void_f c) noex {
	return ucatfork_data.record(b,p,c) ;
} /* end subroutine */

int uc_atforkexp(void_f b,void_f p,void_f c) noex {
	return ucatfork_data.expunge(b,p,c) ;
} /* end subroutine */

int ucatfork_init() noex {
	return ucatfork_data.init() ;
} /* end subroutine */

int ucatfork_fini() noex {
	return ucatfork_data.fini() ;
} /* end subroutine */

int ucatfork_trackbegin() noex {
	return ucatfork_data.trackbegin() ;
} /* end subroutine */


/* local subroutines */

int ucatfork_head::init() noex {
	int		rs = SR_NXIO ;
	int		f = false ;
	if (!fvoid) {
	    cint	to = utimeout[uto_busy] ;
	    rs = SR_OK ;
	    if (! finit.testandset) {
	        if ((rs = mx.create) >= 0) ylikely {
	            void_f	sb = ucatfork_atforkbefore ;
	            void_f	sp = ucatfork_atforkparent ;
	            void_f	sc = ucatfork_atforkchild ;
	            if ((rs = uatfork(sb,sp,sc)) >= 0) ylikely {
	                if ((rs = u_atexit(ucatfork_exit)) >= 0) ylikely {
	        	    finitdone = true ;
	        	    f = true ;
	                } /* end if (uc_atexit) */
			if (rs < 0) {
			    u_atforkexp(sb,sp,sc) ;
			} /* end if (error) */
	            } /* end if (u_atfork) */
		    if (rs < 0) {
		        mx.destroy() ;
		    } /* end if (error) */
	        } /* end if (ptm-create) */
	        if (rs < 0) {
	            finit = false ;
		} /* end if (error) */
	    } else if (! finitdone) {
	        timewatch	tw(to) ;
	        auto lamb = [this] () -> int {
	            int		rsl = SR_OK ;
	            if (!finit) {
		        rsl = SR_LOCKFAIL ;		/* <- failure */
	            } else if (finitdone) {
		        rsl = 1 ;			/* <- OK ready */
	            }
	            return rsl ;
	        } ; /* end lambda (lamb) */
	        rs = tw(lamb) ;		/* <- time-watching occurs in there */
	    } /* end if (initialization) */
	} /* end if (no-void) */
	return (rs >= 0) ? f : rs ;
} /* end subroutine (ucatfork_head::init) */

int ucatfork_head::fini() noex {
	int		rs = SR_OK ;	
	int		rs1 ;
	if (finitdone) ylikely {
	    fvoid = true ;
	    if (ftrack) {
	        rs1 = trackend() ;
		if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = mx.destroy() ;
		if (rs >= 0) rs = rs1 ;
	    }
	    finit = false ;
	    finitdone = false ;
	} /* end if (was initialized) */
	return rs ;
} /* end subroutine (ucatfork_head::fini) */

int ucatfork_head::record(void_f sb,void_f sp,void_f sc) noex {
	int		rs ;
	int		rs1 ;
	if (sigblocker b ; (rs = b.start) >= 0) ylikely {
	    if ((rs = init()) >= 0) ylikely {
	        if ((rs = uc_forklockbegin(-1)) >= 0) { /* multi */
	            if ((rs = mx.lockbegin) >= 0) { /* single */
			ucatfork_ent	*ep{} ;
	                if ((rs = ucatfork_trackbegin()) >= 0) {
	                    cint	esz = szof(UCAF_ENT) ;
	                    if ((rs = lm_mall(esz,&ep)) >= 0) {
				ucatfork_list	*lp = &hlist ;
				entry_load(ep,sb,sp,sc) ;
				list_add(lp,ep) ;
			    } /* end if (memory-acquire) */
	                } /* end if (track-begin) */
	                rs1 = mx.lockend ;
			if (rs >= 0) rs = rs1 ;
	            } /* end if (mutex) */
	            rs1 = uc_forklockend() ;
		    if (rs >= 0) rs = rs1 ;
	        } /* end if (forklock) */
	    } /* end if (init) */
	    rs1 = b.finish ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (sigblocker) */
	return rs ;
} /* end subroutine (ucatfork_head::record) */

int ucatfork_head::expunge(void_f sb,void_f sp,void_f sc) noex {
	int		rs ;
	int		rs1 ;
	int		c = 0 ;
	if (sigblocker b ; (rs = b.start) >= 0) ylikely {
	    if ((rs = init()) >= 0) ylikely {
	        if ((rs = uc_forklockbegin(-1)) >= 0) { /* multi */
	            if ((rs = mx.lockbegin) >= 0) { /* single */
			ucatfork_list	*lp = &hlist ;
	                if ((rs = ucatfork_trackbegin()) >= 0) {    
	                    UCAF_ENT	*ep = lp->head ;
	                    UCAF_ENT	*nep ;
	                    while (ep) {
	                        nep = ep->next ;
				if (entry_match(ep,sb,sp,sc)) {
				    c += 1 ;
				    list_rem(lp,ep) ;
				    lm_free(ep) ;
	                        } /* end if (match) */
	                        ep = nep ;
	                    } /* end while (deleting matches) */
	                } /* end if (track-begin) */
	                rs1 = mx.lockend ;
			if (rs >= 0) rs = rs1 ;
	            } /* end if (mutex) */
	            rs1 = uc_forklockend() ;
		    if (rs >= 0) rs = rs1 ;
	        } /* end if (forklock) */
	    } /* end if (init) */
	    rs1 = b.finish ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (sigblock) */
	return (rs >= 0) ? c : rs ;
} /* end subroutine (ucatfork_head::expunge) */

int ucatfork_head::trackbegin() noex {
	ftrack = true ;
	return SR_OK ;
} /* end subroutine (ucatfork_head::trackbegin) */

int ucatfork_head::trackend() noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (ftrack) {
	    ucatfork_ent	*ep = hlist.head ;
	    ucatfork_ent	*nep ;
	    ftrack = false ;
	    while (ep) {
	        nep = ep->next ;
	        rs1 = lm_free(ep) ;
		if (rs >= 0) rs = rs1 ;
	        ep = nep ;
	    } /* end while */
	    hlist.head = nullptr ;
	    hlist.tail = nullptr ;
	} /* end if (tracking was started) */
	return rs ;
} /* end subroutine (ucatfork_head::trackend) */

/* traverse the list backwards (tail to head) */
void ucatfork_head::atforkbefore() noex {
	int		rs = SR_OK ;
	if (finitdone) {
	    if ((rs = mx.lockbegin) >= 0) {
	        ucatfork_ent	*ep = hlist.tail ;
	        ucatfork_ent	*pep ;
	        while (ep) {
	            pep = ep->prev ;
	            if (ep->sub_before) {
			(*ep->sub_before)() ;
		    }
	            ep = pep ;
	        } /* end if while */
	    } /* end if (locked) */
	}
	(void) rs ;
} /* end subroutine (ucatfork_head::atforkbefore) */

/* traverse the list forwards (head to tail) */
void ucatfork_head::atforkparent() noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (finitdone) ylikely {
	    ucatfork_ent	*ep = hlist.head ;
	    ucatfork_ent	*nep ;
	    while (ep) {
	        nep = ep->next ;
	        if (ep->sub_parent) {
		    (*ep->sub_parent)() ;
		}
	        ep = nep ;
	    } /* end while */
	    {
	        rs1 = mx.lockend() ;
		if (rs >= 0) rs = rs1 ;
	    }
	}
	(void) rs ;
} /* end subroutine (ucatfork_head::atforkparent) */

/* traverse the list forwards (head to tail) */
void ucatfork_head::atforkchild() noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (finitdone) {
	    ucatfork_ent	*ep = hlist.head ;
	    ucatfork_ent	*nep ;
	    while (ep) {
	        nep = ep->next ;
	        if (ep->sub_child) {
		    (*ep->sub_child)() ;
		}
	        ep = nep ;
	    } /* end while */
	    {
	        rs1 = mx.lockend() ;
	        if (rs >= 0) rs = rs1 ;
	    }
	}
	(void) rs ;
} /* end subroutine (ucatfork_head::atforkchild) */

local void ucatfork_atforkbefore() noex {
	ucatfork_data.atforkbefore() ;
} /* end subroutine */

local void ucatfork_atforkparent() noex {
	ucatfork_data.atforkparent() ;
} /* end subroutine */

local void ucatfork_atforkchild() noex {
	ucatfork_data.atforkchild() ;
} /* end subroutine */

local void ucatfork_exit() noex {
	ucatfork_data.fvoid = true ;
} /* end subroutine */

/* add at the tail */
local int list_add(ucatfork_list *lp,ucatfork_ent *ep) noex {
	ucatfork_ent	*lep = lp->tail ;
	if (lep) {
	    lep->next = ep ;
	    ep->prev = lep ;
	} else {
	    ep->prev = nullptr ;
	}
	lp->tail = ep ;
	if (lp->head == nullptr) {
	    lp->head = ep ;
	}
	return SR_OK ;
} /* end subroutine (list_add) */

/* this is an entry-unlink operation (remove from middle) */
local int list_rem(ucatfork_list *lp,ucatfork_ent *ep) noex {
	ucatfork_ent	*pep = ep->prev ;
	ucatfork_ent	*nep = ep->next ;
	if (nep) {
	    nep->prev = ep->prev ;
	} else {
	    lp->tail = pep ;
	}
	if (pep) {
	    pep->next = ep->next ;
	} else {
	    lp->head = nep ;
	}
	return SR_OK ;
} /* end subroutine (list_rem) */

local int entry_load(ucatfork_ent *ep,void_f sb,void_f sp,void_f sc) noex {
	ep->sub_before = sb ;
	ep->sub_parent = sp ;
	ep->sub_child = sc ;
	ep->next = nullptr ;
	return SR_OK ;
} /* end subroutine (entry_load) */

local int entry_match(ucatfork_ent *ep,void_f sb,void_f sp,void_f sc) noex {
	int		f = true ;
	f = f && (ep->sub_before == sb) ;
	f = f && (ep->sub_parent == sp) ;
	f = f && (ep->sub_child == sc) ;
	return f ;
} /* end subroutine (entry_match) */


