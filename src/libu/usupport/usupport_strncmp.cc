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
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usupport.h>		/* LIBU */
#include	<usysdefs.h>		/* LIBU */
#include	<stdclib.hh>		/* STDCLIB |std_strncasecmp(3u)| */
#include	<mkchar.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */
#include	<cmporders.h>		/* LIBU */

#include	"usupport_strncmp.hh"

import ureserve ;			/* |char_{x}(3u)| */

/* local defines */

#define	TOFC(ch)	tofc(ch)


/* imported namespaces */

using stdclib::std_strnbasecmp ;	/* subroutine */
using stdclib::std_strncasecmp ;	/* subroutine */
using libu::tobc ;			/* subroutine */
using libu::touc ;			/* subroutine */
using libu::tofc ;			/* subroutine */


/* local typedefs */

typedef cmporders	co ;

extern "C" {
    typedef int (*strncmp_f)(cchar *,cchar *,int) noex ;
} /* end extern */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

local int strnxcmp(strncmp_f fun,cchar *s1,cchar *s2,int n,co fo) noex {
    	int		rc = 0 ;
	if (s1 || s2) {
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

namespace libu {
    int strnbasecmp(cchar *s1,cchar *s2,int slen) noex {
	return std_strnbasecmp(s1,s2,slen) ;
    } /* end subroutine (strnbasecmp) */
    int strncasecmp(cchar *s1,cchar *s2,int slen) noex {
	return std_strncasecmp(s1,s2,slen) ;
    } /* end subroutine (strncasecmp) */
    int strnfoldcmp(cchar *e1p,cchar *e2p,int n) noex {
	int		rc = 0 ;
	if (e1p && e2p) {
	    for (int i = 0 ; (i < n) ; i += 1) {
	        cint ch1 = TOFC(*e1p++) ;
	        cint ch2 = TOFC(*e2p++) ;
	        rc = ch1 - ch2 ;
	        if (rc || !ch1 || !ch2) break ;
	    } /* end for */
	} /* end if (non-null) */
	return rc ;
    } /* end subroutine (strnfoldcmp) */
} /* end namespace (libu) */

namespace libu {
    int strnbasecmpo(cchar *e1p,cchar *e2p,int n) noex {
    	return (+ strnbasecmp(e1p,e2p,n)) ;
    }
    int strnbasecmpr(cchar *e1p,cchar *e2p,int n) noex {
    	return (- strnbasecmp(e1p,e2p,n)) ;
    }
} /* end namespace (libu) */

namespace libu {
    int strncasecmpo(cchar *e1p,cchar *e2p,int n) noex {
    	return (+ strncasecmp(e1p,e2p,n)) ;
    }
    int strncasecmpr(cchar *e1p,cchar *e2p,int n) noex {
    	return (- strncasecmp(e1p,e2p,n)) ;
    }
} /* end namespace (libu) */

namespace libu {
    int strnfoldcmpo(cchar *e1p,cchar *e2p,int n) noex {
    	return (+ strnfoldcmp(e1p,e2p,n)) ;
    }
    int strnfoldcmpr(cchar *e1p,cchar *e2p,int n) noex {
    	return (- strnfoldcmp(e1p,e2p,n)) ;
    }
} /* end namespace (libu) */

namespace libu {
    int strnxbasecmpo(cchar *s1,cchar *s2,int n) noex {
    	return strnxcmp(strnbasecmp,s1,s2,n,cmporder_obverse) ;
    }
    int strnxbasecmpr(cchar *s1,cchar *s2,int n) noex {
    	return strnxcmp(strnbasecmp,s1,s2,n,cmporder_reverse) ;
    }
} /* end namespace (libu) */

namespace libu {
    int strnxcasecmpo(cchar *s1,cchar *s2,int n) noex {
    	return strnxcmp(strncasecmp,s1,s2,n,cmporder_obverse) ;
    }
    int strnxcasecmpr(cchar *s1,cchar *s2,int n) noex {
    	return strnxcmp(strncasecmp,s1,s2,n,cmporder_reverse) ;
    }
} /* end namespace (libu) */

namespace libu {
    int strnxfoldcmpo(cchar *s1,cchar *s2,int n) noex {
    	return strnxcmp(strnfoldcmp,s1,s2,n,cmporder_obverse) ;
    }
    int strnxfoldcmpr(cchar *s1,cchar *s2,int n) noex {
    	return strnxcmp(strnfoldcmp,s1,s2,n,cmporder_reverse) ;
    }
} /* end namespace (libu) */


/* local subroutines */


