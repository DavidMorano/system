/* naturalwords SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* create some "naturalwords" constants */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was modeled from an original assembly
	language version.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	naturalwords

	Description:
	This subroutine contains constants for managing facilities
	that relate to natural-word processing.

	Synopsis:
	in_addr_t mknaturalwords() noex {

	Arguments:
	-	*none*

	Returns:
	-	the "bad" Internet-V4 address

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

#include	"naturalwords.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| */

/* local defines */


/* local namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */

const naturalwords	naturalword ;


/* exported subroutines */


