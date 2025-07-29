/* filerec1 SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* implement a map container of blocks (of a given structure) */
/* version %I% last-modified %G% */


/* revision history:

	= 2011-04-12, David A­D­ Morano
	This code was originally written.  Only the introduction
	of C++11 has allowed this (finally), due to the addition
	(in C++11) of |unordered_map(3c++)|.

*/

/* Copyright © 2011 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	filerec

	Description:
	This object implements a set (an un-ordered set) with a key
	consisting of device and inode numbers.

	Symopsis:
	int filerec_start(int n = 0) noex

	Arguments:
	n		suggested starting length

	Returns:
	>=0		ok
	<0		error (system-return)

*******************************************************************************/

module ;

#include	<envstandards.h>	/* ordered first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<new>
#include	<utility>		/* |pair(3c++)| */
#include	<unordered_set>
#include	<usyscalls.h>
#include	<ulogerror.h>
#include	<localmisc.h>

module filerec ;

/* local defines */


/* imported namespaces */

using std::nullptr_t ;			/* type */
using std::unordered_set ;              /* type */
using std::pair ;                       /* type */
using libu::umemallocstrw ;		/* subroutine */
using libu::umemalloc ;			/* subroutine */
using libu::umemfree ;			/* subroutine */
using std::nothrow ;			/* constant */


/* local typedefs */

typedef filerec::stype::iterator	setiter ;


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */


/* local subroutines (methods) */

int filerec::istart(int n) noex {
	cnullptr	np{} ;
	int		rs = SR_INVALID ;
	if (n >= 0) ylikely {
	    if (n == 0) n = filerec_deftablen ;
	    try {
	        rs = SR_NOMEM ;
	        if ((setp = new(nothrow) stype(n)) != np) {
	            rs = SR_OK ;
	        } /* end if (new-stype) */
	    } catch (...) {
		rs = SR_NOMEM ;
	    }
	} /* end if (valid) */
	return rs ;
} /* end method (filerec::istart) */

int filerec::ifinish() noex {
	int		rs = SR_NOTOPEN ;
	int		rs1 ;
	if (setp) ylikely {
	    rs = SR_OK ;
	    {
		rs1 = finents() ;
		if (rs >= 0) rs = rs1 ;
	    }
	    {
	        delete setp ;
	        setp = nullptr ;
	    }
	} /* end if (open) */
	return rs ;
} /* end method (filerec::ifinish) */

int filerec::checkin(custat *sbp,cchar *fn) noex {
	int		rs = SR_FAULT ;
	int		f = false ; /* 0=not_added, 1=added */
	if (sbp && fn) ylikely {
	    const dev_t		dev = sbp->st_dev ;
	    const ino_t		ino = sbp->st_ino ;
	    custime		timod = sbp->st_mtime ;
	    coff		fsize = sbp->st_size ;
	    cmode		fmode = sbp->st_mode ;
	    rs = SR_BUGCHECK ;
	    if (setp) ylikely {
	        filerec_ent	k(dev,ino,fmode) ;
		k.load(timod,fsize) ;
		if (! setp->contains(k)) ylikely {
		    if (cchar *cp ; (rs = umemallocstrw(fn,-1,&cp)) >= 0) {
			k.fname = cp ;
		        try {
	                    pair<setiter,bool>	ret = setp->insert(k) ;
		            rs = SR_OK ;
		            f = ret.second ; /* 0=not_added, 1=added */
		        } catch (...) {
		            rs = SR_NOMEM ;
		        }
		        if (rs < 0) {
			    char *bp = cast_const<charp>(cp) ;
			    umemfree(bp) ;
			    k.fname = nullptr ;
			} /* end if (error) */
		    } /* end if (memory-allocation) */
		} /* end if (already) */
	    } /* end if (initialize) */
	} /* end if (non-null) */
	return (rs >= 0) ? f : rs ;
} /* end method (filerec::checkin) */

int filerec::icount() noex {
	int		rs = SR_BUGCHECK ;
	if (setp) ylikely {
	    csize cnt = setp->size() ;
	    rs = intconv(cnt) ;
	} /* end if (non-null) */
	return rs ;
} /* end method (filerec::icount) */

int filerec::finents() noex {
	int		rs = SR_OK ;
	int		rs1 ;
	{
	    stype::iterator	ite = setp->end() ;
	    for (stype::iterator it = setp->begin() ; it != ite ; ++it) {
		char **fpp = cast_const<charpp>(&it->fname) ;
		char *bp = cast_const<charp>(it->fname) ;
		rs1 = umemfree(bp) ;
		if (rs >= 0) rs = rs1 ;
		*fpp = nullptr ;
	    } /* end for */
	} /* end block */
	return rs ;
} /* end method (filerec::finents) */

void filerec::dtor() noex {
	if (cint rs = finish ; rs < 0) nlikely {
	    ulogerror("filerec",rs,"fini-finish") ;
	}
} /* end method (filerec::dtor) */

filerec::operator int () noex {
    	return icount() ;
} /* end method (filerec::operator) */

int filerec_co::operator () (int a) noex {
	int		rs = SR_BUGCHECK ;
	if (op) ylikely {
	    switch (w) {
	    case filerecmem_start:
	        rs = op->istart(a) ;
	        break ;
	    case filerecmem_finish:
	        rs = op->ifinish() ;
	        break ;
	    case filerecmem_count:
	        rs = op->icount() ;
	        break ;
	    } /* end switch */
	} /* end if (non-null) */
	return rs ;
} /* end method (filerec_co::operator) */


