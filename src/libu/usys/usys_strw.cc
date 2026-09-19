/* usys_strw SUPPORT */
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
#include	<usysrets.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

#include	"usys_strw.hh"


namespace usys {
    char *strwcpy(char *dp,cchar *sp,int sl) noex {
    	if (dp && sp) ylikely {
	    if (sl >= 0) {
	        while (sl-- && *sp) *dp++ = *sp++ ;
	        *dp = '\0' ;
	    } else {
	        dp = stpcpy(dp,sp) ;
	    } /* end if */
	} else {
	    dp = nullptr ;
	} /* end if (non-null) */
	return dp ;
    } /* end subroutine (strwcpy) */
} /* end namespace (usys) */


