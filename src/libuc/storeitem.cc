/* storeitem SUPPORT */
/* charset=ISO8859-1 */
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
#include	<cstdlib>
#include	<cstdint>
#include	<bit>			/* |has_single_bit(3c++)| */
#include	<usystem.h>
#include	<ctdec.h>
#include	<intceil.h>
#include	<snwcpy.h>
#include	<localmisc.h>		/* |DIGBUFLEN| */

#include	"storeitem.h"

import libutil ;

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
	if (op && dbuf) ylikely {
	    rs = SR_INVALID ;
	    if (dlen > 0) ylikely {
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
	if (op) ylikely {
	    rs = SR_NOTOPEN ;
	    if (op->dbuf) ylikely {
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
	if (op && sp) ylikely {
	    rs = SR_NOTOPEN ;
	    if (op->dbuf) ylikely {
		rs = op->index ;
		if (op->index >= 0) ylikely {
	            int		dlen = (op->dlen - op->index) ;
	            char	*dbuf = (op->dbuf + op->index) ;
		    {
		        char	*dp = dbuf ;
			while (sl-- && *sp && dlen--) {
	    		    *dp++ = *sp++ ;
			}
			*dp = '\0' ;
			if (dlen >= 0) {
	    		    wlen = intconv(dp - dbuf) ;
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
	if (op && vbp) ylikely {
	    rs = SR_NOTOPEN ;
	    if (op->dbuf) ylikely {
		rs = op->index ;
	        if (op->index >= 0) ylikely {
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
	    		    wlen = intconv(dp - dbuf) ;
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
	if (op) ylikely {
	    rs = SR_NOTOPEN ;
	    if (op->dbuf) ylikely {
		rs = op->index ;
	        if (op->index >= 0) ylikely {
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
	            if (rs >= 0) ylikely {
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
	if (op) ylikely {
	    rs = SR_NOTOPEN ;
	    if (op->dbuf) ylikely {
		rs = op->index ;
	        if (op->index >= 0) ylikely {
		    int		dlen = (op->dlen - op->index) ;
		    char	*dbuf = (op->dbuf + op->index) ;
		    if ((dlen >= 0) && ((wlen+1) <= dlen)) ylikely {
	    	        char	*bp = dbuf ;
	    	        *bp++ = char(ch) ;
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
	if (op && vpp) ylikely {
	    rs = SR_NOTOPEN ;
	    if (op->dbuf) ylikely {
		rs = op->index ;
		if (op->index >= 0) ylikely {
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
			    inc = intsat(nv - v) ;
			    if (rlen >= inc) {
	    		        char	*bp = rbuf ;
				rs = SR_OK ;
	                        memclear(bp,inc) ;
	    		        op->index += inc ;
	    		        *vpp = voidp(nv) ;
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
	cint		align = szof(void *) ;
	void		**vpp = voidpp(vppp) ;
	int		rs = SR_FAULT ;
	if (op && vpp) ylikely {
	    rs = SR_INVALID ;
	    if (n > 0) ylikely {
	        cint	bs = ((n + 1) * align) ;
	        if ((rs = storeitem_block(op,bs,align,vpp)) >= 0) ylikely {
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
	if (op) ylikely {
	    rs = SR_NOTOPEN ;
	    if (op->dbuf) ylikely {
		rs = op->index ;
		if (op->index >= 0) ylikely {
		    if (op->f_overflow) rs = SR_OVERFLOW ;
		    len = op->index ;
		} /* end if (no-errors) */
	    } /* end if (open) */
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (storeitem_getlen) */

int storeitem::start(char *rbuf,int rlen) noex {
	return storeitem_start(this,rbuf,rlen) ;
}

int storeitem::strw(cchar *sp,int sl,cchar **rpp) noex {
	return storeitem_strw(this,sp,sl,rpp) ;
}

int storeitem::buf(cvoid *sp,int sl,cchar **rpp) noex {
	return storeitem_buf(this,sp,sl,rpp) ;
}

int storeitem::dec(int v,cchar **rpp) noex {
	return storeitem_dec(this,v,rpp) ;
}

int storeitem::chr(int ch,cchar **rpp) noex {
	return storeitem_chr(this,ch,rpp) ;
}

int storeitem::nul(cchar **rpp) noex {
	return storeitem_nul(this,rpp) ;
}

int storeitem::ptab(int n,void ***vppp) noex {
	return storeitem_ptab(this,n,vppp) ;
}

int storeitem::block(int bsz,int bal,void **vpp) noex {
	return storeitem_block(this,bsz,bal,vpp) ;
}

void storeitem::dtor() noex {
	if (cint rs = int(finish) ; rs < 0) {
	    ulogerror("storeitem",rs,"fini-finish") ;
	}
} /* end method (storeitem::dtor) */

storeitem_co::operator int () noex {
	int		rs = SR_BUGCHECK ;
	if (op) ylikely {
	    switch (w) {
	    case storeitemmem_getlen:
	        rs = storeitem_getlen(op) ;
		break ;
	    case storeitemmem_finish:
	        rs = storeitem_finish(op) ;
		break ;
	    } /* end switch */
	} /* end if */
	return rs ;
}
/* end method (storeitem::operator) */


