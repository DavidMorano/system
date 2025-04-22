/* msort HEADER */
/* encoding=ISO8859-1 */
/* lang=C20 */

/* My-Sort (another Quick-Sort algorithm) */
/* version %I% last-modified %G% */


/* revision history:

	= 2001-10-04, David A­D­ Morano
	This was originally written.

*/

/* Copyright © 2001 David A­D­ Morano.  All rights reserved. */

#ifndef	MSORT_INCLUDE
#define	MSORT_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>


EXTERNC_begin

typedef int	(*msortcmp_f)(cvoid *,cvoid *) ;

extern int	msort(void *,int,int,msortcmp_f) ;

EXTERNC_end


#endif /* MSORT_INCLUDE */


