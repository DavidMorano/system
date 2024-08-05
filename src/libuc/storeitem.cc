/* storeitem SUPPORT */
/* lang=C++23 */

/* storage object */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-17, David A­D­ Morano
	This object was originally written for use in some old
	Personal Communication Services (PCS) code (from some time
	ago, 1993?).

	= 2023-11-04, David A­D­ Morano
	I updated this a little bit to take advantage of the built-in
	"bit" features of C++23, like |has_isingle_bit()|.  Otherwise,
	this code could have stayed in C89! :-)  Am I just looking
	for an excuse to try out some C++23 feature?

*/

/* Copyright © 1998,2023 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	storeitem

	Description:
	This object is used to store individual items of data into
	a common buffer.  An example is to store individual c-strings
	into a common buffer when creating a PASSWD entry from
	parsing a c-string from the PASSWD database (for example).

	Arguments:
	op		pointer to the storage object
	<others>	depending on the particular call made

	Returns:
	>=0		length of the newly stored item
	<0		error code (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdint>
#include	<cstring>
#include	<bit>
#include	<usystem.h>
#include	<ctdec.h>
#include	<intceil.h>
#include	<snwcpy.h>
#include	<localmisc.h>		/* |DIGBUFLEN| */

#include	"storeitem.h"


/* local defines */


/* imported namespaces */

using std::has_single_bit ;		/* subroutine-template */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int storeitem_start(storeitem *op,char *dbuf,int dlen) noex {
	int		rs = SR_FAULT ;
	if (op && dbuf) {
	    rs = SR_INVALID ;
	    if (dlen > 0) {
		rs = SR_OK ;
	        op->dbuf = dbuf ;
	        op->dlen = dlen ;
	        op->index = 0 ;
	        op->f_overflow = false ;
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (storeitem_start) */

int storeitem_finish(storeitem *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_NOTOPEN ;
	    if (op->dbuf) {
		rs = SR_OVERFLOW ;
		if (!op->f_overflow) {
	    	    rs = (op->index + 1) ;
		} /* end if (not-overflow) */
		op->dbuf = nullptr ;
		op->index = -1 ;
	    } /* end if (open) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (storeitem_finish) */

int storeitem_strw(storeitem *op,cchar *sp,int sl,cchar **rpp) noex {
	int		rs = SR_FAULT ;
	int		wlen = 0 ;
	if (rpp) *rpp = nullptr ;
	if (op && sp) {
	    rs = SR_NOTOPEN ;
	    if (op->dbuf) {
		rs = op->index ;
		if (op->index >= 0) {
	            int		dlen = (op->dlen - op->index) ;
	            char	*dbuf = (op->dbuf + op->index) ;
		    {
		        char	*dp = dbuf ;
			while (sl-- && *sp && dlen--) {
	    		    *dp++ = *sp++ ;
			}
			*dp = '\0' ;
			if (dlen >= 0) {
	    		    wlen = (dp - dbuf) ;
	    		    op->index += (wlen + 1) ;
	            	    if (rpp) *rpp = dbuf ;
			} else {
	    		    op->f_overflow = true ;
	    		    rs = SR_OVERFLOW ;
			    op->index = rs ;
			}
		    } /* end block */
		} /* end if (no-errors) */
	    } /* end if (open) */
	} /* end if (non-null) */
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (storeitem_strw) */

int storeitem_buf(storeitem *op,cvoid *vbp,int vbl,cchar **rpp) noex {
	int		rs = SR_FAULT ;
	int		wlen = 0 ;
	if (rpp) *rpp = nullptr ;
	if (op && vbp) {
	    rs = SR_NOTOPEN ;
	    if (op->dbuf) {
		rs = op->index ;
	        if (op->index >= 0) {
		    int		dlen = (op->dlen - op->index) ;
		    char	*dbuf = (op->dbuf + op->index) ;
		    {
		        int	sl = vbl ;
			cchar	*sp = (cchar *) vbp ;
			char	*dp = dbuf ;
			while (sl-- && dlen--) {
	    		    *dp++ = *sp++ ;
			}
			*dp = '\0' ;
			if (dlen >= 0) {
	    		    wlen = (dp - dbuf) ;
	    		    op->index += (wlen + 1) ;
	    		    if (rpp) *rpp = dbuf ;
			} else {
	    		    op->f_overflow = true ;
	    		    rs = SR_OVERFLOW ;
	    		    op->index = rs ;
			}
		    } /* end block */
		} /* end if (no-errors) */
	    } /* end if (open) */
	} /* end if (non-null) */
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (storeitem_buf) */

int storeitem_dec(storeitem *op,int v,cchar **rpp) noex {
	int		rs = SR_OK ;
	int		wlen = 0 ;
	if (rpp) *rpp = nullptr ;
	if (op) {
	    rs = SR_NOTOPEN ;
	    if (op->dbuf) {
		rs = op->index ;
	        if (op->index >= 0) {
	    	    cint	tlen = DIGBUFLEN ;
		    int		dlen = (op->dlen - op->index) ;
		    char	*dbuf = (op->dbuf + op->index) ;
	            if (dlen >= (tlen+1)) {
	                rs = ctdeci(dbuf,tlen,v) ;
	                wlen = rs ;
	            } else {
	                char	tbuf[DIGBUFLEN+1] ;
	                if ((rs = ctdeci(tbuf,tlen,v)) >= 0) {
		            if (dlen >= (rs+1)) {
			        rs = snwcpy(dbuf,dlen,tbuf,rs) ;
			        wlen = rs ;
		            } else {
	                        op->f_overflow = true ;
	                        rs = SR_OVERFLOW ;
		            }
		        } /* end if */
		    } /* end if (alternatives) */
	            if (rs >= 0) {
	                op->index += (wlen + 1) ;
	                if (rpp) *rpp = dbuf ;
	            } else {
			op->index = rs ;
		    }
		} /* end if (no-errors) */
	    } /* end if (open) */
	} /* end if (non-null) */
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (storeitem_dec) */

int storeitem_chr(storeitem *op,int ch,cchar **rpp) noex {
	cint		wlen = 1 ;
	int		rs = SR_FAULT ;
	if (rpp) *rpp = nullptr ;
	if (op) {
	    rs = SR_NOTOPEN ;
	    if (op->dbuf) {
		rs = op->index ;
	        if (op->index >= 0) {
		    int		dlen = (op->dlen - op->index) ;
		    char	*dbuf = (op->dbuf + op->index) ;
		    if ((dlen >= 0) && ((wlen+1) <= dlen)) {
	    	        char	*bp = dbuf ;
	    	        *bp++ = ch ;
	    	        *bp = '\0' ;
	    	        op->index += (wlen + 1) ;
	    	        if (rpp) *rpp = dbuf ;
		    } else {
	    	        op->f_overflow = true ;
	    	        rs = SR_OVERFLOW ;
	    	        op->index = rs ;
		    }
		} /* end if (no-errors) */
	    } /* end if (open) */
	} /* end if (non-null) */
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (storeitem_chr) */

int storeitem_nul(storeitem *op,cchar **rpp) noex {
	return storeitem_strw(op,op->dbuf,0,rpp) ;
}
/* end subroutine (storeitem_nul) */

int storeitem_block(storeitem *op,int bsize,int align,void **vpp) noex {
	int		rs = SR_FAULT ;
	int		inc = 0 ;
	if (op && vpp) {
	    rs = SR_NOTOPEN ;
	    if (op->dbuf) {
		rs = op->index ;
		if (op->index >= 0) {
		    uint	ua = uint(align) ;
		    rs = SR_INVALID ;
		    if ((bsize > 0) && (align > 0) && has_single_bit(ua)) {
			int	rlen = (op->dlen + op->index) ;
			char	*rbuf = (op->dbuf + op->index) ;
			{
			    uintptr_t	nv ;
			    uintptr_t	v = uintptr_t(rbuf) ;
			    nv = uipceil(v,align) ;
			    nv += uintptr_t(iceil(bsize,align)) ;
			    inc = (nv - v) ;
			    if (rlen >= inc) {
	    		        char	*bp = rbuf ;
				rs = SR_OK ;
	                        memclear(bp,inc) ;
	    		        op->index += inc ;
	    		        *vpp = (void *) nv ;
			    } else {
	    	        	op->f_overflow = true ;
	    			rs = SR_OVERFLOW ;
	    			op->index = rs ;
			    }
			} /* end block */
		    } /* end if (valid arguments) */
		} /* end if (no-errors) */
	    } /* end if (open) */
	} /* end if (non-null) */
	return (rs >= 0) ? inc : rs ;
}
/* end subroutine (storeitem_block) */

int storeitem_ptab(storeitem *op,int n,void ***vppp) noex {
	cint		align = sizeof(void *) ;
	void		**vpp = (void **) vppp ;
	int		rs = SR_FAULT ;
	if (op && vpp) {
	    rs = SR_INVALID ;
	    if (n > 0) {
	        cint	bs = ((n+1) * align) ;
	        if ((rs = storeitem_block(op,bs,align,vpp)) >= 0) {
		    (*vppp)[n] = nullptr ;
	        }
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (storeitem_ptab) */

int storeitem_getlen(storeitem *op) noex {
	int		rs = SR_FAULT ;
	int		len = 0 ;
	if (op) {
	    rs = SR_NOTOPEN ;
	    if (op->dbuf) {
		rs = op->index ;
		if (op->index >= 0) {
		    if (op->f_overflow) rs = SR_OVERFLOW ;
		    len = op->index ;
		} /* end if (no-errors) */
	    } /* end if (open) */
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (storeitem_getlen) */


