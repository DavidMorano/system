/* strnvalcmp SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* string field comparisons */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-12-12, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	strnvalcmp

	Description:
	This subroutines are used to compare fields of a c-string
	(like an environment variables type of string 'HOME=/here').
	Fields that can be compared are:
		key
		value

	The value part of the given c-string is compared with the
	counted c-string value as specified.

	Synopses:
	int strnvalcmp(cchar *sp,cchar *vp,int vl) noex

	Arguments:
	sp		key=value c-string pointer
	vp		value c-string pointer
	vl		value c-string length

	Returns:
	>0		greater-than
	==0		equal-to
	<0		less-than

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>		/* |strchr(3c)| + |strncmp(3c)| */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<localmisc.h>

#include	"strnxcmp.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| */

/* local defines */


/* local namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int strnvalcmp(cchar *sp,cchar *vp,int vl) noex {
	int		rc = 0 ;
	if (sp && vp) {
	    if (vl < 0) vl = lenstr(vp) ;
	    if (cchar *tp ; (tp = strchr(sp,'=')) != nullptr) {
	        sp = (tp + 1) ;
	        while (*sp) {
	            if ((strncmp(sp,vp,vl) == 0) &&
	                ((sp[vl] == '\0') || (sp[vl] == ':'))) {
		        rc = 0 ;
		    }
	            if ((tp = strchr(sp,':')) == nullptr) break ;
	            sp = (tp + 1) ;
		    if (rc) break ;
	        } /* end while */
	    } /* end if */
	} /* end if (non-null) */
	return rc ;
}
/* end subroutine (strnvalcmp) */

#ifdef	COMMENT

int strnvalcmp(cchar *sp,cchar *vp,int µvl) noex {
	int		rc = 0 ;
	if (sp && vp) {
	    if (cint vl = getlenstr(vp,µvl) ; vl >= 0) {
	        if (cchar *tp ; (tp = strchr(sp,'=')) != nullptr) {
	            sp = (tp + 1) ;
		    rs = strncmp(sp,vp,vl) ;
	        } /* end if */
	    } /* end if (getlenstr) */
	} /* end if (non-null) */
	return rc ;
}
/* end subroutine (strnvalcmp) */

#endif /* COMMENT */


