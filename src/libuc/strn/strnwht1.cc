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
	char *strnwht(cchar *sp,int sl) noex
	char *strnwhtchr(cchar *sp,int sl,int sch) noex
	char *strnwhtbrk(cchar *sp,int sl,cchar *ss) noex
	char *strnwhtbrk(cchar *sp,int sl,con chrset *setp) noex

	Arguments:
	sp		test c-string pointer
	sp		test c-string length
	ss		c-string of characters to compare against
	setp		CHRSET object pointer w/ selected characters
	sch		search character to search for

	Returns:
	-		pointer to found character or NULL if not found

*******************************************************************************/

module ;

#include	<envstandards.h>	/* MUST be first to configure */
#include	<climits>		/* CSTD |CHAR_MAX| */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<new>			/* C++STD |nullptr_t| */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */
#include	<char.h>		/* LIBYC |CHAR_ISWHT(3uc)| */
#include	<ischarx.h>		/* LIBYC |iswht(3uc)| */
#include	<mkchar.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

#pragma		GCC dependency		"mod/strnwht.ccm"
#pragma		GCC dependency		"mod/libutil.ccm"
#pragma		GCC dependency		"mod/chrset.ccm"

module strnwht ;

import libutil ;			/* |getlenstr(3u)| */
import chrset ;

/* local defines */

#define	ISWHT(c)	CHAR_ISWHT(c)


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
    char *strnwht(cchar *sp,int µsl) noex {
	char		*rsp = nullptr ;
	if (int sl ; (sl = getlenstr(sp,µsl)) > 0) ylikely {
	    cchar	*lsp = (sp + sl) ;
	    bool	f = false ;
	    while ((sp < lsp) && *sp && (*sp != '\n')) {
		cint	ch = mkchar(*sp) ;
		if ((f = ISWHT(ch))) break ;
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
	    rsp = strnwhtbrk(sp,sl,&sset) ;
	}
	return rsp ;
    } /* end subroutine (strnwhtbrk) */
    char *strnwhtchr(cchar *sp,int sl,int sch) noex {
    	char		*rsp = nullptr ;
	if (sp) ylikely {
	    if (sch) ylikely {
    	        chrset sset ; sset.set(sch) ;
	        rsp = strnwhtbrk(sp,sl,&sset) ;
	    } else {
		rsp = strnwht(sp,sl) ;
	    }
	} /* end if (non-null) */
	return rsp ;
    } /* end subroutine (strnwhtchr) */
} /* end extern (C) */

extern "C++" {
    char *strnwhtbrk(cchar *sp,int µsl,con chrset *setp) noex {
	char		*rsp = nullptr ;
	if (int sl ; setp && (sl = getlenstr(sp,µsl)) > 0) ylikely {
	    cchar	*lsp = (sp + sl) ;
	    bool	fwht = false ;
	    while ((sp < lsp) && *sp) {
		cint	ch = mkchar(*sp) ;
		if (((fwht = ISWHT(ch))) || setp->tst(ch)) {
		    rsp = charp(sp) ;
		    break ;
		}
	        sp += 1 ;
	    } /* end while */
	    if (fwht) {
		bool fchr = false ;
	        while ((sp < lsp) && *sp) {
		    cint	ch = mkchar(*sp) ;
		    if (((fchr = setp->tst(ch))) || (! ISWHT(ch))) break ;
		    sp += 1 ;
		} /* end while */
		if (fchr) rsp = charp(sp) ;
	    } /* end if (had white-space) */
	} /* end if (non-zero positive) */
	return rsp ;
    } /* end subroutine (strnwhtbrk) */
    char *strnwhtchr(cchar *sp,int sl,con chrset *setp) noex {
	return strnwhtbrk(sp,sl,setp) ;
    } /* end subroutine */
} /* end extern (C++) */


