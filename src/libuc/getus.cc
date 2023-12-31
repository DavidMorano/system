/* getus SUPPORT */
/* lang=C20 */

/* get user-shell entries */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-06-16, David A�D� Morano
	This subroutine was originally written.

	= 2011-07-12, David A-D- Morano
	I updated this for new 'noexcept' w/ C++11

*/

/* Copyright � 1998,2011 David A�D� Morano.  All rights reserved. */

/*******************************************************************************

	These subroutines were was written to provide a more
	consistent interface to the system 'user-shells" database.

	These subroutines are multi-thread safe, because the
	underlying subroutines are themselves multi-thread safe.

	Although safe for multithreading, different threads using
	these subroutines at the same time will enumerate separate
	subsets of the accessed database.  This is the way the UNIX�
	gods seemed to want these sorts of subroutines to work (did
	they really have a choice given that these existed before
	multi-threading came into vogue?).

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<unistd.h>
#include	<usystem.h>
#include	<localmisc.h>

#include	"getus.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported subroutines */

int getus_begin() noex {
	return uc_setus() ;
}
/* end subroutine (getus_begin) */

int getus_end() noex {
	return uc_endus() ;
}
/* end subroutine (getus_end) */

int getus_ent(char *rbuf,int rlen) noex {
	return uc_getus(rbuf,rlen) ;
}
/* end subroutine (getus_ent) */


