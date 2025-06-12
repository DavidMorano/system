/* ischarx SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* test a character for a property */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

	= 2014-08-19, David A­D­ Morano
	I changed this to use the C++ |bitset| object instead of
	an array of bytes (holding bits for lookup) for some of the
	single-bit truth-value observers.  I also changed the
	|ishexlatin()| subroutine to use a lookup table (implemented
	w/ |bitset|) rather than computing the answer on the fly.
	Oringally, a look-up table was used to implement that
	subroutine (I used to use that particular subroutine tons).
	But a benchmark analysis years ago determined (sort of
	against the prevailing thinking) that the computed version
	of the subroutine was much faster than the look-up table
	version.  I think that I do know the reason for that after
	all.  So some time ago, years ago now (without making a
	revision note above), I changed that subroutine to use the
	computed version (both versions were in the code with only
	a compile-time preprocessor define used to determine which
	version got compiled in).  I also took out (erased out) the
	computed version of that subroutine with this update and
	also removed the associated preprocessor define.  So the
	current code only has a look-up table version -- in the
	form of looking up the value inside of a |bitset| object
	-- and it is probably slower than the previous computed
	version.  But whatever, I am not so obscessed with performance
	as I once was.  Rather, now-a-days we all (including myself
	I think) are more obscessed with having readable code.  The
	old look-up table version was (indeed) readable, but not
	quite as elegant as using the |bitset| object.  Anyway,
	there you have it.

	= 2023-10-23, David A-D- Morano
	I changed the helper object ('ischarx_data') to be initialized
	w/ C++11 |constexpr| keyword.  I did it because ... it could
	be done (|bitset(3c++)| became constant-expression capable
	in C++23).  There is not really a better reason.

*/

/* Copyright © 1998,2014,2023 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	isalphalatin
	isdigitlatin
	isalnumlatin
	ishexlatin
	islowerlatin
	isupperlatin
	isnumsign
	isprintlatin
	isprintterm
	isprintbad
	iswhite
	isdict
	ismmclass_{x}
	iscmdstart
	iseol
	iszero

	Description:
	These subroutines are sort of like |isalpha(3c)| and their
	friends, but allow for ISO Latin-1 (ISO-8859-1) characters
	also.

	Synopsis:
	int ischarx(int ch) noex

	Arguments:
	ch		character to test

	Returns:
	0		false
	1		true


	Name:
	isprintbad

	Description:
	Determine if a character is bad to print.

	Synopsis:
	int isprintbad(int ch) noex

	Arguments:
	ch		character to check

	Returns:
	true		character is not printable
	false		character is printable


	Name:
	isdict

	Description:
	This subroutine is similar to the |isalnum(3c)| subroutine
	except that a space character is considered a character (but
	is otherwise not used in a comparison).

	Synopsis:
	int isdict(int ch) noex

	Arguments:
	ch	character to test

	Returns:
	false	character is not a dictionary-significant character
	true	character is a dictionary-significant character


	Name:
	iscmdstart

	Description:
	This subroutine checks if the given (passed) character could
	be the start of a terminal command.

	Synopsis:
	int iscmdstart(int ch) noex

	Arguments:
	ch		character to test

	Returns:
	0		no
	1		yes


	Notes:
	1. It is 2014.  This implemetation has remnants of using the
	|char(3uc)| facility for some of our functions (determining
	lower or upper case for example).  I do not apologize for
	that.  That old |char(3uc)| facility has earned its place
	in solid gold for all the work is has done over these last
	few (about three) decades (and more).  This all (almost
	everything here and elsewhere) came from my old embedded days
	in the 1980s, when I had to literally write every single
	thing from absolute scratch.  So I do not apologize for still
	using (grandfathered in) some of my old code pieces.  Enjoy.
		-- 2014-08-19, David A-D- Morano

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<climits>		/* |UCHAR_MAX| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<bitset>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<ascii.h>
#include	<char.h>
#include	<localmisc.h>

#include	"contentencodings.h"
#include	"ischarx.h"


/* local defines */


/* imported namespaces */

using std::bitset ;			/* type */


/* local typedefs */


/* external subroutines */


/* external variables */


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
	    f = CHAR_ISLC(ch) ;
	}
	return f ;
}
/* end subroutine (islowerlatin) */

bool isupperlatin(int ch) noex {
	bool		f = false ;
	if ((ch >= 0) && (ch < chtablen)) {
	    f = CHAR_ISUC(ch) ;
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
	    f = CHAR_ISWHITE(ch) ;
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

/* is it 7-bit text (no controls or other weirdo) */
bool ismmclass_7bit(int ch) noex {
	bool		f = false ;
	ch &= UCHAR_MAX ;
	if (ch < 0x80) {
	    f = f || isprintlatin(ch) ;
	    f = f || (ch == '\n') ;
	    f = f || (ch == '\r') ;
	    f = f || (ch == CH_TAB) ;
	    f = f || (ch == CH_SP) ;
	}
	return f ;
}
/* end subroutine (ismmclass_7bit) */

/* does it *require* 8-bit but *only* 8-bit */
bool ismmclass_8bit(int ch) noex {
	bool		f = false ;
	ch &= UCHAR_MAX ;
	if ((ch >= 0x80) && (ch < chtablen)) {
	    f = ((ch & 0x7f) >= 0x20) ;
	}
	return f ;
}
/* end subroutine (ismmclass_8bit) */

/* does it *require* binary */
bool ismmclass_binary(int ch) noex {
	bool		f = false ;
	ch &= UCHAR_MAX ;
	if (((ch & 0x7f) < 0x20) || (ch == CH_DEL)) {
	    f = true ;
	    f = f && (ch != CH_TAB) ;
	    f = f && (ch != '\n') ;
	    f = f && (ch != '\r') ;
	}
	return f ;
}
/* end subroutine (ismmclass_binary) */


/* COMMENTS */

/* comments: |ishexlatin| */
/* = 2011-08-19, David A­D­ Morano */
/* for records-keeping reasons, here is the old computed version */

/****

For the record, this version below (computed) was (substantially)
faster than my older look-up table version (as determined by
benchmarks).  But using |bitset(3c++)| is probably the slowest of
them all.  But whatever.  I am not so much into performing speed
benchmarks now-a-days.

****/

/****

The -- even older (and original) -- look-up table version is not
recorded here, but may be in some old archives.  For the record,
my old (very old now) look-up table version was probably faster
than the |bitset(3c++)| object look-up, but who is counting?

****/

#ifdef	COMMENT
bool ishexlatin(int ch) noex {
	bool		f = false ;
	f = f || ((ch >= '0') && (ch <= '9')) ;
	f = f || ((ch >= 'a') && (ch <= 'f')) ;
	f = f || ((ch >= 'A') && (ch <= 'F')) ;
	return f ;
}
/* end subroutine (ishexlatin) */
#endif /* COMMENT */


