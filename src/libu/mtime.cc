/* mtime SUPPORT */
/* lang=C++20 */

/* Millisecond Time */
/* version %I% last-modified %G% */


/* revision history:

	= 2000-05-14, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2000 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Sort of like |time(2)| but returns milliseconds rather than
	seconds.  Unlike |time(2)|, this subroutine takes no
	arguments.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<clanguage.h>

#include	"mtime.h"


/* local defines */


/* external subroutines */


/* local variables */


/* exported varaibles */


/* exported subroutines */

mtime_t mtime(void) noex {
	TIMEVAL		tv ;
	mtime_t		t ;
	mtime_t		m = 0 ;
	if (gettimeofday(&tv,nullptr) >= 0) {
	    t = tv.tv_sec ;
	    m += (t*1000) ;
	    m += (tv.tv_usec / 1000) ;
	} else {
	    t = time(nullptr) ; /* good until year 2038! */
	    m += (t*1000) ;
	}
	return m ;
}
/* end subroutine (mtime) */


