/* burn HEADER */
/* encoding=ISO8859-1 */
/* lang=C20 */

/* allocated string */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	BURN_INCLUDE
#define	BURN_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>


EXTERNC_begin

extern int	burn(cchar *,int,randomvar *) noex ;

EXTERNC_end

#ifdef	__cplusplus

inline int burn(cchar *fn,int bc = 1) noex {
    	return burn(fn,bc,nullptr) ;
}

#endif	/* __cplusplus */


#endif /* BURN_INCLUDE */


