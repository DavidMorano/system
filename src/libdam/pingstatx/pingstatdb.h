/* pingstatdb HEADER (Ping-Status-Database) */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* object to manipulate a PINGSTATDB file */
/* version %I% last-modified %G% */

/* revision history:

	= 1998-04-13, David A-D- Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	PINGSTATDB_INCLUDE
#define	PINGSTATDB_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>		/* POSIX® */
#include	<sys/timeb.h>		/* POSIX® */
#include	<netdb.h>		/* POSIX® */
#include	<vechand.h>		/* LIBUC */
#include	<bfile.h>		/* LIBB */


#define	PINGSTATDB		struct pingstatdb_head
#define	PINGSTATDB_FL		struct pingstatdb_flags
#define	PINGSTATDB_ENT		struct pingstatdb_entry
#define	PINGSTATDB_CUR		struct pingstatdb_cursor
#define	PINGSTATDB_UP		struct pingstatdb_upper
#define	PINGSTATDB_MAGIC	0x31415926


struct pingstatdb_flags {
	uint		readlocked:1 ;		/* file is read locked */
	uint		writelocked:1 ;		/* file is write locked */
	uint		cached:1 ;		/* file is cached */
	uint		writable:1 ;		/* file is writable */
	uint		cursor:1 ;		/* cursor is out */
	uint		tzset:1 ;		/* was tzset() called */
} ; /* end struct */

struct pingstatdb_head {
	bfile		*pfp ;		/* ??-file-pointer */
	vechand		*rlp ;		/* record-list-pointer */
	TIMEB		*nowp ;
	cchar		*fname ;
	cchar		*znbuf ;	/* zone-name buffer */
	time_t		mtime ;
	PINGSTATDB_FL	fl ;
	uint		magval ;
} ; /* end struct */

struct pingstatdb_cursor {
	int		i ;
} ; /* end struct */

struct pingstatdb_entry {
	time_t		ti_change ;	/* last change */
	time_t		ti_ping ;	/* last ping */
	uint		cnt ;
	int		f_up ;		/* UP-DOWN status */
	char		hostbuf[MAXHOSTNAMELEN+1] ;
} ; /* end struct */

struct pingstatdb_upper {
	uint		timestamp ;
	uint		timechange ;
	int		count ;
} ; /* end struct */

typedef	PINGSTATDB		pingstatdb ;
typedef	PINGSTATDB_FL		pingstatdb_fl ;
typedef	PINGSTATDB_ENT		pingstatdb_ent ;
typedef	PINGSTATDB_CUR		pingstatdb_cur ;
typedef	PINGSTATDB_UP		pingstatdb_up ;

EXTERNC_begin

extern int pingstatdb_open(pingstatdb *,cchar *,int,mode_t) noex ;
extern int pingstatdb_close(pingstatdb *) noex ;
extern int pingstatdb_match(pingstatdb *,cchar *,pingstatdb_ent *) noex ;
extern int pingstatdb_curbegin(pingstatdb *,pingstatdb_cur *) noex ;
extern int pingstatdb_curend(pingstatdb *,pingstatdb_cur *) noex ;
extern int pingstatdb_curenum(pingstatdb *,pingstatdb_cur *,
		pingstatdb_ent *) noex ;
extern int pingstatdb_update(pingstatdb *,cchar *,int,time_t) noex ;
extern int pingstatdb_uptime(pingstatdb *,cchar *,pingstatdb_up *) noex ;
extern int pingstatdb_check(pingstatdb *,time_t) noex ;

EXTERNC_end


#endif /* PINGSTATDB_INCLUDE */


