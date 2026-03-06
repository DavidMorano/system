/* hostequiv HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* get a stack size (of some type) */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-07-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	HOSTEQUIV_INCLUDE
#define	HOSTEQUIV_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<usysbase.h>


EXTERNC_begin

extern int	hostequiv(cchar *,cchar *,cchar *) noex ;

EXTERNC_end


#endif /* HOSTEQUIV_INCLUDE */


