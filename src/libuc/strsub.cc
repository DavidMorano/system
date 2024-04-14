/* strsub SUPPORT */
/* lang=C++20 */

/* find a substring within a larger string */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-23, David A­D­ Morano
	This subroutine was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

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

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<cstring>
#include	<utypedefs.h>
#include	<clanguage.h>
#include	<localmisc.h>

#include	"strsub.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local variables */


/* exported variables */


/* exported subroutines */

char *strsub(cchar *sp,cchar *ss) noex {
	return strstr(sp,ss) ;
}
/* end subroutine (strsub) */


