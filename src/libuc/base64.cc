/* base64 SUPPORT */
/* lang=C++20 */

/* perform BASE64 encoding and decoding */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-08-10, David A�D� Morano
	This subroutine was originally written.

*/

/* Copyright � 1998 David A�D� Morano.  All rights reserved. */

/*******************************************************************************

	Names:
	base64_e
	base64_d

	Description:
	We perform both BASE64 encoding and decoding operations with the
	subroutines 'base64_e' and 'base64_d' respectively.

	Synopsis:
	int base64_e(cchar *inbuf,int len,char *outbuf) noex
	int base64_d(cchar *sp,int sl,char *outbuf) noex

	Arguments:
	sp		source string
	sl		length of source string
	outbuf		result buffer (must be at least 3/4 times input length)

	Returns:
	>=0		length of result in bytes
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<usysrets.h>		/* we have one SR below */
#include	<utypedefs.h>
#include	<clanguage.h>
#include	<mkchar.h>

#include	"base64.h"


/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* forward references */

static int	base64_dg(cchar *,char *) noex ;

static void	base64_eg(cchar *,char *) noex ;


/* local variables */

constexpr bool		f_comment = false ;


/* exported variables */

/* encode table */
const uchar	base64_et[] = {
	'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M',
	'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
	'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm',
	'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
	'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '+', '/'
} ;

/* decode table (this table was made w/ the MKBASE64DT program) */
const uchar	base64_dt[] = {
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
	0xFF, 0xFF, 0xFF, 0x3E, 0xFF, 0xFF, 0xFF, 0x3F, 
	0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 
	0x3C, 0x3D, 0xFF, 0xFF, 0xFF, 0xFE, 0xFF, 0xFF, 
	0xFF, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 
	0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 
	0x0F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 
	0x17, 0x18, 0x19, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
	0xFF, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F, 0x20, 
	0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 
	0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F, 0x30, 
	0x31, 0x32, 0x33, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
} ;


/* exported subroutines */

int base64_e(cchar *inbuf,int len,char *outbuf) noex {
	int		i = 0 ;
	int		j = 0 ;
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
	    altinbuf[0] = inbuf[i] ;
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
	int		i = 0 ;
	int		j = 0 ;
	int		dlen ;
	while ((j >= 0) && (len >= 4)) {
	    if ((dlen = base64_dg((inbuf + i),(outbuf + j))) >= 0) {
	        len -= 4 ;
	        i += 4 ;
	        j += dlen ;
	    } else {
		j = -1 ;
	    }
	} /* end while */
	return j ;
}
/* end subroutine (base64_d) */


/* local subroutines */

/* encode a group */
static void base64_eg(cchar *inbuf,char *outbuf) noex {
	ulong		hold = 0 ;
	int		i{} ;
	for (i = 0 ; i < 3 ; i += 1) {
	    hold = (hold << 8) ;
	    hold |= mkchar(inbuf[i]) ;
	} /* end for */
	for (i = 0 ; i < 4 ; i += 1) {
	    outbuf[i] = base64_et[(hold >> ((3 - i) * 6)) & 0x3F] ;
	}
}
/* end subroutine (base64_eg) */

/* decode a group */
static int base64_dg(cchar *inbuf,char *outbuf) noex {
	ulong		hold = 0 ;
	int		dlen = 0 ;
	int		rs = 0 ;
	for (int i = 0 ; i < 4 ; i += 1) {
	    cint	ich = mkchar(inbuf[i]) ;
	    if (int ch ; (ch = mkchar(base64_dt[ich])) != 0xff) {
	        hold = (hold << 6) ;
	        if (ch != 0xFE) {
	            hold |= ch ;
	            dlen += 1 ;
	        }
	    } else {
	        rs = SR_INVALID ;
	    }
	    if (rs < 0) break ;
	} /* end for */
	if constexpr (f_comment) {
	    if (rs >= 0) {
	        for (int i = 0 ; i < 3 ; i += 1) {
	            outbuf[i] = mkchar(hold >> ((2 - i) * 6)) ;
	        }
	        dlen -= 1 ;
	    }
	} else {
	    if (rs >= 0) {
	        outbuf[0] = mkchar(hold >> 16) ;
	        outbuf[1] = mkchar(hold >> 8) ;
	        outbuf[2] = mkchar(hold >> 0) ;
	        dlen -= 1 ;
	    }
	} /* end if-constexpr */
	return (rs >= 0) ? dlen : rs ;
}
/* end subroutine (base64_dg) */


