/* sortlist HEADER */
/* lang=C20 */

/* sorted list structures (Sorted List) */
/* version %I% last-modified %G% */


/* revision history:

	 = 2007-12-01, David A.D. Morano
	Module was originally written.

*/

/* Copyright © 2007 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	sortlist

	Description:
	These routines are used when the caller just wants to store
	an item in a sorted list.  The item us usually just a pointer
	to the user's real data.  Items have keys and values, like
	with a hash table structure except that we use a balanced
	binary tree to store everything.

*******************************************************************************/

#ifndef	SORTLIST_INCLUDE
#define	SORTLIST_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<usyscalls.h>


/* object defines */
#define	SORTLIST_MAGIC	0x31415926
#define	SORTLIST	struct sortlist_head
#define	SORTLIST_ENT	struct sortlist_ent


struct sortllist_ent {
	SORTLIST_ENT	*left ;
	SORTLIST_ENT	*right ;
	void		*keyp ;
	void		*valuep ;
	int		balance ;
} ;

struct sortlist_head {
	int		magic ;
	int		(*cmpfunc)(void *,void *) ;
	SORTLIST_ENT	*root ;
} ;


typedef struct sortlist_head	sortlist ;

EXTERNC_begin

extern int sortlist_start(sortlist *,int (*)()) noex ;
extern int sortlist_add(sortlist *,void *,void *,int) noex ;
extern int sortlist_get(sortlist *,void *,int,void **) noex ;
extern int sortlist_del(sortlist *,void *) noex ;
extern int sortlist_count(sortlist *) noex ;
extern int sortlist_search(sortlist *,void *,int (*)(),void *) noex ;
extern int sortlist_finish(sortlist *) noex ;

EXTERNC_end


#endif /* SORTLIST_INCLUDE */


