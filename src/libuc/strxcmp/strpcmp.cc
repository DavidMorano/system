/* strpcmp SUPPORT (String Prefix Compare) */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

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

	Notes:
	1. 2026-02-07 (David A-D- Morano) I could not find usage
	of this subroutine anywhere within any of the the current
	active code bases (repos).
	2. 2026-02-07 (David A-D- Morano) I just searched two old
	obsoleted code bases and I could not find usage of this
	subroutine in either of them.  This subroutine must be
	(actually) very (very) old and once used before the dawn
	of the modern era.  My guess is that this might have been
	once used in the 1980s or perhaps the very early 1990s
	somewhere (perhaps embedded) and then never used again.
	Though, I am glad (somewhat amused) that it survived the
	journey to see some persual here after all of these decades.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<strings.h>		/* |strcasecmp(3c)| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>		/* |getenv(3c)| */
#include	<cstring>		/* |strchr(3c)| */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<mkchar.h>
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
	if (s1 && s2) {
	    while (*s1 && *s2) {
	        if (*s1 != *s2) break ;
	        s1 += 1 ;
	        s2 += 1 ;
	    } /* end while */
	    /* exact match */
	    if ((*s1 | *s2) != 0) {
	        rc = +1 ;
	        /* no match */
	        if (*s1 != '\0') {
		    /* prefix match */
		    rc = +0 ;		/* <- note */
		    if (*s2 != '\0') {
		        /* no match */
		        cint ch1 = mkchar(*s1) ;
		        cint ch2 = mkchar(*s2) ;
		        rc = (ch1 - ch2) ;
		    } /* end if */
	        } /* end if */
	    } /* end if */
	} /* end if (non-null) */
	return rc ;
}
/* end subroutine (strpcmp) */


