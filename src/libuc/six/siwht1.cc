/* siwht1 MODULE (module-implementation-unit) */
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

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	siwht{xxx}

	Description:
	This subroutine searchs for an a character that is not part
	of a certain character class.

	Synopsis:
	int siwht{xxx}(cchar *sp,int sl) noex

	Arguments:
	xxx	one of: oct, dec, hex
	sp	string to be examined
	sl	length of string of break character to break on

	Returns:
	>=0	index of found character
	<0	the search character was not found

*******************************************************************************/

module ;

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<strn.h>
#include	<localmisc.h>

module siwht ;

import strnwht ;			/* |strnwhtbrk(3uc)| */

/* local defines */


/* local namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

extern "C" {
    int siwht(cchar *sp,int sl) noex {
    	cnullptr	np{} ;
    	int		si = -1 ;
	if (sp) ylikely {
	    if (cchar *tp ; (tp = strnwht(sp,sl)) != np) {
	        si = intconv(tp - sp) ;
	    }
	} /* end if (non-null) */
    	return si ;
    } /* end subroutine (siwht) */
    int siwhtbrk(cchar *sp,int sl,cchar *ss) noex {
    	cnullptr	np{} ;
    	int		si = -1 ;
	if (sp) ylikely {
	    if (cchar *tp ; (tp = strnwhtbrk(sp,sl,ss)) != np) {
	        si = intconv(tp - sp) ;
	    }
	} /* end if (non-null) */
    	return si ;
    } /* end subroutine (siwhtbrk) */
    int siwhtchr(cchar *sp,int sl,int sch) noex {
    	cnullptr	np{} ;
    	int		si = -1 ;
	if (sp) ylikely {
	    if (cchar *tp ; (tp = strnwhtchr(sp,sl,sch)) != np) {
	        si = intconv(tp - sp) ;
	    }
	} /* end if (non-null) */
    	return si ;
    } /* end subroutine (siwhtchr) */
} /* end extern */

extern "C++" {
    int siwhtbrk(cchar *sp,int sl,const chrset &sset) noex {
    	cnullptr	np{} ;
    	int		si = -1 ;
	if (sp) ylikely {
	    if (cchar *tp ; (tp = strnwhtbrk(sp,sl,sset)) != np) {
	        si = intconv(tp - sp) ;
	    }
	} /* end if (non-null) */
    	return si ;
    } /* end subroutine (siwhtbrk) */
} /* end extern */


