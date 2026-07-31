/* sflast SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* find the last <n> characters in a given string */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-23, David A-D- Morano
	This code was originally written.

*/

/* Copyright © 1998 David A-D- Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	sflast

	Description:
	This subroutine will find the last <n> characters in the
	given string.

	Synopsis:
	int sflast(cchar *sp,int sl,int n,cchar **rpp) noex

	Arguments:
	sp		given string to test
	sl		length of string to test
	n		find this (<n>) number of last characters
	rpp		pointer to receive

	Returns:
	>=0		length of found string
	<0		bad

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

#include	"sfx.h"

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

int sflast(cchar *sp,int sl,int n,cchar **rpp) noex {
    	if (sp) ylikely {
	    if (sl < 0) sl = lenstr(sp) ;
	    if ((n >= 0) && (sl > n)) {
	        sl = n ;
	        sp += (sl - n) ;
	    }
	} else {
	    sl = -1 ;
	}
	if (rpp) *rpp = sp ;
	return sl ;
} /* end subroutine (sflast) */


