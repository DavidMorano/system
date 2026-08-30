/* convdec SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* convert a long (signed or unsigned) integer to its decimal string */
/* version %I% last-modified %G% */

#define	CF_XTOSTR	1	/* use |xtostr(3u)| subroutine */

/* revision history:

	= 1998-03-01, David A­D­ Morano
	This subroutine was adapted from an old version that was
	used in the old days at AT&T Bell Laboratores (known as
	Bell Telephone Laboratories back then).  In those days,
	processors did not have native hardware support (or "good"
	native hardware support) for 64-bit (or sometimes not even
	32-bit) division.

	= 2025-07-27, David A­D­ Morano
	I am reviewing this code file again since I found it somewhere
	and copied it into the LIBDAM library source code area
	(really for no particular reason).  I remember writing this
	originally in 1983 (in C-language pretty much as shown
	below).  This was used in the CPU circuit board for the
	so-called "data-base" processor complex of the AT&T Audix®
	Voice-Mail product.  Let us see.  How long ago was that?
	2025-1983=42 years ago now.  I am cracking myself up a
	little bit.  This code was used by my |printf| subroutine
	that I wrote at that time also (also for the Audix® product).
	I had assembly language convert-integer-to-string subroutines
	for both decimal and hexadecimal (very heavily used back
	then for hardware development), but for some reason this
	code (below) was still used within |printf|.

*/

/* Copyright © 1998,2025 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Names:
	convdecu{x}
	convdecs{x}

  	Description:
	The |convdec{x}()| subroutines convert {unsigned|signed}
	integers to printable decimal (base-10 digis) and places
	the resulting characters in a buffer relative to 'endptr'
	(a pointer to the end of a char buffer).  The value returned
	is the address of the first non-zero character.

	Synopsis:
	char *convdecs{x}(s{xx} val,char *endptr) noex
	char *convdecu{x}(u{xx} val,char *endptr) noex

	Arguments:
	{x}		one of: i, l, ll
	{xx}		one of: int, long, longlong
	val		value (unsigned or signed) to be converted
	endptr		pointer to one byte beyond the end the filled
			part of the result buffer
	
	Returns:
	-		pointer to start of convered string
	
	Implementation note:
	I develop the decimal digits in two stages.  Since speed
	counts here I do it in two loops.  The first loop gets
	"lval" ("long value") down until it is no larger than
	INT_MAX.  The second loop uses integer divides rather than
	long divides to speed it up.  Also note that I do not use
	*both* a division and a modulus operation to develop the
	digits (as some people do).  Instead I use a single division
	and a multiplication to develop the digits.  If I did this
	in assembly language on some architectures I could get a
	division and a modulus result with a single combined
	division-modulus instruction.  But in C language (a high-level
	language) I do not have this luxury, unless a compiler is
	super smart and can somehow figure out that a single
	devision-modulus instruction can provide both results at
	different points in a loop when each operation is using the
	same operands.  Some compilers have claimed to be able to
	do this, but in general I do not want to rely on that
	(rather remote) possibility here.  Of course, I assume
	that doing both a single division and a multiplication
	operation per loop iteration is over-all faster than doing
	both a division and a modulus operation per loop iteration.
	This seems to be a reasonable assumption.  On platform
	architectures that do *not* have a good hardware division
	instruction (performance), this can be a huge win.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<climits>		/* CSTD |INT_MAX| */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */
#include	<stdintx.h>		/* LIBU */
#include	<xxtostr.h>		/* LIBU |xtostr(3u)| */
#include	<stdintx.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

#include	"convdec.h"


/* local defines */

#ifndef	CF_XTOSTR
#define	CF_XTOSTR	1	/* use |ultostr(3u)| subroutine */
#endif


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

cbool		f_xtostr = CF_XTOSTR ;

template<typename UT>
local char *local_xtostrux(UT unum,char *endptr) noex {
    	UT		lval = unum ;
    	cuint		b10 = 10 ;	/* base-10 */
    	char		*bp = endptr ; /* return-value */
	/* zero is a special case */
	if (lval > 0) {
	    /* first loop to get value down to <= INT_MAX */
	    for (UT nv ; lval > INT_MAX ; lval = nv) {
	        nv = (lval / b10) ;
	        *--bp = char((lval - (nv * b10)) + '0') ;
	    } /* end for */
	    /* does not lose precision since 'ival' is <= INT_MAX */
	    for (uint nv, ival = uint(lval) ; ival > 0 ; ival = nv ) {
	        nv = (ival / b10) ;
	        *--bp = char((ival - (nv * b10)) + '0') ;
	    } /* end for */
	} else {
	    *--bp = '0' ;
	}
	return bp ;
} /* end subroutine (local_xtostrux) */

template<typename UT>
char *convdecux(UT unum,char *endptr) noex {
	char		*bp = nullptr ;
	if (endptr) ylikely {
	    *endptr = '\0' ;
	    if_constexpr (f_xtostr) {
	        bp = xtostr(unum,endptr) ;
	    } else {
	        bp = local_xtostr(unum,endptr) ;
	    }
	} /* end if (non-null) */
	return bp ;
} /* end subroutine (convdecux) */

template<typename UT,typename ST>
char *convdecsx(ST snum,char *endptr) noex {
	char		*bp = nullptr ;
	if (endptr) ylikely {
	    UT	unum = UT(snum) ;
	    if (snum < 0) unum = (neg unum) ;
	    bp = convdecux(unum,endptr) ;
	    if (snum < 0) *--bp = '-' ;
	} /* end if (non-null) */
	return bp ;
} /* end subroutine (convdecsx) */


/* local variables */


/* exported variables */


/* exported subroutines */

char *convdecsi		(sint sv,	char *endp) noex {
    	return convdecsx<uint>		(sv,endp) ;
} /* end */

char *convdecsl		(slong sv,	char *endp) noex {
    	return convdecsx<ulong>		(sv,endp) ;
} /* end */

char *convdecsll	(slonglong sv,	char *endp) noex {
    	return convdecsx<ulonglong>	(sv,endp) ;
} /* end */


char *convdecui		(uint uv,	char *endp) noex {
    	return convdecux(uv,endp) ;
} /* end */

char *convdecul		(ulong uv,	char *endp) noex {
    	return convdecux(uv,endp) ;
} /* end */

char *convdecull	(ulonglong uv,	char *endp) noex {
    	return convdecux(uv,endp) ;
} /* end */


/* local subroutines */


