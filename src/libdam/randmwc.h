/* randmwc HEADER (RNG Multiply-With-Carry) */
/* charset=ISO8859-1 */
/* lang=C20 */

/* random number generator (Multiply-With-Carry) */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	RANDMWC_INCLUDE
#define	RANDMWC_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */


#define	RANDMWC		struct randmwc_head


struct randmwc_head {
	uint		a ;
	ulong		x, c ;
} ; /* end struct */

typedef	RANDMWC		randmwc ;

EXTERNC_begin

extern int randmwc_start	(randmwc *,int,uint) noex ;
extern int randmwc_finish	(randmwc *) noex ;
extern int randmwc_getulong	(randmwc *,ulong *) noex ;

EXTERNC_end


#endif /* RANDMWC_INCLUDE */


