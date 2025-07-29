/* sncpyx SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* concatenate c-strings */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This code was originally written.

	= 1999-12-03, David A­D­ Morano
	This was updated to use |strlcpy(3c)| when it was rumored
	to be coming as a new standard.  We are currently using our
	own implementation of that, but when it is supported by
	vendors this will all seemlessly (pretty much) transistion
	to using the vendor version. My |strlcpy(3uc)| uses an
	|int| for the buffer size while the new |strlcpy(3c)| is
	suppoed to use |size_t|.

*/

/* Copyright © 1998,1999 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Group:
	sncpy{x}

	Description:
	This subroutine concatenates c-strings into a single resulting
	c-string.

	Observation 2015-09-29, David A­D­ Morano:
	Just some musing here: was the choice of changing to use
	|strlcpy(3c)| really such a good idea after all?  Just
	wondering.  Yes, we used it now for tons of years, but was
	it really faster than what we had before?  This goes diddo
	for the other places which changed the code to use
	|strlcpy(3c)|.

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
#include	<localmisc.h>

#include	"sncpyx.h"


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int sncpy1(char *dp,int dl,cc *s1) noex {
	return sncpyx(dp,dl,1,s1) ;
}
/* end subroutine (sncpy1) */

int sncpy2(char *dp,int dl,cc *s1,cc *s2) noex {
	return sncpyx(dp,dl,2,s1,s2) ;
}
/* end subroutine (sncpy2) */

int sncpy3(char *dp,int dl,cc *s1,cc *s2,cc *s3) noex {
	return sncpyx(dp,dl,3,s1,s2,s3) ;
}
/* end subroutine (sncpy3) */

int sncpy4(char *dp,int dl,cc *s1,cc *s2,cc *s3,cc *s4) noex {
	return sncpyx(dp,dl,4,s1,s2,s3,s4) ;
}
/* end subroutine (sncpy4) */

int sncpy5(char *dp,int dl,cc *s1,cc *s2,cc *s3,cc *s4,cc *s5) noex {
	return sncpyx(dp,dl,5,s1,s2,s3,s4,s5) ;
}
/* end subroutine (sncpy5) */

int sncpy6(char *dp,int dl,cc *s1,cc *s2,cc *s3,cc *s4,cc *s5,cc *s6) noex {
	return sncpyx(dp,dl,6,s1,s2,s3,s4,s5,s6) ;
}
/* end subroutine (sncpy6) */

int sncpyx(char *dp,int dl,int n,...) noex {
	va_list		ap ;
	int		rs = SR_FAULT ;
	int		rl = 0 ; /* return-value */
	char		*bp = dp ;
	if (dl < 0) dl = (INT_MAX - 1) ;
	if (dp) ylikely {
	    size_t	rlen = (dl+1) ;
	    va_begin(ap,n) ;
	    rs = SR_OK ;
	    dp[0] = '\0' ;
	    for (int i = 0 ; (rs >= 0) && (i < n) ; i += 1) {
		size_t	ml ;
	        cc	*sp = (cc *) va_arg(ap,cc *) ;
	        if ((ml = strlcpy(bp,sp,rlen)) >= rlen) {
	            rs = SR_OVERFLOW ;
		    break ;
	        }
	        bp += ml ;
	        rlen -= ml ;
	    } /* end for */
	    rl = intconv(bp - dp) ;
	    va_end(ap) ;
	} /* end if (non-null) */
	return (rs >= 0) ? rl : rs ;
}
/* end subroutine (sncpyx) */


