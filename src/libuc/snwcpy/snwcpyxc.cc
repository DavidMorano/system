/* snwcpyxc SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* copy a c-string to desination with a given case */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Group:
	snwcpyxc

	Description:
	This subroutine constructs a single string from a single
	specificed string.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<climits>		/* |INT_MAX| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<toxc.h>		/* including |chtoxc(3uc)| */
#include	<localmisc.h>

#include	"snwcpyxc.h"


/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* forward references */

template<char (*chtoxc)(int)>
int snwcpyxc(char *dbuf,int dlen,cchar *sp,int sl) noex {
    	int		rs = SR_OVERFLOW ; /* return-value */
	if (dbuf && sp) {
	    int		i = 0 ; /* used-afterwards */
	    char	*dp = dbuf ;
	    if (dlen < 0) dlen = INT_MAX ;
	    while ((i < dlen) && sl && sp[i]) {
	        *dp++ = chtoxc(sp[i++]) ;
	        sl -= 1 ;
	    }
	    *dp = '\0' ;
	    if ((sl == 0) || (sp[i] == '\0')) {
	        rs = intconv(dp - dbuf) ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine-template (snwcpyxc) */


/* local variables */


/* exported variables */


/* exported subroutines */

int snwcpybc(char *dp,int dl,cchar *sp,int sl) noex {
	return snwcpyxc<chtobc>(dp,dl,sp,sl) ;
}

int snwcpylc(char *dp,int dl,cchar *sp,int sl) noex {
	return snwcpyxc<chtolc>(dp,dl,sp,sl) ;
}

int snwcpyuc(char *dp,int dl,cchar *sp,int sl) noex {
	return snwcpyxc<chtouc>(dp,dl,sp,sl) ;
}

int snwcpyfc(char *dp,int dl,cchar *sp,int sl) noex {
	return snwcpyxc<chtofc>(dp,dl,sp,sl) ;
}


