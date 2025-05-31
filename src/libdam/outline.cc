/* outline SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* manage printing lines */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-01, David A­D­ Morano
	This code was originally written.

	= 1999-02-01, David A­D­ Morano
	I added a little code to "post" articles that do not have
	a valid newsgroup to a special "dead article" directory in
	the BB spool area.

*/

/* Copyright © 1998,1999 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	outline

	Description:
	This object deals with printing lines.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>		/* |strlen(3c)| */
#include	<usystem.h>
#include	<estrings.h>
#include	<localmisc.h>		/* |NTABCOLS| */

#include	"outline.h"
#include	"contentencodings.h"


/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int outline_start(outline *op,bfile *ofp,int maxlen) noex {
	int		rs = SR_FAULT ;
	if (op && ofp) {
	    rs = memclear(op) ;
	    op->maxlen = maxlen ;
	    op->rlen = (maxlen -1) ;
	    op->ofp = ofp ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (outline_start) */

int outline_finish(outline *op) noex {
	int		rs = SR_FAULT ;
	int		wlen = 0 ;
	if (op) {
	    rs = SR_BUGCHECK ;
	    if (op->ofp) {
		rs = SR_OK ;
	        if (op->llen > 0) {
	            rs = bputc(op->ofp,'\n') ;
	            wlen += rs ;
	            op->wlen += rs ;
	            op->rlen = (op->maxlen - 1) ;
	            op->llen = 0 ;
	        }
	        op->ofp = nullptr ;
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (outline_finish) */

int outline_item(outline *op,cchar *vp,int vl) noex {
	int		rs = SR_FAULT ;
	int		wlen = 0 ;
	if (op && vp) {
	    rs = SR_OK ;
	    if (vl < 0) vl = strlen(vp) ;
	    if (vl > 0) {
	        op->f.comma = true ;
	        rs = outline_value(op,vp,vl) ;
	        wlen += rs ;
	        op->c_items += 1 ;
	    }
	} /* end if (non-null) */
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (outline_item) */

int outline_value(outline *op,cchar *vp,int vl) noex {
	int		rs = SR_FAULT ;
	int		wlen = 0 ;
	if (op && vp) {
	    rs = SR_OK ;
	    if (vp[0]) {
	        int	nlen ;
	        int	cl, cl2 ;
	        int	f_comma = FALSE ;
	        cchar	*fmt ;
	        cchar	*tp, *cp ;
	        if (vl < 0) vl = strlen(vp) ;
	        op->c_values = 0 ;
	        while ((rs >= 0) && (vl > 0)) {
	            if ((cl = sfnext(vp,vl,&cp)) > 0) {
	                f_comma = (op->f.comma && (op->c_items > 0)) ;
	                nlen = outline_needlength(op,cl) ;
	                if (nlen > op->rlen) {
	                    if (op->llen > 0) {
	                        fmt = "\n" ;
	                        if (f_comma) {
	                            f_comma = FALSE ;
	                            op->f.comma = FALSE ;
	                            fmt = ",\n" ;
	                        }
	                        rs = bwrite(op->ofp,fmt,-1) ;
	                        wlen += rs ;
	                    }
	                    op->rlen = (op->maxlen - 1) ;
	                    op->llen = 0 ;
	                    op->c_values = 0 ;
	                } /* end if (overflow) */
	                if (rs >= 0) {
	                    fmt = " %r" ;
	                    if (f_comma) {
	                        f_comma = FALSE ;
	                        op->f.comma = FALSE ;
	                        fmt = ", %r" ;
	                    }
	                    rs = bprintf(op->ofp,fmt,cp,cl) ;
	                    wlen += rs ;
	                    op->llen += rs ;
	                    op->rlen -= rs ;
	                } /* end if (ok) */
	                op->c_values += 1 ;
	                cl2 = ((cp+cl) - vp) ;
	                vp += cl2 ;
	                vl -= cl2 ;
	            } else if ((tp = strnchr(vp,vl,'\n')) != nullptr) {
	                vl -= ((tp + 1) - vp) ;
	                vp = (tp + 1) ;
	            } else {
	                vl = 0 ;
	            }
	        } /* end while */
	        op->wlen += wlen ;
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (outline_value) */

int outline_write(outline *op,cchar *vp,int vl) noex {
	int		rs = SR_FAULT ;
	int		wlen = 0 ;
	if (op && vp) {
	    rs = SR_OK ;
	    if (vl < 0) vl = strlen(vp) ;
	    if (vl > 0) {
	        if ((rs = bwrite(op->ofp,vp,vl)) >= 0) {
	            wlen += rs ;
	            op->wlen += rs ;
	            op->llen += rs ;
	            op->rlen -= rs ;
	        }
	    }
	} /* end if (non-null) */
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (outline_write) */

int outline_printf(outline *op,cchar *fmt,...) noex {
	int		rs = SR_FAULT ;
	int		wlen = 0 ;
	if (op && fmt) {
	    va_list	ap ;
	    va_begin(ap,fmt) ;
	    if ((rs = bvprintf(op->ofp,fmt,ap)) >= 0) {
	        wlen += rs ;
	        op->wlen += rs ;
	        op->llen += rs ;
	        op->rlen -= rs ;
	    } /* end if (bvprintf) */
	    va_end(ap) ;
	} /* end if (non-null) */
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (outline_printf) */

int outline_needlength(outline *op,int cl) noex {
	int		rs = SR_FAULT ;
	int		nlen = (cl + 1) ;
	if (op) {
	    rs = SR_OK ;
	    if (op->llen == 0) {
	        nlen += 1 ;
	    }
	    if (op->f.comma && (op->c_items > 0)) {
	        nlen += 1 ;
	    }
	} /* end if (non-null) */
	return (rs >= 0) ? nlen : rs ;
}
/* end subroutine (outline_needlength) */


