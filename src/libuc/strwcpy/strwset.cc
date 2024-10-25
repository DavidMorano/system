/* strwset SUPPORT */
/* lang=C++20 */

/* set a string to a number of characters */
/* version %I% last-modified %G% */


/* revision history:

	= 2000-05-14, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2000 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	strwset

	Description:
	Copy a specified character to a destination for the number of
	characters specified.  The resulting string is NUL-terminated.

	Synopsis:
	char *strwset(char *dp,int ch,int n) noex 

	Arguments:
	dp	c-string buffer pointer
	ch	character to set in the destination string
	n	number of characters to set

	Returns:
	-	the character pointer to the end of the destination

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<localmisc.h>

#include	"strwset.h"


/* local defines */


/* local namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

char *strwset(char *dp,int ch,int n) noex {
	while (n-- > 0) {
	    *dp++ = ch ;
	}
	*dp = '\0' ;
	return dp ;
}
/* end subroutine (strwset) */


