/* timeval HEADER */
/* lang=C20 */

/* time-value object methods */
/* version %I% last-modified %G% */


/* Copyright © 1998,2014 David A­D­ Morano.  All rights reserved. */

#ifndef	TIMEVAL_INCLUDE
#define	TIMEVAL_INCLUDE


#include	<envstandards.h>	/* first to configure */
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<clanguage.h>


EXTERNC_begin

extern int timeval_load(TIMEVAL *,time_t,int) noex ;
extern int timeval_add(TIMEVAL *,TIMEVAL *,TIMEVAL *) noex ;
extern int timeval_sub(TIMEVAL *,TIMEVAL *,TIMEVAL *) noex ;

EXTERNC_end


#endif /* TIMEVAL_INCLUDE */


