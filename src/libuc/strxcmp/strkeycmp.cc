/* strkeycmp SUPPORT */
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
	strkeycmp

	Description:
	This subroutine makes a comparison of the key of a string
	that looks like a SHELL variable assignment.  For example,
	consider the following compound string:
		A=the_dog_house
	The 'A' would be the key, and the part 'the_dog_house' is
	the value.

	Synopsis:
	int strkeybasecmp(cchar *e1p,cchar *e2p) noex
	int strkeycasecmp(cchar *e1p,cchar *e2p) noex
	int strkeyfoldcmp(cchar *e1p,cchar *e2p) noex

	Arguments:
	e1p		c-string pointer
	e2p		c-string pointer

	Returns:
	>0		the first key string is greater than the second
	0		the keys of the strings are equal
	<0		the first key is less than the second

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<strings.h>		/* |strcasecmp(3c)| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>		/* |getenv(3c)| */
#include	<cstring>		/* |strchr(3c)| */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<toxc.h>
#include	<mkchar.h>
#include	<localmisc.h>
#include	<cmporders.h>

#include	"strkeycmp.h"


/* local defines */


/* imported namespaces */


/* local typedefs */

typedef cmporders	co ;

extern "C" {
    typedef int (*toxc_f)(int) noex ;
    typedef int (*strcmp_f)(cchar *,cchar *) noex ;
}


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

local inline bool isend(int ch) noex {
    	return (ch == '=') || (ch == '\0') ;
}

local int strkeyxcmp(toxc_f tox,cchar *e1p,cchar *e2p) noex {
	int		rc = 0 ;
	if (e1p && e2p) {
	    while (*e1p && *e2p) {
		cint ch1 = mkchar(*e1p) ;
		cint ch2 = mkchar(*e2p) ;
	        if ((ch1 == '=') || (ch2 == '=')) break ;
	        if (tox(ch1) != tox(ch2)) break ;
	        e1p += 1 ;
	        e2p += 1 ;
	    } /* end while */
	    if (*e1p != *e2p) {
		cint ch1 = mkchar(*e1p) ;
		cint ch2 = mkchar(*e2p) ;
	        if (isend(ch1)) {
		    rc = isend(ch2) ? 0 : (- ch2) ;
	        } else if (isend(ch2)) {
		    rc = (+ ch1) ;
	        } else {
		    rc = tox(ch1) - tox(ch2) ;
		}
	    } /* end if (resolution) */
	} /* end if (non-null) */
	return rc ;
} /* end subroutine (strkeycmp) */

local int strxcmp(strcmp_f fun,cchar *s1,cchar *s2,co fo) noex {
    	int		rc = 0 ;
	if (s1 || s2) {
	    rc = +1 ;
	    if (s1) {
		rc = -1 ;
		if (s2) {
		    if (fo) {
		        rc = (- fun(s1,s2)) ;
		    } else {
		        rc = (+ fun(s1,s2)) ;
		    }
		}
	    }
	}
	return rc ;
} /* end subroutine (strxcmp) */


/* local variables */


/* exported variables */


/* exported subroutines */

int strkeybasecmp(cchar *e1p,cchar *e2p) noex {
	return strkeyxcmp(tobc,e1p,e2p) ;
}

int strkeycasecmp(cchar *e1p,cchar *e2p) noex {
	return strkeyxcmp(touc,e1p,e2p) ;
}

int strkeyfoldcmp(cchar *e1p,cchar *e2p) noex {
	return strkeyxcmp(tofc,e1p,e2p) ;
}


int strkeybasecmpo(cchar *s1,cchar *s2) noex {
	return (+ strkeybasecmp(s1,s2)) ;
}
int strkeybasecmpr(cchar *s1,cchar *s2) noex {
	return (- strkeybasecmp(s1,s2)) ;
}

int strkeycasecmpo(cchar *s1,cchar *s2) noex {
    	return (+ strkeycasecmp(s1,s2)) ;
}
int strkeycasecmpr(cchar *s1,cchar *s2) noex {
    	return (- strkeycasecmp(s1,s2)) ;
}

int strkeyfoldcmpo(cchar *s1,cchar *s2) noex {
    	return (+ strkeyfoldcmp(s1,s2)) ;
}
int strkeyfoldcmpr(cchar *s1,cchar *s2) noex {
    	return (- strkeyfoldcmp(s1,s2)) ;
}


int strkeyxbasecmpo(cchar *s1,cchar *s2) noex {
    	return strxcmp(strkeybasecmp,s1,s2,cmporder_obverse) ;
}
int strkeyxbasecmpr(cchar *s1,cchar *s2) noex {
    	return strxcmp(strkeybasecmp,s1,s2,cmporder_reverse) ;
}

int strkeyxcasecmpo(cchar *s1,cchar *s2) noex {
    	return strxcmp(strkeycasecmp,s1,s2,cmporder_obverse) ;
}
int strkeyxcasecmpr(cchar *s1,cchar *s2) noex {
    	return strxcmp(strkeycasecmp,s1,s2,cmporder_reverse) ;
}

int strkeyxfoldcmpo(cchar *s1,cchar *s2) noex {
    	return strxcmp(strkeyfoldcmp,s1,s2,cmporder_obverse) ;
}
int strkeyxfoldcmpr(cchar *s1,cchar *s2) noex {
    	return strxcmp(strkeyfoldcmp,s1,s2,cmporder_reverse) ;
}


