/* uctimer SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* interface component for UNIX® library-3c */
/* per-process timer management */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-13, David A-D- Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A-D- Morano.  All rights reserved. */

/*******************************************************************************

  	Group:
	uc_timercreate
	uc_timerdestroy
	uc_timerset
	uc_timerget
	uc_timerover

	Description:
	POSIX® timer (|timer(3rt)|) operations.

	Synopsis:
	int uc_timercreate() noex
	int uc_timerdestroy() noex
	int uc_timerset() noex
	int uc_timerget() noex
	int uc_timerover() noex

	Arguments:

	Returns:
	>=0		ok
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<unistd.h>
#include	<cerrno>
#include	<ctime>			/* |{x}itimer(3c)| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<numeric>		/* |cast_saturate(3c++)| */
#include	<clanguage.h>
#include	<usysbase.h>
#include	<usupport.h>
#include	<utimeout.h>
#include	<errtimer.hh>
#include	<localmisc.h>

#include	"uctimer.h"


/* local defines */

#define	ITS	itimerspec
#define	CITS	const itimerspec


/* imported namespaces */

using std::cast_saturate ;		/* subroutine */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct uctimer ;
    typedef int (uctimer::*mem_m)(timer_t) noex ;
    struct uctimer {
	mem_m		m ;
	sigevent	*sep ;
	timer_t		*tmp ;
	ITS		*otvp ;
	const ITS	*ntvp ;
	clockid_t	cid ;
	int		tf ;
	uctimer() noex {} ;
	uctimer(clockid_t c,sigevent *sp,timer_t *tp) noex : tmp(tp) {
	    cid = c ;
	    sep = sp ;
	} ;
	uctimer(int f,ITS *o,const ITS *n) noex : tf(f) {
	    otvp = o ;
	    ntvp = n ;
	} ;
	uctimer(ITS *o) noex : otvp(o) { } ;
	int operator () (timer_t) noex ;
	int create(timer_t) noex ;
	int destroy(timer_t) noex ;
	int set(timer_t) noex ;
	int get(timer_t) noex ;
	int over(timer_t) noex ;
    } ; /* end struct (uctimer) */
} /* end namespace */


/* forward references */


/* local variables */

constexpr timer_t	tnull{} ;


/* exported variables */


/* exported subroutines */

int uc_timercreate(clockid_t cid,sigevent *sep,timer_t *tmp) noex {
	int		rs = SR_FAULT ;
	if (tmp) {
	    uctimer	uco(cid,sep,tmp) ;
	    uco.m = &uctimer::create ;
	    rs = uco(tnull) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (uc_timercreate) */

int uc_timerdestroy(timer_t tid) noex {
	uctimer		uco ;
	uco.m = &uctimer::destroy ;
	return uco(tid) ;
}
/* end subroutine (uc_timerdestroy) */

int uc_timerset(timer_t tid,int tf,CITS *ntvp,ITS *otvp) noex {
	uctimer		uco(tf,otvp,ntvp) ;
	uco.m = &uctimer::set ;
	return uco(tid) ;
}
/* end method (uctimer::set) */

int uc_timerget(timer_t tid,ITS *otvp) noex {
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
	errtimer	to_again	= utimeout[uto_again] ;
	errtimer        to_busy         = utimeout[uto_busy] ;
	errtimer        to_nomem        = utimeout[uto_nomem] ;
	reterr		r ;
	int		rs ;
	repeat {
	    if ((rs = (this->*m)(tid)) < 0) {
		r(rs) ;
		switch (rs) {
		case SR_AGAIN:
		    r = to_again(rs) ;
		    break ;
		case SR_BUSY:
		    r = to_busy(rs) ;
		    break ;
		case SR_NOMEM:
		    r = to_nomem(rs) ;
		    break ;
		case SR_INTR:
		    r(false) ;
		    break ;
		} /* end switch */
		rs = r ;
	    } /* end if (error) */
	} until ((rs >= 0) || r.fexit) ;
	return rs ;
}
/* end subroutine (uctimer::operator) */

int uctimer::create(timer_t) noex {
	int		rs = SR_FAULT ;
	if (tmp) {
	    if ((rs = timer_create(cid,sep,tmp)) < 0) {
		rs = (- errno) ;
	    }
	}
	return rs ;
}
/* end method (uctimer::create) */

int uctimer::destroy(timer_t tid) noex {
	int		rs ;
	if ((rs = timer_delete(tid)) < 0) {
	    rs = (- errno) ;
	}
	return rs ;
}
/* end method (uctimer::destroy) */

int uctimer::set(timer_t tid) noex {
	int		rs = SR_FAULT ;
	if (ntvp) {
	    if ((rs = timer_settime(tid,tf,ntvp,otvp)) < 0) {
		rs = (- errno) ;
	    }
	}
	return rs ;
}
/* end method (uctimer::set) */

int uctimer::get(timer_t tid) noex {
	int		rs = SR_FAULT ;
	if (otvp) {
	    if ((rs = timer_gettime(tid,otvp)) < 0) {
		rs = (- errno) ;
	    }
	}
	return rs ;
}
/* end method (uctimer::get) */

int uctimer::over(timer_t tid) noex {
	int		rs ;
	if ((rs = timer_getoverrun(tid)) < 0) {
	    rs = (- errno) ;
	}
	return rs ;
}
/* end method (uctimer::over) */


