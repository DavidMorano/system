/* ucsys_sup SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* USYS support */
/* version %I% last-modified %G% */


/* revision history:

	= 2001-04-11, David A-D- Morano
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
#include	<unistd.h>
#include	<cerrno>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<usysbase.h>
#include	<localmisc.h>

#include	"ucsys_sup.hh"


/* local defines */


/* external subroutines */


/* external variables */


/* local variables */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

namespace ucsys {
    errno_t getresolvec(int herr) noex {
	errno_t		ec = 0 ;
	switch (herr) {
	case NETDB_SUCCESS:
	    break ;
	case NETDB_NOTFOUND:
	    ec = ENOENT ;
	    break ;
	case NETDB_TRYAGAIN:
	    ec = EAGAIN ;
	    break ;
	case NETDB_NORECOVERY:
	    ec = ENOTRECOVERABLE ;
	    break ;
	case NETDB_NOADDRESS:
	    ec = EAFNOSUPPORT ;
	    break ;
	case NETDB_INTERNAL:
	    ec = errno ;		/* suggested from Solaris® */
	    break ;
	default:
	    ec = EPROTONOSUPPORT ;
	    break ;
	} /* end switch */
	return ec ;
    } /* end subroutine (getresolvec) */
} /* end namespace (ucsys) */


