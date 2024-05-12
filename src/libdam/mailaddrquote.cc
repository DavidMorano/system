/* mailaddrquote SUPPORT */
/* lang=C++20 */

/* subroutine to quote mail-addresses */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was originally written.

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
	int mailaddrquote_start(MAILADDRQUOTE *op,cc *ap,int al,cc **rpp) noex

	Arguments:
	op		object pointer
	ap		raw mail-address to quote
	al		length of specified raw mail-address
	rpp		pointer to returned result

	Returns:
	>=0		length of used destination buffer from conversion
	<0		destination buffer was not big enough or other problem

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<baops.h>
#include	<ascii.h>
#include	<bufstr.h>
#include	<fieldterms.h>
#include	<strn.h>
#include	<sncpyx.h>
#include	<mkchar.h>
#include	<hasx.h>
#include	<localmisc.h>

#include	"mailaddrquote.h"


/* local defines */

#define	MAQ	mailaddrquote


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

static int	mailaddrquote_quote(MAQ *,cc *,cchar *,int) noex ;
static int	siterms(cc *,cchar *,int) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int mailaddrquote_start(MAQ *op,cc *abuf,int alen,cc **rpp) noex {
	int		rs = SR_FAULT ;
	int		len = 0 ;
	if (op && abuf) {
	    char	qterms[32] ;
	    cchar	*qchars = "\"\\<>()" ;
	    cchar	*rp ;
	    bool	f_white ;
	    if (alen < 0) alen = strlen(abuf) ;
    /* what are the SHELL characters that we should look out for? */
	    fieldterms(qterms,0,qchars) ;
	    f_white = haswhite(abuf,alen) ;
	    rp = abuf ;
	    len = alen ;
	    if (f_white || (siterms(qterms,abuf,alen) >= 0)) {
	        if ((rs = mailaddrquote_quote(op,qterms,abuf,alen)) >= 0) {
	            rs = bufstr_get(&op->qaddr,&rp) ;
	            len = rs ;
	        }
	    }
	    if (rpp) {
	        *rpp = (rs >= 0) ? rp : nullptr ;
	    }
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (mailaddrquote_start) */

int mailaddrquote_finish(MAQ *op) noex {
	int		rs = SR_FAULT ;
	int		len = 0 ;
	if (op) {
	    rs = SR_OK ;
	    if (op->f.qaddr) {
	        op->f.qaddr = false ;
	        len = bufstr_finish(&op->qaddr) ;
	        if (rs >= 0) rs = len ;
	    }
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (mailaddrquote_finish) */


/* local subroutines */

static int mailaddrquote_quote(MAQ *op,cc *qterms,cc *abuf,int alen) noex {
	bufstr		*bsp = &op->qaddr ;
	int		rs ;
	int		al ;
	cchar		*ap ;
	if ((rs = bufstr_start(bsp)) >= 0) {
	    int		si ;
	    op->f.qaddr = true ;
	    bufstr_char(bsp,CH_DQUOTE) ;
	    ap = abuf ;
	    al = alen ;
	    while ((rs >= 0) && ((si = siterms(qterms,ap,al)) >= 0)) {
	        bufstr_strw(bsp,ap,si) ;
	        bufstr_char(bsp,CH_BSLASH) ;
	        rs = bufstr_char(bsp,ap[si]) ;
	        ap += (si+1) ;
	        al -= (si+1) ;
	    } /* end while */
	    if ((rs >= 0) && (al > 0)) {
	        rs = bufstr_strw(bsp,ap,al) ;
	    }
	    if (rs >= 0) {
	        rs = bufstr_char(bsp,CH_DQUOTE) ;
	    }
	} /* end if (buffer_start) */
	return rs ;
}
/* end subroutine (mkquoted_quote) */

static int siterms(cc *qterms,cc *sbuf,int slen) noex {
	int		i ;
	int		f = false ;
	if (slen < 0) slen = strlen(sbuf) ;
	for (i = 0 ; (i < slen) && sbuf[i] ; i += 1) {
	    cint	ch = mkchar(sbuf[i]) ;
	    f = BATST(qterms,ch)  ;
	    if (f) break ;
	} /* end for */
	return (f) ? i : -1 ;
}
/* end subroutine (siterms) */


