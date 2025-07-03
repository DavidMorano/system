/* mkstrunique SUPPORT */
/* charset=ISO8859-1 */
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
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<bitset>		/* |bitset(3c++)| */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<usysrets.h>
#include	<mkchar.h>
#include	<localmisc.h>

#include	"mkx.h"


/* local defines */


/* imported namespaces */


/* local typedefs */

using std::nullptr_t ;			/* type */
using std::bitset ;			/* type */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */

constexpr int		nchars = (UCHAR_MAX + 1) ;


/* exported variables */


/* exported subroutines */

int mkstrunique(char *bp,int bl) noex {
	int		rl = 0 ;
	if (bl > 1) {
	    bitset<nchars>	seen ;
	    while (bl-- && *bp) {
	        cint	ch = mkchar(*bp) ;
		if (!seen[ch]) {
		    bp[rl++] = char(ch) ;
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


