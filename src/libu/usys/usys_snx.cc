/* usys_snx SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* miscelllaneous (SNX) operating system support */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-06-16, David A­D­ Morano
	This subroutine was written to get some commonality across
	our most used operating systems. 

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	sn{x}

	Description:
	These are string-copy operations.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* |getprogname(3c)| |execname(3c) */
#include	<cstring>		/* CSTD |strncpy(3c)| */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */
#include	<usysrets.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

#include	"usys_strw.hh"		/* |strwcpy(3usys)| */
#include	"usys_snx.hh"


namespace usys {
    int sncpy1(char *dp,int dl,cchar *sp) noex {
	int		rs = SR_FAULT ;
	int		rl = 0 ; /* return-value */
	if (dp && sp) ylikely {
	    rs = SR_INVALID ;
	    if (dl >= 0) ylikely {
		rs = SR_OK ;
		for (rl = 0 ; (rl < dl) && *sp ; rl += 1) {
		    *dp++ = *sp++ ;
		} /* end for */
		if ((rl == dl) && *sp) {
		    rs = SR_OVERFLOW ;
		}
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? rl : rs ;
    } /* end subroutine (sncpy1) */
} /* end namespace (usys) */

namespace usys {
    int snwcpy(char *dp,int dl,cchar *sp,int sl) noex {
	int		rs = SR_FAULT ;
	if (dp && sp) ylikely {
	    if (dl >= 0) {
	        if (sl >= 0) {
	            if (sl > dl) {
	                rs = sncpy1(dp,dl,sp) ;
	            } else {
	                rs = intconv(strwcpy(dp,sp,sl) - dp) ;
		    }
	        } else {
	            rs = sncpy1(dp,dl,sp) ;
	        }
	    } else {
	        rs = intconv(strwcpy(dp,sp,sl) - dp) ;
	    }
	} /* end if (non-null) */
	return rs ;
    } /* end subroutine (snwcpy) */
} /* end namespace (usys) */


