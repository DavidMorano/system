/* hasclass SUPPORT */
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
	has{x}

	Name:
	hasalpha
	hasalnum
	hasdigit
	hasoctal
	hasdigex
	haswhite
	hasblank
	hasprint
	hasterm
	haslc
	hasuc

	Aliases:
	hasdig
	hasoct
	hasdec
	hashex
	haswht
	hasblk

	Description:
	Does the given counted c-string have any of the characters 
	in the class?

	Synopsis:
	bool has{x}(cchar *sp,int sl) noex

	Arguments:
	{x}		alpha, alnum, digit, digex, white, lc, uc
	{x}		dig, dec, hex, wht
	sp		test c-string pointer
	sl		test c-string length

	Returns:
	false		c-string does not have all digits
	true		c-string has all digits in it

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */
#include	<mkchar.h>		/* LIBU */
#include	<char.h>		/* LIBUC */
#include	<ischarx.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */

#include	"hasclass.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |getlenstr(3u)| + |lenstr(3u)| */

/* local defines */


/* imported namespaces */


/* local typedefs */

extern "C" {
    typedef bool (*ischar_f)(int) noex ;
}


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

local bool hasx(ischar_f isx,cchar *sp,int sl) noex {
	bool		f = false ;
	if (sp) ylikely {
	    for (int ch ; sl-- && ((ch = mkchar(*sp))) ; sp += 1) {
	        if ((f = isx(ch))) break ;
	    } /* end for */
	} /* end if (non-null) */
	return f ;
} /* end subroutine (hasx) */


/* local variables */


/* exported variables */


/* exported subroutines */

bool hasalpha(cchar *sp,int sl) noex {
    	return hasx(isalphalatin,sp,sl) ;
} /* end subroutine (hasalpha) */

bool hasalnum(cchar *sp,int sl) noex {
    	return hasx(isalnumlatin,sp,sl) ;
} /* end subroutine (hasalnum) */

bool hasdigit(cchar *sp,int sl) noex {
    	return hasx(isdigitlatin,sp,sl) ;
} /* end subroutine (hasdigit) */

bool hasdigex(cchar *sp,int sl) noex {
    	return hasx(isdigexlatin,sp,sl) ;
} /* end subroutine (hasdigex) */

bool hasoctal(cchar *sp,int sl) noex {
    	return hasx(isoctallatin,sp,sl) ;
} /* end subroutine (hasoctal) */

bool haswhite(cchar *sp,int sl) noex {
    	return hasx(iswhitelatin,sp,sl) ;
} /* end subroutine (haswhite) */

bool hasblank(cchar *sp,int sl) noex {
    	return hasx(isblanklatin,sp,sl) ;
} /* end subroutine (hasblank) */

bool hasprint(cchar *sp,int sl) noex {
    	return hasx(isprintlatin,sp,sl) ;
} /* end subroutine (hasprint) */

bool hasterm(cchar *sp,int sl) noex {
    	return hasx(istermlatin,sp,sl) ;
} /* end subroutine (hasterm) */

bool haslc(cchar *sp,int sl) noex {
    	return hasx(islowerlatin,sp,sl) ;
} /* end subroutine (haslc) */

bool hasuc(cchar *sp,int sl) noex {
    	return hasx(isupperlatin,sp,sl) ;
} /* end subroutine (hasuc) */


/* local subroutines */


