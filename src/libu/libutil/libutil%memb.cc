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
#include	<climits>		/* CSTD |INT_MAX| */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstdarg>		/* CSTD */
#include	<cstring>		/* CSTD |strlen(3c)| + |strnlen(3c)| */
#include	<algorithm>		/* C++STD |min(C++)| */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */
#include	<usysrets.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

module libutil ;


int membcpy(void *dp,int dl,cvoid *sp,int sl) noex {
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

int membcpyz(voidp dp,int dl,cvoid *sp,int sl) noex {
	int		rs = SR_FAULT ;
    	int		len = 0 ; /* return-value */
	if (dp && sp) ylikely {
	    rs = SR_INVALID ;
	    if (dl >= 0) ylikely {
	        if (sl > 0) ylikely {
		    rs = SR_OVERFLOW ;
		    if (sl <= dl) ylikely {
	                caddr_t	dst = caddr_t(dp) ;
		        rs = SR_OK ;
	                    csize srcsize = size_t(sl) ;
		            memcpy(dst,sp,srcsize) ;
	                    len = sl ;
		        dst += sl ;
		        if (sl < dl) {
		            csize zsize = size_t(dl - sl) ;
		                memset(dst,0,zsize) ;
		        } /* end if (zero-fill) */
		    } /* end if (no-overflow) */
	        } /* end if (non-zero positive) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
} /* end subroutine (membcpyz) */

int membset(void *dp,int dl,int ch,int n) noex {
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


