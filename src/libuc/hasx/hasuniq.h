/* hasuniq HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* has a counted c-string have all unique characters? */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright (c) 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	HASUNIQ_INCLUDE
#define	HASUNIQ_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>


EXTERNC_begin

extern bool	hasuniq(cchar *,int) noex ;

EXTERNC_end


#endif /* HASUNIQ_INCLUDE */


