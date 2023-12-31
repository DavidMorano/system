/* memtrack SUPPORT */
/* lang=C++20 */

/* track memory blocks */
/* version %I% last-modified %G% */


/* revision history:

	= 2011-04-12, David A�D� Morano
	This subroutine was originally written. This is a sort of
	test to replace the previous memory tracking implementation
	inside of the |ucmemalloc(3uc)| facility (so loved).

*/

/* Copyright � 2011 David A�D� Morano.  All rights reserved. */

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
	-		the current process PID

*******************************************************************************/

#include	<envstandards.h>
#include	<sys/types.h>
#include	<unistd.h>
#include	<cstring>
#include	<usystem.h>
#include	<usupport.h>

#include	"memtrack.hh"


/* code comments */

#ifdef	COMMENT
struct memtrack_ent {
	void	*addr ;
	int	size ;
} ;
extern int	memtrack_start(int) noex ;
extern int	memtrack_ins(void *,int size) noex ;
extern int	memtrack_rem(void *) noex ;
extern int	memtrack_get(void *,memtrack_ent *) noex ;
extern int	memtrack_finish() noex ;
#endif /* COMMENT */


/* local defines */


/* local namespaces */


/* local typedefs */


/* external subroutines */


/* local structures */


/* forward references */


/* lcaal variables */

constexpr uint		memtrack_magic = MEMTRACK_MAGIC ;


/* exported subroutines */

int memtrack::ins(cvoid *addr,int size) noex {
	int		rs = SR_NOTOPEN ;
	if (magic == memtrack_magic) {
	    const uintptr_t	a = uintptr_t(addr) ;
	    rs = SR_INVALID ;
	    if (addr && (size > 0)) {
		const ent	e = { addr, size } ;
		rs = t.ins(a,e) ;
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
		rs = t.rem(a) ;
	    } /* end if (valid addr) */
	} /* end if (was open) */
	return rs ;
}
/* end method (memtrack::present) */

int memtrack::present(cvoid *addr) noex {
	int		rs = SR_NOTOPEN ;
	if (magic == memtrack_magic) {
	    const uintptr_t	a = uintptr_t(addr) ;
	    rs = SR_INVALID ;
	    if (addr) {
		ent	e ;
		if ((rs = t.get(a,&e)) >= 0) {
		    rs = e.size ;
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
		if ((rs = t.get(a,&e)) >= 0) {
		    rs = e.size ;
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
	    if ((rs = t.start(n)) >= 0) {
		magic = memtrack_magic ;
	    }
	} /* end if (valid) */
	return rs ;
}
/* end method (memtrack::istart) */

int memtrack::ifinish() noex {
	int		rs = SR_NOTOPEN ;
	if (magic == memtrack_magic) {
	    magic = 0 ;
	    rs = t.finish() ;
	} /* end if (was open) */
	return rs ;
}
/* end method (memtrack::ifinish) */

int memtrack::icount() noex {
	int		rs = SR_NOTOPEN ;
	if (magic == memtrack_magic) {
	    rs = t.count() ;
	} /* end if (was open) */
	return rs ;
}
/* end method (memtrack::icount) */

void memtrack::dtor() noex {
	ulogerror("memtrack",SR_BUGCHECK,"fini-dtor called") ;
	if (magic) {
	    cint	rs = ifinish() ;
	    if (rs < 0) {
		ulogerror("memtrack",rs,"fini-dtor error") ;
	    }
	}
}
/* end method (memtrack::dtor) */

int memtrack_start::operator () (int n) noex { 
	int		rs = SR_BUGCHECK ;
	if (op) {
	    return op->istart(n) ;
	}
	return rs ;
}
/* end method (memtrack_start::operator) */

memtrack_start::operator int () noex { 
	return (*this)() ;
}
/* end method (memtrack_start::operator) */

memtrack_co::operator int () noex {
	int		rs = SR_BUGCHECK ;
	if ((w >= 0) && op) {
	    switch (w) {
	    case memtrackmem_count:
		rs = op->icount() ;
		break ;
	    case memtrackmem_finish:
		rs = op->ifinish() ;
		break ;
	    } /* end switch */
	} /* end if (valid) */
	return rs ;
}
/* end method (memtrack_co::operator int) */


