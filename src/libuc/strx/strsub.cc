/* strsub SUPPORT */
/* lang=C++20 */

/* find a substring within a larger string */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-23, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	str{x}sub

	Description:
	These subroutines are similar to |strstr(3c)|.

	Synopsis:
	char *str{x}sub(cchar *sp,cchar *ss) noex

	Arguments:
	{x}	base, case, fold
	sp	string to be examined
	ss	null terminated substring to search for

	Returns:
	-	pointer to found substring
	NULL	substring was not found

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>		/* |strstr(3c)| */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<strn.h>
#include	<localmisc.h>

#include	"strsub.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

char *strbasesub(cchar *sp,cchar *ss) noex {
	return strstr(sp,ss) ;
}
/* end subroutine (strsub) */

char *strcasesub(cchar *sp,cchar *ss) noex {
	return strncasesub(sp,-1,ss) ;
}
/* end subroutine (strcasesub) */

char *strfoldsub(cchar *sp,cchar *ss) noex {
	return strnfoldsub(sp,-1,ss) ;
}
/* end subroutine (strfoldsub) */


