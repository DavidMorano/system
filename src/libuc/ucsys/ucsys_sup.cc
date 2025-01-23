/* ucsys_sup SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* USYS support */
/* version %I% last-modified %G% */


/* revision history:

	= 2001-04-11, David D-A- Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright (c) 2001 David A­D­ Morano.  All rights reserved. */

/******************************************************************************* 
  	Purpose:
	library-support

	Description:
	These subroutines perform character case conversions
	(cheaply).  Yes, case-conversion is used a lot in embedded
	systems!

	Implementation note:
	The |CHAR(3uc)| translations (conversions) return an unsigned
	character ('uchar') type.  This means that when cajoled into
	becoming an 'int' type on return, it will not be sign-extended.

	= 2014-01-13
	We now use the |CHAR(3uc)| translation tables only (no
	computed conversion compile-time option).  Deal with it.  See
	my note above under the revision history.  Look how trivial
	the code is now!

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cerrno>
#include	<climits>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<memx.h>

#include	"ucsys.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local variables */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int ucsys_getresolvec(int herr) noex {
	int	ec = EBUGCHECK ;
	switch (herr) {
	case HOST_NOT_FOUND:
	    ec = ENOENT ;
	    break ;
	case TRY_AGAIN:
	    ec = EAGAIN ;
	    break ;
	case NO_RECOVERY:
	    ec = ENOTRECOVERABLE ;
	    break ;
	case NO_DATA:
	    ec = ENODATA ;
	    break ;
	case NETDB_INTERNAL:
	    ec = EAFNOSUPPORT ;		/* suggested from Solaris® */
	    break ;
	default:
	    ec = EPROTONOSUPPORT ;
	    break ;
	} /* end switch */
	return ec ;
}
/* end subroutine (ucsys_getresolvec) */


