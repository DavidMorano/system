/* vecsorthand HEADER */
/* lang=C20 */

/* vector of sorted handles */
/* version %I% last-modified %G% */


/* Copyright © 2011 David A­D­ Morano.  All rights reserved. */

#ifndef	VECSORTHAND_INCLUDE
#define	VECSORTHAND_INCLUDE


#include	<envstandards.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<clanguage.h>
#include	<localmisc.h>


#define	VECSORTHAND		struct vecsorthand_head


#ifndef	TYPEDEF_VECENTCMP
#define	TYPEDEF_VECENTCMP
#ifdef	__cplusplus
extern "C" {
#endif

typedef int (*vecentcmp_f)(cvoid *,cvoid *) noex ;

#ifdef	__cplusplus
}
#endif
#endif /* TYPEDEF_VECENTCMP */

struct vecsorthand_head {
	vecentcmp_f	cmpfunc ;
	void		**va ;
	int		c ;		/* count of items in list */
	int		i ;		/* highest index */
	int		e ;		/* extent of array */
} ;

typedef struct vecsorthand_head	vecsorthand ;

#ifdef	__cplusplus
extern "C" {
#endif

extern int vecsorthand_start(vecsorthand *,int,vecentcmp_f) noex ;
extern int vecsorthand_add(vecsorthand *,cvoid *) noex ;
extern int vecsorthand_finish(vecsorthand *) noex ;
extern int vecsorthand_get(vecsorthand *,int,void *) noex ;
extern int vecsorthand_del(vecsorthand *,int) noex ;
extern int vecsorthand_delhand(vecsorthand *,cvoid *) noex ;
extern int vecsorthand_count(vecsorthand *) noex ;
extern int vecsorthand_search(vecsorthand *,cvoid *,void *) noex ;

#ifdef	__cplusplus
}
#endif


#endif /* VECSORTHAND_INCLUDE */


