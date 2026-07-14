/* stdintx SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* extended integer operations */
/* version %I% last-modified %G% */


/* revision history:

	= 2001-03-24, David A­D­ Morano
	This code is newly written but modeled after some code that
	was originally written to get some standardized variable
	serialization for the Ethernet-Controller (ETHCON) development
	work.

*/

/* Copyright © 2001 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Group:
	abs
	llabs

	Description:
	Thanke the absolute value of am (extended) integer.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<localmisc.h>

#include	"stdintx.h"


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

longlong llabs(longlong v) noex attrconst {
    	if (v < 0) v = (neg v) ;
    	return v ;
}  /* end subroutine (llabs) */


