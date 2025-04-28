/* cfutil HEADER */
/* encoding=ISO8859-1 */
/* lang=C20 */

/* convert a decimal digit string to its binary floating value */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	CFUTIL_INCLUDE
#define	CFUTIL_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>


#ifdef	__cplusplus

namespace cfx {
    extern int	rmleadzero(cchar *,int) noex ;
    extern int	sfdigs(cchar *,int,cchar **) noex ;
    extern int	sfchars(cchar *,int,cchar **) noex ;
}

#endif /* __cplusplus */


#endif /* CFUTIL_INCLUDE */


