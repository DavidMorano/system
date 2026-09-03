/* sysconfcmds HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* system-configuation-commands */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-11, David A-D- Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	sysconfcmds

	Description:
	These are some of the commands to the |sysconf(2)| system
	call that may not actually exist on the current system.
	For those commands that do not exit on the current system
	(this system), they are emulated.

*******************************************************************************/

#ifndef	SYSCONFCMDS_INCLUDE
#define	SYSCONFCMDS_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<unistd.h>		/* POSIX |_SC_xx| */
#include	<stddef.h>		/* CSTD */
#include	<stdlib.h>		/* CSTD */
#include	<clanguage.h>		/* CSTD */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */


enum sysconfcmds {
    sysconfcmd_synthetic = 1000,
    sysconfcmd_maxpid,		/* max PID value */
    sysconfcmd_maxline,
    sysconfcmd_maxlink,		/* file-system hard links */
    sysconfcmd_maxlogin,
    sysconfcmd_maxgroups,
    sysconfcmd_maxsymloop,	/* file-system symbolic (soft) links */
    sysconfcmd_maxsymbol,
    sysconfcmd_maxnodename,	/* UNIX® system nodename length */
    sysconfcmd_maxname,		/* file-system max-name length */
    sysconfcmd_maxpath,		/* file-system max-path length */
    sysconfcmd_maxtzname,	/* time-zone name */
    sysconfcmd_maxtzabbr,	/* time-zone abbreviation */
    sysconfcmd_maxzoneinfo,	/* zone-info *something* */
    sysconfcmd_maxusername,	/* mame system-account user-name length */
    sysconfcmd_maxgroupname,	/* name system-account group-name length */
    sysconfcmd_maxprojectname,	/* name system-account project-name length */
    sysconfcmd_maxprotname,	/* name network protocol-name */
    sysconfcmd_maxnetwname,	/* name network network-name */
    sysconfcmd_maxhostname,	/* name network host-name */
    sysconfcmd_maxservname,	/* name network service-name */
    sysconfcmd_maxmsg,
    sysconfcmd_maxmailaddr,
    sysconfcmd_utent,		/* entry buffer size - UTMP */
    sysconfcmd_pwent,		/* entry buffer size - PASSWD */
    sysconfcmd_spent,		/* entry buffer size - SHADOW */
    sysconfcmd_uaent,		/* entry buffer size - USERATTR */
    sysconfcmd_grent,		/* entry buffer size - GROUP */
    sysconfcmd_pjent,		/* entry buffer size - PROJECT */
    sysconfcmd_prent,		/* entry buffer size - PROTOCOL */
    sysconfcmd_nwent,		/* entry buffer size - NETWORK */
    sysconfcmd_hoent,		/* entry buffer size - HOST */
    sysconfcmd_svent,		/* entry buffer size - SERVICE */
    sysconfcmd_fstype,		/* filesystem-type (c-string length) */
    sysconfcmd_maxaio,		/* no-limit */
    sysconfcmd_maxatexit,	/* no-limit */
    sysconfcmd_maxmqopen,	/* no-limit */
    sysconfcmd_maxthreads,	/* no-limit */
    sysconfcmd_maxkeys,		/* no-limit */
    sysconfcmd_maxdestructors,	/* no-limit */
    sysconfcmd_overlast
} ; /* end enum (sysconfcmds) */


#ifndef	_SC_MAXPID
#define	_SC_MAXPID		sysconfcmd_maxpid
#endif
#ifndef	_SC_LINE_MAX
#define	_SC_LINE_MAX		sysconfcmd_maxline
#endif
#ifndef	_SC_LINK_MAX
#define	_SC_LINK_MAX		sysconfcmd_maxlink
#endif
#ifndef	_SC_LOGIN_NAME_MAX
#define	_SC_LOGIN_NAME_MAX	sysconfcmd_maxlogin
#endif
#ifndef	_SC_NGROUPS_MAX
#define	_SC_NGROUPS_MAX		sysconfcmd_maxgroups
#endif
#ifndef	_SC_SYMLOOP_MAX
#define	_SC_SYMLOOP_MAX		sysconfcmd_maxsymloop
#endif /* _SC_SYMLOOP_MAX */
#ifndef	_SC_SYMBOL_MAX
#define	_SC_SYMBOL_MAX		sysconfcmd_maxsymbol
#endif
#ifndef	_SC_NODENAME_MAX
#define	_SC_NODENAME_MAX	sysconfcmd_maxnodename
#endif /* _SC_NODENAME_MAX */
#ifndef	_SC_NAME_MAX
#define	_SC_NAME_MAX		sysconfcmd_maxname
#endif
#ifndef	_SC_PATH_MAX
#define	_SC_PATH_MAX		sysconfcmd_maxpath
#endif
#ifndef	_SC_TZNAME_MAX
#define	_SC_TZNAME_MAX		sysconfcmd_maxtzname
#endif
#ifndef	_SC_TZABBR_MAX
#define	_SC_TZABBR_MAX		sysconfcmd_maxtzabbr
#endif
#ifndef	_SC_ZONEINFO_MAX
#define	_SC_ZONEINFO_MAX	sysconfcmd_maxzoneinfo
#endif
#ifndef	_SC_USERNAME_MAX
#define	_SC_USERNAME_MAX	sysconfcmd_maxusername
#endif
#ifndef	_SC_GROUPNAME_MAX
#define	_SC_GROUPNAME_MAX	sysconfcmd_maxgroupname
#endif
#ifndef	_SC_PROJECTNAME_MAX
#define	_SC_PROJECTNAME_MAX	sysconfcmd_maxprojectname
#endif

#ifndef	_SC_PROTNAME_MAX	/* network protocol-name */
#define	_SC_PROTNAME_MAX	sysconfcmd_maxprotname
#endif /* _SC_PROTNAME_MAX */

#ifndef	_SC_NETWNAME_MAX	/* network network-name */
#define	_SC_NETWNAME_MAX	sysconfcmd_maxnetwname
#endif /* _SC_PROTNAME_MAX */

#ifndef	_SC_HOSTNAME_MAX	/* network host-name */
#ifdef	_SC_HOST_NAME_MAX	/* really the node-name length */
#define	_SC_HOSTNAME_MAX	_SC_HOST_NAME_MAX
#else
#define	_SC_HOSTNAME_MAX	sysconfcmd_maxhostname
#endif
#endif /* _SC_HOSTNAME_MAX */

#ifndef	_SC_HOST_NAME_MAX	/* really the node-name length */
#define	_SC_HOST_NAME_MAX	sysconfcmd_maxhostname
#endif /* _SC_HOST_NAME_MAX */

#ifndef	_SC_SERVNAME_MAX	/* network service-name */
#define	_SC_SERVNAME_MAX	sysconfcmd_maxservname
#endif /* _SC_SERVNAME_MAX */

#ifndef	_SC_UTMPENT_SIZE_MAX
#define	_SC_UTMPENT_SIZE_MAX	sysconfcmd_utent
#endif
#ifndef	_SC_GETPW_R_SIZE_MAX
#define	_SC_GETPW_R_SIZE_MAX	sysconfcmd_pwent
#endif
#ifndef	_SC_GETSP_R_SIZE_MAX
#define	_SC_GETSP_R_SIZE_MAX	sysconfcmd_spent
#endif
#ifndef	_SC_GETUA_R_SIZE_MAX
#define	_SC_GETUA_R_SIZE_MAX	sysconfcmd_uaent
#endif
#ifndef	_SC_GETGR_R_SIZE_MAX
#define	_SC_GETGR_R_SIZE_MAX	sysconfcmd_grent
#endif
#ifndef	_SC_GETPJ_R_SIZE_MAX
#define	_SC_GETPJ_R_SIZE_MAX	sysconfcmd_pjent
#endif
#ifndef	_SC_GETPR_R_SIZE_MAX
#define	_SC_GETPR_R_SIZE_MAX	sysconfcmd_prent
#endif
#ifndef	_SC_GETNW_R_SIZE_MAX
#define	_SC_GETNW_R_SIZE_MAX	sysconfcmd_nwent
#endif
#ifndef	_SC_GETHO_R_SIZE_MAX
#define	_SC_GETHO_R_SIZE_MAX	sysconfcmd_hoent
#endif
#ifndef	_SC_GETSV_R_SIZE_MAX
#define	_SC_GETSV_R_SIZE_MAX	sysconfcmd_svent
#endif
#ifndef	_SC_FSTYPE
#define	_SC_FSTYPE		sysconfcmd_fstype
#endif
#ifndef	_SC_MSG_MAX
#define	_SC_MSG_MAX		sysconfcmd_maxmsg
#endif
#ifndef	_SC_MAILADDR_MAX
#define	_SC_MAILADDR_MAX	sysconfcmd_maxmailaddr
#endif

/* value requests but with no-limit */
#ifndef	_SC_AIO_MAX
#define	_SC_AIO_MAX		sysconfcmd_maxaio
#endif
#ifndef	_SC_ATEXIT_MAX
#define	_SC_ATEXIT_MAX		sysconfcmd_maxatexit
#endif
#ifndef	_SC_MQ_OPEN_MAX
#define	_SC_MQ_OPEN_MAX		sysconfcmd_maxmqopen
#endif
#ifndef	_SC_THREAD_THREADS_MAX
#define	_SC_THREAD_THREADS_MAX	sysconfcmd_maxthreads
#endif
#ifndef	_SC_THREAD_KEYS_MAX
#define	_SC_THREAD_KEYS_MAX	sysconfcmd_maxkeys
#endif
#ifndef	_SC_THREAD_DESTRUCTOR_ITERATIONS
#define	_SC_THREAD_DESTRUCTOR_ITERATIONS	sysconfcmd_maxdestructors
#endif

/* aliases (since the real one did not follow the typical convention) */
#ifndef	_SC_PID_MAX
#define	_SC_PID_MAX		_SC_MAXPID
#endif
#ifndef	_SC_THREAD_DESTRUCTORS
#define	_SC_THREAD_DESTRUCTORS	_SC_THREAD_DESTRUCTOR_ITERATIONS
#endif


#endif /* SYSCONFCMDS_INCLUDE */


