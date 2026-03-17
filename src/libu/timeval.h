/* timeval HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

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


#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/time.h>		/* |TIMEVAL| */
#include	<clanguage.h>
#include	<usysbase.h>


EXTERNC_begin

extern int timeval_load(TIMEVAL *,time_t,int) noex ;
extern int timeval_add(TIMEVAL *,CTIMEVAL *,CTIMEVAL *) noex ;
extern int timeval_sub(TIMEVAL *,CTIMEVAL *,CTIMEVAL *) noex ;

EXTERNC_end

#ifdef	__cplusplus

struct timeval_t : timeval {
	timeval_t &operator += (int v) noex {
	    tv_sec += v ;
	    return *this ;
	} ;
	timeval_t(time_t s,int us) noex {
	    tv_sec = s ;
	    tv_usec = us ;
	} ;
	timeval_t &operator += (const timeval_t &) noex ;
	timeval_t &operator -= (const timeval_t &) noex ;
} ; /* end struct (timeval_t) */

timeval operator + (const timeval &,const timeval &) noex ;
timeval operator - (const timeval &,const timeval &) noex ;

#endif /* __cplusplus */


#endif /* TIMEVAL_INCLUDE */


