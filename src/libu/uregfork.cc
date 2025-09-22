/* uregfork SUPPORT */
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

	+ uregfork_rec(3uc)
	+ uregfork_exp(3uc)

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

#include	"uregfork.hh"

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
    } ;
    struct uregfork_list {
	uregfork_ent	*head ;
	uregfork_ent	*tail ;
    } ;
    struct uregfork {
	ptm		mx ;		/* data mutex */
	uregfork_list	hlist ;		/* memory allocations */
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
	destruct uregfork() noex {
	    if (cint rs = fini() ; rs < 0) {
		ulogerror("uregfork",rs,"dtor-fini") ;
	    }
	} ; /* end dtor (uregfork) */
    } ; /* end struct (uregfork) */
} /* end namespace */


/* forward references */

extern "C" {
    static void uregfork_atforkbefore() noex ;
    static void uregfork_atforkparent() noex ;
    static void uregfork_atforkchild() noex ;
    static void uregfork_exit() noex ;
}

static int	list_add(URF_LIST *,URF_ENT *) noex ;
static int	list_rem(URF_LIST *,URF_ENT *) noex ;

static int	entry_load(URF_ENT *,void_f,void_f,void_f) noex ;
static int	entry_match(URF_ENT *,void_f,void_f,void_f) noex ;


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
    }
    int uregfork_exp(void_f b,void_f p,void_f c) noex {
	int		rs = SR_INVALID ;
	if (b || p || c) {
	    rs = uregfork_data.expunge(b,p,c) ;
	}
	return rs ;
    }
    int uregfork_init() noex {
	return uregfork_data.init() ;
    }
    int uregfork_fini() noex {
	return uregfork_data.fini() ;
    }
} /* end namespace (libu) */


/* local subroutines */

int uregfork::init() noex {
	int		rs = SR_NXIO ;
	int		f = false ;
	if (!fvoid) {
	    cint	to = utimeout[uto_busy] ;
	    rs = SR_OK ;
	    if (! finit.testandset) {
	        if ((rs = mx.create) >= 0) {
	            void_f	sb = uregfork_atforkbefore ;
	            void_f	sp = uregfork_atforkparent ;
	            void_f	sc = uregfork_atforkchild ;
	            if ((rs = uatfork(sb,sp,sc)) >= 0) {
	                if ((rs = uatexit(uregfork_exit)) >= 0) {
	        	    finitdone = true ;
	        	    f = true ;
	                } /* end if (uatexit) */
	            } /* end if (uregfork_rec) */
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
/* end subroutine (uregfork::init) */

int uregfork::fini() noex {
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
/* end subroutine (uregfork::fini) */

int uregfork::record(void_f sb,void_f sp,void_f sc) noex {
	int		rs ;
	int		rs1 ;
	if (usigblock b ; (rs = b.start) >= 0) {
	    if ((rs = init()) >= 0) {
	            if ((rs = mx.lockbegin) >= 0) { /* single */
			uregfork_ent	*ep{} ;
	                if ((rs = trackbegin()) >= 0) {
	                    cint	esz = szof(URF_ENT) ;
	                    if ((rs = umem.malloc(esz,&ep)) >= 0) {
				uregfork_list	*lp = &hlist ;
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
/* end subroutine (uregfork::record) */

int uregfork::expunge(void_f sb,void_f sp,void_f sc) noex {
	int		rs ;
	int		rs1 ;
	int		c = 0 ;
	if (usigblock b ; (rs = b.start) >= 0) {
	    if ((rs = init()) >= 0) {
                if ((rs = mx.lockbegin) >= 0) { /* single */
                    uregfork_list   *lp = &hlist ;
                    if ((rs = trackbegin()) >= 0) {    
                        URF_ENT     *ep = lp->head ;
                        URF_ENT     *nep ;
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
	} /* end if (sigblock) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (uregfork::expunge) */

int uregfork::trackbegin() noex {
	ftrack = true ;
	return SR_OK ;
}
/* end subroutine (uregfork::trackbegin) */

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
	    } /* end while */
	    hlist.head = nullptr ;
	    hlist.tail = nullptr ;
	} /* end if (tracking was started) */
	return rs ;
}
/* end subroutine (uregfork::trackend) */

/* traverse the list backwards (tail to head) */
void uregfork::atforkbefore() noex {
	int		rs = SR_OK ;
	if (finitdone) {
	    if ((rs = mx.lockbegin) >= 0) {
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
	}
	(void) rs ;
}
/* end subroutine (uregfork::atforkbefore) */

/* traverse the list forwards (head to tail) */
void uregfork::atforkparent() noex {
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
	    }
	}
	(void) rs ;
}
/* end subroutine (uregfork::atforkparent) */

/* traverse the list forwards (head to tail) */
void uregfork::atforkchild() noex {
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
	    }
	}
	(void) rs ;
}
/* end subroutine (uregfork::atforkchild) */

static void uregfork_atforkbefore() noex {
	uregfork_data.atforkbefore() ;
}

static void uregfork_atforkparent() noex {
	uregfork_data.atforkparent() ;
}

static void uregfork_atforkchild() noex {
	uregfork_data.atforkchild() ;
}

static void uregfork_exit() noex {
	uregfork_data.fvoid = true ;
}

/* add at the tail */
static int list_add(uregfork_list *lp,uregfork_ent *ep) noex {
	uregfork_ent	*lep = lp->tail ;
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
static int list_rem(uregfork_list *lp,uregfork_ent *ep) noex {
	uregfork_ent	*pep = ep->prev ;
	uregfork_ent	*nep = ep->next ;
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

static int entry_load(uregfork_ent *ep,void_f sb,void_f sp,void_f sc) noex {
	ep->sub_before = sb ;
	ep->sub_parent = sp ;
	ep->sub_child = sc ;
	ep->next = nullptr ;
	return SR_OK ;
}
/* end subroutine (entry_load) */

static int entry_match(uregfork_ent *ep,void_f sb,void_f sp,void_f sc) noex {
	int		f = true ;
	f = f && (ep->sub_before == sb) ;
	f = f && (ep->sub_parent == sp) ;
	f = f && (ep->sub_child == sc) ;
	return f ;
}
/* end subroutine (entry_match) */


