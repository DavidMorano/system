/* uutime SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* translation layer interface for UNIX® equivalents */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services
	(RNS).

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This system call (|utime(2)|) is the older obsolete form
	of the newer |utimes(2)| call.  On some (newer) operating
	systems (Apple Darwin, or others), this call is emulated
	with the newer |utimes(2)| call.

	Notes:
	1. The fields of the UTIMBUF object are:
		actime 		- access time in seconds
		modtime 	- modification time in seconds
	2. In the Apple-Darwin operating system, this system-call,
	it is supported (both its include file and the call itself)
	but as a library call rather than a system-call.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<sys/types.h>
#include	<utime.h>		/* <- the money shot */
#include	<cerrno>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysrets.h>
#include	<localmisc.h>

#include	"usyscalls.h"


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

int u_utime(cchar *fname,CUTIMBUF *utp) noex {
	int		rs = SR_FAULT ;
	if (fname && utp) {
	    rs = SR_OK ;
	    repeat {
	        if (utime(fname,utp) < 0) {
		    rs = (- errno) ;
		}
	    } until (rs != SR_INTR) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (u_utime) */


