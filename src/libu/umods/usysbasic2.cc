/* usysbasic2 MODULE (implementation) */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* get the Present-Working-Directory (PWD) of the process */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-07-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Names:

	Description:

*******************************************************************************/

module ;

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstring>		/* CSTD |stpcpy(3c)| */
#include	<usysbase.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

module usysbasic ;

/* local defines */


/* imported namespaces */


/* local typenames */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

    char *strwcpy(char *dp,cchar *sp,int sl) noex {
    	if (dp && sp) ylikely {
	    if (sl >= 0) {
	        while (sl-- && *sp) *dp++ = *sp++ ;
	        *dp = '\0' ;
	    } else {
	        dp = stpcpy(dp,sp) ;
	    } /* end if */
	} else {
	    dp = nullptr ;
	} /* end if (non-null) */
	return dp ;
    } /* end subroutine (strwcpy) */

    int sncpy1(char *dp,int dl,cchar *sp) noex {
	int		rs = SR_FAULT ;
	int		rl = 0 ; /* return-value */
	if (dp && sp) ylikely {
	    rs = SR_INVALID ;
	    if (dl >= 0) ylikely {
		rs = SR_OK ;
		for (rl = 0 ; (rl < dl) && *sp ; rl += 1) {
		    *dp++ = *sp++ ;
		} /* end for */
		if ((rl == dl) && *sp) {
		    rs = SR_OVERFLOW ;
		}
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? rl : rs ;
    } /* end subroutine (sncpy1) */

    int snwcpy(char *dp,int dl,cchar *sp,int sl) noex {
	int		rs = SR_FAULT ;
	if (dp && sp) ylikely {
	    if (dl >= 0) {
	        if (sl >= 0) {
	            if (sl > dl) {
	                rs = sncpy1(dp,dl,sp) ;
	            } else {
	                rs = intconv(strwcpy(dp,sp,sl) - dp) ;
		    }
	        } else {
	            rs = sncpy1(dp,dl,sp) ;
	        }
	    } else {
	        rs = intconv(strwcpy(dp,sp,sl) - dp) ;
	    }
	} /* end if (non-null) */
	return rs ;
    } /* end subroutine (snwcpy) */


/* local subroutines */


