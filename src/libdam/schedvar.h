/* schedvar HEADER (libuc-permx) */
/* charset=ISO8859-1 */
/* lang=C20 */

/* creates the substitution varaiables for 'scheduled' type operations */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	SCHEDVAR_INCLUDE
#define	SCHEDVAR_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<vecstr.h>		/* LIBUC */


#define	SCHEDVAR	struct schedvar_head
#define	SCHEDVAR_CUR	struct schedvar_cursor
#define	SCHEDVAR_MAGIC	0x89362514


struct schedvar_cursor {
	int		i ;
} ; /* end struct */

struct schedvar_head {
	vecstr		*slp ;
	uint		magval ;
} ; /* end struct */

typedef	SCHEDVAR	schedvar ;
typedef	SCHEDVAR_CUR	schedvar_cur ;

EXTERNC_begin

extern int schedvar_start(schedvar *) noex ;
extern int schedvar_add(schedvar *,cchar *,cchar *,int) noex ;
extern int schedvar_curbegin(schedvar *,schedvar_cur *) noex ;
extern int schedvar_curenum(schedvar *,schedvar_cur *,char *,int,
		char *,int) noex ;
extern int schedvar_curend(schedvar *,schedvar_cur *) noex ;
extern int schedvar_findkey(schedvar *,cchar *,cchar **) noex ;
extern int schedvar_del(schedvar *,cchar *) noex ;
extern int schedvar_expand(schedvar *,char *,int,cchar *,int) noex ;
extern int schedvar_finish(schedvar *) noex ;

EXTERNC_end


#endif /* SCHEDVAR_INCLUDE */


