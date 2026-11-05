/* tliaddr_config HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* TLIADDR program */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services (RNS).

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	TLIADDRCONFIG_INCLUDE
#define	TLIADDRCONFIG_INCLUDE


#define	VERSION		"0a"
#define	WHATINFO	"@(#)TLIADDR "
#define	BANNER		"TLI Address"
#define	SEARCHNAME	"tliaddr"
#define	VARPRNAME	"LOCAL"

#ifndef	PROGRAMROOT
#define	PROGRAMROOT	"/usr/add-on/local"
#endif

#define	VARPROGRAMROOT1	"TLIADDR_PROGRAMROOT"
#define	VARPROGRAMROOT2	VARPRNAME
#define	VARPROGRAMROOT3	"PROGRAMROOT"

#define	VARBANNER	"TLIADDR_BANNER"
#define	VARSEARCHNAME	"TLIADDR_NAME"
#define	VAROPTS		"TLIADDR_OPTS"
#define	VARAFNAME	"TLIADDR_AF"
#define	VAREFNAME	"TLIADDR_EF"
#define	VARERRORFNAME	"TLIADDR_ERRORFILE"

#define	VARDEBUGFNAME	"TLIADDR_DEBUGFILE"
#define	VARDEBUGFD1	"TLIADDR_DEBUGFD"
#define	VARDEBUGFD2	"DEBUGFD"

#define	VARHOMEDNAME	"HOME"
#define	VARTMPDNAME	"TMPDIR"
#define	VARMAILDNAME	"MAILDIR"
#define	VARMAILDNAMES	"MAILDIRS"

#define	VARPRLOCAL	"LOCAL"
#define	VARPRPCS	"PCS"

#define	LOGFNAME	"log/tliaddr"
#define	USERFNAME	"log/tliaddr.users"
#define	HELPFNAME	"help"

#define	DEFINTERFACE	"socket"
#define	DEFSOCKFAMILY	"unix"

#define	INETPORT_LISTEN		"listen"
#define	INETPORTNUM_LISTEN	2766


#endif /* TLIADDRCONFIG_INCLUDE */


