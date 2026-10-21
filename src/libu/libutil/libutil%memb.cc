/* libutil%memb MODULE (implementation) */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* UNIX® kernel support subroutines */
/* version %I% last-modified %G% */


/* revision history:

	= 2020-10-13, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 2020 David A­D­ Morano.  All rights reserved. */

module ;

#include	<envstandards.h>	/* ordered first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstring>		/* CSTD |memcpy(3c)| */
#include	<algorithm>		/* C++STD |min(C++)| */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */
#include	<usysrets.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

module libutil ;


int membcpy(voidp dp,int dl,cvoidp sp,int sl) noex {
	int		rs = SR_FAULT ;
	int		len = 0 ; /* return-value */
	if (dp && sp) ylikely {
	    rs = SR_INVALID ;
	    if (dl >= 0) ylikely {
		rs = SR_OK ;
	        if (sl > 0) ylikely {
		    rs = SR_OVERFLOW ;
		    if (sl <= dl) ylikely {
			rs = SR_OK ;
	            	csize srcsize = size_t(sl) ;
	                memcpy(dp,sp,srcsize) ;
	                len = sl ;
		    } /* end if (ok) */
		} else if (sl < 0) {
		    charp	dstp = cast_static<charp>(dp) ;
		    ccharp	srcp = cast_static<ccharp>(sp) ;
		    while (dl && *srcp) {
	            	*dstp++ = *srcp++ ;
	                dl -= 1 ;
		    } /* end while */
	            len = dl ;
	        } /* end if (source length) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
} /* end subroutine (membcpy) */

int membcpyz(voidp dp,int dl,cvoidp sp,int sl) noex {
	int		rs ;
    	int		len = 0 ; /* return-value */
	if ((rs = membcpy(dp,dl,sp,sl)) >= 0) {
	    if ((len = rs) < dl) {
		caddr_t zp = cast_static<caddr_t>(dp) ;
		rs = memclear((zp + len),(dl - len)) ;
	    } /* end if (zero-out) */
	} /* end if (membcpy) */
	return (rs >= 0) ? len : rs ;
} /* end subroutine (membcpyz) */

int membset(voidp dp,int dl,int ch,int n) noex {
	int		rs = SR_FAULT ;
	int		rl = -1 ; /* return-value */
	if (dp) ylikely {
	    rs = SR_INVALID ;
	    if (dl >= 0) {
		rs = SR_OVERFLOW ;
		if (n <= dl) {
	            rs = SR_OK ;
		    if (n < 0) n = dl ;
	            char *bp = charp(dp) ;
	            while (n-- > 0) {
		        *bp++ = char(ch) ;
	            } /* end while */
	            rl = intconv(bp - charp(dp)) ;
	        } /* end if (no-overflow) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? rl : rs ;
} /* end subroutine (membset) */


