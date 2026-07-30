/* holidays HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* access for the HOLIDAYS database */
/* version %I% last-modified %G% */


/* revision history:

	= 2008-10-01, David A­D­ Morano
	This module was originally written.

*/

/* Copyright © 2008 David A­D­ Morano.  All rights reserved. */

#ifndef	HOLIDAYS_INCLUDE
#define	HOLIDAYS_INCLUDE


#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<sys/types.h>		/* POSIX */
#include	<time.h>		/* POSIX |time_t| */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */


#define	HOLIDAYS	struct holidays_head
#define	HOLIDAYS_OBJ	struct holidays_object
#define	HOLIDAYS_CITE	struct holidays_query
#define	HOLIDAYS_CUR	struct holidays_cursor
#define	HOLIDAYS_MAGIC	0x63328183


struct holidays_object {
	cchar		*name ;
	uint		objsz ;
	uint		cursz ;
} ; /* end struct */

struct holidays_query {
	ushort		y ;
	uchar		m, d ;
} ; /* end struct */

struct holidays_cursor {
	uint		chash ;
	int		i ;
} ; /* end struct */

struct holidays_head {
	cchar		*pr ;
	cchar		*fname ;
	uint		(*rt)[3] ;	/* the records (sorted by m-d) */
	int		(*kit)[3] ;	/* key-index-table */
	char		*kst ;		/* key string-table */
	char		*vst ;		/* val string-table */
	time_t		ti_check ;
	time_t		ti_mtime ;
	uint		magval ;
	int		year ;
	int		kslen ;
	int		vslen ;
	int		rtlen ;
	int		itlen ;
	int		ncursors ;
} ; /* end struct */

typedef	HOLIDAYS	holidays ;
typedef	HOLIDAYS_OBJ	holidays_obj ;
typedef	HOLIDAYS_CITE	holidays_cite ;
typedef	HOLIDAYS_CUR	holidays_cur ;

EXTERNC_begin

extern int holidays_open(holidays *,cchar *,int,cchar *) noex ;
extern int holidays_count(holidays *) noex ;
extern int holidays_curbegin(holidays *,holidays_cur *) noex ;
extern int holidays_curend(holidays *,holidays_cur *) noex ;
extern int holidays_curenum(holidays *,holidays_cur *,
		holidays_cite *,char *,int) noex ;
extern int holidays_fetchcite(holidays *,holidays_cite *,holidays_cur *,
		char *,int) noex ;
extern int holidays_fetchname(holidays *,cchar *,int,holidays_cur *,
		holidays_cite *,char *,int) noex ;
extern int holidays_check(holidays *,time_t) noex ;
extern int holidays_audit(holidays *) noex ;
extern int holidays_close(holidays *) noex ;

#ifdef	COMMENT
extern int holidays_lookday(holidays *,holidays_cur *,holidays_cite *) noex ;
extern int holidays_lookname(holidays *,holidays_cur *,cchar *,int) noex ;
extern int holidays_read(holidays *,holidays_cur *,
			holidays_cite *,char *,int) noex ;
#endif

EXTERNC_end

extern const holidays_obj	holidays_modinfo ;


#endif /* HOLIDAYS_INCLUDE */


