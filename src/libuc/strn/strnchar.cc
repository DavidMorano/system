/* strnchar SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* find a character in a counted c-string */
/* version %I% last-modified %G% */


/* revision history:

	= 1999-06-08, David A­D­ Morano
	This subroutine was written again for Rightcore Network
	Services (RNS).

*/

/* Copyright © 1999 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	strn{xx}

	Description:
	Yes, this is quite the same as |strchr(3c)| except that a
	length of the test string (the first argument) can be given.
	Also, a character that belongs to a particular character-class
	(already established) is searched for.

	Synopsis:
	char *strn{xx}(cchar *sp,int sl) noex

	Arguments:
	{xx}		name of character class: alpha, alnum, digit, xdigit
	sp		string to search through
	sl		maximum number of character to search

	Returns:
	-		address of found character or |nullptr| if not found

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<mkchar.h>
#include	<ischarx.h>
#include	<localmisc.h>

#include	"strnchar.h"

import libutil ;

/* local defines */


/* imported namespaces */


/* local typedefs */

extern "C" {
    typedef bool (*xchr_f)(int) noex ;
}


/* external subroutines */


/* external subroutines */


/* local structures */


/* forward references */

static char *strnxx(xchr_f xchr,cchar *sp,int sl) noex {
	char		*rsp = nullptr ;
	if (sp) ylikely {
	    bool	f = false ;
	    while (sl-- && *sp) {
		cint	ch = mkchar(*sp) ;
		f = xchr(ch) ;
		if (f) break ;
		sp += 1 ;
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
}

char *strnalnum(cchar *sp,int sl) noex {
    	return strnxx(isalnumlatin,sp,sl) ;
}

char *strndigit(cchar *sp,int sl) noex {
    	return strnxx(isdigitlatin,sp,sl) ;
}

char *strnxdigit(cchar *sp,int sl) noex {
    	return strnxx(ishexlatin,sp,sl) ;
}


