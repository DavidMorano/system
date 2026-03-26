/* ba HEADER (BitArray) */
/* charset=ISO8859-1 */
/* lang=C20 */

/* perform some bit-array type operations */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-15, David A­D­ Morano
	This code was started.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	BA_INCLUDE
#define	BA_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<usysbase.h>


#define	BA		struct ba_head
#define	BA_DIGIT	ulong


struct ba_head {
	BA_DIGIT	*a ;
	int		nbits ;
	int		nwords ;
} ; /* end struct */

typedef	BA		ba ;

EXTERNC_begin

extern int	ba_start	(ba *,int) noex ;
extern int	ba_setones	(ba *) noex ;
extern int	ba_zero		(ba *) noex ;
extern int	ba_countdown	(ba *) noex ;
extern int	ba_and		(ba *,ba *) noex ;
extern int	ba_numones	(ba *) noex ;
extern int	ba_shr		(ba *,int) noex ;
extern int	ba_shl		(ba *,int) noex ;
extern int	ba_finish	(ba *) noex ;

EXTERNC_end


#endif /* BA_INCLUDE */


