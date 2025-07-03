/* mapstrint HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* Map (database) for String-Integer pairs */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-01, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	MAPSTRINT_INCLUDE
#define	MAPSTRINT_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<hdb.h>


#define	MAPSTRINT	hdb
#define	MAPSTRINT_CUR	hdb_cur


typedef	MAPSTRINT	mapstrint ;
typedef	MAPSTRINT_CUR	mapstrint_cur ;

EXTERNC_begin

extern int mapstrint_start(mapstrint *,int) noex ;
extern int mapstrint_count(mapstrint *) noex ;
extern int mapstrint_add(mapstrint *,cchar *,int,int) noex ;
extern int mapstrint_already(mapstrint *,cchar *,int) noex ;
extern int mapstrint_fetch(mapstrint *,cchar *,int,
		mapstrint_cur *,int *) noex ;
extern int mapstrint_fetchrec(mapstrint *,cchar *,int,
		mapstrint_cur *,cchar **,int *) noex ;
extern int mapstrint_curget(mapstrint *,mapstrint_cur *,cchar **,int *) noex ;
extern int mapstrint_curenum(mapstrint *,mapstrint_cur *,cchar **,int *) noex ;
extern int mapstrint_delkey(mapstrint *,cchar *,int) noex ;
extern int mapstrint_curdel(mapstrint *,mapstrint_cur *,int) noex ;
extern int mapstrint_curnext(mapstrint *,mapstrint_cur *) noex ;
extern int mapstrint_nextkey(mapstrint *,cchar *,int,mapstrint_cur *) noex ;
extern int mapstrint_curbegin(mapstrint *,mapstrint_cur *) noex ;
extern int mapstrint_curend(mapstrint *,mapstrint_cur *) noex ;
extern int mapstrint_cursetval(mapstrint *,mapstrint_cur *,int) noex ;
extern int mapstrint_finish(mapstrint *) noex ;

EXTERNC_end


#endif /* MAPSTRINT_INCLUDE */


