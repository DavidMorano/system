/* addrset SUPPORT */
/* lang=C++20 */

/* track memory addresses */
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
	addrset::start
	addrset::count
	addrset::finish
	addrset::ins
	addrset::rem
	addrset::present
	addrset::get
	addrset::curbegin
	addrset::curenum
	addrset::curend

	addrset::istart
	addrset::icount
	addrset::ifinish

	Description:
	Reack memory blocks.

	Symopsis:
	int addrset::start(int n) noex

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
#include	<unordered_set>
#include	<usystem.h>

#include	"addrset.hh"


/* code comments */

#ifdef	COMMENT
struct addrset_ent {
	void	*addr ;
	int	asize ;
} ;
extern int	addrset_start(int) noex ;
extern int	addrset_ins(void *,int) noex ;
extern int	addrset_rem(void *) noex ;
extern int	addrset_get(void *,addrset_ent *) noex ;
extern int	addrset_finish() noex ;
#endif /* COMMENT */


/* local defines */


/* imported namespaces */

using std::nullptr_t ;			/* type */
using std::unordered_set ;		/* type */
using std::pair ;			/* type */
using std::cerr ;			/* variable */
using std::nothrow ;			/* constant */


/* local typedefs */

typedef unordered_set<addrset_ent>	track ;
typedef unordered_set<addrset_ent> *	trackp ;
typedef track::iterator			setiter ;
typedef track::iterator	 *		setiterp ;
typedef pair<setiter,bool>		setret ;
typedef addrset_cur			cur ;


/* external subroutines */


/* local structures */


/* forward references */


/* lcaal variables */

constexpr uint		addrset_magic = ADDRSET_MAGIC ;


/* exported variables */


/* exported subroutines */

int addrset::ins(cvoid *addr,size_t asize) noex {
	int		rs = SR_NOTOPEN ;
	if (magic == addrset_magic) {
	    trackp	tp = trackp(setp) ;
	    rs = SR_INVALID ;
	    if (addr && (asize > 0)) {
		const ent	e = { addr, asize } ;
		try {
		    setret	ret = tp->insert(e) ;
		    if (ret.second) {
		        rs = SR_OK ;
		    } else {
		        rs = SR_EXISTS ;
		    }
		} catch (...) {
		    rs = SR_NOMEM ;
		}
	    } /* end if (valid addr) */
	} /* end if (was open) */
	return rs ;
}
/* end method (addrset::ins) */

int addrset::rem(cvoid *addr) noex {
	int		rs = SR_NOTOPEN ;
	if (magic == addrset_magic) {
	    trackp	tp = trackp(setp) ;
	    ent		e = { addr, 0uz } ;
	    rs = SR_INVALID ;
	    if (addr) {
		csize r = tp->erase(e) ;
		rs = (r) ? SR_OK : SR_NOEXIST ;
	    } /* end if (valid addr) */
	} /* end if (was open) */
	return rs ;
}
/* end method (addrset::present) */

int addrset::present(cvoid *addr) noex {
	int		rs = SR_NOTOPEN ;
	if (magic == addrset_magic) {
	    trackp	tp = trackp(setp) ;
	    ent		e = { addr, 0uz } ;
	    rs = SR_INVALID ;
	    if (addr) {
		if (bool f ; (f = tp->contains(e)) == true) {
		    rs = SR_OK ;
		} else {
		    rs = SR_NOEXIST ;
		}
	    } /* end if (valid addr) */
	} /* end if (was open) */
	return rs ;
}
/* end method (addrset::present) */

int addrset::get(cvoid *addr,addrset_ent *ep) noex {
	int		rs = SR_NOTOPEN ;
	if (magic == addrset_magic) {
	    trackp	tp = trackp(setp) ;
	    ent		e = { addr, 0uz } ;
	    rs = SR_INVALID ;
	    if (addr) {
		setiter ite = tp->end() ;
		setiter	it = tp->find(e) ;
		if (it != ite) {
		    e = *it ;
		    rs = SR_OK ;
		} else {
		    rs = SR_NOEXIST ;
		}
	    } /* end if (valid addr) */
	    if (ep) *ep = e ;
	} /* end if (magic) */
	return rs ;
}
/* end method (addrset::get) */

int addrset::curbegin(cur *curp) noex {
	int		rs = SR_NOTOPEN ;
	if (magic == addrset_magic) {
	    rs = SR_FAULT ;
	    if (curp) {
	        cnullptr	np{} ;
	        trackp		tp = trackp(setp) ;
	        *curp = {} ;
		rs = SR_NOMEM ;
	        if (setiter *itcp ; (itcp = new(nothrow) setiter) != np) {
		    *itcp = tp->begin() ;
	            if (setiter *itep ; (itep = new(nothrow) setiter) != np) {
		        *itep = tp->end() ;
		        curp->vitcp = itcp ;
		        curp->vitep = itep ;
		        rs = SR_OK ;
		    } /* end if (new-setiter) */
		    if (rs < 0) {
			delete itcp ;
		    }
	        } /* end if (new-setiter) */
	    } /* end if (non-null) */
	} /* end if (magic) */
	return rs ;
}
/* end method (addrset::curbegin) */

int addrset::curend(cur *curp) noex {
	int		rs = SR_NOTOPEN ;
	if (magic == addrset_magic) {
	    rs = SR_FAULT ;
	    if (curp) {
	        setiter		*itcp = setiterp(curp->vitcp) ;
	        setiter		*itep = setiterp(curp->vitep) ;
		delete itcp ;
		delete itep ;
		*curp = {} ;
		rs = SR_OK ;
	    } /* end if (non-null) */
	} /* end if (magic) */
	return rs ;
}
/* end method (addrset::curend) */

int addrset::curenum(cur *curp,ent *ep) noex {
	int		rs = SR_NOTOPEN ;
	if (magic == addrset_magic) {
	    rs = SR_FAULT ;
	    if (curp && ep) {
		rs = SR_NOTOPEN ;
		if (curp->vitcp && curp->vitep) {
	            setiter	*itcp = setiterp(curp->vitcp) ;
	            setiter	*itep = setiterp(curp->vitep) ;
		    {
			setiter	&itc = *itcp ;
			setiter	&ite = *itep ;
			if (itc != ite) {
			    *ep = *itc++ ;
	            	    rs = intsat(ep->asize) ;
			} else {
			    *ep = {} ;
			    rs = SR_OK ;
			}
		    } /* end block */
		} /* end if (open) */
	    } /* end if (non-null) */
	} /* end if (magic) */
	return rs ;
}
/* end method (addrset::curenum) */


/* local subroutines */

int addrset::istart(int n) noex {
	int		rs = SR_INVALID ;
	if (n >= 0) {
	    try {
	        if (trackp tp ; (tp = new(nothrow) track) != nullptr) {
		    setp = tp ;
		    magic = addrset_magic ;
		    rs = SR_OK ;
	        } /* end if (new-mapblock) */
	    } catch (...) {
		rs = SR_NOMEM ;
	    }
	} /* end if (valid) */
	return rs ;
}
/* end method (addrset::istart) */

int addrset::ifinish() noex {
	int		rs = SR_NOTOPEN ;
	if (magic == addrset_magic) {
	    trackp	tp = trackp(setp) ;
	    rs = SR_OK ;
	    {
		delete tp ;
	    }
	    setp = nullptr ;
	    magic = 0 ;
	} /* end if (was open) */
	return rs ;
}
/* end method (addrset::ifinish) */

int addrset::icount() noex {
	int		rs = SR_NOTOPEN ;
	if (magic == addrset_magic) {
	    trackp	tp = trackp(setp) ;
	    rs = int(tp->size()) ;
	} /* end if (was open) */
	return rs ;
}
/* end method (addrset::icount) */

void addrset::dtor() noex {
	ulogerror("addrset",SR_BUGCHECK,"dtor called") ;
	if (magic) {
	    cint	rs = ifinish() ;
	    if (rs < 0) {
		ulogerror("addrset",rs,"dtor-finish") ;
	    }
	}
}
/* end method (addrset::dtor) */

int addrset_co::operator () (int a) noex {
	int		rs = SR_BUGCHECK ;
	if ((w >= 0) && op) {
	    switch (w) {
	    case addrsetmem_start:
		rs = op->istart(a) ;
		break ;
	    case addrsetmem_count:
		rs = op->icount() ;
		break ;
	    case addrsetmem_finish:
		rs = op->ifinish() ;
		break ;
	    default:
		std::unreachable() ;
	    } /* end switch */
	} /* end if (valid) */
	return rs ;
}
/* end method (addrset_co::operator int) */


