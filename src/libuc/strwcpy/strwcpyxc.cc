/* strwcpyxc SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* copy a c-string to "x" case */
/* version %I% last-modified %G% */


/* revision history:

	= 2000-05-14, David A�D� Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright � 2000 David A�D� Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	strwcpyxc

	Description
	Copy a source string to a destination until either the end
	of the source string is reached (by its end-marker) or the
	length of the source string is exhausted.

	Synopsis:
	char *strwcpyxc(char *dp,cchar *sp,int sl) noex

	Arguments:
	dp	string buffer that receives the copy
	sp	the source string that is to be copied
	sl	the maximum length to be copied

	Returns:
	-	the character pointer to the end of the destination


*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<toxc.h>
#include	<localmisc.h>

#include	"strwcpyxc.h"


/* local defines */


/* imported namespaces */


/* local typedefs */

extern "C" {
    typedef int (*toxc_f)(int) noex ;
}


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local subroutine-templates */

template<toxc_f toxc>
char *strwcpyxc(char *dp,cchar *sp,int sl) noex {
	if (sl >= 0) {
	    while (sl-- && *sp) *dp++ = toxc(*sp++) ;
	} else {
	    while (*sp) *dp++ = toxc(*sp++) ;
	} /* end if */
	*dp = '\0' ;
	return dp ;
}
/* end subroutine-template (strwcpyxc) */


/* local variables */


/* exported variables */


/* exported subroutines */

char *strwcpybc(char *dp,cchar *sp,int sl) noex {
	return strwcpyxc<tobc>(dp,sp,sl) ;
}

char *strwcpylc(char *dp,cchar *sp,int sl) noex {
	return strwcpyxc<tolc>(dp,sp,sl) ;
}

char *strwcpyuc(char *dp,cchar *sp,int sl) noex {
	return strwcpyxc<touc>(dp,sp,sl) ;
}

char *strwcpyfc(char *dp,cchar *sp,int sl) noex {
	return strwcpyxc<tofc>(dp,sp,sl) ;
}


