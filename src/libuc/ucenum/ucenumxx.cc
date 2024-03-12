/* ucenumxx SUPPORT */
/* lang=C++20 */

/* enumerate system XX entries */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-24, David A­D­ Morano
	This object module was morphed from some previous one. I
	do not remember what the previous one was.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	We enumerate (reentrantly and thread safely) user names
	from the system PASSWD database.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>
#include	<unistd.h>
#include	<climits>
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<localmisc.h>

#include	"ucenumxx.h"


/* local defines */


/* local namespaces */

using std::nothrow ;			/* constant */


/* local typedefs */


/* external subroutines */


/* local structures */


/* forward references */

template<typename ... Args>
static inline int ucenumxx_ctor(ucenumxx *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = SR_NOMEM ;
	    op->magic = 0 ;
	    if ((op->fmp = new(nothrow) filemap) != nullptr) {
		rs = SR_OK ;
	    } /* end if (new-filemap) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine ucenumxx_ctor) */

static inline int ucenumxx_dtor(ucenumxx *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	    if (op->fmp) {
		delete op->fmp ;
		op->fmp = nullptr ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine ucenumxx_dtor) */


/* local variables */


/* exported variables */


/* exported subroutines */

int ucenumxxbase::open(cchar *efname) noex {
	int		rs ;
	if ((rs = ucenumxx_ctor(op,efname)) >= 0) {
	    cint	of = O_RDONLY ;
	    csize	max = INT_MAX ;
	    if ((rs = filemap_open(fmp,efname,of,max)) >= 0) {
	        magic = mxx ;
	    }
	    if (rs < 0) {
		ucenumxx_dtor(op) ;
	    }
	} /* end if (ucenumxx_ctor) */
	return rs ;
}
/* end if (ucenumxxbase::open) */

int ucenumxxbase::close() noex {
	int		rs = SR_NOTOPEN ;
	int		rs1 ;
	if (op->magic == mxx) {
	    rs = SR_OK ;
	    {
		rs1 = filemap_close(op->fmp) ;
		if (rs >= 0) rs = rs1 ;
	    }
	    {
		rs1 = ucenumxx_dtor(op) ;
		if (rs >= 0) rs = rs1 ;
	    }
	    op->magic = 0 ;
	} /* end if (magic) */
	return rs ;
} 
/* end subroutine (ucenumxxbase::close) */

int ucenumxxbase::reset() noex {
	int		rs = SR_NOTOPEN ;
	if (op->magic == mxx) {
	    rs = filemap_rewind(op->fmp) ;
	} /* end if (open) */
	return rs ;
}
/* end subroutine (ucenumxxbase::reset) */


