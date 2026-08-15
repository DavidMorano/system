/* calyears HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* CALYEARS object implementation */
/* version %I% last-modified %G% */


/* revision history:

	- 2008-10-01, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2008 David A­D­ Morano.  All rights reserved. */

#ifndef	CALYEARS_INCLUDE
#define	CALYEARS_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<vechand.h>		/* LIBUC */
#include	<calent.h>		/* LIBDAM */
#include	<calcite.h>		/* LIBDAM */
#include	<holidayer.h>		/* LIBDAM */
#include	<vecobj.h>		/* LIBDAM */


#define	CALYEARS	struct calyears_head
#define	CALYEARS_OBJ	struct calyears_object
#define	CALYEARS_Q	calcite
#define	CALYEARS_QUERY	calcite
#define	CALYEARS_CITE	calcite
#define	CALYEARS_CUR	struct calyears_cursor
#define	CALYEARS_FL	struct calyears_flags
#define	CALYEARS_MAGIC	0x99447245


struct calyears_object {
	cchar		*name ;
	uint		objsz ;
	uint		cursz ;
} ; /* end struct */

struct calyears_cursor {
	void		*results ;
	uint		nresults ;
	uint		magval ;
	int		i ;
} ; /* end struct */

struct calyears_flags {
	uint		doms:1 ;		/* day-of-month(s) */
	uint		hols:1 ;		/* holidays */
} ; /* end struct */

struct calyears_head {
	cchar		*pr ;
	cchar		*tmpdname ;
	char		*a ;			/* memory-allocation */
	holidayer	*holp ;
	vechand		*domp ;
	vechand		*calp ;			/* calendars */
	CALYEARS_FL	fl ;
	CALYEARS_FL	init ;
	CALYEARS_FL	open ;
	uint		magval ;
	int		nentries ;
	int		ncursors ;
	int		year ;			/* current year */
	int		isdst ;			/* current is-dst */
	int		gmtoff ;		/* current offset from GMT */
} ; /* end struct */

typedef	CALYEARS	calyears ;
typedef	CALYEARS_FL	calyears_fl ;
typedef	CALYEARS_OBJ	calyears_obj ;
typedef	CALYEARS_CUR	calyears_cur ;
typedef	CALYEARS_Q	calyears_q ;

EXTERNC_begin

extern int calyears_open(calyears *,cchar *,cchar **,cchar **) noex ;
extern int calyears_count(calyears *) noex ;
extern int calyears_curbegin(calyears *,calyears_cur *) noex ;
extern int calyears_curlook(calyears *,calyears_cur *,con calyears_q *) noex ;
extern int calyears_curread(calyears *,calyears_cur *,mut calyears_q *,
		char *,int) noex ;
extern int calyears_curend(calyears *,calyears_cur *) noex ;
extern int calyears_check(calyears *,time_t) noex ;
extern int calyears_audit(calyears *) noex ;
extern int calyears_close(calyears *) noex ;
extern int calyears_already(calyears *,vecobj *,calent *) noex ;
extern int calyears_havestart(calyears *,calyears_q *,int,cchar *,int) noex ;

EXTERNC_end

#ifdef	__cplusplus
extern const calyears_obj	calyears_modinfo ;
#endif /* __cplusplus */


#endif /* CALYEARS_INCLUDE */


