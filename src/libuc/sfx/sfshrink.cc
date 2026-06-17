/* sfshrink SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* remove leading and trailing white space */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-05-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	sfshrink

	Description:
	This subroutine will identify the non-white-space portion
	of the buffer by ignoring white space at the beginning and
	at the end of the given buffer.  No modifications to the
	buffer are made.

	Synopsis:
	int sfshrink(cchar *sp,int sl,cchar **rpp) noex

	Arguments:
	sp	source c-string pointer
	sl	source c-string length
	rpp	pointer to pointer to resulting string

	Returns:
	>=0	non-white-space string length (if OK), otherwise 0
	<0	error (-1)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<char.h>		/* LIBUC |CHAR_ISWHITE(3uc)| */
#include	<localmisc.h>		/* LIBU */

#include	"sfx.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u{)| */

/* local defines */

#define	ISWHT(ch)	CHAR_ISWHITE(ch)


/* local namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int sfshrink(cchar *sp,int sl,cchar **rpp) noex {
    	if (sp) ylikely {
	    if (sl >= 0) {
	        while ((sl > 0) && ISWHT(*sp)) {
	            sp += 1 ;
	            sl -= 1 ;
	        } /* end while */
	        if (sp[0] == '\0') sl = 0 ;
	    } else {
	        while (ISWHT(*sp)) {
	            sp += 1 ;
	        }
	        sl = lenstr(sp) ;
	    } /* end if */
	    while ((sl > 0) && ISWHT(sp[sl - 1])) {
	        sl -= 1 ;
	    }
	} else {
	    sl = -1 ;
	}
	if (rpp) *rpp = sp ;
	return sl ;
} /* end subroutine (sfshrink) */


