/* snrealname SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* string-copy a real-name (consisting of parts) */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A�D� Morano
	This code was originally written.

*/

/* Copyright � 1998 David A�D� Morano.  All rights reserved. */

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
	snkeval(3uc)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<climits>		/* |INT_MAX| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<usystem.h>
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

int snrealname(char *dbuf,int dlen,mainv a,int n) noex {
	int		rs = SR_FAULT ;
	int		len = 0 ;
	if (dbuf && a) {
	    storebuf	sb(dbuf,dlen) ;
	    int		c = 0 ;
	    rs = SR_OK ;
	    if (n < 0) n = INT_MAX ;
	    for (int i = 0 ; (rs >= 0) && (i < n) && a[i] ; i += 1) {
	        if (a[i][0] != '\0') {
		    if (c++ > 0) {
		        rs = sb.chr('.') ;
		    }
		    if (rs >= 0) {
	    	        rs = sb.strw(a[i]) ;
	            }
	        }
	    } /* end for */
	    len = sb.idx ;
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (snrealname) */


