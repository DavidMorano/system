/* strval SUPPORT */
/* charset=ISO8859-1 */
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
	Create a string out of a decimal number (given as an 32-bit
	integer) and return a pointer to the given buffer (which
	is also supplied by the caller).  The integer is converted
	to decimal digits assuming a base-10 representation.  This
	kind of function is used in special circumstances where an
	economy of code is valued over other considerations (like
	debugging code, for example).

	Synopsis:
	cchar *strval(char *rbuf,int v) noex

	Arguments:
	rbuf		temporary buffer to store resuling conversion
	v		value to represent (convert to digits)

	Returns:
	-		pointer to temporary buffer w/ value representation

	Notes:
	1. Why the incredible fuss over such a simple function?  I
	guess (because I hardly know myself) that the reason is to
	take up as little buffer space (which is also copletely
	supplied by the caller anyway) on the stack, where presumably
	the buffer is allocated by the caller.  This function is
	very historical and I do not really know what all the reasons
	were in the establishment of its calling API.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<climits>		/* for |INT_MAX| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<usystem.h>
#include	<strdcpyx.h>
#include	<ctdec.h>
#include	<localmisc.h>		/* for |DECBUFLEN| */

#include	"strval.h"

import uconstants ;			/* |digbufsize(3u)| */

/* local defines */

#define	NUMBASE		10		/* base-10 */
#define	TYPELOG		2		/* |int32_t| */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */

static cint		rlen = digbufsize.bufsize[TYPELOG][NUMBASE] ;


/* exported variables */


/* exported subroutines */

cchar *strval(char *rbuf,cint val) noex {
	if ((val >= 0) && (val >= 0)) {
	    if (ctdec(rbuf,rlen,val) < 0) {
	        strdcpy(rbuf,rlen,"invalid") ;
	    }
	} else {
	    strdcpy(rbuf,rlen,"negative") ;
	}
	return rbuf ;
}
/* end subroutine (strval) */


