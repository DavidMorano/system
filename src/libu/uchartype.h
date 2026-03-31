/* uchartype HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* C-language library-extension subroutines */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright (c) 1998 David A­D­ Morano.  All rights reserved. */

#ifndef UCHARTYPE_INCLUDE
#define	UCHARTYPE_INCLUDE


#include	<envstandards.h>	/* order first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>

EXTERNC_begin

extern bool	isdigex(int) noex attrpure ;
extern bool	iswhite(int) noex attrpure ;

EXTERNC_end


#endif	/* UCHARTYPE_INCLUDE */


