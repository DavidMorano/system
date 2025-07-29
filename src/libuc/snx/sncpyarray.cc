/* sncpyarray SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* string-copy-array-of-strings */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	ancpyarray

	Description:
	This subroutine copies the successive elements of an array
	of strings to the given destination (result) buffer.

	Synopsis:
	int sncpyarray(char *rbuf,int rlen,cchar **a,int n) noex

	Arguments:
	rbuf		result buffer
	rlen		size of supplied result buffer
	a		array of strings
	v		number of strings in array

	Returns:
	>=0		resulting length
	<0		error (system-return)

	See-also:
	snfsflags(3uc)
	snopenflags(3uc)
	snpollflags(3uc)
	snxtilook(3uc)
	sninetaddr(3uc)
	snsigabbr(3uc)
	snabbr(3uc)
	snshellunder(3uc)
	snfilemode(3uc)
	sntid(3uc)
	snerrabbr(3uc)
	snrealname(3uc)
	snloadavg(3uc)
	snkeyval(3uc)
	snwvprintf(3uc)
	snwprintf(3uc)
	snkeyval(3uc)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<climits>		/* |INT_MAX| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<storebuf.h>
#include	<localmisc.h>

#include	"snx.h"


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

int sncpyarray(char *dbuf,int dlen,cchar **a,int n) noex {
	int		rs = SR_FAULT ;
	int		si = 0 ;
	if (dbuf && a) ylikely {
	    rs = SR_OK ;
	    if (n < 0) n = INT_MAX ;
	    for (int i = 0 ; (rs >= 0) && (i < n) && a[i] ; i += 1) {
	        if (a[i][0]) {
	            rs = storebuf_strw(dbuf,dlen,si,a[i],-1) ;
		    si += rs ;
	        }
	    } /* end for */
	} /* end if (non-null) */
	return (rs >= 0) ? si : rs ;
}
/* end subroutine (sncpyarray) */


