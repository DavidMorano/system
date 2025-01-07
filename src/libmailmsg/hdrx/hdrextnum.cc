/* hdrextnum SUPPORT */
/* encoding=ISO8859-1 */
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
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<sbuf.h>
#include	<ascii.h>
#include	<six.h>
#include	<cfdec.h>
#include	<char.h>
#include	<mkchar.h>
#include	<localmisc.h>		/* |DIGBUFLEN| */

#include	"hdrextnum.h"


/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

extern int	hdrextnum_ext(char *,cchar *,int) noex ;


/* local variables */

constexpr int	digbuflen = DIGBUFLEN ;


/* exported variables */


/* exported subroutines */

int hdrextnum(cchar *sp,int sl) noex {
	int		rs = SR_FAULT ;
	int		v = 0 ;
	if (sp) {
	    char	digbuf[digbuflen + 1] ;
	    if ((rs = hdrextnum_ext(digbuf,sp,sl)) > 0) {
	        int	len = rs ;
	        if (int si ; (si = sibreak(digbuf,len," \t")) >= 0) {
		    len = si ;
	        }
	        rs = cfdeci(digbuf,len,&v) ;
	    }
	} /* end if (non-null) */
	return (rs >= 0) ? v : rs ;
}
/* end subroutine (hdrextnum) */


/* local subroutines */

int hdrextnum_ext(char *digbuf,cchar *sp,int sl) noex {
	int		rs ;
	int		vl = 0 ;
	if (sl < 0) sl = strlen(sp) ;
	/* skip over any leading white space */
	while ((sl > 0) && CHAR_ISWHITE(*sp)) {
	    sp += 1 ;
	    sl -= 1 ;
	}
	/* initialize for this entry */
	if (sbuf b ; (rs = b.start(digbuf,digbuflen)) >= 0) {
	    int		c_comment = 0 ;
	    bool	f_quote = false ;
	    bool	f_wslast = false ;
	    bool	f_wsnew = false ;
	    bool	f_exit = false ;
	    while ((sl >= 0) && (*sp != '\0')) {
	        cint	ch = mkchar(*sp) ;
	        f_wsnew = false ;
	        switch (ch) {
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
	                    }
	                }
	            } else {
	                if (c_comment == 0) {
	                    b.chr(*sp++) ;
	                    sl -= 1 ;
	                } else {
	                    sp += 1 ;
	                    sl -= 1 ;
	                }
	            }
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
	                }
	            }
	            break ;
	        case CH_RPAREN:
	            if (! f_quote) {
	                sp += 1 ;
	                sl -= 1 ;
	                if (c_comment > 0)
	                    c_comment -= 1 ;
	            } else {
	                if (c_comment == 0) {
	                    b.chr(*sp++) ;
	                    sl -= 1 ;
	                } else {
	                    sp += 1 ;
	                    sl -= 1 ;
	                }
	            }
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
	            }
	            f_wsnew = true ;
	            break ;
	        default:
	            if (c_comment == 0) {
	                b.chr(*sp++) ;
	                sl -= 1 ;
	            } else {
	                sp += 1 ;
	                sl -= 1 ;
	            }
	            break ;
	        } /* end switch */
	        f_wslast = f_wsnew ;
	        if (f_exit) break ;
	    } /* end while (scanning characters) */
	    vl = b.finish ;
	    if (rs >= 0) rs = vl ;
	} /* end if (sbuf) */
	return (rs >= 0) ? vl : rs ;
}
/* end subroutine (hdrextnum_ext) */


