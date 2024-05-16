/* rectab HEADER */
/* lang=C20 */

/* record-table */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	RECTAB_INCLUDE
#define	RECTAB_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>		/* systems IDs */
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<clanguage.h>
#include	<localmisc.h>


#define	RECTAB		struct rectab_head


struct rectab_head {
	uint		*rt ;
	int		i ;		/* highest index */
	int		n ;		/* extent */
} ;

typedef	RECTAB		rectab ;

EXTERNC_begin

extern int	rectab_start(rectab *,int) noex ;
extern int	rectab_add(rectab *,uint) noex ;
extern int	rectab_done(rectab *) noex ;
extern int	rectab_getvec(rectab *,uint **) noex ;
extern int	rectab_extend(rectab *) noex ;
extern int	rectab_finish(rectab *) noex ;
extern int	rectab_count(rectab *) noex ;

EXTERNC_end


#endif /* RECTAB_INCLUDE */


