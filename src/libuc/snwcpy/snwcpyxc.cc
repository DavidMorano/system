/* snwcpyxc SUPPORT */
/* lang=C++20 */

/* copy a c-string to desination with a given case */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This subroutine constructs a single string from a single
	specificed string.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<climits>		/* |INT_MAX| */
#include	<utypedefs.h>
#include	<usysrets.h>
#include	<toxc.h>
#include	<clanguage.h>

#include	"snwcpyxc.h"


/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* local subroutine-templates */

template<int (*toxc)(int)>
int snwcpyxc(char *dbuf,int dlen,cchar *sp,int sl) noex {
	int		i = 0 ;
	char		*dp = dbuf ;
	if (dlen < 0) dlen = INT_MAX ;
	while ((i < dlen) && sl && sp[i]) {
	    *dp++ = toxc(sp[i++]) ;
	    sl -= 1 ;
	}
	*dp = '\0' ;
	return ((sl == 0) || (sp[i] == '\0')) ? (dp - dbuf) : SR_OVERFLOW ;
}
/* end subroutine-template (snwcpyxc) */


/* exported variables */


/* exported subroutines */

int snwcpybc(char *dp,int dl,cchar *sp,int sl) noex {
	return snwcpyxc<tobc>(dp,dl,sp,sl) ;
}

int snwcpylc(char *dp,int dl,cchar *sp,int sl) noex {
	return snwcpyxc<tolc>(dp,dl,sp,sl) ;
}

int snwcpyuc(char *dp,int dl,cchar *sp,int sl) noex {
	return snwcpyxc<touc>(dp,dl,sp,sl) ;
}

int snwcpyfc(char *dp,int dl,cchar *sp,int sl) noex {
	return snwcpyxc<tofc>(dp,dl,sp,sl) ;
}


