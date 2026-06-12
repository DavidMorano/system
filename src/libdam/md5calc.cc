/* md5calc SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* calculate a MD5 digest */
/* version %I% last-modified %G% */


/* revision history:

	= 2009-01-20, David A­D­ Morano
	This is new code.

*/

/* Copyright © 2009 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	md5calc

	Description:
	This subroutine is used to form a digest of some data using
	the MD5 hash algorithm.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<endian.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */
#include	<md5.h>			/* LIBDAM */

#include	"md5caclc.h"


/* local defines */


/* local namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int md5calc(ulong *rp,cchar *sp,int sl) noex {
    	int		rs = SR_FAULT ;
	if (rp && sp) {
	    ulong	hv = 0 ;
	    uchar	out[16] ;
	    md5_calc(out,(uchar *) sp,(uint) sl) ;
	    for (int i = 0 ; i < 8 ; i += 1) {
	        ulong v = out[i] ;
	        if (ENDIAN) {
	            hv = ((hv << 8) | v) ;
	        } else {
	            v = (v << (i * 8)) ;
	            hv |= v ;
	        } /* end if */
	    } /* end for */
	    *rp = hv ;
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (md5calc) */


