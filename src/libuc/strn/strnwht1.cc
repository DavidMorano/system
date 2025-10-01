/* strnwht1 MODULE (module-implementation-unit) */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* find a character in a counted string */
/* version %I% last-modified %G% */


/* revision history:

	= 1999-06-08, David A­D­ Morano
	This code was originally written for some reason.

	= 2020-03-22, David A­D­ Morano
	I modularized this (made into a C++20 module).

*/

/* Copyright © 1999,2020 David A-D- Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	strnwht
	strnwht{x}

	Description:
	These subroutines search for a break-point that is either
	a given character or a character within a given c-string,
	or failing that, it finds the first occurance of white-space
	instead.  So preference is given to finding the given
	character first (either the specified single character or
	a chracter within a character-class), and only failing that
	is the first character of white-space returned.

	Synopsis:
	char *strwht(cchar *sp,int sl) noex
	char *strwhtchr(cchar *sp,int sl,int sch) noex
	char *strwhtbrk(cchar *sp,int sl,cchar *ss) noex
	char *strwhtbrk(cchar *sp,int sl,chrset &sset) noex

	Arguments:
	sp		test c-string pointer
	sp		test c-string length
	ss		c-string of characters to compare against
	sset		CHRSET object w/ selected characters
	sch		search character to search for

	Returns:
	-		pointer to found character or NULL if not found

*******************************************************************************/

module ;

#include	<envstandards.h>	/* MUST be first to configure */
#include	<climits>		/* |CHAR_MAX| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<new>			/* |nullptr_t| */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<mkchar.h>
#include	<ischarx.h>		/* |iswhite(3uc)| */
#include	<localmisc.h>

#pragma		GCC dependency	"mod/strnwht.ccm"
#pragma		GCC dependency	"mod/libutil.ccm"
#pragma		GCC dependency	"mod/chrset.ccm"

module strnwht ;

import libutil ;			/* |getlenstr(3u)| */
import chrset ;

/* local defines */


/* local namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */

static constexpr cauto		isw = iswhite ;


/* exported variables */


/* exported subroutines */

extern "C" {
    char *strnwht(cchar *sp,int µsl) noex {
	char		*rsp = nullptr ;
	if (int sl ; (sl = getlenstr(sp,µsl)) > 0) ylikely {
	        cchar	*lsp = (sp + sl) ;
		bool	f = false ;
	        while ((sp < lsp) && *sp && (*sp != '\n')) {
		    cint	ch = mkchar(*sp) ;
		    if ((f = isw(ch))) break ;
		    sp += 1 ;
		} /* end while */
	        rsp = (f) ? charp(sp) : nullptr ;
	} /* end if (non-zero positive) */
	return rsp ;
    } /* end subroutine (strnwht) */
    char *strnwhtbrk(cchar *sp,int sl,cchar *ss) noex {
    	char		*rsp = nullptr ;
	if (sp && ss) ylikely {
    	    chrset	sset(ss) ;
	    rsp = strnwhtbrk(sp,sl,sset) ;
	}
	return rsp ;
    } /* end subroutine (strnwhtbrk) */
    char *strnwhtchr(cchar *sp,int sl,int sch) noex {
    	char		*rsp = nullptr ;
	if (sp) ylikely {
	    if (sch) ylikely {
    	        chrset sset ; sset.set(sch) ;
	        rsp = strnwhtbrk(sp,sl,sset) ;
	    } else {
		rsp = strnwht(sp,sl) ;
	    }
	} /* end if (non-null) */
	return rsp ;
    } /* end subroutine (strnwhtchr) */
} /* end extern */

extern "C++" {
    char *strnwhtbrk(cchar *sp,int µsl,const chrset &sset) noex {
	char		*rsp = nullptr ;
	if (int sl ; (sl = getlenstr(sp,µsl)) > 0) ylikely {
	        cchar	*lsp = (sp + sl) ;
	        bool	f = false ;
	        while ((sp < lsp) && *sp) {
		    cint	ch = mkchar(*sp) ;
		    if (((f = isw(ch))) || sset[ch]) {
			rsp = charp(sp) ;
			break ;
		    }
	            sp += 1 ;
	        } /* end while */
		if (f) {
	            while ((sp < lsp) && *sp) {
		        cint	ch = mkchar(*sp) ;
			if (((f = sset[ch])) || (! isw(ch))) break ;
			sp += 1 ;
		    } /* end while */
		    if (f) rsp = charp(sp) ;
		} /* end if (had white-space) */
	} /* end if (non-zero positive) */
	return rsp ;
    } /* end subroutine (strnwhtbrk) */
    char *strnwhtchr(cchar *sp,int sl,const chrset &sset) noex {
	return strnwhtbrk(sp,sl,sset) ;
    }
} /* end extern */


