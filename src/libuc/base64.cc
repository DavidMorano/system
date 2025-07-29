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
	We perform both BASE64 encoding and decoding operations
	with the subroutines |base64_e| and |base64_d| respectively.
	The |base64_dec| subroutine provides access to the decoder
	table.

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
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>		/* we have one SR below */
#include	<mkchar.h>		/* |mkchar(3uc)| */
#include	<localmisc.h>

#include	"base64.h"


/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

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
	constexpr mkdecoder() noex {
	    for (int j = 0 ; j < tablen ; j += 1) {
		tab[j] = uchar(UCHAR_MAX) ;
	    }
	    for (int i = 0 ; i < nelem(base64_et) ; i += 1) {
	        uchar	uch = base64_et[i] ;
	        tab[uch] = uchar(i) ;
	    } /* end for */
	    tab['='] = chx_equal ;	/* <- special value */
	} ; /* end ctor */
    } ; /* end struct (mkdecoder) */
} /* end namespace */


/* forward references */

static int	base64_dg(cchar *,char *) noex ;

static void	base64_eg(cchar *,char *) noex ;


/* local variables */

constexpr mkdecoder	base64decode ;

constexpr bool		f_comment = false ;


/* exported variables */


/* exported subroutines */

int base64_e(cchar *inbuf,int len,char *outbuf) noex {
	int		i = 0 ;
	int		j = 0 ; /* return-value */
	char		altinbuf[4] ;
	while ((len - i) >= 3) {
	    base64_eg((inbuf + i),(outbuf + j)) ;
	    i += 3 ;
	    j += 4 ;
	} /* end while */
	switch (len - i) {
	case 1:
	    altinbuf[0] = inbuf[i] ;
	    altinbuf[1] = 0x00 ;
	    altinbuf[2] = 0x00 ;
	    base64_eg(altinbuf,(outbuf + j)) ;
	    outbuf[j + 2] = '=' ;
	    outbuf[j + 3] = '=' ;
	    j += 4 ;
	    break ;
	case 2:
	    altinbuf[0] = inbuf[i + 0] ;
	    altinbuf[1] = inbuf[i + 1] ;
	    altinbuf[2] = 0x00 ;
	    base64_eg(altinbuf,outbuf + j) ;
	    outbuf[j + 3] = '=' ;
	    j += 4 ;
	    break ;
	} /* end switch */
	return j ;
}
/* end subroutine (base64_e) */

/* decode */
int base64_d(cchar *inbuf,int len,char *outbuf) noex {
	int		j = 0 ; /* return-value */
	int		dlen ;
	for (int i = 0 ; (j >= 0) && (len >= 4) ; ) {
	    if ((dlen = base64_dg((inbuf + i),(outbuf + j))) >= 0) {
	        len -= 4 ;
	        i += 4 ;
	        j += dlen ;
	    } else {
		j = -1 ;
	    }
	} /* end for */
	return j ;
}
/* end subroutine (base64_d) */

int base64_enc(int v) noex {
    	return int(base64_et[v & UCHAR_MAX]) ;
}

int base64_dec(int v) noex {
    	return int(base64decode.tab[v & UCHAR_MAX]) ;
}


/* local subroutines */

/* encode a group */
static void base64_eg(cchar *inbuf,char *outbuf) noex {
	uint32_t	hold = 0 ;
	for (int i = 0 ; i < 3 ; i += 1) {
	    hold <<= CHAR_BIT ;
	    hold |= uint32_t(mkchar(inbuf[i])) ;
	} /* end for */
	for (int i = 0 ; i < 4 ; i += 1) {
	    cint	idx = int((hold >> ((3 - i) * 6)) & 0x3F) ;
	    outbuf[i] = base64_et[idx] ;
	}
}
/* end subroutine (base64_eg) */

/* decode a group */
static int base64_dg(cchar *inbuf,char *outbuf) noex {
	uint32_t	hold = 0 ;
	int		rs = SR_OK ;
	int		dlen = 0 ; /* return-value */
	for (int i = 0 ; i < 4 ; i += 1) {
	    cint	ich = mkchar(inbuf[i]) ;
	    if (int ch ; (ch = mkchar(base64decode.tab[ich])) != UCHAR_MAX) {
	        hold <<= 6 ;
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
	        for (int i = 0 ; i < 3 ; i += 1) {
	            outbuf[i] = char(hold >> ((2 - i) * 6)) ;
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


