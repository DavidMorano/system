/* storeitem SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++23 */

/* storage object */
/* version %I% last-modified %G% */

#define	CF_DEBUG	0		/* debugging */

/* revision history:

	= 1998-02-17, David A­D­ Morano
	This object was originally written for use in some old
	Personal Communication Services (PCS) code (from some time
	ago, 1993?).

	= 2023-11-04, David A­D­ Morano
	I updated this a little bit to take advantage of the built-in
	"bit" features of C++23, like |has_single_bit()|.  Otherwise,
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
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstdarg>		/* CSTD |va_list(3c)| */
#include	<cstdint>		/* CSTD |uintptr_t| */
#include	<bit>			/* C++STD |has_single_bit(3c++)| */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<intceil.h>		/* LIBU */
#include	<stdintx.h>		/* LIBU */
#include	<uclibmem.h>		/* LIBUC */
#include	<ctdec.h>		/* LIBUC */
#include	<snwcpy.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU |DIGBUFLEN| */
#include	<deb.hh>		/* LIBU */

#include	"storeitem.h"

#pragma		GCC dependency		"mod/libutil.ccm"
#pragma		GCC dependency		"mod/deb.ccm"

import libutil ;			/* |lenstr(3u)| */
import deb ;				/* debugging */

/* local defines */

#ifndef	CF_DEBUG
#define	CF_DEBUG	0		/* debugging */
#endif


/* imported namespaces */

using std::has_single_bit ;		/* subroutine-template */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */

cint			szmax		= 32 ;		/* rather arbitary */
cbool			f_debug		= CF_DEBUG ;


/* exported variables */


/* exported subroutines */

int storeitem_start(storeitem *op,char *dbuf,int dlen) noex {
	int		rs = SR_FAULT ;
	DEBPRINTF("ent\n") ;
	if (op && dbuf) ylikely {
	    rs = SR_INVALID ;
	    dbuf[0] = eos ;
	    if (dlen > 0) ylikely {
		rs = SR_OK ;
	        op->dbuf = dbuf ;
	        op->dlen = dlen ;
	        op->index = 0 ;
	        op->f_overflow = false ;
	    } /* end if (valid) */
	} /* end if (non-null) */
	DEBPRINTF("ret rs=%d\n",rs) ;
	return rs ;
} /* end subroutine (storeitem_start) */

int storeitem_finish(storeitem *op) noex {
	int		rs = SR_FAULT ;
	DEBPRINTF("ent\n") ;
	if (op) ylikely {
	    rs = SR_NOTOPEN ;
	    if (op->dbuf) ylikely {
		rs = SR_OVERFLOW ;
		if (!op->f_overflow) {
	    	    rs = op->index ;
		} /* end if (not-overflow) */
		op->dbuf = nullptr ;
		op->index = -1 ;
	    } /* end if (open) */
	} /* end if (non-null) */
	DEBPRINTF("ret rs=%d\n",rs) ;
	return rs ;
} /* end subroutine (storeitem_finish) */

int storeitem_strw(storeitem *op,cchar *sp,int sl,cchar **rpp) noex {
	int		rs = SR_FAULT ;
	int		inc = 0 ; /* return-value */
	if (rpp) *rpp = nullptr ;
	if (op && sp) ylikely {
	    rs = SR_NOTOPEN ;
	    if (op->dbuf) ylikely {
		if ((rs = op->index) >= 0) ylikely {
	            int		dl = (op->dlen - op->index) ;
	            charp	dp = (op->dbuf + op->index) ;
		    {
			charp dbase = dp ;
			while (sl-- && *sp && dl--) {
	    		    *dp++ = *sp++ ;
			} /* end while */
			if (dl >= 1) {
			    *dp++ = eos ;
			    inc = intconv(dp - dbase) ;
	    		    op->index += inc ;
	            	    if (rpp) *rpp = dbase ;
			} else {
	    		    op->f_overflow = true ;
	    		    rs = SR_OVERFLOW ;
			    op->index = rs ;
			} /* end */
		    } /* end block */
		} /* end if (no-errors) */
	    } /* end if (open) */
	} /* end if (non-null) */
	return (rs >= 0) ? inc : rs ;
} /* end subroutine (storeitem_strw) */

int storeitem_buf(storeitem *op,cvoid *vbp,int vbl,cchar **rpp) noex {
	int		rs = SR_FAULT ;
	int		inc = 0 ;
	if (rpp) *rpp = nullptr ;
	if (op && vbp) ylikely {
	    rs = SR_NOTOPEN ;
	    if (op->dbuf) ylikely {
	        if ((rs = op->index) >= 0) ylikely {
		    int		dlen = (op->dlen - op->index) ;
		    charp	dbuf = (op->dbuf + op->index) ;
		    {
		        int	sl = vbl ;
			cchar	*sp = (cchar *) vbp ;
			char	*dp = dbuf ;
			while (sl-- && dlen--) {
	    		    *dp++ = *sp++ ;
			} /* end while */
			if (dlen >= 0) {
	    		    inc = intconv(dp - dbuf) ;
	    		    op->index += inc ;
	    		    if (rpp) *rpp = dbuf ;
			} else {
	    		    op->f_overflow = true ;
	    		    rs = SR_OVERFLOW ;
	    		    op->index = rs ;
			} /* end */
		    } /* end block */
		} /* end if (no-errors) */
	    } /* end if (open) */
	} /* end if (non-null) */
	return (rs >= 0) ? inc : rs ;
} /* end subroutine (storeitem_buf) */

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
		    charp	dbuf = (op->dbuf + op->index) ;
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
		            } /* end */
		        } /* end if */
		    } /* end if (alternatives) */
	            if (rs >= 0) ylikely {
	                op->index += (wlen + 1) ;
	                if (rpp) *rpp = dbuf ;
	            } else {
			op->index = rs ;
		    } /* end */
		} /* end if (no-errors) */
	    } /* end if (open) */
	} /* end if (non-null) */
	return (rs >= 0) ? wlen : rs ;
} /* end subroutine (storeitem_dec) */

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
		    charp	dbuf = (op->dbuf + op->index) ;
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
		    } /* end */
		} /* end if (no-errors) */
	    } /* end if (open) */
	} /* end if (non-null) */
	return (rs >= 0) ? wlen : rs ;
} /* end subroutine (storeitem_chr) */

int storeitem_nul(storeitem *op,cchar **rpp) noex {
	return storeitem_strw(op,op->dbuf,0,rpp) ;
} /* end subroutine (storeitem_nul) */

int storeitem_block(storeitem *op,int bsz,int esz,void ***vppp) noex {
	int		rs = SR_FAULT ;
	int		inc = 0 ; /* return-value */
	DEBPRINTF("ent hsz=%d align=%d\n",bsz,esz) ;
	if (op && vppp) ylikely {
	    if ((rs = op->aligner(esz)) >= 0) {
	        int	bl = (op->dlen - op->index) ;
	        charp	bp = (op->dbuf + op->index) ;
	        inc += rs ;
	        rs = SR_OVERFLOW ;
	DEBPRINTF("mid1 rs=%d inc=%d\n",rs,inc) ;
	        if (bl >= bsz) {
		    voidpp vpp = voidpp(bp) ;
		    rs = memclear(bp,bsz) ;
		    inc += bsz ;
		    op->index += bsz ;
		    *vppp = vpp ;
	        } else {
	            op->f_overflow = true ;
		    op->index = rs ;
	        } /* end if (overflow or not) */
	DEBPRINTF("mid2 rs=%d inc=%d\n",rs,inc) ;
	    } /* end if (aligner) */
	} /* end if (non-null) */
	if (rs < 0) *vppp = nullptr ;
	DEBPRINTF("ret rs=%d inc=%d\n",rs,inc) ;
	return (rs >= 0) ? inc : rs ;
} /* end subroutine (storeitem_block) */

int storeitem_ptab(storeitem *op,int n,void ***vppp) noex {
	cint		esz = szof(void *) ;
	int		rs = SR_FAULT ;
	if (op && vppp) ylikely {
	    rs = SR_INVALID ;
	    if (n >= 0) ylikely {
	        cint	bs = ((n + 1) * esz) ;
		voidpp rpp{} ;
	        if ((rs = storeitem_block(op,bs,esz,&rpp)) >= 0) ylikely {
		    *vppp = rpp ;
		    rpp[n] = nullptr ;
	        } /* end */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (storeitem_ptab) */

int storeitem_getlen(storeitem *op) noex {
	int		rs = SR_FAULT ;
	int		len = 0 ;
	if (op) ylikely {
	    rs = SR_NOTOPEN ;
	    if (op->dbuf) ylikely {
		if ((rs = op->index) >= 0) ylikely {
		    if (op->f_overflow) rs = SR_OVERFLOW ;
		    len = op->index ;
		} /* end if (no-errors) */
	    } /* end if (open) */
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
} /* end subroutine (storeitem_getlen) */

int storeitem::start(char *rbuf,int rlen) noex {
	return storeitem_start(this,rbuf,rlen) ;
} /* end method */

int storeitem::strw(cchar *sp,int sl,cchar **rpp) noex {
	return storeitem_strw(this,sp,sl,rpp) ;
} /* end method */

int storeitem::strw(cchar **rpp,cchar *sp,int sl) noex {
	return storeitem_strw(this,sp,sl,rpp) ;
} /* end method */

int storeitem::buf(cvoid *sp,int sl,cchar **rpp) noex {
	return storeitem_buf(this,sp,sl,rpp) ;
} /* end method */

int storeitem::dec(int v,cchar **rpp) noex {
	return storeitem_dec(this,v,rpp) ;
} /* end method */

int storeitem::chr(int ch,cchar **rpp) noex {
	return storeitem_chr(this,ch,rpp) ;
} /* end method */

int storeitem::nul(cchar **rpp) noex {
	return storeitem_nul(this,rpp) ;
} /* end method */

int storeitem::ptab(int n,void ***vppp) noex {
	return storeitem_ptab(this,n,vppp) ;
} /* end method */

int storeitem::block(int bsz,int esz,void ***vppp) noex {
	return storeitem_block(this,bsz,esz,vppp) ;
} /* end method */

int storeitem::strtab(ccharpp *rsap,con mainv sa) noex  {
    	int		rs = SR_FAULT ;
	int		inc = 0 ; /* return-value */
	DEBPRINTF("ent\n") ;
	if (rsap) ylikely {
	    rs = SR_OK ;
	    if (sa) ylikely {
	        cint n = lenstrarr(sa) ;
	        if (void **tab{} ; (rs = ptab(n,&tab)) >= 0) {
		    int	i ; /* used-afterwards */
	            ccharpp rsa = ccharpp(tab) ;
		    inc += rs ;
	DEBPRINTF("ptab rs=%d inc=%d\n",rs,inc) ;
	            for (i = 0 ; (rs >= 0) && (i < n) && sa[i] ; i += 1) {
	                rs = strw((rsa + i),sa[i]) ;
			inc += rs ;
	DEBPRINTF("strw rs=%d inc=%d\n",rs,inc) ;
	            } /* end for */
	            rsa[i] = nullptr ;
		    *rsap = rsa ;
	DEBPRINTF("for rs=%d inc=%d\n",rs,inc) ;
		} else {
	            *rsap = nullptr ;
	        } /* end if (storeitem-ptab) */
	    } else {
	        *rsap = nullptr ;
	    } /* end if (aliases) */
	} /* end if (non-null) */
	DEBPRINTF("ret rs=%d inc=%d\n",rs,inc) ;
	return (rs >= 0) ? inc : rs ;
} /* end method (storeitem::strtab) */

int storeitem::getinc(int sz) noex {
    	uintptr_t nv, ov = uintptr_t(dbuf) ;
    	nv = ceil(ov,sz) ;
	return intconv(nv - ov) ;
} /* end method (storeitem::getinc) */

int storeitem::aligner(int sz) noex {
    	cuint		usz = uint(sz) ;
    	int		rs = SR_INVALID ;
	int		inc = 0 ; /* return-value */
	DEBPRINTF("ent align=%d\n",sz) ;
	if ((sz >= 0) && (sz <= szmax) && has_single_bit(usz)) {
	    if ((rs = index) >= 0) {
		if ((inc = getinc(sz)) > 0) {
		    index += inc ;
		    if (index > dlen) {
			rs = SR_OVERFLOW ;
			index = rs ;
		    } /* end */
		} /* end */
	    } /* end if (ok) */
	} /* end if (valid-alignment) */
	DEBPRINTF("ret rs=%d inc=%d\n",rs,inc) ;
	return (rs >= 0) ? inc : rs ;
} /* end method (storeitem::aligner) */

int storeitem::wordx(cvoid **rpp,int sz,...) noex {
    	va_list		ap ;
	int		rs ;
	int		inc = 0 ;
	if (rpp) *rpp = nullptr ;
	if ((rs = aligner(sz)) >= 0) {
	    int		bl = (dlen - index) ;
	    charp	bp = (dbuf + index) ;
	    inc += rs ;
	    rs = SR_OVERFLOW ;
	    if (bl >= sz) {
	        va_beg(ap,sz) ;
		rs = SR_OK ;
	        switch (sz) {
	        case szof(char):
	            {
			cint v = (int) va_arg(ap,int) ;
	                charp p = cast_reinterpret<charp>(bp) ;
	                *p = char(v) ;
	            }
	            break ;
	        case szof(short):
	            {
			cint v = (int) va_arg(ap,int) ;
	                shortp p = cast_reinterpret<shortp>(bp) ;
	                *p = short(v) ;
	            }
	            break ;
	        case szof(int):
	            {
			cint v = (int) va_arg(ap,int) ;
	                intp p = cast_reinterpret<intp>(bp) ;
	                *p = v ;
	            }
	            break ;
	        case szof(long):
	            {
			clong v = (long) va_arg(ap,long) ;
	                longp p = cast_reinterpret<longp>(bp) ;
	                *p = v ;
	            }
	            break ;
	        case szof(longlong):
	            {
			clonglong v = (longlong) va_arg(ap,longlong) ;
	                longlongp p = cast_reinterpret<longlongp>(bp) ;
	                *p = v ;
	            }
	            break ;
	        } /* end switch */
		if (rpp) {
		    *rpp = cvoidp(bp) ;
		}
	        inc += sz ;
	        index += sz ;
	        va_end(ap) ;
	    } /* end if (space-available) */
	} /* end if (aligner) */
	return (rs >= 0) ? inc : rs ;
} /* end method (storeitem::wordx) */

void storeitem::dtor() noex {
	if (cint rs = int(finish) ; rs < 0) {
	    ulogerror("storeitem",rs,"fini-finish") ;
	} /* end */
} /* end method (storeitem::dtor) */

storeitem::operator int () noex {
    	return storeitem_getlen(this) ;
} /* end method */

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
} /* end method (storeitem::operator) */


