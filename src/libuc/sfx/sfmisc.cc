/* sfmisc SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* get a substring present *before* the first dot */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-07-01, David A­D­ Morano
	This subroutine gets the leading string before the first
	dot character (white-space cleaned up).

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	sfwhitedot

	Description:
	This subroutine retrieves the substring *before* the first
	dot character.  If there is no explict terminator (dot
	character), an empty indication (result = 0) is returned.
	The returned string is white-space shrunken.

	Synopsis:
	int sfwhitedot(cchar *sp,int sl,cchar **rpp) noex

	Arguments:
	sp		given c-string pointer
	sl		given c-string length
	rpp		pointer to hold result pointer

	Returns:
	>=0		length of retrieved nodename
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usys.h>		/* LIBU */
#include	<usupport.h>		/* LIBU */
#include	<char.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */

#include	"sfxchr.h"
#include	"sfmisc.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| */

/* local defines */

#define	ISWHT(ch)	CHAR_ISWHT(ch)


/* imported namespaces */

using libu::strnchr ;			/* subroutine */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int sfwhitedot(cchar *sp,int sl,cchar **rpp) noex {
    	int	rl = -1 ;
	if (sp && rpp) {
	    while (sl && ISWHT(*sp)) {
		sp += 1 ;
		sl -= 1 ;
	    } /* end while */
	    rl = sl ;
	    if (charp tp = strnchr(sp,sl,'.')) {
		rl = intconv(tp - sp) ;
		while (rl && ISWHT(sp[rl - 1])) {
		    rl -= 1 ;
		} /* end while */
	    } /* end if */
	    *rpp = sp ;
	} /* end if (non-null) */
	return rl ;
} /* end subroutine (sfwhitedot) */


/* local subroutines */


