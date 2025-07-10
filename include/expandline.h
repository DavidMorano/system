/* expandline HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* subroutine to expand a line out for TROFF rendering */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-10, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1987 David A­D­ Morano.  All rights reserved. */

#ifndef	EXPANDLINE_INCLUDE
#define	EXPANDLINE_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>


EXTERNC_begin

extern int	expandline(cchar *,int,char *,int,int *) noex ;

EXTERNC_end


#endif /* EXPANDLINE_INCLUDE */


