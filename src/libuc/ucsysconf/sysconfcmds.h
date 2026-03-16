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
#include	<unistd.h>		/* |_SC_xx| */
#include	<stddef.h>		/* |nullptr_t| */
#include	<stdlib.h>
#include	<clanguage.h>
#include	<usysbase.h>


enum sysconfcmds {
    sysconfcmd_synthetic = 1000,
    sysconfcmd_maxpid,
    sysconfcmd_maxline,
    sysconfcmd_maxlogin,
    sysconfcmd_maxgroups,
    sysconfcmd_maxsymbol,
    sysconfcmd_maxtzname,
    sysconfcmd_maxzoneinfo,
    sysconfcmd_maxname,
    sysconfcmd_maxpath,
    sysconfcmd_maxusername,
    sysconfcmd_maxgroupname,
    sysconfcmd_maxprojectname,
    sysconfcmd_maxnodename,
    sysconfcmd_maxprotname,
    sysconfcmd_maxhostname,
    sysconfcmd_maxservname,
    sysconfcmd_maxmsg,
    sysconfcmd_maxmailaddr,
    sysconfcmd_pwent,
    sysconfcmd_uaent,
    sysconfcmd_grent,
    sysconfcmd_pjent,
    sysconfcmd_fstype,
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
#ifndef	_SC_LOGIN_NAME_MAX
#define	_SC_LOGIN_NAME_MAX	sysconfcmd_maxlogin
#endif
#ifndef	_SC_NGROUPS_MAX
#define	_SC_NGROUPS_MAX		sysconfcmd_maxgroups
#endif
#ifndef	_SC_NAME_MAX
#define	_SC_NAME_MAX		sysconfcmd_maxname
#endif
#ifndef	_SC_PATH_MAX
#define	_SC_PATH_MAX		sysconfcmd_maxpath
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

#ifndef	_SC_NODENAME_MAX
#define	_SC_NODENAME_MAX	sysconfcmd_maxnodename
#endif /* _SC_NODENAME_MAX */

#ifndef	_SC_PROTNAME_MAX	/* network protocol-name */
#define	_SC_PROTNAME_MAX	sysconfcmd_maxprotname
#endif /* _SC_PROTNAME_MAX */

#ifndef	_SC_HOSTNAME_MAX
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

#ifndef	_SC_GETPW_R_SIZE_MAX
#define	_SC_GETPW_R_SIZE_MAX	sysconfcmd_pwent
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
#ifndef	_SC_FSTYPE
#define	_SC_FSTYPE		sysconfcmd_fstype
#endif
#ifndef	_SC_SYMBOL_MAX
#define	_SC_SYMBOL_MAX		sysconfcmd_maxsymbol
#endif
#ifndef	_SC_TZNAME_MAX
#define	_SC_TZNAME_MAX		sysconfcmd_maxtzname
#endif
#ifndef	_SC_ZONEINFO_MAX
#define	_SC_ZONEINFO_MAX	sysconfcmd_maxzoneinfo
#endif
#ifndef	_SC_MSG_MAX
#define	_SC_MSG_MAX		sysconfcmd_maxmsg
#endif
#ifndef	_SC_MAILADDR_MAX
#define	_SC_MAILADDR_MAX	sysconfcmd_maxmailaddr
#endif

/* value requests but with no-limit */
#ifndef	_SC_AIO_MAX
#define	_SC_AIO_MAX		 sysconfcmd_maxaio
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


