/* cal_config HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* CAL program */
/* version %I% last-modified %G% */


/* revision history:

	= 2000-05-14, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2000 David A­D­ Morano.  All rights reserved. */

#ifndef	CALCONFIG_INCLUDE
#define	CALCONFIG_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<stddef.h>		/* POSIX® */
#include	<stdlib.h>		/* POSIX® */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */


#define	VERSION		"0"
#define	WHATINFO	"@(#)CAL "
#define	BANNER		"Calendar Month"
#define	SEARCHNAME	"cal"
#define	VARPRNAME	"LOCAL"

#ifndef	PROGRAMROOT
#define	PROGRAMROOT	"/usr/add-on/local"
#endif

#define	VARPROGRAMROOT1	"CAL_PROGRAMROOT"
#define	VARPROGRAMROOT2	VARPRNAME
#define	VARPROGRAMROOT3	"PROGRAMROOT"

#define	VARBANNER	"CAL_BANNER"
#define	VARSEARCHNAME	"CAL_NAME"
#define	VARPROGCAL	"CAL_PROGCAL"
#define	VAROPTS		"CAL_OPTS"
#define	VARAFNAME	"CAL_AF"
#define	VAREFNAME	"CAL_EF"

#define	VARDEBUGFNAME	"CAL_DEBUGFILE"
#define	VARDEBUGFD1	"CAL_DEBUGFD"
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
#define	LOGCNAME	"log"

#define	CONFFNAME	"conf"
#define	LOGFNAME	"log/cal"
#define	HELPFNAME	"help"

#define	PROG_CAL	"/usr/bin/cal"
#define	PROG_CALNAME	"cal"

#define	PO_OPTION	"option"

#define	LOGSIZE		(80*1024)


#endif /* CALCONFIG_INCLUDE */


