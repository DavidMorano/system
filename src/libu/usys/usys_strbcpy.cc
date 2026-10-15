/* usys_strbcpy SUPPORT */
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
#include	<cstdlib>		/* CSTD */
#include	<cstring>		/* CSTD |stpcpy(3c)| */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

#include	"usys_libstr.h"
#include	"usys_strbcpy.hh"


local inline bool hasgot(cchar *sp,int sl) noex {
    	return (sl || *sp) ;
} /* end subroutine (hasgot) */

local char *strbcpy_pos(char *dp,int dl,cchar *sp,int sl) noex {
	if (sl >= 0) {
	    while (dl && sl && *sp) {
		*dp++ = *sp++ ;
		sl -= 1 ;
		dl -= 1 ;
	    } /* end while */
	    *dp = '\0' ;
	    if ((dl == 0) && hasgot(sp,sl)) dp = nullptr ;
	} else {
	    while (dl && *sp) {
	        *dp++ = *sp++ ;
		dl -= 1 ;
	    } /* end while */
	    *dp = '\0' ;
	    if ((dl == 0) && *sp) dp = nullptr ;
	} /* end if */
	return dp ;
} /* end subroutine */

local char *strbcpy_neg(char *dp,int dl,cchar *sp,int sl) noex {
	(void) dl ;
	if (sl >= 0) {
	    while (sl-- && *sp) {
	        *dp++ = *sp++ ;
	    } /* end while */
	    *dp = '\0' ;
	} else {
	    dp = stpcpy(dp,sp) ;
	} /* end if */
    	return dp ;
} /* end subroutine */

namespace usys {
    char *strbcpy(char *dp,int dl,cchar *sp,int sl) noex {
	char *rp = nullptr ;
    	if (dp && sp) ylikely {
	    if (dl >= 0) {
		rp = strbcpy_pos(dp,dl,sp,sl) ;
	    } else {
		rp = strbcpy_neg(dp,dl,sp,sl) ;
	    } /* end if */
	} /* end if (non-null) */
	return rp ;
    } /* end subroutine (strbcpy) */
} /* end namespace (usys) */


