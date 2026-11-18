/* cmpstrval HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* c-string comparisons w/ case variations */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	CMPSTRVAL_INCLUDE
#define	CMPSTRVAL_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */


EXTERNC_begin
extern int	cmpstrval	(cchar *,cchar *) noex ;
extern int	cmpstrvalø	(cchar *,cchar *) noex ;
EXTERNC_end


#endif /* CMPSTRVAL_INCLUDE */


