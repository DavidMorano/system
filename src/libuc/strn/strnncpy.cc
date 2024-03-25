/* strnncpy SUPPORT */
/* lang=C++20 */

/* copy a string and fill destination out with NULs */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A�D� Morano
	This subroutine was originally written.

*/

/* Copyright � 1998 David A�D� Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	strnncpy

	Description:
	Copy a source string to a destination until either the end
	of the source string is reached (by its end-marker) or the
	length of the source string is exhausted.  Additionally we
	always zero out to the length of the destination string.

	Synopsis:
	char *strnncpy(char *d,cchar *s,int slen,int n) noex

	Arguments:
	d	string buffer that receives the copy
	s	the source string that is to be copied
	slen	length of string to copy
	n	the maximum length to be copied

	Returns:
	-	the character pointer to the end of the destination

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstring>		/* |strncpy(3c)| */
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<clanguage.h>
#include	<localmisc.h>

#include	"strn.h"


/* local defines */


/* imported namespaces */

using std::min ;			/* subroutine-template */
using std::max ;			/* subroutine-template */


/* external subroutines */

extern "C" {
    extern char	*strwcpy(char *,cchar *,int) noex ;
}


/* local typedefs */


/* exported variables */


/* exported subroutines */

char *strnncpy(char *d,cchar *sp,int sl,int sz) noex {
	char		*rp = d ;
	if (sz >= 0) {
	    if (sl >= 0) {
	        cint	ml = min(sl,sz) ;
	        strncpy(d,sp,ml) ;
	        if (ml < sz) {
	            memset((d + ml),0,(sz - ml)) ;
	        }
	    } else {
	        strncpy(d,sp,sz) ;
	    }
	    rp = (d + sz) ;
	} else {
	    strncpy(d,sp,sl) ;
	    rp = (d + sl) ;
	}
	return rp ;
}
/* end subroutine (strnncpy) */


