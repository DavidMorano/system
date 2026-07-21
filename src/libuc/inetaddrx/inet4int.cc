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
	I convert an INETv4 address (in network-order form) to a
	host (host-ordered) unsigned-integer.  Here I just interpret
	"network-order" as "big-endian" order.  This subroutine 
	should be essentially almost identical to |ntohl(3c)|.

	Synopsis:
	uint inet4int(cvoid *ap) noex

	Arguments:
	ap		pointer to INETv4 address in network form;
			address in byte order: <33><22><11><00>


	Returns:
	-		integer that is the INETv4 address in network order

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<climits>		/* CSTD |CHAR_BIT| */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU |UC(3u)| */

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
	        v <<= CHAR_BIT ;
	        v |= uv ;
	    } /* end for */
	} /* end if (non-null) */
	return v ;
} /* end subroutine (inet4int) */


