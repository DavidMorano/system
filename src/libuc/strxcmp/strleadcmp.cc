/* strleadcmp SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

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
#include	<strings.h>		/* BSD |strcasecmp(3c)| */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstring>		/* CSTD |strchr(3c)| */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */
#include	<mkchar.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */
#include	<cmporders.h>		/* LIBU */

#include	"strleadcmp.h"


/* local defines */


/* local namespaces */


/* local typedefs */

typedef cmporders	co ;


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int strleadcmp(cchar *s1,cchar *s2) noex {
	int		rc = 0 ;
	if (s1 && s2) ylikely {
	    while (*s2) {
		cint ch1 = mkchar(*s1++) ;
		cint ch2 = mkchar(*s2++) ;
	        rc = ch1 - ch2 ;
		if (rc) break ;
	    } /* end while */
	} /* end if (non-null) */
	return rc ;
} /* end subroutine (strleadcmp) */

int strxleadcmp(cchar *s1,cchar *s2) noex {
    	int		rc = 0 ;
	if (s1 || s2) ylikely {
	    rc = +1 ;
	    if (s1) {
		rc = -1 ;
		if (s2) {
		    rc = strleadcmp(s1,s2) ;
		}
	    }
	}
	return rc ;
} /* end subroutine (strxleadcmp) */


