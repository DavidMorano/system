/* strval SUPPORT */
/* lang=C++20 */

/* String-Value */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-10, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

	= 2011-01-25, David A­D­ Morano
	I changed the code from using |sncpyx(3uc)| to |strdcpyx(3uc)|.
	This change is pretty innocuous normally, but in an overflow
	error condition related to the supplied buffer, the
	|sncpyx(3uc)| group does not gurantee that anything is
	actually written to the destination buffer.  Where the use
	of |strdcpy(3uc)| does not discriminate in that regard and
	writes as much as it can to the destination buffer regardless
	of overflow.  At least that is the behavior of |strdcpyx(3uc)|
	as of this writing.  Enjoy.

*/

/* Copyright © 1998,2011 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	strval

	Description:
	Create a string out of a decimal number and return a pointer
	to the given buffer.  This kind of function is used in
	special circumstances where an economy of code is valued
	over other considerations (like debugging code, for example).

	Synopsis:
	cchar *strval(char *tbuf,int v) noex

	Arguments:
	tbuf		temporary buffer
	v		value to represent

	Returns:
	-		pointer to temporary buffer w/ value representation

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<climits>		/* for |INT_MAX| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<usystem.h>
#include	<strdcpyx.h>
#include	<ctdec.h>
#include	<localmisc.h>		/* for |DIGBUFLEN| */

#include	"strval.h"


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

cchar *strval(char *rbuf,cint val) noex {
	cint		rlen = DIGBUFLEN ;
	if ((val >= 0) && (val < INT_MAX)) {
	    if (ctdec(rbuf,rlen,val) < 0) {
	        strdcpy(rbuf,rlen,"bad") ;
	    }
	} else {
	    strdcpy(rbuf,rlen,"max") ;
	}
	return rbuf ;
}
/* end subroutine (strval) */


