/* isort HEADER (Insertion-Sort) */
/* charset=ISO8859-1 */
/* lang=C20 */

/* looks like an Insertion-Sort */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-10, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	ISORT_INCLUDE
#define	ISORT_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */


EXTERNC_begin

typedef int	(*sortcmp_f)(cvoid *,cvoid *) noex ;

extern int isort(void *,int,int,sortcmp_f) noex ;

EXTERNC_end


#endif /* ISORT_INCLUDE */


