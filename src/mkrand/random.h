/* stdrandom HEADER */
/* lang=C++20 */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	STDRANDOM_INCLUDE
#define	STDRANDOM_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<usyscalls.h>


#define	STDRANDOM	struct stdrandom_head


struct stdrandom_head {
	ulong		*fptr ;
	ulong		*rptr ;
	ulong		*end_ptr ;
	ulong		state[64] ;
	int		rand_type ;
	int		rand_deg ;
	int		rand_sep ;
} ;

typedef	STDRANDOM	stdrandom ;

EXTERNC_begin

extern int	stdrandom_start(stdrandom *,int,uint) noex ;
extern int	stdrandom_finish(stdrandom *) noex ;
extern int	stdrandom_getuint(stdrandom *,uint *) noex ;
extern int	stdrandom_getulong(stdrandom *,ulong *) noex ;

EXTERNC_end


#endif /* STDRANDOM_INCLUDE */


