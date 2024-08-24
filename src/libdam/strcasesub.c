/* strcasestr SUPPORT */
/* lang=C++20 */

/* find a substring within a larger string (case insensitive) */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-23, David A­D­ Morano
	This subroutine was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	strcasestr

	Description:
	This subroutine determines if the parameter string (argument
	's2') is or is not a substring specified by the first
	argument.  This subroutine either returns a pointer to the
	the begining of the found substring or NULL if not found.
	The string comparisons are case insensitive.

	Synopsis:
	char *strcasestr(cchar *s,cchar *s2) noex

	Arguments:
	s	string to be examined
	s2	null terminated substring to search for

	Returns:
	-	pointer to found substring
	NULL	substring was not found

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<localmisc.h>


/* local defines */


/* external subroutines */

extern "C" {
extern char	*strcasestr(cchar *s,cchar *s2) noex ;
}


/* local variables */


/* exported variables */


/* exported subroutines */

char *strcasesub(cchar *s,cchar *s2) noex {
	return strcasestr(s,s2) ;
}
/* end subroutine (strcasesub) */


