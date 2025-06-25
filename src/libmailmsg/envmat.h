/* envmat HEADER */
/* charset=ISO8859-1 */
/* lang=C20 (conformance reviewed) */

/* mail-message envelope matching */
/* version %I% last-modified %G% */


/* revision history:

	= 1995-04-01, David A­D­ Morano
	This code was started for Levo related work.

*/

/* Copyright © 1995 David A­D­ Morano.  All rights reserved. */

#ifndef	ENVMAT_INCLUDE
#define	ENVMAT_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>


EXTERNC_begin

extern int	envmat(cchar *,int) noex ;

EXTERNC_end


#endif /* ENVMAT_INCLUDE */


