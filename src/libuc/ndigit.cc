/* ndigit SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* calculate the number of digits in a number (integer) */
/* version %I% last-modified %G% */

#define	CF_MATH		1		/* use C-C++ math */

/* revision history:

	= 1998-12-01, David A­D­ Morano
	This module was originally written.

	= 2025-04-28, David A­D­ Morano
	Inspired by what I just did with the facility |digbufsizes(3u)|
	I modified this old subroutine (actually this subroutine way
	predates 1998 but who is counting), I decided to try that
	closed-form math calculation thing here.  Is this really
	faster then repeated division, I do not really know?  Why
	do I say that?  Because taking a logarithm of something
	could be time consuming depending on how it is implemented.
	I am assuming that taking a logarithm is going to be much
	faster then the repeated-division algorithm needed for the
	very large types handled here.

*/

/* Copyright © 1998,2025 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	ndigit{x}

	Description:
	This is a support subroutine that is supposed to be cheaper
	than an integer-to-ascii conversion in order to get the
	number of digits in an integer.  Note that this subroutine
	is essentially getting the logarithm of the given number
	but alleviates having to load-link the whole math library
	in order to just get the |LOG(3math)| subroutine from it.
	But it still uses a divide in its algorithm (at least only
	an integer divide).  But a divide is, well, a divide.  And
	all divides are expensive.  And without full divide hardware
	(which used to exist in the old days), a divide can be very
	expensive!

	Synopsis:
	int ndigit{x}(int v,int base) noex

	Arguments:
	v	value to find the number of ASCII digits for
	base	number-base to use

	Returns:
	-	number of base-<base> digits in given number

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<climits>		/* |CHAR_BIT| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>		/* |abs(3c)| */
#include	<cmath>			/* |log2(3c++)| + |ceil(3c++)| */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<localmisc.h>

#include	"ndigit.h"


/* local defines */

#ifndef	CF_MATH
#define	CF_MATH		1		/* use C-C++ math */
#endif


/* imported namespaces */

using std::abs ;			/* C++ subroutine (template) */
using std::log2 ;			/* C++ subroutine (template) */
using std::ceil ;			/* C++ subroutine (template) */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

template<typename T> static int ndivx(T v,int b) noex {
	int		n = 0 ;
	if (v >= 0) {
	    const uint	ub = uint(b) ;	/* unsigned division is faster? */
	    if (b >= 2) {
	        if (v == 0) {
	            n = 1 ;
	        } else {
	            while (v != 0) {
	                n += 1 ;
	                v = (v / ub) ;
	            }
		} /* end if (non-zero positive) */
	    } /* end if (valid base) */
	} /* end if (valid) */
	return n ;
}
/* end subroutine-template (ndivx) */

template<typename T> static T abs(T v) noex {
    	return 0 ;
}

template<> [[maybe_unused]] longlong abs(longlong v) noex {
    	if (v < 0) v = (- v) ;
    	return v ;
}


/* local variables */

cbool		f_math = CF_MATH ;


/* local subroutine-templates */

template<typename T> static int ndigitx(T v,int b) noex {
    	int		n = 0 ;
	if (b >= 2) {
	    if_constexpr (f_math && (szof(T) <= szof(long))) {
	        clongdouble 	fv = longdouble(v) ;
	        {
		    cdouble	fbase = double(b) ;
	            cdouble	fbits = double(log2(fv)) ;
		    n = int(ceil(fbits / log2(fbase))) ;
	        }
	    } else {
	        n = ndivx(v,b) ;
	    } /* end if_constexpr (f_math) */
	} /* end if (valid base) */
    	return n ;
} /* end subroutine-template (ndigitx) */


/* exported variables */


/* exported subroutines */

int ndigiti(int v,int b) noex {
    	cuint		uv = uint(abs(v)) ;
	return ndigitx(uv,b) + 1 ;
}
int ndigitl(long v,int b) noex {
    	culong		uv = ulong(abs(v)) ;
	return ndigitx(uv,b) + 1 ;
}
int ndigitll(longlong v,int b) noex {
    	culonglong	uv = ulonglong(abs(v)) ;
	return ndigitx(uv,b) + 1 ;
}

int ndigitui(uint uv,int b) noex {
	return ndigitx(uv,b) ;
}
int ndigitul(ulong uv,int b) noex {
	return ndigitx(uv,b) ;
}
int ndigitull(ulonglong uv,int b) noex {
	return ndigitx(uv,b) ;
}


