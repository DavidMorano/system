/* ischarx SUPPORT */
/* charset=ISO8859-1 */
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
	|isdigexlatin()| subroutine to use a lookup table (implemented
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
	isalnumlatin
	isdigitlatin
	isdigexlatin
	iswhitelatin
	isalnumlatin
	isprintlatin
	isprintterm
	islowerlatin
	isupperlatin
	isprintbad
	isnumsign
	isdict
	iscmdstart
	iseol
	iszero
	ishdrkey

	Aliases:
	isdiglatin
	isoctlatin
	isdeclatin
	ishexlatin
	iswhtlatin
	iswhite

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


	Name:
	ishdrkey

	Description:
	Determine if the characters is valid in the context of a
	mail-message header-key value-string.

	Synopsis:
	bool ishdrkey(int ch) noex

	Arguments:
	ch		character to check

	Returns:
	true		yes, is a mail-message header-key character
	false		no, is not


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
    struct charinfo {
	bitset<chtablen>	isalpha ;
	bitset<chtablen>	isalnum ;
	bitset<chtablen>	isdigex ;
	bitset<chtablen>	isprint ;
	bitset<chtablen>	isterm ;
	consteval void mkalpha(bitset<chtablen> &) noex ;
	consteval void mkisalpha() noex ;
	consteval void mkisalnum() noex ;
	consteval void mkisdigex() noex ;
	consteval void mkisprint() noex ;
	consteval void mkisterm() noex ;
	consteval charinfo() noex {
	    mkisalpha() ;
	    mkisalnum() ;
	    mkisdigex() ;
	    mkisprint() ;
	    mkisterm() ;
	} ; /* end ctor */
    } ; /* end struct (charinfo) */
} /* end namespace */


/* forward references */

consteval void charinfo::mkalpha(bitset<chtablen> &s) noex {
	for (int ch = 'A' ; ch <= 'Z' ; ch += 1) {
	    s.set(ch,true) ;
	    s.set((ch + 0x20),true) ;
	}
	for (int ch = 0xC0 ; ch < chtablen ; ch += 1) {
	    s.set(ch,true) ;
	}
	s.set(UC('×'),false) ;
	s.set(UC('÷'),false) ;
} /* end method (charinfo::mkalpha) */

consteval void charinfo::mkisalpha() noex {
    	mkalpha(isalpha) ;
} /* end method (charinfo::mkisalpha) */

consteval void charinfo::mkisalnum() noex {
    	mkalpha(isalnum) ;
	for (int ch = '0' ; ch <= '9' ; ch += 1) {
	    isalnum.set(ch,true) ;
	}
} /* end method (charinfo::mkisalnum) */

consteval void charinfo::mkisdigex() noex {
	for (int ch = '0' ; ch <= '9' ; ch += 1) {
	    isdigex.set(ch,true) ;
	}
	for (int ch = 'A' ; ch <= 'F' ; ch += 1) {
	    isdigex.set(ch,true) ;
	    isdigex.set((ch + 0x20),true) ;
	}
} /* end method (charinfo::mkisdigex) */

consteval void charinfo::mkisprint() noex {
    	for (int ch = 0 ; ch < chtablen ; ch += 1) {
	    bool f = ((ch & 0x7f) >= 0x20) && (ch != CH_DEL) ;
	    f = f || (ch == CH_TAB) ;
	    if (f) {
	        isprint.set(ch,true) ;
	    }
	} /* end for */
} /* end method (charinfo::mkisprint) */

constexpr uchar		termchars[] = {
	CH_TAB,	CH_CR,	CH_NL,	CH_BS,
	CH_BEL,	CH_VT,	CH_FF, 
	CH_SO,	CH_SI,	CH_SS2,	CH_SS3,
	CH_ESC,	CH_CSI,	CH_DCS
} ; /* end array (termchars) */

consteval void charinfo::mkisterm() noex {
    	for (int ch = 0 ; ch < chtablen ; ch += 1) {
	    if (((ch & 0x7f) >= 0x20) && (ch != CH_DEL)) {
		isterm.set(ch,true) ;
	    }
	} /* end for */
	for (cauto &ch : termchars) {
	    isterm.set(ch,true) ;
	} /* end for */
} /* end method (charinfo::mkisterm) */


/* local variables */

constexpr charinfo	ischarx_data ;


/* exported variables */


/* exported subroutines */

bool isalphalatin(int ch) noex {
	bool		f = false ;
	if ((ch >= 0) && (ch < chtablen)) ylikely {
	    f = ischarx_data.isalpha[ch] ;
	}
	return f ;
}
/* end subroutine (isalphalatin) */

bool isalnumlatin(int ch) noex {
	bool		f = false ;
	if ((ch >= 0) && (ch < chtablen)) ylikely {
	    f = ischarx_data.isalnum[ch] ;
	}
	return f ;
}
/* end subroutine (isalnumlatin) */

bool isdigitlatin(int ch) noex {
    	return ((ch >= '0') && (ch <= '9')) ;
}
/* end subroutine (isdigitlatin) */

bool isdigexlatin(int ch) noex {
	bool		f = false ;
	if ((ch >= 0) && (ch < chtablen)) ylikely {
	    f = ischarx_data.isdigex[ch] ;
	}
	return f ;
}
/* end subroutine (isdigexlatin) */

bool iswhitelatin(int ch) noex {
	bool		f = false ;
	if ((ch >= 0) && (ch < chtablen)) ylikely {
	    f = CHAR_ISWHITE(ch) ;
	}
	return f ;
}
/* end subroutine (iswhitelatin) */

bool islowerlatin(int ch) noex {
	bool		f = false ;
	if ((ch >= 0) && (ch < chtablen)) ylikely {
	    f = CHAR_ISLC(ch) ;
	}
	return f ;
}
/* end subroutine (islowerlatin) */

bool isupperlatin(int ch) noex {
	bool		f = false ;
	if ((ch >= 0) && (ch < chtablen)) ylikely {
	    f = CHAR_ISUC(ch) ;
	}
	return f ;
}
/* end subroutine (isupperlatin) */

bool isprintlatin(int ch) noex {
	bool		f = false ;
	if ((ch >= 0) && (ch < chtablen)) ylikely {
	    f = ischarx_data.isprint[ch] ;
	}
	return f ;
}
/* end subroutine (isprintlatin) */

bool istermlatin(int ch) noex {
    	return isprintterm(ch) ;
}

bool isprintterm(int ch) noex {
	bool		f = false ;
	if ((ch >= 0) && (ch < chtablen)) ylikely {
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
	if ((ch >= 0) && (ch < chtablen)) ylikely {
	    f = ischarx_data.isalnum[ch] || (ch == CH_SP) ;
	}
	return f ;
}
/* end subroutine (isdict) */

namespace {
    struct cmdstarter {
	bitset<chtablen>	tab ;
	consteval void mktab() noex ;
	consteval cmdstarter() noex {
	    mktab() ;
	} ; /* end ctor */
	constexpr bool operator [] (int ch) const noex {
	    return tab[ch & UCHAR_MAX] ;
	} ;
    } ; /* end struct (cmdstarter) */
} /* end namespace */

constexpr uchar		cmdchars[] = {
	CH_ESC,
	CH_CSI,
	CH_DCS,
	CH_SO,
	CH_SI,
	CH_SS2,
	CH_SS3
} ; /* end array (cmdchars) */

consteval void cmdstarter::mktab() noex {
    	for (cauto &ch : cmdchars) {
	    tab.set(ch,true) ;
	}
} /* end method (cmdstarter::mktab) */

constexpr cmdstarter	cmd_data ;

bool iscmdstart(int ch) noex {
    	return cmd_data[ch] ;
}
/* end subroutine (iscmdstart) */

bool ishdrkey(int ch) noex {
	return (isalnumlatin(ch) || (ch == '-') || (ch == '_')) ;
}
/* end subroutine (ishdrkey) */


/* COMMENTS */

/* comments: |isdigexlatin| */
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
bool isdigexlatin(int ch) noex {
	bool		f = false ;
	f = f || ((ch >= '0') && (ch <= '9')) ;
	f = f || ((ch >= 'a') && (ch <= 'f')) ;
	f = f || ((ch >= 'A') && (ch <= 'F')) ;
	return f ;
}
/* end subroutine (issigexlatin) */
#endif /* COMMENT */


