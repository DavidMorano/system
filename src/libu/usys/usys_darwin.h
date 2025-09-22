/* usys_darwin HEADER */
/* charset=ISO8859-1 */
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

  	Name:
	usys_darwin

	Description:
	This file contains the UNIX® system types that the brain-damaged
	MacOS operating system does NOT have.  We are trying in a
	very small way to make up for some of the immense brain-damage
	within the Apple Darwin operating system.

*******************************************************************************/

#ifndef	USYSDARWIN_INCLUDE
#define	USYSDARWIN_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */

/* USYSDARWIN start */
#if	defined(OSNAME_Darwin) && (OSNAME_Darwin > 0)

#include	<sys/types.h>
#include	<sys/wait.h>		/* <- type |idtype_t| is there */
#include	<sys/time.h>		/* <- |TIMESPEC| is there */
#include	<sys/sysctl.h>		/* <- Darwin |sysctl(2)| */
#include	<signal.h>
#include	<time.h>
#include	<pthread.h>
#include	<semaphore.h>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>

#include	<usysargz.hh>

/*----------------------------------------------------------------------------*/
/* DEFINES begin */

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

/* DEFINES end */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/* SIGSEND begin */
#if	(!defined(SYSHAS_SIGSEND)) || (SYSHAS_SIGSEND == 0)

#ifndef	TYPEDEF_IDOP
#define	TYPEDEF_IDOP
typedef int		idop_t ;
#endif

#ifndef	STRUCT_PROCSET
#define	STRUCT_PROCSET
struct procset {
	idop_t		p_op ;
	idtype_t	p_lidtype ;
	id_t		p_lid ;
	idtype_t	p_ridtype ;
	id_t		p_rid ;
} ;
#endif /* STRUCT_PROCSET */

#ifndef	TYPEDEF_PROCSET
#define	TYPEDEF_PROCSET
typedef struct procset		procset_t ;
#endif

EXTERNC_begin

extern unixret_t sigsend(idtype_t,id_t,int) noex ;
extern unixret_t sigsendset(procset_t *,int) noex ;
extern unixret_t sigqueue(pid_t,int,const union sigval) noex ;

EXTERNC_end

#endif /* (!defined(SYSHAS_SIGSEND)) || (SYSHAS_SIGSEND == 0) */
/* SIGSEND end */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/* SIGWAITINFO begin */
#if	(!defined(SYSHAS_SIGWAITINFO)) || (SYSHAS_SIGWAITINFO == 0)

EXTERNC_begin

extern unixret_t sigwaitinfo(const sigset_t *,siginfo_t *) noex ;
extern unixret_t sigtimedwait(const sigset_t *,siginfo_t *,CTIMESPEC *) noex ;
extern unixret_t sigwaitinfoto(const sigset_t *,siginfo_t *,CTIMESPEC *) noex ;

EXTERNC_end

#endif /* (!defined(SYSHAS_SIGWAITINFO)) || (SYSHAS_SIGWAITINFO == 0) */
/* SIGWAITINFO end */
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
#endif /* TYPEDEF_ITIMERSPEC */

#ifndef	TYPEDEF_TIMER
#define	TYPEDEF_TIMER
typedef int	timer_t ;
#endif

#ifndef	ITIMERSPEC
#define	ITIMERSPEC	struct itimerspec
#endif

#ifndef	CITIMERSPEC
#define	CITIMERSPEC	const ITIMERSPEC
#endif

#ifndef	TIMER_ABSTIME
#define	TIMER_ABSTIME	1		/* hack! */
#endif

EXTERNC_begin

extern errno_t timer_create(clockid_t,SIGEVENT *,timer_t *) noex ;
extern errno_t timer_delete(timer_t) noex ;
extern errno_t timer_settime(timer_t,int,CITIMERSPEC *,ITIMERSPEC *) noex ;
extern errno_t timer_gettime(timer_t,ITIMERSPEC *) noex ;
extern errno_t timer_getoverrun(timer_t) noex ;

EXTERNC_end

#endif /* (!defined(SYSHAS_TIMER)) || (SYSHAS_TIMER == 0) */
/* TIMER end */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/* RELTIMEDWAIT begin */
#if	(!defined(SYSHAS_RELTIMEDWAIT)) || (SYSHAS_RELTIMEDWAIT == 0)

#include	<pthread.h>

#ifndef	PTM
#define	PTM		pthread_mutex_t
#endif

#ifndef	PTC
#define	PTC		pthread_cond_t
#endif

EXTERNC_begin

extern errno_t pthread_cond_reltimedwait_np(PTC *,PTM *,CTIMESPEC *) noex ;

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

extern errno_t pthread_mutexattr_setrobust_np(PTMA *,int) noex ;
extern errno_t pthread_mutexattr_getrobust_np(PTMA *,int *) noex ;

EXTERNC_end
#endif /* SUBROUTINE_PTHREADMUTEXATTR */

#endif /* (!defined(SYSHAS_MUTEXROBUST)) || (SYSHAS_MUTEXROBUST == 0) */
/* MUTEXROBUST end */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/* UCTIMEOUT begin */

#ifndef	SIGTIMEOUT
#define	SIGTIMEOUT	SIGVTALRM		/* hack for now (or always) */
#endif

/* UCTIMEOUT begin */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/* MEMCNTL begin */
#if	(!defined(SYSHAS_MEMCNTL)) || (SYSHAS_MEMCNTL == 0)

#ifndef	SUBROUTINE_MEMCNTL
#define	SUBROUTINE_MEMCNTL
EXTERNC_begin

extern unixret_t memcntl(void *,size_t,int,void *,int,int) noex ;

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

extern unixret_t darwinsem_init(sem_t *,int,unsigned int) noex ;
extern unixret_t darwinsem_destroy(sem_t *) noex ;

EXTERNC_end
#endif /* SUBROUTINE_SEMINIT */

#endif /* (!defined(SYSHAS_PSEM)) || (SYSHAS_PSEM == 0) */
#endif /* CF_PSEMADAPT */
/* PSEM end */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/* UTMPENT-FIELDS begin */
#if	defined(SYSHAS_UTMPX) && (SYSHAS_UTMPX > 0)

#ifndef	UTMPXSTRUCTGIELD_SESSION
#define	UTMPXSTRUCTGIELD_SESSION

#define	ut_session	ut_type

#endif /* UTMPXSTRUCTGIELD_SESSION */

#ifndef	UTMPXSTRUCTGIELD_SYSLEN
#define	UTMPXSTRUCTGIELD_SYSLEN

#define	ut_syslen	ut_type

#endif /* UTMPXSTRUCTGIELD_SYSLEN */

#endif /* defined(SYSHAS_UTMPX) && (SYSHAS_UTMPX > 0) */
/* UTMPENT_FIELDS end */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
#ifdef	COMMENT /* should not be necessary on Darwin */
#ifndef	SUBROUTINE_RENAME
#define	SUBROUTINE_RENAME
EXTERNC_begin
extern unixret_t rename(cchar *,cchar *) noex ;
EXTERNC_end
#endif
#endif /* COMMENT */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/* SUBROUTINES begin */
#ifdef	__cplusplus

namespace usys {
    extern sysret_t darwin_usysctl(char *,int,cchar *) noex ;
    extern sysret_t darwin_ttyname(int,char *,int) noex ; /* currently unused */
    extern sysret_t darwin_execname(char *,int) noex ;
    extern sysret_t darwin_argz(char *,int) noex ;
    extern cchar *darwin_getargz() noex ;
}

#endif /* __cplusplus */
/* SUBROUTINES end */
/*----------------------------------------------------------------------------*/

#endif /* defined(OSNAME_Darwin) && (OSNAME_Darwin > 0) */
/* USYSDARWIN finish */


#endif /* USYSDARWIN_INCLUDE */


