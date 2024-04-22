/* snrealname SUPPORT */
/* lang=C++20 */

/* string-copy a real-name (consisting of parts) */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	snrealname

	Description:
	This subroutine copies a real-name consisting of string
	parts to a target buffer.

	Synopsis:
	int snrealname(char *rbuf,int rlen,cchar **a,int n) noex

	Arguments:
	rbuf		result buffer
	rlen		size of supplied result buffer
	a		array of string parts
	n		number of string parts

	Returns:
	>=0		resulting length
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<climits>		/* <- for |INT_MAX| */
#include	<usystem.h>
#include	<storebuf.h>

#include	"snx.h"


/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* exported subroutines */

int snrealname(char *dbuf,int dlen,cchar **a,int n) noex {
	int		rs = SR_OK ;
	int		si = 0 ;
	int		c = 0 ;
	if (n < 0) n = INT_MAX ;
	for (int i = 0 ; (rs >= 0) && (i < n) && a[i] ; i += 1) {
	    if (a[i][0] != '\0') {
		if (c++ > 0) {
		    rs = storebuf_chr(dbuf,dlen,si,'.') ;
		    si += rs ;
		}
		if (rs >= 0) {
	    	    rs = storebuf_strw(dbuf,dlen,si,a[i],-1) ;
		    si += rs ;
	        }
	    }
	} /* end for */
	return (rs >= 0) ? si : rs ;
}
/* end subroutine (snrealname) */


