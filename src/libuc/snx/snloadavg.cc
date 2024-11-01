/* snloadavg SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* make string version of a load-average */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	snloadacg

	Description:
	Ths subroutine creates a string that represents a load-average
	value.  Load-averages -- in their native form -- are
	fixed-point numbers with a 24-bit integer part and an 8-bit
	fractional part.

	Synopsis:
	int snloadavg(char *dbuf,int dlen,uint *la,int w,int p,int fill) noex
	
	Arguments:
	dbuf		destination string buffer
	dlen		destination string buffer length
	la		load-average (32-bit unsigned fixed-point)
	w		width
	p		precision
	fill		fill indicator

	Returns:
	>=0		number of bytes in result
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/param.h>		/* |FSCALE| + |FSHIFT| */
#include	<cstring>
#include	<usystem.h>
#include	<storebuf.h>
#include	<ctdec.h>
#include	<localmisc.h>		/* |DIGBUFLEN| */

#include	"snx.h"


/* local defines */

#define	PARTS		struct parts


/* external subroutines */


/* external variables */


/* local structures */

struct parts {
	uint	parti ;
	uint	partf ;
} ;


/* forward references */

static int	parts_load(PARTS *,uint) noex ;
static int	parts_round(PARTS *,int) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int snloadavg(char *dbuf,int dlen,uint la,int w,int p,int fill) noex {
	int		rs = SR_FAULT ;
	int		i = 0 ;
	if (dbuf) {
	    PARTS	partla, *pp = &partla ;
	    cint	diglen = DIGBUFLEN ;
	    int		zfprec ;
	    int		dl ;
	    int		prec = -1 ;
	    cchar	*dp ;
	    char	digbuf[DIGBUFLEN+1] ;
	    rs = SR_OK ;
	    if (p > 3) p = 3 ;
/* calculate parts */
	    parts_load(pp,la) ;
/* round out the value according to the specified precision */
	    parts_round(pp,p) ;
/* calculate some parameters */
	    if (w >= 0) {
	        if (p > 0) {
	            prec = (w - 1 - p) ;
	        } else {
	            prec = w ;
	        }
	        if (prec < 0) prec = 0 ;
	    } /* end if */
/* put the resulting string together */
	    zfprec = (fill == 0) ? prec : 0 ;
	    dp = digbuf ;
	    if ((rs = ctdecpui(digbuf,diglen,zfprec,pp->parti)) >= 0) {
	        dl = rs ;
	        if ((prec >= 0) && (prec < dl)) {
	            dp += (dl-prec) ;
	            dl = prec ;
	        }
	        rs = storebuf_strw(dbuf,dlen,i,dp,dl) ;
	        i += rs ;
	    } /* end if (ctdec) */
	    if ((rs >= 0) && (p >= 0)) {
	        if ((rs = storebuf_chr(dbuf,dlen,i,'.')) >= 0) {
	            i += rs ;
	            if (p > 0) {
	                if ((rs = ctdecpui(digbuf,diglen,p,pp->partf)) >= 0) {
	                    dl = rs ;
	                    if (dl > p) dl = p ;
	                    rs = storebuf_strw(dbuf,dlen,i,digbuf,dl) ;
	                    i += rs ;
	                } /* end if (ctdec) */
		    } /* end if (p positive) */
	        } /* end if (storebuf_chr) */
	    } /* end if */
	} /* end if (non-null) */
	return (rs >= 0) ? i : rs ;
}
/* end subroutine (snloadavg) */


/* local subroutines */

static int parts_load(PARTS *pp,uint la) noex {
	uint		partf = (la & (FSCALE-1)) ;
	partf = (partf * 1000) ;
	partf = (partf / FSCALE) ;
	pp->parti = (la >> FSHIFT) ;
	pp->partf = partf ;
	return 0 ;
}
/* end subroutine (parts_load) */

static int parts_round(PARTS *pp,int prec) noex {
	int		r ;
	switch (prec) {
	case 3: /* no change needed */
	    break ;
	case 2:
	    r = (pp->partf % 10) ;
	    if (r >= 5) pp->partf += (10-r) ;
	    break ;
	case 1:
	    r = (pp->partf % 100) ;
	    if (r >= 50) pp->partf += (100-r) ;
	    break ;
	case 0:
	    r = pp->partf ;
	    if (r >= 500) pp->partf += (1000-r) ;
	    break ;
	} /* end switch */
	if (pp->partf >= 1000) {
	    pp->partf = (pp->partf % 1000) ;
	    pp->parti += 1 ;
	}
	return 0 ;
}
/* end subroutine (parts_round) */


