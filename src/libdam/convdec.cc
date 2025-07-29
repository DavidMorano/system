/* convdec SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* subroutine to convert integers to strings */
/* version %I% last-modified %G% */

#define	CF_ULTOSTR	1	/* use |ultostr(3u)| subroutine */

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
	originally in 1983 (in C-language just as shown below).
	Yes, that is when I first wrote this.  This was used in the
	CPU circuit board for the so-called "data-base" processor
	complex of the AT&T Audix® Voice-Mail product.  Let us see.
	How long ago was that?  2025-1983=42 years ago now.  I am
	cracking myself up a little bit.  Yes, I am an older man
	now.  This code was used by my |printf| subroutine that I
	wrote at that time also (also for the Audix® product).  I
	had assembly language convert-integer-to-string subroutines
	for both decimal and hexadecimal (very heavily used back
	then for hardware development), but for some reason this
	code (below) was still used within |printf|.  Interestingly,
	the |printf| subroutine was not actually used a lot (or at
	all) in the ROM and debug monitor code back then.  This was
	most likely because I had to write the ROM and debug monitor
	codes before I wrote the higher-level library subroutines
	like |printf|.  Almost everyting was "bare-metal" back in
	the 1980s.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Names:
	convdecu
	convdecs

  	Description:
	The |convdec{x}()| subroutines convert the {unsigned|signed}
	long (64-bit) integer "lval" to printable decimal and places
	it in a buffer identified as relative to 'endptr' (a pointer
	to a char buffer).  The value returned is the address of
	the first non-zero character.  Short of assembly language
	magical smart stuff (which I have coded elsewhere), this
	implementation of decimal conversion should be reasonably
	fast (or fast enough), despite its simplicity, for our
	purposes.

	Synopsis:
	char *convdecu(ulong lval,char *endptr) noex
	char *convdecs(ulong lval,char *endptr) noex

	Arguments:
	lval		value (unsigned or signed) to be converted
	endptr		pointer to one byte beyond the end of a result buffer
	
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
#include	<stdintx.h>
#include	<cstring>		/* |INT_MAX| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<xxtostr.h>		/* |ultostr(3u)| */
#include	<localmisc.h>

#include	"convdec.h"


/* local defines */

#ifndef	CF_ULTOSTR
#define	CF_ULTOSTR	1	/* use |ultostr(3u)| subroutine */
#endif


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

static char	*local_ultostr(ulong,char *) noex ;


/* local variables */

cbool		f_ultostr = CF_ULTOSTR ;


/* exported variables */


/* exported subroutines */

char *convdecu(ulong unum,char *endptr) noex {
	char		*bp = endptr ;
	if (endptr) ylikely {
	    *endptr = '\0' ;
	    if_constexpr (f_ultostr) {
	        bp = ultostr(unum,endptr) ;
	    } else {
	        bp = local_ultostr(unum,endptr) ;
	    }
	} /* end if (non-null) */
	return bp ;
} /* end subroutine (convdec) */

char *convdecs(long snum,char *endptr) noex {
	char		*bp = nullptr ;
	if (endptr) {
	    ulong	unum = ulong(snum) ;
	    if (snum < 0) unum = (- unum) ;
	    bp = convdecu(unum,endptr) ;
	    if (snum < 0) *--bp = '-' ;
	} /* end if (non-null) */
	return bp ;
} /* end subroutine (convdecs) */


/* local subroutines */

static char *local_ultostr(ulong unum,char *endptr) noex {
    	ulong		lval = unum ;
    	char		*bp = endptr ;
	/* zero is a special case */
	if (lval > 0) {
	    /* first loop to get value down to <= INT_MAX */
	    for (ulong nv ; lval > INT_MAX ; lval = nv) {
	        nv = (lval / 10) ;
	        *--bp = char((lval - (nv * 10)) + '0') ;
	    } /* end for */
	    /* does not lose precision since 'ival' is <= INT_MAX */
	    for (uint nv,ival = uint(lval) ; ival > 0 ; ival = nv ) {
	        nv = (ival / 10) ;
	        *--bp = char((ival - (nv * 10)) + '0') ;
	    } /* end for */
	} else {
	    *--bp = '0' ;
	}
	return bp ;
} /* end subroutine (local_convdecu) */


