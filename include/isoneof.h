/* isoneof HEADER */
/* lang=C20 */

/* test is an integer is one of a group of integers */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright (c) 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	ISONEOF_INCLUDE
#define	ISONEOF_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<utypedefs.h>
#include	<clanguage.h>


EXTERNC_begin

extern bool isOneOf(cint *,int) noex ;

EXTERNC_end


#endif /* ISONEOF_INCLUDE */


