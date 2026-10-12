/* strbcpyxc SUPPORT */
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
	strbcpybc
	strbcpylc
	strbcpyuc
	strbcpyfc

	Description:
	These subroutines copy a source c-string to a (uncounted)
	destination while converting the case of the characters
	(according to which subroutine is called).

	Synopsis:
	rp = *strbcpy{x}c(char *dp,int dl,cchar *src ...) noex

	Arguments:
	dp	destination result buffer pointer
	dl	destination result buffer length
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
#include	<usysdefs.h>		/* LIBU */
#include	<usupport.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

#include	"strbcpyxc.h"


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

local char *strbcpyxc(chtoxc_f chtoxc,char *dp,int dl,cchar *sp) noex {
    	if (dp && sp) ylikely {
	    if (dl >= 0) {
	        while (dl && *sp) {
	            *dp++ = chtoxc(*sp++) ;
		    dl -= 1 ;
	        } /* end while */
	        *dp = '\0' ;
	        if ((dl == 0) && *sp) dp = nullptr ;
	    } else {
	        while (*sp) {
	            *dp++ = chtoxc(*sp++) ;
		    dl -= 1 ;
	        } /* end while */
	        *dp = '\0' ;
	    } /* end if */
	} else {
	    dp = nullptr ;
	} /* end if (non-null) */
	return dp ;
} /* end subroutine (strbcpyxc) */

local char *strbcpyxcx(chtoxc_f chtoxc,char *dp,int dl,int na,va_list ap) noex {
	if (dp) ylikely {
    	    bool fbad = false ;
	    for (int i = 0 ; i < na ; i += 1) {
	        ccp sp = (charp) va_arg(ap,charp) ;
	        while (dl && *sp) {
	            *dp++ = chtoxc(*sp++) ;
		    dl -= 1 ;
	        } /* end while */
	        if ((fbad = ((dl == 0) && *sp))) break ;
	    } /* end for */
	    *dp = '\0' ;
	    if (fbad) dp = nullptr ;
	} /* end if (non-null) */
	return dp ;
} /* end subroutine (strbcpyxcx) */


/* local variables */

constexpr bool		f_stpcpy = CF_STPCPY ;


/* exported variables */


/* exported subroutines */

char *strbcpybc(char *dp,int dl,cchar *sp) noex {
	char		*rp = nullptr ;
	if (dp && sp) ylikely {
	    if (dl >= 0) {
		rp = strbcpyxc(chtobc,dp,dl,sp) ;
	    } else {
	        if_constexpr (f_stpcpy) {
	            rp = stpcpy(dp,sp) ;
	        } else {
	            rp = strbcpyxc(chtobc,dp,dl,sp) ;
	        } /* end */
	    } /* end */
	} /* end if (non-null) */
	return rp ;
} /* end subroutine (strbcpybc) */

char *strbcpylc(char *dp,int dl,cchar *sp) noex {
	return strbcpyxc(chtolc,dp,dl,sp) ;
} /* end subroutine */

char *strbcpyuc(char *dp,int dl,cchar *sp) noex {
	return strbcpyxc(chtouc,dp,dl,sp) ;
} /* end subroutine */

char *strbcpyfc(char *dp,int dl,cchar *sp) noex {
	return strbcpyxc(chtofc,dp,dl,sp) ;
} /* end subroutine */

char *strbcpybcx(char *dp,int dl,int na,...) noex {
    	va_list ap ;
	if (dp) ylikely {
	    va_begin(ap,na) ;
	    dp = strbcpyxcx(chtobc,dp,dl,na,ap) ;
	    va_end(ap) ;
	} /* end if (non-null) */
    	return dp ;
} /* end subroutine */

char *strbcpylcx(char *dp,int dl,int na,...) noex {
    	va_list ap ;
	if (dp) ylikely {
	    va_begin(ap,na) ;
	    dp = strbcpyxcx(chtolc,dp,dl,na,ap) ;
	    va_end(ap) ;
	} /* end if (non-null) */
    	return dp ;
} /* end subroutine */

char *strbcpyucx(char *dp,int dl,int na,...) noex {
    	va_list ap ;
	if (dp) ylikely {
	    va_begin(ap,na) ;
	    dp = strbcpyxcx(chtouc,dp,dl,na,ap) ;
	    va_end(ap) ;
	} /* end if (non-null) */
    	return dp ;
} /* end subroutine */

char *strbcpyfcx(char *dp,int dl,int na,...) noex {
    	va_list ap ;
	if (dp) ylikely {
	    va_begin(ap,na) ;
	    dp = strbcpyxcx(chtofc,dp,dl,na,ap) ;
	    va_end(ap) ;
	} /* end if (non-null) */
    	return dp ;
} /* end subroutine */


