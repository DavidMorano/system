/* ischx MODULE (module-implementation-unit) */
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
	ischx

	Description:
	This module provides some character conversion tables for
	use in either the 'C' locale or when using the ISO-Latin-1
	character set (locale 'en_US.ISO8859-1').  These tables are
	provided for fast conversions when the locale is not necessary
	to be changable.  Only 8-bit characters are supported
	(ISO-Latin-1 character set).  For other character sets, use
	the system-supplied facilities.

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
#include	<localmisc.h>		/* |UC(3misc)| */

module ischx ;

/* local defines */


/* imported namespaces */

using std::bitset ;			/* type */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

constexpr int   chtablen = (UCHAR_MAX + 1) ;

namespace {
    struct charinfo {
	bitset<chtablen>	isalpha ;
	bitset<chtablen>	isalnum ;
	bitset<chtablen>	iswhite ;
	bitset<chtablen>	islc ;
	bitset<chtablen>	isuc ;
	bitset<chtablen>	isfc ;
	consteval void mkalpha(bitset<chtablen> &s) noex ;
	consteval void mkisalpha() noex ;
	consteval void mkisalnum() noex ;
	consteval void mkiswhite() noex ;
	consteval void mkislc() noex ;
	consteval void mkisuc() noex ;
	consteval void mkisfc() noex ;
	consteval charinfo() noex {
	    mkisalpha() ;
	    mkisalnum() ;
	    mkiswhite() ;
	    mkislc() ;
	    mkisuc() ;
	    mkisfc() ;
	} ; /* end ctor */
    } ; /* end struct (charinfo) */
} /* end namespace */

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

consteval void charinfo::mkiswhite() noex {
	constexpr char	w[] = " \t\f\v\r" ;
	for (int i = 0 ; w[i] ; i += 1) {
	    cint	ch = w[i] ;
	    iswhite.set(ch,true) ;
	}
} /* end method (charinfo::mkiswhite) */

consteval void charinfo::mkislc() noex {
	for (int ch = 'a' ; ch <= 'z' ; ch += 1) {
	    islc.set(ch,true) ;
	}
	for (int ch = UC('à') ; ch <= UC('ÿ') ; ch += 1) {
	    islc.set(ch,true) ;
	}
	islc.set(UC('÷'),false) ;
	islc.set(UC('ß'),true) ; 	/* <- this is 'ss' in German */
} /* end method (charinfo::mkislc) */

consteval void charinfo::mkisuc() noex {
	for (int ch = 'A' ; ch <= 'Z' ; ch += 1) {
	    isuc.set(ch,true) ;
	}
	for (int ch = UC('À') ; ch <= UC('Þ') ; ch += 1) {
	    isuc.set(ch,true) ;
	}
	isuc.set(UC('×'),false) ;
} /* end method (charinfo::mkisuc) */

consteval void charinfo::mkisfc() noex {
	for (int ch = 'A' ; ch <= 'Z' ; ch += 1) {
	    isfc.set(ch,true) ;
	}
	isfc.set(UC('Ð'),false) ;
	isfc.set(UC('Þ'),false) ; /* 0xDE - 'PB' in German */
} /* end method (charinfo::mkisfc) */


/* local variables */

constexpr charinfo	ischx_data ;


/* exported variables */


/* exported variables */


/* exported subroutines */

    bool ischalpha(int ch) noex attrpure {
	return ischx_data.isalpha[ch & UCHAR_MAX] ;
    }
    bool ischalnum(int ch) noex attrpure {
	return ischx_data.isalnum[ch & UCHAR_MAX] ;
    }
    bool ischwhite(int ch) noex attrpure {
	return ischx_data.iswhite[ch & UCHAR_MAX] ;
    }
    bool ischlower(int ch) noex attrpure {
	return ischx_data.islc[ch & UCHAR_MAX] ;
    }
    bool ischupper(int ch) noex attrpure {
	return ischx_data.isuc[ch & UCHAR_MAX] ;
    }
    bool ischfold(int ch) noex attrpure {
	return ischx_data.isfc[ch & UCHAR_MAX] ;
    }


/* local subroutines */


