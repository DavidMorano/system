/* stpcpyxc SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* copy a c-string to destination buffer w/ case conversions */
/* version %I% last-modified %G% */

#define	CF_STPCPY	1		/* use |stpcpy(3c)| */

/* revision history:

	= 1998-11-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Names:
	stpcpybc
	stpcpylc
	stpcpyuc
	stpcpyfc

	Description:
	These subroutines copy a source c-string to a (uncounted)
	destination while converting the case of the characters
	(according to which subroutine is called).

	Synopsis:
	rp = *stpcpy{x}c(char *dst,cchar *src) noex

	Arguments:
	dst	destination result buffer (pointer)
	src	source c-string pointer
	rp	result pointer (pointing to resulting NUL character 
		in 'dst'

	Returns:
	-	a pointer to the NUL character in the destination buffer

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstring>		/* CSTD |stpcpy(3c)| */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usupport.h>		/* LIBU */
#include	<usysdefs.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

#include	"stpcpyxc.h"


/* local defines */

#ifndef	CF_STPCPY
#define	CF_STPCPY	1		/* use |stpcpy(3c)| */
#endif


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

local char *stpcpyxc(chtoxc_f chtoxc,char *dp,cchar *sp) noex {
    	if (dp && sp) ylikely {
	    while (*sp) {
	        *dp++ = chtoxc(*sp++) ;
	    } /* end while */
	    *dp = '\0' ;
	} else {
	    dp = nullptr ;
	} /* end if (non-null) */
	return dp ;
} /* end subroutine (stpcpyxc) */

local char *stpcpyxcx(chtoxc_f chtoxc,char *dp,int na,va_list ap) noex {
	for (int i = 0 ; i < na ; i += 1) {
	    ccp sp = (charp) va_arg(ap,charp) ;
	    while (*sp) {
	        *dp++ = chtoxc(*sp++) ;
	    } /* end while */
	} /* end for */
	*dp = '\0' ;
	return dp ;
} /* end subroutine (stpcpyxcx) */


/* local variables */

constexpr bool		f_stpcpy = CF_STPCPY ;


/* exported variables */


/* exported subroutines */

char *stpcpybc(char *dp,cchar *sp) noex {
	char		*rp = nullptr ;
	if (dp && sp) ylikely {
	    if_constexpr (f_stpcpy) {
	        rp = stpcpy(dp,sp) ;
	    } else {
	        rp = stpcpyxc(chtobc,dp,sp) ;
	    } /* end */
	} /* end if (non-null) */
	return rp ;
} /* end subroutine (stpcpybc) */

char *stpcpylc(char *dp,cchar *sp) noex {
	return stpcpyxc(chtolc,dp,sp) ;
} /* end subroutine */

char *stpcpyuc(char *dp,cchar *sp) noex {
	return stpcpyxc(chtouc,dp,sp) ;
} /* end subroutine */

char *stpcpyfc(char *dp,cchar *sp) noex {
	return stpcpyxc(chtofc,dp,sp) ;
} /* end subroutine */

char *stpcpybcx(char *dp,int na,...) noex {
    	va_list ap ;
	if (dp) ylikely {
	    va_begin(ap,na) ;
	    dp = stpcpyxcx(chtobc,dp,na,ap) ;
	    va_end(ap) ;
	} /* end if (non-null) */
    	return dp ;
} /* end subroutine */

char *stpcpylcx(char *dp,int na,...) noex {
    	va_list ap ;
	if (dp) ylikely {
	    va_begin(ap,na) ;
	    dp = stpcpyxcx(chtolc,dp,na,ap) ;
	    va_end(ap) ;
	} /* end if (non-null) */
    	return dp ;
} /* end subroutine */

char *stpcpyucx(char *dp,int na,...) noex {
    	va_list ap ;
	if (dp) ylikely {
	    va_begin(ap,na) ;
	    dp = stpcpyxcx(chtouc,dp,na,ap) ;
	    va_end(ap) ;
	} /* end if (non-null) */
    	return dp ;
} /* end subroutine */

char *stpcpyfcx(char *dp,int na,...) noex {
    	va_list ap ;
	if (dp) ylikely {
	    va_begin(ap,na) ;
	    dp = stpcpyxcx(chtofc,dp,na,ap) ;
	    va_end(ap) ;
	} /* end if (non-null) */
    	return dp ;
} /* end subroutine */


