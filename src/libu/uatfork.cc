/* uatfork SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* interface components for UNIX® library-3c */
/* an enhanced UNIX®-like |pthread_atfork(3pthread)| subroutine */
/* version %I% last-modified %G% */


/* revision history:

	= 2014-05-09, David A­D­ Morano
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

/* Copyright © 2014,2018 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	We are attempting to add an "unregister" feature to the
	|pthread_atfork(3pthread)| facility.  We need to create a
	whole new interface for this.  This new interface will consist
	of:

	+ u_atforkrec(3uc)
	+ u_atforkexp(3uc)

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

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<usupport.h>
#include	<umem.hh>
#include	<timewatch.hh>
#include	<ptm.h>
#include	<localmisc.h>

#include	"uatfork.h"

import usigblock ;			/* |usigblock(3u)| */
import usysbasic ;

/* local defines */

#define	UAF		uatfork_head
#define	UAF_ENT		uatfork_ent
#define	UAF_LIST	uatfork_list


/* imported namespaces */

using libu::umem ;			/* variable */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct uatfork_ent {
	uatfork_ent	*next ;
	uatfork_ent	*prev ;
	void_f		sub_before ;
	void_f		sub_parent ;
	void_f		sub_child ;
    } ;
    struct uatfork_list {
	uatfork_ent	*head ;
	uatfork_ent	*tail ;
    } ;
    struct uatfork_head {
	ptm		mx ;		/* data mutex */
	uatfork_list	hlist ;		/* memory allocations */
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
	destruct uatfork_head() noex {
	    if (cint rs = fini() ; rs < 0) {
		ulogerror("uatfork",rs,"dtor-fini") ;
	    }
	} ; /* end dtor (uatfork) */
    } ; /* end struct (uatfork_head) */
} /* end namespace */


/* forward references */

extern "C" {
    static void uatfork_atforkbefore() noex ;
    static void uatfork_atforkparent() noex ;
    static void uatfork_atforkchild() noex ;
    static void uatfork_exit() noex ;
}

static int	list_add(UAF_LIST *,UAF_ENT *) noex ;
static int	list_rem(UAF_LIST *,UAF_ENT *) noex ;

static int	entry_load(UAF_ENT *,void_f,void_f,void_f) noex ;
static int	entry_match(UAF_ENT *,void_f,void_f,void_f) noex ;


/* local variables */

static uatfork_head	uatfork_data ;


/* exported variables */


/* exported subroutines */

int u_atforkrec(void_f b,void_f p,void_f c) noex {
	int		rs = SR_INVALID ;
	if (b || p || c) {
	    rs = uatfork_data.record(b,p,c) ;
	}
	return rs ;
}

int u_atforkexp(void_f b,void_f p,void_f c) noex {
	int		rs = SR_INVALID ;
	if (b || p || c) {
	    rs = uatfork_data.expunge(b,p,c) ;
	}
	return rs ;
}

int uatfork_init() noex {
	return uatfork_data.init() ;
}

int uatfork_fini() noex {
	return uatfork_data.fini() ;
}

int uatfork_trackbegin() noex {
	return uatfork_data.trackbegin() ;
}


/* local subroutines */

int uatfork_head::init() noex {
	int		rs = SR_NXIO ;
	int		f = false ;
	if (!fvoid) {
	    cint	to = utimeout[uto_busy] ;
	    rs = SR_OK ;
	    if (! finit.testandset) {
	        if ((rs = mx.create) >= 0) {
	            void_f	sb = uatfork_atforkbefore ;
	            void_f	sp = uatfork_atforkparent ;
	            void_f	sc = uatfork_atforkchild ;
	            if ((rs = uatfork(sb,sp,sc)) >= 0) {
	                if ((rs = uatexit(uatfork_exit)) >= 0) {
	        	    finitdone = true ;
	        	    f = true ;
	                } /* end if (u_atexit) */
	            } /* end if (u_atfork) */
		    if (rs < 0) {
		        mx.destroy() ;
		    }
	        } /* end if (ptm-create) */
	        if (rs < 0) {
	            finit = false ;
		}
	    } else if (! finitdone) {
	        timewatch	tw(to) ;
	        auto lamb = [this] () -> int {
	            int		rsl = SR_OK ;
	            if (!finit) {
		        rsl = SR_LOCKLOST ;		/* <- failure */
	            } else if (finitdone) {
		        rsl = 1 ;			/* <- OK ready */
	            }
	            return rsl ;
	        } ; /* end lambda (lamb) */
	        rs = tw(lamb) ;		/* <- time-watching occurs in there */
	    } /* end if (initialization) */
	} /* end if (no-void) */
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (uatfork_head::init) */

int uatfork_head::fini() noex {
	int		rs = SR_OK ;	
	int		rs1 ;
	if (finitdone) {
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
}
/* end subroutine (uatfork_head::fini) */

int uatfork_head::record(void_f sb,void_f sp,void_f sc) noex {
	int		rs ;
	int		rs1 ;
	if (usigblock b ; (rs = b.start) >= 0) {
	    if ((rs = init()) >= 0) {
	            if ((rs = mx.lockbegin) >= 0) { /* single */
			uatfork_ent	*ep{} ;
	                if ((rs = uatfork_trackbegin()) >= 0) {
	                    cint	esz = szof(UAF_ENT) ;
	                    if ((rs = umem.malloc(esz,&ep)) >= 0) {
				uatfork_list	*lp = &hlist ;
				entry_load(ep,sb,sp,sc) ;
				list_add(lp,ep) ;
	                    } /* end if (memory-allocation) */
	                } /* end if (track-begin) */
	                rs1 = mx.lockend ;
			if (rs >= 0) rs = rs1 ;
	            } /* end if (mutex) */
	    } /* end if (init) */
	    rs1 = b.finish ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (usigblock) */
	return rs ;
}
/* end subroutine (uatfork_head::record) */

int uatfork_head::expunge(void_f sb,void_f sp,void_f sc) noex {
	int		rs ;
	int		rs1 ;
	int		c = 0 ;
	if (usigblock b ; (rs = b.start) >= 0) {
	    if ((rs = init()) >= 0) {
                if ((rs = mx.lockbegin) >= 0) { /* single */
                    uatfork_list    *lp = &hlist ;
                    if ((rs = uatfork_trackbegin()) >= 0) {    
                        UAF_ENT     *ep = lp->head ;
                        UAF_ENT     *nep ;
                        while (ep) {
                            nep = ep->next ;
                            if (entry_match(ep,sb,sp,sc)) {
                                c += 1 ;
                                list_rem(lp,ep) ;
                                umem.free(ep) ;
                            } /* end if (match) */
                            ep = nep ;
                        } /* end while (deleting matches) */
                    } /* end if (track-begin) */
                    rs1 = mx.lockend ;
                    if (rs >= 0) rs = rs1 ;
                } /* end if (mutex) */
	    } /* end if (init) */
	    rs1 = b.finish ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (usigblock) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (uatfork_head::expunge) */

int uatfork_head::trackbegin() noex {
	ftrack = true ;
	return SR_OK ;
}
/* end subroutine (uatfork_head::trackbegin) */

int uatfork_head::trackend() noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (ftrack) {
	    uatfork_ent	*ep = hlist.head ;
	    uatfork_ent	*nep ;
	    ftrack = false ;
	    while (ep) {
	        nep = ep->next ;
	        rs1 = umem.free(ep) ;
		if (rs >= 0) rs = rs1 ;
	        ep = nep ;
	    } /* end while */
	    hlist.head = nullptr ;
	    hlist.tail = nullptr ;
	} /* end if (tracking was started) */
	return rs ;
}
/* end subroutine (uatfork_head::trackend) */

/* traverse the list backwards (tail to head) */
void uatfork_head::atforkbefore() noex {
	int		rs = SR_OK ;
	if (finitdone) {
	    if ((rs = mx.lockbegin) >= 0) {
	        uatfork_ent	*ep = hlist.tail ;
	        uatfork_ent	*pep ;
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
}
/* end subroutine (uatfork_head::atforkbefore) */

/* traverse the list forwards (head to tail) */
void uatfork_head::atforkparent() noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (finitdone) {
	    uatfork_ent	*ep = hlist.head ;
	    uatfork_ent	*nep ;
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
}
/* end subroutine (uatfork_head::atforkparent) */

/* traverse the list forwards (head to tail) */
void uatfork_head::atforkchild() noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (finitdone) {
	    uatfork_ent	*ep = hlist.head ;
	    uatfork_ent	*nep ;
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
}
/* end subroutine (uatfork_head::atforkchild) */

static void uatfork_atforkbefore() noex {
	uatfork_data.atforkbefore() ;
}

static void uatfork_atforkparent() noex {
	uatfork_data.atforkparent() ;
}

static void uatfork_atforkchild() noex {
	uatfork_data.atforkchild() ;
}

static void uatfork_exit() noex {
	uatfork_data.fvoid = true ;
}

/* add at the tail */
static int list_add(uatfork_list *lp,uatfork_ent *ep) noex {
	uatfork_ent	*lep = lp->tail ;
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
}
/* end subroutine (list_add) */

/* this is an entry-unlink operation (remove from middle) */
static int list_rem(uatfork_list *lp,uatfork_ent *ep) noex {
	uatfork_ent	*pep = ep->prev ;
	uatfork_ent	*nep = ep->next ;
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
}
/* end subroutine (list_rem) */

static int entry_load(uatfork_ent *ep,void_f sb,void_f sp,void_f sc) noex {
	ep->sub_before = sb ;
	ep->sub_parent = sp ;
	ep->sub_child = sc ;
	ep->next = nullptr ;
	return SR_OK ;
}
/* end subroutine (entry_load) */

static int entry_match(uatfork_ent *ep,void_f sb,void_f sp,void_f sc) noex {
	int		f = true ;
	f = f && (ep->sub_before == sb) ;
	f = f && (ep->sub_parent == sp) ;
	f = f && (ep->sub_child == sc) ;
	return f ;
}
/* end subroutine (entry_match) */


