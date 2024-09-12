/* usigmask SUPPORT */
/* lang=C++20 */

/* set a p-thread signal mask */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Yes, Virginia. We need this for p-thread work. We need it
	for the same reasons we need any other 'uc_xxx(3uc)'
	subroutine.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<sys/types.h>
#include	<pthread.h>
#include	<climits>
#include	<cerrno>
#include	<cstddef>		/* |nullptr_t| */
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysrets.h>
#include	<usyscalls.h>
#include	<clanguage.h>


/* local defines */


/* local variables */


/* exported variables */


/* exported subroutines */

int u_sigmask(int how,sigset_t *setp,sigset_t *osetp) noex {
	int		rs ;
	errno = 0 ;
	if ((rs = pthread_sigmask(how,setp,osetp)) > 0) {
	    rs = (- rs) ;
	} else if (rs < 0) {
	    rs = (- errno) ;
	}
	return rs ;
}
/* end subroutine (u_sigmask) */


