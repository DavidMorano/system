/* mhcom SUPPORT (Message Header Comment) */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* comment-separate (parse) a mail header field value */
/* version %I% last-modified %G% */


/* revision history:

	= 2002-05-29, David A­D­ Morano
	This code was adapted from the EMA (E-Mail Address) code
	(which does a similar function).

*/

/* Copyright © 2002 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	mhcom

	Description:
	This object separates comment characters from the value
	characters in a mail message header field value.  Comment
	characters are introduced with parentheses.  See RFC-822
	for more information on how comments operate within header
	field values.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<usystem.h>
#include	<sbuf.h>
#include	<ascii.h>
#include	<char.h>
#include	<mkchar.h>
#include	<localmisc.h>

#include	"mhcom.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |getlenstr(3u)| */

/* local defines */

#define	MHCOM_SVALUE	0
#define	MHCOM_SCOMMENT	1
#define	MHCOM_SOVERLAST	2


/* imported namespaces */

using libuc::libmem ;			/* variable */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

template<typename ... Args>
static inline int mhcom_magic(mhcom *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magic == MHCOM_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}
/* end subroutine (mhcom_magic) */

local int	mhcom_bake(mhcom *,int,cchar *,int) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int mhcom_start(mhcom *op,cchar *sp,int sl) noex {
    	MHCOM		*hop = op ;
	int		rs = SR_FAULT ;
	if (op && sp) {
	    int		sz ;
	    int		buflen ;
	    memclear(hop) ;
	    if (sl < 0) sl = lenstr(sp) ;
	    while ((sl > 0) && CHAR_ISWHITE(*sp)) {
	        sp += 1 ;
	        sl -= 1 ;
	    }
	    buflen = (sl + 2) ;
	    sz = ( 2* buflen) ;
	    if (void *p ; (rs = libmem.mall(sz,&p)) >= 0) {
	        op->a = charp(p) ;
	        op->value = (op->a + (0*buflen)) ;
	        op->comment = (op->a + (1*buflen)) ;
	        if ((rs = mhcom_bake(op,buflen,sp,sl)) >= 0) {
	            op->magic = MHCOM_MAGIC ;
	        }
	        if (rs < 0) {
	            libmem.free(op->a) ;
	            op->a = nullptr ;
	        } /* end if (error) */
	    } /* end if (m-a) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (mhcom_start) */

int mhcom_finish(mhcom *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = mhcom_magic(op)) >= 0) {
	    if (op->a) {
	        rs1 = libmem.free(op->a) ;
	        if (rs >= 0) rs = rs1 ;
	        op->a = nullptr ;
	    }
	    op->value = nullptr ;
	    op->comment = nullptr ;
	    op->magic = 0 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (mhcom_finish) */

int mhcom_getval(mhcom *op,cchar **rpp) noex {
	int		rs ;
	if ((rs = mhcom_magic(op)) >= 0) {
	    if (rpp) *rpp = op->value ;
	    rs = (op->value) ? op->vlen : SR_NOENT ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (mhcom_getval) */

int mhcom_getcom(mhcom *op,cchar **rpp) noex {
	int		rs ;
	if ((rs = mhcom_magic(op)) >= 0) {
	    if (rpp) *rpp = op->comment ;
	    rs = (op->comment) ? op->clen : SR_NOENT ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (mhcom_getcom) */


/* private subroutines */

local int mhcom_bake(mhcom *op,int bl,cchar *sp,int sl) noex {
	sbuf		as[MHCOM_SOVERLAST] ;
	int		rs ;
	int		rs1 ;
	int		vl = 0 ;
	if ((rs = sbuf_start((as+MHCOM_SVALUE),op->value,bl)) >= 0) {
	    if ((rs = sbuf_start((as+MHCOM_SCOMMENT),op->comment,bl)) >= 0) {
	        int	state = MHCOM_SVALUE ;
		int	cl ;
		int	pstate = MHCOM_SVALUE ;
		int	c_comment = 0 ;
		int	pc ;
		int	f_quote = false ;
	        while ((sl != 0) && (*sp != '\0')) {
	            switch (cint ch = mkchar(*sp) ; ch) {
	            case '\\':
	                sp += 1 ;
	                if ((f_quote || c_comment) &&
	                    (sl > 1) && (sp[0] != '\0')) {
	                    sbuf_chr((as + state),*sp++) ;
	                    sl -= 1 ;
	                }
	                break ;
	            case CH_DQUOTE:
	                f_quote = (! f_quote) ;
	                sbuf_chr((as + state),*sp++) ;
	                break ;
	            case CH_LPAREN:
	                if (! f_quote) {
	                    if (c_comment == 0) {
	                        pc = sbuf_getprev(as + state) ;
	                        if ((pc >= 0) && (pc != ' ')) {
	                            sbuf_chr((as + state),' ') ;
				}
	                        pstate = state ;
	                        state = MHCOM_SCOMMENT ;
	                        pc = sbuf_getprev(as + state) ;
	                        if ((pc >= 0) && (pc != ' ')) {
	                            sbuf_chr((as + state),' ') ;
				}
	                        sp += 1 ;
	                    } else {
	                        sbuf_chr((as + state),*sp++) ;
			    }
	                    c_comment += 1 ;
	                } else {
	                    sbuf_chr((as + state),*sp++) ;
			}
	                break ;
	            case CH_RPAREN:
	                if ((! f_quote) && (c_comment > 0)) {
	                    c_comment -= 1 ;
	                    if (c_comment == 0) {
	                        state = pstate ;
	                        sp += 1 ;
	                    } else {
	                        sbuf_chr((as + state),*sp++) ;
			    }
	                } else {
	                    sbuf_chr((as + state),*sp++) ;
			}
	                break ;
		    /* some optimizations (not required?) */
	            case '\t':
	            case ' ':
	                if (! f_quote) {
	                    if (c_comment == 0) {
	                        cl = sbuf_getlen(as + state) ;
	                        pc = sbuf_getprev(as + state) ;
	                        if ((cl == 0) || (pc == ' ')) {
	                            sp += 1 ;
	                            break ;
	                        }
	                    } else {
	                        cl = sbuf_getlen(as + state) ;
	                        pc = sbuf_getprev(as + state) ;
	                        if ((cl == 0) || (pc == ' ')) {
	                            sp += 1 ;
	                            break ;
	                        }
	                    } /* end if */
	                } /* end if (not in a quote) */
			fallthrough ;
			/* FALLTHROUGH */
	            default:
	                if (c_comment) {
	                    sbuf_chr((as + MHCOM_SCOMMENT),*sp++) ;
	                } else {
	                    sbuf_chr((as + state),*sp++) ;
			}
	                break ;
	            } /* end switch */
	            if (sl > 0) sl -= 1 ;
	        } /* end while (scanning characters) */
	        rs1 = sbuf_finish(&as[MHCOM_SCOMMENT]) ;
	        if (rs >= 0) rs = rs1 ;
	        op->clen = rs1 ;
	    } /* end if (sbuf) */
	    vl = sbuf_finish(&as[MHCOM_SVALUE]) ;
	    if (rs >= 0) rs = vl ;
	    op->vlen = vl ;
	} /* end if (sbuf) */
	return (rs >= 0) ? vl : rs ;
}
/* end subroutine (mhcom_bake) */

int mhcom::start(cchar *sp,int sl) noex {
	return mhcom_start(this,sp,sl) ;
}

int mhcom::getval(cchar **rpp) noex {
	return mhcom_getval(this,rpp) ;
}

int mhcom::getcom(cchar **rpp) noex {
	return mhcom_getcom(this,rpp) ;
}

void mhcom::dtor() noex {
	if (cint rs = finish ; rs < 0) {
	    ulogerror("mhcom",rs,"fini-finish") ;
	}
}

mhcom_co::operator int () noex {
	int		rs = SR_BUGCHECK ;
	if (op) {
	    switch (w) {
	    case mhcommem_finish:
	        rs = mhcom_finish(op) ;
	        break ;
	    } /* end switch */
	} /* end if (non-null) */
	return rs ;
}
/* end method (mhcom_co::operator) */



