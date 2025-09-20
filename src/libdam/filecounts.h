/* filecounts HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* manage a file-based counter database */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	FILECOUNTS_INCLUDE
#define	FILECOUNTS_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<time.h>		/* |time_t| */
#include	<fcntl.h>		/* |mode_t| */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>


#define	FILECOUNTS_MAGIC	0x22436893
#define	FILECOUNTS_NUMDIGITS	9	
#define	FILECOUNTS_LOGZLEN	23	/* total (theoretical) length */

#define	FILECOUNTS		struct filecounts_head
#define	FILECOUNTS_N		struct filecounts_name
#define	FILECOUNTS_INFO		struct filecounts_information
#define	FILECOUNTS_CUR		struct filecounts_cursor
#define	FILECOUNTS_II		struct filecounts_iil
#define	FILECOUNTS_FL		struct filecounts_flags


struct filecounts_iil {
	cchar		*name ;
	time_t		utime ;
	int		value ;
} ;

struct filecounts_cursor {
	FILECOUNTS_II	*listp ;	/* list-pointer */
	uint		magic ;
	int		listn ;		/* list-number */
	int		i ;
} ;

struct filecounts_information {
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
	FILECOUNTS_FL	fl ;
	uint		magic ;
	int		fd ;
	int		ncursors ;
} ;

typedef	FILECOUNTS		filecounts ;
typedef	FILECOUNTS_N		filecounts_n ;
typedef	FILECOUNTS_INFO		filecounts_info ;
typedef	FILECOUNTS_CUR		filecounts_cur ;
typedef	FILECOUNTS_II		filecounts_ii ;
typedef	FILECOUNTS_FL		filecounts_fl ;

EXTERNC_begin

extern int filecounts_open(filecounts *,cchar *,int,mode_t) noex ;
extern int filecounts_process(filecounts *,filecounts_n *) noex ;
extern int filecounts_curbegin(filecounts *,filecounts_cur *) noex ;
extern int filecounts_cursnap(filecounts *,filecounts_cur *) noex ;
extern int filecounts_curread(filecounts *,filecounts_cur *,
		filecounts_info *,char *,int) noex ;
extern int filecounts_curend(filecounts *,filecounts_cur *) noex ;
extern int filecounts_close(filecounts *) noex ;

EXTERNC_end


#endif /* FILECOUNTS_INCLUDE */


