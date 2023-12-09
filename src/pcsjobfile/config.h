/* config */


#define	VERSION		"0a"
#define	WHATINFO	"@(#)PCSJOBFILE "
#define	BANNER		"PCS JobFile"
#define	SEARCHNAME	"pcsjobfile"

#define	VARPROGRAMROOT1	"PCSJOBFILE_PROGRAMROOT"
#define	VARPROGRAMROOT2	"PCS"
#define	VARPROGRAMROOT3	"PROGRAMROOT"

#define	VARBANNER	"PCSJOBFILE_BANNER"
#define	VARSEARCHNAME	"PCSJOBFILE_NAME"
#define	VARJOBDNAME	"PCSJOBFILE_DNAME"
#define	VARERRORFNAME	"PCSJOBFILE_ERRORFILE"

#define	VARDEBUGFNAME	"PCSJOBFILE_DEBUGFILE"
#define	VARDEBUGFD1	"PCSJOBFILE_DEBUGFD"
#define	VARDEBUGFD2	"DEBUGFD"

#define	VARSYSNAME	"SYSNAME"
#define	VARRELEASE	"RELEASE"
#define	VARMACHINE	"MACHINE"
#define	VARARCHITECTURE	"ARCHITECTURE"
#define	VARHZ		"HZ"
#define	VARNODE		"NODE"
#define	VARCLUSTER	"CLUSTER"
#define	VARSYSTEM	"SYSTEM"
#define	VARDOMAIN	"DOMAIN"
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
#define	VARTZ		"TZ"
#define	VARUSERNAME	"USERNAME"
#define	VARLOGNAME	"LOGNAME"

#define	VARHOMEDNAME	"HOME"
#define	VARTMPDNAME	"TMPDIR"
#define	VARMAILDNAME	"MAILDIR"
#define	VARMAILDNAMES	"MAILDIRS"

#define	VARPRLOCAL	"LOCAL"
#define	VARPRPCS	"PCS"

#ifndef	PROGRAMROOT
#define	PROGRAMROOT	"/usr/add-on/pcs"
#endif

#define	TMPDNAME	"/tmp"

#define	MKJOB1DNAME	"var/spool/locks"
#define	MKJOB2DNAME	"/tmp/pcs/locks"
#define	MKJOB3DNAME	"/tmp/locks"

#define	LOGFNAME	"log"
#define	HELPFNAME	"help"

#define	TRIES		2		/* tries of 'mkjobfile()' */
#define	WAITTIME	3		/* time to wait if 'mkjobfile' fails */
#define	JOBTIME		(5 * 60)	/* default time job file stays around */
#define	JOBFILEMODE	0664		/* job file creation mode */



