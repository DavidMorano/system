/* usupport_siwhtx MODULE (module-implementation-unit) */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* subroutine to find the index of the first character not something */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-23, David A­D­ Morano
	This code was originally written.

	= 2020-03-22, David A­D­ Morano
	I modularized this (made into a C++20 module).

*/

/* Copyright © 1998,2020 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	siwhtchr
	siwhtbrk

	Description:
	These subroutine searchs for an a character that is either
	the specified character (if provided), or for a character
	in a specified chracter-class (if provided), or failing
	that a white-space character.

	Synopsis:
	int siwhtchr(cchar *sp,int sl,int sch) noex
	int siwhtbrk(cchar *sp,int sl,cchar *ss) noex

	Arguments:
	sp	string to be examined
	sl	length of string of break character to break on
	sch	search character
	ss	search character-class (a c-string)

	Returns:
	>=0	index of found character
	<0	the search character was not found

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

#include	"usupport_strnwht.hh"
#include	"usupport_siwhtx.hh"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |getlenstr(3u)| */
import ureserve ;			/* |ischarx(3u)| */
import chrset ;

/* local defines */


/* local namespaces */

using libu::strnwhtchr ;		/* subroutine */
using libu::strnwhtbrk ;		/* subroutine */


/* local typedefs */


/* external subroutines */

namespace libu {
    extern char *strnwhtbrk(cchar *,int,con chrset *) noex ;
    extern char *strnwhtchr(cchar *,int,con chrset *) noex ;
} /* end namespace (libu) */

namespace libu {
    extern int siwhtbrk(cchar *,int,con chrset *) noex ;
} /* end namespace */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

namespace libu {
    int siwhtchr(cchar *sp,int sl,int sch) noex {
    	int		si = -1 ;
	if (sp) ylikely {
	    if (cchar *tp = strnwhtchr(sp,sl,sch) ; tp) {
	        si = intconv(tp - sp) ;
	    }
	} /* end if (non-null) */
    	return si ;
    } /* end subroutine (siwhtchr) */
    int siwhtbrk(cchar *sp,int sl,cchar *ss) noex {
    	int		si = -1 ;
	if (sp) ylikely {
	    if (cchar *tp = strnwhtbrk(sp,sl,ss) ; tp) {
	        si = intconv(tp - sp) ;
	    }
	} /* end if (non-null) */
    	return si ;
    } /* end subroutine (siwhtbrk) */
    int siwhtbrk(cchar *sp,int sl,con chrset *setp) noex {
    	int		si = -1 ;
	if (sp) ylikely {
	    if (cchar *tp = libu::strnwhtbrk(sp,sl,setp) ; tp) {
	        si = intconv(tp - sp) ;
	    }
	} /* end if (non-null) */
    	return si ;
    } /* end subroutine (siwhtbrk) */
} /* end namespace (libu) */


