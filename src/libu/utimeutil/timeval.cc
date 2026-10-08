/* timeval SUPPORT (Time-Value) */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* subroutines to manipulate TIMEVAL values */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	timeval

	Description:
	We manage (a little bit) the TIMEVAL object.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<ctime>			/* CSTD |TIMEVAL| */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<compare>		/* C++STD */
#include	<numeric>		/* C++STD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

#include	"timeval.hh"


/* local defines */

#ifndef	INTMILLION
#define	INTMILLION	1000000		/* one million */
#endif


/* imported namespaces */


/* local typedefs */

using ord = std::ordering_strong ;	/* type */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */

constexpr int		onemillion = INTMILLION ;


/* exported variables */


/* exported subroutines */

int timeval_load(mut TIMEVAL *dst,time_t sec,int usec) noex {
	int		rs = SR_FAULT ;
	if (dst) ylikely {
	    rs = SR_OK ;
	    while (usec >= onemillion) {
	        sec += 1 ;
	        usec -= onemillion ;
	    } /* end while */
	    dst->tv_sec = sec ;
	    dst->tv_usec = usec ;
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (timeval_load) */

int timeval_add(mut TIMEVAL *dst,CTIMEVAL *src1,CTIMEVAL *src2) noex {
	int		rs = SR_FAULT ;
	if (dst && src1 && src2) ylikely {
	    rs = SR_OK ;
	    dst->tv_sec = src1->tv_sec + src2->tv_sec ;
	    dst->tv_usec = src1->tv_usec + src2->tv_usec ;
	    if (dst->tv_usec >= onemillion) {
	        dst->tv_usec -= onemillion ;
	        dst->tv_sec += 1 ;
	    } /* end */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (timeval_add) */

int timeval_sub(mut TIMEVAL *dst,CTIMEVAL *src1,CTIMEVAL *src2) noex {
	int		rs = SR_FAULT ;
	if (dst && src1 && src2) ylikely {
	    rs = SR_OK ;
	    dst->tv_sec = src1->tv_sec - src2->tv_sec ;
	    dst->tv_usec = src1->tv_usec - src2->tv_usec ;
	    if (dst->tv_usec < 0) {
	        dst->tv_usec += onemillion ;
	        dst->tv_sec -= 1 ;
	    } /* end */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (timeval_sub) */

timeval operator + (const timeval &t1,const timeval &t2) noex {
	timeval	r ;
	timeval_add(&r,&t1,&t2) ;
	return r ;
} /* end subroutine (timeval::operator) */

timeval operator - (const timeval &t1,const timeval &t2) noex {
	timeval	r ;
	timeval_sub(&r,&t1,&t2) ;
	return r ;
} /* end subroutine (timeval::operator) */

timeval operator + (const timeval &t1,con time_t t) noex {
	timeval	r = t1 ;
	r.tv_sec += t ;
	return r ;
} /* end subroutine (timeval::operator) */

timeval operator - (const timeval &t1,con time_t t) noex {
	timeval	r = t1 ;
	r.tv_sec -= t ;
	return r ;
} /* end subroutine (timeval::operator) */

ordcmp_strong operator <=> (con timeval &v1,con timeval &v2) noex {
    	ord res = ord::equal ;
	int rc = 0 ;
	if ((rc = intconv(v1.tv_sec - v2.tv_sec)) == 0L) {
	    rc = intconv(v1.tv_usec - v2.tv_usec) ;
	}
	if (rc) {
	    res = (rc > 0) ? ord::greater : ord::less ;
	}
	return res ;
} /* end subroutine (timeval::operator) */

ordcmp_strong operator <=> (con timeval &v1,con time_t t) noex {
    	ord res = ord::equal ;
	int rc = 0 ;
	if ((rc = intconv(v1.tv_sec - t)) == 0L) {
	    rc = intconv(v1.tv_usec - 0) ;
	}
	if (rc) {
	    res = (rc > 0) ? ord::greater : ord::less ;
	}
	return res ;
} /* end subroutine (timeval::operator) */

ordcmp_strong operator <=> (con timeval &v1,con int v) noex {
    	ord res = ord::equal ;
	int rc = 0 ;
	if ((rc = intconv(v1.tv_sec - v)) == 0L) {
	    rc = intconv(v1.tv_usec - 0) ;
	}
	if (rc) {
	    res = (rc > 0) ? ord::greater : ord::less ;
	}
	return res ;
} /* end subroutine (timeval::operator) */

timeval_t &timeval_t::operator *= (int m) noex {
    	const time_t		vs = m ;
	const suseconds_t	vu = m ;
	suseconds_t		usec ;
	tv_sec	= sat_mul(tv_sec,vs) ;
	usec	= sat_mul(tv_usec,vu) ;
	while (usec >= onemillion) {
	    usec -= onemillion ;
	    tv_sec += 1 ;
	} /* end while */
	tv_usec = usec ;
	return *this ;
} /* end method (timeval::operator) */

timeval_t &timeval_t::operator += (const timeval_t &o) noex {
	suseconds_t	usec = (tv_usec + o.tv_usec) ;
	tv_sec += o.tv_sec ;
	if (usec >= onemillion) {
	    usec -= onemillion ;
	    tv_sec += 1 ;
	} /* end */
	tv_usec = usec ;
	return *this ;
} /* end method (timeval::operator) */

timeval_t &timeval_t::operator -= (const timeval_t &o) noex {
	suseconds_t	usec = (tv_usec - o.tv_usec) ;
	tv_sec -= o.tv_sec ;
	if (usec < 0) {
	    usec += onemillion ;
	    tv_sec -= 1 ;
	} /* end */
	tv_usec = usec ;
	return *this ;
} /* end method (timeval::operator) */

bool timeval_t::operator == (const timeval_t &o) const {
	return (tv_sec == o.tv_sec) && (tv_usec == o.tv_usec) ;
} /* end method (timeval_t::operator) */

ord timeval_t::operator <=> (const timeval_t &o) const {
    	ord res = ord::equal ;
	int rc = 0 ;
	if ((rc = intconv(tv_sec - o.tv_sec)) == 0) {
	    rc = intconv(tv_usec - o.tv_usec) ;
	}
	if (rc) {
	    res = (rc > 0) ? ord::greater : ord::less ;
	}
	return res ;
} /* end smethod (timeval_t::operator) */

bool timeval_t::operator == (const time_t o) const {
	return (tv_sec == o) && (tv_usec == 0L) ;
} /* end method (timeval_t::operator) */

ord timeval_t::operator <=> (const time_t o) const {
    	ord res = ord::equal ;
	int rc = 0 ;
	if ((rc = intconv(tv_sec - o)) == 0L) {
	    rc = intconv(tv_usec - 0) ;
	}
	if (rc) {
	    res = (rc > 0) ? ord::greater : ord::less ;
	}
	return res ;
} /* end smethod (timeval_t::operator) */


