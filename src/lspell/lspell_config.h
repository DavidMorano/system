/* lspell_config HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* Local-Spell-Checker */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services (RNS).

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	LSPELLCONFIG_INCLUDE
#define	LSPELLCONFIG_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */


#define	VERSION		"0"
#define	WHATINFO	"@(#)lspell "
#define	BANNER		"Local Spell"
#define	SEARCHNAME	"lspell"
#define	VARPRNAME	"LOCAL"

#ifndef	PROGRAMROOT
#define	PROGRAMROOT	"/usr/add-on/local"
#endif

#define	VARPROGRAMROOT1	"LSPELL_PROGRAMROOT"
#define	VARPROGRAMROOT2	VARPRNAME
#define	VARPROGRAMROOT3	"PROGRAMROOT"

#define	VARBANNER	"LSPELL_BANNER"
#define	VARSEARCHNAME	"LSPELL_NAME"
#define	VAROPTS		"LSPELL_OPTS"
#define	VARFILEROOT	"LSPELL_FILEROOT"
#define	VARLOGTAB	"LSPELL_LOGTAB"
#define	VARAFNAME	"LSPELL_AF"
#define	VAREFNAME	"LSPELL_EF"

#define	VARDEBUGFNAME	"LSPELL_DEBUGFILE"
#define	VARDEBUGFD1	"LSPELL_DEBUGFD"
#define	VARDEBUGFD2	"DEBUGFD"

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
#define	IPASSWDFNAME	"ipasswd"

#define	PIDFNAME	"run/lspell"		/* mutex PID file */
#define	LOGFNAME	"var/log/lspell"	/* activity log */
#define	LOCKFNAME	"spool/locks/lspell"	/* lock mutex file */

#define	LOGSIZE		(80*1024)

#define	DEFSIZESPEC	"100000"		/* default target log size */


#endif /* LSPELLCONFIG_INCLUDE */


