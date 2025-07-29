/* strnxchr SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* find a character in a counted c-string */
/* version %I% last-modified %G% */

#define	CF_STRCHR	1		/* use |strchr(3c)| */
#define	CF_STRRCHR	1		/* use |strrchr(3c)| */
#define	CF_MEMCHR	1		/* use |memchr(3c)| */

/* revision history:

	= 1999-06-08, David A­D­ Morano
	This subroutine was written again for Rightcore Network
	Services (RNS).

*/

/* Copyright © 1999 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	strn{x}chr

	Description:
	Yes, this is quite the same as |strchr(3c)| except that a
	length of the test string (the first argument) can be given.

	Synopsis:
	char *strn{x}chr(cchar *sp,int sl,int ch) noex

	Arguments:
	{x}		base, case, fold
	sp		string to search through
	sl		maximum number of character to search
	ch		the character to search for

	Returns:
	-		address of found character or |nullptr| if not found

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<climits>		/* |UCHAR_MAX| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>		/* |strchr(3c)| + |strrchr(3c)| */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<mkchar.h>
#include	<localmisc.h>

#include	"strnxchr.h"

import libutil ;

/* local defines */

#ifndef	CF_STRCHR
#define	CF_STRCHR	0
#endif
#ifndef	CF_STRRCHR
#define	CF_STRRCHR	0
#endif
#ifndef	CF_MEMCHR
#define	CF_MEMCHR	0
#endif


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external subroutines */


/* local structures */


/* forward references */


/* local variables */

cbool		f_strchr	= CF_STRCHR ;
cbool		f_strrchr	= CF_STRRCHR ;
cbool		f_memchr	= CF_MEMCHR ;


/* exported variables */


/* exported subroutines */

char *strnochr(cchar *sp,int sl,int sch) noex {
        cnullptr	np{} ;
	char		*rsp = nullptr ;
	if (sp) ylikely {
	    bool	f = false ;
	    sch &= UCHAR_MAX ;
	    if (sl >= 0) {
		if_constexpr (f_memchr) {
		    csize msize = size_t(sl) ;
		    if (cc *tp ; (tp = charp(memchr(sp,sch,msize))) != np) {
			rsp = charp(tp) ;
		    }
		} else {
	            for (cchar *lsp = (sp + sl) ; (sp < lsp) && *sp ; ) {
			cint	ch = mkchar(*sp) ;
	                f = (ch == sch) ;
		        if (f) break ;
	                sp += 1 ;
	            } /* end while */
		    if (f) rsp = charp(sp) ;
		} /* end if_constexpr (f_memchr) */
	    } else {
	        if_constexpr (f_strchr) {
	            rsp = strchr(sp,sch) ;
	        } else {
	            while (*sp) {
			cint	ch = mkchar(*sp) ;
	                f = (ch == sch) ;
		        if (f) break ;
	                sp += 1 ;
	            } /* end while */
		    if (f) rsp = charp(sp) ;
	        } /* end if_constexpr (f_strchr) */
	    } /* end if */
	} /* end if (non-null) */
	return rsp ;
}
/* end subroutine (strnochr) */

char *strnrchr(cchar *sp,int sl,int sch) noex {
	char		*rsp = nullptr ;
	if (sp) ylikely {
	    sch &= UCHAR_MAX ;
	    if (sl >= 0) {
	        for (cchar *csp = (sp + sl) ; rsp && (--csp >= sp) ; ) {
	            cint	ch = mkchar(*csp) ;
	            if (ch == sch) rsp = charp(csp) ;
	        } /* end while */
	    } else {
		if_constexpr (f_strrchr) {
	            rsp = strrchr(sp,sch) ;
		} else {
	            sl = lenstr(sp) ;
		    rsp = strnrchr(sp,sl,sch) ;
		} /* end if_constexpr (f_strrchr) */
	    } /* end if */
	} /* end if (non-null) */
	return rsp ;
}
/* end subroutine (strnrchr) */


