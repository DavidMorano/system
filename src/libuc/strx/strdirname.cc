/* strdirname SUPPORT deprecated */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* get the directory part out of a file name path */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-08-27, David A­D­ Morano
	This is a replacement for some systems (UNIX yes, but not
	all others -- hence this code up) that do not have a
	|dirname(3c)| type of subroutine.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	strdirname

	Description:
	This routine returns the directory portion of a file name
	path.

	Synopsis:
	char *strdirname(char *s) noex

	Arguments:
	s	string buffer address

	Returns:
	-	pointer to directory part of modified string buffer

	Notes:
	This subroutine is deprecated because it writes (a NUL
	character) to the passed c-string (dangerous).

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


/* local variables */

static constexpr cchar		strdirname_dot[] = "." ;


/* exported variables */


/* exported subroutines */

char *strdirname(char *s) noex {
	int		si ;
	int		sl = lenstr(s) ;
	/* remove trailing slash characters */
	while ((sl > 1) && (s[sl - 1] == '/')) {
	    sl -= 1 ;
	}
	/* find the next previous slash character (if there is one) */
	for (si = sl ; si > 0 ; si -= 1) {
	    if (s[si - 1] == '/') break ;
	}
	/* nuke it here */
	if (si > 1) {
	    s[si - 1] = '\0' ;
	} else if (si == 1) {
	    s[si] = '\0' ;		/* <- deprecated */
	} else if (si == 0) {
	    s = (char *) strdirname_dot ;
	}
	return s ;
}
/* end subroutine (strdirname) */


