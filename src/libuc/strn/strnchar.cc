/* strnchar SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* find a character in a counted c-string */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written again for Rightcore Network
	Services (RNS).

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Gorup:
	strn{xx}

	Description:
	Yes, this is quite the same as |strchr(3c)| except that a
	length of the test string (the first argument) can be given.
	Also, a character that belongs to a particular character-class
	(already established) is searched for.

	Synopsis:
	char *strn{xx}(cchar *sp,int sl) noex

	Arguments:
	{xx}		name of character class: alpha, alnum, digit
	sp		string to search through
	sl		maximum number of character to search

	Returns:
	-		address of found character or |nullptr| if not found

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */
#include	<mkchar.h>		/* LIBU */
#include	<ischarx.h>		/* LIBUC <- money shot! */
#include	<localmisc.h>		/* LIBU */

#include	"strnchar.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |getlenstr(3u)| */

/* local defines */


/* imported namespaces */


/* local typedefs */

extern "C" {
    typedef bool (*xchr_f)(int) noex ;
} /* end extern (C) */


/* external subroutines */


/* external subroutines */


/* local structures */


/* forward references */

local char *strnxx(xchr_f xchr,cchar *sp,int sl) noex {
	char		*rsp = nullptr ;
	if (sp) ylikely {
	    bool	f = false ;
	    for (int ch ; sl-- && ((ch = mkchar(*sp))) ; sp += 1) {
		if ((f = xchr(ch))) break ;
	    } /* end while */
	    if (f) rsp = charp(sp) ;
	} /* end if (non-null) */
	return rsp ;
} /* end subroutine (strnxx) */


/* local variables */


/* exported variables */


/* exported subroutines */

char *strnalpha(cchar *sp,int sl) noex {
    	return strnxx(isalphalatin,sp,sl) ;
} /* end subroutine */

char *strnalnum(cchar *sp,int sl) noex {
    	return strnxx(isalnumlatin,sp,sl) ;
} /* end subroutine */

char *strndigit(cchar *sp,int sl) noex {
    	return strnxx(isdigitlatin,sp,sl) ;
} /* end subroutine */

char *strndigex(cchar *sp,int sl) noex {
    	return strnxx(isdigexlatin,sp,sl) ;
} /* end subroutine */

char *strnoctal(cchar *sp,int sl) noex {
    	return strnxx(isoctallatin,sp,sl) ;
} /* end subroutine */

char *strnblank(cchar *sp,int sl) noex {
    	return strnxx(isblanklatin,sp,sl) ;
} /* end subroutine */


