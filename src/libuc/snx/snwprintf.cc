/* snwprintf SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* perform a c-string creation function similar to |snprintf(3c)| */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A�D� Morano
	This code was originally written.

*/

/* Copyright � 1998 David A�D� Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	snwprinf
	vsnwprintf

	Description:
	Create a c-string according to some formatting rules.

	Synopsis:
	int snwprintf(char *rbuf,int rlen,cchar *fmt,...) noex
	int snwvprintf(char *rbuf,int rlen,cchar *fmt,va_list *ap) noex

	Arguments:
	rbuf		c-string buffer pointer
	rlen		c-string buffer length
	fmt		c-string specifying format conversions
	ap		of type |va_list| for variatic arguments

	Returns:
	>=0		length of written c-string to result buffer
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
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdarg>
#include	<cstdio>
#include	<algorithm>		/* |min(3c++)| + |mac(3c++)| */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<localmisc.h>

#include	"snx.h"


/* local defines */


/* imported namespaces */

using std::nullptr_t ;			/* type */
using std::min ;			/* subroutine-template */
using std::max ;			/* subroutine-template */
using std::nothrow ;			/* constant */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int snwvprintf(char *rbuf,int rlen,cchar *fmt,va_list ap) noex {
	int		rs = SR_FAULT ;
	int		len = 0 ;
	if (rbuf && fmt && ap) {
	    rs = SR_INVALID ;
	    if ((rlen >= 0) && fmt[0]) {
		csize	rsize = size_t(rlen) ;
		if ((rs = vsnprintf(rbuf,(rsize+1),fmt,ap)) >= 0) {
		    len = min(rs,rlen) ;
		} else {
		    rs = SR_BADFMT ;
		}
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (snwvprintf) */

int snwprintf(char *rbuf,int rlen,cchar *fmt,...) noex {
	va_list		ap ;
	int		rs = SR_FAULT ;
	if (rbuf && fmt) {
	    rs = SR_INVALID ;
	    if ((rlen >= 0) && fmt[0]) {
		va_begin(ap,fmt) ;
		rs = snwvprintf(rbuf,rlen,fmt,ap) ;
		va_end(ap) ;
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (snwprintf) */


