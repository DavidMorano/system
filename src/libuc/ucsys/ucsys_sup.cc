/* ucsys_sup */
/* lang=C++20 */

/* USYS support */
/* version %I% last-modified %G% */


/* revision history:

	= 2001-04-11, David D-A- Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright (c) 2001 David A�D� Morano.  All rights reserved. */

/******************************************************************************* 
	These subroutines perform character case conversions (cheaply).  Yes,
	case-conversion is used a lot in embedded systems!

	Implementation note:

        The "CHAR(3dam)' translations (conversions) return an unsigned character
        ('uchar') type. This means that when cajoled into becoming an 'int' type
        on return, it will not be sign-extended.

	= 2014-01-13
        We now use the 'CHAR(3dam)' translation tables only (no computed
        conversion compile-time option). Deal with it. See my note above under
        the revision history. Look how trivial the code is now!

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<clanguage.h>
#include	<usysrets.h>

#include	"ucsys.h"


/* local defines */


/* external subroutines */


/* external variables */


/* forward references */


/* local variables */


/* exported subroutines */

int ucsys_getec(int herr) noex {
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
	default:
	    ec = EBUGCHECK ;
	    break ;
	} /* end switch */
	return ec ;
}
/* end subroutine (ucsys_getec) */


