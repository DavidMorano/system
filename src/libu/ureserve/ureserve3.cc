/* ureserve3 MODULE */
/* charset=ISO8859-1 */
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

	Module:
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

constexpr int   chtablen = (UCHAR_MAX + 1) ;

namespace {
    struct charinfo {
	bitset<chtablen>	isalpha ;
	bitset<chtablen>	isalnum ;
	bitset<chtablen>	isdigex ;
	bitset<chtablen>	isprint ;
	bitset<chtablen>	isterm ;
	constexpr void mkalpha(bitset<chtablen> &) noex ;
	constexpr void mkisalpha() noex ;
	constexpr void mkisalnum() noex ;
	constexpr void mkisdigex() noex ;
	constexpr void mkisprint() noex ;
	constexpr void mkisterm() noex ;
	constexpr charinfo() noex {
	    mkisalpha() ;
	    mkisalnum() ;
	    mkisdigex() ;
	    mkisprint() ;
	    mkisterm() ;
	} ; /* end ctor */
    } ; /* end struct (charinfo) */
} /* end namespace */


/* forward references */

constexpr void charinfo::mkalpha(bitset<chtablen> &s) noex {
	for (int ch = 'A' ; ch <= 'Z' ; ch += 1) {
	    s.set(ch,true) ;
	    s.set((ch + 0x20),true) ;
	} /* end for */
	for (int ch = 0xC0 ; ch < chtablen ; ch += 1) {
	    s.set(ch,true) ;
	} /* end for */
	s.set(UC('×'),false) ;
	s.set(UC('÷'),false) ;
}
/* end method (charinfo::mkalpha) */

constexpr void charinfo::mkisalpha() noex {
    	mkalpha(isalpha) ;
}
/* end method (charinfo::mkisalpha) */

constexpr void charinfo::mkisalnum() noex {
    	mkalpha(isalnum) ;
	for (int ch = '0' ; ch <= '9' ; ch += 1) {
	    isalnum.set(ch,true) ;
	} /* end for */
}
/* end method (charinfo::mkisalnum) */

constexpr void charinfo::mkisdigex() noex {
	for (int ch = '0' ; ch <= '9' ; ch += 1) {
	    isdigex.set(ch,true) ;
	} /* end for */
	for (int ch = 'A' ; ch <= 'F' ; ch += 1) {
	    isdigex.set(ch,true) ;
	    isdigex.set((ch + 0x20),true) ;
	} /* end for */
}
/* end method (charinfo::mkisdigex) */

constexpr void charinfo::mkisprint() noex {
    	for (int ch = 0 ; ch < chtablen ; ch += 1) {
	    bool f = ((ch & 0x7f) >= 0x20) && (ch != CH_DEL) ;
	    f = f || (ch == CH_TAB) ;
	    if (f) {
	        isprint.set(ch,true) ;
	    }
	} /* end for */
}
/* end method (charinfo::mkisprint) */

constexpr void charinfo::mkisterm() noex {
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
		if (f) {
		    isterm.set(ch,true) ;
		}
	    } /* end if */
	} /* end for */
} /* end method (charinfo::mkisterm) */


/* local variables */

constexpr charinfo	ischarx_data ;


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

bool isdigexlatin(int ch) noex {
	bool		f = false ;
	if ((ch >= 0) && (ch < chtablen)) {
	    f = ischarx_data.isdigex[ch] ;
	}
	return f ;
}
/* end subroutine (isdigexlatin) */

bool iswhitelatin(int ch) noex {
	bool		f = false ;
	if ((ch >= 0) && (ch < chtablen)) {
	    f = char_iswhite(ch) ;
	}
	return f ;
}
/* end subroutine (iswhitelatin) */

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
	f = f || (ch == CH_SS2) ;
	f = f || (ch == CH_SS3) ;
	return f ;
}
/* end subroutine (iscmdstart) */

bool ishdrkey(int ch) noex {
	return (isalnumlatin(ch) || (ch == '-') || (ch == '_')) ;
}
/* end subroutine (ishdrkey) */


