/* utimezone_support SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* time-zone information retriebal */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-13, David A-D- Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */
/* Use is subject to license terms. */

/*******************************************************************************

  	Names:
	uftime
	ugettimeofday

	Description:
	These subroutines retrieve information about the current
	time-zone for the current process.  This retrieved inforamtion
	is constant over the lifetime of the process.

	Synopsis:
	int libu::uftime(timezone *tzp) noex
	int libu::ugettimeofday(timeval *tvp,TIMEZONE *tzp) noex

	Arguments:
	tzp		|timezone| object pointer
	rbuf		result buffer pointer
	rlen		result buffer length

	Returns:
	>=0		result buffer returned number of bytes
	<0		error (system-return) 

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<ctime>			/* CSTD */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<new>			/* C++STD placement-new */
#include	<chrono>		/* C++STD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<umem.hh>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

#include	"utimezone.h"

import utimezones ;

/* local defines */


/* local namespaces */

using libu::umem ;			/* variable */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct syscaller ;
    typedef int (syscaller::*syscaller_m)() noex ;
    struct syscaller : utimezonestd {
	syscaller_m	m = nullptr ;
	TIMEB		*tbp ;
	TIMEVAL		*tvp ;
	TIMEZONE	*tzp ;
	syscaller(TIMEB *b,TIMEVAL *v,TIMEZONE *z) noex : tbp(b) {
	    tvp = v ;
	    tzp = z ;
	} ; /* end ctor */
	syscaller(TIMEB *b) noex : syscaller(b,nullptr,nullptr) { } ;
	syscaller(TIMEVAL *v,TIMEZONE *z) noex : syscaller(nullptr,v,z) { } ;
        int callstd() noex override {
            int         rs = SR_BUGCHECK ;
            if (m) {
                rs = (this->*m)() ;
            }
            return rs ;
        } ; /* end method */
	sysret_t std_ftime() noex ;
	sysret_t std_gettimeofday() noex ;
    } ; /* end struct (syscaller) */
} /* end namespace */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

namespace libu {
    int uftime(timeb *tbp) noex {
    	int		rs = SR_FAULT ;
	if (tbp) ylikely {
            syscaller sc(tbp) ;
            sc.m = &syscaller::std_ftime ;
            rs = sc ;
	} /* end if (non-null) */
	return rs ;
    } /* end subroutine (uftime) */
    int ugettimeofday(timeval *tvp,TIMEZONE *tzp) noex {
	int		rs = SR_FAULT ;
	if (tvp) {
            syscaller sc(tvp,tzp) ;
            sc.m = &syscaller::std_gettimeofday ;
            rs = sc ;
	} /* end if (non-null) */
	return rs ;
    } /* end subroutine (ugettimeofday) */
} /* end namespace (libu) */


/* local subroutines */

sysret_t syscaller::std_ftime() noex {
    	int		rs ;
	errno = 0 ;
	if ((rs = ftime(tbp)) < 0) {
	    rs = (neg errno) ;
	} /* end */
	return rs ;
} /* end method (syscaller::std_ftime) */

sysret_t syscaller::std_gettimeofday() noex {
    	int		rs ;
	errno = 0 ;
	if ((rs = gettimeofday(tvp,tzp)) < 0) {
	    rs = (neg errno) ;
	} /* end */
	return rs ;
} /* end method (syscaller::std_gettimeoday) */


