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
	This subroutine is used to compare fields of a c-string
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
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */
#include	<usys.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */
#include	<cmporders.h>		/* LIBU */

#include	"strnvalcmp.h"

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

int strnvalcmp(cchar *sp,cchar *valp,int µvall) noex {
    	cnullptr	np{} ;
	int		rc = 0 ;
	if (sp) ylikely {
	    if (int vall ; (vall = getlenstr(valp,µvall)) >= 0) ylikely {
	        if (cchar *tp = strochr(sp,'=')) {
	            sp = (tp + 1) ;
	            while (*sp) {
			if (cmpstrn(sp,valp,vall) == 0) {
			    cint ch_end = sp[vall] ;
			    if ((ch_end == '\0') || (ch_end == ':')) {
		               rc = 0 ;
		            }
			} /* end */
	                if ((tp = strochr(sp,':')) == np) break ;
	                sp = (tp + 1) ;
		        if (rc) break ;
	            } /* end while */
	        } /* end if */
	    } /* end if (getlenstr) */
	} /* end if (non-null) */
	return rc ;
} /* end subroutine (strnvalcmp) */

#ifdef	COMMENT
int strnvalcmp(cchar *sp,cchar *vp,int µvl) noex {
	int		rc = 0 ;
	if (sp && vp) {
	    if (cint vl = getlenstr(vp,µvl) ; vl >= 0) {
	        if (cchar *tp = strochr(sp,'=')) {
	            sp = (tp + 1) ;
		    rc = cmpstrn(sp,vp,vl) ;
	        } /* end if */
	    } /* end if (getlenstr) */
	} /* end if (non-null) */
	return rc ;
} /* end subroutine (strnvalcmp) */
#endif /* COMMENT */


