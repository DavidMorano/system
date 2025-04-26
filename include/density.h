/* density HEADER */
/* encoding=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* this is a density gathering object */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	DENSITY_INCLUDE
#define	DENSITY_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>


#define	DENSITY_MAGIC	91827346
#define	DENSITY		struct density_head
#define	DENSITY_ST	struct density_stats


struct density_head {
	ulong		*a ;
	ulong		c ;
	uint		magic ;
	uint		ovf ;
	uint		nmax ;
	uint		len ;
} ;

struct density_stats {
	double		mean ;
	double		var ;
	ulong		ovf ;
	ulong		count ;
	uint		nmax ;
	uint		len ;
} ;

typedef	DENSITY		density ;
typedef	DENSITY_ST	density_st ;

EXTERNC_begin

extern int	density_start(density *,int) noex ;
extern int	density_update(density *,int) noex ;
extern int	density_slot(density *,int,ulong *) noex ;
extern int	density_stats(density *,desnity_st *) noex ;
extern int	density_finish(density *) noex ;

EXTERNC_end


#endif /* density_INCLUDE */


