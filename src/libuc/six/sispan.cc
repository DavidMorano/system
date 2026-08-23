/* sispan SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* find the index of character pass a span of a string */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-23, David A-D- Morano
	This code was originally written.

*/

/* Copyright © 1998 David A-D- Morano.  All rights reserved. */

/*******************************************************************************

	Group:
	sispan
	sispanalpha
	sispanalnum
	sispandigit
	sispandigex
	sispanwhite
	sispanblank

	Description:
	These subroutines return the number of characters skipped
	in the string due to belonging to the specified character
	class.

	Synopsis:
	int sispan(cchar *sp,int sl,cchar *class) noex
	int sispanalpha(cchar *sp,int sl) noex
	int sispanalnum(cchar *sp,int sl) noex
	int sispandigit(cchar *sp,int sl) noex
	int sispandigex(cchar *sp,int sl) noex
	int sispanwhite(cchar *sp,int sl) noex
	int sispanblank(cchar *sp,int sl) noex

	Arguments:
	sp	c-string to be examined pointer
	sl	c-string to be examined length
	class	string of characters defining a character class to skip

	Returns:
	>0	number of characters skipped
	==0	no characters were skipped
	<0	call-usage-error (should not normally happen)


	Name:
	sispanwht

	Description:
	This subroutine returns the number of leading white-space
	characters skipped in the given c-string.

	Synopsis:
	int sispanwht(cchar *sp,int sl) noex

	Arguments:
	sp	c-string pointer
	sl	c-string length

	Returns:
	>0	number of characters skipped
	==0	no characters were skipped (no whitespace)
	<0	error (null-pointer)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstring>		/* CSTD |strchr(3c)| */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<mkchar.h>		/* LIBU */
#include	<ischarx.h>		/* LIBUC |is{x}(3uc)| */
#include	<localmisc.h>		/* LIBU */

#include	"sispan.h"


/* local defines */


/* imported namespaces */


/* typedefs */

extern "C" {
    typedef bool (*isclass_f)(int) noex ;
} /* end extern (C) */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

local int sispanclass(isclass_f isclass,cchar *sp,int sl) noex {
	int		i = -1 ; /* used afterwards */
	if (sp) ylikely {
	    for (i = 0 ; sl-- && sp[i] ; i += 1) {
		cint ch = mkchar(sp[i]) ;
	        if (! isclass(ch)) break ;
	    } /* end for */
	} /* end if (non-null) */
	return i ;
} /* end subroutine (sispanclass) */
	

/* local variables */


/* exported variables */


/* exported subroutines */

int sispan(cchar *sp,int sl,cchar *strclass) noex {
	int		i = -1 ; /* return-value */
	if (sp && strclass) {
	    for (i = 0 ; sl-- && sp[i] ; i += 1) {
	        cint	ch = mkchar(sp[i]) ;
	        if (strchr(strclass,ch) == nullptr) break ;
	    } /* end for */
	} /* end if (non-null) */
	return i ;
} /* end subroutine (sispan) */

int sispanalpha(cchar *sp,int sl) noex {
    	return sispanclass(isalphalatin,sp,sl) ;
} /* end subroutine (sispanalpha) */

int sispanalnum(cchar *sp,int sl) noex {
    	return sispanclass(isalnumlatin,sp,sl) ;
} /* end subroutine (sispanalnum) */

int sispandigit(cchar *sp,int sl) noex {
    	return sispanclass(isdigitlatin,sp,sl) ;
} /* end subroutine (sispandigit) */

int sispandigex(cchar *sp,int sl) noex {
    	return sispanclass(isdigexlatin,sp,sl) ;
} /* end subroutine (sispandigex) */

int sispanoctal(cchar *sp,int sl) noex {
    	return sispanclass(isoctallatin,sp,sl) ;
} /* end subroutine (sispanoctal) */

int sispanwhite(cchar *sp,int sl) noex {
    	return sispanclass(iswhitelatin,sp,sl) ;
} /* end subroutine (sispanwhite) */

int sispanblank(cchar *sp,int sl) noex {
    	return sispanclass(isblanklatin,sp,sl) ;
} /* end subroutine (sispanblank) */


