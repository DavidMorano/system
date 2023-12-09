/* timeval INCLUDE */
/* lang=C20 */

/* time-value object methods */


/* Copyright © 1998,2014 David A­D­ Morano.  All rights reserved. */

#ifndef	TIMEVAL_INCLUDE
#define	TIMEVAL_INCLUDE


#include	<envstandards.h>
#include	<sys/types.h>
#include	<time.h>


#ifndef	TIMEVAL
#define	TIMEVAL		struct timeval
#endif


#ifdef	__cplusplus
extern "C" {
#endif

extern int timeval_load(TIMEVAL *,time_t,int) ;
extern int timeval_add(TIMEVAL *,TIMEVAL *,TIMEVAL *) ;
extern int timeval_sub(TIMEVAL *,TIMEVAL *,TIMEVAL *) ;

#ifdef	__cplusplus
}
#endif


#endif /* TIMEVAL_INCLUDE */


