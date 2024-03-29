/* strwset SUPPORT */
/* lang=C++20 */

/* set a string to a number of characters */
/* version %I% last-modified %G% */


/* revision history:

	= 2000-05-14, David A�D� Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright � 2000 David A�D� Morano.  All rights reserved. */

/*******************************************************************************

	Copy a specified character to a destination for the number of
	characters specified.  The resulting string is NUL-terminated.

	Synopsis:

	char *strwset(char *dp,int ch,int w) noex 

	Arguments:
	dp	c-string buffer pointer
	ch	character to set in the destination string
	w	the maximum length to be copied

	Returns:
	-	the character pointer to the end of the destination

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<utypedefs.h>
#include	<clanguage.h>

#include	"strwset.h"


/* local defines */


/* exported variables */


/* exported subroutines */

char *strwset(char *dp,int ch,int w) noex {
	while (w-- > 0) *dp++ = ch ;
	*dp = '\0' ;
	return dp ;
}
/* end subroutine (strwset) */


