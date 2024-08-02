/* svcent HEADER */
/* lang=C20 */

/* subroutines for simple SVCFILE_ENT object management */
/* version %I% last-modified %G% */


/* revision history:

	= 2017-10-13, David A­D­ Morano
	This was split out of the HOMEPAGE program (where it was originally
	local).

*/

/* Copyright © 2017 David A­D­ Morano.  All rights reserved. */

#ifndef	SVCENT_INCLUDE
#define	SVCENT_INCLUDE


#include	<envstandards.h>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysrets.h>
#include	<svcfile.h>


#define	SVCENT	SVCFILE_ENT


EXTERNC_begin

extern int	svcent_islib(SVCENT *,cchar **) noex ;
extern int	svcent_getval(SVCENT *,cchar *,cchar **) noex ;
extern int	svcent_getdeval(SVCENT *,cchar *,cchar **) noex ;

EXTERNC_end


#endif /* SVCENT_INCLUDE */


