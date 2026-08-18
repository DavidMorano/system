/* holidayer HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* access for the HOLIDAYER database */
/* version %I% last-modified %G% */


/* Copyright © 2016 David A­D­ Morano.  All rights reserved. */

#ifndef	HOLIDAYER_INCLUDE
#define	HOLIDAYER_INCLUDE


#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<sys/types.h>		/* POSIX® */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<ids.h>			/* LIBUC */
#include	<vechand.h>		/* LIBUC */

#include	"holidays.h"


#define	HOLIDAYER	struct holidayer_head
#define	HOLIDAYER_FL	struct holidayer_flags
#define	HOLIDAYER_OBJ	struct holidayer_object
#define	HOLIDAYER_CUR	struct holidayer_cursor
#define	HOLIDAYER_CITE	holidays_cite
#define	HOLIDAYER_Q	holidays_cite
#define	HOLIDAYER_MAGIC	0x63328184


struct holidayer_object {
	cchar		*name ;
	uint		objsz ;
	uint		cursz ;
} ; /* end struct */

struct holidayer_cursor {
	holidays_cur	*hcurp ;
	holidays	*hop ;
	uint		magval ;
	uint		year ;
} ; /* end struct */

struct holidayer_flags {
	uint		hols:1 ;
} ; /* end struct */

struct holidayer_head {
	cchar		*pr ;
	ids		*idp ;
	vechand		*hlp ;
	HOLIDAYER_FL	fl ;
	uint		magval ;
	uint		year ;
	int		ncursors ;
} ; /* end struct */

typedef	HOLIDAYER	holidayer ;
typedef	HOLIDAYER_FL	holidayer_fl ;
typedef	HOLIDAYER_OBJ	holidayer_obj ;
typedef	HOLIDAYER_CUR	holidayer_cur ;
typedef	HOLIDAYER_CITE	holidayer_cite ;
typedef	HOLIDAYER_Q	holidayer_q ;

EXTERNC_begin

extern int holidayer_open(holidayer *,cchar *) noex ;
extern int holidayer_curbegin(holidayer *,holidayer_cur *) noex ;
extern int holidayer_curend(holidayer *,holidayer_cur *) noex ;
extern int holidayer_fetchcite(holidayer *,holidayer_cite *,
		holidayer_cur *,char *,int) noex ;
extern int holidayer_fetchname(holidayer *,uint,cchar *,int,
		holidayer_cur *,holidayer_cite *,char *,int) noex ;
extern int holidayer_curenum(holidayer *,holidayer_cur *,
		holidayer_cite *,char *,int,uint) noex ;
extern int holidayer_check(holidayer *,time_t) noex ;
extern int holidayer_audit(holidayer *) noex ;
extern int holidayer_close(holidayer *) noex ;

EXTERNC_end

extern const holidayer_obj	holidayer_modinfo ;


#endif /* HOLIDAYER_INCLUDE */


