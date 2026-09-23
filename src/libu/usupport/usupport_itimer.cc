/* usupport_timer SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* UNIX® kernel support subroutines */
/* version %I% last-modified %G% */

#define	CF_DEBUG	0		/* debugging */

/* revision history:

	= 1998-03-26, David A­D­ Morano
	This was first written to give a little bit to UNIX® what
	we have in our own circuit-pack OSes!

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	msleep

	Description:
	This subroutine sleeps for some number of milliseconds.

	Synopsis:
	int msleep(int msec) noex

	Arguments:
	msec		number of millisecond to sleep

	Returns:
	>=0		amount of data returned
	<0		error (system-return)


	Name:
	umtime

	Description:
	This is sort of like |time(2)| but returns milliseconds
	rather than seconds.  Unlike |time(2)|, this subroutine
	takes no arguments.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<unistd.h>		/* POSIX® */
#include	<ctime>			/* CSTD */
#include	<cerrno>		/* CSTD */
#include	<climits>		/* CSTD |INT_MAX| */
#include	<cstddef>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<stdintx.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */
#include	<dprint.hh>		/* LIBU |DPRINTF(3u)| */

#include	"usupport_itimer.hh"


/* local defines */

#ifndef	CF_DEBUG
#define	CF_DEBUG	0		/* debugging */
#endif


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */

cbool			f_debug		= CF_DEBUG ;


/* exported variables */


/* exported subroutines */

namespace libu {
    sysret_t uitimer_get(int w,ITIMERVAL *otvp) noex {
	int		rs = SR_FAULT ;
	if (otvp) ylikely {
	    rs = SR_INVALID ;
	    if (w >= 0) ylikely {
	        repeat {
	        if ((rs = getitimer(w,otvp)) < 0) {
		        rs = (neg errno) ;
	            }
	        } until (rs != SR_INTR) ;
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
    } /* end subroutine (uitimer_get) */
    sysret_t uitimer_set(int w,CITIMERVAL *ntvp,ITIMERVAL *otvp) noex {
	int		rs = SR_FAULT ;
	DPRINTF("ent w=%d\n",w) ;
	if (ntvp) ylikely {
	    rs = SR_INVALID ;
	    if (w >= 0) ylikely {
	        repeat {
	            if ((rs = setitimer(w,ntvp,otvp)) < 0) {
		        rs = (neg errno) ;
	            }
	        } until (rs != SR_INTR) ;
	    } /* end if (valid) */
	} /* end if (non-null) */
	DPRINTF("ret rs=%d\n",rs) ;
	return rs ;
    } /* end subroutine (uitimer_set) */
} /* end namespace (libu) */


