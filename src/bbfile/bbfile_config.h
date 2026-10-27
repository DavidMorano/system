/* bbfile_config HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* test program */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-13, David A-D- Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */
/* Use is subject to license terms. */

#ifndef	BBFILECONFIG_INCLUDE
#define	BBFILECONFIG_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<stddef.h>		/* POSIX® */
#include	<stdlib.h>		/* POSIX® */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */


#define	VERSION		"0"
#define	WHATINFO	"@(#)bbfile "
#define	SEARCHNAME	"bbfile"

#define	VARPROGRAMROOT1	"BBFILE_PROGRAMROOT"
#define	VARPROGRAMROOT2	"PCS"
#define	VARPROGRAMROOT3	"PROGRAMROOT"

#define	VAROPTS		"BBFILE_OPTS"

#define	VARDEBUGFD1	"BBFILE_DEBUGFD"
#define	VARDEBUGFD2	"DEBUGFD"

#ifndef	PROGRAMROOT
#define	PROGRAMROOT	"/usr/add-on/pcs"
#endif

#define	LOGFNAME	"log/bbfile"
#define	USERFNAME	"log/bbfile.users"
#define	NEWSDIR		"spool/boards"

#ifndef	TMPDIR
#define	TMPDIR		"/tmp"
#endif


#endif /* BBFILECONFIG_INCLUDE */


