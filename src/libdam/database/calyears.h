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


#include	<envstandards.h>
#include	<sys/types.h>
#include	<vechand.h>
#include	<vecobj.h>
#include	<holidayer.h>
#include	<localmisc.h>
#include	"calent.h"
#include	"calcite.h"


#define	CALYEARS_MAGIC	0x99447245
#define	CALYEARS	struct calyears_head
#define	CALYEARS_OBJ	struct calyears_object
#define	CALYEARS_Q	CALCITE
#define	CALYEARS_QUERY	CALCITE
#define	CALYEARS_CITE	CALCITE
#define	CALYEARS_CUR	struct calyears_cursor
#define	CALYEARS_FL	struct calyears_flags


struct calyears_object {
	cchar		*name ;
	uint		objsize ;
	uint		cursize ;
} ;

struct calyears_cursor {
	uint		magic ;
	void		*results ;
	uint		nresults ;
	int		i ;
} ;

struct calyears_flags {
	uint		doms:1 ;		/* day-of-month(s) */
	uint		hols:1 ;		/* holidays */
} ;

struct calyears_head {
	cchar		*a ;			/* memory-allocation */
	cchar		*pr ;
	cchar		*tmpdname ;
	HOLIDAYER	hols ;
	vechand		doms ;
	vechand		cals ;			/* calendars */
	CALYEARS_FL	fl ;
	CALYEARS_FL	init, open ;
	uint		magic ;
	int		nentries ;
	int		ncursors ;
	int		year ;			/* current year */
	int		isdst ;			/* current is-dst */
	int		gmtoff ;		/* current offset from GMT */
} ;

typedef	CALYEARS	calyears ;
typedef	CALYEARS_FL	calyears_fl ;
typedef	CALYEARS_OBJ	calyears_obj ;
typedef	CALYEARS_CUR	calyears_cur ;
typedef	CALYEARS_Q	calyears_q ;

EXTERNC_begin

extern int calyears_open(calyears *,cchar *,cchar **,cchar **) noex ;
extern int calyears_count(calyears *) noex ;
extern int calyears_curbegin(calyears *,CALYEARS_CUR *) noex ;
extern int calyears_lookcite(calyears *,CALYEARS_CUR *,CALYEARS_Q *) noex ;
extern int calyears_read(calyears *,CALYEARS_CUR *,CALYEARS_Q *,
		char *,int) noex ;
extern int calyears_curend(calyears *,CALYEARS_CUR *) noex ;
extern int calyears_check(calyears *,time_t) noex ;
extern int calyears_audit(calyears *) noex ;
extern int calyears_close(calyears *) noex ;
extern int calyears_already(calyears *,vecobj *,CALENT *) noex ;
extern int calyears_havestart(calyears *,CALCITE *,int,cchar *,int) noex ;

EXTERNC_end


#endif /* CALYEARS_INCLUDE */


