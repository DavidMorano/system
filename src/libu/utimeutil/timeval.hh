/* timeval HEADER (Time-Value) */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* time-value object methods */
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

#ifndef	TIMEVAL_INCLUDE
#define	TIMEVAL_INCLUDE
#ifdef	__cplusplus


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */


/* operations on TIMEVALs */
#ifndef	timerclear
#define timerclear(tvp)         (tvp)->tv_sec = (tvp)->tv_usec = 0
#endif

#ifndef	timerisset
#define timerisset(tvp)         ((tvp)->tv_sec || (tvp)->tv_usec)
#endif

#ifndef	timercmp
#define timercmp(tvp, uvp, cmp)                                         \
        (((tvp)->tv_sec == (uvp)->tv_sec) ?                             \
            ((tvp)->tv_usec cmp (uvp)->tv_usec) :                       \
            ((tvp)->tv_sec cmp (uvp)->tv_sec))
#endif

#ifndef	timeradd
#define timeradd(tvp, uvp, vvp)                                         \
        do {                                                            \
                (vvp)->tv_sec = (tvp)->tv_sec + (uvp)->tv_sec ;         \
                (vvp)->tv_usec = (tvp)->tv_usec + (uvp)->tv_usec ;      \
                if ((vvp)->tv_usec >= 1000000) {                        \
                        (vvp)->tv_sec++ ;                                \
                        (vvp)->tv_usec -= 1000000 ;                      \
                }                                                       \
        } while (0)
#endif /* timeradd */

#ifndef	timersub
#define timersub(tvp, uvp, vvp)                                         \
        do {                                                            \
                (vvp)->tv_sec = (tvp)->tv_sec - (uvp)->tv_sec ;         \
                (vvp)->tv_usec = (tvp)->tv_usec - (uvp)->tv_usec ;      \
                if ((vvp)->tv_usec < 0) {                               \
                        (vvp)->tv_sec-- ;                               \
                        (vvp)->tv_usec += 1000000 ;                     \
                }                                                       \
        } while (0)
#endif /* timersub */

/* more operations on TIMEVALs (I guess from a different origin) */
#ifndef	timevalclear
#define timevalclear(tvp)	((tvp)->tv_sec = (tvp)->tv_usec = 0)
#endif

#ifndef	timevalisset
#define timevalisset(tvp)	((tvp)->tv_sec || (tvp)->tv_usec)
#endif

#ifndef	timevalcmp
#define timevalcmp(l,r,cmp)   timercmp(l,r,cmp) /* FreeBsd */
#endif


EXTERNC_begin

extern int timeval_load	(mut TIMEVAL *,time_t,int) noex ;
extern int timeval_add	(mut TIMEVAL *,CTIMEVAL *,CTIMEVAL *) noex ;
extern int timeval_sub	(mut TIMEVAL *,CTIMEVAL *,CTIMEVAL *) noex ;

EXTERNC_end

extern timeval operator + (con timeval &,const timeval &) noex ;
extern timeval operator - (con timeval &,const timeval &) noex ;

extern timeval operator + (con timeval &,time_t) noex ;
extern timeval operator - (con timeval &,time_t) noex ;

inline bool operator == (con timeval &v1,con timeval &v2) noex {
    	return (v1.tv_sec == v2.tv_sec) && (v1.tv_usec == v2.tv_usec) ;
} /* end subroutine (timeval::operator) */

extern ordcmp_strong operator <=> (con timeval &,con timeval &) noex ;

inline bool operator == (con timeval &v1,con time_t t) noex {
    	return (v1.tv_sec == t) && (v1.tv_usec == 0) ;
} /* end subroutine (timeval::operator) */

extern ordcmp_strong operator <=> (con timeval &,con time_t) noex ;

inline bool operator == (con timeval &v1,con int t) noex {
    	return (v1.tv_sec == t) && (v1.tv_usec == 0) ;
} /* end subroutine (timeval::operator) */

extern ordcmp_strong operator <=> (con timeval &,con int) noex ;

struct timeval_t : timeval {
    	timeval_t(time_t ªsec = 0,suseconds_t ªusec = 0) noex {
	    tv_sec	= ªsec ;
	    tv_usec	= ªusec ;
	} ; /* end ctor */
    	timeval_t(const timeval &o) noex {
	    tv_sec	= o.tv_sec ;
	    tv_usec	= o.tv_usec ;
	} ; /* end ctor */
    	timeval_t &operator = (const timeval &o) noex {
	    tv_sec	= o.tv_sec ;
	    tv_usec	= o.tv_usec ;
	    return *this ;
	} ; /* end ctor */
    	timeval_t(const timeval_t &o) noex {
	    tv_sec	= o.tv_sec ;
	    tv_usec	= o.tv_usec ;
	} ; /* end ctor */
    	timeval_t &operator = (const timeval_t &o) noex {
	    tv_sec	= o.tv_sec ;
	    tv_usec	= o.tv_usec ;
	    return *this ;
	} ; /* end ctor */
    	timeval_t &operator = (int sec) noex {
	    tv_sec = sec ;
	    tv_usec = 0 ;
	    return *this ;
	} ; /* end ctor */
	timeval_t &operator += (int v) noex {
	    tv_sec += v ;
	    return *this ;
	} ; /* end ctor */
	timeval_t &operator *= (int v) noex ;
	timeval_t &operator += (const timeval_t &) noex ;
	timeval_t &operator -= (const timeval_t &) noex ;
	bool operator == (const timeval_t &) const ;
	std::ordering_strong operator <=> (const timeval_t &) const ;
	bool operator == (const time_t)  const ;
	std::ordering_strong operator <=> (const time_t) const ;
} ; /* end struct (timeval_t) */


#endif /* __cplusplus */
#endif /* TIMEVAL_INCLUDE */


