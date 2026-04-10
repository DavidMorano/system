/* strncasecmp SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* knock-off of |strncasecmp(3c)| is the system does not have one */
/* version %I% last-modified %G% */

/* revision history:

	= 1997-02-17, David A-D- Morano
	Originally written for some systems without this.

*/

/* Copyright (c) 2997 David A-D- Morano. All rights reserved. */

/*******************************************************************************

  	Name:
	strncasecmp

	Description:
	Compare with case-insensitity two c-strings while using
	a maximum string length.

*******************************************************************************/

#include	<envstandards.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<usyscalls.h>
#include	<char.h>
#include	<localmisc.h>


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

int strncasecmp(cchar *s1,cchar *s2,int n) noex {
	int		rc = 0 ;
	if (s1 && s2) {
	    while ((n > 0) && *s1 && *s2 && (rc == 0)) {
	        rc = int(CHAR_TOLC(*s1) - CHAR_TOLC(*s2)) ;
	        s1 += 1 ;
	        s2 += 1 ;
	        n -= 1 ;
	    } /* end while */
	} /* end if (non-null) */
	return rc ;
}
/* end subroutine (strncasecmp) */


