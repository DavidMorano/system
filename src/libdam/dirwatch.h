/* dirwatch HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* directory watch */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	DIRWATCH_INCLUDE
#define	DIRWATCH_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* POSIX system types */
#include	<sys/stat.h>		/* POSIX */
#include	<time.h>		/* CSTD |time_t| */
#include	<stdlib.h>		/* CSTD |size_t| */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<vecstr.h>		/* LIBUC */
#include	<vecobj.h>		/* LIBUC */


#define	DIRWATCH		struct dw_head
#define	DIRWATCH_FL		struct dw_flags
#define	DIRWATCH_ENT		struct dw_entry
#define	DIRWATCH_CUR		struct dw_cursor
#define	DIRWATCH_MAGIC		0x31415926
#define	DIRWATCH_INTCK		20		/* default check time */


enum dwstates {
    	dwstate_reserved,
    	dwstate_new,
    	dwstate_check,
    	dwstate_ready,
    	dwstate_notsup,
    	dwstate_running,
    	dwstate_retry,
    	dwstate_user,
	dwstate_overlast
} ; /* end enum */

struct dw_entry {
	cchar		*name ;
	time_t		itime ;
	time_t		timod ;
	size_t		fsize ;
	int		state ;
} ; /* end struct */

struct dw_flags {
	uint		subdirs:1 ;
} ; /* end struct */

EXTERNC_begin
struct dw_head {
	vecobj		*elp ;			/* directory entries */
	cchar		*dirname ;		/* directory path */
	cvoid		*argp ;
	time_t		tiopen ;		/* time FD was cached */
	time_t		timod ;			/* directory mod-time */
	time_t		ticheck ;		/* time last checked */
	time_t		tiremove ;		/* last checked for removed */
	DIRWATCH_FL	fl ;
	uint		magval ;
	int		intcheck ;		/* file check interval */
	int		fd ;			/* cached directory FD */
	int		count_new ;
	int		count_checkable ;
} ; /* end struct (dw_head) */
EXTERNC_end

struct dw_cursor {
	int		i ;
} ; /* end struct */

typedef	DIRWATCH		dirwatch ;
typedef	DIRWATCH_FL		dirwatch_fl ;
typedef	DIRWATCH_ENT		dirwatch_ent ;
typedef	DIRWATCH_CUR		dirwatch_cur ;

EXTERNC_begin

extern int dirwatch_start	(dirwatch *,cchar *,int) noex ;
extern int dirwatch_finish	(dirwatch *) noex ;
extern int dirwatch_find	(dirwatch *,cchar *,dirwatch_ent *,
					char *,int) noex ;
extern int dirwatch_del		(dirwatch *,dirwatch_cur *) noex ;
extern int dirwatch_check	(dirwatch *,time_t) noex ;
extern int dirwatch_curbegin	(dirwatch *,dirwatch_cur *) noex ;
extern int dirwatch_curend	(dirwatch *,dirwatch_cur *) noex ;
extern int dirwatch_curenum	(dirwatch *,dirwatch_cur *,dirwatch_ent *,
					char *,int) noex ;
extern int dirwatch_curenumck	(dirwatch *,dirwatch_cur *,dirwatch_ent *,
					char *,int) noex ;
extern int dirwatch_state	(dirwatch *,int,int) noex ;

EXTERNC_end

/* returns */
#define	DIRWATCH_SRESERVED	dwstate_reserved
#define	DIRWATCH_SNEW		dwstate_new
#define	DIRWATCH_SCHECK		dwstate_check	/* ready for validation check */
#define	DIRWATCH_SREADY		dwstate_ready
#define	DIRWATCH_SNOTSUP	dwstate_notdup
#define	DIRWATCH_SRUNNING	dwstate_running
#define	DIRWATCH_SRETRY		dwstate_retry
#define	DIRWATCH_SUSER		dwstate_user


#endif /* DIRWATCH_INCLUDE */


