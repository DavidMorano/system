/* hasnot SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* does the given counted c-string have some characteristic? */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-10-10, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Group:
	hasnot

	Names:
	hasnotdots
	hasnotempty
	
	Aliases:
	hasNotDots
	hasNotEmpty

	Description:
	These subroutines determine if a given counted c-string
	does not have something.

	Synopsis:
	bool hasnotdots(cchar *sp,int sl) noex
	bool hasnotempty(cchar *sp,int sl) noex

	Arguments:
	sp		given counted c-string pointer
	sl		given counted c-string length

	Returns:
	true		determination is true
	false		determination is false

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */
#include	<ascii.h>		/* LIBU */
#include	<mkchar.h>		/* LIBU */
#include	<char.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */

#include	"hasnot.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |getlenstr(3u)| */

/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

local bool	isnotempry(int) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

bool hasnotdots(cchar *sp,int µsl) noex {
	bool		f = true ;
	if (int sl ; (sl = getlenstr(sp,µsl)) > 0) {
	    if (sp[0] == '.') {
		switch (sl) {
		case 1:
		    f = false ;
		    break ;
		case 2:
		    f = (sp[1] != '.') ;
		    break ;
		} /* end switch */
	    } /* end if (had a dot) */
	} /* end if (getlenstr) */
	return f ;
} /* end subroutine (hasnotdots) */

bool hasnotempty(cchar *sp,int sl) noex {
	bool		f = false ;
	if (sp) ylikely {
	    for (int ch ; sl-- && ((ch = mkchar(*sp))) ; sp += 1) {
	        if ((f = isnotempry(ch))) break ;
	    } /* end for */
	} /* end if (non-null) */
	return f ;
} /* end subroutine (hasnotempty) */


/* local subroutines */

local bool isnotempry(int ch) noex {
    	bool f = true ;
	f = f && (! CHAR_ISWHITE(ch)) ;
	f = f && (ch != CH_NL) ;
	return f ;
} /* end subroutine (isnotempry) */


