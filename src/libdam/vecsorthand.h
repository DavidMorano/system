/* vecsorthand INCLUDE */
/* lang=C20 */

/* vector of sorted handles */


/* revision history:

	= 2010-12-01, David A.D. Morano
	Original.

	= 2019-01-17, David A.D. Morano
	Added new method |vecsorthand_dellall(3dam)|. A use for this new method
	has come up.

*/

/* Copyright � 2010,2019 David A-D- Morano.  All rights reserved. */

#ifndef	VECSORTHAND_INCLUDE
#define	VECSORTHAND_INCLUDE


#ifndef	TYPEDEF_VECENTCMP
#define	TYPEDEF_VECENTCMP	1
typedef int (*vecentcmp_t)(const void *,const void *) ;
#endif


#define	VECSORTHAND		struct vecsorthand_head


struct vecsorthand_head {
	vecentcmp_t	cmpfunc ;
	const void	**va ;
	int		i ;		/* highest index */
	int		e ;		/* extent of array */
	int		c ;		/* count of items in list */
} ;


typedef struct vecsorthand_head	vecsorthand ;


#ifndef	VECSORTHAND_MASTER

#ifdef	__cplusplus
extern "C" {
#endif

extern int vecsorthand_start(vecsorthand *,int,vecentcmp_t) ;
extern int vecsorthand_add(vecsorthand *,const void *) ;
extern int vecsorthand_finish(vecsorthand *) ;
extern int vecsorthand_get(vecsorthand *,int,void *) ;
extern int vecsorthand_del(vecsorthand *,int) ;
extern int vecsorthand_delall(vecsorthand *) ;
extern int vecsorthand_delhand(vecsorthand *,const void *) ;
extern int vecsorthand_count(vecsorthand *) ;
extern int vecsorthand_search(vecsorthand *,const void *,void *) ;

#ifdef	__cplusplus
}
#endif

#endif /* VECSORTHAND_MASTER */


#endif /* VECSORTHAND_INCLUDE */


