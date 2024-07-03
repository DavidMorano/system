/* sispan SUPPORT */
/* lang=C++20 */

/* find the index of character pass a span of a string */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-23, David A-D- Morano
	This subroutine was originally written.

*/

/* Copyright © 1998 David A-D- Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	sispan

	Description:
	This subroutine returns the number of characters skipped
	in the string due to belonging to the specified character
	class.

	Synopsis:
	int sispan(cchar *sp,int sl,cchar *class) noex

	Arguments:
	sp	string to be examined
	sl	length of string of break character to break on
	class	string of characters defining a character class to skip

	Returns:
	>0	number of characters skipped
	==0	no characters were skipped

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstring>		/* for |strchr(3c)| */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<strnxchr.h>
#include	<ascii.h>
#include	<toxc.h>
#include	<mkchar.h>
#include	<localmisc.h>

#include	"six.h"


/* external subroutines */


/* forward references */


/* exported variables */


/* exported subroutines */

int sispan(cchar *sp,int sl,cchar *strclass) noex {
	int		i ; /* userd afterwards */
	for (i = 0 ; sl-- && sp[i] ; i += 1) {
	    cint	ch = mkchar(sp[i]) ;
	    if (strchr(strclass,ch) != nullptr) break ;
	} /* end for */
	return i ;
}
/* end subroutine (sispan) */


