/* ucchmodsuid */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* interface component for UNIX® library-3c */
/* set or clear the SUID bit on the file permissions mode */

#define	CF_DEBUGS	0		/* compile-time debugging */

/* revision history:

	= 1998-04-13, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/uio.h>
#include	<sys/stat.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<poll.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>		/* |getenv(3c)| */
#include	<clanguage.h>
#include	<usysbase.h>
#include	<usyscalls.h>
#include	<localmisc.h>


/* local defines */

#ifndef	S_IXSUID
#define	S_IXSUID	(S_ISUID | S_IXUSR)
#endif


/* forward references */


/* exported subroutines */

int uc_chmodsuid(cchar *fname,int f) noex {
	int		rs = SR_FAULT ;
	int		fprev = false ;
	if (fname) {
	    rs = SR_INVALID ;
	    if (fname[0]) {
	        if (ustat sb ; (rs = u_stat(fname,&sb)) >= 0) {
	            mode_t mperm = sb.st_mode ;
	            fprev = ((mperm & S_IXSUID) == S_IXSUID) ;
	            if (! LEQUIV(fprev,f)) {
	                if (f) {
	                    mperm |= S_IXSUID ;
	                } else {
	                    mperm &= (~ S_ISUID) ;
		        }
	                rs = u_chmod(fname,mperm) ;
	            } /* end if (needed a change) */
	        } /* end if (stat) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? fprev : rs ;
}
/* end subroutine (uc_chmodsuid) */


