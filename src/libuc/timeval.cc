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

#include	<envstandards.h>
#include	<ctime>
#include	<usystem.h>
#include	<localmisc.h>


/* local defines */

#ifndef	INTMILLION
#define	INTMILLION	1000000
#endif


/* exported variables */


/* exported subroutines */

int timeval_load(TIMEVAL *dst,time_t sec,int usec) noex {
	int		rs = SR_FAULT ;
	if (dst) {
	    while (usec >= INTMILLION) {
	        sec += 1 ;
	        usec -= INTMILLION ;
	    }
	    dst->tv_sec = sec ;
	    dst->tv_usec = usec ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (timeval_load) */

int timeval_add(TIMEVAL *dst,TIMEVAL *src1,TIMEVAL *src2) noex {
	int		rs = SR_FAULT ;
	if (dst && src1 && src2) {
	    dst->tv_sec = src1->tv_sec + src2->tv_sec ;
	    dst->tv_usec = src1->tv_usec + src2->tv_usec ;
	    if (dst->tv_usec >= 1000000) {
	        dst->tv_usec -= 1000000 ;
	        dst->tv_sec += 1 ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (timeval_add) */

int timeval_sub(TIMEVAL *dst,TIMEVAL *src1,TIMEVAL *src2) noex {
	int		rs = SR_FAULT ;
	if (dst && src1 && src2) {
	    dst->tv_sec = src1->tv_sec - src2->tv_sec ;
	    dst->tv_usec = src1->tv_usec - src2->tv_usec ;
	    if (dst->tv_usec < 0) {
	        dst->tv_usec += 1000000 ;
	        dst->tv_sec -= 1 ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (timeval_sub) */


