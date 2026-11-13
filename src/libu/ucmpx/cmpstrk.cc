/* cmpstrk SUPPORT */
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

	Group:
	cmpstrk

	Description:
	This subroutine makes a comparison of the key of a string
	that looks like a SHELL variable assignment.  For example,
	consider the following compound string:
		A=the_dog_house
	The 'A' would be the key, and the part 'the_dog_house' is
	the value.

	Synopsis:
	int cmpstrkbc(cchar *e1p,cchar *e2p) noex
	int cmpstrkic(cchar *e1p,cchar *e2p) noex
	int cmpstrkfc(cchar *e1p,cchar *e2p) noex

	Arguments:
	e1p		c-string pointer
	e2p		c-string pointer

	Returns:
	>0		the first key string is greater than the second
	0		the keys of the strings are equal
	<0		the first key is less than the second

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */
#include	<usupport.h>		/* LIBU |toxc(3u)| */
#include	<mkchar.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */
#include	<cmporders.h>		/* LIBU */

#include	"cmpstrk.h"


/* local defines */


/* imported namespaces */

using libu::tobc ;			/* subroutine */
using libu::touc ;			/* subroutine */
using libu::tofc ;			/* subroutine */


/* local typedefs */

typedef cmporders	co ;

extern "C" {
    typedef int (*toxc_f)	(int) noex ;
    typedef int (*strcmp_f)	(cchar *,cchar *) noex ;
} /* end extern (C) */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

local inline bool isend(int ch) noex {
    	return (ch == '=') || (ch == '\0') ;
} /* end subroutine */

local int cmpstrkx(toxc_f tox,cchar *e1p,cchar *e2p) noex {
	int		rc = 0 ;
	if (e1p && e2p) ylikely {
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
		} /* end */
	    } /* end if (resolution) */
	} /* end if (non-null) */
	return rc ;
} /* end subroutine (strkeycmp) */

local int cmpstrx(strcmp_f fun,cchar *s1,cchar *s2,co fo) noex {
    	int		rc = 0 ;
	if (s1 || s2) ylikely {
	    rc = +1 ;
	    if (s1) {
		rc = -1 ;
		if (s2) {
		    if (fo) {
		        rc = (- fun(s1,s2)) ;
		    } else {
		        rc = (+ fun(s1,s2)) ;
		    } /* end if (comparison-order) */
		}
	    }
	} /* end if */
	return rc ;
} /* end subroutine (cmpstrx) */


/* local variables */


/* exported variables */


/* exported subroutines */

int cmpstrkbc(cchar *e1p,cchar *e2p) noex {
	return cmpstrkx(tobc,e1p,e2p) ;
} /* end subroutine */

int cmpstrkic(cchar *e1p,cchar *e2p) noex {
	return cmpstrkx(touc,e1p,e2p) ;
} /* end subroutine */

int cmpstrkfc(cchar *e1p,cchar *e2p) noex {
	return cmpstrkx(tofc,e1p,e2p) ;
} /* end subroutine */

int cmpstrkbco(cchar *s1,cchar *s2) noex {
	return (+ cmpstrkbc(s1,s2)) ;
} /* end subroutine */

int cmpstrkbcr(cchar *s1,cchar *s2) noex {
	return (- cmpstrkbc(s1,s2)) ;
} /* end subroutine */

int cmpstrkico(cchar *s1,cchar *s2) noex {
    	return (+ cmpstrkic(s1,s2)) ;
} /* end subrouine */

int cmpstrkicr(cchar *s1,cchar *s2) noex {
    	return (- cmpstrkic(s1,s2)) ;
} /* end subroutine */

int cmpstrkfco(cchar *s1,cchar *s2) noex {
    	return (+ cmpstrkfc(s1,s2)) ;
} /* end subroutine */

int cmpstrkfcr(cchar *s1,cchar *s2) noex {
    	return (- cmpstrkfc(s1,s2)) ;
} /* end subroutine */

int strkeyxbasecmpo(cchar *s1,cchar *s2) noex {
    	return cmpstrx(cmpstrkbc,s1,s2,cmporder_obverse) ;
} /* end subrouine */

int strkeyxbasecmpr(cchar *s1,cchar *s2) noex {
    	return cmpstrx(cmpstrkbc,s1,s2,cmporder_reverse) ;
} /* end subroutine */

int cmpstrkøico(cchar *s1,cchar *s2) noex {
    	return cmpstrx(cmpstrkic,s1,s2,cmporder_obverse) ;
} /* end subrouine */

int cmpstrkøicr(cchar *s1,cchar *s2) noex {
    	return cmpstrx(cmpstrkic,s1,s2,cmporder_reverse) ;
} /* end subroutine */

int cmpstrkøfco(cchar *s1,cchar *s2) noex {
    	return cmpstrx(cmpstrkfc,s1,s2,cmporder_obverse) ;
} /* end subroutine */

int cmpstrkøfcr(cchar *s1,cchar *s2) noex {
    	return cmpstrx(cmpstrkfc,s1,s2,cmporder_reverse) ;
} /* end subroutine */


