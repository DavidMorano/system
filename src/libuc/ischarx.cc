/* ischarx SUPPORT */
/* lang=C++20 */

/* test a character for a property */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

	= 2011-08-19, David A­D­ Morano
	I changed this to use the C++11 |bitset| object instead of
	an array of bytes (holding bits for lookup) for some of the
	single-bit truth-value observers. I also changed the
	|ishexlatin()| subroutine to use a lookup table (implemented
	w/ C++11 |bitset|) rather than computing the answer on the
	fly.  Oringally, a look-up table was used to implement that
	subroutine (I used to use that particular subroutine tons).
	But a benchmark analysis years ago determined (sort of
	against the prevailing thinking) that the computed version
	of the subroutine was much faster than the look-up table
	version.  I think that I do know the reason for that after
	all.  So some time ago, years ago now (without making a
	revision note above), I changed that subroutine to use the
	computed version (both versions were in the code with only
	a compile-time preprocessor define used to determine which
	version got compiled in). I also took out (erased out) the
	computed version of that subroutine with this update and
	also removed the associated preprocessor define. So the
	current code only has a look-up table version -- in the
	form of looking up the value inside of a C++11 |bitset|
	object -- and it is probably slower than the previous
	computed version. But whatever, I am not so obscessed with
	performance as I once was.  Rather, now-a-days we all
	(including myself I think) are more obscessed with having
	readable code.  The old look-up table version was (indeed)
	readable, but not quite as elegant as using the C++11
	|bitset| object.  Anyway, there you have it.

	= 2023-10-23, David A-D- Morano
	I changed the instantiated helper object ('ischarx_data') to
	be initialized w/ C++20 |constinit| keyword.  I did it because
	... it could be done. There is not really a better reason.

*/

/* Copyright © 1998,2011,2023 David A­D­ Morano.  All rights reserved. */

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
	ismmclass
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
	except that it checks if characters are "dictionary"
	significant.  Dictionary significant characters are:
		letters
		digits

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
	1. It is 2011. This implemetation has remnants of using the
	|char(3uc)| facility for some of our functions (determining
	lower or upper case for example).  I do not apologize for
	that.  That old |char(3uc)| facility has earned its place
	in solid gold for all the work is has done over these last
	few (about three) decades (and more).  This all (almost
	everything here and elsewhere) came from my old embedded days
	in the 1980s, when I had to litterally write every single
	thing from absolute scratch.  So I do not apologize for still
	using (grandfathered in) some of my old code pieces. Enjoy.
		-- 2011-08-19, David A-D- Morano

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<climits>
#include	<cstdlib>
#include	<bitset>
#include	<utypedefs.h>
#include	<clanguage.h>
#include	<ascii.h>
#include	<stdfnames.h>
#include	<matstr.h>
#include	<char.h>
#include	<localmisc.h>

#include	"contentencodings.h"
#include	"ischarx.h"


/* local defines */


/* imported namespaces */

using std::bitset ;


/* local typedefs */


/* external subroutines */


/* external variables */


/* forward references */


/* local variables */

constexpr int   chtablen = (UCHAR_MAX+1) ;


/* local structures */

namespace {
    struct ischarinfo {
	bitset<chtablen>	isalpha ;
	bitset<chtablen>	isalnum ;
	bitset<chtablen>	ishex ;
	constexpr void mkisalpha() noex ;
	constexpr void mkisalnum() noex ;
	constexpr void mkishex() noex ;
	constexpr ischarinfo() noex {
	     mkisalpha() ;
	     mkisalnum() ;
	     mkishex() ;
	} ;
    } ; /* end struct (ischarinfo) */
}


/* local methods */

constexpr void ischarinfo::mkisalpha() noex {
	for (int ch = 'A' ; ch <= 'Z' ; ch += 1) {
	    isalpha.set(ch,true) ;
	    isalpha.set((ch+0x20),true) ;
	}
	for (int ch = 0xC0 ; ch < 0x100 ; ch += 1) {
	    isalpha.set(ch,true) ;
	}
	isalpha.set(UC('×'),false) ;
	isalpha.set(UC('÷'),false) ;
}
/* end method (ischarinfo::mkisalpha) */

constexpr void ischarinfo::mkisalnum() noex {
	for (int ch = '0' ; ch <= '9' ; ch += 1) {
	    isalnum.set(ch,true) ;
	}
	for (int ch = 'A' ; ch <= 'Z' ; ch += 1) {
	    isalnum.set(ch,true) ;
	    isalnum.set((ch+0x20),true) ;
	}
	for (int ch = 0xC0 ; ch < 0x100 ; ch += 1) {
	    isalnum.set(ch,true) ;
	}
	isalnum.set(UC('×'),false) ;
	isalnum.set(UC('÷'),false) ;
}
/* end method (ischarinfo::mkisalnum) */

constexpr void ischarinfo::mkishex() noex {
	for (int ch = '0' ; ch <= '9' ; ch += 1) {
	    ishex.set(ch,true) ;
	}
	for (int ch = 'A' ; ch <= 'F' ; ch += 1) {
	    ishex.set(ch,true) ;
	    ishex.set((ch+0x20),true) ;
	}
}
/* end method (ischarinfo::mkishex) */


/* local variables */

static constexpr ischarinfo	ischarx_data ;


/* exported variables */


/* exported subroutines */

bool isalphalatin(int ch) noex {
	bool		f = false ;
	if ((ch >= 0) && (ch < 0x100)) {
	    f = ischarx_data.isalpha[ch] ;
	}
	return f ;
}
/* end subroutine (isalphalatin) */

bool isalnumlatin(int ch) noex {
	bool		f = false ;
	if ((ch >= 0) && (ch < 0x100)) {
	    f = ischarx_data.isalnum[ch] ;
	}
	return f ;
}
/* end subroutine (isalnumlatin) */

bool ishexlatin(int ch) noex {
	bool		f = false ;
	if ((ch >= 0) && (ch < 0x100)) {
	    f = ischarx_data.ishex[ch] ;
	}
	return f ;
}
/* end subroutine (ishexlatin) */

bool islowerlatin(int ch) noex {
	bool		f = false ;
	if ((ch >= 0) && (ch < 0x100)) {
	    f = CHAR_ISLC(ch) ;
	}
	return f ;
}
/* end subroutine (islowerlatin) */

bool isupperlatin(int ch) noex {
	bool		f = false ;
	if ((ch >= 0) && (ch < 0x100)) {
	    f = CHAR_ISUC(ch) ;
	}
	return f ;
}
/* end subroutine (isupperlatin) */

bool isnumsign(int ch) noex {
	return ((ch == '+') || (ch == '-')) ;
}

bool isprintlatin(int ch) noex {
	bool		f = false ;
	if ((ch >= 0) && (ch < 0x100)) {
	    f = ((ch & 0x7f) >= 0x20) && (ch != CH_DEL) ;
	    f = f || (ch == CH_TAB) ;
	}
	return f ;
}
/* end subroutine (isprintlatin) */

bool isprintterm(int ch) noex {
	bool		f = isprintlatin(ch) ;
	f = f || (ch == CH_BEL) ;
	f = f || (ch == CH_TAB) ;
	f = f || (ch == CH_BS) ;
	f = f || (ch == CH_CR) ;
	f = f || (ch == CH_NL) ;
	f = f || (ch == '\v') ;
	f = f || (ch == '\f') ;
	f = f || (ch == CH_SO) || (ch == CH_SI) ;
	f = f || (ch == CH_SS2) || (ch == CH_SS3) ;
	return f ;
}
/* end subroutine (isprintterm) */

bool isprintbad(int ch) noex {
	return (! isprintlatin(ch)) ;
}
/* end subroutine (isprintbad) */

bool iswhite(int ch) noex {
	bool		f = false ;
	if ((ch >= 0) && (ch < 0x100)) {
	    f = CHAR_ISWHITE(ch) ;
	}
	return f ;
}
/* end subroutine (iswhite) */

bool isdict(int ch) noex {
	bool		f = false ;
	if ((ch >= 0) && (ch < 0x100)) {
	    f = (ch < 0x80) && ischarx_data.isalnum[ch] ;
	    f = f || ((ch >= 0xC0) && (ch != 0xD7) && (ch != 0xF7)) ;
	}
	return f ;
}
/* end subroutine (isdict) */

/* is it 7-bit text (no controls or other weirdo) */
bool ismmclass_7bit(int ch) noex {
	bool		f = false ;
	ch &= UCHAR_MAX ;
	if (ch < 128) {
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
	if ((ch >= 128) && (ch < 0x100)) {
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
	    f = f && (ch != '\n') ;
	    f = f && (ch != '\r') ;
	    f = f && (ch != CH_TAB) ;
	}
	return f ;
}
/* end subroutine (ismmclass_binary) */

bool isfnamespecial(cchar *fp,int fl) noex {
	return (matstr(stdfnames,fp,fl) >= 0) ;
}
/* end subroutine (isfnamespecial) */

bool iscmdstart(int ch) noex {
	bool		f = false ;
	f = f || (ch == CH_ESC) ;
	f = f || (ch == CH_CSI) ;
	f = f || (ch == CH_DCS) ;
	f = f || (ch == CH_SS3) ;
	return f ;
}
/* end subroutine (iscmdstart) */


/* COMMENTS */

/* comments: ishexlatin */
/* = 2011-08-19, David A­D­ Morano */
/* for records-keeping reasons, here is the old computed version */

/****
For the record, this version below (computed) was (substantially)
faster than my older look-up table version (as shown with benchmarks).
But the newer (C++11) |bitset| version is probably the slowest of
them all.  But whatever. I am not so much into performing speed
benchmarks now-a-days.
****/

/****
The -- even older (and original) -- look-up table version is
not recorded here, but may be in some old archives. For the record,
my old (very old now) look-up table version was probably faster
than the C++11 |bitset| object look-up, but who is counting?
****/

#ifdef	COMMENT
int ishexlatin(int ch) noex {
	bool		f = false ;
	f = f || ((ch >= '0') && (ch <= '9')) ;
	f = f || ((ch >= 'a') && (ch <= 'f')) ;
	f = f || ((ch >= 'A') && (ch <= 'F')) ;
	return f ;
}
/* end subroutine (ishexlatin) */
#endif /* COMMENT */


