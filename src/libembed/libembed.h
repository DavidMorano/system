/* libuc HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* revision history:

	= 1998-04-13, David A-D- Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2017 David A­D­ Morano.  All rights reserved. */
/* Use is subject to license terms. */

#ifndef	LIBUC_INCLUDE
#define	LIBUC_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>


EXTERNC_begin

extern int	cupper(cchar *,cchar *,int) noex ;
extern int	cmpc(int,cchar *,cchar *) noex ;

EXTERNC_end


#endif /* LIBUC_INCLUDE */


