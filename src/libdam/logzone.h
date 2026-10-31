/* logzone HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* log timezone names */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-01, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	LOGZONE_INCLUDE
#define	LOGZONE_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */


#define	LOGZONE			struct logzone_head
#define	LOGZONE_FL		struct logzone_flags
#define	LOGZONE_ENT		struct logzone_entry
#define	LOGZONE_CUR		struct logzone_cursor
#define	LOGZONE_MAGIC		0x91824563
#define	LOGZONE_ENTLEN		48
#define	LOGZONE_NENTS		100
#define	LOGZONE_ZNAMESIZE	8
#define	LOGZONE_STAMPSIZE	23
#define	LOGZONE_NOZONEOFFSET	(13 * 60)


struct logzone_entry {
	uint		count ;
	short		off ;
	short		znl ;
	char		znb[LOGZONE_ZNAMESIZE + 1] ;
	char		st[LOGZONE_STAMPSIZE + 1] ;
} ; /* end struct */

struct logzone_flags {
	uint		writable:1 ;
	uint		lockedread:1 ;
	uint		lockedwrite:1 ;
	uint		cursor:1 ;		/* cursor-locked ? */
	uint		cursorlockbroken:1 ;	/* cursor lock broken */
	uint		cursoracc:1 ;		/* accessed while cursored ? */
	uint		remote:1 ;		/* remote mounted file */
} ; /* end struct */

struct logzone_head {
	cchar		*fname ;
	char		*bufp ;
	time_t		opentime ;		/* file open time */
	time_t		accesstime ;		/* file access time */
	time_t		timod ;			/* file modification time */
	off_t		fsize ;
	LOGZONE_FL	fl ;
	uint		magval ;
	int		oflags ;
	int		pagesz ;
	int		bufl ;
	int		fd ;
	mode_t		operms ;
} ; /* end struct */

struct logzone_cursor {
	int		i ;
} ; /* end struct */

enum logzonemems {
	logzonemem_finish,
	logzonemem_overlast
} ; /* end enum (logzonemems) */

#ifdef	__cplusplus
struct logzone ;
struct logzone_co {
	logzone		*op = nullptr ;
	int		w = -1 ;
	void operator () (logzone *p,int m) noex {
	    op = p ;
	    w = m ;
	} ; /* end */
	operator int () noex ;
	int operator () () noex { 
	    return operator int () ;
	} ; /* end */
} ; /* end struct (logzone_co) */


typedef LOGZONE		logzone ;



typedef	LOGZONE_ENT	logzone_ent ;
typedef	LOGZONE_CUR	logzone_cur ;
typedef	LOGZONE_FL	logzone_fl ;

EXTERNC_begin
extern int logzone_open(logzone *,cchar *,int,mode_t) noex ;
extern int logzone_curbegin(logzone *,logzone_cur *) noex ;
extern int logzone_curend(logzone *,logzone_cur *) noex ;
extern int logzone_curenum(logzone *,logzone_cur *,logzone_ent *) noex ;
extern int logzone_match(logzone *,cchar *,int,int,logzone_ent *) noex ;
extern int logzone_update(logzone *,cchar *,int,int,cchar *) noex ;
extern int logzone_check(logzone *,time_t) noex ;
extern int logzone_close(logzone *) noex ;
EXTERNC_end


#endif /* LOGZONE_INCLUDE */


