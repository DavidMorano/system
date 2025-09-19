/* memtrack1 MODULE */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* track memory blocks */
/* version %I% last-modified %G% */


/* revision history:

	= 2011-04-12, David A­D­ Morano
	This code was originally written. This is a sort of
	test to replace the previous memory tracking implementation
	inside of the |ucmem(3uc)| facility (so loved).

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
	Track memory blocks.

	Symopsis:
	int memtrack::start(int n) noex

	Arguments:
	n		suggested starting size

	Returns:
	>=0		OK
	<0		error (system-return)

	Notes:
	This object uses the |mapblock(3uc)| object for its
	main dependency.

*******************************************************************************/

module ;

#include	<envstandards.h>	/* ordered first to configure */
#include	<unistd.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdint>		/* |uintptr_t| */
#include	<utility>		/* |std::unreachable()| */
#include	<new>			/* |nothrow| */
#include	<usystem.h>

module memtrack ;

/* code comments */


/* local defines */


/* imported namespaces */

using std::nullptr_t ;			/* type */
using std::nothrow ;			/* constant */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

template<typename ... Args>
static inline int memtrack_magic(memtrack *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    rs = (op->magic == memtrack_magicval) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}
/* end subroutine (memtrack_magic) */

/* lcaal variables */


/* exported variables */


/* exported subroutines */

int memtrack::ins(cvoid *addr,int asize) noex {
	int		rs ;
	if ((rs = memtrack_magic(this)) >= 0) ylikely {
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
	int		rs ;
	if ((rs = memtrack_magic(this)) >= 0) ylikely {
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
	int		rs ;
	if ((rs = memtrack_magic(this)) >= 0) ylikely {
	    const uintptr_t	a = uintptr_t(addr) ;
	    rs = SR_INVALID ;
	    if (addr) {
		if (ent e{} ; (rs = tp->get(a,&e)) >= 0) {
		    rs = e.asize ;
		}
	    } /* end if (valid addr) */
	} /* end if (was open) */
	return rs ;
}
/* end method (memtrack::present) */

int memtrack::get(cvoid *addr,memtrack_ent *ep) noex {
	int		rs ;
	if ((rs = memtrack_magic(this)) >= 0) ylikely {
	    const uintptr_t	a = uintptr_t(addr) ;
	    rs = SR_INVALID ;
	    if (addr) {
		if (ent e{} ; (rs = tp->get(a,&e)) >= 0) {
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
	if (n >= 0) ylikely {
	    if ((tp = new(nothrow) track_t) != nullptr) {
	        if ((rs = tp->start(n)) >= 0) {
		    magic = memtrack_magicval ;
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
	int		rs ;
	int		rs1 ;
	if ((rs = memtrack_magic(this)) >= 0) ylikely {
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
	int		rs ;
	if ((rs = memtrack_magic(this)) >= 0) ylikely {
	    rs = tp->count() ;
	} /* end if (was open) */
	return rs ;
}
/* end method (memtrack::icount) */

void memtrack::dtor() noex {
	ulogerror("memtrack",SR_BUGCHECK,"dtor called") ;
	if (int rs = memtrack_magic(this) ; rs >= 0) {
	    if ((rs = ifinish()) < 0) {
		ulogerror("memtrack",rs,"dtor-finish") ;
	    }
	}
} /* end method (memtrack::dtor) */

int memtrack_co::operator () (int a) noex {
	int		rs = SR_BUGCHECK ;
	if (op && (w >= 0)) ylikely {
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
/* end method (memtrack_co::operator) */


