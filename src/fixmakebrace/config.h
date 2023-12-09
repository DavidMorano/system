/* config */


/* revision history:

	= 2000-05-14, David A�D� Morano

	Originally written for Rightcore Network Services.


*/

/* Copyright � 2000 David A�D� Morano.  All rights reserved. */


#define	VERSION		"0"
#define	WHATINFO	"@(#)FIXMAKEBRACE "
#define	BANNER		"Fix Makefile Braces"
#define	SEARCHNAME	"fixmakebrace"
#define	VARPRNAME	"LOCAL"

#ifndef	PROGRAMROOT
#define	PROGRAMROOT	"/usr/add-on/local"
#endif

#define	VARPROGRAMROOT1	"FIXMAKEBRACE_PROGRAMROOT"
#define	VARPROGRAMROOT2	VARPRNAME
#define	VARPROGRAMROOT3	"PROGRAMROOT"

#define	VARBANNER	"FIXMAKEBRACE_BANNER"
#define	VARSEARCHNAME	"FIXMAKEBRACE_NAME"
#define	VAROPTS		"FIXMAKEBRACE_OPTS"
#define	VARAFNAME	"FIXMAKEBRACE_AF"
#define	VAREFNAME	"FIXMAKEBRACE_EF"
#define	VARERRORFNAME	"FIXMAKEBRACE_ERRORFILE"

#define	VARDEBUGFNAME	"FIXMAKEBRACE_DEBUGFILE"
#define	VARDEBUGFD1	"FIXMAKEBRACE_DEBUGFD"
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

#define	TMPDNAME	"/tmp"

#define	CONFIGFNAME	"etc/fixmakebrace/conf"
#define	LOGFNAME	"log/fixmakebrace"
#define	HELPFNAME	"help"

#define	PO_SUFFIX	"suffix"
#define	PO_OPTION	"option"


