/* uc_ftime SUPPORT */
/* lang=C++20 */

/* interface component for UNIX® library-3c */
/* cleaned up version of 'ftime(3c)' */


/* revision history:

	= 2000-05-14, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2000 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This was originally supposed to implement the (so-called)
	|ftime(3c)| subroutine. But that subroutine is *broken* by
	design. Why is it broken?  It is broken because if always
	returns the standard time-zone offset from GMT in its
	'timezone' structure member. This is essentually useless
	in real life.  The only useful thing to return there is the
	*current* time-zone offset as indicated by the 'dstflag'
	member. So this is what we do instead of the original
	behavior.  We instead return the current time-zone offset
	in the 'timezone' variable, as indicated by the 'dstflag'
	member variable.

	Use of this subroutine (|uc_ftime(3dam)|) and its UNIX®
	inspired piece of crap (|ftime(3c)|) should be eliminated
	from all code as soon as time permits.

A 'timeb' structure looks like:

struct timeb {
	time_t		time ; 		// time, seconds since the epoch
	unsigned short 	millitm ;	// 1000 msec of additional accuracy
	short		timezone ;	// timezone, minutes west of GMT
	short		dstflag ;	// DST flag
} ;

	Note:
	Only Apple Darwin still fully supports this subroutine.
	Solaris® supports it only partially (not filling in the
	POSIX® optional fields).  Linux does not (no longer) supports
	this subroutine at all.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/timeb.h>		/* <- the money shot */
#include	<unistd.h>
#include	<ctime>
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<usysflag.h>
#include	<tmtime.h>


/* local defines */


/* external subroutines */


/* external variables */


/* forward references */


/* local variables */

constexpr bool		f_darwin = F_DARWIN ;


/* exported variables */


/* exported subroutines */

int uc_ftime(TIMEB *tbp) noex {
	int		rs = SR_FAULT ;
	if (tbp) {
	    if_constexpr (f_darwin) {
		ftime(tbp) ;
		rs = SR_OK ;
	    } else {
	        TIMEVAL		tv ;
	        memclear(tbp) ;
	        if ((rs = uc_gettimeofday(&tv,nullptr)) >= 0) {
	            TMTIME	tmt ;
	            tbp->time = tv.tv_sec ;
	            tbp->millitm = (tv.tv_usec / 1000) ;
	            if ((rs = tmtime_localtime(&tmt,tbp->time)) >= 0) {
	                tbp->timezone = (tmt.gmtoff / 60) ;
	                tbp->dstflag = tmt.isdst ;
	                rs = (tmt.isdst > 0) ? 1 : 0 ;
	            } /* end if (tmtime_localtime) */
	        } /* end if (uc_gettimeofday) */
	    } /* end if_constexpr (f_darwin) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (uc_ftime) */


