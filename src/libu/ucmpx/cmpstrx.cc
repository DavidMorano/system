/* cmpstrx SUPPORT (Compre-String) */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* c-string comparisons w/ case variations */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-09-10, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Group:
	cmpstrx

	Description:
	This subroutine performs a c-string comparison using three
	types of comparisons of the characters.  The three comparison
	types are:
		1. none			(b)
		2. case-insensitibity	(i)
		3. case-folding		(f)

	Synopsis:
	int cmpstr{x}c(cchar *e1p,cchar *e2p) noex

	Arguments:
	{x}		one of: b, i, f
	e1p		c-string pointer first
	e2p		c-string pointer second

	Returns:
	>0		first c-string is greater than the second
	==0		both c-strings are equal
	<0		first c-string is less than the second

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstring>		/* CSTD |strcmp(3c)| */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */
#include	<usupport.h>		/* LIBU |toxc(3u)| */
#include	<localmisc.h>		/* LIBU */
#include	<cmporders.h>		/* LIBU */

#include	"cmpstrx.h"


/* local defines */


/* imported namespaces */

using libu::tobc ;			/* subroutine */
using libu::touc ;			/* subroutine */
using libu::tofc ;			/* subroutine */


/* local typedefs */

typedef cmporders	co ;

extern "C" {
    typedef int (*cmpstr_f)	(cchar *,cchar *) noex ;
    typedef int (*toxc_f)	(int) noex ;
} /* end extern (C) */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

local int cmpstrxc(toxc_f tox,cchar *s1,cchar *s2) noex {
	int		rc = 0 ;
	if (s1 && s2) ylikely {
	    while (rc == 0) {
	        cint ch1 = tox(*s1++) ;
	        cint ch2 = tox(*s2++) ;
	        rc = ch1 - ch2 ;
	        if (!ch1 || !ch2) break ;
	    } /* end while */
	} /* end if (non-null) */
	return rc ;
} /* end subroutine (cmpstrxc) */

local int cmpstrøx(cmpstr_f fun,cchar *s1,cchar *s2,co fo) noex {
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
		    } /* end if (order) */
		}
	    }
	} /* end if */
	return rc ;
} /* end subroutine (cmpstrøx) */


/* local variables */


/* exported variables */


/* exported subroutines */

int cmpstrbc(cchar *s1,cchar *s2) noex {
    	int		rc = 0 ;
	if (s1 && s2) ylikely {
	    rc = strcmp(s1,s2) ;
	}
	return rc ;
} /* end subroutine (cmpstrbc) */
int cmpstric(cchar *s1,cchar *s2) noex {
    	return cmpstrxc(touc,s1,s2) ;
} /* end subroutine (cmpstric) */
int cmpstrfc(cchar *s1,cchar *s2) noex {
    	return cmpstrxc(tofc,s1,s2) ;
} /* end subroutine (cmpstrfc) */

/* base */
int cmpstrbco(cchar *s1,cchar *s2) noex {
	return (+ cmpstrbc(s1,s2)) ;
} /* end subroutine */
int cmpstrbcr(cchar *s1,cchar *s2) noex {
	return (- cmpstrbc(s1,s2)) ;
} /* end subroutine */

/* insensitive */
int cmpstrico(cchar *s1,cchar *s2) noex {
	return (+ cmpstric(s1,s2)) ;
} /* end subroutine */
int cmpstricr(cchar *s1,cchar *s2) noex {
	return (- cmpstric(s1,s2)) ;
} /* end subroutine */

/* fold */
int cmpstrfco(cchar *s1,cchar *s2) noex {
	return (+ cmpstrfc(s1,s2)) ;
} /* end subroutine */
int cmpstrfcr(cchar *s1,cchar *s2) noex {
	return (- cmpstrfc(s1,s2)) ;
} /* end subroutine */

int cmpstrøbco(cchar *s1,cchar *s2) noex {
    	return cmpstrøx(cmpstrbc,s1,s2,cmporder_obverse) ;
} /* end subroutine */
int cmpstrøbcr(cchar *s1,cchar *s2) noex {
    	return cmpstrøx(cmpstrbc,s1,s2,cmporder_reverse) ;
} /* end subroutine */

int cmpstrøico(cchar *s1,cchar *s2) noex {
    	return cmpstrøx(cmpstrbc,s1,s2,cmporder_obverse) ;
} /* end subroutine */
int cmpstrøicr(cchar *s1,cchar *s2) noex {
    	return cmpstrøx(cmpstrbc,s1,s2,cmporder_reverse) ;
} /* end subroutine */

int cmpstrøfco(cchar *s1,cchar *s2) noex {
    	return cmpstrøx(cmpstrfc,s1,s2,cmporder_obverse) ;
} /* end subroutine */
int cmpstrøfcr(cchar *s1,cchar *s2) noex {
    	return cmpstrøx(cmpstrfc,s1,s2,cmporder_reverse) ;
} /* end subroutine */


