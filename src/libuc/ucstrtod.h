/* strtod HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	STRTOD_INCLUDE
#define	STRTOD_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<stdintx.h>


EXTERNC_begin

extern int uc_strtod(cchar *,char **,double *) noex ;

EXTERNC_end


#endif /* STRTOD_INCLUDE */


