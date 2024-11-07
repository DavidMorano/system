/* getus SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* get user-shell entries */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-06-16, David A­D­ Morano
	This code was originally written.

	= 2011-07-12, David A-D- Morano
	I updated this for new 'noexcept' w/ C++11

*/

/* Copyright © 1998,2011 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Group:
	getus

	Description:
	These subroutines were was written to provide a more
	consistent interface to the system 'user-shells" database.
	These subroutines are multi-thread safe, because the
	underlying subroutines are themselves multi-thread safe.
	Although safe for multithreading, different threads using
	these subroutines at the same time will enumerate separate
	subsets of the accessed database.  This is the way the UNIX®
	gods seemed to want these sorts of subroutines to work (did
	they really have a choice given that these existed before
	multi-threading came into vogue?).

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<usystem.h>

#include	"getus.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int getus_begin() noex {
	return uc_getusbegin() ;
}
/* end subroutine (getus_begin) */

int getus_end() noex {
	return uc_getusend() ;
}
/* end subroutine (getus_end) */

int getus_ent(char *rbuf,int rlen) noex {
	return uc_getusent(rbuf,rlen) ;
}
/* end subroutine (getus_ent) */


