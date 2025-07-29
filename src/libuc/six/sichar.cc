/* sichar SUPPORT */
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
	sixdigit

	Description:
	This subroutine searchs for an alpha character within a
	given string and returns the index to that character (if
	it is found).  It returns -1 if the character does not exist
	within the given string.

	Synopsis:
	int sialpha(cchar *sp,int sl) noex
	int sialnum(cchar *sp,int sl) noex
	int sidigit(cchar *sp,int sl) noex
	int sixdigit(cchar *sp,int sl) noex

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
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<mkchar.h>
#include	<ischarx.h>		/* |is{x}latin(3u)| */
#include	<localmisc.h>

#include	"sichar.h"


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

static int sichar(ischr_f ischr,cchar *sp,int sl) noex {
	int		i = 0 ; /* used-afterwards */
	bool		f = false ;
	if (sp) ylikely {
	    for (i = 0 ; sl && sp[i] ; i += 1) {
	        cint	ch = mkchar(sp[i]) ;
	        f = ischr(ch) ;
	        if (f) break ;
	        sl -= 1 ;
	    } /* end for */
	} /* end if (non-null) */
	return (f) ? i : -1 ;
} /* end subroutine (sichar) */


/* local variables */


/* exported variables */


/* exported subroutines */

int sialpha(cchar *sp,int sl) noex {
    	return sichar(isalphalatin,sp,sl) ;
} /* end subroutine (sialpha) */

int sialnum(cchar *sp,int sl) noex {
    	return sichar(isalnumlatin,sp,sl) ;
} /* end subroutine (sialnum) */

int sidigit(cchar *sp,int sl) noex {
    	return sichar(isdigitlatin,sp,sl) ;
} /* end subroutine (sidigit) */

int sixdigit(cchar *sp,int sl) noex {
    	return sichar(ishexlatin,sp,sl) ;
} /* end subroutine (sixdigit) */


