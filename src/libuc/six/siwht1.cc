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

/* Copyright © 1998,2020 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	siwht
	siwhtchr
	siwhtbrk

	Description:
	These subroutine searchs for an a character that is either
	the specified character (if provided), or for a character
	in a specified chracter-class (if provided), or failing
	that a white-space character.

	Synopsis:
	int siwht(cchar *sp,int sl) noex
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

module ;

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */
#include	<strn.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */

#pragma		GCC dependency		"mod/siwht.ccm"
#pragma		GCC dependency		"mod/strnwht.ccm"

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
    	int		si = -1 ;
	if (sp) ylikely {
	    if (cchar *tp = strnwht(sp,sl) ; tp) {
	        si = intconv(tp - sp) ;
	    }
	} /* end if (non-null) */
    	return si ;
    } /* end subroutine (siwht) */
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
} /* end extern (C) */

extern "C++" {
    int siwhtbrk(cchar *sp,int sl,con chrset *setp) noex {
    	int		si = -1 ;
	if (sp) ylikely {
	    if (cchar *tp = strnwhtbrk(sp,sl,setp) ; tp) {
	        si = intconv(tp - sp) ;
	    }
	} /* end if (non-null) */
    	return si ;
    } /* end subroutine (siwhtbrk) */
} /* end extern (C++) */


