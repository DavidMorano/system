/* shiftreg HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* Shift Register of ULONGs */
/* version %I% last-modified %G% */


/* revision history:

	= 2001-07-10, David A­D­ Morano
	This code was started.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	SHIFTREG_INCLUDE
#define	SHIFTREG_INCLUDE


#include	<envstandards.h>
#include	<usystem.h>


/* object defines */
#define	SHIFTREG_MAGIC		0x74896233
#define	SHIFTREG		struct shiftreg_head


struct shiftreg_head {
	ulong		*regs ;
	uint		magic ;
	int		n ;
	int		i ;		/* next write index */
} ;

typedef	SHIFTREG	shiftreg ;

EXTERNC_begin

extern int shiftreg_start(shiftreg *,int) noex ;
extern int shiftreg_finish(shiftreg *) noex ;
extern int shiftreg_shiftin(shiftreg *,ulong) noex ;
extern int shiftreg_get(shiftreg *,int,ulong *) noex ;

EXTERNC_end


#endif /* SHIFTREG_INCLUDE */


