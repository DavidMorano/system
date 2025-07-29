/* snloadavg SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* make string version of a load-average */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	snloadavg

	Description:
	Ths subroutine creates a string that represents a load-average
	value.  Load-averages -- in their native form -- are
	fixed-point numbers with a 24-bit integer part and an 8-bit
	fractional part.

	Synopsis:
	int snloadavg(char *rbuf,int rlen,uint *la,int w,int p,int fill) noex
	
	Arguments:
	rbuf		destination string buffer
	rlen		destination string buffer length
	la		load-average (32-bit unsigned fixed-point)
	w		width
	p		precision
	fill		fill indicator

	Returns:
	>=0		number of bytes in result
	<0		error (system-return)

	See-also:
	snfsflags(3uc)
	snopenflags(3uc)
	snpollflags(3uc)
	snxtilook(3uc)
	sninetaddr(3uc)
	snsigabbr(3uc)
	snabbr(3uc)
	snshellunder(3uc)
	snfilemode(3uc)
	sntid(3uc)
	snerrabbr(3uc)
	snrealname(3uc)
	snloadavg(3uc)
	snkeyval(3uc)
	snwvprintf(3uc)
	snwprintf(3uc)
	snkeyval(3uc)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/param.h>		/* |FSCALE| + |FSHIFT| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<storebuf.h>
#include	<ctdec.h>
#include	<localmisc.h>		/* |DIGBUFLEN| */

#include	"snx.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct parts {
	uint		parti ;
	uint		partf ;
	int load(uint) noex ;
	int round(int) noex ;
    } ; /* end struct (parts) */
} /* end namespace */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int snloadavg(char *rbuf,int rlen,uint la,int w,int p,int fill) noex {
	int		rs = SR_FAULT ;
	int		len = 0 ; /* return-value */
	if (rbuf) ylikely {
	    storebuf	buf(rbuf,rlen) ;
	    parts	pa ;
	    cint	diglen = DIGBUFLEN ;
	    int		zfprec ;
	    int		dl ;
	    int		prec = -1 ;
	    cchar	*dp ;
	    char	digbuf[DIGBUFLEN+1] ;
	    rs = SR_OK ;
	    if (p > 3) p = 3 ;
	    /* calculate parts */
	    pa.load(la) ;
	    /* round out the value according to the specified precision */
	    pa.round(p) ;
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
	    if ((rs = ctdecpui(digbuf,diglen,zfprec,pa.parti)) >= 0) ylikely {
	        dl = rs ;
	        if ((prec >= 0) && (prec < dl)) {
	            dp += (dl - prec) ;
	            dl = prec ;
	        }
	        rs = buf.strw(dp,dl) ;
	    } /* end if (ctdec) */
	    if ((rs >= 0) && (p >= 0)) ylikely {
	        if ((rs = buf.chr('.')) >= 0) ylikely {
	            if (p > 0) ylikely {
	                if ((rs = ctdecpui(digbuf,diglen,p,pa.partf)) >= 0) {
	                    dl = rs ;
	                    if (dl > p) dl = p ;
	                    rs = buf.strw(digbuf,dl) ;
	                } /* end if (ctdec) */
		    } /* end if (p positive) */
	        } /* end if (storebuf_chr) */
	    } /* end if */
	    len = buf.idx ;
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (snloadavg) */


/* local subroutines */

int parts::load(uint la) noex {
	partf = (la & (FSCALE - 1)) ;
	partf = (partf * 1000) ;
	partf = (partf / FSCALE) ;
	parti = (la >> FSHIFT) ;
	return 0 ;
}
/* end method (parts::load) */

int parts::round(int prec) noex {
	int		r{} ;
	switch (prec) {
	case 3: /* no change needed */
	    break ;
	case 2:
	    r = (partf % 10) ;
	    if (r >= 5) partf += (10 - r) ;
	    break ;
	case 1:
	    r = (partf % 100) ;
	    if (r >= 50) partf += (100 - r) ;
	    break ;
	case 0:
	    r = partf ;
	    if (r >= 500) partf += (1000 - r) ;
	    break ;
	} /* end switch */
	if (partf >= 1000) {
	    partf = (partf % 1000) ;
	    parti += 1 ;
	}
	return 0 ;
}
/* end method (parts::round) */


