/* cfutil HEADER */
/* charset=ISO8859-1 */
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
#ifdef	__cplusplus /* C++ only */


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>

namespace cfx {
    extern int	rmleadzero(cchar *,int) noex ;		/* used internally */
    extern int	sfdigs(cchar *,int,cchar **) noex ;
    extern int	sfchars(cchar *,int,cchar **) noex ;
    extern int	getsign(cchar *,int,bool *) noex ;
} /* end namespace */


#endif /* __cplusplus (C++ only) */
#endif /* CFUTIL_INCLUDE */


