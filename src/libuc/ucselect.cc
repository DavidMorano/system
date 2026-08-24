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
#include	<sys/types.h>		/* POSIX® */
#include	<sys/time.h>		/* POSIX® */
#include	<unistd.h>		/* POSIX® */
#include	<cerrno>		/* CSTD */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */


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
	        rs = (neg errno) ;
	    }
	} /* end if (valid) */
	return rs ;
} /* end subroutine (uc_select) */


