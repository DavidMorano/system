/* sfbreak SUPPORT */
/* lang=C20 */

/* subroutine to find a sub-string in a given string */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-23, David A­D­ Morano
	This subroutine was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	sfbreak

	Description:
	This subroutine determines if any of the characters from
	the parameter string (argument 's2') are found in the primary
	string buffer specified.

	Synopsis:
	int sfbreak(cchar *sp,int sl,cchar *ss,cchar **rpp) noex

	Arguments:
	sp	string to be examined
	sl 	length of string to be examined
	sb	null-terminated string of break characters to break on
	rpp	result pointer of beginning of found break=string

	Returns:
	>=0	length of the break-string
	<0	(will be '-1') no characters from string 'sb' present in 'sp'

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<string.h>		/* <- for |strlen(3c)| */
#include	<utypedefs.h>
#include	<clanguage.h>

#include	"sfx.h"


/* local defines */


/* exported variables */


/* exported subroutines */

int sfbreak(cchar *sp,int sl,cchar *sb,cchar **rpp) noex {
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
/* end subroutine (sfbreak) */


