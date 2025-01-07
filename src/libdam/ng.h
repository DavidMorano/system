/* ng HEADER */
/* encoding=ISO8859-1 */
/* lang=C20 */

/* search (and other things) a newsgroup list for a newsgroup name */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-01, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	NG_INCLUDE
#define	NG_INCLUDE


#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<vecitem.h>


#define	NG		vecitem
#define	NG_ENT		struct ng_entry


struct ng_entry {
	cchar		*name ;
	cchar		*dir ;
	int		len ;
} ;

typedef	NG		ng ;
typedef	NG_ENT		ng_ent ;

EXTERNC_begin

extern int ng_start(ng *) noex ;
extern int ng_addparse(ng *,cchar *,int) noex ;
extern int ng_add(ng *,cchar *,int,cchar *) noex ;
extern int ng_copy(ng *,vecitem *) noex ;
extern int ng_search(ng *,cchar *,ng_ent **) noex ;
extern int ng_get(ng *,int,ng_ent **) noex ;
extern int ng_count(ng *) noex ;
extern int ng_finish(ng *) noex ;

EXTERNC_end


#endif /* NG_INCLUDE */


