/* cmpstrkn SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* c-string comparisons w/ maximum length and case variations */
/* string key comparison */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-09-10, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	cmpstrkn

	Description:
	This subroutine makes a comparison of the key of a string
	that looks like a SHELL variable assignment.  For example,
	consider the following compound string:
		A=the_dog_house
	The 'A' would be the key, and the part 'the_dog_house' is
	the value.

	Synopsis:
	int cmpstrkn{x}c(cchar *e1p,cchar *e2p,int n) noex

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
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<algorithm>		/* C++STD |min(3c++)| + |max(3c++)| */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */
#include	<usupport.h>		/* LIBU |toxc(3u)| */
#include	<mkchar.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */
#include	<cmporders.h>		/* LIBU */

#include	"cmpstrkn.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| */

/* local defines */


/* imported namespaces */

using std::min ;			/* subroutine */
using std::max ;			/* subroutine */
using libu::tobc ;			/* subroutine */
using libu::touc ;			/* subroutine */
using libu::tofc ;			/* subroutine */


/* local typedefs */

extern "C" {
    typedef int (*toxc_f)	(int) noex ;
    typedef int (*strncmp_f)	(cchar *,cchar *,int) noex ;
} /* end extern (C) */

typedef cmporders	co ;


/* external subroutines */


/* external variables */


/* local structures */


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
} /* end subroutine */

local int cmpstrknx(toxc_f tox,cchar *e1p,cchar *e2p,int n) noex {
	int		rc = 0 ;
	if (e1p && e2p) ylikely {
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
                    } /* end if */
                } /* end if (resolution) */
	    } /* end block */
	} /* end if (non-null) */
	return rc ;
} /* end subroutine (cmpstrknx) */

local int cmpstrnø(strncmp_f fun,cchar *s1,cchar *s2,int n,co fo) noex {
    	int		rc = 0 ;
	if (s1 || s2) ylikely {
	    rc = +1 ;
	    if (s1) {
		rc = -1 ;
		if (s2) {
		    if (fo) {
		        rc = (- fun(s1,s2,n)) ;
		    } else {
		        rc = (+ fun(s1,s2,n)) ;
		    } /* end if (compare-order) */
		}
	    }
	} /* end if */
	return rc ;
} /* end subroutine (cmpstrnø) */


/* local variables */


/* exported variables */


/* exported subroutines */

int cmpstrknbc(cchar *e1p,cchar *e2p,int n) noex {
	return cmpstrknx(tobc,e1p,e2p,n) ;
} /* end subroutine */

int cmpstrknic(cchar *e1p,cchar *e2p,int n) noex {
	return cmpstrknx(touc,e1p,e2p,n) ;
} /* end subroutine */

int cmpstrknfc(cchar *e1p,cchar *e2p,int n) noex {
	return cmpstrknx(tofc,e1p,e2p,n) ;
} /* end subroutine */

int cmpstrknbco(cchar *s1,cchar *s2,int n) noex {
	return (+ cmpstrknbc(s1,s2,n)) ;
} /* end subroutine */

int cmpstrknbcr(cchar *s1,cchar *s2,int n) noex {
	return (- cmpstrknbc(s1,s2,n)) ;
} /* end subroutine */

int cmpstrknico(cchar *s1,cchar *s2,int n) noex {
    	return (+ cmpstrknic(s1,s2,n)) ;
} /* end subroutine */

int cmpstrknicr(cchar *s1,cchar *s2,int n) noex {
    	return (- cmpstrknic(s1,s2,n)) ;
} /* end subroutine */

int cmpstrknfco(cchar *s1,cchar *s2,int n) noex {
    	return (+ cmpstrknfc(s1,s2,n)) ;
} /* end subroutine */

int cmpstrknfcr(cchar *s1,cchar *s2,int n) noex {
    	return (- cmpstrknfc(s1,s2,n)) ;
} /* end subroutine */

int cmpstrknøbco(cchar *s1,cchar *s2,int n) noex {
    	return cmpstrnø(cmpstrknbc,s1,s2,n,cmporder_obverse) ;
} /* end subroutine */

int cmpstrknøbcr(cchar *s1,cchar *s2,int n) noex {
    	return cmpstrnø(cmpstrknbc,s1,s2,n,cmporder_reverse) ;
} /* end subroutine */

int cmpstrknøico(cchar *s1,cchar *s2,int n) noex {
    	return cmpstrnø(cmpstrknic,s1,s2,n,cmporder_obverse) ;
} /* end subroutine */

int cmpstrknøicr(cchar *s1,cchar *s2,int n) noex {
    	return cmpstrnø(cmpstrknic,s1,s2,n,cmporder_reverse) ;
} /* end subroutine */

int cmpstrknøfco(cchar *s1,cchar *s2,int n) noex {
    	return cmpstrnø(cmpstrknfc,s1,s2,n,cmporder_obverse) ;
} /* end subroutine */

int cmpstrknøfcr(cchar *s1,cchar *s2,int n) noex {
    	return cmpstrnø(cmpstrknfc,s1,s2,n,cmporder_reverse) ;
} /* end subroutine */


