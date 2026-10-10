/* stpncpyx SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* concatenate strings */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Group:
	stpncpyx
	stpncpy{x}

	Description:
	This subroutine concatenates c-strings into a single resulting
	destination c-string.  It will not overflow the destiantion
	character buffer length.¹  Also, if the number of bytes
	copied ito the destination buffer is less than the given
	length of the destination buffer, the destiantion buffer
	is zero-filled.

	Notes: 
	1. The result is **not** NUL-terminated if the given source
	c-strings amount to more bytes than the given destiantion
	buffer length.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<climits>		/* CSTD */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstring>		/* CSTD |stpncpy(3c)| */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */

#include	"usys_stpncpy.h"


/* local defines */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

char *stpncpy1(char *dp,
		cc *s1,size_t n) noex {
    	cint dl = intconv(n) ;
	return stpncpyx(dp,dl,1,s1) ;
} /* end subroutine (stpncpy1) */

char *stpncpy2(char *dp,
		cc *s1,cc *s2,size_t n) noex {
    	cint dl = intconv(n) ;
	return stpncpyx(dp,dl,2,s1,s2) ;
} /* end subroutine (stpncpy2) */

char *stpncpy3(char *dp,
		cc *s1,cc *s2,cc *s3,size_t n) noex {
    	cint dl = intconv(n) ;
	return stpncpyx(dp,dl,3,s1,s2,s3) ;
} /* end subroutine (stpncpy3) */

char *stpncpy4(char *dp,
		cc *s1,cc *s2,cc *s3,cc *s4,size_t n) noex {
    	cint dl = intconv(n) ;
	return stpncpyx(dp,dl,4,s1,s2,s3,s4) ;
} /* end subroutine (stpncpy4) */

char *stpncpy5(char *dp,
		cc *s1,cc *s2,cc *s3,cc *s4,cc *s5,size_t n) noex {
    	cint dl = intconv(n) ;
	return stpncpyx(dp,dl,5,s1,s2,s3,s4,s5) ;
} /* end subroutine (stpncpy5) */

char *stpncpy6(char *dp,
		cc *s1,cc *s2,cc *s3,cc *s4,cc *s5,cc *s6,size_t n) noex {
    	cint dl = intconv(n) ;
	return stpncpyx(dp,dl,6,s1,s2,s3,s4,s5,s6) ;
} /* end subroutine (stpncpy6) */

char *stpncpyx(char *dp,int dl,int na,...) noex {
	va_list		ap ;
	if (dp) ylikely {
	    va_begin(ap,na) ;
	    bool fbad = false ;
	    char *ep = (dp + dl) ;
	    for (int i = 0 ; (dp < ep) && (i < na) ; i += 1) {
	        ccharp sp = (cchar *) va_arg(ap,char *) ;
		if ((fbad = (sp == nullptr))) break ;
	        while ((dp < ep) && *sp) {
		    *dp++ = *sp++ ;
	        } /* end while */
	    } /* end for */
	    if (dp < ep) {
		for (char *zp = dp ; zp < ep ; zp += 1) {
		    *zp = '\0' ;
		} /* end for */
	    } /* end if (zero-fill) */
	    if (fbad) dp = nullptr ;
	    va_end(ap) ;
	} /* end if (non-null) */
	return dp ;
} /* end subroutine (stpncpyx) */


