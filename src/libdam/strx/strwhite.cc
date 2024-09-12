/* strwhite SUPPORT */
/* lang=C++20 */

/* find the next white space character in a string */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	strwhite

	Description:
	This subroutine will scan a string and return the first
	white-space-like character found.  This subroutine is really
	just a short cut for something like:

		char *strpbrk(s," \v\t\r\n")
		const char	s[] ;

	Synopsis:
	char *strwhite(cchar *s) noex

	Arguments:
	s		string to search in

	Returns:
	NULL		if no white space was found
	!= NULL		the pointer to the first white space character

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<char.h>
#include	<localmisc.h>

#include	"strx.h"


/* local defines */


/* external subroutines */


/* external variables */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

char *strwhite(cchar *s) noex {
	return strpbrk(s," \t\r\n\v") ;
}
/* end subroutine (strwhite) */


