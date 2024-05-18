/* dirname SUPPORT */
/* lang=C++20 */

/* get the directory part out of a file name path */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	dirname

	Description:
	This routine returns the directory portion of a file name path.

	Synopsis:

	char *dirname(char *s) noex

	Arguments:
	+	string buffer address

	Returns:
		pointer to directory part of modified string buffer

*******************************************************************************/

#include	<sys/types.h>
#include	<utypedefs.h>
#include	<clanguage.h>


/* external subroutines */

extern "C" {
    extern size_t strlen(const char *) noex ;
}


/* exported subroutines */

char *dirname(char *s) noex {
	int		si ;
	int		sl = strlen(s) ;
/* remove trailing slash characters */
	while ((sl > 0) && (s[sl - 1] == '/')) {
	    sl -= 1 ;
	}
/* find the next previous slash character */
	for (si = sl ; si > 0 ; si -= 1) {
	    if (s[si - 1] == '/') break ;
	}
/* nuke it here */
	s[si - 1] = '\0' ;
	return s ;
}
/* end subroutine (dirname) */


