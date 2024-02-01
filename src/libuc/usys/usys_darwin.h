/* usys_darwin HEADER */
/* lang=C20 */

/* Apple Darwin operating system support */
/* version %I% last-modified %G% */

#define	CF_PSEMADAPT	1		/* 1=adapt, 0=no-adapt */

/* revision history:

	= 1998-03-21, David A­D­ Morano
	This module was originally written.

	= 2017-08-01, David A­D­ Morano
	Updated for lack of interfaces in MacOS Darwin

*/

/* Copyright © 1998,2017 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This file contains the UNIX® system types that the brain-damaged
	MacOS operating system does NOT have.  We are trying in a very
	small way to make up for some of the immense brain-damage within
	the Apple Darwin operating system.


*******************************************************************************/

#ifndef	USYS_DARWIN_INCLUDE
#define	USYS_DARWIN_INCLUDE


#include	<envstandards.h>

/* USYS_DARWIN start */
#if	defined(OSNAME_Darwin) && (OSNAME_Darwin > 0)

#include	<sys/types.h>
#include	<sys/wait.h>		/* <- type |idtype_t| is there */
#include	<sys/time.h>		/* <- |TIMESPEC| is there */
#include	<signal.h>
#include	<pthread.h>
#include	<semaphore.h>
#include	<time.h>
#include	<clanguage.h>


#ifndef	SIGEVENT
#define	SIGEVENT	struct sigevent
#endif

#ifndef	SIGVAL
#define	SIGVAL		union sigval
#endif

#ifndef	CTIMESPEC
#define	CTIMESPEC	const struct timespec
#endif

#ifndef	CSIGVAL
#define	CSIGVAL		const union sigval
#endif


/*----------------------------------------------------------------------------*/
/* SIGSEND begin */
#if	(!defined(SYSHAS_SIGSEND)) || (SYSHAS_SIGSEND == 0)

#ifndef	TYPEDEF_IDOP
#define	TYPEDEF_IDOP
typedef int		idop_t ;
#endif

struct procset {
	idop_t		p_op ;
	idtype_t	p_lidtype ;
	id_t		p_lid ;
	idtype_t	p_ridtype ;
	id_t		p_rid ;
} ;

#ifndef	TYPEDEF_PROCSET
#define	TYPEDEF_PROCSET
typedef struct procset		procset_t ;
#endif

EXTERNC_begin

extern int sigsend(idtype_t,id_t,int) noex ;
extern int sigsendset(procset_t *,int) noex ;
extern int sigqueue(pid_t,int,const union sigval) noex ;

EXTERNC_end

#endif /* (!defined(SYSHAS_SIGSEND)) || (SYSHAS_SIGSEND == 0) */
/* SIGSEND end */
/*----------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------*/
/* SIGWAIT begin */
#if	(!defined(SYSHAS_SIGWAIT)) || (SYSHAS_SIGWAIT == 0)

EXTERNC_begin

extern int sigwait(const sigset_t *,int *) noex ;
extern int sigwaitinfo(const sigset_t *,siginfo_t *) noex ;
extern int sigtimedwait(const sigset_t *,siginfo_t *,CTIMESPEC *) noex ;
extern int sigwaitinfoto(const sigset_t *,siginfo_t *,CTIMESPEC *) noex ;

EXTERNC_end

#endif /* (!defined(SYSHAS_SIGWAIT)) || (SYSHAS_SIGWAIT == 0) */
/* SIGWAIT end */
/*----------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------*/
/* TIMER begin (POSIX real-time) */
#if	(!defined(SYSHAS_TIMER)) || (SYSHAS_TIMER == 0)

#ifndef	TYPEDEF_ITIMERSPEC
#define	TYPEDEF_ITIMERSPEC
struct itimerspec {
	struct timespec	it_interval ;
	struct timespec	it_value ;
} ;
typedef int	timer_t ;
#endif /* TYPEDEF_ITIMERSPEC */

#ifndef	TYPEDEF_TIMER
#define	TYPEDEF_TIMER
typedef int	timer_t ;
#endif

#ifndef	ITIMERSPEC
#define	ITIMERSPEC	struct itimerspec
#endif

#ifndef	TIMER_ABSTIME
#define	TIMER_ABSTIME	0		/* hack! */
#endif

EXTERNC_begin

extern int timer_create(clockid_t,SIGEVENT *,timer_t *) noex ;
extern int timer_delete(timer_t) noex ;
extern int timer_settime(timer_t,int,ITIMERSPEC *,ITIMERSPEC *) noex ;
extern int timer_gettime(timer_t,ITIMERSPEC *) noex ;
extern int timer_getoverrun(timer_t) noex ;

EXTERNC_end

#endif /* (!defined(SYSHAS_TIMER)) || (SYSHAS_TIMER == 0) */
/* TIMER end */
/*----------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------*/
/* RELTIMEDWAIT begin */
#if	(!defined(SYSHAS_RELTIMEDWAIT)) || (SYSHAS_RELTIMEDWAIT == 0)

#ifndef	PTM
#define	PTM		pthread_mutex_t
#endif

#ifndef	PTC
#define	PTC		pthread_cond_t
#endif

EXTERNC_begin

extern int pthread_cond_reltimedwait_np(PTC *,PTM *,CTIMESPEC *) noex ;

EXTERNC_end

#endif /* (!defined(SYSHAS_RELTIMEDWAIT)) || (SYSHAS_RELTIMEDWAIT == 0) */
/* RELTIMEDWAIT end */
/*----------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------*/
/* MUTEXROBUST start */
#if	(!defined(SYSHAS_MUTEXROBUST)) || (SYSHAS_MUTEXROBUST == 0)

#include	<pthread.h>

#ifndef	PTMA
#define	PTMA		pthread_mutexattr_t
#endif

#ifndef	SUBROUTINE_PTHREADMUTEXATTR
#define	SUBROUTINE_PTHREADMUTEXATTR
EXTERNC_begin

extern int pthread_mutexattr_setrobust_np(PTMA *,int) noex ;
extern int pthread_mutexattr_getrobust_np(PTMA *,int *) noex ;

EXTERNC_end
#endif /* SUBROUTINE_PTHREADMUTEXATTR */

#endif /* (!defined(SYSHAS_MUTEXROBUST)) || (SYSHAS_MUTEXROBUST == 0) */
/* MUTEXROBUST end */
/*----------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------*/
/* UCTIMEOUT begin */

#ifndef	SIGTIMEOUT
#define	SIGTIMEOUT	0		/* hack for now */
#endif

/* UCTIMEOUT begin */
/*----------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------*/
/* MEMCNTL begin */
#if	(!defined(SYSHAS_MEMCNTL)) || (SYSHAS_MEMCNTL == 0)

#ifndef	SUBROUTINE_MEMCNTL
#define	SUBROUTINE_MEMCNTL
EXTERNC_begin

extern int memcntl(void *,size_t,int,void *,int,int) noex ;

EXTERNC_end
#endif /* SUBROUTINE_MEMCNTL */

#endif /* (!defined(SYSHAS_MEMCNTL)) || (SYSHAS_MEMCNTL == 0) */
/* MEMCNTL end */
/*----------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------*/
/* PSEM begin (unnamed POSIX® semaphores) */
#if	CF_PSEMADAPT
#if	(!defined(SYSHAS_PSEM)) || (SYSHAS_PSEM == 0)

#ifndef	SUBROUTINE_SEMINIT
#define	SUBROUTINE_SEMINIT
EXTERNC_begin

extern int darwinsem_init(sem_t *,int,unsigned int) noex ;
extern int darwinsem_destroy(sem_t *) noex ;

EXTERNC_end
#endif /* SUBROUTINE_SEMINIT */

#endif /* (!defined(SYSHAS_PSEM)) || (SYSHAS_PSEM == 0) */
#endif /* CF_PSEMADAPT */
/* PSEM end */
/*----------------------------------------------------------------------------*/


#endif /* defined(OSNAME_Darwin) && (OSNAME_Darwin > 0) */
/* USYS_DARWIN finish */


#endif /* USYS_DARWIN_INCLUDE */


