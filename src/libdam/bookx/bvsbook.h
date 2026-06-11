/* bvsbook HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* manage a BVS "book" entry */
/* version %I% last-modified %G% */


/* revision history:

	= 2008-10-01, David A­D­ Morano
	This module was originally written.

*/

/* Copyright © 2008 David A­D­ Morano.  All rights reserved. */

#ifndef	BVSBOOK_INCLUDE
#define	BVSBOOK_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */


#define	BVSBOOK		struct bvsbook_head


struct bvsbook_head {
	uchar		*ap ;		/* array of verses for each chapter */
	ushort		nverses ;	/* total for the whole book */
	ushort		nzverses ;	/* non-zero for the whole book */
	uchar		ci ;		/* chapter index */
	uchar		book ;
	uchar		al ;		/* also is the number of chapters */
} ; /* end struct */

typedef	BVSBOOK		bvsbook ;

EXTERNC_begin

extern int	bvsbook_set(bvsbook *,ushort *) noex ;
extern int	bvsbook_get(bvsbook *,ushort *) noex ;

EXTERNC_end


#endif /* BVSBOOK_INCLUDE */


