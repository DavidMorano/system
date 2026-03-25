/* strlinelen SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* calculate the number of characters in a c-string up to a given maximum */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	strlinelen

	Descrption:
	This subroutine calculates the number of characters within
	a given counted c-string up to a given maximum value or up
	to the end-of-line.  Any end-of-line is not included in the
	returned character count.

	Synopsis:
	int strlinelen(cchar *sp,int sl,int mlen) noex

	Arguments:
	sp	c-string pointer
	sl	c-string length
	mlen	maximum length desired to be returned

	Returns:
	-	maximum length of the string up to its first NL character

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<clanguage.h>
#include	<usysbase.h>
#include	<strn.h>		/* |strnchr(3uc)| */
#include	<localmisc.h>		/* |COLUMNS| */

#include	"strlinelen.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| */
import ulibvals ;			/* |ulibval(3u)| */

/* local defines */


/* imported namespaces */

using std::min ;			/* subroutine-template */
using std::max ;			/* subroutine-template */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */

static cint		maxline = ulibval.maxline ;


/* exported variables */


/* exported subroutines */

int strlinelen(cchar *sp,int sl,int mlen) noex {
	int		len = 0 ; /* return-value */
	if (mlen < 0) mlen = COLUMNS ;
	if (sp && (sl != 0) && (mlen > 0)) {
	    if (sl < 0) sl = lenstr(sp,maxline) ;
	    len = min(sl,mlen) ;
	    if (cchar *tp = strnchr(sp,len,'\n') ; tp) {
	        len = intconv(tp - sp) ;
  	        while ((len > 0) && (sp[len-1] == '\r')) {
		    len -= 1 ;
		}
	    } /* end if (strnchr) */
	} /* end if (have stuff) */
	return len ;
}
/* end subroutine (strlinelen) */

int strlinelen(cchar *sp,int sl) noex {
    	cint mcols = (COLUMNS - 30) ;
    	return strlinelen(sp,sl,mcols) ;
}
/* end subroutine (strlinelen) */


