/* printhelp HEADER */
/* lang=C++20 */

/* convert a single character to a given case */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-10, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright (c) 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	PRINTHELP_INCLUDE
#define	PRINTHELP_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<ostream>
#include	<utypedefs.h>
#include	<clanguage.h>


EXTERNC_begin

extern int printhelp(std::ostream *,cchar *,cchar *,cchar *) noex ;

EXTERNC_end


#endif /* PRINTHELP_INCLUDE */


