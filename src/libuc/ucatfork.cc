/* ucatfork SUPPORT */
/* lang=C++20 */

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

	= 2018-09-28, David A.D. Morano
	Small refactor (mostly pretty-up).
	
*/

/* Copyright © 2014,2018 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	We are attempting to add an "unregister" feature to the
	|pthread_atfork(3pthread)| facility. We need to create a
	whole new interface for this. This new interface will consist
	of:

	+ uc_atforkrecord(3uc)
	+ uc_atforkexpunge(3uc)

	We suffered a lot when first learning that
	|pthread_atfork(3pthread)| does not get its registered
	subroutines removed at module unload time (as though using
	something like |dlclose(3dl)|) on all OSes (even POSIX-compliant
	OSes). So we attempt here to provide something that does
	the un-registering at module unload time. The failure by
	some OSes to call and remove at-fork handlers on module
	unload was just a failure by those OSes.  And I will not
	name names on some of the supposedly more responsible OSes
	which made this failure (Slowlaris!).  This should have
	been in the original POSIX specification on at-fork handlers,
	but was somehow not mentioned.

	Enjoy.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<csignal>
#include	<cstring>
#include	<usystem.h>
#include	<usupport.h>
#include        <timewatch.hh>
#include	<sigblocker.h>
#include	<ptm.h>
#include	<localmisc.h>


/* local defines */

#define	UCATFORK	struct ucatfork
#define	UCATFORK_ENT	struct ucatfork_ent
#define	UCATFORK_LIST	struct ucatfork_list


/* local namespaces */


/* local typedefs */


/* external subroutines */

extern "C" {
    extern int uc_atforkrecord(void_f,void_f,void_f) noex ;
}


/* local structures */

namespace {
    struct ucatfork_ent {
	ucatfork_ent	*next ;
	ucatfork_ent	*prev ;
	void_f		sub_before ;
	void_f		sub_parent ;
	void_f		sub_child ;
    } ;
    struct ucatfork_list {
	ucatfork_ent	*head ;
	ucatfork_ent	*tail ;
    } ;
    struct ucatfork {
	ptm		m ;		/* data mutex */
	ucatfork_list	list ;		/* memory allocations */
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
	~ucatfork() noex {
	    cint	rs = fini() ;
	    if (rs < 0) {
		ulogerror("ucatfork",rs,"dtor-fini") ;
	    }
	} ; /* end dtor (ucatfork) */
    } ; /* end struct (ucatfork) */
}


/* forward references */

extern "C" {
    static void ucatfork_atforkbefore() noex ;
    static void ucatfork_atforkparent() noex ;
    static void ucatfork_atforkchild() noex ;
    static void ucatfork_exit() noex ;
}

static int	list_add(UCATFORK_LIST *,UCATFORK_ENT *) noex ;
static int	list_rem(UCATFORK_LIST *,UCATFORK_ENT *) noex ;

static int	entry_load(UCATFORK_ENT *,void_f,void_f,void_f) noex ;
static int	entry_match(UCATFORK_ENT *,void_f,void_f,void_f) noex ;


/* local variables */

static ucatfork		ucatfork_data ;


/* exported variables */


/* exported subroutines */

int uc_atfork(void_f b,void_f p,void_f c) noex {
	return ucatfork_data.record(b,p,c) ;
}

int uc_atforkrecord(void_f b,void_f p,void_f c) noex {
	return ucatfork_data.record(b,p,c) ;
}

int uc_atforkexpunge(void_f b,void_f p,void_f c) noex {
	return ucatfork_data.expunge(b,p,c) ;
}

int ucatfork_init() noex {
	return ucatfork_data.init() ;
}

int ucatfork_fini() noex {
	return ucatfork_data.fini() ;
}

int ucatfork_trackbegin() noex {
	return ucatfork_data.trackbegin() ;
}


/* local subroutines */

int ucatfork::init() noex {
	int		rs = SR_NXIO ;
	int		f = false ;
	if (!fvoid) {
	    cint		to = utimeout[uto_busy] ;
	    rs = SR_OK ;
	    if (! finit.testandset) {
	        if ((rs = m.create) >= 0) {
	            void_f	sb = ucatfork_atforkbefore ;
	            void_f	sp = ucatfork_atforkparent ;
	            void_f	sc = ucatfork_atforkchild ;
	            if ((rs = pt_atfork(sb,sp,sc)) >= 0) {
	                if ((rs = uc_atexit(ucatfork_exit)) >= 0) {
	        	    finitdone = true ;
	        	    f = true ;
	                } /* end if (uc_atexit) */
	            } /* end if (pt_atfork) */
		    if (rs < 0) {
		        m.destroy() ;
		    }
	        } /* end if (ptm-create) */
	        if (rs < 0) {
	            finit = false ;
		}
	    } else if (! finitdone) {
	        timewatch	tw(to) ;
	        auto lamb = [this] () -> int {
	            int		rs = SR_OK ;
	            if (!finit) {
		        rs = SR_LOCKLOST ;		/* <- failure */
	            } else if (finitdone) {
		        rs = 1 ;			/* <- OK ready */
	            }
	            return rs ;
	        } ; /* end lambda (lamb) */
	        rs = tw(lamb) ;		/* <- time-watching occurs in there */
	    } /* end if (initialization) */
	} /* end if (no-void) */
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (ucatfork::init) */

int ucatfork::fini() noex {
	int		rs = SR_OK ;	
	int		rs1 ;
	if (finitdone) {
	    fvoid = true ;
	    if (ftrack) {
	        rs1 = trackend() ;
		if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = m.destroy() ;
		if (rs >= 0) rs = rs1 ;
	    }
	    finit = false ;
	    finitdone = false ;
	} /* end if (was initialized) */
	return rs ;
}
/* end subroutine (ucatfork::fini) */

int ucatfork::record(void_f sb,void_f sp,void_f sc) noex {
	sigblocker	b ;
	int		rs ;
	int		rs1 ;
	if ((rs = b.start) >= 0) {
	    if ((rs = init()) >= 0) {
	        if ((rs = uc_forklockbegin(-1)) >= 0) { /* multi */
	            if ((rs = m.lockbegin) >= 0) { /* single */
			ucatfork_ent	*ep{} ;
	                if ((rs = ucatfork_trackbegin()) >= 0) {
	                    cint	esize = sizeof(UCATFORK_ENT) ;
	                    if ((rs = uc_libmalloc(esize,&ep)) >= 0) {
				ucatfork_list	*lp = &list ;
				entry_load(ep,sb,sp,sc) ;
				list_add(lp,ep) ;
	                    } /* end if (memory-allocation) */
	                } /* end if (track-begin) */
	                rs1 = m.lockend ;
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
}
/* end subroutine (ucatfork::record) */

int ucatfork::expunge(void_f sb,void_f sp,void_f sc) noex {
	sigblocker	b ;
	int		rs ;
	int		rs1 ;
	int		c = 0 ;
	if ((rs = b.start) >= 0) {
	    if ((rs = init()) >= 0) {
	        if ((rs = uc_forklockbegin(-1)) >= 0) { /* multi */
	            if ((rs = m.lockbegin) >= 0) { /* single */
			ucatfork_list	*lp = &list ;
	                if ((rs = ucatfork_trackbegin()) >= 0) {    
	                    UCATFORK_ENT	*ep = lp->head ;
	                    UCATFORK_ENT	*nep ;
	                    while (ep) {
	                        nep = ep->next ;
				if (entry_match(ep,sb,sp,sc)) {
				    c += 1 ;
				    list_rem(lp,ep) ;
				    uc_libfree(ep) ;
	                        } /* end if (match) */
	                        ep = nep ;
	                    } /* end while (deleting matches) */
	                } /* end if (track-begin) */
	                rs1 = m.lockend ;
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
}
/* end subroutine (ucatfork::expunge) */

int ucatfork::trackbegin() noex {
	ftrack = true ;
	return SR_OK ;
}
/* end subroutine (ucatfork::trackbegin) */

int ucatfork::trackend() noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (ftrack) {
	    ucatfork_ent	*ep = list.head ;
	    ucatfork_ent	*nep ;
	    ftrack = false ;
	    while (ep) {
	        nep = ep->next ;
	        rs1 = uc_libfree(ep) ;
		if (rs >= 0) rs = rs1 ;
	        ep = nep ;
	    } /* end while */
	    list.head = nullptr ;
	    list.tail = nullptr ;
	} /* end if (tracking was started) */
	return rs ;
}
/* end subroutine (ucatfork::trackend) */

/* traverse the list backwards (tail to head) */
void ucatfork::atforkbefore() noex {
	int		rs = SR_OK ;
	if (finitdone) {
	    if ((rs = m.lockbegin) >= 0) {
	        ucatfork_ent	*ep = list.tail ;
	        ucatfork_ent	*pep ;
	        while (ep) {
	            pep = ep->prev ;
	            if (ep->sub_before) (*ep->sub_before)() ;
	            ep = pep ;
	        } /* end if while */
	    } /* end if (locked) */
	}
	(void) rs ;
}
/* end subroutine (ucatfork::atforkbefore) */

/* traverse the list forwards (head to tail) */
void ucatfork::atforkparent() noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (finitdone) {
	    ucatfork_ent	*ep = list.head ;
	    ucatfork_ent	*nep ;
	    while (ep) {
	        nep = ep->next ;
	        if (ep->sub_parent) (*ep->sub_parent)() ;
	        ep = nep ;
	    } /* end while */
	    {
	        rs1 = m.lockend() ;
		if (rs >= 0) rs = rs1 ;
	    }
	}
	(void) rs ;
}
/* end subroutine (ucatfork::atforkparent) */

/* traverse the list forwards (head to tail) */
void ucatfork::atforkchild() noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (finitdone) {
	    ucatfork_ent	*ep = list.head ;
	    ucatfork_ent	*nep ;
	    while (ep) {
	        nep = ep->next ;
	        if (ep->sub_child) (*ep->sub_child)() ;
	        ep = nep ;
	    } /* end while */
	    {
	        rs1 = m.lockend() ;
	        if (rs >= 0) rs = rs1 ;
	    }
	}
	(void) rs ;
}
/* end subroutine (ucatfork::atforkchild) */

static void ucatfork_atforkbefore() noex {
	ucatfork_data.atforkbefore() ;
}

static void ucatfork_atforkparent() noex {
	ucatfork_data.atforkparent() ;
}

static void ucatfork_atforkchild() noex {
	ucatfork_data.atforkchild() ;
}

static void ucatfork_exit() noex {
	ucatfork_data.fvoid = true ;
}

/* add at the tail */
static int list_add(ucatfork_list *lp,ucatfork_ent *ep) noex {
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
}
/* end subroutine (list_add) */

/* this is an entry-unlink operation (remove from middle) */
static int list_rem(ucatfork_list *lp,ucatfork_ent *ep) noex {
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
}
/* end subroutine (list_rem) */

static int entry_load(ucatfork_ent *ep,void_f sb,void_f sp,void_f sc) noex {
	ep->sub_before = sb ;
	ep->sub_parent = sp ;
	ep->sub_child = sc ;
	ep->next = nullptr ;
	return SR_OK ;
}
/* end subroutine (entry_load) */

static int entry_match(ucatfork_ent *ep,void_f sb,void_f sp,void_f sc) noex {
	int		f = true ;
	f = f && (ep->sub_before == sb) ;
	f = f && (ep->sub_parent == sp) ;
	f = f && (ep->sub_child == sc) ;
	return f ;
}
/* end subroutine (entry_match) */


