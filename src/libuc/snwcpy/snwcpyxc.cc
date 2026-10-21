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
#include	<climits>		/* CSTD |INT_MAX| */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<toxc.h>		/* LIBUC |chtoxc(3uc)| */
#include	<localmisc.h>		/* LIBU */

#include	"snwcpyxc.h"


/* local defines */


/* imported namespaces */


/* local typedefs */

extern "C" {
    typedef char (*chtoxc_f)(int) noex ;
} /* extern (C) */


/* external subroutines */


/* external variables */


/* forward references */

local int snwcpyxc(chtoxc_f cvt,char *dbuf,int dlen,cchar *sp,int sl) noex {
    	int		rs = SR_FAULT ; /* return-value */
	if (dbuf && sp) ylikely {
	    int		i = 0 ; /* used-afterwards */
	    char	*dp = dbuf ;
    	    rs = SR_OVERFLOW ;
	    if (dlen < 0) dlen = INT_MAX ;
	    while ((i < dlen) && sl && sp[i]) {
	        *dp++ = cvt(sp[i++]) ;
	        sl -= 1 ;
	    } /* end while */
	    *dp = eos ;
	    if ((sl == 0) || (sp[i] == eos)) {
	        rs = intconv(dp - dbuf) ;
	    } /* end if (ok) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine-template (snwcpyxc) */


/* local variables */


/* exported variables */


/* exported subroutines */

int snwcpybc(char *dp,int dl,cchar *sp,int sl) noex {
	return snwcpyxc(chtobc,dp,dl,sp,sl) ;
} /* end */

int snwcpylc(char *dp,int dl,cchar *sp,int sl) noex {
	return snwcpyxc(chtolc,dp,dl,sp,sl) ;
} /* end */

int snwcpyuc(char *dp,int dl,cchar *sp,int sl) noex {
	return snwcpyxc(chtouc,dp,dl,sp,sl) ;
} /* end */

int snwcpyfc(char *dp,int dl,cchar *sp,int sl) noex {
	return snwcpyxc(chtofc,dp,dl,sp,sl) ;
} /* end */


