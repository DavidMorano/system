/* random HEADER */
/* lang=C20 */

/* RANDOM object */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	RANDOM_INCLUDE
#define	RANDOM_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<clanguage.h>


#define	RANDOM		struct random_head


struct random_head {
	ulong		*fptr ;
	ulong		*rptr ;
	ulong		*end_ptr ;
	ulong		state[64] ;
	int		rand_type ;
	int		rand_deg ;
	int		rand_sep ;
} ;

typedef RANDOM		random ;

EXTERNC_begin

extern int	random_start(random *,int,uint) noex ;
extern int	random_finish(random *) noex ;
extern int	random_getuint(random *,uint *) noex ;
extern int	random_getulong(random *,ulong *) noex ;

EXTERNC_end


#endif /* RANDOM_INCLUDE */


