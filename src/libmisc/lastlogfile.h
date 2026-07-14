/* lastlogfile HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* manage reading or writing a LASTLOG file */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	LASTLOGFILE_INCLUDE
#define	LASTLOGFILE_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<utmpx.h>		/* POSIX® */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */


#define	LASTLOGFILE		struct lastlogfile_head
#define	LASTLOGFILE_CUR		struct lastlogfile_cursor
#define	LASTLOGFILE_ENT		struct lastlogx
#define	LASTLOGFILE_MAGIC	0x10923874
#define	LASTLOGFILE_FILEPATH	"/var/adm/lastlog"
#define	LASTLOGFILE_LHOST	16
#define	LASTLOGFILE_LLINE	8


struct lastlogfile_cursor {
	int		i ;
} ; /* end struct */

struct lastlogfile_head {
	cchar		*fname ;	/* stored file name */
	off_t		fsize ;		/* file size */
	time_t		otime ;		/* open time (for FD caching) */
	time_t		mtime ;		/* last modification time */
	uint		magval ;
	int		pagesz ;
	int		oflags ;	/* open flags */
	int		fd ;		/* file descriptor */
} ; /* end struct */

typedef	LASTLOGFILE	lastlogfile ;
typedef	LASTLOGFILE_CUR	lastlogfile_cur ;
typedef	LASTLOGFILE_ENT	lastlogfile_ent ;

EXTERNC_begin

extern int lastlogfile_open(lastlogfile *,cchar *,int) noex ;
extern int lastlogfile_rdinfo(lastlogfile *,uid_t,time_t *,
		char *,char *) noex ;
extern int lastlogfile_wrinfo(lastlogfile *,uid_t,time_t,
		cchar *,cchar *) noex ;
extern int lastlogfile_rdent(lastlogfile *,uid_t,lastlogfile_ent *) noex ;
extern int lastlogfile_wrent(lastlogfile *,uid_t,lastlogfile_ent *) noex ;
extern int lastlogfile_check(lastlogfile *,time_t) noex ;
extern int lastlogfile_curbegin(lastlogfile *,lastlogfile_cur *) noex ;
extern int lastlogfile_curend(lastlogfile *,lastlogfile_cur *) noex ;
extern int lastlogfile_curenum(lastlogfile *,lastlogfile_cur *,
		uid_t *,time_t *,char *,char *) noex ;
extern int lastlogfile_close(lastlogfile *) noex ;

EXTERNC_end


#endif /* LASTLOGFILE_INCLUDE */


