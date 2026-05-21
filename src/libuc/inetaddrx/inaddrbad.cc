/* inaddrbad SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* create an "inaddrbad" constant */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was modeled from an original assembly
	language version.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	inaddrbad

	Description:
	This subroutine creates a constant value that represents
	a "bad" Internet-V4 address.

	Synopsis:
	in_addr_t mkinaddrbad() noex {

	Arguments:
	-	*none*

	Returns:
	-	the "bad" Internet-V4 address

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<localmisc.h>

#include	"inaddrbad.hh"

#pragma		GCC dependency	"mod/libutil.ccm"

import libutil ;

/* local defines */


/* local namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */


