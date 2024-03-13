/* isleapyear SUPPORT */
/* lang=C++20 */

/* is the given year a leap year? */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-08-01, David A�D� Morano
	This subroutine was originally written.

*/

/* Copyright � 1998 David A�D� Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	isleapyear

	Description:
	This subroutine determines if the supplied given year is a
	leap year.

	Synopsis:
	bool isleapyear(int) noex

	Arguments:
	yr		current year (ex: 1998)

	Returns:
	-		TRUE or FALSE

	Notes:
	= Remember to account for leap-year:

define	isleap(y) ((((y) % 4) == 0) && (((y) % 100) != 0 || ((y) % 400) == 0))

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<utypedefs.h>
#include	<clanguage.h>

#include	"ismisc.h"


/* local defines */


/* external subroutines */


/* local structures */


/* forward references */


/* local variables */


/* exported subroutines */

bool isleapyear(int y) noex {
	bool	f = ((y % 4) == 0) ;
	f = f && (((y % 100) != 0) || ((y % 400) == 0)) ;
	return f ;
}
/* end subroutine (isleapyear) */

