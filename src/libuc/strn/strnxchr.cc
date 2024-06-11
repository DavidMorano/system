/* strnxchr SUPPORT */
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
	sp		string to search through
	sl		maximum number of character to search
	ch		the character to search for

	Returns:
	-		address of found character or |nullptr| if not found

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<climits>		/* <- for |UCHAR_MAX| */
#include	<cstring>		/* |strlen(3c)| + |strchr(3c)| + ... */
#include	<utypedefs.h>
#include	<clanguage.h>
#include	<mkchar.h>

#include	"strnxchr.h"


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

typedef char	*charp ;


/* external subroutines */


/* external subroutines */


/* forward references */


/* local variables */

constexpr bool		f_strchr = CF_STRCHR ;
constexpr bool		f_strrchr = CF_STRRCHR ;
constexpr bool		f_memchr = CF_MEMCHR ;


/* exported variables */


/* exported subroutines */

char *strnochr(cchar *sp,int sl,int sch) noex {
	char		*rsp = nullptr ;
	if (sp) {
	    bool	f = false ;
	    sch &= UCHAR_MAX ;
	    if (sl < 0) {
	        if constexpr (f_strchr) {
	            rsp = strchr(sp,sch) ;
	        } else {
	            while (*sp) {
	                f = (mkchar(*sp) == sch) ;
		        if (f) break ;
	                sp += 1 ;
	            } /* end while */
		    if (f) rsp = charp( sp) ;
	        } /* end if_constexpr (f_strchr) */
	    } else {
		if constexpr (f_memchr) {
		    cchar	*tp = charp(memchr(sp,sch,sl)) ;
		    if (tp) {
			rsp = charp(tp) ;
		    }
		} else {
	            cchar	*lsp = (sp + sl) ;
	            while ((sp < lsp) && *sp) {
	                f = (mkchar(*sp) == sch) ;
		        if (f) break ;
	                sp += 1 ;
	            } /* end while */
		    if (f) rsp = charp(sp) ;
		} /* end if */
	    } /* end if */
	} /* end if (non-null) */
	return rsp ;
}
/* end subroutine (strnochr) */

char *strnrchr(cchar *sp,int sl,int sch) noex {
	char		*rsp = nullptr ;
	if (sp) {
	    bool	fdone = false ;
	    sch &= UCHAR_MAX ;
	    if (sl < 0) {
		if constexpr (f_strrchr) {
		    fdone = true ;
	            rsp = strrchr(sp,sch) ;
		} else {
	            sl = strlen(sp) ;
		} /* end if_constexpr (f_strrchr) */
	    } /* end if (no length given) */
	    if ((!fdone) && (sl >= 0)) {
	        bool	f = false ;
	        cchar	*csp = (sp+sl) ;
	        while (--csp >= sp) {
	            cint	ch = mkchar(*csp) ;
	            f = (ch == sch) ;
	            if (f) break ;
	        } /* end while */
		if (f) rsp = ((char *) csp) ;
	    } /* end if */
	} /* end if (non-null) */
	return rsp ;
}
/* end subroutine (strnrchr) */


