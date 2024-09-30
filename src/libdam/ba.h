/* ba HEADER (BitArray) */
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
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>


#define	BA		struct ba_head
#define	BA_NUM		struct ba_numarr
#define	BANUM		BA_NUM


struct ba_numarr {
	int		*num ;
} ;

struct ba_head {
	BA_NUM		*cnp ;
	ulong		*a ;
	int		nbits ;
	int		nwords ;
} ;

typedef	BA		ba ;
typedef	BA_NUM		ba_num ;

EXTERNC_begin

extern int	ba_start(ba *,BA_NUM *,int) noex ;
extern int	ba_setones(ba *) noex ;
extern int	ba_zero(ba *) noex ;
extern int	ba_countdown(ba *) noex ;
extern int	ba_and(ba *,BA *) noex ;
extern int	ba_finish(ba *) noex ;
/* helpers functions */
extern int	banum_prepare(BA_NUM *) noex ;
extern int	banum_forsake(BA_NUM *) noex ;

EXTERNC_end


#endif /* BA_INCLUDE */


