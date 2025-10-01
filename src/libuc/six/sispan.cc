/* sispan SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* find the index of character pass a span of a string */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-23, David A-D- Morano
	This code was originally written.

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
	sp	c-string to be examined pointer
	sl	c-string to be examined length
	class	string of characters defining a character class to skip

	Returns:
	>0	number of characters skipped
	==0	no characters were skipped
	<0	call-usage-error (should not normally happen)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>		/* |strchr(3c)| */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<mkchar.h>
#include	<localmisc.h>

#include	"sispan.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int sispan(cchar *sp,int sl,cchar *strclass) noex {
	int		i = -1 ; /* return-value */
	if (sp && strclass) {
	    for (i = 0 ; sl-- && sp[i] ; i += 1) {
	        cint	ch = mkchar(sp[i]) ;
	        if (strchr(strclass,ch) != nullptr) break ;
	    } /* end for */
	} /* end if (non-null) */
	return i ;
}
/* end subroutine (sispan) */


