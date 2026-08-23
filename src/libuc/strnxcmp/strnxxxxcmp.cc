/* strnxxxxcmp SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* string comparison w/ folded case */
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
#include	<strings.h>		/* BSD |strncasecmp(3c)| */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstring>		/* CSTD |strcmp(3c)| */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */
#include	<stdclib.hh>		/* LIBU |std_strncasecmp(3u)| */
#include	<mkchar.h>		/* LIBU */
#include	<char.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */
#include	<cmporders.h>		/* LIBU */

#include	"strnxxxxcmp.h"


/* local defines */

#define	TOFC(ch)	CHAR_TOFC(ch)
#define	strncasecmp	std_strncasecmp


/* imported namespaces */

using stdclib::std_strncasecmp ;	/* subroutine */


/* local typedefs */

typedef cmporders	co ;

extern "C" {
    typedef int (*strncmp_f)(cchar *,cchar *,int) noex ;
} /* end extern (C) */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

local int strnxcmp(strncmp_f fun,cchar *s1,cchar *s2,int n,co fo) noex {
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
		    }
		}
	    }
	}
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
	    rc = strncmp(s1,s2,slen) ;
	} /* end if (non-null) */
	return rc ;
} /* end subroutine (strnnasecmp) */

int strnfoldcmp(cchar *e1p,cchar *e2p,int n) noex {
	int		rc = 0 ;
	if (e1p && e2p) ylikely {
	    for (int i = 0 ; (i < n) ; i += 1) {
	        cint ch1 = TOFC(*e1p++) ;
	        cint ch2 = TOFC(*e2p++) ;
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

int strncasecmpo(cchar *e1p,cchar *e2p,int n) noex {
    	return (+ strncasecmp(e1p,e2p,n)) ;
} /* end subroutine */

int strncasecmpr(cchar *e1p,cchar *e2p,int n) noex {
    	return (- strncasecmp(e1p,e2p,n)) ;
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
    	return strnxcmp(strncasecmp,s1,s2,n,cmporder_obverse) ;
} /* end subroutine */

int strnxcasecmpr(cchar *s1,cchar *s2,int n) noex {
    	return strnxcmp(strncasecmp,s1,s2,n,cmporder_reverse) ;
} /* end subroutine */

int strnxfoldcmpo(cchar *s1,cchar *s2,int n) noex {
    	return strnxcmp(strnfoldcmp,s1,s2,n,cmporder_obverse) ;
} /* end subroutine */

int strnxfoldcmpr(cchar *s1,cchar *s2,int n) noex {
    	return strnxcmp(strnfoldcmp,s1,s2,n,cmporder_reverse) ;
} /* end subroutine */


/* local subroutines */


