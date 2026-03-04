/* quickselecti HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* Quick-Select for-intgers function */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-09-10, David A­D­ Morano
	This subroutine was created for Levo research.  This
	subroutine is a helper function for several simulated
	branch predictors.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	QUICKSELECTI_INCLUDE
#define	QUICKSELECTI_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>


EXTERNC_begin

extern int	quickselecti(int *,int,int,int) noex ;

EXTERNC_end


#endif /* QUICKSELECTI_INCLUDE */


