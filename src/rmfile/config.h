/* config */


#define	VERSION		"0"
#define	WHATINFO	"@(#)rmfile "
#define	SEARCHNAME	"rmfile"
#define	BANNER		"Remove File"
#define	VARPRNAME	"LOCAL"

#define	VARPROGRAMROOT1	"RMFILE_PROGRAMROOT"
#define	VARPROGRAMROOT2	VARPRNAME
#define	VARPROGRAMROOT3	"PROGRAMROOT"

#define	VARBANNER	"RMFILE_BANNER"
#define	VARSEARCHNAME	"RMFILE_NAME"
#define	VARERRFILE	"TMFILE_ERRFILE"
#define	VARFILEROOT	"RMFILE_FILEROOT"
#define	VARLOGTAB	"RMFILE_LOGTAB"
#define	VARERRORFNAME	"RMFILE_ERRORFILE"
#define	VARLFNAME	"RMFILE_EL"
#define	VAREFNAME	"RMFILE_EF"

#define	VARDEBUGFNAME	"RMFILE_DEBUGFILE"
#define	VARDEBUGFD1	"RMFILE_DEBUGFD"
#define	VARDEBUGFD2	"DEBUGFD"

#define	VARNODE		"NODE"
#define	VARSYSNAME	"SYSNAME"
#define	VARRELEASE	"RELEASE"
#define	VARMACHINE	"MACHINE"
#define	VARARCHITECTURE	"ARCHITECTURE"
#define	VARCLUSTER	"CLUSTER"
#define	VARSYSTEM	"SYSTEM"
#define	VARNISDOMAIN	"NISDOMAIN"
#define	VARTERM		"TERM"
#define	VARPRINTER	"PRINTER"
#define	VARLPDEST	"LPDEST"
#define	VARPAGER	"PAGER"
#define	VARMAIL		"MAIL"
#define	VARORGANIZATION	"ORGANIZATION"
#define	VARLINES	"LINES"
#define	VARCOLUMNS	"COLUMNS"
#define	VARNAME		"NAME"
#define	VARFULLNAME	"FULLNAME"

#define	VARHOMEDNAME	"HOME"
#define	VARTMPDNAME	"TMPDIR"
#define	VARMAILDNAME	"MAILDIR"
#define	VARMAILDNAMES	"MAILDIRS"

#define	VARPRLOCAL	"LOCAL"
#define	VARPRPCS	"PCS"

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

#define	PIDFNAME	"run/rmfile"		/* mutex PID file */
#define	LOGFNAME	"var/log/rmfile"	/* activity log */
#define	LOCKFNAME	"spool/locks/rmfile"	/* lock mutex file */

#define	LOGSIZE		(80*1024)

#define	DEFSIZESPEC	"100000"		/* default target log size */



