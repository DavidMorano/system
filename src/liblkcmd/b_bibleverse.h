/* bibleverse_config HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* retrieve bible verses by citation */
/* version %I% last-modified %G% */


/* revision history:

	= 2000-05-14, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2000 David A­D­ Morano.  All rights reserved. */

#ifndef	BIBLEVERSECONFIG_INCLUDE
#define	BIBLEVERSECONFIG_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */


#define	VERSION		"0"
#define	WHATINFO	"@(#)bibleverse "
#define	BANNER		"Bible Verse"
#define	SEARCHNAME	"bibleverse"
#define	VARPRNAME	"LOCAL"

#ifndef	PROGRAMROOT
#define	PROGRAMROOT	"/usr/add-on/local"
#endif

#define	VARPROGRAMROOT1	"BIBLEVERSE_PROGRAMROOT"
#define	VARPROGRAMROOT2	VARPRNAME
#define	VARPROGRAMROOT3	"PROGRAMROOT"

#define	VARBANNER	"BIBLEVERSE_BANNER"
#define	VARSEARCHNAME	"BIBLEVERSE_NAME"
#define	VARFILEROOT	"BIBLEVERSE_FILEROOT"
#define	VARLOGTAB	"BIBLEVERSE_LOGTAB"
#define	VAROPTS		"BIBLEVERSE_OPTS"
#define	VARNDB		"BIBLEVERSE_NDB"
#define	VARVDB		"BIBLEVERSE_VDB"
#define	VARSDB		"BIBLEVERSE_SDB"	/* "structure" DB */
#define	VARPDB		"BIBLEVERSE_PDB"
#define	VARLINELEN	"BIBLEVERSE_LINELEN"
#define	VARAFNAME	"BIBLEVERSE_AF"
#define	VAREFNAME	"BIBLEVERSE_EF"

#define	VARDEBUGFNAME	"BIBLEVERSE_DEBUGFILE"
#define	VARDEBUGFD1	"BIBLEVERSE_DEBUGFD"
#define	VARDEBUGFD2	"DEBUGFD"

#define	VARSYSNAME	"SYSNAME"
#define	VARRELEASE	"RELEASE"
#define	VARMACHINE	"MACHINE"
#define	VARARCHITECTURE	"ARCHITECTURE"
#define	VARHZ		"HZ"
#define	VARNODE		"NODE"
#define	VARCLUSTER	"CLUSTER"
#define	VARSYSTEM	"SYSTEM"
#define	VARNISDOMAIN	"NISDOMAIN"
#define	VARPRINTER	"PRINTER"
#define	VARLINES	"LINES"
#define	VARCOLUMNS	"COLUMNS"
#define	VARUSERNAME	"USERNAME"
#define	VARLOGNAME	"LOGNAME"
#define	VARTZ		"TZ"
#define	VARNAME		"NAME"
#define	VARFULLNAME	"FULLNAME"

#define	VARTMPDNAME	"TMPDIR"

#define	VARPRLOCAL	"LOCAL"
#define	VARPRPCS	"PCS"

#define	TMPDNAME	"/tmp"
#define	WORKDNAME	"/tmp"
#define	LOGCNAME	"log"

#define	DEFLOGFNAME	"/etc/default/login"
#define	DEFINITFNAME	"/etc/default/init"
#define	NISDOMAINNAME	"/etc/defaultdomain"

#define	CONFIGFNAME	"conf"
#define	HELPFNAME	"help"

#define	PIDFNAME	"run/bibleverse"
#define	LOGFNAME	"var/log/bibleverse"
#define	LOCKFNAME	"spool/locks/bibleverse"

#define	NDBNAME		"english"	/* default name-db */
#define	VDBNAME		"av"		/* default verse-db */
#define	SDBNAME		"bibles"	/* default structure-db */
#define	PDBNAME		"default"	/* default paragraph-db (automatic) */

#define	LOGSIZE		(80*1024)

#define	MAXDBENTRIES	32000		/* hack: assume as max DB entries */
#define	DEFPRECISION	5		/* default precision numbers */

#define	TO_MKWAIT	(1 * 50)	/* make-db timeout */

#define	OPT_INDENT	1		/* value */
#define	OPT_BOOKNAME	true		/* boolean */
#define	OPT_DEFNULL	true		/* boolean */
#define	OPT_SEPARATE	false		/* boolean */
#define	OPT_TRYCACHE	true		/* boolean */


#endif /* BIBLEVERSECONFIG_INCLUDE */


