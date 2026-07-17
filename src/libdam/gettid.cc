/* gettid SUPPORT */
/* charset=ISO8859-1 */
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
	Return the current POSIX® Thread-ID (TID).

	Synopsis:
	pthread_t int gettid() noex

	Arguments:
	-		N/A

	Returns:
	-		thread ID

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<sys/types.h>		/* POSIX® */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<upt.h>			/* LIBU */
#include	<localmisc.h>		/* LIBU */

#include	"gettid.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

pthread_t gettid() noex {
	pthread_t	tid ;
	uptself(&tid) ;
	return tid ;
} /* end subroutine (gettid) */


