/* uctimer */
/* lang=C++20 */

/* interface component for UNIX® library-3c */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-13, David A-D- Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A-D- Morano.  All rights reserved. */

/*******************************************************************************

	POSIX® timer (|timer(3rt)|) operations.

	System subroutines provided:
	
	uc_timercreate(3rt)
	uc_timerdestroy(3rt)
	uc_timerset(3rt)
	uc_timerget(3rt)
	uc_timerover(3rt)


*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<unistd.h>
#include	<cerrno>
#include	<cstdlib>
#include	<cstring>
#include	<ctime>
#include	<usystem.h>
#include	<utimeout.h>
#include	<usupport.h>
#include	<utypealiases.h>
#include	<localmisc.h>

#include	"uctimer.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */

struct uctimer ;

typedef int (uctimer::*mem_f)(timer_t) noex ;

struct uctimer {
	mem_f		m ;
	sigevent	*sep ;
	timer_t		*tmp ;
	itimerspec	*otvp ;
	itimerspec	*ntvp ;
	clockid_t	cid ;
	int		tf ;
	uctimer() noex {} ;
	uctimer(clockid_t c,sigevent *sp,timer_t *tp) noex : tmp(tp) {
	    cid = c ;
	    sep = sp ;
	} ;
	uctimer(int f,itimerspec *o,itimerspec *n) noex : tf(f) {
	    otvp = o ;
	    ntvp = n ;
	} ;
	uctimer(itimerspec *o) noex : otvp(o) { } ;
	int operator () (timer_t) noex ;
	int create(timer_t) noex ;
	int destroy(timer_t) noex ;
	int set(timer_t) noex ;
	int get(timer_t) noex ;
	int over(timer_t) noex ;
} ; /* end struct (uctimer) */


/* forward references */


/* local variables */

constexpr timer_t	tnull{} ;


/* exported subroutines */

int uc_timercreate(clockid_t cid,sigevent *sep,timer_t *tmp) noex {
	int		rs = SR_FAULT ;
	if (tmp) {
	    uctimer	uco(cid,sep,tmp) ;
	    uco.m = &uctimer::create ;
	    rs = uco(tnull) ;
	}
	return rs ;
}
/* end subroutine (uc_timercreate) */

int uc_timerdestroy(timer_t tid) noex {
	uctimer		uco ;
	uco.m = &uctimer::destroy ;
	return uco(tid) ;
}
/* end subroutine (uc_timerdestroy) */

int uc_timerset(timer_t tid,int tf,itimerspec *ntvp,itimerspec *otvp) noex {
	uctimer		uco(tf,otvp,ntvp) ;
	uco.m = &uctimer::set ;
	return uco(tid) ;
}
/* end method (uctimer::set) */

int uc_timerget(timer_t tid,itimerspec *otvp) noex {
	uctimer		uco(otvp) ;
	uco.m = &uctimer::get ;
	return uco(tid) ;
}
/* end method (uctimer::get) */

int uc_timerover(timer_t tid) noex {
	uctimer		uco ;
	uco.m = &uctimer::over ;
	return uco(tid) ;
}
/* end subroutine (uc_timerover) */


/* local subroutines */

int uctimer::operator () (timer_t tid) noex {
	int		to_again = utimeout[uto_again] ;
	int		rs ;
	bool		f_exit = false ;
	repeat {
	    if ((rs = (this->*m)(tid)) < 0) {
		switch (rs) {
		case SR_AGAIN:
		    if (to_again-- > 0) {
			msleep(1000) ;
		    } else {
			f_exit = true ;
		    }
		    break ;
		case SR_INTR:
		    break ;
		default:
		    f_exit = true ;
		    break ;
		} /* end switch */
	    } /* end if (error) */
	} until ((rs >= 0) || f_exit) ;
	return rs ;
}
/* end subroutine (uctimer::operator) */

int uctimer::create(timer_t) noex {
	int		rs = SR_FAULT ;
	if (tmp) {
	    if ((rs = timer_create(cid,sep,tmp)) < 0) rs = (- errno) ;
	}
	return rs ;
}
/* end method (uctimer::create) */

int uctimer::destroy(timer_t tid) noex {
	int		rs ;
	if ((rs = timer_delete(tid)) < 0) rs = (- errno) ;
	return rs ;
}
/* end method (uctimer::destroy) */

int uctimer::set(timer_t tid) noex {
	int		rs = SR_FAULT ;
	if (ntvp) {
	    if ((rs = timer_settime(tid,tf,ntvp,otvp)) < 0) rs = (- errno) ;
	}
	return rs ;
}
/* end method (uctimer::set) */

int uctimer::get(timer_t tid) noex {
	int		rs = SR_FAULT ;
	if (otvp) {
	    if ((rs = timer_gettime(tid,otvp)) < 0) rs = (- errno) ;
	}
	return rs ;
}
/* end method (uctimer::get) */

int uctimer::over(timer_t tid) noex {
	int		rs ;
	if ((rs = timer_getoverrun(tid)) < 0) rs = (- errno) ;
	return rs ;
}
/* end method (uctimer::over) */


