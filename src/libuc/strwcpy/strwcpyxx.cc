/* strwcpyxx SUPPORT */
/* lang=C++20 */

/* copy a c-string to a maximum extent */
/* version %I% last-modified %G% */


/* revision history:

	= 2000-05-14, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2000 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	strwcpyblanks

	Description:
	Copy blank characters to a destination for the number of
	characters specified.  The resulting string is NUL-terminated.

	Synopsis:
	char *strwcpyblanks(char *dp,int w) noex

	Arguments:
	dp	string buffer that receives the copy
	w	the maximum length to be copied

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
#include	<climits>
#include	<cstddef>
#include	<cstring>		/* <- |strlen(3c)| */
#include	<utypedefs.h>
#include	<sfx.h>
#include	<strwcpy.h>
#include	<char.h>
#include	<mkchar.h>
#include	<localmisc.h>

#include	"strwcpyxx.h"


/* local defines */


/* imported namespaces */


/* external subroutines */


/* exported subroutines */

char *strwcpyblanks(char *dp,int w) noex {
	while (w-- > 0) *dp++ = ' ' ;
	*dp = '\0' ;
	return dp ;
}
/* end subroutine (strwcpyblanks) */

char *strwcpycompact(char *dp,cchar *sp,int sl) noex {
	int		c = 0 ;
	int		cl ;
	cchar		*cp ;
	if (sl < 0) sl = strlen(sp) ;
	while ((cl = sfnext(sp,sl,&cp)) > 0) {
	    if (c++ > 0) {
	        *dp++ =  ' ' ;
	    }
	    dp = strwcpy(dp,cp,cl) ;
	    sl -= ((cp+cl) - sp) ;
	    sp = (cp+cl) ;
	} /* end while (looping through string pieces) */
	*dp = '\0' ;
	return dp ;
}
/* end subroutine (strwcpycompact) */

char *strwcpyopaque(char *dp,cchar *sp,int sl) noex {
	if (sl >= 0) {
	    while (sl && (*sp != '\0')) {
		if (! CHAR_ISWHITE(*sp)) *dp++ = *sp ;
		sp += 1 ;
		sl -= 1 ;
	    }
	} else {
	    while (*sp != '\0') {
		if (! CHAR_ISWHITE(*sp)) *dp++ = *sp ;
		sp += 1 ;
	    }
	} /* end if */
	*dp = '\0' ;
	return dp ;
}
/* end subroutine (strwcpyopaque) */

char *strwcpyrev(char *dp,cchar *sp,int sl) noex {
	int		i ;
	if (sl < 0) sl = strlen(sp) ;
	for (i = (sl-1) ; i >= 0 ; i += 1) {
	    *dp++ = sp[i] ;
	} /* end for */
	*dp = '\0' ;
	return dp ;
}
/* end subroutine (strwcpyrev) */

char *strwcpywide(char *dp,const wchar_t *sp,int sl) noex {
	int		ch ;
	if (sl >= 0) {
	    while (sl-- && *sp) {
		if ((ch = mkchar(*sp++)) >= UCHAR_MAX) ch = '¿' ;
	        *dp++ = char(ch) ;
	    }
	} else {
	    while (*sp) {
		if ((ch = mkchar(*sp++)) >= UCHAR_MAX) ch = '¿' ;
	        *dp++ = char(ch) ;
	    }
	} /* end if */
	*dp = '\0' ;
	return dp ;
}
/* end subroutine (strwcpywide) */


