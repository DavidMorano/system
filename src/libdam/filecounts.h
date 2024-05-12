/* filecounts HEADERS */
/* lang=C20 */

/* manage a file-based counter database */
/* version %I% last-modified %G% */


/* Copyright © 2008 David A­D­ Morano.  All rights reserved. */

#ifndef	FILECOUNTS_INCLUDE
#define	FILECOUNTS_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<localmisc.h>


#define	FILECOUNTS_MAGIC	0x22436893
#define	FILECOUNTS_NUMDIGITS	9	
#define	FILECOUNTS_LOGZLEN	23	/* total (theoretical) length */

#define	FILECOUNTS		struct filecounts_head
#define	FILECOUNTS_N		struct filecounts_name
#define	FILECOUNTS_INFO		struct filecounts_infomation
#define	FILECOUNTS_CUR		struct filecounts_cursor
#define	FILECOUNTS_II		struct filecounts_iil
#define	FILECOUNTS_FL		struct filecounts_flags


struct filecounts_iil {
	cchar		*name ;
	time_t		utime ;
	int		value ;
} ;

struct filecounts_cursor {
	uint		magic ;
	FILECOUNTS_II	*list ;
	int		nlist ;
	int		i ;
} ;

struct filecounts_infomation {
	time_t		utime ;		/* last update time */
	int		value ;		/* counter previous value */
} ;

struct filecounts_name {
	cchar		*name ;		/* counter name */
	int		value ;		/* counter previous value */
} ;

struct filecounts_flags {
	uint		rdonly:1 ;
} ;

struct filecounts_head {
	cchar		*fname ;	/* file-name */
	FILECOUNTS_FL	f ;
	uint		magic ;
	int		fd ;
	int		ncursors ;
} ;

extern	FILECOUNTS		filecounts ;
extern	FILECOUNTS_N		filecounts_n ;
extern	FILECOUNTS_INFO		filecounts_info ;
extern	FILECOUNTS_CUR		filecounts_cur ;
extern	FILECOUNTS_II		filecounts_ii ;
extern	FILECOUNTS_FL		filecounts_fl ;

EXTERNC_begin

extern int filecounts_open(FILECOUNTS *,cchar *,int,mode_t) noex ;
extern int filecounts_process(FILECOUNTS *,FILECOUNTS_N *) noex ;
extern int filecounts_curbegin(FILECOUNTS *,FILECOUNTS_CUR *) noex ;
extern int filecounts_snap(FILECOUNTS *,FILECOUNTS_CUR *) noex ;
extern int filecounts_read(FILECOUNTS *,FILECOUNTS_CUR *,
		FILECOUNTS_INFO *,char *,int) noex ;
extern int filecounts_curend(FILECOUNTS *,FILECOUNTS_CUR *) noex ;
extern int filecounts_close(FILECOUNTS *) noex ;

EXTERNC_end


#endif /* FILECOUNTS_INCLUDE */


