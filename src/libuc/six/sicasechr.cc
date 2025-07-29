/* sicasechr SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* subroutine to find the index of a character in a given string */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-23, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	sicasechr

	Description:
	This subroutine searches for a character within a given
	string and returns the index to that character (if it is
	found).  It returns (-1) if the character does not exist
	within the given string.

	Synopsis:
	int sicasechr(cchar *sp,int sl,int sch) noex

	Arguments:
	sp	string to be examined
	sl	length of string of break character to break on
	sch	character to search for

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
#include	<nleadstr.h>
#include	<toxc.h>		/* |tobc(3u)| + |tolc(3u)| + ... */
#include	<mkchar.h>
#include	<ischarx.h>
#include	<localmisc.h>

#include	"sicasechr.h"


/* local defines */


/* external subroutines */


/* local namespaces */


/* local typedefs */


/* imported namespaces */


/* local typedefs */

extern "C" {
    typedef int (*toxc_f)(int) noex ;
}


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

template<toxc_f toxc>
static int sixchr(cchar *sp,int sl,int sch) noex {
	int		i = 0 ; /* used-afterwards */
	bool		f = false ;
	if (sp) ylikely {
	    sch = toxc(sch) ;
	    for (i = 0 ; sl-- && sp[i] ; i += 1) {
	        cint	ch = toxc(sp[i]) ;
	        f = (ch == sch) ;
	        if (f) break ;
	    } /* end for */
	} /* end if (non-null) */
	return (f) ? i : -1 ;
} /* end subroutine-template (sixchr) */


/* local variables */


/* exported variables */


/* exported subroutines */

int sibasechr(cchar *sp,int sl,int sch) noex {
    	return sixchr<tobc>(sp,sl,sch) ;
} /* end subroutine (sibasechr) */

int sicasechr(cchar *sp,int sl,int sch) noex {
    	return sixchr<touc>(sp,sl,sch) ;
} /* end subroutine (sicasechr) */

int sifoldchr(cchar *sp,int sl,int sch) noex {
    	return sixchr<tofc>(sp,sl,sch) ;
} /* end subroutine (sifoldchr) */


