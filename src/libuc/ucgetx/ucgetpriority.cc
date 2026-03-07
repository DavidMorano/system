/* ucgetpriority */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* interface component for UNIX® library-3c */
/* get a process priority (old style) */

#define	CF_DEBUGS	0		/* compile-time debugging */

/* revision history:

	= 1998-03-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This subroutine gets a process priority (the old style
	priority from the beginning days).


*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/resource.h>	/* 'getpriority(2)' */
#include	<unistd.h>
#include	<cerrno>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>		/* |getenv(3c)| */
#include	<clanguage.h>
#include	<usysbase.h>
#include	<usyscalls.h>
#include	<localmisc.h>

#include	"ucgetx.h"


/* local defines */


/* exported variables */


/* exported subroutines */

int uc_getpriority(int which,id_t who,int *rp) noex {
	int		rs = SR_INVALID ;
	if (which >= 0) {
	    int		prio ;
	    rs = SR_OK ;
	    errno = 0 ;
	    prio = getpriority(which,who) ;
	    if ((prio == -1) && (errno != 0)) {
	        rs = (- errno) ;
	    }
	    if (rp) {
	        *rp = prio ;
	    }
	} /* end if (valid) */
	return rs ;
}
/* end subroutine (uc_getpriority) */


