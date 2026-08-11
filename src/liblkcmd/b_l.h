/* l_config HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* this is a SHELL built-in version of |l(1)| */
/* version %I% last-modified %G% */


/* revision history:

	= 2000-05-14, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2000 David A­D­ Morano.  All rights reserved. */

#ifndef	LCONFIG_INCLUDE
#define	LCONFIG_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */


#define	VERSION		"0"
#define	WHATINFO	"@(#)l "
#define	BANNER		"List files"
#define	SEARCHNAME	"l"
#define	VARPRNAME	"LOCAL"

#ifndef	PROGRAMROOT
#define	PROGRAMROOT	"/usr/add-on/local"
#endif

#define	VARPROGRAMROOT1	"L_PROGRAMROOT"
#define	VARPROGRAMROOT2	VARPRNAME
#define	VARPROGRAMROOT3	"PROGRAMROOT"

#define	VARBANNER	"L_BANNER"
#define	VARSEARCHNAME	"L_NAME"
#define	VAROPTS		"L_OPTS"
#define	VARFILEROOT	"L_FILEROOT"
#define	VARLOGTAB	"L_LOGTAB"
#define	VARAFNAME	"L_AF"
#define	VAREFNAME	"L_EF"
#define	VAROFNAME	"L_OF"
#define	VARIFNAME	"L_IF"

#define	VARDEBUGFNAME	"L_DEBUGFILE"
#define	VARDEBUGFD1	"L_DEBUGFD"
#define	VARDEBUGFD2	"DEBUGFD"

#define	VARNODE		"NODE"
#define	VARSYSNAME	"SYSNAME"
#define	VARRELEASE	"RELEASE"
#define	VARMACHINE	"MACHINE"
#define	VARARCHITECTURE	"ARCHITECTURE"
#define	VARCLUSTER	"CLUSTER"
#define	VARSYSTEM	"SYSTEM"
#define	VARNISDOMAIN	"NISDOMAIN"
#define	VARPRINTER	"PRINTER"

#define	VARTMPDNAME	"TMPDIR"

#define	VARPRLOCAL	"LOCAL"
#define	VARPRPCS	"PCS"

#define	TMPDNAME	"/tmp"
#define	WORKDNAME	"/tmp"
#define	LOGCNAME	"log"

#define	DEFINITFNAME	"/etc/default/init"
#define	DEFLOGFNAME	"/etc/default/login"
#define	NISDOMAINNAME	"/etc/defaultdomain"

#define	CONFIGFNAME	"conf"
#define	ENVFNAME	"environ"
#define	PATHSFNAME	"paths"
#define	HELPFNAME	"help"

#define	PIDFNAME	"run/l"			/* mutex PID file */
#define	LOGFNAME	"var/log/l"		/* activity log */
#define	LOCKFNAME	"spool/locks/l"		/* lock mutex file */


#endif /* LCONFIG_INCLUDE */


