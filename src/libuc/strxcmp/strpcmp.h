/* strpcmp HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* c-string comparisons */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	STRPCMP_INCLUDE
#define	STRPCMP_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>


EXTERNC_begin

extern int	strpcmp(cchar *,cchar *) noex ;

extern int	strxpcmp(cchar *,cchar *) noex ;

EXTERNC_end


#endif /* STRPCMP_INCLUDE */


