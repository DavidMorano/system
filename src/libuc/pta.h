/* pta INCLUDE */
/* lang=C20 */

/* POSIX Thread Attribute manipulation */
/* version %I% last-modified %G% */


/* revision history:

	= 1999-07-28, David A-D- Morano
	Originally written for Rightcore Network Services.

*/
/* Copyright © 1999 David A­D­ Morano.  All rights reserved. */

#ifndef	PTA_INCLUDE
#define	PTA_INCLUDE


#include	<envstandards.h>
#include	<sys/types.h>
#include	<sys/param.h>
#include	<pthread.h>
#include	<clanguage.h>


#define	PTA	pthread_attr_t


#ifndef	TYPEDEF_PTA
#define	TYPEDEF_PTA
typedef PTA	pta ;
#endif

#ifdef	__cplusplus
extern "C" {
#endif

extern int	pta_create(PTA *) noex ;
extern int	pta_destroy(PTA *) noex ;
extern int	pta_setstacksize(PTA *,size_t) noex ;
extern int	pta_getstacksize(PTA *,size_t *) noex ;
extern int	pta_setguardsize(PTA *,size_t) noex ;
extern int	pta_getguardsize(PTA *,size_t *) noex ;
extern int	pta_setstackaddr(PTA *,void *) noex ;
extern int	pta_getstackaddr(PTA *,void **) noex ;
extern int	pta_setdetachstate(PTA *,int) noex ;
extern int	pta_getdetachstate(PTA *,int *) noex ;
extern int	pta_setscope(PTA *,int) noex ;
extern int	pta_getscope(PTA *,int *) noex ;
extern int	pta_setinheritsched(PTA *,int) noex ;
extern int	pta_getinheritsched(PTA *,int *) noex ;
extern int	pta_setschedpolicy(PTA *,int) noex ;
extern int	pta_getschedpolicy(PTA *,int *) noex ;
extern int	pta_setschedparam(PTA *,const struct sched_param *) noex ;
extern int	pta_getschedparam(PTA *,struct sched_param *) noex ;
extern int	pta_setstack(PTA *,void *,size_t) noex ;

#ifdef	__cplusplus
}
#endif


#endif /* PTA_INCLUDE */


