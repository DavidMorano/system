/* nusers SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* retrieve the number of users (logged-in) on system */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-18, David A­D­ Morano
	This little subroutine was put together to get the current
	number of logged-in users without having to resort to an
	underlying API that may be different for different OSes.
	Other motivations were along the same lines as to why the
	subroutine 'ncpu(3dam)' was written.  Among these was
	thread-safeness.  We completely live in a multi-threaded
	world; but we are constantly buffeted by subroutines which
	still think that we live in a single-threaded world.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	nusers

	Description:
	I calculate the number of users logged into the system.
	Sadly (tragically) not all systems support the UTMPX
	interface!!  Can you believe it?  Why can not those systems
	that do not support UTMPX get their tragic butt-acts together?
	Of course, unless the TMPX object (or some other MT-safe
	API) is used in the implementation of this subroutine (which
	TMPX object is not even presently coded up here to be
	conditionally compiled), it is not MT-safe nor reentrant.

	Synopsis:
	int nusers() noex

	Arguments:
	-		no arguements

	Returns:
	>=0		number of logged in users on the system
	<0		error (system-return)

	Notes: 
	1. Why some subroutine like this was not provided by
	the UNIX® developers themselves is a mystery.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<utmpacc.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */

#include	"nusers.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported subroutines */

int nusers() noex {
	return utmpacc_users(0) ;
} /* end subroutine (nusers) */


