/* inetaddrparse SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* this little thing parses an INET address into parts */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-24, David A­D­ Morano
	This object module was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	inetaddrparse

	Description:
        This subroutine (object?) parses an INET host-address-port
        specification.

	Synopsis:
	int inetaddrparse_load(inetaddrparse *ap,cchar *sp,int sl) noex

	Arguments:
	- ap		pointer to object (to hold results)
	- s		string to parse
	- slen		length of string to parse

	Rerurns:
	<0		bad
	>=0		OK; number of items in specification

	Notes:
	The string is parsed as follows:
		[[<af>:]<host>:]<port>

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<usystem.h>
#include	<strn.h>
#include	<localmisc.h>

#include	"inetaddrparse.h"

import libutil ;

/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int inetaddrparse_load(inetaddrparse *ap,cchar *sp,int sl) noex {
	int		rs = SR_FAULT ;
	int		n = 0 ;
	if (ap && sp) ylikely {
	    if (sl < 0) sl = lenstr(sp) ;
	    rs = memclear(ap) ;
	    n += 1 ;
	    if (cchar *tp ; (tp = strnchr(sp,sl,':')) != nullptr) {
	        int	cl ;
	        cchar	*cp ;
	        n += 1 ;
	        ap->af.sp = sp ;
	        ap->af.sl = intconv(tp - sp) ;
	        ap->host.sp = sp ;
	        ap->host.sl = intconv(tp - sp) ;
	        cp = (tp + 1) ;
	        cl = intconv((sp + sl) - cp) ;
	        if ((tp = strnchr(cp,cl,':')) != nullptr) {
	            n += 1 ;
	            ap->host.sp = cp ;
	            ap->host.sl = intconv(tp - cp) ;
	            ap->port.sp = (tp + 1) ;
	            ap->port.sl = intconv((sp + sl) - (tp + 1)) ;
	        } else {
	            ap->af.sp = nullptr ;
	            ap->af.sl = 0 ;
	            ap->port.sp = cp ;
	            ap->port.sl = cl ;
	        } /* end if */
	    } else {
	        ap->port.sp = sp ;
	        ap->port.sl = sl ;
	    } /* end if */
	} /* end if (non-null) */
	return (rs >= 0) ? n : rs ;
}
/* end subroutine (inetaddrparse_load) */


