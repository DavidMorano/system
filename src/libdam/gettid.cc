/* gettid SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* get the current thread ID */
/* version %I% last-modified %G% */


/* revision history:

	= 2001-04-11, David A­D­ Morano
	This was originally written.

*/

/* Copyright © 2001 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	gettid

  	Description:
	Return the current Thread-ID (TID).


	Synopsis:
	int gettid(void) noex

	Arguments:
	-		N/A

	Returns:
	-		thread ID

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<sys/types.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<usystem.h>
#include	<upt.h>
#include	<localmisc.h>


/* local defines */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

pthread_t gettid() noex {
	pthread_t	tid ;
	uptself(&tid) ;
	return tid ;
}
/* end subroutine (gettid) */


