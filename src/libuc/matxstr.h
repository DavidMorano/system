/* matxstr HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* Match a String */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A�D� Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright � 1998 David A�D� Morano.  All rights reserved. */

#ifndef	MATXSTR_INCLUDE
#define	MATXSTR_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>

#include	<matostr.h>
#include	<matpstr.h>
#include	<matkeystr.h>
#include	<matsubstr.h>


EXTERNC_begin

extern int	matnstr(mainv,cchar *,int) noex ;
extern int	matlocalfs(cchar *,int) noex ;

EXTERNC_end


#endif /* MATXSTR_INCLUDE */


