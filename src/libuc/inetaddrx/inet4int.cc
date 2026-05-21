/* inet4int SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* convert an INETv4 address (in network form) to an unsigned-integer */
/* version %I% last-modified %G% */


/* revision history:

	= 2003-11-06, David A­D­ Morano
        This subroutine exists to make some sensible version out of the
        combination of |inet_addr(3n)| and |inet_pton(3socket)|.

*/

/* Copyright © 2003 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	inet4int

	Description:
        We convert an INETv4 address (in network form) to a host
        unsigned-integer in network order.

	Synopsis:
	uint inet4int(cvoid *ap,int al) noex

	Arguments:
	ap		pointer to INETv4 address in network form
	al		length of INETv4 address

	Returns:
	-		integer that is the INETv4 address in network order

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<localmisc.h>		/* |UC(3local)| */

#include	"inet4int.h"


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

uint inet4int(cvoid *ap) noex {
	uint		v = 0 ;
	if (ap) ylikely {
	    cchar	*cp = charp(ap) ;
	    for (int i = 0 ; i < 4 ; i += 1) {
	        uint	uv = UC(cp[i]) ;
	        v <<= 8 ;
	        v |= uv ;
	    } /* end for */
	} /* end if (non-null) */
	return v ;
}
/* end subroutine (inet4int) */


