/* base128 SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* perform BASE64 encoding and decoding */
/* version %I% last-modified %G% */


/* revision history:

	= 2025-05-03, David A-D- Morano
	This code was based on the same (sort-of) function that I
	originally wrote for Aruba Networks.  It is also modeled
	after the module |base64(3uc)| (which I wrote from time
	immemorial).

*/

/* Copyright © 1998,2025 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Group:
	base128_e		- encode a group
	base128_d		- decode a group
	base128_enc		- access to the encoder table
	base128_dec		- access to the decoder table

	Description:
	These subroutines faciilitate both the encoding and decoding
	of the so-called 'base128' coding scheme.  This is (currently)
	not standardized.  Further, unlike the very popular 'base64'
	coding scheme, this present scheme performs a little-endian
	approch towards the byte encoding.

	Synopsis:
	int base128_e(cchar *inbuf,int len,char *outbuf) noex
	int base128_d(cchar *sp,int sl,char *outbuf) noex
	int base128_enc(int ch) noex
	int base128_dec(int ch) noex

	Arguments:
	inbuf		source data pointer to encode
	len		source data length
	sp		source string
	sl		length of source string
	outbuf		result buffer (must be at least ¾ times input length)
	ch		character to decode (through the decoding table)

	Returns:
	>=0		length of result in bytes
	>=0		result of decoding (when |base128_dec| was called)
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<climits>		/* |UCHAR_MAX| + |CHAR_BIT| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstdint>		/* |uint64_t| */
#include	<bit>			/* |countl_zero(3c++)| */
#include	<concepts>
#include	<clanguage.h>
#include	<usysbase.h>
#include	<mkchar.h>		/* |mkchar(3uc)| */
#include	<localmisc.h>

#include	"base128.h"

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

constexpr int		base = 128 ;
constexpr int		enclen = 128 ;
constexpr int		chx_equal = 0xFE ; /* special value in decoder table */

namespace {
    constexpr int	tablen = (UCHAR_MAX + 1) ;
    struct helper {
	uchar		enc[enclen] ;
	uchar		dec[tablen] ;
	constexpr void helper_mkenc() noex {
    	    int	i = 0 ;
	    for (int j = 0 ; j < 10 ; j += 1) {
	        enc[i++] = uchar('0' + j) ;
	    }
	    for (int j = 0 ; j < 26 ; j += 1) {
	        enc[i++] = uchar('A' + j) ;
	    }
	    for (int j = 0 ; j < 26 ; j += 1) {
	        enc[i++] = uchar('a' + j) ;
	    }
	    for (int j = 0 ; j < (16+7) ; j += 1) {
	        enc[i++] = uchar(UC('À') + j) ;
	    }
	    for (int j = 0 ; j < 31 ; j += 1) {
	        enc[i++] = uchar(UC('Ø') + j) ;
	    }
	    for (int j = 0 ; j < 8 ; j += 1) {
	        enc[i++] = uchar(UC('ø') + j) ;
	    }
	    enc[i++] = '°' ;
	    enc[i++] = '¹' ;
	    enc[i++] = '¹' ;
	    enc[i++] = '³' ;
	} ; /* end sub (helper_mkenc) */
	constexpr void helper_mkdec() noex {
	    for (int j = 0 ; j < tablen ; j += 1) {
		dec[j] = uchar(UCHAR_MAX) ;
	    }
	    for (int i = 0 ; i < nelem(enc) ; i += 1) {
	        uchar	uch = enc[i] ;
	        dec[uch] = uchar(i) ;
	    } /* end for */
	    dec['='] = chx_equal ;	/* <- special value */
	} ; /* end sub (helper_mkdec) */
	constexpr helper() noex {
	    helper_mkenc() ;
	    helper_mkdec() ;
	} ; /* end ctor */
    } ; /* end struct (helper) */
} /* end namespace */


/* forward references */

template <integral T> consteval int nzeros(T v) noex {
    	return ffbs(v) ;
} /* end subroutine (nzeros) */

local int		base128_dg(cchar *,char *) noex ;

local void		base128_eg(cchar *,char *) noex ;


/* local variables */

constexpr helper	base128mgr ;

constexpr int		bits = nzeros(base) ;

constexpr bool		f_comment = false ;


/* exported variables */


/* exported subroutines */

int base128_e(cchar *inbuf,int inlen,char *outbuf) noex {
    	int		i = 0 ;
	int		ol = 0 ; /* return-value */
	while ((inlen - i) >= 7) {
	    base128_eg((inbuf + i),(outbuf + ol)) ;
	    i += 7 ;
	    ol += 8 ;
	} /* end while */
	if (cint n = (inlen - i) ; n > 0) {
	    char abuf[8] ;
	    memcopy(abuf,inbuf,n) ;
	    memclear((abuf + n),(8 - n)) ;
	    base128_enc(abuf,(obuf + ol)) ;
	    switch (inlen - i) {
	    case 1:
	        altinbuf[0] = inbuf[i] ;
	        altinbuf[1] = 0x00 ;
	        altinbuf[2] = 0x00 ;
	        base128_eg(altinbuf,(outbuf + j)) ;
	        outbuf[ol + 2] = chx_equal ;
	        outbuf[ol + 3] = '=' ;
	        ol += 4 ;
	        break ;
	    case 2:
	        altinbuf[0] = inbuf[i] ;
	        altinbuf[1] = inbuf[i + 1] ;
	        altinbuf[2] = 0x00 ;
	        base128_eg(altinbuf,outbuf + ol) ;
	        outbuf[ol + 3] = '=' ;
	        ol += 4 ;
	        break ;
	    } /* end switch */
	} /* end block */
	return ol ;
}
/* end subroutine (base128_e) */

/* decode */
int base128_d(cchar *inbuf,int len,char *outbuf) noex {
	int		j = 0 ; /* return-value */
	for (int i = 0 ; (j >= 0) && (len >= 4) ; ) {
	    if (int dl ; (dl = base128_dg((inbuf + i),(outbuf + j))) >= 0) {
	        len -= 4 ;
	        i += 4 ;
	        j += dl ;
	    } else {
		j = -1 ;
	    }
	} /* end for */
	return j ;
}
/* end subroutine (base128_d) */

int base128_enc(int v) noex {
    	return int(base128mgr.enc[v & (base - 1)]) ;
}

int base128_dec(int v) noex {
    	return int(base128mgr.dec[v & UCHAR_MAX]) ;
}


/* local subroutines */

local uint64_t mkhold(cchar *inbuf,int n) noex {
    	uint64_t	hold = 0 ;
	for (int i = (n - 1) ; i >= 0 ; i -= 1) {
	    hold <<= CHAR_BIT ;
	    hold |= uint64_t(inbuf[i] & UCHAR_MAX) ;
	} /* end for */
	return hold ;
} /* end subroutine (mkhold) */

local void encbuf(char *obuf,int olen,const uint64_t hold) noex {
	for (int i = 0 ; i < olen ; i += 1) {
	    cint idx = intconv((hold >> (i * bits)) & (base - 1)) ;
	    obuf[i] = base128mgr.enc[idx] ;
	} /* end for */
} /* end subroutine (encbuf) */

/* encode a group */
local void base128_eg(cchar *inbuf,char *obuf) noex {
	const uint64_t	hold = mkhold(inbuf,7) ;
	encbuf(obuf,8,hold) ;
}
/* end subroutine (base128_eg) */

/* decode a group */
local int base128_dg(cchar *inbuf,char *outbuf) noex {
	uint		hold = 0 ;
	int		rs = SR_OK ;
	int		dlen = 0 ; /* return-value */
	for (int i = 0 ; i < 4 ; i += 1) {
	    cint	ich = mkchar(inbuf[i]) ;
	    if (int ch ; (ch = mkchar(base128mgr.dec[ich])) != UCHAR_MAX) {
	        hold = (hold << 6) ;
	        if (ch != chx_equal) {
	            hold |= ch ;
	            dlen += 1 ;
	        } /* end if */
	    } else {
	        rs = SR_INVALID ;
	    }
	    if (rs < 0) break ;
	} /* end for */
	if (rs >= 0) {
	    if_constexpr (f_comment) {
	        for (int i = 0 ; i < 3 ; i += 1) {
	            cint	val = hold >> ((2 - i) * 6) ;
	            outbuf[i] = char(val) ;
	        } /* end for */
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
/* end subroutine (base128_dg) */


