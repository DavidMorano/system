/* networks_config HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* networks-up word program */
/* version %I% last-modified %G% */


/* revision history:

	= 2000-05-14, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2000 David A­D­ Morano.  All rights reserved. */

#ifndef	NETWORKSCONFIG_INCLUDE
#define	NETWORKSCONFIG_INCLUDE


#define	VERSION		"0a"
#define	WHATINFO	"@(#)networks "
#define	BANNER		"Look"
#define	SEARCHNAME	"networks"
#define	VARPRNAME	"LOCAL"

#ifndef	PROGRAMROOT
#define	PROGRAMROOT	"/usr/add-on/local"
#endif

#define	VARPROGRAMROOT1	"NETWORKS_PROGRAMROOT"
#define	VARPROGRAMROOT2	VARPRNAME
#define	VARPROGRAMROOT3	"PROGRAMROOT"

#define	VARBANNER	"NETWORKS_BANNER"
#define	VARSEARCHNAME	"NETWORKS_NAME"
#define	VAROPTS		"NETWORKS_OPTS"
#define	VARWORDS	"NETWORKS_WORDS"
#define	VARFILEROOT	"NETWORKS_FILEROOT"
#define	VARAFNAME	"NETWORKS_AF"
#define	VAREFNAME	"NETWORKS_EF"
#define	VARLFNAME	"NETWORKS_LF"
#define	VARERRORFNAME	"NETWORKS_ERRORFILE"

#define	VARDEBUGFNAME	"NETWORKS_DEBUGFILE"
#define	VARDEBUGFD1	"NETWORKS_DEBUGFD"
#define	VARDEBUGFD2	"DEBUGFD"

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

#define	PIDFNAME	"run/networks"		/* mutex PID file */
#define	LOGFNAME	"var/log/networks"	/* activity log */
#define	LOCKFNAME	"spool/locks/networks"	/* lock mutex file */


#endif /* NETWORKSCONFIG_INCLUDE */


