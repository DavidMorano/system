/* upt INCLUDE */
/* lang=C20 */

/* UNIX® POSIX Thread manipulation */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-08-20, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	UPT_INCLUDE
#define	UPT_INCLUDE


#include	<envstandards.h>
#include	<sys/types.h>
#include	<pthread.h>
#include	<usystem.h>
#include	<pta.h>
#include	<localmisc.h>


#ifdef	__cplusplus
extern "C" {
#endif

typedef int (*upt_f)(void *) noex ;
typedef void (*uptvoid_f)(void *) noex ;

extern int uptcreate(pthread_t *,pthread_attr_t *,upt_f,void *) noex ;
extern int uptexit(int) noex ;
extern int uptonce(pthread_once_t *,uptvoid_f) noex ;
extern int uptjoin(pthread_t,int *) noex ;
extern int uptdetach(pthread_t) noex ;
extern int uptcancel(pthread_t) noex ;
extern int uptkill(pthread_t,int) noex ;
extern int uptsetschedparam(pthread_t,int,struct sched_param *) noex ;
extern int uptgetschedparam(pthread_t,int *,struct sched_param *) noex ;
extern int uptgetconcurrency() noex ;
extern int uptsetconcurrency(int) noex ;
extern int uptsetcancelstate(int,int *) noex ;
extern int uptsetcanceltype(int,int *) noex ;
extern int upttestcancel() noex ;
extern int uptequal(pthread_t,pthread_t) noex ;
extern int uptself(pthread_t *) noex ;
extern int uptatfork(uptvoid_f,uptvoid_f,uptvoid_f) noex ;
extern int uptncpus(int) ;

#ifdef	__cplusplus
}
#endif


#endif /* UPT_INCLUDE */


