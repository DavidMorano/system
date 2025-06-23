/* bvsbook SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* manage a BVS "book" entry */
/* version %I% last-modified %G% */


/* revision history:

	= 2008-10-01, David A­D­ Morano
	This module was originally written.

*/

/* Copyright © 2008 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	bvsbook

	Description:
	This module manages a BVS "book" entry.

	Synopsis:
	int bvsbook_get(BVSBOOK *bep,ushort *a) noex

	Arguments:
	- bep		book entry pointer
	- a		entry array buffer

	Returns:
	>=0		OK
	<0		error code (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* must be before others */
#include	<climits>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<localmisc.h>

#include	"bvsbook.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int bvsbook_set(bvsbook *bep,ushort *a) noex {
    	BVSBOOK		*hop = bep ;
    	int		rs = SR_FAULT ;
	if (bep && a) {
	    rs = memclear(hop) ;
	    a[0] = bep->al ;
	    a[1] = bep->ci ;
	    a[2] = bep->nverses ;
	    a[3] = bep->nzverses ;
	}
	return rs ;
}
/* end subroutine (bvsbook_set) */

int bvsbook_get(bvsbook *bep,ushort *a) noex {
    	int		rs = SR_FAULT ;
	if (bep && a) {
	    rs = SR_OK ;
	    bep->al = (uchar) a[0] ;
	    bep->ci = a[1] ;
	    bep->nverses = a[2] ;
	    bep->nzverses = a[3] ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (bvsbook_get) */


