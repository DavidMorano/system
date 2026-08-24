/* ucinitgroups SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* interface component for UNIX® library-3c */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-13, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* POSIX® */
#include	<cerrno>		/* CSTD */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */


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

int uc_initgroups(cchar *name,gid_t gid) noex {
	int		rs = SR_FAULT ;
	if (name) {
	    rs = SR_INVALID ;
	    if (name[0]) {
		rs = SR_OK ;
	        if (initgroups(name,gid) < 0) {
		    rs = (neg errno) ;
		}
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (uc_initgroups) */


