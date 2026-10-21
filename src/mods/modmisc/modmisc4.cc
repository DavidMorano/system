/* modmisc4 MODULE */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* UNIX® Miscellaneous support */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-07-19, David A­D­ Morano
	This code was originally written.

	= 2020-05-07, David A-D- Morano
	I converted this (formerly a header-only file) to a module.

*/

/* Copyright © 1998,2020 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Module:
	modmisc

	Description:
	This module provides miscellaneous UNIX® or other
	common (library oriented) subroutines.

*******************************************************************************/

module ;

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */
#include	<usysrets.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

module modmisc ;

/* local defines */


/* local namespaces */

using modmisc_ns::strwcpy ;		/* subroutine */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

local inline bool hasmore(ccharp sp,int sl) noex {
    	return (sl && *sp) ;
} /* end subroutine */


/* local variables */


/* exported variables */


/* exported subroutines */

int snaddw(char *rbuf,int rlen,int i,cchar *sp,int sl) noex {
	int		rs = SR_FAULT ;
	int		len = 0 ;
	if (rbuf && sp) ylikely {
	    rs = SR_INVALID ;
 	    if (i >= 0) ylikely {	
	        charp bp = (rbuf + i) ;
	        rs = SR_OK ;
	        if (rlen >= 0) ylikely {
		    charp ep = (rbuf + rlen) ;
	            if (sl >= 0) {
	                while ((bp < ep) && hasmore(sp,sl)) {
	                    *bp++ = *sp++ ;
	                    sl -= 1 ;
	                } /* end while */
		        if ((bp == ep) && hasmore(sp,sl)) {
		            rs = SR_OVERFLOW ;
		        } /* end if (error) */
		    } else {
	                while ((bp < ep) && *sp) {
	                    *bp++ = *sp++ ;
		        } /* end while */
		        if ((bp == ep) && *sp) {
		            rs = SR_OVERFLOW ;
		        } /* end if (error) */
	            } /* end if */
	            *bp = eos ;
		} else {
		    bp = strwcpy(bp,sp,sl) ;
	        } /* end if */
		len = intconv(bp - (rbuf + i)) ;
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
} /* end subroutine (snaddw) */


