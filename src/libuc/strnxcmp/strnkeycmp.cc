/* strnkeycmp SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* string key comparison */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-09-10, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	strnkeycmp

	Description:
	This subroutine makes a comparison of the key of a string
	that looks like a SHELL variable assignment.  For example,
	consider the following compound string:
		A=the_dog_house
	The 'A' would be the key, and the part 'the_dog_house' is
	the value.

	Synopsis:
	int strnkeycmp(cchar *e1p,cchar *e2p,int n) noex

	Arguments:
	e1p		first string
	e2p		second string
	n		maximum number of characters to compare

	Returns:
	>0		the second key is greater than the first
	0		the keys of the strings are equal
	<0		the first key is less than the second

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<toxc.h>
#include	<mkchar.h>
#include	<localmisc.h>

#include	"strnxcmp.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| */

/* local defines */


/* imported namespaces */

using std::min ;			/* subroutine */
using std::max ;			/* subroutine */


/* local typedefs */

extern "C" {
    typedef int (*toxc_f)(int) noex ;
    typedef int (*strncmp_f)(cchar *,cchar *,int) noex ;
}


/* external subroutines */


/* external variables */


/* local structures */

enum order : uchar {
    order_obv,
    order_rev
} ; /* end enum (order) */


/* forward references */

local inline int getn(cchar *s1,cchar *s2,int n) noex {
	if (n < 0) {
	    cint n1 = lenstr(s1) ;
	    cint n2 = lenstr(s2) ;
	    n = min(n1,n2) ;
	} /* end if */
	return n ;
} /* end subroutine (getn) */

local inline bool isend(int ch) noex {
    	return (ch == '=') || (ch == '\0') ;
}

local int strnkeyxcmp(toxc_f tox,cchar *e1p,cchar *e2p,int n) noex {
	int		rc = 0 ;
	if (e1p && e2p) {
	    if ((n = getn(e1p,e2p,n)) > 0) {
	        for (int i = 0 ; (i < n) && *e1p && *e2p ; i += 1) {
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
	    } /* end block */
	} /* end if (non-null) */
	return rc ;
}
/* end subroutine (strnkeyxcmp) */

local int strnxcmp(strncmp_f fun,cchar *s1,cchar *s2,int n,order forder) noex {
    	int		rc = 0 ;
	if (s1 || s2) {
	    rc = +1 ;
	    if (s1) {
		rc = -1 ;
		if (s2) {
		    if (forder) {
		        rc = (- fun(s1,s2,n)) ;
		    } else {
		        rc = (+ fun(s1,s2,n)) ;
		    }
		}
	    }
	}
	return rc ;
} /* end subroutine (strnxcmp) */


/* local variables */


/* exported variables */


/* exported subroutines */

int strnkeybasecmp(cchar *e1p,cchar *e2p,int n) noex {
	return strnkeyxcmp(tobc,e1p,e2p,n) ;
}

int strnkeycasecmp(cchar *e1p,cchar *e2p,int n) noex {
	return strnkeyxcmp(touc,e1p,e2p,n) ;
}

int strnkeyfoldcmp(cchar *e1p,cchar *e2p,int n) noex {
	return strnkeyxcmp(tofc,e1p,e2p,n) ;
}


int strnkeybasecmpo(cchar *s1,cchar *s2,int n) noex {
	return (+ strnkeybasecmp(s1,s2,n)) ;
}
int strnkeybasecmpr(cchar *s1,cchar *s2,int n) noex {
	return (- strnkeybasecmp(s1,s2,n)) ;
}

int strnkeycasecmpo(cchar *s1,cchar *s2,int n) noex {
    	return (+ strnkeycasecmp(s1,s2,n)) ;
}
int strnkeycasecmpr(cchar *s1,cchar *s2,int n) noex {
    	return (- strnkeycasecmp(s1,s2,n)) ;
}

int strnkeyfoldcmpo(cchar *s1,cchar *s2,int n) noex {
    	return (+ strnkeyfoldcmp(s1,s2,n)) ;
}
int strnkeyfoldcmpr(cchar *s1,cchar *s2,int n) noex {
    	return (- strnkeyfoldcmp(s1,s2,n)) ;
}


int strnkeyxbasecmpo(cchar *s1,cchar *s2,int n) noex {
    	return strnxcmp(strnkeybasecmp,s1,s2,n,order_obv) ;
}
int strnkeyxbasecmpr(cchar *s1,cchar *s2,int n) noex {
    	return strnxcmp(strnkeybasecmp,s1,s2,n,order_rev) ;
}

int strnkeyxcasecmpo(cchar *s1,cchar *s2,int n) noex {
    	return strnxcmp(strnkeycasecmp,s1,s2,n,order_obv) ;
}
int strnkeyxcasecmpr(cchar *s1,cchar *s2,int n) noex {
    	return strnxcmp(strnkeycasecmp,s1,s2,n,order_rev) ;
}

int strnkeyxfoldcmpo(cchar *s1,cchar *s2,int n) noex {
    	return strnxcmp(strnkeyfoldcmp,s1,s2,n,order_obv) ;
}
int strnkeyxfoldcmpr(cchar *s1,cchar *s2,int n) noex {
    	return strnxcmp(strnkeyfoldcmp,s1,s2,n,order_rev) ;
}


