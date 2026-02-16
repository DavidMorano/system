/* strvalcmp SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* string field comparisons */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-12-12, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	strvalcmp

	Description:
	These subroutines are used to compare fields of a string
	(like an environment variables type of string 'HOME=/here').
	Fields that can be compared are:
		key
		value

	Synopses:
	int strvalcmp(cchar *sp,cchar *vp,int vl) noex

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
#include	<strings.h>		/* |strcasecmp(3c)| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>		/* |strchr(3c)| + |strncmp(3c)| */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<localmisc.h>

#include	"strvalcmp.h"

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

int strvalcmp(cchar *sp,cchar *vp) noex {
	int		rc = 0 ;
	if (sp && vp) {
	    if (cchar *tp ; (tp = strchr(sp,'=')) != nullptr) {
	        sp = (tp + 1) ;
		rc = strcmp(sp,vp) ;
	    } /* end if */
	} /* end if (non-null) */
	return rc ;
}
/* end subroutine (strvalcmp) */


