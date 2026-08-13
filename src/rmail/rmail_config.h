/* rmail_config HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* mail delivery agent */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-05-01, David A­D­ Morano
	This code module was completely rewritten to replace the
	previous mail-delivery program for PCS, written around 1990
	or so.

*/

/* Copyright © 1998,2004 David A­D­ Morano.  All rights reserved. */

#ifndef	RMAILCONFIG_INCLUDE
#define	RMAILCONFIG_INCLUDE


#define	P_RMAIL		1

#define	VERSION		"0"
#define	WHATINFO	"@(#)rmail "
#define	BANNER		"Remote Mail"

#define	VARPROGRAMROOT1	"RMAIL_PROGRAMROOT"
#define	VARPROGRAMROOT2	"PCS"
#define	VARPROGRAMROOT3	"PROGRAMROOT"

#define	VARSEARCHNAME	"RMAIL_NAME"
#define	VARPROGMODE	"RMAIL_MODE"
#define	VAROPTS		"RMAIL_OPTS"
#define	VARMAILBOX	"RMAIL_MAILBOX"

#define	VARDEBUGFD1	"RMAIL_DEBUGFD"
#define	VARDEBUGFD2	"DEBUGFD"

#ifndef	PROGRAMROOT
#define	PROGRAMROOT	"/usr/add-on/pcs"
#endif

#define	SEARCHNAME	"rmail"

#define	TMPDNAME	"/tmp"
#define SPOOLDNAME	"/var/mail"

#define	HELPFNAME	"help"
#define	MBFNAME		"mbtab"
#define	WLFNAME		"whitelist"
#define	BLFNAME		"blacklist"

#define	SERIALFNAME	"var/serial"
#define	COMSATFNAME	"etc/rmail.nodes"
#define	SPAMFNAME	"etc/rmail.spam"
#define	LOGFNAME	"log/rmail"
#define	USERFNAME	"log/rmail.users"
#define	LOGFNAME	"log/rmail"
#define	LOGENVFNAME	"log/rmail.env"
#define	LOGZONEFNAME	"log/rmail.zones"
#define	MSGIDDBNAME	"var/rmail"

#define	MAILGNAME	"mail"
#define	MAILGID		6

#define	DIVERTUSER	"adm"

#define	LINELEN		256
#define	FIELDLEN	4096

#define	LOGSIZE		(80*1024)

#define	MAILLOCKAGE	(5 * 60)

#define	DEFTIMEOUT	(10 * 60)
#define	TO_LOCK		(10 * 60)
#define	TO_MSGREAD	10

#define	MAXMSGID	490

#define	PORTSPEC_COMSAT		"biff"
#define	PORTSPEC_MAILPOLL	"mailpoll"


#endif /* RMAILCONFIG_INCLUDE */


