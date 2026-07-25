/* wsnlen SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* calculate the number of columns used by a wide-string */
/* version %I% last-modified %G% */


/* revision history:

	= 2009-04-10, David A-D- Morano
	This code was originally written.

	= 2018-10-18, David A-D- Morano
	Fix for proper current column.

*/

/* Copyright © 2009,2018 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	wsnlen

	Description:
	This subroutine calculates the number of wide-characters
	in the given wide-character string.

	Synopsis:
	int wsnlen(const wchar_t *wsp,int wsl) noex

	Arguments:
	wsp		wide-string pointer
	wsl		wide-string length (in characters)

	Returns:
	>=0		number of wide-characters in wide-character string
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* CSTD + |wchar_t| */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */
#include	<usysrets.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

#include	"wsnx.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int wsnlen(const wchar_t *wsp,int wsl) noex {
    	int		nc = -1 ; /* return-value */
	if (wsp) {
	    nc = 0 ;
	    if (wsl >= 0) {
	        while (wsl-- && *wsp++) {
		    nc += 1 ;
	        }
	    } else {
	        while (*wsp++) {
		    nc += 1 ;
	        }
	    }
	} /* end if (non-null) */
	return nc ;
} /* end subroutine (wsnlen) */


