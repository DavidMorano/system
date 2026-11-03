/* ucforkat SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* interface components for UNIX® library-3c */
/* an enhanced UNIX®-like |pthread_forkat(3pthread)| subroutine */
/* version %I% last-modified %G% */

#define	CF_DEBUG	0		/* debugging */

/* revision history:

	= 1998-04-10, David A­D­ Morano
	This is being written to add an "unregister" feature to the
	'forkat' capability that came with POSIX threads.  In the
	past we always had pure reentrant subroutines without hidden
	locks associated with them.  But more recently we have
	started to dabble with hidden locks.  The problem is that
	there is no way to put these hidden locks into a library
	that also gets unloaded (unmapped) after it is done being
	used.  The standard |pthread_forkat(3pthread)| does not
	have an "unregister" feature associated with it.  This is
	what we are creating here.

	= 2018-09-28, David A-D- Morano
	Small refactor (mostly pretty-up).
	
*/

/* Copyright © 1998,2018 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Group:
	ucforkat

	Names:
	ucforkat_init
	ucforkat_fini
	ucforkat_trackbeg
	ucforkat_trackend
	uc_forkatrec
	uc_forkatexp

	Description:
	UNIX® operations realted to |fork(2)|.
	We are attempting to add an "unregister" feature to the
	|pthread_forkat(3pthread)| facility.  We need to create a
	whole new interface for this.  This new interface will consist
	of:

	+ uc_forkatrec(3uc)
	+ uc_forkatexp(3uc)

	We suffered a lot when first learning that
	|pthread_forkat(3pthread)| does not get its registered
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
	int ucforkat_init() noex
	int ucforkat_fini() noex
	int ucforkat_trackbeg() noex
	int ucforkat_trackend() noex
	int uc_forkatrec(void_f b,void_f p,void_f c) noex
	int uc_forkatexp(void_f b,void_f p,void_f c) noex

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
#include	<dprint.hh>		/* LIBU |DPRINTF(3u)| */

#include	"ucforkat.h"

#pragma		GCC dependency		"mod/usysbasic.ccm"

import usysbasic ;			/* |uatexit(3u)| */

/* local defines */

#define	UCAF		ucforkat_head
#define	UCAF_ENT	ucforkat_ent
#define	UCAF_LIST	ucforkat_list
#define	UCAF_TOMTX	5
#define	UCAF_TOFORK	3

#ifndef	CF_DEBUG
#define	CF_DEBUG	0		/* debugging */
#endif


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* local structures */

namespace {
    struct ucforkat_ent {
	ucforkat_ent	*next ;
	ucforkat_ent	*prev ;
	void_f		sub_before ;
	void_f		sub_parent ;
	void_f		sub_child ;
    } ; /* end struct */
    struct ucforkat_list {
	ucforkat_ent	*head ;
	ucforkat_ent	*tail ;
    } ; /* end struct */
    struct ucforkat_head {
	ptm		mx ;		/* data mutex */
	ucforkat_list	hlist ;		/* memory allocations */
	aflag		fvoid ;
	aflag		finit ;
	aflag		finitdone ;
	aflag		ftrack ;
	int		init() noex ;
	int		fini() noex ;
	int		record(void_f,void_f,void_f) noex ;
	int		expunge(void_f,void_f,void_f) noex ;
	int		trackbeg() noex ;
	int		trackend() noex ;
	void		forkatbefore() noex ;
	void		forkatparent() noex ;
	void		forkatchild() noex ;
	destruct ucforkat_head() noex {
	    if (cint rs = fini() ; rs < 0) {
		ulogerror("ucforkat",rs,"dtor-fini") ;
	    } /* end */
	} ; /* end dtor (ucforkat) */
    } ; /* end struct (ucforkat) */
} /* end namespace */


/* forward references */

extern "C" {
    local void ucforkat_forkatbefore() noex ;
    local void ucforkat_forkatparent() noex ;
    local void ucforkat_forkatchild() noex ;
    local void ucforkat_exit() noex ;
} /* end extern (C) */

local int list_add	(UCAF_LIST *,UCAF_ENT *) noex ;
local int list_rem	(UCAF_LIST *,UCAF_ENT *) noex ;

local int entry_load	(UCAF_ENT *,void_f,void_f,void_f) noex ;
local int entry_match	(UCAF_ENT *,void_f,void_f,void_f) noex ;


/* local variables */

static ucforkat_head	ucforkat_data ;
cint			to_fork		= UCAF_TOFORK ;
cint			to_mtx		= UCAF_TOMTX ;
cbool			f_debug		= CF_DEBUG ;


/* exported variables */


/* exported subroutines */

int uc_forkatrec(void_f b,void_f p,void_f c) noex {
	return ucforkat_data.record(b,p,c) ;
} /* end subroutine */

int uc_forkatexp(void_f b,void_f p,void_f c) noex {
	return ucforkat_data.expunge(b,p,c) ;
} /* end subroutine */

int ucforkat_init() noex {
	return ucforkat_data.init() ;
} /* end subroutine */

int ucforkat_fini() noex {
	return ucforkat_data.fini() ;
} /* end subroutine */

int ucforkat_trackbeg() noex {
	return ucforkat_data.trackbeg() ;
} /* end subroutine */


/* local subroutines */

int ucforkat_head::init() noex {
	int		rs = SR_NXIO ;
	int		f = false ;
	if (!fvoid) {
	    cint	to = utimeout[uto_busy] ;
	    rs = SR_OK ;
	    if (! finit.testandset) {
	        if ((rs = mx.create) >= 0) ylikely {
	            void_f	sb = ucforkat_forkatbefore ;
	            void_f	sp = ucforkat_forkatparent ;
	            void_f	sc = ucforkat_forkatchild ;
	            if ((rs = uforkat(sb,sp,sc)) >= 0) ylikely {
	                if ((rs = u_atexit(ucforkat_exit)) >= 0) ylikely {
	        	    finitdone = true ;
	        	    f = true ;
	                } /* end if (uc_atexit) */
			if (rs < 0) {
			    u_forkatexp(sb,sp,sc) ;
			} /* end if (error) */
	            } /* end if (uforkat) */
		    if (rs < 0) {
		        mx.destroy() ;
		    } /* end if (error) */
	        } /* end if (ptm-create) */
	        if (rs < 0) {
	            finit = false ;
		} /* end if (error) */
	    } else if (! finitdone) {
	        timewatch tw(to) ;
	        cauto lamb = [this] () -> int {
	            int rsl = SR_OK ;
	            if (!finit) {
		        rsl = SR_LOCKFAIL ;		/* <- failure */
	            } else if (finitdone) {
		        rsl = 1 ;			/* <- OK ready */
	            } /* end */
	            return rsl ;
	        } ; /* end lambda (lamb) */
	        rs = tw(lamb) ;		/* <- time-watching occurs in there */
	    } /* end if (initialization) */
	} /* end if (no-void) */
	return (rs >= 0) ? f : rs ;
} /* end subroutine (ucforkat_head::init) */

int ucforkat_head::fini() noex {
	int		rs = SR_OK ;	
	int		rs1 ;
	if (finitdone) ylikely {
	    fvoid = true ;
	    if (ftrack) {
	        rs1 = trackend() ;
		if (rs >= 0) rs = rs1 ;
	    } /* end */
	    {
	        rs1 = mx.destroy() ;
		if (rs >= 0) rs = rs1 ;
	    } /* end */
	    finit = false ;
	    finitdone = false ;
	} /* end if (was initialized) */
	return rs ;
} /* end subroutine (ucforkat_head::fini) */

int ucforkat_head::record(void_f sb,void_f sp,void_f sc) noex {
	int		rs ;
	int		rs1 ;
	if (sigblocker b ; (rs = b.start) >= 0) ylikely {
	    if ((rs = init()) >= 0) ylikely {
	        if ((rs = uc_forklockbeg(to_fork)) >= 0) { /* multi */
	            if ((rs = mx.lockbeg(to_mtx)) >= 0) { /* single */
			ucforkat_ent	*ep{} ;
	                if ((rs = ucforkat_trackbeg()) >= 0) {
	                    cint	esz = szof(UCAF_ENT) ;
	                    if ((rs = lm_mall(esz,&ep)) >= 0) {
				ucforkat_list	*lp = &hlist ;
				entry_load(ep,sb,sp,sc) ;
				list_add(lp,ep) ;
			    } /* end if (memory-acquire) */
			    if (rs < 0) {
				rs = ucforkat_trackbeg() ;
			    } /* end if (error) */
	                } /* end if (track-beg) */
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
} /* end subroutine (ucforkat_head::record) */

int ucforkat_head::expunge(void_f sb,void_f sp,void_f sc) noex {
	int		rs ;
	int		rs1 ;
	int		c = 0 ;
	if (sigblocker b ; (rs = b.start) >= 0) ylikely {
	    if ((rs = init()) >= 0) ylikely {
	        if ((rs = uc_forklockbeg(-1)) >= 0) { /* multi */
	            if ((rs = mx.lockbeg) >= 0) { /* single */
			ucforkat_list	*lp = &hlist ;
	                if ((rs = ucforkat_trackbeg()) >= 0) {    
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
	                } /* end if (track-beg) */
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
} /* end subroutine (ucforkat_head::expunge) */

int ucforkat_head::trackbeg() noex {
	ftrack = true ;
	return SR_OK ;
} /* end subroutine (ucforkat_head::trackbeg) */

int ucforkat_head::trackend() noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (ftrack) {
	    ucforkat_ent	*ep = hlist.head ;
	    ucforkat_ent	*nep ;
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
} /* end subroutine (ucforkat_head::trackend) */

/* traverse the list backwards (tail to head) */
void ucforkat_head::forkatbefore() noex {
	int		rs = SR_OK ;
	if (finitdone) {
	    if ((rs = mx.lockbeg) >= 0) {
	        ucforkat_ent	*ep = hlist.tail ;
	        ucforkat_ent	*pep ;
	        while (ep) {
	            pep = ep->prev ;
	            if (ep->sub_before) {
			(*ep->sub_before)() ;
		    }
	            ep = pep ;
	        } /* end if while */
	    } /* end if (locked) */
	} /* end if (finitdone) */
	(void) rs ;
} /* end subroutine (ucforkat_head::forkatbefore) */

/* traverse the list forwards (head to tail) */
void ucforkat_head::forkatparent() noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (finitdone) ylikely {
	    ucforkat_ent	*ep = hlist.head ;
	    ucforkat_ent	*nep ;
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
	    } /* end */
	} /* end if (finitdone) */
	(void) rs ;
} /* end subroutine (ucforkat_head::forkatparent) */

/* traverse the list forwards (head to tail) */
void ucforkat_head::forkatchild() noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (finitdone) {
	    ucforkat_ent	*ep = hlist.head ;
	    ucforkat_ent	*nep ;
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
	    } /* end */
	} /* end if (finitdone) */
	(void) rs ;
} /* end subroutine (ucforkat_head::forkatchild) */

local void ucforkat_forkatbefore() noex {
	ucforkat_data.forkatbefore() ;
} /* end subroutine */

local void ucforkat_forkatparent() noex {
	ucforkat_data.forkatparent() ;
} /* end subroutine */

local void ucforkat_forkatchild() noex {
	ucforkat_data.forkatchild() ;
} /* end subroutine */

local void ucforkat_exit() noex {
	ucforkat_data.fvoid = true ;
} /* end subroutine */

/* add at the tail */
local int list_add(ucforkat_list *lp,ucforkat_ent *ep) noex {
	ucforkat_ent	*lep = lp->tail ;
	if (lep) {
	    lep->next = ep ;
	    ep->prev = lep ;
	} else {
	    ep->prev = nullptr ;
	} /* end */
	lp->tail = ep ;
	if (lp->head == nullptr) {
	    lp->head = ep ;
	} /* end */
	return SR_OK ;
} /* end subroutine (list_add) */

/* this is an entry-unlink operation (remove from middle) */
local int list_rem(ucforkat_list *lp,ucforkat_ent *ep) noex {
	ucforkat_ent	*pep = ep->prev ;
	ucforkat_ent	*nep = ep->next ;
	if (nep) {
	    nep->prev = ep->prev ;
	} else {
	    lp->tail = pep ;
	} /* end */
	if (pep) {
	    pep->next = ep->next ;
	} else {
	    lp->head = nep ;
	} /* end */
	return SR_OK ;
} /* end subroutine (list_rem) */

local int entry_load(ucforkat_ent *ep,void_f sb,void_f sp,void_f sc) noex {
	ep->sub_before = sb ;
	ep->sub_parent = sp ;
	ep->sub_child = sc ;
	ep->next = nullptr ;
	return SR_OK ;
} /* end subroutine (entry_load) */

local int entry_match(ucforkat_ent *ep,void_f sb,void_f sp,void_f sc) noex {
	int		f = true ;
	f = f && (ep->sub_before == sb) ;
	f = f && (ep->sub_parent == sp) ;
	f = f && (ep->sub_child == sc) ;
	return f ;
} /* end subroutine (entry_match) */


