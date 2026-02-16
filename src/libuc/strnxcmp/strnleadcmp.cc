/* strnleadcmp SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* check if string 's2' is a leading substring of string 's1' */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	strnleadcmp

	Description:
	This subroutine returns true if 's2' is an initial substring
	of 's1'.  But only up to the maximum number of characters
	are checked.

	Synopsis:
	int strnleadcmp(cchar *s1,cchar *s2,int lr) noex

	Arguments:
	s1		string-1 pointer
	s2		string-2 pointer
	lr		length?

	Returns:
	>0		greater-than
	==0		equal-to
	<0		less-than

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<mkchar.h>
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

int strnleadcmp(cchar *s1,cchar *s2,int lr) noex {
	int		rc = 0 ;
	if (s1 && s2) {
	    if (lr < 0) lr = lenstr(s2) ;
	    while (lr-- > 0) {
		cint ch1 = mkchar(*s1++) ;
		cint ch2 = mkchar(*s2++) ;
	        rc = ch1 - ch2 ;
	        if (rc || !ch1 || !ch2) break ;
	    } /* end while */
	} /* end if (non-null) */
	return rc ;
}
/* end subroutine (strnleadcmp) */


