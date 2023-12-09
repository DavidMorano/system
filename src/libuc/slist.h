/* slist INCLUDE */
/* lang=C20 */

/* regular (no-frills) pointer queue */


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


#ifdef	__cplusplus
extern "C" {
#endif

extern int slist_start(SLIST *) noex ;
extern int slist_ins(SLIST *,SLIST_ENT *) noex ;
extern int slist_insgroup(SLIST *,SLIST_ENT *,int,int) noex ;
extern int slist_present(SLIST *,SLIST_ENT *) noex ;
extern int slist_gettail(SLIST *,SLIST_ENT **) noex ;
extern int slist_rem(SLIST *,SLIST_ENT **) noex ;
extern int slist_unlink(SLIST *,SLIST_ENT *) noex ;
extern int slist_curbegin(SLIST *,SLIST_CUR *) noex ;
extern int slist_curend(SLIST *,SLIST_CUR *) noex ;
extern int slist_enum(SLIST *,SLIST_CUR *,SLIST_ENT **) noex ;
extern int slist_count(SLIST *) noex ;
extern int slist_audit(SLIST *) noex ;
extern int slist_finish(SLIST *) noex ;

#ifdef	__cplusplus
}
#endif


#endif /* SLIST_INCLUDE */


