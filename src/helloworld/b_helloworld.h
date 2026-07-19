/* b_helloworld HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */


/* Korn-Shell built-in |helloworld| */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services (RNS).

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */


#define	VERSION		"0a"
#define	WHATINFO	"@(#)HELLOWORLD "
#define	BANNER		"HelloWorld"
#define	SEARCHNAME	"helloworld"
#define	VARPRNAME	"LOCAL"

#ifndef	PROGRAMROOT
#define	PROGRAMROOT	"/usr/add-on/local"
#endif

#define	VARPROGRAMROOT1	"HELLOWORLD_PROGRAMROOT"
#define	VARPROGRAMROOT2	VARPRNAME
#define	VARPROGRAMROOT3	"PROGRAMROOT"

#define	VARBANNER	"HELLOWORLD_BANNER"
#define	VARSEARCHNAME	"HELLOWORLD_NAME"
#define	VAROPTS		"HELLOWORLD_OPTS"
#define	VARFTYPES	"HELLOWORLD_FTYPES"
#define	VARSUFREQ	"HELLOWORLD_SUFREQ"
#define	VARSA		"HELLOWORLD_SUFACC"
#define	VARSR		"HELLOWORLD_SUFREJ"
#define	VARPRUNE	"HELLOWORLD_PRUNE"
#define	VARTARDNAME	"HELLOWORLD_TARDIR"
#define	VARRFNAME	"HELLOWORLD_RF"
#define	VARAFNAME	"HELLOWORLD_AF"
#define	VAREFNAME	"HELLOWORLD_EF"

#define	VARDEBUGLEVEL	"HELLOWORLD_DEBUGLEVEL"
#define	VARDEBUGFNAME	"HELLOWORLD_DEBUGFILE"
#define	VARDEBUGFD1	"HELLOWORLD_DEBUGFD"
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

#define	CONFIGFNAME	"etc/hello/conf"
#define	LOGFNAME	"log/hello"
#define	HELPFNAME	"help"

#define	PO_OPTION	"option"
#define	PO_FTS		"fts"			/* file-types */
#define	PO_SUFREQ	"sufreq"		/* suffix-required */
#define	PO_SUFREJ	"sufacc"		/* suffix-reject */
#define	PO_SUFACC	"sufrej"		/* suffix-accept */
#define	PO_PRUNE	"prune"			/* prune components */
#define	PO_TARDIRS	"tardirs"


