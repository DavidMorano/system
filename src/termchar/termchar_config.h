/* termchar_config HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* revision history:

	= 1998-04-13, David A-D- Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */
/* Use is subject to license terms. */

#ifndef	TERMCHARCONFIG_INCLUDE
#define	TERMCHARCONFIG_INCLUDE


#define	VERSION		"0"
#define	WHATINFO	"@(#)termchar "
#define	BANNER		"Terminal Characters"
#define	SEARCHNAME	"termchar"
#define	VARPRNAME	"LOCAL"

#define	VARPROGRAMROOT1	"TERMCHAR_PROGRAMROOT"
#define	VARPROGRAMROOT2	VARPRNAME
#define	VARPROGRAMROOT3	"PROGRAMROOT"

#define	VARBANNER	"TERMCHAR_BANNER"
#define	VARSEARCHNAME	"TERMCHAR_NAME"
#define	VAROPTS		"TERMCHAR_OPTS"
#define	VARFILEROOT	"TERMCHAR_FILEROOT"
#define	VARLOGTAB	"TERMCHAR_LOGTAB"
#define	VARAFNAME	"TERMCHAR_AF"
#define	VAREFNAME	"TERMCHAR_EF"
#define	VARERRORFNAME	"TERMCHAR_ERRORFILE"

#define	VARDEBUGFNAME	"TERMCHAR_DEBUGFILE"
#define	VARDEBUGFD1	"TERMCHAR_DEBUGFD"
#define	VARDEBUGFD2	"DEBUGFD"

#ifndef	PROGRAMROOT
#define	PROGRAMROOT	"/usr/add-on/local"
#endif

#define	TMPDNAME	"/tmp"
#define	WORKDNAME	"/tmp"

#define	DEFINITFNAME	"/etc/default/init"
#define	DEFLOGFNAME	"/etc/default/login"
#define	NISDOMAINNAME	"/etc/defaultdomain"

#define	CONFIGFNAME	"conf"
#define	ENVFNAME	"environ"
#define	PATHSFNAME	"paths"
#define	HELPFNAME	"help"
#define	IPASSWDFNAME	"ipasswd"

#define	PIDFNAME	"run/termchar"		/* mutex PID file */
#define	LOGFNAME	"var/log/termchar"	/* activity log */
#define	LOCKFNAME	"spool/locks/termchar"	/* lock mutex file */

#define	LOGSIZE		(80*1024)

#define	DEFSIZESPEC	"100000"		/* default target log size */

#define	TO_FILEMOD	(1 * 60 * 60)		/* IPASSWD timeout */


#endif /* TERMCHARCONFIG_INCLUDE */


