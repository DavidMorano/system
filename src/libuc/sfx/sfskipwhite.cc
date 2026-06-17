/* sfskipwhite SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* remove leading white space */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-23, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	sfskipwhite

	Description:
	This subroutine will identify the non-white-space portion
	of the buffer by ignoring white space at the beginning of
	the given buffer.  No modifications to the buffer are made.

	Synopsis:
	int sfskipwhite(cchar *sp,int sl,cchar **rpp) noex

	Arguments:
	sp		given string to test
	sl		length of string to test
	rpp		pointer to receive

	Returns:
	>=0		non-white-space string length
	<0		error (-1)

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

import libutil ;			/* |lenstr(3u)| */

/* local defines */

#define	ISWHT(ch)	CHAR_ISWHITE(ch)


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int sfskipwhite(cchar *sp,int sl,cchar **rpp) noex {
    	if (sp) ylikely {
	    if (sl < 0) {
	        while (ISWHT(*sp)) {
		    sp += 1 ;
		} /* end while */
	        sl = lenstr(sp) ;
	    } else {
	        while (sl && ISWHT(*sp)) {
	            sp += 1 ;
	            sl -= 1 ;
	        } /* end while */
	    } /* end if */
	} else {
	    sl = -1 ;
	} /* end if (non-null) */
	if (rpp) *rpp = sp ;
	return sl ;
} /* end subroutine (sfskipwhite) */


