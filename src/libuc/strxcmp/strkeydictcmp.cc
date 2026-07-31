/* strkeydictcmp SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* string key comparison */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-09-10, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	strkeydictcmp

	Description:
	This subroutine makes a comparison of the key of a string
	that looks like a SHELL variable assignment.  For example,
	consider the following compound string:
		A=the_dog_house
	The 'A' would be the key, and the part 'the_dog_house' is
	the value.

	Synopsis:
	int strkeydictcmp(cchar *s1,cchar *s2) noex

	Arguments:
	s1		first string
	s2		second string

	Returns:
	>0		the second key is greater than the first
	0		the keys of the strings are equal
	<0		the first key is less than the second

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<strings.h>		/* CSTD |strcasecmp(3c)| */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstring>		/* CSTD |strchr(3c)| */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */
#include	<dictdiff.h>		/* LIBUC */
#include	<ischarx.h>		/* LIBUC */
#include	<mkchar.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */
#include	<cmporders.h>		/* LIBU */

#include	"strkeycmp.h"


/* local defines */


/* imported namespaces */


/* local typedefs */

typedef cmporders	co ;


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int strkeydictcmp(cchar *s1,cchar *s2) noex {
	int		rc = 0 ;
	if (s1 && s2) ylikely {
            while (*s1 && *s2) {
                if ((*s1 == '=') || (*s2 == '=')) break ;
                if (! isdict(*s1)) {
                    s1 += 1 ;
                    continue ;
                }
                if (! isdict(*s2)) {
                    s2 += 1 ;
                    continue ;
                }
                rc = dictdiff(*s1,*s2) ;
                if (rc != 0) break ;
                s1 += 1 ;
                s2 += 1 ;
            } /* end while */
	    {
		cint ch = mkchar(*s2) ;
                if (rc == ch) rc = 0 ;
	    }
            if (*s1 == '=') rc = (*s2 == '\0') ? 0 : -1 ;
            if (*s2 == '=') rc = (*s1 == '\0') ? 0 : +1 ;
	} /* end if (non-null) */
	return rc ;
} /* end subroutine (strkeydictcmp) */


