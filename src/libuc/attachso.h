/* attachso HEADER */
/* lang=C20 */

/* ATTACHSO function */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-01, David A�D� Morano
	This subroutine was originally written.

*/

/* Copyright � 1998 David A�D� Morano.  All rights reserved. */

#ifndef	ATTACHSO_INCLUDE
#define	ATTACHSO_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<clanguage.h>


EXTERNC_begin

extern int attachso(cchar **,cchar *,cchar **,cchar **,int,void **) noex ;

EXTERNC_end


#endif /* ATTACHSO_INCLUDE */


