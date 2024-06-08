/* timeval SUPPORT */
/* lang=C++20 */

/* subroutines to manipulate TIMEVAL values */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

	= 2014-03-24, David A­D­ Morano
	I added the |init()| method.

*/

/* Copyright © 1998,2014 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	We manage (a little bit) the TIMEVAL object.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<ctime>			/* |TIMEVAL| */
#include	<usystem.h>
#include	<localmisc.h>

#include	"timeval.h"


/* local defines */

#ifndef	INTMILLION
#define	INTMILLION	1000000
#endif


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */

constexpr int		onemillion = INTMILLION ;


/* exported variables */


/* exported subroutines */

int timeval_load(TIMEVAL *dst,time_t sec,int usec) noex {
	int		rs = SR_FAULT ;
	if (dst) {
	    while (usec >= onemillion) {
	        sec += 1 ;
	        usec -= onemillion ;
	    }
	    dst->tv_sec = sec ;
	    dst->tv_usec = usec ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (timeval_load) */

int timeval_add(TIMEVAL *dst,CTIMEVAL *src1,CTIMEVAL *src2) noex {
	int		rs = SR_FAULT ;
	if (dst && src1 && src2) {
	    dst->tv_sec = src1->tv_sec + src2->tv_sec ;
	    dst->tv_usec = src1->tv_usec + src2->tv_usec ;
	    if (dst->tv_usec >= onemillion) {
	        dst->tv_usec -= onemillion ;
	        dst->tv_sec += 1 ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (timeval_add) */

int timeval_sub(TIMEVAL *dst,CTIMEVAL *src1,CTIMEVAL *src2) noex {
	int		rs = SR_FAULT ;
	if (dst && src1 && src2) {
	    dst->tv_sec = src1->tv_sec - src2->tv_sec ;
	    dst->tv_usec = src1->tv_usec - src2->tv_usec ;
	    if (dst->tv_usec < 0) {
	        dst->tv_usec += onemillion ;
	        dst->tv_sec -= 1 ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (timeval_sub) */

timeval operator + (const timeval &t1,const timeval &t2) noex {
	timeval		r ;
	timeval_add(&r,&t1,&t2) ;
	return r ;
}

timeval operator - (const timeval &t1,const timeval &t2) noex {
	timeval		r ;
	timeval_sub(&r,&t1,&t2) ;
	return r ;
}

timeval_t &timeval_t::operator += (const timeval_t &o) noex {
	suseconds_t	usec = (tv_usec + o.tv_usec) ;
	tv_sec += o.tv_sec ;
	if (usec >= onemillion) {
	    usec -= onemillion ;
	    tv_sec += 1 ;
	}
	tv_usec = usec ;
	return *this ;
}

timeval_t &timeval_t::operator -= (const timeval_t &o) noex {
	suseconds_t	usec = (tv_usec - o.tv_usec) ;
	tv_sec -= o.tv_sec ;
	if (usec < 0) {
	    usec += onemillion ;
	    tv_sec -= 1 ;
	}
	tv_usec = usec ;
	return *this ;
}


