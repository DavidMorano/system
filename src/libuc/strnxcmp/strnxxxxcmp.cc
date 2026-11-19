/* strnxxxxcmp SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* counted c-string comparisons */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-09-10, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Names:
	strnbasecmp
	strnfoldcmp

	Description:
	This subroutine performs a c-string comparisonn using folded
	case.  This is almost identical to |strcasecmp(3c)| except
	that the comparison is done using folded case.

	Synopsis:
	int strnbasecmp(cchar *e1p,cchar *e2p,int n) noex
	int strnfoldcmp(cchar *e1p,cchar *e2p,int n) noex

	Arguments:
	e1p		first string
	e2p		second string
	n		maximum number of characters to compare

	Returns:
	>0		first c-string is greater than the second
	==0		both c-strings are equal
	<0		first c-string is less than the second

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */
#include	<usupport.h>		/* LIBU */
#include	<mkchar.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */
#include	<cmporders.h>		/* LIBU */

#include	"strnxxxxcmp.h"


/* local defines */


/* imported namespaces */

using libu::tobc ;			/* subroutine */
using libu::touc ;			/* subroutine */
using libu::tofc ;			/* subroutine */
using libu::cmpstrnbc ;


/* local typedefs */

typedef cmporders	co ;

extern "C" {
    typedef int (*cmpstrn_f)(cchar *,cchar *,int) noex ;
} /* end extern (C) */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

local int strnxcmp(cmpstrn_f fun,cchar *s1,cchar *s2,int n,co fo) noex {
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
		    } /* end */
		}
	    }
	} /* end */
	return rc ;
} /* end subroutine (strxcmp) */


/* local variables */


/* exported variables */


/* local variables */


/* exported variables */


/* exported subroutines */

int strnbasecmp(cchar *s1,cchar *s2,int slen) noex {
    	int		rc = 0 ;
	if (s1 && s2) ylikely {
	    rc = cmpstrn(s1,s2,slen) ;
	} /* end if (non-null) */
	return rc ;
} /* end subroutine (strnnasecmp) */

int strnfoldcmp(cchar *e1p,cchar *e2p,int n) noex {
	int		rc = 0 ;
	if (e1p && e2p) ylikely {
	    for (int i = 0 ; (i < n) ; i += 1) {
	        cint ch1 = tofc(*e1p++) ;
	        cint ch2 = tofc(*e2p++) ;
	        rc = ch1 - ch2 ;
	        if (rc || !ch1 || !ch2) break ;
	    } /* end for */
	} /* end if (non-null) */
	return rc ;
} /* end subroutine (strnfoldcmp) */

int strnbasecmpo(cchar *e1p,cchar *e2p,int n) noex {
    	return (+ strnbasecmp(e1p,e2p,n)) ;
} /* end subroutine */

int strnbasecmpr(cchar *e1p,cchar *e2p,int n) noex {
    	return (- strnbasecmp(e1p,e2p,n)) ;
} /* end subroutine */

int cmpstrnbco(cchar *e1p,cchar *e2p,int n) noex {
    	return (+ cmpstrnbc(e1p,e2p,n)) ;
} /* end subroutine */

int cmpstrnbcr(cchar *e1p,cchar *e2p,int n) noex {
    	return (- cmpstrnbc(e1p,e2p,n)) ;
} /* end subroutine */

int strnfoldcmpo(cchar *e1p,cchar *e2p,int n) noex {
    	return (+ strnfoldcmp(e1p,e2p,n)) ;
} /* end subroutine */

int strnfoldcmpr(cchar *e1p,cchar *e2p,int n) noex {
    	return (- strnfoldcmp(e1p,e2p,n)) ;
} /* end subroutine */

int strnxbasecmpo(cchar *s1,cchar *s2,int n) noex {
    	return strnxcmp(strnbasecmp,s1,s2,n,cmporder_obverse) ;
} /* end subroutine */

int strnxbasecmpr(cchar *s1,cchar *s2,int n) noex {
    	return strnxcmp(strnbasecmp,s1,s2,n,cmporder_reverse) ;
} /* end subroutine */

int strnxcasecmpo(cchar *s1,cchar *s2,int n) noex {
    	return strnxcmp(cmpstrnbc,s1,s2,n,cmporder_obverse) ;
} /* end subroutine */

int strnxcasecmpr(cchar *s1,cchar *s2,int n) noex {
    	return strnxcmp(cmpstrnbc,s1,s2,n,cmporder_reverse) ;
} /* end subroutine */

int strnxfoldcmpo(cchar *s1,cchar *s2,int n) noex {
    	return strnxcmp(strnfoldcmp,s1,s2,n,cmporder_obverse) ;
} /* end subroutine */

int strnxfoldcmpr(cchar *s1,cchar *s2,int n) noex {
    	return strnxcmp(strnfoldcmp,s1,s2,n,cmporder_reverse) ;
} /* end subroutine */


/* local subroutines */


