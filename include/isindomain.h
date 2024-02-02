/* isindomain HEADER */
/* lang=C20 */

/* is a given c-string a valid INET address (either 4 or 6)? */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A�D� Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright (c) 1998 David A�D� Morano.  All rights reserved. */

#ifndef	ISINDOMAIN_INCLUDE
#define	ISINDOMAIN_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<utypedefs.h>
#include	<clanguage.h>


EXTERNC_begin

extern int	isindomain(cchar *,cchar *) noex ;

EXTERNC_end


#endif /* ISINDOMAIN_INCLUDE */

