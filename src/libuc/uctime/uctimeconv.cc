/* uctimeconvx SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* interface component for UNIX® library-3c */
/* time-conversion functions */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-13, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Group:
	uctimeconv

	Names:
	uc_timelocal
	uc_timegm
	uc_timex
	uc_mktime

	Description:
	Time conversion subroutines.

	Synopsis:
	int uc_timelocal	(custime *dt,TM *tmp) noex
	int uc_timegm		(custime *dt,TM *tmp) noex
	int uc_timex		(custime *tp,TM *tsp,int z) noex
	int uc_mktime		(TM *tmp,time_t *rp) noex

	Arguments:
	dt	time to convert
	tmp	pointer to TM object
	z	??
	rp	result-pointer
	
	Returns:
	>=0	OK
	<0	error (system-return)

	Notes:
	1. POSIX® (or somebody) says that calls to |localtime_r(3c)|
	or |gmtime_r(3c)| (like what I am doing below) are not
	required to call the subroutine |tzset(3c)| before executing
	their own particular function.  So I call |tzset(3c)| muself
	before calling those other subroutines.  But this only has to be
	done once in the lifetime of a program.
	2. Of course, the |tzset(3c)| subroutine has its own mutual
	exclusion lock to prevent it repeaing its own extensive
	initialization (reading the zone-info file, et cetera) on
	repeated calls.  But I restrict multiple calls to that
	subroutine myself for ... what ... fun.
	3. Blah, blah, blah, blah, blag.  Yes, I am not using a real
	memory-barrier atomic lock below.  It is not a real problem.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<ctime>			/* CSTD */
#include	<cerrno>		/* CSTD */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cassert>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

#include	"uctimeconv.h"


/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct initer {
	bool		finit ;
	operator int () noex ;
    } ; /* end struct (initer) */
} /* end namespace */


/* forward references */


/* local variables */

static initer		init ;


/* exported variables */


/* exported subroutines */

int uc_timelocal(custime *tp,TM *tsp) noex {
	int		rs = SR_FAULT ;
	assert(tp && tsp) ;
	if (tp && tsp) ylikely {
	    if ((rs = init) >= 0) ylikely {
	        errno = 0 ;
	        if (syshas.localtimer) ylikely {
	            if (TM *rp = localtime_r(tp,tsp) ; (!rp)) {
	                rs = (neg errno) ;
		    }
	        } else {
		    rs = SR_NOSYS ;
	        } /* end if_constexpr (f_reentrant) */
	    } /* end if (init) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (uc_timelocal) */

int uc_timegm(custime *tp,TM *tsp) noex {
	int		rs = SR_FAULT ;
	assert(tp && tsp) ;
	if (tp && tsp) ylikely {
	    if ((rs = init) >= 0) ylikely {
	        errno = 0 ;
	        if (syshas.gmtimer) ylikely {
	            if (TM *rp = gmtime_r(tp,tsp) ; (!rp)) {
	                rs = (neg errno) ;
		    }
	        } else {
		    rs = SR_NOSYS ;
	        } /* end if_constexpr (f_reentrant) */
	    } /* end if (init) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (uc_timegm) */

int uc_timex(custime *tp,TM *tsp,int z) noex {
	int		rs ;
	assert(tp && tsp) ;
	if (z) {
	    rs = uc_timelocal(tp,tsp) ;
	} else {
	    rs = uc_timegm(tp,tsp) ;
	}
	return rs ;
} /* end subroutine (uc_timex) */

int uc_mktime(TM *tmp,time_t *rp) noex {
	int		rs = SR_FAULT ;
	assert(tmp && rp) ;
	if (tmp && rp) ylikely {
	    if ((rs = init) >= 0) ylikely {
	        time_t	res = 0 ; /* used-multiple */
	        errno = 0 ;
	        if ((res = mktime(tmp)) < 0) {
	            if (errno) rs = (neg errno) ;
	        } /* end if (error) */
	        if (rp) {
	            *rp = (rs >= 0) ? res : 0 ;
	        }
	    } /* end if (init) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (uc_mktime) */


/* local subroutine */

initer::operator int () noex {
    	int		rs = SR_OK ;
    	if (! finit) {
	    tzset() ;
	    finit = true ;
	} /* end if (initialization needed) */
	return (rs >= 0) ? int(finit) : rs ;
} /* end method (initer::operator) */


