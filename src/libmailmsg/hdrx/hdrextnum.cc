/* hdrextnum SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* comment separate (parse) a mail header field value */
/* version %I% last-modified %G% */


/* revision history:

	= 2004-05-29, David A­D­ Morano
	This code was adapted from the EMA (E-Mail Address) code
	(which has code that does a similar function).

*/

/* Copyright © 2004 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	hdrextnum

	This subroutine extracts a numeric value from the characters
	in a mail message header field value. This is not meant for
	use with mail addresses (although they certainly need to
	be comment-separated) but rather with shorter fixed-length
	header field values. This subroutine was especially created
	to extract the numeric value element from the CONTENT-LENGTH
	header field value!

	Synopsis:
	int hdrextnum(cchar *sp,int sl) noex

	Arguments:
	sp		source c-string pointer
	sl		source c-string length

	Returns:
	>=0		the value extracted (always positive)
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<ascii.h>		/* LIBU */
#include	<uclibmem.h>		/* LIBUC */
#include	<sbuf.h>		/* LIBUC */
#include	<six.h>			/* LIBUC |sibrk(3uc)| */
#include	<cfdec.h>		/* LIBUC */
#include	<char.h>		/* LIBUC */
#include	<mkchar.h>		/* LIBU */
#include	<localmisc.h>		/* |DIGBUFLEN| */

#include	"hdrextnum.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| */

/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

local int	hdrextnum_ext(char *,cchar *,int) noex ;


/* local variables */

cint		digbuflen = DIGBUFLEN ;


/* exported variables */


/* exported subroutines */

int hdrextnum(cchar *sp,int sl) noex {
	int		rs = SR_FAULT ;
	int		v = 0 ;
	if (sp) {
	    char	digbuf[digbuflen + 1] ;
	    if ((rs = hdrextnum_ext(digbuf,sp,sl)) > 0) {
	        int	len = rs ;
	        if (int si ; (si = sibrk(digbuf,len," \t")) >= 0) {
		    len = si ;
	        } /* end */
	        rs = cfdeci(digbuf,len,&v) ;
	    } /* end */
	} /* end if (non-null) */
	return (rs >= 0) ? v : rs ;
} /* end subroutine (hdrextnum) */


/* local subroutines */

local int hdrextnum_ext(char *digbuf,cchar *sp,int sl) noex {
	int		rs ;
	int		vl = 0 ;
	if (sl < 0) sl = lenstr(sp) ;
	/* skip over any leading white space */
	while ((sl > 0) && CHAR_ISWHITE(*sp)) {
	    sp += 1 ;
	    sl -= 1 ;
	} /* end while */
	/* initialize for this entry */
	if (sbuf b ; (rs = b.start(digbuf,digbuflen)) >= 0) {
	    int		c_comment = 0 ;
	    bool	f_quote = false ;
	    bool	f_wslast = false ;
	    bool	f_wsnew = false ;
	    bool	f_exit = false ;
	    while ((sl >= 0) && (*sp != '\0')) {
	        f_wsnew = false ;
	        switch (cint ch = mkchar(*sp) ; ch) {
	        case '\\':
	            if (f_quote) {
	                sp += 1 ;
	                sl -= 1 ;
	                if ((sl > 0) && (sp[0] != '\0')) {
	                    if (c_comment == 0) {
	                        b.chr(*sp++) ;
	                        sl -= 1 ;
	                    } else {
	                        sp += 1 ;
	                        sl -= 1 ;
	                    } /* end if */
	                } /* end if */
	            } else {
	                if (c_comment == 0) {
	                    b.chr(*sp++) ;
	                    sl -= 1 ;
	                } else {
	                    sp += 1 ;
	                    sl -= 1 ;
	                } /* end if */
	            } /* end if */
	            break ;
	        case CH_DQUOTE:
	            f_quote = (! f_quote) ;
	            sp += 1 ;
	            sl -= 1 ;
	            break ;
	        case CH_LPAREN:
	            if (! f_quote) {
	                sp += 1 ;
	                sl -= 1 ;
	                c_comment += 1 ;
	            } else {
	                if (c_comment == 0) {
	                    b.chr(*sp++) ;
	                    sl -= 1 ;
	                } else {
	                    sp += 1 ;
	                    sl -= 1 ;
	                } /* end if */
	            } /* end if */
	            break ;
	        case CH_RPAREN:
	            if (! f_quote) {
	                sp += 1 ;
	                sl -= 1 ;
	                if (c_comment > 0) {
	                    c_comment -= 1 ;
			}
	            } else {
	                if (c_comment == 0) {
	                    b.chr(*sp++) ;
	                    sl -= 1 ;
	                } else {
	                    sp += 1 ;
	                    sl -= 1 ;
	                } /* end if */
	            } /* end if */
	            break ;
	        case CH_COMMA:
	            if (c_comment) {
	                sp += 1 ;
	                sl -= 1 ;
	            } else {
	                f_exit = true ;
		    }
	            break ;
	        case ' ':
	        case '\t':
	            if ((c_comment == 0) && (! f_wslast)) {
	                b.chr(*sp++) ;
	                sl -= 1 ;
	            } else {
	                sp += 1 ;
	                sl -= 1 ;
	            } /* end if */
	            f_wsnew = true ;
	            break ;
	        default:
	            if (c_comment == 0) {
	                b.chr(*sp++) ;
	                sl -= 1 ;
	            } else {
	                sp += 1 ;
	                sl -= 1 ;
	            } /* end if */
	            break ;
	        } /* end switch */
	        f_wslast = f_wsnew ;
	        if (f_exit) break ;
	    } /* end while (scanning characters) */
	    vl = b.finish ;
	    if (rs >= 0) rs = vl ;
	} /* end if (sbuf) */
	return (rs >= 0) ? vl : rs ;
} /* end subroutine (hdrextnum_ext) */


