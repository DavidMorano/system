/* sfxchr SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* get a substring present *before* the first dot */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-07-01, David A­D­ Morano
	This subroutine gets the leading string before the first
	dot character (white-space cleaned up).

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Names:
	sfochr
	afrchr

	Description:
	These subroutines search for the specified character and
	then return the c-string **before** the found character.
	the returned c-string is white-space-shrunked before being
	returned.  Both searching forward (obverse) from the beginning
	of the given c-string, and searching backward from the
	(reverse) from the end of the given c-string is provided.

	Synopsis:
	int sfochr(ccharp sp,int sl,cchar **rpp) noex
	int afrchr(ccharp sp,int sl,cchar **rpp) noex

	Arguments:
	sp	given search c-string pointer
	sl	given search c-string length
	rpp	pointer to hold pointer to result string if found

	Returns:
	>=0	number of bytes found in given c-string
	<0	the character was **not** found in the given string

	Philosophy:
	The terminator is *required* in order for a non-empty result
	string to be returned.

	Notes:
	1. The terminator is *required* to be found in order for a
	non-empty result string to be returned.
	2. The returned string (if found) is what is found **before**
	the given c-string.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<strn.h>		/* LIBUC |strnxchr(3uc)| */
#include	<char.h>		/* LIBUC |CHAR_ISWHITE(3uc)| */
#include	<localmisc.h>		/* LIBU */

#include	"sfx.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| */

/* local defines */

#define	ISWHT(ch)	CHAR_ISWHITE(ch)


/* imported namespaces */


/* local typedefs */

extern "C" {
    typedef charp (*strxchr_f)(cchar *,int,int) noex ;
} /* end extern (C) */


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct sub_sfxchr {
	strxchr_f fun ;
	sub_sfxchr(strxchr_f f) noex : fun(f) { } ;
	int operator () (cchar *,int,int,cchar **) noex ;
    } ; /* end struct (sub_sfxchr) */
} /* end namespace */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int sfochr(cchar *sp,int sl,int sch,cchar **rpp) noex {
	sub_sfxchr so(strnochr) ;
	return so(sp,sl,sch,rpp) ;
} /* end subroutine */

int sfrchr(cchar *sp,int sl,int sch,cchar **rpp) noex {
	sub_sfxchr so(strnrchr) ;
	return so(sp,sl,sch,rpp) ;
} /* end subroutine */


/* local subroutines */

int sub_sfxchr::operator () (cchar *sp,int µsl,int sch,cchar **rpp) noex {
	int		rl = -1 ; /* return-value */
	cchar		*rp = nullptr ;
	if (int sl = getlenstr(sp,µsl) ; sl > 0) ylikely {
	    while (sl && ISWHT(*sp)) {
	        sp += 1 ;
	        sl -= 1 ;
	    } /* end while */
	    if (cchar *tp = fun(sp,sl,sch)) {
	        rl = intconv(tp - sp) ;
		rp = sp ;
	        while (rl && ISWHT(rp[rl - 1])) {
	            rl -= 1 ;
	        } /* end white */
	    } /* end if (hit) */
	} /* end if (getlenstr) */
	if (rpp) *rpp = rp ;
	return rl ;
} /* end method (sub_sfxchr::operator) */


