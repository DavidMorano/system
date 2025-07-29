/* strbasename SUPPORT deprecated */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* get the base file name out of a path */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-07-19, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	strbasename

	Description:
	This routine returns the pointer in the given string of the
	start of the basename portion.

	Note:
	This subroutine is deprecated because it can give an erroneous
	result when the passed (specified) source path c-string ends
	in something like a slash ('/') character.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<localmisc.h>

import libutil ;

/* local defines */


/* exported variables */


/* exported subroutines */

char *strbasename(char *s) noex {
	int		si = 0 ;
	int		sl = lenstr(s) ;
	/* remove trailing slash characters */
	while ((sl > 1) && (s[sl - 1] == '/')) {
	    sl -= 1 ;
	}
	s[sl] = '\0' ;
	/* find the next previous slash character */
	for (si = sl ; si > 0 ; si -= 1) {
	    if (s[si - 1] == '/') break ;
	} /* end for */
	if (s[1] == '\0') si = 0 ;
	return (s + si) ;
}
/* end subroutine (strbasename) */


