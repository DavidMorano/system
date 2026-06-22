/* cci_config HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* program to correct indentations */
/* version %I% last-modified %G% */


/* revision history:

	= 1985-02-12, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1985 David A­D­ Morano.  All rights reserved. */

#ifndef	CCICONFIG_INCLUDE
#define	CCICONFIG_INCLUDE


#define	VERSION		"0"
#define	WHATINFO	"@(#)CCI "
#define	SEARCHNAME	"cci"
#define	BANNER		"CC Indent"

#define	VARPROGRAMROOT1	"CCI_PROGRAMROOT"
#define	VARPROGRAMROOT2	"HOME"
#define	VARPROGRAMROOT3	"PROGRAMROOT"
#define	VARBANNER	"CCI_BANNER"
#define	VARSEARCHNAME	"CCI_NAME"
#define	VAROPTS		"CCI_OPTS"
#define	VARERRORFNAME	"CCI_ERRORFILE"
#define	VARAFNAME	"CCI_AF"
#define	VAREFNAME	"CCI_EF"

#define	VARDEBUGFNAME	"CCI_DEBUGFILE"
#define	VARDEBUGFD1	"CCI_DEBUGFD"
#define	VARDEBUGFD2	"DEBUGFD"

#ifndef	PROGRAMROOT
#define	PROGRAMROOT	"/usr/add-on/local"
#endif

#define	TMPDNAME	"/tmp"

#define	LOGFNAME	"log/cci"
#define	USERFNAME	"log/cci.users"
#define	CMDHELPFNAME	"lib/cci/cmdhelp"


#endif /* CCICONFIG_INCLUDE */


