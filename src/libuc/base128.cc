/* base128 SUPPORT */
/* encoding=ISO8859-1 */
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
	We perform both BASE64 encoding and decoding operations
	with the subroutines |base128_e| and |base128_d| respectively.
	The |base128_dec| subroutine provides access to the decoder
	table.

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
#include	<climits>		/* |UCHAR_MAX| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>		/* we have one SR below */
#include	<mkchar.h>		/* |mkchar(3uc)| */
#include	<localmisc.h>

#include	"base128.h"


/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

constexpr int		chx_equal = 0xFE ; /* special value in decoder table */

namespace {
    constexpr int	tablen = (UCHAR_MAX + 1) ;
    struct helper {
	uchar		enc[128] ;
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
	        enc[i++] = uchar('À' + j) ;
	    }
	    for (int j = 0 ; j < 31 ; j += 1) {
	        enc[i++] = uchar('Ø' + j) ;
	    }
	    for (int j = 0 ; j < 8 ; j += 1) {
	        enc[i++] = uchar('ø' + j) ;
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

static int	base128_dg(cchar *,char *) noex ;

static void	base128_eg(cchar *,char *) noex ;


/* local variables */

constexpr helper	base128mgr ;

constexpr bool		f_comment = false ;


/* exported variables */


/* exported subroutines */

int base128_e(cchar *inbuf,int len,char *outbuf) noex {
	int		i = 0 ;
	int		j = 0 ; /* return-value */
	char		altinbuf[4] ;
	while ((len - i) >= 3) {
	    base128_eg((inbuf + i),(outbuf + j)) ;
	    i += 3 ;
	    j += 4 ;
	} /* end while */
	switch (len - i) {
	case 1:
	    altinbuf[0] = inbuf[i] ;
	    altinbuf[1] = 0x00 ;
	    altinbuf[2] = 0x00 ;
	    base128_eg(altinbuf,(outbuf + j)) ;
	    outbuf[j + 2] = '=' ;
	    outbuf[j + 3] = '=' ;
	    j += 4 ;
	    break ;
	case 2:
	    altinbuf[0] = inbuf[i] ;
	    altinbuf[1] = inbuf[i + 1] ;
	    altinbuf[2] = 0x00 ;
	    base128_eg(altinbuf,outbuf + j) ;
	    outbuf[j + 3] = '=' ;
	    j += 4 ;
	    break ;
	} /* end switch */
	return j ;
}
/* end subroutine (base128_e) */

/* decode */
int base128_d(cchar *inbuf,int len,char *outbuf) noex {
	int		j = 0 ; /* return-value */
	int		dlen ;
	for (int i = 0 ; (j >= 0) && (len >= 4) ; ) {
	    if ((dlen = base128_dg((inbuf + i),(outbuf + j))) >= 0) {
	        len -= 4 ;
	        i += 4 ;
	        j += dlen ;
	    } else {
		j = -1 ;
	    }
	} /* end for */
	return j ;
}
/* end subroutine (base128_d) */

int base128_enc(int v) noex {
    	return int(base128mgr.enc[v & UCHAR_MAX]) ;
}

int base128_dec(int v) noex {
    	return int(base128mgr.dec[v & UCHAR_MAX]) ;
}


/* local subroutines */

/* encode a group */
static void base128_eg(cchar *inbuf,char *outbuf) noex {
	uint		hold = 0 ;
	for (int i = 0 ; i < 3 ; i += 1) {
	    hold = (hold << 8) ;
	    hold |= mkchar(inbuf[i]) ;
	} /* end for */
	for (int i = 0 ; i < 4 ; i += 1) {
	    cint	idx = (hold >> ((3 - i) * 6)) & 0x3F ;
	    outbuf[i] = base128mgr.enc[idx] ;
	}
}
/* end subroutine (base128_eg) */

/* decode a group */
static int base128_dg(cchar *inbuf,char *outbuf) noex {
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
	        }
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
/* end subroutine (base128_dg) */


