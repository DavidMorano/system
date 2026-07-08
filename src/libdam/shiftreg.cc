/* shiftreg SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* Shift Register of Longs */
/* version %I% last-modified %G% */


/* revistion history:

	= 2001-07-10, David A­D­ Morano
	This code was originally started (for Levo machine deadlock
	detection).

*/

/* Copyright © 2001 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	shiftreg

	Description:
	This object just implements a vanilla shift register (not
	clocked).  It should be used in data analysis applications
	and not as a Levo machine hardware component.  There is no
	levo specific aspects to this object (like there has to be
	for regular Levo machine hardware components).

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<ucmem.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */

#include	"shiftreg.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |memclear(3u)| */

/* local defines */


/* local namespaces */

using libuc::mem ;			/* variable */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

template<typename ... Args>
local inline int shiftreg_magic(shiftreg *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    rs = (op->magval == SHIFTREG_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
} /* end subroutine (shiftreg_magic) */


/* local variables */


/* exported variables */


/* exported subroutines */

int shiftreg_start(shiftreg *op,int n) noex {
    	SHIFTREG	*hop = op ;
	int		rs = SR_FAULT ;
	if (n < 1) n = 1 ;
	if (op) ylikely {
	    cint	sz = n * szof(ulong) ;
	    memclear(hop) ;
	    if (ulong *a ; (rs = mem.mall(sz,&a)) >= 0) ylikely {
		op->regs = a ;
	        op->n = n ;
	        for (int i = 0 ; i < op->n ; i += 1) {
	            op->regs[i] = i ;
	        } /* end for */
	        op->magval = SHIFTREG_MAGIC ;
	    } /* end if (memory-acquire) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine */

int shiftreg_finish(shiftreg *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = shiftreg_magic(op)) >= 0) ylikely {
	    if (op->regs) ylikely {
	        rs1 = mem.free(op->regs) ;
	        if (rs >= 0) rs = rs1 ;
	        op->regs = nullptr ;
	    } /* end if (memory-release) */
	    op->magval = 0 ;
	} /* end if (magic) */
	return rs ;
} /* end subroutine */

int shiftreg_shiftin(shiftreg *op,ulong val) noex {
    	int		rs ;
	if ((rs = shiftreg_magic(op)) >= 0) {
	    op->regs[op->i] = val ;
	    op->i = (op->i + 1) % op->n ;
	} /* end if (magic) */
	return rs ;
} /* end subroutine */

int shiftreg_get(shiftreg *op,int i,ulong *rp) noex {
    	int		rs ;
	if ((rs = shiftreg_magic(op,rp)) >= 0) {
	    rs = SR_NOTFOUND ;
	    if ((i >= 0) && (i < op->n)) {
		*rp = op->regs[i] ;
	    }
	} /* end if (magic) */
	return rs ;
} /* end subroutine (shiftreg_get) */


