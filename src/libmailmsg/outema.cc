/* outema SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */
 
/* manage printing EMAs (E-Mail-Addresses) as lines */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	outema

	Description:
	This object deals with printing lines.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstdarg>
#include	<cstring>		/* |strlen(3c)| */
#include	<usystem.h>
#include	<mallocxx.h>
#include	<ascii.h>
#include	<estrings.h>
#include	<buffer.h>
#include	<strn.h>
#include	<sfx.h>
#include	<localmisc.h>		/* |NTABCOLS| + |COLUMNS| */

#include	"outema.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |getlenstr(3u)| */

/* local defines */


/* imported namespaces */

using libuc::libmem ;			/* variable */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

template<typename ... Args>
static inline int outema_magic(outema *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magic == OUTEMA_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}
/* end subroutine (outema_magic) */

static int	filer_outpart(filer *,int,cchar *,int) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int outema_start(outema *op,filer *ofp,int maxlen) noex {
    	OUTEMA		*hop = op ;
	int		rs = SR_FAULT ;
	if (op && ofp) {
	    rs = memclear(hop) ;
	    op->maxlen = maxlen ;
	    op->rlen = maxlen ;
	    op->ofp = ofp ;
	    op->magic = OUTEMA_MAGIC ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (outema_start) */

int outema_finish(outema *op) noex {
	int		rs ;
	int		wlen = 0 ;
	if ((rs = outema_magic(op)) >= 0) {
	    rs = SR_BUGCHECK ;
	    if (op->ofp) {
		rs = SR_OK ;
	        if (op->llen > 0) {
	            rs = filer_println(op->ofp,nullptr,0) ;
	            op->wlen += rs ;
	            op->rlen = op->maxlen ;
	            op->llen = 0 ;
	        }
	        wlen = op->wlen ;
	        op->ofp = nullptr ;
	    } /* end if (bugcheck) */
	    op->magic = 0 ;
	} /* end if (magic) */
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (outema_finish) */

int outema_ent(outema *op,EMA_ENT *ep) noex {
	int		rs ;
	int		rs1 ;
	int		wlen = 0 ;
	if ((rs = outema_magic(op)) >= 0) {
	    if (buffer b ; (rs = b.start(COLUMNS)) >= 0) {
	        int	bl ;
	        int	c = 0 ;
	        if ((rs >= 0) && (ep->ap != nullptr) && (ep->al > 0)) {
	            if (c++ > 0) rs = b.chr(CH_SP) ;
	            if (rs >= 0) {
	                rs = b.strquote(ep->ap,ep->al) ;
		    }
	        }
	        if ((rs >= 0) && (ep->rp != nullptr) && (ep->rl > 0)) {
	            if (c++ > 0) rs = b.chr(CH_SP) ;
	            if (rs >= 0) rs = b.chr(CH_LANGLE) ;
	            if (rs >= 0) rs = b.stropaque(ep->rp,ep->rl) ;
	            if (rs >= 0) rs = b.chr(CH_RANGLE) ;
	        }
	        if ((rs >= 0) && (ep->cp != nullptr) && (ep->cl > 0)) {
	            cchar	*cp ;
	            if (int cl ; (cl = sfshrink(ep->cp,ep->cl,&cp)) > 0) {
	                if (c++ > 0) rs = b.chr(CH_SP) ;
	                if (rs >= 0) {
	                    cint	sz = (cl+2+1) ;
	                    if (char *ap ; (rs = libmem.mall(sz,&ap)) >= 0) {
				char	*tbp = ap ;
	                        *tbp++ = CH_LPAREN ;
	                        if ((rs = snwcpycompact(tbp,cl,cp,cl)) >= 0) {
	                            tbp += rs ;
	                            *tbp++ = CH_RPAREN ;
				    {
				       cint	tbl = intconv(tbp - ap) ;
	                               rs = b.strw(tbp,tbl) ;
				    }
	                        } /* end if (snwcpycompact) */
	                        rs1 = libmem.free(ap) ;
				if (rs >= 0) rs = rs1 ;
	                    } /* end if (m-a-f) */
	                } /* end if (ok) */
	            } /* end if (shrink) */
	        } /* end if (comment) */
	        if (rs >= 0) {
	            if (cchar *bp ; (rs = b.get(&bp)) > 0) {
	                bl = rs ;
	                rs = outema_item(op,bp,bl) ;
	                wlen += rs ;
	            }
	        }
	        rs1 = b.finish ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (buffer-ret) */
	} /* end if (magic) */
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (outema_ent) */

int outema_item(outema *op,cchar *vp,int vl) noex {
	int		rs ;
	int		wlen = 0 ;
	if ((rs = outema_magic(op)) >= 0) {
	    if (vl < 0) vl = lenstr(vp) ;
	    if (vl > 0) {
	        bool	f_prevcomma = op->fl.comma ;
	        op->fl.comma = true ;
	        rs = outema_value(op,vp,vl) ;
	        wlen += rs ;
	        op->c_items += 1 ;
	        op->fl.comma = f_prevcomma ;
	    } /* end if */
	} /* end if (magic) */
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (outema_item) */

int outema_value(outema *op,cchar *vp,int vl) noex {
	int		rs ;
	int		wlen = 0 ;
	if ((rs = outema_magic(op,vp)) >= 0) {
	    if (vp && vp[0]) {		/* <- not an error to be empty */
	        int	nlen ;
	        int	cl, cl2 ;
	        bool	f_comma = false ;
	        cchar	*fmt ;
	        cchar	*tp, *cp ;
	        if (vl < 0) vl = lenstr(vp) ;
	        op->c_values = 0 ;
	        while ((rs >= 0) && (vl > 0)) {
	            if ((cl = sfnextqtok(vp,vl,&cp)) > 0) {
	                f_comma = (op->fl.comma && (op->c_items > 0)) ;
	                nlen = outema_needlength(op,cl) ;
	                if (nlen > op->rlen) {
	                    if (op->llen > 0) {
	                        fmt = "\n" ;
	                        if (f_comma) {
	                            f_comma = false ;
	                            op->fl.comma = false ;
	                            fmt = ",\n" ;
	                        }
	                        rs = filer_write(op->ofp,fmt,-1) ;
	                        wlen += rs ;
	                    }
	                    op->rlen = op->maxlen ;
	                    op->llen = 0 ;
	                    op->c_values = 0 ;
	                } /* end if (overflow) */
	                if (rs >= 0) {
	                    if (f_comma) {
	                        op->fl.comma = false ;
	                    }
	                    rs = filer_outpart(op->ofp,f_comma,cp,cl) ;
	                    wlen += rs ;
	                    op->llen += rs ;
	                    op->rlen -= rs ;
	                    f_comma = false ;
	                } /* end if */
	                op->c_values += 1 ;
	                cl2 = intconv(cp + cl - vp) ;
	                vp += cl2 ;
	                vl -= cl2 ;
	            } else if ((tp = strnchr(vp,vl,'\n')) != nullptr) {
	                vl -= intconv((tp + 1) - vp) ;
	                vp = (tp + 1) ;
	            } else {
	                vl = 0 ;
	            }
	        } /* end while */
	        op->wlen += wlen ;
	    } /* end if (valid) */
	} /* end if (magic) */
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (outema_value) */

int outema_write(outema *op,cchar *v,int vlen) noex {
	int		rs ;
	int		wlen = 0 ;
	if ((rs = outema_magic(op)) >= 0) {
	    if (vlen < 0) vlen = lenstr(v) ;
	    if (vlen > 0) {
	        rs = filer_write(op->ofp,v,vlen) ;
	        wlen += rs ;
	        op->llen += rs ;
	        op->rlen -= rs ;
	    }
	    op->wlen += wlen ;
	} /* end if (magic) */
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (outema_write) */

#ifdef	COMMENT

int outema_printf(outema *op,cchar *fmt,...) noex {
	va_list		ap ;
	int		rs ;
	int		rs1 ;
	int		wlen = 0 ;
	if ((rs = outema_magic(op,fmt)) >= 0) {
	    if (char *fbuf ; (rs = malloc_mailaddr(&fbuf)) >= 0) {
	        va_begin(ap,fmt) ;
	        cint	flen = rs ;
	        if ((rs = bufvprintf(fbuf,flen,fmt,ap)) >= 0) {
	    	    cint	len = rs ;
	            if ((rs = filer_write(op->ofp,fbuf,len)) >= 0) {
	                wlen += rs ;
	                op->wlen += rs ;
	                op->llen += rs ;
	                op->rlen -= rs ;
		    } /* end if (filer_write) */
	        } /* end if (bufvprintf) */
	        va_end(ap) ;
		rs1 = malloc_free(fbuf) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (m-a-f) */
	} /* end if (magic) */
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (outema_printf) */

#endif /* COMMENT */

int outema_hdrkey(outema *op,cchar *kname) noex {
	int		rs ;
	int		wlen = 0 ;
	int		nlen = 0 ;
	if ((rs = outema_magic(op,kname)) >= 0) {
	    rs = SR_INVALID ;
	    if (kname[0]) {
		filer	*ofp = op->ofp ;
		rs = SR_OK ;
	        if ((rs >= 0) && (op->llen > 0)) {
	            rs = ofp->println(kname,0) ;
	            wlen += rs ;
	            op->llen = 0 ;
	            op->rlen = op->maxlen ;
	        }
	        if (rs >= 0) {
	            rs = ofp->write(kname,-1) ;
	            wlen += rs ;
	            nlen += rs ;
	        }
	        if (rs >= 0) {
	            char	buf[2] ;
	            buf[0] = ':' ;
	            buf[1] = '\0' ;
	            rs = ofp->write(buf,1) ;
	            wlen += rs ;
	            nlen += rs ;
	        }
	        if ((rs >= 0) && (nlen > 0)) {
	            op->llen += nlen ;
	            op->rlen -= nlen ;
	        }
	        op->wlen += wlen ;
	    } /* end if (valid) */
	} /* end if (magic) */
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (outema_hdrkey) */

int outema_needlength(outema *op,int cl) noex {
	int		rs ;
	int		nlen = (cl + 1) ;
	if ((rs = outema_magic(op)) >= 0) {
	    if (op->llen == 0) {
	        nlen += 1 ;
	    }
	    if (op->fl.comma && (op->c_items > 0)) {
	        nlen += 1 ;
	    }
	} /* end if (magic) */
	return (rs >= 0) ? nlen : rs ;
}
/* end subroutine (outema_needlength) */


/* private subroutines */

static int filer_outpart(filer *fbp,int f_comma,cchar *cp,int cl) noex {
	int		rs = SR_BUGCHECK ;
	int		wlen = 0 ;
	if (fbp) {
	    int		i = 0 ;
	    char	buf[3] ;
	    if (f_comma) {
		buf[i++] = CH_COMMA ;
	    }
	    buf[i++] = ' ' ;
	    buf[i] = '\0' ;
	    if ((rs = fbp->write(buf,i)) >= 0) {
	        wlen += rs ;
	        rs = fbp->write(cp,cl) ;
	        wlen += rs ;
	    }
	} /* end if (non-null) */
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (filer_outpart) */


