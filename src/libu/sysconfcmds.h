/* sysconfcmds HEADER */
/* encoding=ISO8859-1 */
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
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>


enum sysconfcmds {
    sysconfcmd_synthetic = 1000,
    sysconfcmd_maxline,
    sysconfcmd_maxlogin,
    sysconfcmd_maxsymbol,
    sysconfcmd_maxtzname,
    sysconfcmd_maxzoneinfo,
    sysconfcmd_maxname,
    sysconfcmd_maxpath,
    sysconfcmd_maxusername,
    sysconfcmd_maxgroupname,
    sysconfcmd_maxnodename,
    sysconfcmd_maxhostname,
    sysconfcmd_maxmailaddr,
    sysconfcmd_pwent,
    sysconfcmd_uaent,
    sysconfcmd_grent,
    sysconfcmd_pjent,
    sysconfcmd_fstype,
    sysconfcmd_overlast
} ; /* end enum (sysconfcmds) */


#ifndef	_SC_LINE_MAX
#define	_SC_LINE_MAX		sysconfcmd_maxline
#endif
#ifndef	_SC_LOGIN_NAME_MAX
#define	_SC_LOGIN_NAME_MAX	sysconfcmd_maxlogin
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
#ifndef	_SC_HOST_NAME_MAX	/* really the node-name length */
#define	_SC_HOST_NAME_MAX	sysconfcmd_maxnodename
#endif
#ifndef	_SC_NODENAME_MAX
#ifdef	_SC_HOST_NAME_MAX	/* really the node-name length */
#define	_SC_NODENAME_MAX	_SC_HOST_NAME_MAX
#else
#define	_SC_NODENAME_MAX	sysconfcmd_maxnodename
#endif
#endif /* _SC_NODENAME_MAX */
#ifndef	_SC_HOSTNAME_MAX
#define	_SC_HOSTNAME_MAX	sysconfcmd_maxhostname
#endif
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
#ifndef	_SC_MAILADDR_MAX
#define	_SC_MAILADDR_MAX	sysconfcmd_maxmailaddr
#endif


#endif /* SYSCONFCMDS_INCLUDE */


