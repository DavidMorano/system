/* memtrack SUPPORT */
/* lang=C++20 */

/* track memory blocks */
/* version %I% last-modified %G% */


/* revision history:

	= 2011-04-12, David A­D­ Morano
	This subroutine was originally written. This is a sort of
	test to replace the previous memory tracking implementation
	inside of the |ucmemalloc(3uc)| facility (so loved).

*/

/* Copyright © 2011 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Names:
	memtrack::start
	memtrack::count
	memtrack::finish
	memtrack::ins
	memtrack::rem
	memtrack::present
	memtrack::get
	memtrack::istart
	memtrack::icount
	memtrack::ifinish

	Description:
	Reack memory blocks.

	Symopsis:
	int memtrack::start(int n) noex

	Arguments:
	n		suggested starting size

	Returns:
	>=0		OK
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<unistd.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdint>
#include	<cstring>
#include	<utility>		/* |std::unreachable()| */
#include	<new>
#include	<usystem.h>

#include	"memtrack.hh"


/* code comments */

#ifdef	COMMENT
struct memtrack_ent {
	void	*addr ;
	int	asize ;
} ;
extern int	memtrack_start(int) noex ;
extern int	memtrack_ins(void *,int) noex ;
extern int	memtrack_rem(void *) noex ;
extern int	memtrack_get(void *,memtrack_ent *) noex ;
extern int	memtrack_finish() noex ;
#endif /* COMMENT */


/* local defines */


/* imported namespaces */

using std::nullptr_t ;			/* type */
using std::nothrow ;			/* constant */


/* local typedefs */


/* external subroutines */


/* local structures */


/* forward references */


/* lcaal variables */

constexpr uint		memtrack_magic = MEMTRACK_MAGIC ;


/* exported variables */


/* exported subroutines */

int memtrack::ins(cvoid *addr,int asize) noex {
	int		rs = SR_NOTOPEN ;
	if (magic == memtrack_magic) {
	    const uintptr_t	a = uintptr_t(addr) ;
	    rs = SR_INVALID ;
	    if (addr && (asize > 0)) {
		const ent	e = { addr, asize } ;
		rs = tp->ins(a,e) ;
	    } /* end if (valid addr) */
	} /* end if (was open) */
	return rs ;
}
/* end method (memtrack::ins) */

int memtrack::rem(cvoid *addr) noex {
	int		rs = SR_NOTOPEN ;
	if (magic == memtrack_magic) {
	    const uintptr_t	a = uintptr_t(addr) ;
	    rs = SR_INVALID ;
	    if (addr) {
		rs = tp->rem(a) ;
	    } /* end if (valid addr) */
	} /* end if (was open) */
	return rs ;
}
/* end method (memtrack::rem) */

int memtrack::present(cvoid *addr) noex {
	int		rs = SR_NOTOPEN ;
	if (magic == memtrack_magic) {
	    const uintptr_t	a = uintptr_t(addr) ;
	    rs = SR_INVALID ;
	    if (addr) {
		ent	e ;
		if ((rs = tp->get(a,&e)) >= 0) {
		    rs = e.asize ;
		}
	    } /* end if (valid addr) */
	} /* end if (was open) */
	return rs ;
}
/* end method (memtrack::present) */

int memtrack::get(cvoid *addr,memtrack_ent *ep) noex {
	int		rs = SR_NOTOPEN ;
	if (magic == memtrack_magic) {
	    const uintptr_t	a = uintptr_t(addr) ;
	    rs = SR_INVALID ;
	    if (addr) {
		ent	e ;
		if ((rs = tp->get(a,&e)) >= 0) {
		    rs = e.asize ;
		    if (ep) *ep = e ;
		}
	    } /* end if (valid addr) */
	} /* end if (was open) */
	return rs ;
}
/* end method (memtrack::get) */


/* local subroutines */

int memtrack::istart(int n) noex {
	int		rs = SR_INVALID ;
	if (n >= 0) {
	    if ((tp = new(nothrow) track_t) != nullptr) {
	        if ((rs = tp->start(n)) >= 0) {
		    magic = memtrack_magic ;
	        }
		if (rs < 0) {
		    delete tp ;
		    tp = nullptr ;
		}
	    } /* end if (new-mapblock) */
	} /* end if (valid) */
	return rs ;
}
/* end method (memtrack::istart) */

int memtrack::ifinish() noex {
	int		rs = SR_NOTOPEN ;
	int		rs1 ;
	if (magic == memtrack_magic) {
	    rs = SR_OK ;
	    {
	        rs1 = tp->finish() ;
		if (rs >= 0) rs = rs1 ;
	    }
	    {
		delete tp ;
		tp = nullptr ;
	    }
	    magic = 0 ;
	} /* end if (was open) */
	return rs ;
}
/* end method (memtrack::ifinish) */

int memtrack::icount() noex {
	int		rs = SR_NOTOPEN ;
	if (magic == memtrack_magic) {
	    rs = tp->count() ;
	} /* end if (was open) */
	return rs ;
}
/* end method (memtrack::icount) */

void memtrack::dtor() noex {
	ulogerror("memtrack",SR_BUGCHECK,"dtor called") ;
	if (magic) {
	    cint	rs = ifinish() ;
	    if (rs < 0) {
		ulogerror("memtrack",rs,"dtor-finish") ;
	    }
	}
}
/* end method (memtrack::dtor) */

int memtrack_co::operator () (int a) noex {
	int		rs = SR_BUGCHECK ;
	if ((w >= 0) && op) {
	    switch (w) {
	    case memtrackmem_start:
		rs = op->istart(a) ;
		break ;
	    case memtrackmem_count:
		rs = op->icount() ;
		break ;
	    case memtrackmem_finish:
		rs = op->ifinish() ;
		break ;
	    default:
		std::unreachable() ;
	    } /* end switch */
	} /* end if (valid) */
	return rs ;
}
/* end method (memtrack_co::operator int) */


