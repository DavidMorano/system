/* banum HEADER (BitNumber) */
/* charset=ISO8859-1 */
/* lang=C20 */

/* perform some bit-array type operations */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-15, David A­D­ Morano
	This code was started.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	BANUM_INCLUDE
#define	BANUM_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<usysbase.h>


#define	BANUM		struct banum_head


struct banum_head {
	int		*num ;
} ; /* end struct */

typedef	BANUM		banum ;

EXTERNC_begin

extern int	banum_start	(banum *) noex ;
extern int	banum_prepare	(banum *) noex ;
extern int	banum_forsake	(banum *) noex ;
extern int	banum_finish	(banum *) noex ;

EXTERNC_end


#endif /* BANUM_INCLUDE */


