/* usupport_sichar SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* subroutine to find the index of an alpha character in a given string */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-23, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Names:
	sialpha
	sialnum
	sidigit
	sidigex
	sioctal
	siwhite
	siblank

	Aliases:
	sidig
	sioct
	sidec
	sihex
	siwht

	Description:
	This subroutine searchs for a character belonging to a
	character-class within a given string, and returns the index
	of that character (if it is found).  It returns -1 if the
	character does not exist within the given string.

	Synopsis:
	int sialpha(cchar *sp,int sl) noex
	int sialnum(cchar *sp,int sl) noex
	int sidigit(cchar *sp,int sl) noex
	int sidigex(cchar *sp,int sl) noex
	int sioctal(cchar *sp,int sl) noex
	int siwhite(cchar *sp,int sl) noex
	int siblank(cchar *sp,int sl) noex

	Arguments:
	sp	string to be examined
	sl	length of string of break character to break on

	Returns:
	>=0	index of search character in the given string
	<0	the search character was not found

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<usysbase.h>
#include	<mkchar.h>		/* LIBU */
#include	<localmisc.h>

#include	"usupport_sichar.h"

import ureserve ;			/* |is{x}(3u)| */

/* local defines */


/* local namespaces */


/* local typedefs */

extern "C" {
    typedef bool (*ischr_f)(int) noex ;
}


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

local int sichar(ischr_f ischr,cchar *sp,int sl) noex {
	int		i = 0 ; /* used-afterwards */
	bool		f = false ;
	if (sp) ylikely {
	    for (i = 0 ; sl && sp[i] ; i += 1) {
	        cint	ch = mkchar(sp[i]) ;
	        if ((f = ischr(ch))) break ;
	        sl -= 1 ;
	    } /* end for */
	} /* end if (non-null) */
	return (f) ? i : -1 ;
} /* end subroutine (sichar) */


/* local variables */


/* exported variables */


/* exported subroutines */

namespace libu {
    int sialpha(cchar *sp,int sl) noex {
    	return sichar(isalphalatin,sp,sl) ;
    } /* end subroutine (sialpha) */
    int sialnum(cchar *sp,int sl) noex {
    	return sichar(isalnumlatin,sp,sl) ;
    } /* end subroutine (sialnum) */
    int sidigit(cchar *sp,int sl) noex {
    	return sichar(isdigitlatin,sp,sl) ;
    } /* end subroutine (sidigit) */
    int sidigex(cchar *sp,int sl) noex {
    	return sichar(isdigexlatin,sp,sl) ;
    } /* end subroutine (sidigex) */
    int sioctal(cchar *sp,int sl) noex {
    	return sichar(isoctallatin,sp,sl) ;
    } /* end subroutine (sioctal) */
    int siwhite(cchar *sp,int sl) noex {
    	return sichar(iswhitelatin,sp,sl) ;
    } /* end subroutine (siwhite) */
    int siblank(cchar *sp,int sl) noex {
    	return sichar(isblanklatin,sp,sl) ;
    } /* end subroutine (siblank) */
} /* end namespace (libu) */


