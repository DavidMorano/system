/* sinext SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* return index to end of next string-field */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-23, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	sinext

	Description:
	Find the index to the end of the first string field within
	the given string.

	Synopsis:
	int sinext(cchar *sp,int sl) noex

	Arguments:
	sp	string to be examined
	sl	length of string to be examined

	Returns:
	>=0	index of beginning of next field
	<0	address-fault or substring not-found

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<char.h>		/* LIBUC |CHAR_ISWHITE(3uc)| */
#include	<localmisc.h>		/* LIBU */

#include	"sinext.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| */

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

int sinext(cchar *sp,int µsl) noex {
	int		si = -1 ; /* return-value */
	if (int sl ; (sl = getlenstr(sp,µsl)) >= 0) {
	    bool	f = false ;
	    int		i = 0 ;
	    while ((i < sl) && ISWHT(sp[i])) {
	        i += 1 ;
	    }
	    while ((i < sl) && sp[i] && (! ISWHT(sp[i]))) {
		f = true ;
	        i += 1 ;
	    }
	    if (f) {
		si = i ;
	    }
	} /* end if (getlenstr) */
	return si ;
} /* end subroutine (sinext) */


