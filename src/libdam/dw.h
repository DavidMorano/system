/* dw HEADER */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* directory watch */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	DW_INCLUDE
#define	DW_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* system types */
#include	<sys/stat.h>		/* |dev_t| + |ino_t| */
#include	<time.h>		/* |time_t| */
#include	<stdlib.h>		/* |size_t| */
#include	<vecstr.h>
#include	<vecobj.h>


/* object defines */
#define	DW_MAGIC	0x31415926
#define	DW		struct dw_head
#define	DW_FL		struct dw_flags
#define	DW_ENT		struct dw_entry
#define	DW_CUR		struct dw_cursor
/* values */
#define	DW_INTCHECK	20		/* default check time (seconds) */

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
} ;

struct dw_entry {
	cchar		*name ;
	time_t		itime ;
	time_t		timod ;
	size_t		fsize ;
	int		state ;
} ;

struct dw_ientry {
	cchar		*name ;
	time_t		itime ;
	time_t		timod ;
	size_t		fsize ;
	int		state ;
} ;

struct dw_flags {
	uint		subdirs:1 ;
} ;

EXTERNC_begin
struct dw_head {
	vecobj		*elp ;			/* directory entries */
	cchar		*dirname ;		/* directory path */
	void		(*callback)(DW_ENT *,int,void *) noex ;
	cvoid		*argp ;
	time_t		tiopen ;		/* time FD was cached */
	time_t		timod ;			/* directory mod-time */
	time_t		ticheck ;		/* time last checked */
	time_t		tiremove ;		/* last checked for removed */
	DW_FL		f ;
	uint		magic ;
	int		intcheck ;		/* file check interval */
	int		fd ;			/* cached directory FD */
	int		count_new ;
	int		count_checkable ;
} ; /* end struct (dw_head) */
EXTERNC_end

struct dw_cursor {
	int		i ;
} ;

typedef	DW		dw ;
typedef	DW_FL		dw_flags ;
typedef	DW_ENT		dw_ent ;
typedef	DW_CUR		dw_cur ;

EXTERNC_begin

extern int dw_start(dw *,cchar *) noex ;
extern int dw_finish(dw *) noex ;
extern int dw_find(dw *,cchar *,dw_ent *,char *,int) noex ;
extern int dw_del(dw *,dw_cur *) noex ;
extern int dw_check(dw *,time_t) noex ;
extern int dw_curbegin(dw *,dw_cur *) noex ;
extern int dw_curend(dw *,dw_cur *) noex ;
extern int dw_curenum(dw *,dw_cur *,dw_ent *,char *,int) noex ;
extern int dw_curenumcheck(dw *,dw_cur *,dw_ent *,char *,int) noex ;
extern int dw_state(dw *,int,int) noex ;

EXTERNC_end

/* returns */
#define	DW_SRESERVED	dwstate_reserved
#define	DW_SNEW		dwstate_new
#define	DW_SCHECK	dwstate_check		/* ready for validation check */
#define	DW_SREADY	dwstate_ready
#define	DW_SNOTSUP	dwstate_notdup
#define	DW_SRUNNING	dwstate_running
#define	DW_SRETRY	dwstate_retry
#define	DW_SUSER	dwstate_user


#endif /* DW_INCLUDE */


