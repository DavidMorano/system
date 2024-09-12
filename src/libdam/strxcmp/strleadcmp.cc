/* strleadcmp SUPPORT */
/* lang=C++20 */

/* check if string 's2' is a leading substring of string 's1' */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	strleadcmp

	Description:
	This subroutine returns TRUE if str2 (second argument) is
	an initial substring of str1 (first argument).

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<localmisc.h>

#include	"strxcmp.h"


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

int strleadcmp(cchar *s1,cchar *s2) noex {
	int		f = true ;
	while (*s2) {
	    f = (*s2++ == *s1++) ;
	    if (! f) break ;
	}
	return f ;
}
/* end subroutine (strleadcmp) */


