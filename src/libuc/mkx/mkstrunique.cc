/* mkstrunique SUPPORT */
/* lang=C++11 */

/* test whether a string consists of all unique characters */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	mkstrunique

	Description:
	We modify the given sring in place to remove any duplicate
	characters found.

	Synopsis:
	int mkstrunique(char *bp,int bl) noex

	Arguments:
	bp		string to test
	bl		length of string to test

	Returns:
	>=0		length of given string
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<climits>		/* |UCHAR_MAX| */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<usysrets.h>
#include	<mkchar.h>
#include	<localmisc.h>

#include	"mkx.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */

constexpr int		nchars = UCHAR_MAX ;


/* exported variables */


/* exported subroutines */

int mkstrunique(char *bp,int bl) noex {
	int		rl = 0 ;
	if (bl > 1) {
	    bool	seen[nchars] = {} ;
	    while (bl-- && *bp) {
	        cint	ch = mkchar(*bp) ;
		if (!seen[ch]) {
		    bp[rl++] = ch ;
	            seen[ch] = true ;
		}
	        bp += 1 ;
	    } /* end while */
	    bp[rl] = '\0' ;
	} /* end if */
	return rl ;
}
/* end subroutine (mkstrunique) */


/* local subroutines */


