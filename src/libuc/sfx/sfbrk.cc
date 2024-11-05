/* sfbrk SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* return a sub-string starting at a break character, if one is found */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-23, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	sfbrk

	Description:
	This subroutine returns a sub-string starting at a break
	character, if one is found within a counted source c-string.

	Synopsis:
	int sfbrk(cchar *sp,int sl,cchar *ss,cchar **rpp) noex

	Arguments:
	sp	string to be examined
	sl 	length of string to be examined
	sb	null-terminated string of break characters to break on
	rpp	result pointer of beginning of found break-string

	Returns:
	>=0	length of the break-string
	<0	(will be '-1') no characters from string 'sb' present in 'sp'

	Notes:
	The resulting 'break-string', if one is found at all, starts
	at the break character itself (not just after the
	break-character).

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstring>		/* <- for |strlen(3c)| */
#include	<utypedefs.h>
#include	<clanguage.h>

#include	"sfx.h"


/* local defines */


/* external subroutines */


/* local variables */


/* exported variables */


/* exported subroutines */

int sfbrk(cchar *sp,int sl,cchar *sb,cchar **rpp) noex {
	int		f = false ;
	if (sl < 0) sl = strlen(sp) ;
	while (sl && *sp) {
	    for (int j = 0 ; sb[j] ; j += 1) {
	        f = (*sp == sb[j]) ;
		if (f) break ;
	    }
	    if (f) break ;
	    sp += 1 ;
	    sl -= 1 ;
	} /* end while */
	if (rpp) {
	    *rpp = (f) ? sp : nullptr ;
	}
	return (f) ? sl : -1 ;
}
/* end subroutine (sfbrk) */


