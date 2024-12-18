/* typenonpath HEADER */
/* encoding=ISO8859-1 */
/* lang=C20 */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	TYPENONPATH_INCLUDE
#define	TYPENONPATH_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>


enum nonpathtypes {
	nonpathtype_not,
	nonpathtype_dialer,
	nonpathtype_fsvc,
	nonpathtype_usvc,
	nonpathtype_overlast
} ;

EXTERNC_begin

extern bool typenonpath(cchar *,int) noex ;

EXTERNC_end


#endif /* TYPENONPATH_INCLUDE */


