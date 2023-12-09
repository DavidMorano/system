/* strval */
/* lang=C++20 */

/* String-Value */
/* version %I% last-modified %G% */


/* revision history:

	= 2011-01-25, David A­D­ Morano
	This code was seperated out for for more modularity. This
	was in turn needed to fix the AST-code sockets library
	definition problems (see notes elsewhere).

	= 2017-08-10, David A­D­ Morano
	This subroutine was borrowed to code MFSERVE.

*/

/* Copyright © 2011,2017 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Create a string out of a decimal number.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<limits.h>
#include	<stdlib.h>
#include	<string.h>
#include	<usystem.h>
#include	<cfdec.h>
#include	<localmisc.h>


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported subroutines */

char *strval(char *rbuf,int val) noex {
	cint		rlen = DIGBUFLEN ;
	int		rs1 ;
	if ((val >= 0) && (val < INT_MAX)) {
	    rs1 = ctdeci(rbuf,rlen,val) ;
	    if (rs1 < 0)
	        sncpy1(rbuf,rlen,"bad") ;
	} else {
	    sncpy1(rbuf,rlen,"max") ;
	}
	return rbuf ;
}
/* end subroutine (strval) */


