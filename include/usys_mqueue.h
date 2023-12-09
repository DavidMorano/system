/* usys_mqueue INCLUDE */
/* lang=C20 */

/* miscelllaneous (XXX) operating system support */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-21, David A­D­ Morano
	This module was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This file contains the UNIX system types that the brain-damaged
	MacOS operating system does NOT have.  We are trying in a very
	small way to make up for some of the immense brain-damage within
	the Apple Darwin operating system.


*******************************************************************************/

#ifndef	USYS_MQUEUE_INCLUDE
#define	USYS_MQUEUE_INCLUDE


#include	<envstandards.h>
#include	<sys/types.h>		/* <- |mode_t| */
#include	<sys/wait.h>		/* <- type |idtype_t| is there */
#include	<sys/time.h>		/* <- |TIMESPEC| is there */
#include	<signal.h>		/* <- |SIGEVENT| */
#include	<pthread.h>
#include	<time.h>
#include	<clanguage.h>


#ifndef	MQATTR
#define	MQATTR		struct mq_attr
#endif /* MQ_ATTR */


/* PMQ begin */
#if	(!defined(SYSHAS_PMQ)) || (SYSHAS_PMQ == 0)

#ifndef	STRUCT_MQATTR
#define	STRUCT_MQATTR

struct mq_attr {
	uint	dummy ;
} ;

#endif /* STRUCT_MQATTR */

#ifndef	TYPEDEF_MQDT
#define	TYPEDEF_MQDT

typedef uintptr_t	mqd_t ;

#endif /* TYPEDEF_MQDT */

#ifndef	SUBROUTINE_PMQ
#define	SUBROUTINE_PMQ
#ifdef	__cplusplus
extern "C" {
#endif

extern mqd_t mq_open(const char *,int,mode_t,const MQATTR *) noex ;
extern int mq_close(mqd_t) noex ;
extern int mq_send(mqd_t,const char *,int,int) noex ;
extern int mq_receive(mqd_t,char *,int,uint *) noex ;
extern int mq_setattr(mqd_t,const MQATTR *,MQATTR *) noex ;
extern int mq_getattr(mqd_t,MQATTR *) noex ;
extern int mq_notify(mqd_t,struct sigevent *) noex ;
extern int mq_unlink(const char *) noex ;

#ifdef	__cplusplus
}
#endif
#endif /* SUBROUTINE_PMQ */

#else /* (!defined(SYSHAS_PMQ)) || (SYSHAS_PMQ == 0) */

#include	<mqueue.h>

#endif /* (!defined(SYSHAS_PMQ)) || (SYSHAS_PMQ == 0) */
/* PMQ end */


#endif /* USYS_MQUEUE_INCLUDE */


