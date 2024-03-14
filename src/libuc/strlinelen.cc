/* strlinelen SUPPORT */
/* lang=C++20 */

/* special hack -- mostly for debugging */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-01, David A­D­ Morano
	This subroutine was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	strlinelen

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
#include	<cstring>		/* |strlen(3c)| */
#include	<algorithm>		/* |min(3c++)| */
#include	<usystem.h>
#include	<strn.h>		/* |strnchr(3uc)| */


/* local defines */


/* imported namespaces */

using std::nullptr_t ;			/* type */
using std::min ;			/* subroutine-template */


/* external subroutines */


/* exported variables */


/* exported subroutines */

int strlinelen(cchar *sp,int sl,int mlen) noex {
	int		len = 0 ;
	if (mlen < 0) mlen = 0 ;
	if ((sp != nullptr) && (sl != 0) && (mlen > 0)) {
	    if (sl < 0) sl = strlen(sp) ;
	    len = min(sl,mlen) ;
	    if (cchar *tp ; (tp = strnchr(sp,len,'\n')) != nullptr) {
	        len = (tp - sp) ;
  	        while ((len > 0) && (sp[len-1] == '\r')) {
		    len -= 1 ;
		}
	    }
	} /* end if (have stuff) */
	return len ;
}
/* end subroutine (strlinelen) */


