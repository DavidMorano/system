/* syshas SUPPORT */
/* charset=ISO8859-1 */
/* lang=C20 */

/* specify what features each operating system has */
/* version %I% last-modified %G% */


/* revision history:

	= 2000-05-14, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2000,2017,2023 David A­D­ Morano.  All rights reserved. */

#include	<envstandards.h>	/* must be ordered first to configure */
#include	<syshas.h>


	constexpr syshas_mgr::syshas_mgr() noexcept {
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
                sigwaitinfo	= SYSHAS_SIGWAITINFO ;
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
	} /* end ctor (syshas_mgr::syshas_mgr) */

constexpr syshas_mgr		syshas ;


