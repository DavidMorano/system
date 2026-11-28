/* cmpstrpre HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* c-string comparisons */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	CMPSTRPRE_INCLUDE
#define	CMPSTRPRE_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */


EXTERNC_beg
extern int	cmpstrpre	(cchar *,cchar *) noex ;
extern int	cmpstrpreø	(cchar *,cchar *) noex ;
EXTERNC_end


#endif /* CMPSTRPRE_INCLUDE */


