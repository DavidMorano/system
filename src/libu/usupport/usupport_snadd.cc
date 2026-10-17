/* usupport_snadd SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* append a c-string to a given counted c-string */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	snaddx

	Description:
	Add some c-string to the end of an existing string.  Return the
	incremental amount added.

	Synopsis:
	int snaddx(char *rbuf,int rlen,int rl,int n,...) noex

	Arguments:
	rbuf		result c-string buffer pointer
	rlen		result c-string buffer length
	rl		length of result buffer already filled
	sp		new c-string pointer to be added
	sl		new c-string length to be added

	Returns:
	>=0		the amount of additional string added to
			the existing c-string buffer
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstdarg>		/* CSTD |va_list(3c)| */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

#include	"usupport_snadd.hh"


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

namespace libu {
    int snaddx(char *rbuf,int rlen,int rl,int n,...) noex {
	va_list		ap ;
	int		rs = SR_FAULT ;
	int		len = 0 ;
	if (rbuf) ylikely {
	    va_begin(ap,n) ;
	    rs = SR_OK ;
	    for (int i = 0 ; (rs >= SR_OK) && (i < n) ; i += 1) {
		cchar	*sp = (char *) va_arg(ap,char *) ;
		rs = snadd(rbuf,rlen,rl,sp) ;
		len += rs ;
		rl += rs ;
	    } /* end for */
	    va_end(ap) ;
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
    } /* end subroutine (snaddx) */
    int snaddw(char *rbuf,int rlen,int i,cchar *sp,int sl) noex {
	int		rs = SR_FAULT ;
	int		len = 0 ;
	if (rbuf && sp) ylikely {
	    rs = SR_INVALID ;
 	    if (i >= 0) ylikely {	
	        char	*bp = (rbuf + i) ;
	        rs = SR_OK ;
	        if (rlen < 0) {
	            if (sl < 0) {
	                while (*sp) {
	                    *bp++ = *sp++ ;
		        } /* end while */
	            } else {
	                while (sl && *sp) {
	                    *bp++ = *sp++ ;
	                    sl -= 1 ;
	                } /* end while */
	            } /* end if */
	        } else {
	            if (sl < 0) {
	                while ((bp < (rbuf + rlen)) && *sp) {
	                    *bp++ = *sp++ ;
		        } /* end while */
		        if ((bp == (rbuf + rlen)) && (*sp != '\0')) {
		            rs = SR_OVERFLOW ;
		        } /* end if (error) */
	            } else {
	                while ((bp < (rbuf + rlen)) && sl && *sp) {
	                    *bp++ = *sp++ ;
	                    sl -= 1 ;
	                } /* end while */
		        if ((bp == (rbuf + rlen)) && sl) {
		            rs = SR_OVERFLOW ;
		        } /* end if (error) */
	            } /* end if */
	        } /* end if */
	        *bp = '\0' ;
		len = intconv(bp - (rbuf + i)) ;
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
    } /* end subroutine (snaddw) */
} /* end namespace (libu) */

namespace libu {
    int snaddslash(char *rbuf,int rlen,int rl) noex {
	int		rs = SR_FAULT ;
	int		len = 0 ;
	if (rbuf) ylikely {
	    rs = SR_INVALID ;
	    if ((rlen >= 0) && (rl >= 0)) ylikely {
	        rs = SR_OK ;
		if ((rl == 0) || (rbuf[rl - 1] != '/')) {
		    rs = SR_OVERFLOW ;
		    if (rl < rlen) {
		        rs = SR_OK ;
		        rbuf[rl] = '/' ;
		        len += 1 ;
		    } /* end if (good) */
		} /* end if (needed) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
    } /* end subroutine (snaddslash) */
} /* end namespace (libu) */

namespace {
    struct adder {
	char	*rbuf ;
	int	rlen ;
	int	rl ;
	adder(charp ªrb,int ªrl,int ªl) noex {
	    rbuf	= ªrb ;
	    rlen	= ªrl ;
	    rl		= ªl ;
	} ; /* end ctor */
	int operator ()	(cchar *,int) noex ;
	int aslash	() noex ;
	int astr	(cchar *,int) noex ;
    } ; /* end struct (adder) */
} /* end namespace */

namespace {
    int snaddpath(char *pbuf,int plen,int pl,cchar *sp,int sl) noex {
	int		rs = SR_FAULT ;
	if (pbuf && sp) ylikely {
	    rs = SR_INVALID ;
	    if ((plen >= 0) && (pl >= 0)) ylikely {
		adder ao(pbuf,plen,pl) ;
		rs = ao(sp,sl) ;
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
    } /* end subroutine (snaddpath) */
} /* end namespace */


/* local subroutines */

int adder::operator () (cchar *sp,int sl) noex {
    	int		rs ;
	int		len = 0 ; /* return-value */
	if ((rs = aslash()) >= 0) {
	    len += rs ;
	    rs = astr(sp,sl) ;
	    len += rs ;
	    rbuf[rl] = eos ;
	} /* end if (aslash) */
	return (rs >= 0) ? len : rs ;
} /* end method (adder::operator) */

int adder::aslash() noex {
	int		rs = SR_OK ;
    	int		len = 0 ;
	if ((rl > 0) && (rbuf[rl - 1] != '/')) {
	    rs = SR_OVERFLOW ;
	    if (rl < rlen) {
		rs = SR_OK ;
	        rbuf[rl++] = '/' ;
	        len += 1 ;
	    } /* end if (good) */
	} /* end if (needed) */
	return (rs >= 0) ? len : rs ;
} /* end method (adder::operator) */

int adder::astr(cchar *sp,int sl) noex {
	int		rs = SR_OVERFLOW ;
	ccharp		ep = (rbuf + rlen) ;
	charp		bp = (rbuf + rl) ;
	if (sl >= 0) {
	    while ((bp < ep) && sl && *sp) {
		*bp++ = *sp++ ;
		sl -= 1 ;
	    } /* end while */
	    if ((sl == 0) || (*sp == eos)) {
		rs = intconv(bp - (rbuf + rl)) ;
		rl += rs ;
	    } /* end if (good) */
	} else {
	    while ((bp < ep) && *sp) {
		*bp++ = *sp++ ;
	    } /* end while */
	    if (*sp == eos) {
		rs = intconv(bp - (rbuf + rl)) ;
		rl += rs ;
	    } /* end if (good) */
	} /* end if */
	return rs ;
} /* end method (adder::operator) */



