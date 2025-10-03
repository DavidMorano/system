/* sileadchr SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* find index to first gicen character after skipping white-space */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-23, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	sileadchr

	Description:
	This subroutine returns the number of leading white-space
	characters skipped before encountering the given character.

	Synopsis:
	int sileadchr(cchar *sp,int sl,int ch) noex

	Arguments:
	sp	c-string pointer
	sl	c-string length
	ch	character to check at end of white-space

	Returns:
	>0	match - number of characters skipped
	==0	match - no characters were skipped (no whitespace)
	<0	no match or error (null-pointer)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<mkchar.h>
#include	<char.h>
#include	<localmisc.h>

#include	"sileadchr.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |getlenstr(3u)| */

/* local defines */


/* local namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */

cauto		iswht = char_iswhite ;


/* exported variables */


/* exported subroutines */

int sileadchr(cchar *sp,int µsl,int chx) noex {
	int		i = -1 ; /* used afterwards */
	bool		f = false ;
	if (sp) ylikely {
	    if (int ch, sl ; (sl = getlenstr(sp,µsl)) > 0) {
	        for (i = 0 ; i < sl ; i += 1) {
		    ch = mkchar(sp[i]) ;
	            if (! iswht(ch)) break ;
	        } /* end for */
		f = (i < sl) && (ch == chx) ;
	    } /* end if (getlenstr) */
	} /* end if (non-null) */
	return (f) ? i : -1 ;
}
/* end subroutine (sileadchr) */


