/* mailaddrquote SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* quote mail-addresses */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	mailaddrquote

	Description:
	This subroutine examines a string supplied by the caller
	and produces an output string with any necessary quotes
	applied appropriate for Internet mail addresses.

	Synopsis:
	int mailaddrquote_start(MAQ *op,cc *ap,int al,cc **rpp) noex

	Arguments:
	op		object pointer
	ap		raw mail-address to quote
	al		length of specified raw mail-address
	rpp		pointer to returned result

	Returns:
	>=0		length of used destination buffer from conversion
	<0		error code (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<climits>		/* CSTD |UCHAR_MAX| + |CHAR_BIT| */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstring>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<baops.h>		/* LIBU */
#include	<ascii.h>		/* LIBU */
#include	<bufstr.h>		/* LIBUC */
#include	<fieldterms.h>		/* LIBUC */
#include	<strn.h>		/* LIBUC */
#include	<six.h>			/* LIBUC |siterm(3uc)| */
#include	<sncpyx.h>		/* LIBUC */
#include	<hasx.h>		/* LIBUC */
#include	<mkchar.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

#include	"mailaddrquote.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| */

/* local defines */

#define	MAQ	mailaddrquote


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

template<typename ... Args>
local int mailaddrquote_ctor(MAQ *op,Args ... args) noex {
    	MAQ		*hop = op ;
	cnullptr	np{} ;
	cnothrow	nt{} ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    rs = SR_NOMEM ;
	    memclear(hop) ;
	    if ((op->bsp = new(nt) bufstr) != np) ylikely {
		rs = SR_OK ;
	    } /* end if (new-bufstr) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (mailaddrquote_ctor) */

local int mailaddrquote_dtor(MAQ *op) noex {
	int		rs = SR_FAULT ;
	if (op) ylikely {
	    rs = SR_OK ;
	    if (op->bsp) ylikely {
		delete op->bsp ;
		op->bsp = nullptr ;
	    } /* end if (memory-release) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (mailaddrquote_dtor) */

template<typename ... Args>
local inline int mailaddrquote_magic(MAQ *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    rs = (op->magval == MAILADDRQUOTE_MAGIC) ? SR_OK : SR_NOTOPEN ;
	} /* end if */
	return rs ;
} /* end subroutine (mailaddrquote_magic) */

local int	mailaddrquote_quote(MAQ *,cchar *,int) noex ;


/* local variables */

constexpr int		termsize = ((UCHAR_MAX +1 ) / CHAR_BIT) ;

constexpr char		qchars[] = R"xx("\<>())xx" ;

static char		qterms[termsize] ;


/* exported variables */


/* exported subroutines */

int mailaddrquote_start(MAQ *op,cc *abuf,int alen,cc **rpp) noex {
	int		rs ;
	int		len = 0 ;
	cchar		*rp = nullptr ;
	if ((rs = mailaddrquote_ctor(op,abuf)) >= 0) ylikely {
	    static cint		rsf = fieldterms(qterms,0,qchars) ;
	    if (alen < 0) alen = lenstr(abuf) ;
	    if ((rs = rsf) >= 0) {
	        cbool	f_white = haswhite(abuf,alen) ;
	        rp = abuf ;
	        len = alen ;
	        if (f_white || (siterm(abuf,alen,qterms) >= 0)) {
	            if ((rs = mailaddrquote_quote(op,abuf,alen)) >= 0) {
	                rs = bufstr_get(op->bsp,&rp) ;
	                len = rs ;
	            } /* end if (mailaddrquote_quote) */
	        } /* end if */
		if (rs >= 0) {
		    op->magval = MAILADDRQUOTE_MAGIC ;
		} /* end if (ok) */
	    } /* end if (fieldterms) */
	    if (rs < 0) {
		mailaddrquote_dtor(op) ;
	    } /* end if (error) */
	} /* end if (mailaddrquote_ctor) */
	if (rpp) {
	    *rpp = (rs >= 0) ? rp : nullptr ;
	}
	return (rs >= 0) ? len : rs ;
} /* end subroutine (mailaddrquote_start) */

int mailaddrquote_finish(MAQ *op) noex {
	int		rs ;
	int		rs1 ;
	int		len = 0 ;
	if ((rs = mailaddrquote_magic(op)) >= 0) ylikely {
	    if (op->bsp && op->fl.qaddr) {
		bufstr *bsp = op->bsp ;
	        op->fl.qaddr = false ;
	        len = bsp->finish ;
	        if (rs >= 0) rs = len ;
	    } /* end if (possible cleanup) */
	    {
		rs1 = mailaddrquote_dtor(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
} /* end subroutine (mailaddrquote_finish) */


/* local subroutines */

local int mailaddrquote_quote(MAQ *op,cc *abuf,int alen) noex {
	int		rs ;
	int		rs1 ;
	if (bufstr *bsp = op->bsp ; (rs = bsp->start) >= 0) ylikely {
	    op->fl.qaddr = true ;
	    {
	        int	al = alen ;
	        cchar	*ap = abuf ;
	        if ((rs = bsp->chr(CH_DQUOTE)) >= 0) {
	            for (int si ; (si = siterm(ap,al,qterms)) >= 0 ; ) {
	                bsp->strw(ap,si) ;
	                bsp->chr(CH_BSLASH) ;
	                rs = bsp->chr(ap[si]) ;
	                ap += (si + 1) ;
	                al -= (si + 1) ;
		        if (rs < 0) break ;
	            } /* end for */
	            if ((rs >= 0) && (al > 0)) {
	                rs = bsp->strw(ap,al) ;
	            }
	            if (rs >= 0) {
	                rs = bsp->chr(CH_DQUOTE) ;
	            }
		} /* end if (bufstr_chr) */
	    } /* end block */
	    op->fl.qaddr = true ;
	    rs1 = bsp->finish ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (buffer_start) */
	return rs ;
} /* end subroutine (mkquoted_quote) */


