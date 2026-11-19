/* usupport_cmpstrn SUPPORT (Compre-String-Maximum) */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* c-string comparisons w/ maximum length and case variations */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-09-10, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Group:
	cmpstrn

	Description:
	This subroutine performs a c-string (w/ maximum length)
	comparison using three types of comparisons of the characters.
	The three comparison types are:
		1. none			(b)
		2. case-insensitibity	(i)
		3. case-folding		(f)

	Synopsis:
	int cmpstrnbc(cchar *e1p,cchar *e2p,int n) noex
	int cmpstrnic(cchar *e1p,cchar *e2p,int n) noex
	int cmpstrnfc(cchar *e1p,cchar *e2p,int n) noex

	Arguments:
	e1p		c-string first
	e2p		c-string second
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
#include	<usys.h>		/* LIBU |cmpstr(3u)| + |cmpstrn(3u)| */
#include	<localmisc.h>		/* LIBU */
#include	<cmporders.h>		/* LIBU */

#include	"usupport_toxc.hh"
#include	"usupport_cmpstrn.hh"


/* local defines */


/* imported namespaces */

using libu::tobc ;			/* subroutine */
using libu::touc ;			/* subroutine */
using libu::tofc ;			/* subroutine */


/* local typedefs */

typedef cmporders	co ;

extern "C" {
    typedef int (*cmpstrn_f)(cchar *,cchar *,int) noex ;
} /* end extern (C) */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

local int cmpstrnø(cmpstrn_f fun,cchar *s1,cchar *s2,int n,co fo) noex {
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
		    } /* end if (comparision order) */
		}
	    }
	} /* end if */
	return rc ;
} /* end subroutine (strxcmp) */


/* local variables */


/* exported variables */


/* local variables */


/* exported variables */


/* exported subroutines */

namespace libu {
    int cmpstrnbc(cchar *s1,cchar *s2,int n) noex {
    	int		rc = 0 ;
	if (s1 && s2) ylikely {
	    rc = cmpstrn(s1,s2,n) ;
	} /* end if (non-null) */
	return rc ;
    } /* end subroutine (strnnasecmp) */
} /* end namespace (libu) */

namespace libu {
    int cmpstrnic(cchar *e1p,cchar *e2p,int n) noex {
	int		rc = 0 ;
	if (e1p && e2p) ylikely {
	    for (int i = 0 ; (i < n) ; i += 1) {
	        cint ch1 = touc(*e1p++) ;
	        cint ch2 = touc(*e2p++) ;
	        rc = ch1 - ch2 ;
	        if (rc || !ch1 || !ch2) break ;
	    } /* end for */
	} /* end if (non-null) */
	return rc ;
    } /* end subroutine */
} /* end namespace (libu) */

namespace libu {
    int cmpstrnfc(cchar *e1p,cchar *e2p,int n) noex {
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
    } /* end subroutine (cmpstrnfc) */
} /* end namespace (libu) */

namespace libu {
    int cmpstrnbco(cchar *e1p,cchar *e2p,int n) noex {
    	return (+ cmpstrnbc(e1p,e2p,n)) ;
    } /* end subroutine */
    int cmpstrnbcr(cchar *e1p,cchar *e2p,int n) noex {
    	return (- cmpstrnbc(e1p,e2p,n)) ;
    } /* end subroutine */
} /* end namespace (libu) */

namespace libu {
    int cmpstrnico(cchar *e1p,cchar *e2p,int n) noex {
    	return (+ cmpstrnic(e1p,e2p,n)) ;
    } /* end subroutine */
    int cmpstrnicr(cchar *e1p,cchar *e2p,int n) noex {
    	return (- cmpstrnic(e1p,e2p,n)) ;
    } /* end subroutine */
} /* end namespace (libu) */

namespace libu {
    int cmpstrnfco(cchar *e1p,cchar *e2p,int n) noex {
    	return (+ cmpstrnfc(e1p,e2p,n)) ;
    } /* end subroutine */
    int cmpstrnfcr(cchar *e1p,cchar *e2p,int n) noex {
    	return (- cmpstrnfc(e1p,e2p,n)) ;
    } /* end subroutine */
} /* end namespace (libu) */

namespace libu {
    int cmpstrnøbco(cchar *s1,cchar *s2,int n) noex {
    	return cmpstrnø(cmpstrnbc,s1,s2,n,cmporder_obverse) ;
    } /* end subroutine */
    int cmpstrnøbcr(cchar *s1,cchar *s2,int n) noex {
    	return cmpstrnø(cmpstrnbc,s1,s2,n,cmporder_reverse) ;
    } /* end subroutine */
} /* end namespace (libu) */

namespace libu {
    int cmpstrnøico(cchar *s1,cchar *s2,int n) noex {
    	return cmpstrnø(cmpstrnic,s1,s2,n,cmporder_obverse) ;
    } /* end subroutine */
    int cmpstrnøicr(cchar *s1,cchar *s2,int n) noex {
    	return cmpstrnø(cmpstrnic,s1,s2,n,cmporder_reverse) ;
    } /* end subroutine */
} /* end namespace (libu) */

namespace libu {
    int cmpstrnøfco(cchar *s1,cchar *s2,int n) noex {
    	return cmpstrnø(cmpstrnfc,s1,s2,n,cmporder_obverse) ;
    } /* end subroutine */
    int cmpstrnøfcr(cchar *s1,cchar *s2,int n) noex {
    	return cmpstrnø(cmpstrnfc,s1,s2,n,cmporder_reverse) ;
    } /* end subroutine */
} /* end namespace (libu) */


/* local subroutines */


