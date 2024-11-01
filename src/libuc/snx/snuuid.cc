/* snuuid SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* string-UUID (String-UUID) */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	snuuid

	Description:
	This subroutine creates a string (in the standard representation)
	from a UUID.

	Synopsis:
	int snuuid(char *rbuf,int rlen,MKUUID *up) noex

	Arguments:
	rbuf		result buffer
	rlen		size of supplied result buffer
	up		pointer to MKUUID object holding a UUID

	Returns:
	>=0		resulting length
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<climits>		/* |UINT_MAX| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<usystem.h>
#include	<sbuf.h>
#include	<cthex.h>
#include	<mkuuid.h>
#include	<localmisc.h>		/* |DIGBUFLEN| */

#include	"snx.h"


/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

static int sbuf_hexp(sbuf *,uint64_t,int) noex ;


/* exported variables */


/* exported subroutines */

int snuuid(char *dbuf,int dlen,MKUUID *up) noex {
	int		rs = SR_FAULT ;
	int		len = 0 ;
	if (dbuf && up) {
	    sbuf	b ;
	    if ((rs = b.start(dbuf,dlen)) >= 0) {
	        uint64_t	v = (up->time & UINT_MAX) ;
	        if (rs >= 0) rs = sbuf_hexp(&b,v,4) ;
	        if (rs >= 0) rs = b.chr('-') ;
	        v = ((up->time >> 32) & UINT_MAX) ;
	        if (rs >= 0) rs = sbuf_hexp(&b,v,2) ;
	        if (rs >= 0) rs = b.chr('-') ;
	        if (rs >= 0) {
		    uint64_t	tv ;
		    v = 0 ;
		    tv = ((up->time >> 48) & UINT_MAX) ;
		    v |= (tv & UCHAR_MAX) ;
		    tv = (up->version & (16-1)) ;
		    v |= (tv << 8) ;
		    tv = ((up->time >> 48) & UINT_MAX) ;
		    v |= (tv << (8+4)) ;
	            rs = sbuf_hexp(&b,v,2) ;
	        } /* end if (ok) */
	        if (rs >= 0) rs = b.chr('-') ;
	        v = (up->clk & UINT_MAX) ;
	        v &= (~ (3 << 14)) ;
	        v |= (2 << 14) ;		/* standardized variant */
	        if (rs >= 0) rs = sbuf_hexp(&b,v,2) ;
	        if (rs >= 0) rs = b.chr('-') ;
	        v = up->node ;
	        if (rs >= 0) rs = sbuf_hexp(&b,v,6) ;
	        len = b.finish ;
	        if (rs >= 0) rs = len ;
	    } /* end if (sbuf) */
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (snuuid) */

static int sbuf_hexp(sbuf *bp,uint64_t v,int n) noex {
	uint		vi ;
	cint		dlen = DIGBUFLEN ;
	int		rs ;
	char		dbuf[DIGBUFLEN+1] ;
	switch (n) {
	case 2:
	    vi = (uint) v ;
	    rs = cthexus(dbuf,dlen,vi) ;
	    break ;
	case 4:
	    vi = (uint) v ;
	    rs = cthexui(dbuf,dlen,vi) ;
	    break ;
	case 6:
	    rs = cthexull(dbuf,dlen,v) ;
	    break ;
	default:
	    rs = SR_INVALID ;
	    break ;
	} /* end switch */
	if (rs >= 0) {
	    cchar	*dp = dbuf ;
	    if (n == 6) dp += ((8-n)*2) ;
	    rs = bp->strw(dp,(n*2)) ;
	} /* end if */
	return rs ;
}
/* end subroutine (sbuf_hexp) */


