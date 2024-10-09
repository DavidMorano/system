/* strnset SUPPORT */
/* lang=C++20 */

/* set a string to a number of characters */
/* version %I% last-modified %G% */


/* revision history:

	= 2000-05-14, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2000 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	strnset

	Description:
	Copy a specified character to a destination for the number
	of characters specified.  The resulting string is *not*
	NUL-terminated.

	Synopsis:
	char *strnset(cchar *bp,int bl,int ch) noex

	Arguments:
	sp	destination string buffer (pointer)
	sl	destination string buffer (length)
	ch	character to set in the destination string

	Returns:
	-	the character pointer to the end of the destination

	Note: 
	This subroutine is similar to:
		void *memset(s,ch,n) ;
	except that a pointer to the end of the string is returned
	instead of a pointer to the beginning of the string!


	Name:
	strnsetblanks

	Description:
	Copy blank characters to a destination for the number of
	characters specified.  The resulting string is *not*
	NUL-terminated.

	Synopsis:
	char *strnsetblanks(char *bp,int bl) noex

	Arguments:
	sp	destination string buffer (pointer)
	sl	destination string buffer (length)

	Returns:
	-	the character pointer to the end of the destination


*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>

#include	"strn.h"


/* local defines */


/* local namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

char *strnset(char *bp,int bl,int ch) noex {
	while (bl-- > 0) {
	    *bp++ = ch ;
	}
	return bp ;
}
/* end subroutine (strnset) */


