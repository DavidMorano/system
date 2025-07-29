/* uutimes SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* translation layer interface for UNIX® equivalents */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services (RNS).

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<sys/wait.h>
#include	<sys/time.h>
#include	<utime.h>
#include	<unistd.h>
#include	<poll.h>
#include	<cerrno>
#include	<ctime>
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

int u_utimes(cchar *fname,CTIMEVAL *tvp) noex {
	int		rs = SR_FAULT ;
	if (fname && tvp) {
	    rs = SR_OK ;
	    repeat {
	        if (utimes(fname,tvp) < 0) {
		    rs = (- errno) ;
		}
	    } until (rs != SR_INTR) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (u_utimes) */


