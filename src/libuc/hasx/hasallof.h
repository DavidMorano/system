/* hasallof HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* does a counted c-string contain a C++ module-name? */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-10-10, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright (c) 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	HASALLOF_INCLUDE
#define	HASALLOF_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<usysbase.h>


EXTERNC_begin

extern int	hasallof(cchar *,int,cchar *) noex ;

EXTERNC_end


#endif /* HASALLOF_INCLUDE */


