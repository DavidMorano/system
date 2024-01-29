/* strval SUPPORT */
/* lang=C++20 */

/* String-Value */
/* version %I% last-modified %G% */


/* revision history:

	= 2011-01-25, David A­D­ Morano
	This code was seperated out for for more modularity.  This
	was in turn needed to fix the AST-code sockets library
	definition problems (see notes elsewhere).

	= 2017-08-10, David A­D­ Morano
	This subroutine was borrowed to code MFSERVE.

*/

/* Copyright © 2011,2017 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	strval

	Description:
	Create a string out of a decimal number.

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
#include	<usystem.h>
#include	<sncpyx.h>
#include	<ctdec.h>
#include	<localmisc.h>		/* for |DIGBUFLEN| */

#include	"strval.h"


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

cchar *strval(char *rbuf,cint val) noex {
	cint		rlen = DIGBUFLEN ;
	if ((val >= 0) && (val < INT_MAX)) {
	    if (ctdec(rbuf,rlen,val) < 0) {
	        sncpy(rbuf,rlen,"bad") ;
	    }
	} else {
	    sncpy(rbuf,rlen,"max") ;
	}
	return rbuf ;
}
/* end subroutine (strval) */


