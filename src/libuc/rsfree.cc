/* rsfree SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* special memory-allocation free-up */
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
#include	<usystem.h>
#include	<localmisc.h>

#include	"rsfree.h"


/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int rsfree(int rs,cvoid *vp) noex {
	cint	rs1 = uc_free(vp) ;
	if (rs >= 0) rs = rs1 ;
	return rs ;
}
/* end subroutine (rsfree) */


