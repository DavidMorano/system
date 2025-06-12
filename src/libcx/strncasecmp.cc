/* strncasecmp SUPPORT */
/* lang=C++20 */

/* knock-off of |strncasecmp(3c)| is the system does not have one */
/* version %I% last-modified %G% */

/* revision history:

	= 1997-02-17, David A-D- Morano
	Originally written for some systems without this.

*/

/* Copyright (c) 2997 David A-D- Morano. All rights reserved. */

#include	<envstandards.h>
#include	<sys/types.h>
#include	<utypedefs.h>
#include	<clanguage.h>
#include	<char.h>


/* exported subroutines */

int strncasecmp(const char *s1,const char *s2,int n) noex {
	int		rc = 0 ;
	while ((n > 0) && *s1 && *s2 && (rc == 0)) {
	    rc = (CHAR_TOLC(*s1) - CHAR_TOLC(*s2)) ;
	    s1 += 1 ;
	    s2 += 1 ;
	    n -= 1 ;
	} /* end while */
	return rc ;
}
/* end subroutine (strncasecmp) */


