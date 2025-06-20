/* siletter HEADER (String-Index-Letter) */
/* charset=ISO8859-1 */
/* lang=C20 */

/* String-Index-Letter string searching function */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-24, David A­D­ Morano
	This subroutine was written as an enhancement for adding
	back-matter (end pages) to the output document.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	SILETTER_INCLUDE
#define	SILETTER_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>


#define	SILETTER_RES	struct siletter_result 


struct siletter_result {
	cchar		*lp ;
	int		ll ;
} ;

typedef SILETTER_RES	siletter_res ;

EXTERNC_begin

extern int	siletter(siletter_res *,cchar *,int) noex ;

EXTERNC_end


#endif /* SILETTER_INCLUDE */


