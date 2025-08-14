/* charinit SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* character test and conversion support */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-05, David A­D­ Morano
	This module was adapted from assembly langauge.

	= 2014-08-19, David A­D­ Morano
	I changed this to use the C++ |bitset| object instead of
	an array of bytes for the single-bit truth-value observers.
	This was not really necessary since an array of |char|s
	treated as an array of bits was completely fine and worked
	just fine (essentially identical to the use of |bitset(3c++)|).

	= 2023-04-08, David A­D­ Morano
	I am taking advantage of the C++23 constant-expresssion
	enhancement |bitset(3c++)| to make the lookup tables
	constant-expression capable.

*/

/* Copyright © 1998,2014,2023 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Group:
	char

	Description:
	This module provides some character conversion tables for
	use in either the 'C' locale or when using the ISO-Latin-1
	character set (locale 'en_US.ISO8859-1').  These tables are
	provided for fast conversions when the locale is not necessary
	to be changable.  Only 8-bit characters are supported
	(ISO-Latin-1 character set).  For other character sets, use
	the system-supplied facilities.

	Notes: 
	1. Note that non-breaking-white-space (NBSP) characters are
	*not* considered to be white-space!
	2. The data tables below that convert characters to a certain
	"case" (upper, lower, and fold) are created by a supporting
	program (forget is name right now).  The table below that
	converts to a dictionary order is also created by an external
	program.  Someday, those tables should be created at module
	load-time.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<climits>		/* |UCHAR_MAX| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<bitset>		/* <- the money shot! */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<localmisc.h>		/* |UC(3misc)| */

#include	"charinit.h"


/* local defines */


/* imported namespaces */

using std::bitset ;			/* type */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    constexpr int   chtablen = (UCHAR_MAX + 1) ;
    struct charinfo {
	bitset<chtablen>	iswhite ;
	bitset<chtablen>	islc ;
	bitset<chtablen>	isuc ;
	uchar			tolc[chtablen] ;
	uchar			touc[chtablen] ;
	uchar			tofc[chtablen] ;
	uchar			toval[chtablen] ;
	constexpr void mkiswhite() noex ;
	constexpr void mkislc() noex ;
	constexpr void mkisuc() noex ;
	constexpr void mktolc() noex ;
	constexpr void mktouc() noex ;
	constexpr void mktofc() noex ;
	constexpr void mktoval() noex ;
	constexpr charinfo() noex {
	    mkiswhite() ;
	    mkislc() ;
	    mkisuc() ;
	    mktolc() ;
	    mktouc() ;
	    mktofc() ;
	    mktoval() ;
	} ; /* end ctor */
    } ; /* end struct (charinfo) */
} /* end namespace */

constexpr void charinfo::mkiswhite() noex {
	constexpr char	w[] = " \t\f\v\r" ;
	for (int i = 0 ; w[i] ; i += 1) {
	    cint	ch = w[i] ;
	    iswhite.set(ch,true) ;
	}
}
/* end method (charinfo::mkiswhite) */

constexpr void charinfo::mkislc() noex {
	for (int ch = 'a' ; ch <= 'z' ; ch += 1) {
	    islc.set(ch,true) ;
	}
	for (int ch = UC('à') ; ch <= UC('ÿ') ; ch += 1) {
	    islc.set(ch,true) ;
	}
	islc.set(UC('÷'),false) ;
	islc.set(UC('ß'),true) ; 	/* <- this is 'ss' in German */
}
/* end method (charinfo::mkislc) */

constexpr void charinfo::mkisuc() noex {
	for (int ch = 'A' ; ch <= 'Z' ; ch += 1) {
	    isuc.set(ch,true) ;
	}
	for (int ch = UC('À') ; ch <= UC('Þ') ; ch += 1) {
	    isuc.set(ch,true) ;
	}
	isuc.set(UC('×'),false) ;
}
/* end method (charinfo::mkisuc) */

constexpr void charinfo::mktolc() noex {
    	for (int ch = 0 ; ch < chtablen ; ch += 1) {
	    int nch = ch ;
	    bool f = false ;
	    f = f || ((ch >= 'A') && (ch <= 'Z')) ;
	    f = f || ((ch >= UC('À')) && (ch <= UC('Ö'))) ;
	    f = f || ((ch >= UC('Ø')) && (ch <= UC('Þ'))) ;
	    if (f) nch += 0x20 ;
	    tolc[ch] = uchar(nch) ;
	} /* end for */
}
/* end method (charinfo::mktolc) */

constexpr void charinfo::mktouc() noex {
    	for (int ch = 0 ; ch < chtablen ; ch += 1) {
	    int nch = ch ;
	    bool f = false ;
    	    f = f || ((ch >= 'a') && (ch <= 'z')) ;
	    f = f || ((ch >= UC('à')) && (ch <= UC('ö'))) ;
	    f = f || ((ch >= UC('ø')) && (ch <= UC('þ'))) ;
	    if (f) nch -= 0x20 ;
	    touc[ch] = uchar(nch) ;
	} /* end for */
}
/* end method (charinfo::mktouc) */

constexpr void charinfo::mktofc() noex {
    	for (int ch = 0 ; ch < chtablen ; ch += 1) {
	    tofc[ch] = uchar(ch) ;
	}
}
/* end method (charinfo::mktofc) */

constexpr void charinfo::mktoval() noex {
        for (int ch = 0 ; ch < chtablen ; ch += 1) {
            if ((ch >= '0') && (ch <= '9')) {
                toval[ch] = uchar(ch - '0') ;
            } else if ((ch >= 'A') && (ch <= 'Z')) {
                toval[ch] = uchar((ch - 'A') + 10) ;
            } else if ((ch >= 'a') && (ch <= 'z')) {
                toval[ch] = uchar((ch - 'a') + 36) ;
            } else if (ch == UC('Ø')) {
                toval[ch] = 62 ;
            } else if (ch == UC('ø')) {
                toval[ch] = 63 ;
            } else {
                toval[ch] = UCHAR_MAX ;
            }
        } /* end for */
}
/* end method (charinfo::mktoval) */


/* local variables */

constexpr charinfo	char_data ;


/* exported variables */

/* dictionary-collating-ordinal */
const short	char_dictorder[] = {
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x006a, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0066,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0065, 0x0000, 0x0000,
	0x03e8, 0x03e9, 0x03ea, 0x03eb, 0x03ec, 0x03ed, 0x03ee, 0x03ef,
	0x03f0, 0x03f1, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x03f2, 0x0402, 0x0404, 0x0408, 0x040a, 0x0414, 0x0416,
	0x0418, 0x041a, 0x0424, 0x0426, 0x0428, 0x042a, 0x042e, 0x0432,
	0x0440, 0x0442, 0x0444, 0x0446, 0x0448, 0x044a, 0x0454, 0x0456,
	0x0458, 0x045a, 0x045f, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0067, 0x03f9, 0x0403, 0x0406, 0x0409, 0x040f, 0x0415, 0x0417,
	0x0419, 0x041f, 0x0425, 0x0427, 0x0429, 0x042b, 0x0430, 0x0438,
	0x0441, 0x0443, 0x0445, 0x0447, 0x0449, 0x044f, 0x0455, 0x0457,
	0x0459, 0x045c, 0x0460, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0069, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0064, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0068, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x03f4, 0x03f3, 0x03f5, 0x03f8, 0x03f7, 0x03f6, 0x0400, 0x0405,
	0x040c, 0x040b, 0x040d, 0x040e, 0x041c, 0x041b, 0x041d, 0x041e,
	0x042c, 0x042f, 0x0434, 0x0433, 0x0435, 0x0437, 0x0436, 0x0000,
	0x043e, 0x044c, 0x044b, 0x044d, 0x044e, 0x045b, 0x0461, 0x0463,
	0x03fb, 0x03fa, 0x03fc, 0x03ff, 0x03fe, 0x03fd, 0x0401, 0x0407,
	0x0411, 0x0410, 0x0412, 0x0413, 0x0421, 0x0420, 0x0422, 0x0423,
	0x042d, 0x0431, 0x043a, 0x0439, 0x043b, 0x043d, 0x043c, 0x0000,
	0x043f, 0x0451, 0x0450, 0x0452, 0x0453, 0x045d, 0x0462, 0x045e
} ; /* end array (char_dictorder) */


/* exported variables */


/* exported subroutines */

bool char_iswhite(int ch) noex {
	return char_data.iswhite[ch & UCHAR_MAX] ;
}

bool char_islc(int ch) noex {
	return char_data.islc[ch & UCHAR_MAX] ;
}

bool char_isuc(int ch) noex {
	return char_data.isuc[ch & UCHAR_MAX] ;
}

int char_toval(int ch) noex {
	return char_data.toval[ch & UCHAR_MAX] ;
}


/* local subroutines */


