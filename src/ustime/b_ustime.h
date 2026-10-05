/* ustime_config HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* UNIX® Standard-Time */
/* version %I% last-modified %G% */

#ifndef	USTIMECONFIG_INCLUDE
#define	USTIMECONFIG_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */
#include	<usysrets.h>		/* LIBU */


#define	VERSION		"0"
#define	WHATINFO	"@(#)ustime "
#define	BANNER		"UNIX® Standard-Time"
#define	SEARCHNAME	"utime"
#define	VARPRNAME	"LOCAL"

#ifndef	PROGRAMROOT
#define	PROGRAMROOT	"/usr/add-on/local"
#endif

#define	VARPROGRAMROOT1	"USTIME_PROGRAMROOT"
#define	VARPROGRAMROOT2	VARPRNAME
#define	VARPROGRAMROOT3	"PROGRAMROOT"

#define	VARBANNER	"USTIME_BANNER"
#define	VARSEARCHNAME	"USTIME_NAME"
#define	VAROPTS		"USTIME_OPTS"
#define	VARFILEROOT	"USTIME_FILEROOT"
#define	VARLOGTAB	"USTIME_LOGTAB"
#define	VARMSFNAME	"USTIME_MSFILE"
#define	VARERRORFNAME	"USTIME_ERRORFILE"

#define	VARDEBUGFNAME	"USTIME_DEBUGFILE"
#define	VARDEBUGFD1	"USTIME_DEBUGFD"
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

#define	DEFINITFNAME	"/etc/default/init"
#define	DEFLOGFNAME	"/etc/default/login"
#define	NISDOMAINNAME	"/etc/defaultdomain"

#define	CONFIGFNAME	"conf"
#define	ENVFNAME	"environ"
#define	PATHSFNAME	"paths"
#define	HELPFNAME	"help"

#define	PIDFNAME	"run/utime"		/* mutex PID file */
#define	LOGFNAME	"var/log/utime"		/* activity log */
#define	LOCKFNAME	"spool/locks/utime"	/* lock mutex file */
#define	MSFNAME		"ms"

#define	LOGSIZE		(80*1024)

#define	DEFSIZESPEC	"100000"		/* default target log size */

#define	DEFRUNINT	60
#define	DEFPOLLINT	8
#define	DEFNODES	50

#define	USAGECOLS	4


#endif /* USTIMECONFIG_INCLUDE */


