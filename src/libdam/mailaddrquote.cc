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
#include	<climits>		/* |UCHAR_MAX| + |CHAR_BIT| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>		/* |lenstr(3c)| */
#include	<usystem.h>
#include	<baops.h>
#include	<ascii.h>
#include	<bufstr.h>
#include	<fieldterms.h>
#include	<strn.h>
#include	<six.h>			/* |siterm(3uc)| */
#include	<sncpyx.h>
#include	<mkchar.h>
#include	<hasx.h>
#include	<localmisc.h>

#include	"mailaddrquote.h"

import libutil ;

/* local defines */

#define	MAQ	mailaddrquote


/* imported namespaces */

using std::nullptr_t ;			/* type */
using std::nothrow ;			/* constant */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

template<typename ... Args>
static int mailaddrquote_ctor(MAQ *op,Args ... args) noex {
    	MAQ		*hop = op ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    cnullptr	np{} ;
	    rs = SR_NOMEM ;
	    memclear(hop) ;
	    if ((op->bsp = new(nothrow) bufstr) != np) {
		rs = SR_OK ;
	    } /* end if (new-bufstr) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (mailaddrquote_ctor) */

static int mailaddrquote_dtor(MAQ *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	    if (op->bsp) {
		delete op->bsp ;
		op->bsp = nullptr ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (mailaddrquote_dtor) */

template<typename ... Args>
static inline int mailaddrquote_magic(MAQ *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magic == MAILADDRQUOTE_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}
/* end subroutine (mailaddrquote_magic) */

static int	mailaddrquote_quote(MAQ *,cchar *,int) noex ;


/* local variables */

constexpr int		termsize = ((UCHAR_MAX+1)/CHAR_BIT) ;

constexpr char		qchars[] = R"xx("\<>())xx" ;

static char		qterms[termsize] ;


/* exported variables */


/* exported subroutines */

int mailaddrquote_start(MAQ *op,cc *abuf,int alen,cc **rpp) noex {
	int		rs ;
	int		len = 0 ;
	cchar		*rp = nullptr ;
	if ((rs = mailaddrquote_ctor(op,abuf)) >= 0) {
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
	            }
	        }
		if (rs >= 0) {
		    op->magic = MAILADDRQUOTE_MAGIC ;
		}
	    } /* end if (fieldterms) */
	    if (rs < 0) {
		mailaddrquote_dtor(op) ;
	    }
	} /* end if (mailaddrquote_ctor) */
	if (rpp) {
	    *rpp = (rs >= 0) ? rp : nullptr ;
	}
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (mailaddrquote_start) */

int mailaddrquote_finish(MAQ *op) noex {
	int		rs ;
	int		rs1 ;
	int		len = 0 ;
	if ((rs = mailaddrquote_magic(op)) >= 0) {
	    if (op->bsp && op->fl.qaddr) {
	        op->fl.qaddr = false ;
	        len = bufstr_finish(op->bsp) ;
	        if (rs >= 0) rs = len ;
	    }
	    {
		rs1 = mailaddrquote_dtor(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (mailaddrquote_finish) */


/* local subroutines */

static int mailaddrquote_quote(MAQ *op,cc *abuf,int alen) noex {
	bufstr		*bsp = op->bsp ;
	int		rs ;
	if ((rs = bufstr_start(bsp)) >= 0) {
	    int		si ;
	    int		al = alen ;
	    cchar	*ap = abuf ;
	    op->fl.qaddr = true ;
	    bufstr_chr(bsp,CH_DQUOTE) ;
	    while ((rs >= 0) && ((si = siterm(ap,al,qterms)) >= 0)) {
	        bufstr_strw(bsp,ap,si) ;
	        bufstr_chr(bsp,CH_BSLASH) ;
	        rs = bufstr_chr(bsp,ap[si]) ;
	        ap += (si+1) ;
	        al -= (si+1) ;
	    } /* end while */
	    if ((rs >= 0) && (al > 0)) {
	        rs = bufstr_strw(bsp,ap,al) ;
	    }
	    if (rs >= 0) {
	        rs = bufstr_chr(bsp,CH_DQUOTE) ;
	    }
	} /* end if (buffer_start) */
	return rs ;
}
/* end subroutine (mkquoted_quote) */


