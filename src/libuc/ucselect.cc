/* ucselect */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* interface component for UNIX® library-3c */
/* version %I% last-modified %G% */

#define	CF_DEBUGS	0		/* compile-time debugging */
#define	CF_INTR		0		/* do not return on an interrupt */

/* revision history:

	= 1998-04-13, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */


#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>
#include	<sys/time.h>
#include	<unistd.h>
#include	<cerrno>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>		/* |getenv(3c)| */
#include	<clanguage.h>
#include	<usysbase.h>
#include	<usyscalls.h>
#include	<localmisc.h>


/* local defines */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int uc_select(int nfds,fd_set *readfds,fd_set *writefds,
		fd_set *errorfds,TIMEVAL *tp) noex {
	int		rs = SR_INVALID ;
	if (readfds || writefds || errorfds) {
	    if ((rs = select(nfds,readfds,writefds,errorfds,tp)) < 0) {
	        rs = (- errno) ;
	    }
	} /* end if (valid) */
	return rs ;
}
/* end subroutine (uc_select) */


