/* strsub */
/* lang=C20 */

/* find a substring within a larger string */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-23, David A�D� Morano
	This subroutine was originally written.

*/

/* Copyright � 1998 David A�D� Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	strsub

	Description:
	This is the same as |strstr(3c)|.

	Synopsis:
	char *strsub(cchar *s,cchar *s2) noex

	Arguments:
	s	string to be examined
	s2	null terminated substring to search for

	Returns:
	-	pointer to found substring
	NULL	substring was not found

*******************************************************************************/

#include	<envstandards.h>
#include	<sys/types.h>
#include	<string.h>
#include	<utypedefs.h>
#include	<clanguage.h>
#include	<localmisc.h>


/* local defines */


/* external subroutines */


/* local variables */


/* exported subroutines */

char *strsub(cchar *sp,cchar *ss) noex {
	return strstr(sp,ss) ;
}
/* end subroutine (strsub) */


