/* strwcpyxx SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* copy a counted c-string to a destination buffer w/ cone manipulation */
/* version %I% last-modified %G% */


/* revision history:

	= 2000-05-14, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2000 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Group:
	strwcpy{xx}

	Name:
	strwcpychrs

	Description:
	Copy a number of a given specified character to a destination
	buffer.  The resulting string is NUL-terminated.

	Synopsis:
	char *strwcpychrs(char *dp,int ch,int w) noex

	Arguments:
	dp	destiantion buffer pointer
	ch	chacter to copy over (fill)
	w	the maximum length to be filled

	Returns:
	-	the character pointer to the end of the destination


	Name:
	strwcpycompact

	Description:
	We copy a source string to a destination until either the
	end of the source string is reached (by its end-marker) or
	the length of the source string is exhausted.  During the
	copy two additional steps are performed:
	1. white-space characters are removed
	2. upper-case characters are converted to lowercase

	Synopsis:
	char *strwcpycompact(char *dp,cchar *sp,int sl) noex

	Arguments:
	dp	string buffer that receives the copy
	sp	the source string that is to be copied
	sl	the maximum length to be copied

	Returns:
	-	the character pointer to the end of the destination


	Name:
	strwcpyopaque

	Description:
	We copy a source string to a destination until either the
	end of the source string is reached (by its end-marker) or
	the length of the source string is exhausted.  During the
	copy all white-space characters are removed.

	Synopsis:
	char *strwcpyopaque(char *dp,cchar *sp,int sl) noex

	Arguments:
	dp	string buffer that receives the copy
	sp	the source string that is to be copied
	sl	the maximum length to be copied

	Returns:
	-	the character pointer to the end of the destination


	Name:
	strwcpyrev

	Description:
	Copy a source string to a destination until either the end
	of the source string is reached (by its end-marker) or the
	length of the source string is exhausted.

	Synopsis:
	char *strwcpyrev(char *dp,cchar *sp,int sl) noex

	Arguments:
	dp	string buffer that receives the copy
	sp	the source string that is to be copied
	sl	the maximum length to be copied

	Returns:
	-	the character pointer to the end of the destination


	Name:
	strwcpywide

	Description:
	Copy a source wide-string to a destination narrow-string
	buffer until either the end of the source wide-string is
	reached (by its end-marker) or the length of the source
	wide-string is exhausted.

	Synopsis:
	char *strwcpywide(char *dp,cchar *sp,int sl) noex

	Arguments:
	dp	string buffer that receives the copy
	sp	the source string that is to be copied
	sl	the maximum length to be copied

	Returns:
	-	the character pointer to the end of the destination

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<climits>		/* |UCHAR_MAX| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<sfx.h>			/* |sfnext(3cu)| */
#include	<char.h>		/* |CHAR_ISWHITE(3uc)| */
#include	<mkchar.h>
#include	<localmisc.h>

#include	"strwcpyxx.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| */

/* local defines */

#define	ISWHT(ch)	CHAR_ISWHITE(ch)


/* imported namespaces */


/* local typedefs */


/* external subroutines */

extern char * strwcpy(char *,cchar *,int = -1) noex ;


/* external variables */


/* local structues */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

char *strwcpychrs(char *dp,int ch,int n) noex {
    	if (dp) ylikely {
	    while (n-- > 0) {
	        *dp++ = char(ch) ;
	    }
	    *dp = '\0' ;
	} /* end if (non-null) */
	return dp ;
}
/* end subroutine (strwcpychrs) */

char *strwcpycompact(char *dp,cchar *sp,int sl) noex {
    	if (dp && sp) ylikely {
	    cchar	*cp ;
	    if (sl < 0) sl = lenstr(sp) ;
	    for (int cl, c = 0 ; (cl = sfnext(sp,sl,&cp)) > 0 ; ) {
	        if (c++ > 0) {
	            *dp++ =  ' ' ;
	        }
	        dp = strwcpy(dp,cp,cl) ;
	        sl -= intconv((cp + cl) - sp) ;
	        sp = (cp + cl) ;
	    } /* end for (looping through string pieces) */
	    *dp = '\0' ;
	} else {
	    dp = nullptr ;
	} /* end if (non-null) */
	return dp ;
}
/* end subroutine (strwcpycompact) */

char *strwcpyopaque(char *dp,cchar *sp,int sl) noex {
    	if (dp && sp) ylikely {
	    if (sl >= 0) {
	        while (sl && (*sp != '\0')) {
		    if (! ISWHT(*sp)) *dp++ = *sp ;
		    sp += 1 ;
		    sl -= 1 ;
	        }
	    } else {
	        while (*sp != '\0') {
		    if (! ISWHT(*sp)) *dp++ = *sp ;
		    sp += 1 ;
	        }
	    } /* end if */
	    *dp = '\0' ;
	} else {
	    dp = nullptr ;
	}
	return dp ;
}
/* end subroutine (strwcpyopaque) */

char *strwcpyrev(char *dp,cchar *sp,int sl) noex {
    	if (dp && sp) ylikely {
	    if (sl < 0) sl = lenstr(sp) ;
	    for (int i = (sl-1) ; i >= 0 ; i += 1) {
	        *dp++ = sp[i] ;
	    } /* end for */
	    *dp = '\0' ;
	} else {
	    dp = nullptr ;
	}
	return dp ;
}
/* end subroutine (strwcpyrev) */

char *strwcpywide(char *dp,const wchar_t *sp,int sl) noex {
    	if (dp && sp) ylikely {
	    wchar_t ch ;
	    if (sl >= 0) {
	        while (sl-- && *sp) {
		    if ((ch = *sp++) > UCHAR_MAX) ch = '¿' ;
	            *dp++ = char(ch) ;
	        }
	    } else {
	        while (*sp) {
		    if ((ch = *sp++) > UCHAR_MAX) ch = '¿' ;
	            *dp++ = char(ch) ;
	        }
	    } /* end if */
	    *dp = '\0' ;
	} else {
	    dp = nullptr ;
	}
	return dp ;
}
/* end subroutine (strwcpywide) */


