/* þ_progcheck HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* check C-language programs for some simple problems */
/* version %I% last-modified %G% */


/* revision history:

	= 2000-05-14, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2000 David A­D­ Morano.  All rights reserved. */

#ifndef	PROGCHECKCONFIG_INCLUDE
#define	PROGCHECKCONFIG_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<stddef.h>		/* POSIX® */
#include	<stdlib.h>		/* POSIX® */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */


#define	VERSION		"0"
#define	WHATINFO	"@(#)progcheck "
#define	BANNER		"Program Check"
#define	SEARCHNAME	"progcheck"
#define	VARPRNAME	"LOCAL"

#ifndef	PROGRAMROOT
#define	PROGRAMROOT	"/usr/add-on/local"
#endif

#define	VARPROGRAMROOT1	"PROGCHECK_PROGRAMROOT"
#define	VARPROGRAMROOT2	VARPRNAME
#define	VARPROGRAMROOT3	"PROGRAMROOT"

#define	VARBANNER	"PROGCHECK_BANNER"
#define	VARSEARCHNAME	"PROGCHECK_NAME"
#define	VAROPTS		"PROGCHECK_OPTS"
#define	VARFILEROOT	"PROGCHECK_FILEROOT"
#define	VARLOGTAB	"PROGCHECK_LOGTAB"
#define	VARAFNAME	"PROGCHECK_AF"
#define	VAREFNAME	"PROGCHECK_EF"
#define	VARERRORFNAME	"PROGCHECK_ERRORFILE"

#define	VARDEBUGFNAME	"PROGCHECK_DEBUGFILE"
#define	VARDEBUGFD1	"PROGCHECK_DEBUGFD"
#define	VARDEBUGFD2	"DEBUGFD"

#ifndef	VARHOMEDNAME
#define	VARHOMEDNAME	"HOME"
#endif
#ifndef	VARTMPDNAME
#define	VARTMPDNAME	"TMPDIR"
#endif
#ifndef	VARMAILDNAME
#define	VARMAILDNAME	"MAILDIR"
#endif
#ifndef	VARMAILDNAMES
#define	VARMAILDNAMES	"MAILDIRS"
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

#define	PIDFNAME	"run/progcheck"		/* mutex PID file */
#define	LOGFNAME	"var/log/progcheck"	/* activity log */
#define	LOCKFNAME	"spool/locks/progcheck"	/* lock mutex file */

#define	LOGSIZE		(80*1024)

#define	TO_FILEMOD	(1 * 60 * 60)		/* IPASSWD timeout */


#endif /* PROGCHECKCONFIG_INCLUDE */


