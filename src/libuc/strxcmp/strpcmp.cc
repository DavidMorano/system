/* strpcmp SUPPORT */
/* lang=C++20 */

/* compares the second string against the prefix of the first */
/* version %I% last-modified %G% */


/* revision history:

	= 2000-05-14, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2000 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	strpcmp

	Description:
	= String Prefix Compare
	This subroutine compares two strings but only looks at the
	leading part (prefix) of the first string (s1) up to the
	length of the second string (s2).

	Synopsis:
	int strpcmp(cchar *s1,cchar *s2) noex

	Arguments:
	s1	one string
	s2	second string

	Returns:
	>0	the first string is bigger than the second
	0	both strings are equal (as compared)
	<0	first string is less than the second

*******************************************************************************/

#include	<envstandards.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstring>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
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

int strpcmp(cchar *s1,cchar *s2) noex {
	int		rc = 0 ;
	while (*s1 && *s2) {
	    if (*s1 != *s2) break ;
	    s1 += 1 ;
	    s2 += 1 ;
	} /* end while */
	/* exact match */
	if ((*s1 | *s2) != 0) {
	    rc = 1 ;
	    /* no match */
	    if (*s1 != '\0') {
		/* prefix match */
		rc = 0 ;
		if (*s2 != '\0') {
		    /* no match */
		    rc = (*s1 - *s2) ;
		}
	    }
	}
	return rc ;
}
/* end subroutine (strpcmp) */


