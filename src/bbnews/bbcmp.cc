/* bbcmp SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* compare two bulleting board names for equality */
/* version %I% last-modified %G% */


/* revision history:

	= 1994-05-01, David A­D­ Morano
        This code was written from scratch to support the correct operation of
        multiple directories for each newsgroup.

	= 1998-11-22, David A­D­ Morano
        I did some clean-up.

*/

/* Copyright © 1994,1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	bbcmp

	Description:
	This subroutine only has to compare for equality (as opposed
	to an actual comparison of greater-than or less-than) so
	we only return 'TRUE=OK' or 'FALSE=BAD'.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */


/* exported variables */


/* exported subroutines */

int bbcmp(cchar *s1,cchar *s2) noex {
	while (*s1) {
	    if (*s1 != *s2) {

		if (*s1 == '*') 
			return OK ;

	        if ((*s1 != '.') && (*s1 != '/'))
	            return BAD ;

	        if ((*s2 != '.') && (*s2 != '/'))
	            return BAD ;

	    }
	    s1 += 1 ;
	    s2 += 1 ;
	} /* end while */
	return ((*s2 == '\0') ? OK : BAD) ;
} /* end subroutine (bbcmp) */



