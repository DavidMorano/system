/* syshas HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* specify what features each operating system has */
/* version %I% last-modified %G% */


/* revision history:

	= 2000-05-14, David A­D­ Morano
	Originally written for Rightcore Network Services.

	= 2017-08-01, David A­D­ Morano
	Updated for lack of interfaces in MacOS Darwin

	= 2023-10-06, David A­D­ Morano
	I updated this to include an additional item that many of
	these operating systems (Darwin and Linux) do not have have;
	namely |strlcpy(3c)| and |strnlen(3c)|.

*/

/* Copyright © 2000,2017,2023 David A­D­ Morano.  All rights reserved. */

#ifndef	SYSHAS_INCLUDE
#define	SYSHAS_INCLUDE


/******************************************************************************/
#if	defined(OSNAME_SunOS) && (OSNAME_SunOS > 0)

/* Solaris®: system has shadow password DB */
#define	SYSHAS_SHADOW		1

/* Solaris®: projects */
#define	SYSHAS_PROJECT		1

/* Solaris®: system has |statvfs(2)| call and friends */
#define	SYSHAS_STATVFS		1

/* Solaris®: system has |poll(2)| call and friends */
#define	SYSHAS_POLL		1

/* Solaris®: system has STREAMS® framework */
#define	SYSHAS_STREAMS		1

/* Solaris®: sustem has POSIX® real-time timers */
#define	SYSHAS_TIMER		1

/* Solaris®: system has ACL framework */
#define	SYSHAS_ACL		1

/* Solaris®: system has user attributes framework */
#define	SYSHAS_USERATTR		1

/* Solaris®: system has Security-Database framework */
#define	SYSHAS_SECDB		1

/* Solaris®: has these two stupid environment manipulation subroutines */
#define	SYSHAS_SETENV		0
#define	SYSHAS_UNSETENV		0

/* Solaris®: system information header for 'sysinfo(2)' */
#define	SYSHAS_SYSINFO		1

/* Solaris®: system has a 'off_t' data type (assumed to be 64 bits) */
#define	SYSHAS_OFFSET		1

/* Solaris®: getcwd(3c) */
#define	SYSHAS_GETCWD		1

/* Solaris®: AIO */
#define	SYSHAS_AIO		1

/* Solaris®: typedefs */
#define	SYSHAS_TYPEUSHORT	0
#define	SYSHAS_TYPEUINT		0

/* Solaris®: |dirent_t| */
#define	SYSHAS_TYPEDIRENT	1

/* Solaris®: |sig_t| */
#define	SYSHAS_TYPESIGT		0

/* Solaris®: |in6_addr_t| */
#define	SYSHAS_TYPEIN6ADDRT	1

/* Solaris®: tasks */
#define	SYSHAS_TASK		1

/* Solaris®: UTMPX */
#define	SYSHAS_UTMPX		1

/* Solaris®: UTMP-name */
#define	SYSHAS_UTMPNAME		1
#define	SYSHAS_UTMPXNAME	1

/* Solaris®: loadavg(3c) */
#define	SYSHAS_LOADAVG		1
#define	SYSHAS_LOADAVGINT	1

/* Solaris®: readdir_r(3c) */
#define	SYSHAS_READDIRR		1

/* Solaris®: getpwxxx_r(3c) */
#define	SYSHAS_GETPWXXXR	1	/* any of the others (logical OR) */
#define	SYSHAS_GETPWGNUR	0	/* has GNU (GLIBC) interface */
#define	SYSHAS_GETPWENTR	1
#define	SYSHAS_GETPWNAMR	1
#define	SYSHAS_GETPWUIDR	1

/* Solaris®: getspxxx_r(3c) */
#define	SYSHAS_GETSPXXXR	1	/* any of the others (logical OR) */
#define	SYSHAS_GETSPGNUR	0	/* has GNU (GLIBC) interface */
#define	SYSHAS_GETSPENTR	1
#define	SYSHAS_GETSPNANR	1

/* Solaris®: getgrxxx_r(3c) */
#define	SYSHAS_GETGRXXXR	1	/* any of the others (logical OR) */
#define	SYSHAS_GETGRGNUR	0	/* has GNU (GLIBC) interface */
#define	SYSHAS_GETGRENTR	1
#define	SYSHAS_GETGRNAMR	1
#define	SYSHAS_GETGRGIDR	1

/* Solaris®: getpjxxx_r(3c) */
#define	SYSHAS_GETPJXXXR	1	/* any of the others (logical OR) */
#define	SYSHAS_GETPJGNUR	0	/* has GNU (GLIBC) interface */
#define	SYSHAS_GETPJENTR	1
#define	SYSHAS_GETPJNAMR	1
#define	SYSHAS_GETPJPIDR	1

/* Solaris®: getprotobyxxx_r(3c) */
#define	SYSHAS_GETPRXXXR	1
#define	SYSHAS_GETPRGNUR	0	/* has GNU (GLIBC) interface */

/* Solaris®: getnetbyxxx_r(3c) */
#define	SYSHAS_GETNWXXXR	1
#define	SYSHAS_GETNWGNUR	0	/* has GNU (GLIBC) interface */

/* Solaris®: gethostbyxxx_r(3c) */
#define	SYSHAS_GETHOXXXR	1
#define	SYSHAS_GETHOGNUR	0	/* has GNU (GLIBC) interface */

/* Solaris®: getservbyxxx_r(3c) */
#define	SYSHAS_GETSVXXXR	1
#define	SYSHAS_GETSVGNUR	0	/* has GNU (GLIBC) interface */

/* Solaris®: getlogin__r(3c) */
#define	SYSHAS_GETLOGINR	1

/* Solaris®: localtime_r(3c) */
#define	SYSHAS_LOCALTIMER	1

/* Solaris®: gmtime_r(3c) */
#define	SYSHAS_GMTIMER		1

/* Solaris®: ttyname_r(3c) */
#define	SYSHAS_TTYNAMER		1

/* Solaris®: postix_openpt(3c) */
#define	SYSHAS_OPENPT		0

/* Solaris®: ptmx(9) */
#define	SYSHAS_PTMX		1

/* Solaris®: POSIX shared memory (|shm(3rt)|) */
#define	SYSHAS_PSHM		1

/* Solaris®: POSIX regular semaphores (|sem(3rt)| */
#define	SYSHAS_PSEM		1

/* Solaris: POSIX named semaphores (|sem(3rt)|) */
#define	SYSHAS_NSEM		1

/* Solaris®: POSIX message queues (|mq(3rt)|) */
#define	SYSHAS_PMQ		1

/* Solaris®: AUDIT - part of Solaris® Basic-Security-Module (BSM) */
#define	SYSHAS_AUDIT		1

/* Solaris®: get-directory-entries (|getdents(2)|) */
#define	SYSHAS_GETDENTS		1

/* Solaris®: XTI */
#define	SYSHAS_XTI		1

/* Solaris®: robust mutexes */
#define	SYSHAS_MUTEXROBUST	1

/* Solaris®: strnlen(3c) */
#define	SYSHAS_STRNLEN		1

/* Solaris®: strlcpy(3c) */
#define	SYSHAS_STRLCPY		1

/* Solaris®: strnstr(3c) */
#define	SYSHAS_STRNSTR		1

/* Solaris®: sigqueue(3c) */
#define	SYSHAS_SIGQUEUE		1

/* Solaris®: sigsend(2) */
#define	SYSHAS_SIGSEND		1

/* Solaris®: sigsendset(2) */
#define	SYSHAS_SIGSENDSET	1

/* Solaris®: sigwait(2) */
#define	SYSHAS_SIGWAIT		1

/* Solaris®: pthread_sigsend(2) */
#define	SYSHAS_PTHREADSIGSEND	1

/* Solaris®: ucontext */
#define	SYSHAS_UCONTEXT		1

/* Solaris®: getcontext(3c) */
#define	SYSHAS_GETCONTEXT	1

/* Solaris®: Relative-Timed-Wait feature */
#define	SYSHAS_RELTIMEDWAIT	1

/* Solaris®: Memory |memcntl(2)| */
#define	SYSHAS_MEMCNTL		1

/* Solaris®: Memory |plock(2)| */
#define	SYSHAS_MEMPLOCK		1

/* Solaris®: |gethrtime(3c)| */
#define	SYSHAS_GETHRTIME	1

/* Solaris®: |getrandom(2)| */
#define	SYSHAS_GETRANDOM	1

/* Solaris®: |mkdirp(3gen)| */
#define	SYSHAS_MKDIRP		1

/* Solaris®: |stime(2)| */
#define	SYSHAS_STIME		1

/* Solaris®: |resolvepath(2)| */
#define	SYSHAS_RESOLVEPATH	1	/* thank-you Solaris® */

/* Solaris®: |waitid(2)| */
#define	SYSHAS_WAITID		1

/* Solaris®: |pipe2(2)| */
#if	defined(OSNUM_Solaris) && (OSNUM_Solaris >= 9)
#define	SYSHAS_PIPES		1
#else
#define	SYSHAS_PIPES		0
#endif

/* Solaris®: |getexecname| */
#define	SYSHAS_GETEXECNAME	1

/* Solaris®: |libproc| */
#define	SYSHAS_LIBPROC		0

/******************************************************************************/
#elif	defined(OSNAME_Darwin) && (OSNAME_Darwin > 0)

/* Darwin: system has shadow password DB */
#define	SYSHAS_SHADOW		0

/* Darwin: projects */
#define	SYSHAS_PROJECT		0

/* Darwin: system has |statvfs(2)| call and friends */
#if	defined(OSNUM_Darwin) && (OSNUM_Darwin >= 8)
#define	SYSHAS_STATVFS		1
#else
#define	SYSHAS_STATVFS		0
#endif

/* Darwin: system has |poll(2)| call and friends */
#if	defined(OSNUM_Darwin) && (OSNUM_Darwin >= 8)
#define	SYSHAS_POLL		1
#else
#define	SYSHAS_POLL		0
#endif

/* Darwin: system has STREAMS® framework */
#define	SYSHAS_STREAMS		0

/* Darwin: does *not* have POSIX real-time timers */
#define	SYSHAS_TIMER		0

/* Darwin: system has ACL framework */
#define	SYSHAS_ACL		0

/* Darwin: system has user attributes framework */
#define	SYSHAS_USERATTR		0

/* Darwin: system has Security-Database framework */
#define	SYSHAS_SECDB		0

/* Darwin: has these two stupid environment manipulation subroutines */
#define	SYSHAS_SETENV		0
#define	SYSHAS_UNSETENV		0

/* Darwin: system information header for |sysinfo(2)| */
#define	SYSHAS_SYSINFO		0

/* Darwin: system has a |offset_t| data type (assumed to be 64 bits) */
#define	SYSHAS_OFFSET		0

/* Darwin: getcwd(3c) */
#define	SYSHAS_GETCWD		1

/* Darwin: AIO */
#if	defined(OSNUM_Darwin) && (OSNUM_Darwin >= 8)
#define	SYSHAS_AIO		1
#else
#define	SYSHAS_AIO		0
#endif

/* Darwin: typedefs */
#define	SYSHAS_TYPEUSHORT	1
#define	SYSHAS_TYPEUINT		1

/* Darwin: |dirent_t| */
#define	SYSHAS_TYPEDIRENT	0

/* Darwin®: |sig_t| */
#define	SYSHAS_TYPESIGT		1

/* Darwin®: |in6_addr_t| */
#define	SYSHAS_TYPEIN6ADDRT	1

/* Darwin: tasks */
#define	SYSHAS_TASK		0

/* Darwin: UTMPX */
#if	defined(OSNUM_Darwin) && (OSNUM_Darwin >= 8)
#define	SYSHAS_UTMPX		1
#else
#define	SYSHAS_UTMPX		0
#endif

/* Darwin: UTMP-name */
#define	SYSHAS_UTMPNAME		0
#if	defined(OSNUM_Darwin) && (OSNUM_Darwin >= 9)
#define	SYSHAS_UTMPXNAME	1
#else
#define	SYSHAS_UTMPXNAME	0
#endif

/* Darwin: loadavg(3c) */
#define	SYSHAS_LOADAVG		1
#define	SYSHAS_LOADAVGINT	0

/* Darwin: readdir_r(3c) */
#define	SYSHAS_READDIRR		1

/* Darwin: getpwxxx_r(3c) */
#define	SYSHAS_GETPWXXXR	1	/* any of the others (logical OR) */
#define	SYSHAS_GETPWGNUR	0	/* has GNU (GLIBC) interface */
#define	SYSHAS_GETPWENTR	0
#define	SYSHAS_GETPWNAMR	1
#define	SYSHAS_GETPWUIDR	1

/* Darwin: getspxxx_r(3c) */
#define	SYSHAS_GETSPXXXR	0	/* any of the others (logical OR) */
#define	SYSHAS_GETSPGNUR	0	/* has GNU (GLIBC) interface */
#define	SYSHAS_GETSPENTR	0
#define	SYSHAS_GETSPNAMR	0

/* Darwin: getgrxxx_r(3c) */
#define	SYSHAS_GETGRXXXR	1	/* any of the others (logical OR) */
#define	SYSHAS_GETGRGNUR	0	/* has GNU (GLIBC) interface */
#define	SYSHAS_GETGRENTR	0	
#define	SYSHAS_GETGRNAMR	1
#define	SYSHAS_GETGRGIDR	1

/* Darwin: getpjxxx_r(3c) */
#define	SYSHAS_GETPJXXXR	0	/* any of the others (logical OR) */
#define	SYSHAS_GETPJGNUR	0	/* has GNU (GLIBC) interface */
#define	SYSHAS_GETPJENTR	0
#define	SYSHAS_GETPJNAMR	0
#define	SYSHAS_GETPJPIDR	0

/* Darwin: getprotobyxxx_r(3c) */
#define	SYSHAS_GETPRXXXR	0
#define	SYSHAS_GETPRGNUR	0	/* has GNU (GLIBC) interface */

/* Darwin: getnetbyxxx_r(3c) */
#define	SYSHAS_GETNWXXXR	0
#define	SYSHAS_GETNWGNUR	0	/* has GNU (GLIBC) interface */

/* Darwin: gethostbyxxx_r(3c) */
#define	SYSHAS_GETHOXXXR	0
#define	SYSHAS_GETHOGNUR	0	/* has GNU (GLIBC) interface */

/* Darwin: getservbyxxx_r(3c) */
#define	SYSHAS_GETSVXXXR	0
#define	SYSHAS_GETSVGNUR	0	/* has GNU (GLIBC) interface */

/* Darwin: getlogin__r(3c) */
#define	SYSHAS_GETLOGINR	0

/* Darwin: localtime_r(3c) */
#define	SYSHAS_LOCALTIMER	1

/* Darwin: gmtime_r(3c) */
#define	SYSHAS_GMTIMER		1

/* Darwin: ttyname_r(3c) */
#define	SYSHAS_TTYNAMER		1

/* Darwin: postix_openpt(3c) */
#define	SYSHAS_OPENPT		1

/* Darwin: ptmx(9) */
#define	SYSHAS_PTMX		1

/* Darwin: POSIX shared memory (|shm(3rt)|) */
#define	SYSHAS_PSHM		1

/* Darwin: POSIX unnamed semaphores (|sem(3rt)|) */
#define	SYSHAS_PSEM		0

/* Darwin: POSIX named semaphores (|sem(3rt)|) */
#define	SYSHAS_NSEM		1

/* Darwin: POSIX message queues (|mq(3rt)|) */
#define	SYSHAS_PMQ		0

/* Darwin: AUDIT - part of Solaris® Basic-Security-Module (BSM) */
#define	SYSHAS_AUDIT		0

/* Darwin: get-directory-entries (|getdents(2)|) */
#define	SYSHAS_GETDENTS		0

/* Darwin: XTI */
#define	SYSHAS_XTI		0

/* Darwin: robust mutexes */
#define	SYSHAS_MUTEXROBUST	0

/* Darwin: strnlen(3c) */
#define	SYSHAS_STRNLEN		1

/* Darwin: strlcpy(3c) */
#define	SYSHAS_STRLCPY		1

/* Darwin®: strnstr(3c) */
#define	SYSHAS_STRNSTR		1

/* Darwin: sigqueue(3c) */
#define	SYSHAS_SIGQUEUE		0

/* Darwin: sigsend(2) */
#define	SYSHAS_SIGSEND		0

/* Darwin: sigsendset(2) */
#define	SYSHAS_SIGSENDSET	0

/* Darwin: sigwait(2) */
#define	SYSHAS_SIGWAIT		0

/* Darwin: pthread_sigsend(2) */
#define	SYSHAS_PTHREADSIGSEND	0

/* Darwin: ucontext */
#define	SYSHAS_UCONTEXT		1

/* Darwin: getcontext(3c) */
#define	SYSHAS_GETCONTEXT	1

/* Darwin: Relative-Timed-Wait feature */
#define	SYSHAS_RELTIMEDWAIT	0

/* Darwin: Memory |memcntl(2)| */
#define	SYSHAS_MEMCNTL		0

/* Darwin: Memory |plock(2)| */
#define	SYSHAS_MEMPLOCK		0

/* Darwin: |gethrtime(3c)| */
#define	SYSHAS_GETHRTIME	0

/* Darwin: |getrandom(2)| */
#define	SYSHAS_GETRANDOM	0

/* Darwin: |mkdirp(3gen)| */
#define	SYSHAS_MKDIRP		0

/* Darwin: |stime(2)| */
#define	SYSHAS_STIME		0

/* Darwin: |resolvepath(2)| */
#define	SYSHAS_RESOLVEPATH	0	/* expected to not have it! */

/* Darwin: |waitid(2)| */
#define	SYSHAS_WAITID		1	/* <- welcome to club! (confirmed) */

/* Darwin: |pipe2(2)| */
#define	SYSHAS_PIPES		0

/* Darwin®: |getexecname| */
#define	SYSHAS_GETEXECNAME	0

/* Darwin®: |libproc| */
#define	SYSHAS_LIBPROC		1

/******************************************************************************/
#elif	defined(OSNAME_Linux) && (OSNAME_Linux > 0)

/* Linux: system has shadow password DB */
#define	SYSHAS_SHADOW		1

/* Linux: projects */
#define	SYSHAS_PROJECT		0

/* Linux: system has |statvfs(2)| call and friends */
#define	SYSHAS_STATVFS		1

/* Linux: system has |poll(2)| call and friends */
#define	SYSHAS_POLL		1

/* Linux: system has STREAMS® framework */
#define	SYSHAS_STREAMS		1

/* Linux: sustem has POSIX® real-time timers */
#define	SYSHAS_TIMER		1

/* Linux: system has ACL framework */
#define	SYSHAS_ACL		0

/* Linux: system has user attributes framework */
#define	SYSHAS_USERATTR		0

/* Linux: system has Security-Database framework */
#define	SYSHAS_SECDB		0

/* Linux: has these two stupid environment manipulation subroutines */
#define	SYSHAS_SETENV		0
#define	SYSHAS_UNSETENV		0

/* Linux: system information header for |sysinfo(2)| */
#define	SYSHAS_SYSINFO		1

/* Linux: system has a |offset_t| data type (assumed to be 64 bits) */
#define	SYSHAS_OFFSET		1

/* Linux: getcwd(3c) */
#define	SYSHAS_GETCWD		1

/* Linux: AIO */
#define	SYSHAS_AIO		1

/* Linux: typedefs */
#define	SYSHAS_TYPEUSHORT	0
#define	SYSHAS_TYPEUINT		0

/* Linux: |dirent_t| */
#define	SYSHAS_TYPEDIRENT	0

/* Linux: |sig_t| */
#define	SYSHAS_TYPESIGT		0

/* Linux®: |in6_addr_t| */
#define	SYSHAS_TYPEIN6ADDRT	0

/* Linux: tasks */
#define	SYSHAS_TASK		1

/* Linux: UTMPX */
#define	SYSHAS_UTMPX		1

/* Linux: UTMP-name */
#define	SYSHAS_UTMPNAME		1
#define	SYSHAS_UTMPXNAME	1

/* Linux: loadavg(3c) */
#define	SYSHAS_LOADAVG		1
#define	SYSHAS_LOADAVGINT	0

/* Linux: readdir_r(3c) */
#define	SYSHAS_READDIRR		1

/* Linux: getpwxxx_r(3c) */
#define	SYSHAS_GETPWXXXR	1	/* any of the others (logical OR) */
#define	SYSHAS_GETPWGNUR	1	/* has GNU (GLIBC) interface */
#define	SYSHAS_GETPWENTR	1
#define	SYSHAS_GETPWNAMR	1
#define	SYSHAS_GETPWUIDR	1

/* Linux: getspxxx_r(3c) */
#define	SYSHAS_GETSPXXXR	1	/* any of the others (logical OR) */
#define	SYSHAS_GETSPGNUR	1	/* has GNU (GLIBC) interface */
#define	SYSHAS_GETSPENTR	1
#define	SYSHAS_GETSPNAMR	1

/* Linux: getgrxxx_r(3c) */
#define	SYSHAS_GETGRXXXR	1	/* any of the others (logical OR) */
#define	SYSHAS_GETGRGNUR	1	/* has GNU (GLIBC) interface */
#define	SYSHAS_GETGRENTR	1
#define	SYSHAS_GETGRNAMR	1
#define	SYSHAS_GETGRGIDR	1

/* Linux: getpjxxx_r(3c) */
#define	SYSHAS_GETPJXXXR	0	/* any of the others (logical OR) */
#define	SYSHAS_GETPJGNUR	0	/* has GNU (GLIBC) interface */
#define	SYSHAS_GETPJENTR	0
#define	SYSHAS_GETPJNAMR	0
#define	SYSHAS_GETPJPIDR	0

/* Linux: getprotobyxxx_r(3c) */
#define	SYSHAS_GETPRXXXR	1	/* has a reentrant interface */
#define	SYSHAS_GETPRGNUR	1	/* has GNU (GLIBC) interface */

/* Linux: getnetbyxxx_r(3c) */
#define	SYSHAS_GETNWXXXR	1	/* has a reentrant interface */
#define	SYSHAS_GETNWGNUR	1	/* has GNU (GLIBC) interface */

/* Linux: gethostbyxxx_r(3c) */
#define	SYSHAS_GETHOXXXR	1	/* has a reentrant interface */
#define	SYSHAS_GETHOGNUR	1	/* has GNU (GLIBC) interface */

/* Linux: getservbyxxx_r(3c) */
#define	SYSHAS_GETSVXXXR	1	/* has a reentrant interface */
#define	SYSHAS_GETSVGNUR	1	/* has GNU (GLIBC) interface */

/* Linux: getlogin__r(3c) */
#define	SYSHAS_GETLOGINR	1

/* Linux: localtime_r(3c) */
#define	SYSHAS_LOCALTIMER	1

/* Linux: gmtime_r(3c) */
#define	SYSHAS_GMTIMER		1

/* Linux: ttyname_r(3c) */
#define	SYSHAS_TTYNAMER		1

/* Linux: postix_openpt(3c) */
#define	SYSHAS_OPENPT		0

/* Linux: ptmx(9) */
#define	SYSHAS_PTMX		1

/* Linux: POSIX shared memory (|shm(3rt)|) */
#define	SYSHAS_PSHM		1

/* Linux: POSIX regular semaphores (|sem(3rt)|) */
#define	SYSHAS_PSEM		1

/* Linux: POSIX named semaphores (|sem(3rt)|) */
#define	SYSHAS_NSEM		1

/* Linux: POSIX message queues (|mq(3rt)|) */
#define	SYSHAS_PMQ		1

/* Linux: AUDIT - part of Linux Basic-Security-Module (BSM) */
#define	SYSHAS_AUDIT		1

/* Linux: get-directory-entries (|getdents(2)|) */
#define	SYSHAS_GETDENTS		1

/* Linux: XTI */
#define	SYSHAS_XTI		1

/* Linux: robust mutexes */
#define	SYSHAS_MUTEXROBUST	1

/* Linux: strnlen(3c) */
#define	SYSHAS_STRNLEN		1	/* finally */

/* Linux: strlcpy(3c) */
#define	SYSHAS_STRLCPY		1	/* finally */

/* Linux®: strnstr(3c) */
#define	SYSHAS_STRNSTR		0

/* Linux: sigqueue(3c) */
#define	SYSHAS_SIGQUEUE		1

/* Linux: sigsend(2) */
#define	SYSHAS_SIGSEND		1

/* Linux: sigsendset(2) */
#define	SYSHAS_SIGSENDSET	0

/* Linux: sigwait(2) */
#define	SYSHAS_SIGWAIT		1

/* Linux: pthread_sigsend(2) */
#define	SYSHAS_PTHREADSIGSEND	0

/* Linux: ucontext */
#define	SYSHAS_UCONTEXT		1

/* Linux: getcontext(3c) */
#define	SYSHAS_GETCONTEXT	1

/* Linux: Relative-Timed-Wait feature */
#define	SYSHAS_RELTIMEDWAIT	0

/* Linux: Memory |memcntl(2)| */
#define	SYSHAS_MEMCNTL		0

/* Linux: Memory |plock(2)| */
#define	SYSHAS_MEMPLOCK		0

/* Linux: |gethrtime(3c)| */
#define	SYSHAS_GETHRTIME	0

/* Linux: |getrandom(2)| */
#define	SYSHAS_GETRANDOM	1

/* Linux: |mkdirp(3gen)| */
#define	SYSHAS_MKDIRP		0

/* Linux: |stime(2)| */
#define	SYSHAS_STIME		0

/* Linux: |resolvepath(2)| */
#define	SYSHAS_RESOLVEPATH	0	/* some sources falsely claim YES! */

/* Linux: |waitid(2)| */
#define	SYSHAS_WAITID		1

/* Linux: |pipe2(2)| */
#define	SYSHAS_PIPES		1

/* Linux®: |getexecname| */
#define	SYSHAS_GETEXECNAME	0

/* Linux®: |libproc| */
#define	SYSHAS_LIBPROC		0

/******************************************************************************/
#endif /* OSNAME */


#ifdef	__cplusplus

struct syshas_mgr {
    	typedef unsigned int	uint ;
        uint    shadow:1 ;
        uint    project:1 ;
        uint    statvfs:1 ;
        uint    poll:1 ;
        uint    streams:1 ;
        uint    timer:1 ;
        uint    acl:1 ;
        uint    userattr:1 ;
        uint    secdb:1 ;
        uint    setenv:1 ;
        uint    unsetenv:1 ;
        uint    sysinfo:1 ;
        uint    offset:1 ;
        uint    getcwd:1 ;
        uint    aio:1 ;
        uint    typeushort:1 ;
        uint    typeuint:1 ;
        uint    typedirentt:1 ;
        uint    typesigt:1 ;
        uint    typein6addrt:1 ;
        uint    task:1 ;
        uint    utmpx:1 ;
        uint    utmpname:1 ;
        uint    utmpxname:1 ;
        uint    loadavg:1 ;
        uint    loadavgint:1 ;
        uint    readdirr:1 ;
        uint    getpwxxxr:1 ;
        uint    getpwgnur:1 ;
        uint    getpwentr:1 ;
        uint    getpwnamr:1 ;
        uint    getpwuidr:1 ;
        uint    getspxxxr:1 ;
        uint    getspgnur:1 ;
        uint    getspentr:1 ;
        uint    getspnamr:1 ;
        uint    getgrxxxr:1 ;
        uint    getgrgnur:1 ;
        uint    getgrentr:1 ;
        uint    getgrnamr:1 ;
        uint    getgrgidr:1 ;
        uint    getpjxxxr:1 ;
        uint    getpjgnur:1 ;
        uint    getpjentr:1 ;
        uint    getpjnamr:1 ;
        uint    getpjpidr:1 ;
        uint    getprxxxr:1 ;
        uint    getprgnur:1 ;
        uint    getnwxxxr:1 ;
        uint    getnwgnur:1 ;
        uint    gethoxxxr:1 ;
        uint    gethognur:1 ;
        uint    getsvxxxr:1 ;
        uint    getsvgnur:1 ;
        uint    getloginr:1 ;
        uint    localtimer:1 ;
        uint    gmtimer:1 ;
        uint    ttynamer:1 ;
        uint    openpt:1 ;
        uint    ptmx:1 ;
        uint    pshm:1 ;
        uint    psem:1 ;
        uint    nsem:1 ;
        uint    pmq:1 ;
        uint    audit:1 ;
        uint    getdents:1 ;
        uint    xti:1 ;
        uint    mutexrobust:1 ;
        uint    strnlen:1 ;
        uint    strlcpy:1 ;
        uint    strnstr:1 ;
        uint    sigqueue:1 ;
        uint    sigsend:1 ;
        uint    sigsendset:1 ;
        uint    sigwait:1 ;
        uint    pthreadsigsend:1 ;
        uint    ucontext:1 ;
        uint    getcontext:1 ;
        uint    reltimedwait:1 ;
        uint    memcntl:1 ;
        uint    memplock:1 ;
        uint    gethrtime:1 ;
        uint    getrandom:1 ;
        uint    mkdirp:1 ;
        uint    stime:1 ;
        uint    resolvepath:1 ;
        uint    waitid:1 ;
        uint    pipes:1 ;
        uint    getexecname:1 ;
        uint    libproc:1 ;
	constexpr syshas_mgr() noexcept {
                shadow		= SYSHAS_SHADOW ;
                project		= SYSHAS_PROJECT ;
                statvfs		= SYSHAS_STATVFS ;
                poll		= SYSHAS_POLL ;
                streams		= SYSHAS_STREAMS ;
                timer		= SYSHAS_TIMER ;
                acl		= SYSHAS_ACL ;
                userattr	= SYSHAS_USERATTR ;
                secdb		= SYSHAS_SECDB ;
                setenv		= SYSHAS_SETENV ;
                unsetenv	= SYSHAS_UNSETENV ;
                sysinfo		= SYSHAS_SYSINFO ;
                offset		= SYSHAS_OFFSET ;
                getcwd		= SYSHAS_GETCWD ;
                aio		= SYSHAS_AIO ;
                typeushort	= SYSHAS_TYPEUSHORT ;
                typeuint	= SYSHAS_TYPEUINT ;
                typedirentt	= SYSHAS_TYPEDIRENT ;
                typesigt	= SYSHAS_TYPESIGT ;
                typein6addrt	= SYSHAS_TYPEIN6ADDRT ;
                task		= SYSHAS_TASK ;
                utmpx		= SYSHAS_UTMPX ;
                utmpname	= SYSHAS_UTMPNAME ;
                utmpxname	= SYSHAS_UTMPXNAME ;
                loadavg		= SYSHAS_LOADAVG ;
                loadavgint	= SYSHAS_LOADAVGINT ;
                readdirr	= SYSHAS_READDIRR ;
                getpwxxxr	= SYSHAS_GETPWXXXR ;
                getpwgnur	= SYSHAS_GETPWGNUR ;
                getpwentr	= SYSHAS_GETPWENTR ;
                getpwnamr	= SYSHAS_GETPWNAMR ;
                getpwuidr	= SYSHAS_GETPWUIDR ;
                getspxxxr	= SYSHAS_GETSPXXXR ;
                getspgnur	= SYSHAS_GETSPGNUR ;
                getspentr	= SYSHAS_GETSPENTR ;
                getspnamr	= SYSHAS_GETSPNAMR ;
                getgrxxxr	= SYSHAS_GETGRXXXR ;
                getgrgnur	= SYSHAS_GETGRGNUR ;
                getgrentr	= SYSHAS_GETGRENTR ;
                getgrnamr	= SYSHAS_GETGRNAMR ;
                getgrgidr	= SYSHAS_GETGRGIDR ;
                getpjxxxr	= SYSHAS_GETPJXXXR ;
                getpjgnur	= SYSHAS_GETPJGNUR ;
                getpjentr	= SYSHAS_GETPJENTR ;
                getpjnamr	= SYSHAS_GETPJNAMR ;
                getpjpidr	= SYSHAS_GETPJPIDR ;
                getprxxxr	= SYSHAS_GETPRXXXR ;
                getprgnur	= SYSHAS_GETPRGNUR ;
                getnwxxxr	= SYSHAS_GETNWXXXR ;
                getnwgnur	= SYSHAS_GETNWGNUR ;
                gethoxxxr	= SYSHAS_GETHOXXXR ;
                gethognur	= SYSHAS_GETHOGNUR ;
                getsvxxxr	= SYSHAS_GETSVXXXR ;
                getsvgnur	= SYSHAS_GETSVGNUR ;
                getloginr	= SYSHAS_GETLOGINR ;
                localtimer	= SYSHAS_LOCALTIMER ;
                gmtimer		= SYSHAS_GMTIMER ;
                ttynamer	= SYSHAS_TTYNAMER ;
                openpt		= SYSHAS_OPENPT ;
                ptmx		= SYSHAS_PTMX ;
                pshm		= SYSHAS_PSHM ;
                psem		= SYSHAS_PSEM ;
                nsem		= SYSHAS_NSEM ;
                pmq		= SYSHAS_PMQ ;
                audit		= SYSHAS_AUDIT ;
                getdents	= SYSHAS_GETDENTS ;
                xti		= SYSHAS_XTI ;
                mutexrobust	= SYSHAS_MUTEXROBUST ;
                strnlen		= SYSHAS_STRNLEN ;
                strlcpy		= SYSHAS_STRLCPY ;
                strnstr		= SYSHAS_STRNSTR ;
                sigqueue	= SYSHAS_SIGQUEUE ;
                sigsend		= SYSHAS_SIGSEND ;
                sigsendset	= SYSHAS_SIGSENDSET ;
                sigwait		= SYSHAS_SIGWAIT ;
                pthreadsigsend	= SYSHAS_PTHREADSIGSEND ;
                ucontext	= SYSHAS_UCONTEXT ;
                getcontext	= SYSHAS_GETCONTEXT ;
                reltimedwait	= SYSHAS_RELTIMEDWAIT ;
                memcntl		= SYSHAS_MEMCNTL ;
                memplock	= SYSHAS_MEMPLOCK ;
                gethrtime	= SYSHAS_GETHRTIME ;
                getrandom	= SYSHAS_GETRANDOM ;
                mkdirp		= SYSHAS_MKDIRP ;
                stime		= SYSHAS_STIME ;
                resolvepath	= SYSHAS_RESOLVEPATH ;
                waitid		= SYSHAS_WAITID ;
                pipes		= SYSHAS_PIPES ;
                getexecname	= SYSHAS_GETEXECNAME ;
                libproc		= SYSHAS_LIBPROC ;
	} ; /* end ctor */
} ; /* end struct (syshas_mgr) */

extern const syshas_mgr		syshas ;

#endif /* __cplusplus */


#endif /* SYSHAS_INCLUDE */


