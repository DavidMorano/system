/* rsfree SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* special memory-acquire free-up */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	rsfree

	Descriptor:
	This subroutine deallocates a previously memory-allocated
	memory element.

	Synopsis:
	int rsfree(int rs,cvoid *vp) noex

	Arguments:
	rs		previous system-return value
	vp		previously memory-allocated item pointer

	Returns:
	>=0		OK
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<ucmem.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */

#include	"rsfree.h"


/* local defines */


/* imported namespaces */

using libuc::mem ;			/* standard memory-management */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int rsfree(int rs,cvoid *cvp) noex {
	int		rs1 ;
	if (cvp) {
	    void *vp = cast_const<void *>(cvp) ;
	    rs1 = mem.free(vp) ;
	    if (rs >= 0) rs = rs1 ;
	} else {
	    rs = SR_FAULT ;
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (rsfree) */


