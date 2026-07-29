/* showfieldterms_config HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 */


/* revision history:

	= 2000-05-14, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2000 David A­D­ Morano.  All rights reserved. */

#ifndef	SHOWFIELDTERMSCONFIG_INCLUDE
#define	SHOWFIELDTERMSCONFIG_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */
#include	<usysrets.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */


#define	VERSION		"0"
#define	WHATINFO	"@(#)SHOWFIELDTERMS "
#define	BANNER		"Show Field Terms"
#define	SEARCHNAME	"showfieldterms"

#ifndef	PROGRAMROOT
#define	PROGRAMROOT	"/usr/add-on/local"
#endif

#define	VARPROGRAMROOT1	"SHOWFIELDTERMS_PROGRAMROOT"
#define	VARPROGRAMROOT2	"LOCAL"
#define	VARPROGRAMROOT3	"PROGRAMROOT"

#define	VARBANNER	"SHOWFIELDTERMS_BANNER"
#define	VARSEARCHNAME	"SHOWFIELDTERMS_NAME"
#define	VAROPTS		"SHOWFIELDTERMS_OPTS"
#define	VAREFNAME	"SHOWFIELDTERMS_EF"
#define	VARERRORFNAME	"SHOWFIELDTERMS_ERRORFILE"

#define	VARDEBUGFNAME	"SHOWFIELDTERMS_DEBUGFILE"
#define	VARDEBUGFD1	"SHOWFIELDTERMS_DEBUGFD"
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

#define	HELPFNAME	"help"
#define	CONFNAME	"etc/showfieldterms/conf"
#define	LOGFNAME	"log/showfieldterms"


#endif /* SHOWFIELDTERMSCONFIG_INCLUDE */


