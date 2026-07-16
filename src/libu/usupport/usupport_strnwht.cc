/* support_strnwht SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* find a character in a counted string */
/* version %I% last-modified %G% */


/* revision history:

	= 1999-06-08, David A­D­ Morano
	This code was originally written for some reason.

*/

/* Copyright © 1999 David A-D- Morano.  All rights reserved. */

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
	char *strnwhtbrk(cchar *sp,int sl,con chrset *sset) noex

	Arguments:
	sp		test c-string pointer
	sp		test c-string length
	ss		c-string of characters to compare against
	sset		CHRSET object w/ selected characters
	sch		search character to search for

	Returns:
	-		pointer to found character or NULL if not found

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<climits>		/* CSTD |CHAR_MAX| */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<mkchar.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

#pragma		GCC dependency		"mod/libutil.ccm"
#pragma		GCC dependency		"mod/chrset.ccm"

import libutil ;			/* |getlenstr(3u)| */
import ureserve ;			/* |char(3u)| */
import chrset ;

/* local defines */

#define	ISWHT(c)	char_iswht(c)


/* local namespaces */


/* local typedefs */


/* external subroutines */

namespace libu {
    extern char *strnwhtbrk(cchar *,int,con chrset *) noex ;
    extern char *strnwhtchr(cchar *,int,con chrset *) noex ;
} /* end namespace (libu) */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

namespace libu {
    char *strnwht(cchar *sp,int µsl) noex {
	char		*rsp = nullptr ;
	if (int sl ; (sl = getlenstr(sp,µsl)) > 0) ylikely {
	    cchar	*lsp = (sp + sl) ;
	    bool	f = false ;
	    for (int ch ; (sp < lsp) && *sp && (*sp != '\n') ; sp += 1) {
		ch = mkchar(*sp) ;
		if ((f = ISWHT(ch))) break ;
	    } /* end for */
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
} /* end namespace (libu) */

namespace libu {
    char *strnwhtbrk(cchar *sp,int µsl,const chrset *setp) noex {
	char		*rsp = nullptr ;
	if (int sl ; (sl = getlenstr(sp,µsl)) > 0) ylikely {
	    cchar	*lsp = (sp + sl) ;
	    bool	fwht = false ;
	    for (int ch ; (sp < lsp) && ((ch = mkchar(*sp))) ; sp += 1) {
		if (((fwht = ISWHT(ch))) || setp->tst(ch)) {
		    rsp = charp(sp) ;
		    break ;
		}
	    } /* end for */
	    if (fwht) {
		bool fchr = false ;
		for (int ch ; (sp < lsp) && ((ch = mkchar(*sp))) ; sp += 1) {
		    if (((fchr = setp->tst(ch))) || (! ISWHT(ch))) break ;
		} /* end for */
		if (fchr) rsp = charp(sp) ;
	    } /* end if (had white-space) */
	} /* end if (non-zero positive) */
	return rsp ;
    } /* end subroutine (strnwhtbrk) */
    char *strnwhtchr(cchar *sp,int sl,const chrset *setp) noex {
	return strnwhtbrk(sp,sl,setp) ;
    }
} /* end namespace (libu) */


