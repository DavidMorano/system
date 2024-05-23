/* schedvar HEADER */
/* lang=C20 */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	SCHEDVAR_INCLUDE
#define	SCHEDVAR_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysrets.h>
#include	<usyscalls.h>
#include	<vecstr.h>


#define	SCHEDVAR	vecstr
#define	SCHEDVAR_CUR	struct schedvar_cursor


struct schedvar_cursor {
	int		i ;
} ;

typedef	SCHEDVAR	schedvar ;
typedef	SCHEDVAR_CUR	schedvar_cur ;

EXTERNC_begin

extern int schedvar_start(schedvar *) noex ;
extern int schedvar_add(schedvar *,cchar *,cchar *,int) noex ;
extern int schedvar_curbegin(schedvar *,schedvar_cur *) noex ;
extern int schedvar_enum(schedvar *,schedvar_cur *,char *,int,char *,int) noex ;
extern int schedvar_curend(schedvar *,schedvar_cur *) noex ;
extern int schedvar_findkey(schedvar *,cchar *,cchar **) noex ;
extern int schedvar_del(schedvar *,cchar *) noex ;
extern int schedvar_expand(schedvar *,char *,int,cchar *,int) noex ;
extern int schedvar_finish(schedvar *) noex ;

EXTERNC_end


#endif /* SCHEDVAR_INCLUDE */


