/* slist HEADER */
/* lang=C20 */

/* regular (no-frills) pointer queue */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-03, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	SLIST_INCLUDE
#define	SLIST_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<utypedefs.h>
#include	<usysrets.h>
#include	<clanguage.h>


#define	SLIST		struct slist_head
#define	SLIST_ENT	struct slist_ent
#define	SLIST_CUR	struct slist_cur


struct slist_cur {
	SLIST_ENT	*entp ;
} ;

struct slist_ent {
	SLIST_ENT	*next ;
} ;

struct slist_head {
	SLIST_ENT	*head ;
	SLIST_ENT	*tail ;
	int		count ;
} ;

typedef	SLIST		slist ;
typedef	SLIST_ENT	slist_ent ;
typedef	SLIST_CUR	slist_cur ;

#ifdef	__cplusplus
extern "C" {
#endif

extern int slist_start(slist *) noex ;
extern int slist_ins(slist *,slist_ent *) noex ;
extern int slist_insgroup(slist *,slist_ent *,int,int) noex ;
extern int slist_present(slist *,slist_ent *) noex ;
extern int slist_gettail(slist *,slist_ent **) noex ;
extern int slist_rem(slist *,slist_ent **) noex ;
extern int slist_unlink(slist *,slist_ent *) noex ;
extern int slist_curbegin(slist *,slist_cur *) noex ;
extern int slist_curend(slist *,slist_cur *) noex ;
extern int slist_enum(slist *,slist_cur *,slist_ent **) noex ;
extern int slist_count(slist *) noex ;
extern int slist_audit(slist *) noex ;
extern int slist_finish(slist *) noex ;

#ifdef	__cplusplus
}
#endif


#endif /* SLIST_INCLUDE */


