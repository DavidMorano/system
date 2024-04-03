/* sncpyarray SUPPORT */
/* lang=C++20 */

/* string-copy-array-of-strings */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was originally written.

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

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<climits>		/* |INT_MAX| */
#include	<usystem.h>
#include	<storebuf.h>

#include	"snx.h"


/* local defines */


/* external variables */


/* exported subroutines */

int sncpyarray(char *dbuf,int dlen,cchar **a,int n) noex {
	int		rs = SR_FAULT ;
	int		si = 0 ;
	if (dbuf && a) {
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


