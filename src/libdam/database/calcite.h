/* calcite HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* calendar citation */
/* version %I% last-modified %G% */


/* Copyright © 2008 David A­D­ Morano.  All rights reserved. */

#ifndef	CALCITE_INCLUDE
#define	CALCITE_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */


#define	CALCITE		struct calcite_head


struct calcite_head {
	ushort		y ;
	uchar		m, d ;
} ; /* end struct */

typedef	CALCITE		calcite ;

EXTERNC_begin

extern int	calcite_load(calcite *,int,int,int) noex ;

EXTERNC_end


#endif /* CALCITE_INCLUDE */


