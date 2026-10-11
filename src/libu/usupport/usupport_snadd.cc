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


