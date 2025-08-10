/* nonpath HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	NONPATH_INCLUDE
#define	NONPATH_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>


enum nonpaths {
    	nonpath_reg,			/* regular: absolute or relative */
    	nonpath_user,
    	nonpath_cd,
	nonpath_dialer,
	nonpath_fsvc,
	nonpath_usvc,
	nonpath_overlast
} ; /* end enum (nonpaths) */


EXTERNC_begin

extern int	nonpath(cchar *,int) noex ;

EXTERNC_end


#endif /* NONPATH_INCLUDE */


