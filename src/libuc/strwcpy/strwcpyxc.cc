/* strwcpyxc SUPPORT (String-Write-Copy) */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* copy a counted c-string to a destination buffer w/ case conversion */
/* version %I% last-modified %G% */


/* revision history:

	= 2000-05-14, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2000 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	strwcpy{x}c

	Description
	Copy a source string to a destination until either the end
	of the source string is reached (by its end-marker) or the
	length of the source string is exhausted.

	Synopsis:
	char *strwcpy{x}c(char *dp,cchar *sp,int sl) noex

	Arguments:
	{x}	one of: b, l, u, f
	dp	string buffer that receives the copy
	sp	the source string that is to be copied
	sl	the maximum length to be copied

	Returns:
	-	the character pointer to the end of the destination


*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */
#include	<usupport.h>		/* LIBU |chtoxc(3u)| */
#include	<localmisc.h>		/* LIBU */

#include	"strwcpyxc.h"


/* local defines */


/* imported namespaces */

using libu::chtobc ;			/* subroutine */
using libu::chtolc ;			/* subroutine */
using libu::chtouc ;			/* subroutine */
using libu::chtofc ;			/* subroutine */


/* local typedefs */

extern "C" {
    typedef char (*chtoxc_f)(int) noex ;
} /* end extern (C) */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

template<chtoxc_f chtoxc>
local char *strwcpyxc(char *dp,cchar *sp,int sl) noex {
    	if (dp && sp) ylikely {
	    if (sl >= 0) {
	        while (sl-- && *sp) {
	            *dp++ = chtoxc(*sp++) ;
	        } /* end while */
	    } else {
	        while (*sp) {
		    *dp++ = chtoxc(*sp++) ;
	        } /* end while */
	    } /* end if */
	    *dp = '\0' ;
	} else {
	    dp = nullptr ;
	} /* end if (non-null) */
	return dp ;
} /* end subroutine-template (strwcpyxc) */


/* local variables */


/* exported variables */


/* exported subroutines */

char *strwcpybc(char *dp,cchar *sp,int sl) noex {
	return strwcpyxc<chtobc>(dp,sp,sl) ;
} /* end subroutine */

char *strwcpylc(char *dp,cchar *sp,int sl) noex {
	return strwcpyxc<chtolc>(dp,sp,sl) ;
} /* end subroutine */

char *strwcpyuc(char *dp,cchar *sp,int sl) noex {
	return strwcpyxc<chtouc>(dp,sp,sl) ;
} /* end subroutine */

char *strwcpyfc(char *dp,cchar *sp,int sl) noex {
	return strwcpyxc<chtofc>(dp,sp,sl) ;
} /* end subroutine */


