/* digval SUPPORT (Character-Digit-To-Value) */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* get the value of a single numeric digit */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-10-01, David A­D­ Morano
	These subroutines were originally adapted from assembly.
	The original assembly goes wa...ay back.

	= 2026¬03-07, David A­D­ Morano
	I changed this to make use of a decoding table for hexadecial
	decoding, made at compile time (introduced really at C++14).
	I think I had too much time on my hands.  I will add: I am
	always amazed (although I should not be) how long a piece
	of code lasts before it gets a change.  Ha-le-lu-jah for
	how long code can last!  I do not know for sure (it has
	been a long time) but I am wondering if these simple
	subroutines actually date from the late 1970s or the early
	1980s, and if they were originally written in assembly
	language (likely VAX-11 assembly language).  These may or
	may not have also been written in M86k asembly language.
	Sadly, I actually remember the year 1998 (a busy year) like
	yesterday, and especially the Summer and Fall of 1998.

*/

/* Copyright © 1998,2026 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Group:
	digval

	Names:
	digval
	digvalbin
	digvaloct
	digvaldec
	digvalhex

	Description:
	We examine a single character, of some number base, and
	return either an error or the integer value of the digit.
	For example, the hexadecimal character-digit 'b' would
	return the decimal value '11'.

	Synopsis:
	int digval(int ch) noex
	int digvalbin(int ch) noex
	int digvaloct(int ch) noex
	int digvaldec(int ch) noex
	int digvalhex(int ch) noex

	Arguments:
	ch		character to evaluate

	Outputs:
	>=0		value of symbolic digit
	<0		error (system-return)

	Notes:
	subroutine	domain
	-----------------------------------------------------------
	digval		base-64		(checked)
	digvalbin	base-2		(checked)
	digvaloct	base-8		(checked)
	digvaldec	base-10		(checked)
	digvalhex	base-16		(checked + case-insensitive)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<climits>		/* |UCHAR_MAX| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<usysbase.h>
#include	<char.h>		/* |char_toval(3uc)| */
#include	<localmisc.h>

#include	"digval.h"


/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

constexpr int   chtablen = (UCHAR_MAX + 1) ;

namespace {
   struct decoder {
	uchar		hextab[chtablen] ;
	constexpr void mkhextab() noex ;
	constexpr decoder() noex {
	    mkhextab() ;
	} ; /* end ctor */
    } ; /* end struct (decoder) */
} /* end namespace */

constexpr void decoder::mkhextab() noex {
        for (int ch = 0 ; ch < chtablen ; ch += 1) {
            if ((ch >= '0') && (ch <= '9')) {
                hextab[ch] = uchar(ch - '0') ;
            } else if ((ch >= 'A') && (ch <= 'Z')) {
                hextab[ch] = uchar((ch - 'A') + 10) ;
            } else if ((ch >= 'a') && (ch <= 'z')) {
                hextab[ch] = uchar((ch - 'a') + 10) ; /* <- '10' */
            } else {
                hextab[ch] = UCHAR_MAX ;
            }
        } /* end for */
} /* end method (decoder::mkhextab) */


/* forward references */

local inline int digvalx(int chx_m,int ch) noex {
	int		v = SR_DOM ;
	if ((ch >= '0') && (ch <= chx_m)) {
	    v = (ch - '0') ;
	}
	return v ;
} /* end subroutine (digvalx) */


/* local variables */

constexpr decoder	hexdecode ;


/* exported variables */


/* exported subroutines */

int digval(int ch) noex {
	int		v = char_toval(ch) ; /* base-64 decode */
	if (v == UCHAR_MAX) v = SR_DOM ;
	return v ;
}
/* end subroutine (digval) */

int digvalbin(int ch) noex {
    	return digvalx('1',ch) ;
}
/* end subroutine (digvalbin) */

int digvaloct(int ch) noex {
    	return digvalx('7',ch) ;
}
/* end subroutine (digvaloct) */

int digvaldec(int ch) noex {
    	return digvalx('9',ch) ;
}
/* end subroutine (digvaldec) */

int digvalhex(int ch) noex {
	int		v = hexdecode.hextab[ch & UCHAR_MAX] ;
	if (v == UCHAR_MAX) v = SR_DOM ;
	return v ;
}
/* end subroutine (digvalhex) */

#ifdef	COMMENT /* previous code */
int digvalhex(int ch) noex {
	int		v = SR_DOM ;
	if ((ch >= '0') && (ch <= '9')) {
	    v = (ch - '0') ;
	} else {
	    if (cint lch = CHAR_TOLC(ch) ; (lch >= 'a') && (lch <= 'f')) {
	        v = ((lch - 'a') + 10) ;
	    }
	}
	return v ;
}
/* end subroutine (digvalhex) */
#endif /* COMMENT */


