/* uregfork SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* interface components for UNIX® library-3c */
/* an enhanced UNIX®-like |pthread_forkat(3pthread)| subroutine */
/* version %I% last-modified %G% */


/* revision history:

	= 2014-05-09, David A­D­ Morano
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

/* Copyright © 2014,2018 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	uregfork

	Description:
	We are attempting to add an "unregister" feature to the
	|pthread_forkat(3pthread)| facility.  We need to create a
	whole new interface for this.  This new interface will consist
	of:

	+ uregfork_rec(3uc)
	+ uregfork_exp(3uc)

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

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usupport.h>		/* LIBU */
#include	<umem.hh>		/* LIBU */
#include	<timewatch.hh>		/* LIBU */
#include	<ptm.h>			/* LIBU */
#include	<localmisc.h>		/* LIBU */

#include	"uregfork.hh"

#pragma		GCC dependency		"mod/usigblock.ccm"
#pragma		GCC dependency		"mod/usysbasic.ccm"

import usigblock ;			/* |usigblock(3u)| */
import usysbasic ;

/* local defines */

#define	URF		uregfork
#define	URF_ENT		uregfork_ent
#define	URF_LIST	uregfork_list


/* imported namespaces */

using libu::umem ;			/* variable */


/* local typedefs */


/* external subroutines */


/* local structures */

namespace {
    struct uregfork_ent {
	uregfork_ent	*next ;
	uregfork_ent	*prev ;
	void_f		sub_before ;
	void_f		sub_parent ;
	void_f		sub_child ;
    } ; /* end struct */
    struct uregfork_list {
	uregfork_ent	*head ;
	uregfork_ent	*tail ;
    } ; /* end struct */
    struct uregfork {
	ptm		mx ;		/* data mutex */
	uregfork_list	hlist ;		/* memory allocated */
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
	void		forkatbefore() noex ;
	void		forkatparent() noex ;
	void		forkatchild() noex ;
	destruct	uregfork() noex ;
    } ; /* end struct (uregfork) */
} /* end namespace */


/* forward references */

extern "C" {
    local void uregfork_forkatbefore() noex ;
    local void uregfork_forkatparent() noex ;
    local void uregfork_forkatchild() noex ;
    local void uregfork_exit() noex ;
} /* end extern */

local int	list_add(URF_LIST *,URF_ENT *) noex ;
local int	list_rem(URF_LIST *,URF_ENT *) noex ;

local int	entry_load(URF_ENT *,void_f,void_f,void_f) noex ;
local int	entry_match(URF_ENT *,void_f,void_f,void_f) noex ;


/* local variables */

static uregfork		uregfork_data ;


/* exported variables */


/* exported subroutines */

namespace libu {
    int uregfork_rec(void_f b,void_f p,void_f c) noex {
	int		rs = SR_INVALID ;
	if (b || p || c) {
	    rs = uregfork_data.record(b,p,c) ;
	}
	return rs ;
    } /* end subroutine */
    int uregfork_exp(void_f b,void_f p,void_f c) noex {
	int		rs = SR_INVALID ;
	if (b || p || c) {
	    rs = uregfork_data.expunge(b,p,c) ;
	}
	return rs ;
    } /* end subroutine */
    int uregfork_init() noex {
	return uregfork_data.init() ;
    } /* end subroutine */
    int uregfork_fini() noex {
	return uregfork_data.fini() ;
    } /* end subroutine */
} /* end namespace (libu) */


/* local subroutines */

int uregfork::init() noex {
	int		rs = SR_NXIO ;
	int		f = false ;
	if (!fvoid) ylikely {
	    cint	to = utimeout[uto_busy] ;
	    rs = SR_OK ;
	    if (! finit.testandset) {
	        if ((rs = mx.create) >= 0) {
	            void_f	sb = uregfork_forkatbefore ;
	            void_f	sp = uregfork_forkatparent ;
	            void_f	sc = uregfork_forkatchild ;
	            if ((rs = uforkat(sb,sp,sc)) >= 0) {
	                if ((rs = uatexit(uregfork_exit)) >= 0) {
	        	    finitdone = true ;
	        	    f = true ;
	                } /* end if (uatexit) */
	            } /* end if (uforkat) */
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
	            } /* end */
	            return rsl ;
	        } ; /* end lambda (lamb) */
	        rs = tw(lamb) ;		/* <- time-watching occurs in there */
	    } /* end if (initialization) */
	} /* end if (no-void) */
	return (rs >= 0) ? f : rs ;
} /* end subroutine (uregfork::init) */

int uregfork::fini() noex {
	int		rs = SR_OK ;	
	int		rs1 ;
	if (finitdone) {
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
} /* end subroutine (uregfork::fini) */

int uregfork::record(void_f sb,void_f sp,void_f sc) noex {
	int		rs ;
	int		rs1 ;
	if (usigblock b ; (rs = b.start) >= 0) ylikely {
	    if ((rs = init()) >= 0) ylikely {
	        if ((rs = mx.lockbeg) >= 0) ylikely { /* single */
		    uregfork_ent	*ep{} ;
	            if ((rs = trackbegin()) >= 0) ylikely {
	                cint	esz = szof(URF_ENT) ;
	                if ((rs = umem.mall(esz,&ep)) >= 0) ylikely {
			    uregfork_list	*lp = &hlist ;
			    entry_load(ep,sb,sp,sc) ;
			    list_add(lp,ep) ;
	                } /* end if (memory-acquire) */
	            } /* end if (track-begin) */
	            rs1 = mx.lockend ;
		    if (rs >= 0) rs = rs1 ;
	        } /* end if (mutex) */
	    } /* end if (init) */
	    rs1 = b.finish ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (usigblock) */
	return rs ;
} /* end subroutine (uregfork::record) */

int uregfork::expunge(void_f sb,void_f sp,void_f sc) noex {
	int		rs ;
	int		rs1 ;
	int		c = 0 ;
	if (usigblock b ; (rs = b.start) >= 0) ylikely {
	    if ((rs = init()) >= 0) ylikely {
                if ((rs = mx.lockbeg) >= 0) ylikely { /* single */
                    uregfork_list   *lp = &hlist ;
                    if ((rs = trackbegin()) >= 0) ylikely {    
                        URF_ENT     *ep = lp->head ;
                        URF_ENT     *nep ;
                        while (ep) {
                            nep = ep->next ;
                            if (entry_match(ep,sb,sp,sc)) {
                                c += 1 ;
                                list_rem(lp,ep) ;
                                umem.free(ep) ;
                            } /* end if (memory-release) */
                            ep = nep ;
                        } /* end while (deleting matches) */
                    } /* end if (track-begin) */
                    rs1 = mx.lockend ;
                    if (rs >= 0) rs = rs1 ;
                } /* end if (mutex) */
	    } /* end if (init) */
	    rs1 = b.finish ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (sigblock) */
	return (rs >= 0) ? c : rs ;
} /* end subroutine (uregfork::expunge) */

int uregfork::trackbegin() noex {
	ftrack = true ;
	return SR_OK ;
} /* end subroutine (uregfork::trackbegin) */

int uregfork::trackend() noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (ftrack) {
	    uregfork_ent	*ep = hlist.head ;
	    uregfork_ent	*nep ;
	    ftrack = false ;
	    while (ep) {
	        nep = ep->next ;
	        rs1 = umem.free(ep) ;
		if (rs >= 0) rs = rs1 ;
	        ep = nep ;
	    } /* end while (memory-release) */
	    hlist.head = nullptr ;
	    hlist.tail = nullptr ;
	} /* end if (tracking was started) */
	return rs ;
} /* end subroutine (uregfork::trackend) */

/* traverse the list backwards (tail to head) */
void uregfork::forkatbefore() noex {
	int		rs = SR_OK ;
	if (finitdone) {
	    if ((rs = mx.lockbeg) >= 0) {
	        uregfork_ent	*ep = hlist.tail ;
	        uregfork_ent	*pep ;
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
} /* end subroutine (uregfork::forkatbefore) */

/* traverse the list forwards (head to tail) */
void uregfork::forkatparent() noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (finitdone) {
	    uregfork_ent	*ep = hlist.head ;
	    uregfork_ent	*nep ;
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
} /* end subroutine (uregfork::forkatparent) */

/* traverse the list forwards (head to tail) */
void uregfork::forkatchild() noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (finitdone) {
	    uregfork_ent	*ep = hlist.head ;
	    uregfork_ent	*nep ;
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
} /* end subroutine (uregfork::forkatchild) */

uregfork::destruct uregfork() noex {
	    if (cint rs = fini() ; rs < 0) {
		ulogerror("uregfork",rs,"dtor-fini") ;
	    } /* end if (error) */
} /* end dtor (uregfork) */

local void uregfork_forkatbefore() noex {
	uregfork_data.forkatbefore() ;
} /* end */

local void uregfork_forkatparent() noex {
	uregfork_data.forkatparent() ;
} /* end */

local void uregfork_forkatchild() noex {
	uregfork_data.forkatchild() ;
} /* end */

local void uregfork_exit() noex {
	uregfork_data.fvoid = true ;
} /* end */

/* add at the tail */
local int list_add(uregfork_list *lp,uregfork_ent *ep) noex {
	uregfork_ent	*lep = lp->tail ;
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
local int list_rem(uregfork_list *lp,uregfork_ent *ep) noex {
	uregfork_ent	*pep = ep->prev ;
	uregfork_ent	*nep = ep->next ;
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

local int entry_load(uregfork_ent *ep,void_f sb,void_f sp,void_f sc) noex {
	ep->sub_before = sb ;
	ep->sub_parent = sp ;
	ep->sub_child = sc ;
	ep->next = nullptr ;
	return SR_OK ;
} /* end subroutine (entry_load) */

local int entry_match(uregfork_ent *ep,void_f sb,void_f sp,void_f sc) noex {
	int		f = true ;
	f = f && (ep->sub_before == sb) ;
	f = f && (ep->sub_parent == sp) ;
	f = f && (ep->sub_child == sc) ;
	return f ;
} /* end subroutine (entry_match) */


