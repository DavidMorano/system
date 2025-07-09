/* chowns HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* make a series of possibly hierarchical directories */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright (c) 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	CHOWNS_INCLUDE
#define	CHOWNS_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<fcntl.h>		/* |mode_t| */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>


EXTERNC_begin

extern int	chowns(cchar *,mode_t) noex ;

EXTERNC_end


#endif /* CHOWNS_INCLUDE */


