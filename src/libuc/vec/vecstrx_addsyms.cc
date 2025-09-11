/* vecstrx_addsyms SUPPORT */
/* charset=ISO8859-1 */
/* lang=C20 */

/* add constucted symbols to the (VECSTRX) object */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-09-10, David A­D­ Morano
	This module was originally written.

	= 2025-04-18, David A-D- Morano
	I make the buffer for holding the created symbol name
	dynamic.  I also make the dynamic allocation permanent for
	the lifetime of the loaded module (that this code may be
	inside of).  Dynamic allocation (and immediate deallocation
	after use) is fine, but what about keeping the allocation
	around for the lifetime of the module?  Was this a good
	idea?  Think about it for a moment (as a temporary opposing
	advocate for evaluation purposes).  The reason that we
	dynamically allocate and then deallocate (immediately)
	afterwards is to take advantage of the statistical multiplexing
	of the use of the heap memory area.  Without (immediate)
	deallocation, we lose this advantage.  OK, I see the reasoning
	(for immediate deallocation) but I will leave this
	(sei-permanent dynamic allocation) in place for now.  What
	am I doing messing with code that has worked well for ...
	decades?  Am I suddenly crazy?  Enjoy.

*/

/* Copyright © 1998,2025 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	vecstrx_addsyms

	Description:
	Add constucted symbols to the object.

	Synopsis:
	int vecstrx_addsyms(vecstrx *vlp,cc *objn,mainv syms) noe

	Arguments:
	op		pointer to VECSTR object
	objn		base-name for constructed symbol
	syms		array of pointees to particular parts of symbols

	Returns:
	>=0		number of elements loaded
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<usyscalls.h>
#include	<usupport.h>		/* |libu::sncpy(3u)| */
#include	<uclibmem.h>
#include	<localmisc.h>

#include	"vecstrx.hh"

#pragma		GCC dependency		"mod/ulibvals.ccm"

import ulibvals ;

/* local defines */


/* imported namespaces */

using libu::sncpy ;			/* subroutine */
using libuc::libmem ;			/* variable */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct vars {
	char		*symbuf ;
	int		symlen ;
	vars() noex = default ;
	operator int () noex ;
	void dtor() noex ;
	destruct vars() {
	    if (symbuf) dtor() ;
	} ;
    } ; /* end struct (vars) */
} /* end namespace */


/* forward references */


/* local variables */

static vars		var ;

static int		symnamelen = ulibval.maxsymbol ;


/* exported variables */


/* exported subroutines */

int vecstrx::addsyms(cc *objn,mainv syms) noex {
	int		rs = SR_FAULT ;
	int		c = 0 ;
	if (objn && syms) {
    	    static cint		rsv = var ;
	    if ((rs = rsv) >= 0) {
	        while ((rs >= 0) && syms[c]) {
	            cchar	*sn = syms[c++] ;
                    if ((rs = sncpy(var.symbuf,var.symlen,objn,"_",sn)) >= 0) {
                        rs = add(var.symbuf,rs) ;
                    }
                } /* end while */
	    } /* end if (vars) */
	} /* end if (non-null) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (vecstrx_addsyms) */


/* private subroutines */

vars::operator int () noex {
    	int		rs ;
	if ((rs = symnamelen) >= 0) {
	    symlen = rs ;
	    if (char *p ; (rs = libmem.mall((symlen + 1),&p)) >= 0) {
		symbuf = p ;
	    }
	}
	return rs ;
} /* end method (vars:operator) */

void vars::dtor() noex {
    	if (symbuf) {
    	    libmem.free(symbuf) ;
	    symbuf = nullptr ;
	    symlen = 0 ;
	}
} /* end method (vars:dtor) */


