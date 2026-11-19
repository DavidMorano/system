/* usupport_cmpstrv SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* c-string comparisons w/ case variations */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-12-12, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	cmpstrv

	Description:
	These subroutines are used to compare fields of a string
	(like an environment variables type of string 'HOME=/here').
	Fields that can be compared are:
		key
		value

	Synopses:
	int cmpstrv(cchar *sp,cchar *vp,int vl) noex

	Arguments:
	sp		c-string pointer
	vp		value c-string pointer
	vl		value c-string length

	Returns:
	>=0		greater-than
	==0		equal-to
	<0		less-than

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */
#include	<usys.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */
#include	<cmporders.h>		/* LIBU */

#include	"usupport_cmpstrv.hh"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| */

/* local defines */


/* local namespaces */


/* local typedefs */

typedef cmporders	co ;


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

namespace libu {
    int cmpstrv(cchar *sp,cchar *vp) noex {
	int		rc = 0 ;
	if (sp && vp) ylikely {
	    if (cchar *tp = strochr(sp,'=')) {
	        sp = (tp + 1) ;
		rc = cmpstr(sp,vp) ;
	    } /* end if */
	} /* end if (non-null) */
	return rc ;
    } /* end subroutine (cmpstrv) */
} /* end namespace (libu) */

namespace libu {
    int cmpstrvø(cchar *sp,cchar *vp) noex {
    	int		rc = 0 ;
	if (sp || vp) ylikely {
	    rc = +1 ;
	    if (sp) {
		rc = -1 ;
		if (vp) {
		    rc = cmpstrv(sp,vp) ;
		} /* end */
	    }
	} /* end if */
	return rc ;
    } /* end subroutine (cmpstrvø) */
} /* end namespace (libu) */


