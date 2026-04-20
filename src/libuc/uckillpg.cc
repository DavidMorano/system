/* uckillpg */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* interface component for UNIX® library-3c */
/* send a kill-signal to a process group */

#define	CF_DEBUGS	0		/* compile-time debugging */

/* revision history:

	= 1998-03-11, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Description:
	Send a signal to a process group.

	Synopsis:
	int uc_killpg(pid_t pgrp,int signo) noex

	Arguments:
	pgrp		program group ID
	signo		signal number

	Returns:
	>=0		OK
	<0		error (system-return

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<csignal>
#include	<cerrno>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>		/* |getenv(3c)| */
#include	<clanguage.h>
#include	<usysbase.h>
#include	<usyscalls.h>
#include	<localmisc.h>


/* local defines */


/* external subroutines */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int uc_killpg(pid_t pgrp,int signo) noex {
	int		rs ;
	repeat {
	    if ((rs = killpg(pgrp,signo)) < 0) rs = (- errno) ;
	} until (rs != SR_INTR) ;
	return rs ;
}
/* end subroutine (uc_killpg) */


