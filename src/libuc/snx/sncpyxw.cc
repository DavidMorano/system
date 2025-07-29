/* sncpyxw SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* concatenate strings */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

	= 1999-12-03, David A­D­ Morano
	This was updated to use |strlcpy(3c)| when it was rumored
	to be coming as a new standard.  We are currently using our
	own implementation of that, but when it is supported by
	vendors this will all seemlessly transistion to using the
	vendor version.

*/

/* Copyright © 1998,1999 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Group:
	sncpy{x}w

	Description:
	This subroutine concatenates strings into a single resulting
	string.

	Notes:
	These sorts of subroutines (|sncpyw(3uc)|, |sncpy(3uc)|,
	etc) are quite carefully crafted in order to avoid buffer
	overflow and segfaults as well as leaving the destination
	buffer NUL-terminated.  Please observe and be careful with
	any possible modifications.

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
#include	<climits>		/* |INT_MAX| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstdarg>		/* |va_list(3c)| */
#include	<cstring>		/* |strlcpy(3c)| */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>

#include	"sncpyxw.h"


/* local typedefs */


/* external subroutines */


/* external varaibles */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int sncpy1w(char *dp,int dl,cc *s1,int sl) noex {
	return sncpyxw(dp,dl,1,s1,sl) ;
}
/* end subroutine (sncpy1w) */

int sncpy2w(char *dp,int dl,cc *s1,cc *s2,int sl) noex {
	return sncpyxw(dp,dl,2,s1,s2,sl) ;
}
/* end subroutine (sncpy2w) */

int sncpy3w(char *dp,int dl,cc *s1,cc *s2,cc *s3,int sl) noex {
	return sncpyxw(dp,dl,3,s1,s2,s3,sl) ;
}
/* end subroutine (sncpy3w) */

int sncpy4w(char *dp,int dl,cc *s1,cc *s2,cc *s3,cc *s4,int sl) noex {
	return sncpyxw(dp,dl,4,s1,s2,s3,s4,sl) ;
}
/* end subroutine (sncpy4w) */

int sncpy5w(char *dp,int dl,cc *s1,cc *s2,cc *s3,cc *s4,cc *s5,int sl) noex {
	return sncpyxw(dp,dl,5,s1,s2,s3,s4,s5,sl) ;
}
/* end subroutine (sncpy5w) */

int sncpy6w(char *dp,int dl,cc *s1,cc *s2,cc *s3,cc *s4,cc *s5,
		cc *s6,int sl) noex {
	return sncpyxw(dp,dl,6,s1,s2,s3,s4,s5,s6,sl) ;
}
/* end subroutine (sncpy6w) */

int sncpyxw(char *dp,int dl,int n,...) noex {
	va_list		ap ;
	int		rs = SR_FAULT ;
	int		rl = 0 ; /* return-value */
	if (dl < 0) dl = (INT_MAX - 1) ;
	if (dp) ylikely {
	    size_t	rmlen = (dl + 1) ;
	    char	*bp = dp ;
	    va_begin(ap,n) ;
	    rs = SR_OK ;
	    dp[0] = '\0' ;
	    for (int i = 0 ; (rs >= 0) && (i < n) ; i += 1) {
		size_t	ml ;
	        cc	*sp = (cchar *) va_arg(ap,char *) ;
	        if (i < (n-1)) {
	            ml = strlcpy(bp,sp,rmlen) ;
	            if (ml < rmlen) bp += ml ;
	        } else { /* emulate |strlcpy(3c)| but w/ given length */
	            int		sl = (int) va_arg(ap,int) ;
		    ml = 0 ;
		    while ((ml < (rmlen - 1)) && sl-- && *sp) {
		        *bp++ = *sp++ ;
		        ml += 1 ;
		    }
		    *bp = '\0' ;
		    if ((sl != 0) && *sp) ml += 1 ; /* error condition */
	        } /* end if */
	        if (ml < rmlen) {
		    rmlen -= ml ;
		} else {
		    rs = SR_OVERFLOW ;
		}
	    } /* end for */
	    rl = intconv(bp - dp) ;
	    va_end(ap) ;
	} /* end if (non-null) */
	return (rs >= 0) ? rl : rs ;
}
/* end subroutine (sncpyxw) */


