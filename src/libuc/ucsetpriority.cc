/* ucsetpriority */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* interface component for UNIX® library-3c */
/* set a process priority (old style) */

#define	CF_DEBUGS	0

/* revision history:

	= 1998-04-13, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This subroutine sets a process priority (the old style
	priority from the beginning days).

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/stat.h>
#include	<sys/resource.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<cerrno>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>		/* |getenv(3c)| */
#include	<clanguage.h>
#include	<usysbase.h>
#include	<usyscalls.h>
#include	<localmisc.h>


/* local defines */


/* exported variables */


/* exported subroutines */

int uc_setpriority(int which,id_t who,int prio) noex {
	int	rs = SR_OK ;
	errno = 0 ;
	if ((rs = setpriority(which,who,prio)) < 0) {
	    if (errno != 0) rs = (- errno) ;
	}
	return rs ;
}
/* end subroutine (uc_setpriority) */


