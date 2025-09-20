/* pingstatdb */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	PINGSTATDB_INCLUDE
#define	PINGSTATDB_INCLUDE	1


#include	<envstandards.h>

#include	<sys/types.h>
#include	<sys/timeb.h>
#include	<netdb.h>

#include	<bfile.h>
#include	<vecitem.h>
#include	<dater.h>
#include	<localmisc.h>


/* object defines */

#define	PINGSTATDB_MAGIC	0x31415926
#define	PINGSTATDB		struct pingstatdb_head
#define	PINGSTATDB_CUR		struct pingstatdb_cursor
#define	PINGSTATDB_ENT		struct pingstatdb_entry
#define	PINGSTATDB_UP		struct pingstatdb_upper
#define	PINGSTATDB_FL		struct pingstatdb_flags


struct pingstatdb_flags {
	uint		readlocked:1 ;		/* file is read locked */
	uint		writelocked:1 ;		/* file is write locked */
	uint		cached:1 ;		/* file is cached */
	uint		writable:1 ;		/* file is writable */
	uint		cursor:1 ;		/* cursor is out */
	uint		tzset:1 ;		/* was tzset() called */
} ;

struct pingstatdb_head {
	cchar		*fname ;
	char		*zname ;
	bfile		pfile ;
	vecitem		entries ;
	TIMEB		now ;
	time_t		mtime ;
	PINGSTATDB_FL	fl ;
	uint		magic ;
} ;

struct pingstatdb_cursor {
	int		i ;
} ;

struct pingstatdb_entry {
	time_t		ti_change ;	/* last change */
	time_t		ti_ping ;	/* last ping */
	uint		count ;
	int		f_up ;		/* UP-DOWN status */
	char		hostname[MAXHOSTNAMELEN+1] ;
} ;

struct pingstatdb_upper {
	uint		timestamp ;
	uint		timechange ;
	uint		count ;
} ;


EXTERNC_begin

extern int pingstatdb_open(pingstatdb *,cchar *,mode_t,int) noex ;
extern int pingstatdb_close(pingstatdb *) noex ;
extern int pingstatdb_match(pingstatdb *,cchar *,pingstatdb_ent *) noex ;
extern int pingstatdb_curbegin(pingstatdb *,pingstatdb_cur *) noex ;
extern int pingstatdb_curend(pingstatdb *,pingstatdb_cur *) noex ;
extern int pingstatdb_enum(pingstatdb *,pingstatdb_cur *,
		pingstatdb_ent *) noex ;
extern int pingstatdb_update(pingstatdb *,cchar *,int,time_t) noex ;
extern int pingstatdb_uptime(pingstatdb *,cchar *,pingstatdb_up *) noex ;
extern int pingstatdb_check(pingstatdb *,time_t) noex ;

EXTERNC_end


#endif /* PINGSTATDB_INCLUDE */


