/* vecstrx_addsyms SUPPORT */
/* encoding=ISO8859-1 */
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
	(sei-permanent dynamic allocation) in place for now.  What am
	I doing messing with code that has worked well for ... decades?
	Am I suddenly crazy?  Enjoy.

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
#include	<cstring>		/* for |strlen(3c)| */
#include	<usystem.h>
#include	<getbufsize.h>
#include	<estrings.h>
#include	<localmisc.h>

#include	"vecstrx.hh"


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct vars {
	int	symlen ;
	char	*symbuf ;
	vars() noex = default ;
	operator int () noex ;
	~vars() ;
    } ;
}


/* forward references */


/* local variables */

static vars		var ;


/* exported variables */


/* exported subroutines */

int vecstrx::addsyms(cc *objn,mainv syms) noex {
    	static cint	rsv = var ;
	int		rs ;
	int		c = 0 ;
	if ((rs = rsv) >= 0) {
	    while ((rs >= 0) && syms[c]) {
	        cchar	*sn = syms[c++] ;
                if ((rs = sncpy(var.symbuf,var.symlen,objn,"_",sn)) >= 0) {
                    rs = add(var.symbuf,rs) ;
                }
            } /* end while */
	} /* end if (vars) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (vecstrx_addsyms) */


/* private subroutines */

vars::operator int () noex {
    	int		rs ;
	if ((rs = getbufsize(getbufsize_sn)) >= 0) {
	    symlen = rs ;
	    if (char *p ; (rs = uc_libmalloc((symlen + 1),&p)) >= 0) {
		symbuf = p ;
	    }
	}
	return rs ;
}

vars::~vars() {
    	if (symbuf) {
    	    uc_libfree(symbuf) ;
	    symbuf = nullptr ;
	    symlen = 0 ;
	}
}


