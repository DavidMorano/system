/* hasvarprefix HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* does a counted c-string have a varoable-prefix? */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright (c) 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	HASVARPREFIX_INCLUDE
#define	HASVARPREFIX_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>


EXTERNC_begin

extern bool	hasvarprefix(cchar *,int) noex ;

EXTERNC_end


#endif /* HASVARPREFIX_INCLUDE */


