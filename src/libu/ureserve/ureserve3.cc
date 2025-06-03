/* ureserve3 MODULE */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* reserved interfaces */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-21, David A­D­ Morano
	This module was originally written.

	= 2020-05-07, David A-D- Morano
	I converted this (formerly a header-only file) to a module.

*/

/* Copyright © 1998,2020 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Family:
	ureserve

	Description:
	This module provides some subroutines are used in certain
	circumstances where the LIBUC library is not availble (for
	whatever reasons).

*******************************************************************************/

module ;

#include	<envstandards.h>	/* MUST be first to configure */
#include	<climits>		/* |UCHAR_MAX| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<bitset>		/* <- the money shot! */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<ascii.h>
#include	<localmisc.h>

module ureserve ;

/* local defines */


/* imported namespaces */

using std::bitset ;			/* type */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* local structures */

constexpr int   chtablen = (UCHAR_MAX+1) ;

namespace {
    struct ischarinfo {
	bitset<chtablen>	isprint ;
	bitset<chtablen>	isalpha ;
	bitset<chtablen>	isalnum ;
	bitset<chtablen>	ishex ;
	bitset<chtablen>	isterm ;
	constexpr void mkalpha(bitset<chtablen> &) noex ;
	constexpr void mkisprint() noex ;
	constexpr void mkisalpha() noex ;
	constexpr void mkisalnum() noex ;
	constexpr void mkishex() noex ;
	constexpr void mkisterm() noex ;
	constexpr ischarinfo() noex {
	    mkisprint() ;
	    mkisalpha() ;
	    mkisalnum() ;
	    mkishex() ;
	    mkisterm() ;
	} ; /* end ctor */
    } ; /* end struct (ischarinfo) */
} /* end namespace */


/* forward references */

constexpr void ischarinfo::mkalpha(bitset<chtablen> &s) noex {
	for (int ch = 'A' ; ch <= 'Z' ; ch += 1) {
	    s.set(ch,true) ;
	    s.set((ch + 0x20),true) ;
	}
	for (int ch = 0xC0 ; ch < chtablen ; ch += 1) {
	    s.set(ch,true) ;
	}
	s.set(UC('×'),false) ;
	s.set(UC('÷'),false) ;
}
/* end method (ischarinfo::mkalpha) */

constexpr void ischarinfo::mkisprint() noex {
    	for (int ch = 0 ; ch < chtablen ; ch += 1) {
	    bool f = ((ch & 0x7f) >= 0x20) && (ch != CH_DEL) ;
	    f = f || (ch == CH_TAB) ;
	    if (f) {
	        isprint.set(ch,true) ;
	    }
	}
}
/* end method (ischarinfo::mkisprint) */

constexpr void ischarinfo::mkisalpha() noex {
    	mkalpha(isalpha) ;
}
/* end method (ischarinfo::mkisalpha) */

constexpr void ischarinfo::mkisalnum() noex {
    	mkalpha(isalnum) ;
	for (int ch = '0' ; ch <= '9' ; ch += 1) {
	    isalnum.set(ch,true) ;
	}
}
/* end method (ischarinfo::mkisalnum) */

constexpr void ischarinfo::mkishex() noex {
	for (int ch = '0' ; ch <= '9' ; ch += 1) {
	    ishex.set(ch,true) ;
	}
	for (int ch = 'A' ; ch <= 'F' ; ch += 1) {
	    ishex.set(ch,true) ;
	    ishex.set((ch + 0x20),true) ;
	}
}
/* end method (ischarinfo::mkishex) */

constexpr void ischarinfo::mkisterm() noex {
    	for (int ch = 0 ; ch < chtablen ; ch += 1) {
	    if (((ch & 0x7f) >= 0x20) && (ch != CH_DEL)) {
		isterm.set(ch,true) ;
	    } else {
		bool	f = false ;
	        f = f || (ch == CH_TAB) ;
	        f = f || (ch == CH_CR) ;
	        f = f || (ch == CH_NL) ;
	        f = f || (ch == CH_BS) ;
	        f = f || (ch == CH_BEL) ;
	        f = f || (ch == CH_VT) || (ch == CH_FF) ;
	        f = f || (ch == CH_SO) || (ch == CH_SI) ;
	        f = f || (ch == CH_SS2) || (ch == CH_SS3) ;
		if (f) isterm.set(ch,true) ;
	    }
	} /* end for */
} /* end method (ischarinfo::mkisterm) */


/* local variables */

constexpr ischarinfo	ischarx_data ;


/* exported variables */


/* exported subroutines */

bool isalphalatin(int ch) noex {
	bool		f = false ;
	if ((ch >= 0) && (ch < chtablen)) {
	    f = ischarx_data.isalpha[ch] ;
	}
	return f ;
}
/* end subroutine (isalphalatin) */

bool isalnumlatin(int ch) noex {
	bool		f = false ;
	if ((ch >= 0) && (ch < chtablen)) {
	    f = ischarx_data.isalnum[ch] ;
	}
	return f ;
}
/* end subroutine (isalnumlatin) */

bool ishexlatin(int ch) noex {
	bool		f = false ;
	if ((ch >= 0) && (ch < chtablen)) {
	    f = ischarx_data.ishex[ch] ;
	}
	return f ;
}
/* end subroutine (ishexlatin) */

bool islowerlatin(int ch) noex {
	bool		f = false ;
	if ((ch >= 0) && (ch < chtablen)) {
	    f = char_islc(ch) ;
	}
	return f ;
}
/* end subroutine (islowerlatin) */

bool isupperlatin(int ch) noex {
	bool		f = false ;
	if ((ch >= 0) && (ch < chtablen)) {
	    f = char_isuc(ch) ;
	}
	return f ;
}
/* end subroutine (isupperlatin) */

bool isprintlatin(int ch) noex {
	bool		f = false ;
	if ((ch >= 0) && (ch < chtablen)) {
	    f = ischarx_data.isprint[ch] ;
	}
	return f ;
}
/* end subroutine (isprintlatin) */

bool isprintterm(int ch) noex {
	bool		f = false ;
	if ((ch >= 0) && (ch < chtablen)) {
	    f = ischarx_data.isterm[ch] ;
	}
	return f ;
}
/* end subroutine (isprintterm) */

bool isprintbad(int ch) noex {
	return (! isprintlatin(ch)) ;
}
/* end subroutine (isprintbad) */

bool iswhite(int ch) noex {
	bool		f = false ;
	if ((ch >= 0) && (ch < chtablen)) {
	    f = char_iswhite(ch) ;
	}
	return f ;
}
/* end subroutine (iswhite) */

bool isdict(int ch) noex {
	bool		f = false ;
	if ((ch >= 0) && (ch < chtablen)) {
	    f = ischarx_data.isalnum[ch] || (ch == CH_SP) ;
	}
	return f ;
}
/* end subroutine (isdict) */

bool iscmdstart(int ch) noex {
	bool		f = false ;
	f = f || (ch == CH_ESC) ;
	f = f || (ch == CH_CSI) ;
	f = f || (ch == CH_DCS) ;
	f = f || (ch == CH_SS3) ;
	return f ;
}
/* end subroutine (iscmdstart) */

bool ishdrkey(int ch) noex {
	return (isalnumlatin(ch) || (ch == '-') || (ch == '_')) ;
}
/* end subroutine (ishdrkey) */


