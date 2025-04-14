/* strnvalcmp SUPPORT */
/* encoding=ISO8859-1 */
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
	These subroutines are used to compare fields of a string
	(like an environment variables type of string 'HOME=/here').
	Fields that can be compared are:
		key
		value

	Synopses:
	int strnkeycmp(cchar *sp,cchar *kp,int kl) noex
	int strnvalcmp(cchar *sp,cchar *vp,int vl) noex

	Arguments:
	sp		c-string pointer
	kp		key c-string pointer
	kl		key c-string length
	vp		value c-string pointer
	vl		value c-string length

	Returns:
	>=0		greater-than
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
#include	<libutil.hh>
#include	<localmisc.h>

#include	"strnxcmp.h"


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
	int		rc = -1 ;
	if (vl < 0) vl = xstrlen(vp) ;
	if (cchar *tp ; (tp = strchr(sp,'=')) != nullptr) {
	    sp = (tp+1) ;
	    while (*sp) {
	        if ((strncmp(sp,vp,vl) == 0) &&
	            ((sp[vl] == '\0') || (sp[vl] == ':'))) {
		    rc = 0 ;
		}
	        if ((tp = strchr(sp,':')) == nullptr) break ;
	        sp = (tp+1) ;
		if (rc == 0) break ;
	    } /* end while */
	} /* end if */
	return rc ;
}
/* end subroutine (strnvalcmp) */


