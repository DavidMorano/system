/* fhm HEADER (Floating-Harmonic-Mean) */
/* charset=ISO8859-1 */
/* lang=C20 */

/* this calculates a floating-point harmonic mean of a set of numbers */
/* version %I% last-modified %G% */


/* Copyright © 2001 David A­D­ Morano.  All rights reserved. */

#ifndef	FHM_INCLUDE
#define	FHM_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */


EXTERNC_begin

extern double	fhm(double *,int) noex ;

EXTERNC_end


#endif /* FHM_INCLUDE */


