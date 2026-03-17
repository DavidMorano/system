/* base64 SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* perform BASE64 encoding and decoding */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-08-10, David A­D­ Morano
	This code was originally written.

	= 2025-05-02, David A-D- Morano
	I changed this to use a compile-time constant-expression
	object to initialize the decode table.  Yes, I am blown
	away how long something can be in service before it is
	noticed for some sort of coding update.  This module (these
	subroutines) really dates back to the early 1990s (when I
	originally wrote this) -- when the very encoding method
	itself (BASE-64) was created by that guy (Nathaniel S-
	Borenstein et al).  Of course, that was as part of the
	creation of the MIME protocols for email.  So this code has
	been lying here peacefully for ... about 30+ years without
	ever being noticed (before now).  Is this funny (with my
	weird sense of humor) or tragic (that it took so long to
	modernize this)?  I find this pretty funny (weird me) and
	I am laughing right now.  Of course this could have been
	done back in 2014 w/ C++14 (11 years ago).

*/

/* Copyright © 1998,2025 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Group:
	base64_e		- encode a group
	base64_d		- decode a group
	base64_enc		- access to the encoder table
	base64_dec		- access to the decoder table

	Description:
	I perform both BASE64 encoding and decoding operations
	with the subroutines |base64_e| and |base64_d| respectively.
	The |base64_dec| subroutine provides access to the decoder
	table.  This encoding method is extensively documented
	elsehere.  So interested readers are encouraged to seek
	out the standard documentation.

	Synopsis:
	int base64_e(cchar *inbuf,int len,char *outbuf) noex
	int base64_d(cchar *sp,int sl,char *outbuf) noex
	int base64_enc(int ch) noex
	int base64_dec(int ch) noex

	Arguments:
	inbuf		source data pointer to encode
	len		source data length
	sp		source string
	sl		length of source string
	outbuf		result buffer (must be at least ¾ times input length)
	ch		character to encode or decode

	Returns:
	>=0		length of result in bytes
	>=0		result of decoding (when |base64_dec| was called)
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<climits>		/* |UCHAR_MAX| + |CHAR_BIT| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstdint>		/* |uint32_t| */
#include	<bit>			/* |countl_zero(3c++)| */
#include	<concepts>
#include	<clanguage.h>
#include	<usysbase.h>
#include	<mkchar.h>		/* |mkchar(3uc)| */
#include	<localmisc.h>

#include	"base64.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| */

import findbit ;			/* |ffbs(3u)| */

/* local defines */


/* imported namespaces */

using std::integral ;			/* concept */
using std::countl_zero ;		/* subroutine */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

constexpr int		base = 64 ;
constexpr int		chx_equal = 0xFE ; /* special value in decoder table */

/* encode table */
const uchar	base64_et[] = {
	'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 
	'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 
	'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 
	'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f', 
	'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 
	'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 
	'w', 'x', 'y', 'z', '0', '1', '2', '3', 
	'4', '5', '6', '7', '8', '9', '+', '/'
} ; /* end array (base64_et) */

namespace {
    constexpr int	tablen = (UCHAR_MAX + 1) ;
    struct mkdecoder {
	uchar		tab[tablen] ;
	consteval mkdecoder() noex {
	    for (int j = 0 ; j < tablen ; j += 1) {
		tab[j] = uchar(UCHAR_MAX) ;
	    } /* end for */
	    for (int i = 0 ; i < nelem(base64_et) ; i += 1) {
	        uchar	uch = base64_et[i] ;
	        tab[uch] = uchar(i) ;
	    } /* end for */
	    tab['='] = chx_equal ;	/* <- special value */
	} ; /* end ctor */
    } ; /* end struct (mkdecoder) */
} /* end namespace */


/* forward references */

template <integral T> constexpr int nzeros(T v) noex {
    	return ffbs(v) ;
} /* end subroutine (nzeros) */

local int	base64_dg(cchar *,char *) noex ;

local void	base64_eg(cchar *,char *) noex ;


/* local variables */

constexpr mkdecoder	base64decode ;

constexpr int		bits		= nzeros(base) ;
constexpr int		bmask		= (base - 1) ;
constexpr int		stagelen	= BASE64_STAGELEN ;
constexpr int		outlen		= BASE64_OUTLEN ;

constexpr bool		f_comment = false ;


/* exported variables */


/* exported subroutines */

int base64_e(cchar *inbuf,int inlen,char *outbuf) noex {
    	int		i = 0 ;
	int		ol = 0 ; /* return-value */
	while ((inlen - i) >= 3) {
	    base64_eg((inbuf + i),(outbuf + ol)) ;
	    i += 3 ;
	    ol += 4 ;
	} /* end while */
	if ((inlen - i) > 0) {
	    char altinbuf[4] ;
	    switch (inlen - i) {
	    case 1:
	        altinbuf[0] = inbuf[i] ;
	        altinbuf[1] = 0x00 ;
	        altinbuf[2] = 0x00 ;
	        base64_eg(altinbuf,(outbuf + ol)) ;
	        outbuf[ol + 2] = '=' ;
	        outbuf[ol + 3] = '=' ;
	        ol += 4 ;
	        break ;
	    case 2:
	        altinbuf[0] = inbuf[i + 0] ;
	        altinbuf[1] = inbuf[i + 1] ;
	        altinbuf[2] = 0x00 ;
	        base64_eg(altinbuf,outbuf + ol) ;
	        outbuf[ol + 3] = '=' ;
	        ol += 4 ;
	        break ;
	    } /* end switch */
	} /* end block */
	return ol ;
}
/* end subroutine (base64_e) */

/* decode */
int base64_d(cchar *inbuf,int len,char *outbuf) noex {
	int		j = 0 ; /* return-value */
	for (int i = 0 ; (j >= 0) && (len >= 4) ; ) {
	    if (int dl ; (dl = base64_dg((inbuf + i),(outbuf + j))) >= 0) {
	        len -= 4 ;
	        i += 4 ;
	        j += dl ;
	    } else {
		j = -1 ;
	    }
	} /* end for */
	return j ;
}
/* end subroutine (base64_d) */

int base64_enc(int v) noex {
    	return int(base64_et[v & bmask]) ;
}

int base64_dec(int v) noex {
    	return int(base64decode.tab[v & UCHAR_MAX]) ;
}


/* local subroutines */

/* encode a group */
local void base64_eg(cchar *inbuf,char *outbuf) noex {
	uint32_t	hold = 0 ;
	cint		n = stagelen ;
	for (int i = 0 ; i < n ; i += 1) {
	    hold <<= CHAR_BIT ;
	    hold |= uint32_t(inbuf[i] & UCHAR_MAX) ;
	} /* end for */
	for (int i = 0 ; i < outlen ; i += 1) {
	    cint	idx = int((hold >> ((n - i) * bits)) & bmask) ;
	    outbuf[i] = base64_et[idx] ;
	} /* end for */
}
/* end subroutine (base64_eg) */

/* decode a group */
local int base64_dg(cchar *inbuf,char *outbuf) noex {
	uint32_t	hold = 0 ;
	int		rs = SR_OK ;
	int		dlen = 0 ; /* return-value */
	for (int i = 0 ; i < outlen ; i += 1) {
	    cint	ich = mkchar(inbuf[i]) ;
	    if (int ch ; (ch = mkchar(base64decode.tab[ich])) != UCHAR_MAX) {
	        hold <<= bits ;
	        if (ch != chx_equal) {
	            hold |= uint32_t(ch) ;
	            dlen += 1 ;
	        }
	    } else {
	        rs = SR_INVALID ;
	    }
	    if (rs < 0) break ;
	} /* end for */
	if (rs >= 0) ylikely {
	    if_constexpr (f_comment) {
		cint nn = (stagelen - 1) ;
	        for (int i = 0 ; i < stagelen ; i += 1) {
	            outbuf[i] = char(hold >> ((nn - i) * CHAR_BIT)) ;
	        }
	        dlen -= 1 ;
	    } else {
	        outbuf[0] = char(hold >> 16) ;
	        outbuf[1] = char(hold >> 8) ;
	        outbuf[2] = char(hold >> 0) ;
	        dlen -= 1 ;
	    } /* end if_constexpr (f_comment) */
	} /* end if (ok) */
	return (rs >= 0) ? dlen : rs ;
}
/* end subroutine (base64_dg) */


