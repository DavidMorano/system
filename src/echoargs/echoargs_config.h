/* echoargs_config HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* ECHOARGS program */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-13, David A-D- Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */
/* Use is subject to license terms. */

#ifndef	ECHOARGSCONFIG_INCLUDE
#define	ECHOARGSCONFIG_INCLUDE


#define	VERSION		"0c"
#define	WHATINFO	"@(#)echoargs "

#define	VARPROGRAMROOT1	"ECHOARGS_PROGRAMROOT"
#define	VARPROGRAMROOT2	"LOCAL"
#define	VARPROGRAMROOT3	"PROGRAMROOT"

#define	VARBANNER	"ECHOARGS_BANNER"
#define	VARSEARCHNAME	"ECHOARGS_NAME"

#define	VARFILEROOT	"ECHOARGS_FILEROOT"
#define	VARLOGTAB	"ECHOARGS_LOGTAB"

#define	NODEVAR		"NODE"
#define	CLUSTERVAR	"CLUSTER"
#define	SYSTEMVAR	"SYSTEM"
#define	PRINTERVAR	"PRINTER"

#define	VARDEBUGFD1	"ECHOARGS_DEBUGFD"
#define	VARDEBUGFD2	"DEBUGFD"

#ifndef	PROGRAMROOT
#define	PROGRAMROOT	"/usr/add-on/local"
#endif

#define	SEARCHNAME	"echoargs"

#define	CONFIGFNAME	"conf"
#define	ENVFNAME	"environ"
#define	PATHSFNAME	"paths"
#define	HELPFNAME	"help"
#define	IPASSWDFNAME	"ipasswd"
#define	FULLFNAME	".fullname"

#define	PIDFNAME	"run/echoargs"		/* mutex PID file */
#define	LOGFNAME	"var/log/echoargs"	/* activity log */
#define	LOCKFNAME	"spool/locks/echoargs"	/* lock mutex file */

#define	WORKDNAME	"/tmp"
#define	TMPDNAME	"/tmp"

#define	DEFINITFNAME	"/etc/default/init"
#define	DEFLOGFNAME	"/etc/default/login"
#define	NISDOMAINNAME	"/etc/defaultdomain"

#define	LOGSIZE		(80*1024)

#define	BANNER		"Echo Arguments"

#define	DEFSIZESPEC	"100000"		/* default target log size */

#define	TO_FILEMOD	(1 * 60 * 60)		/* IPASSWD timeout */


#endif /* ECHOARGSCONFIF_INCLUDE */


