/* getcanonical HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* get a cannical host-name */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	GETCANONICAL_INCLUDE
#define	GETCANONICAL_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<usysbase.h>


EXTERNC_begin

extern int	getcanonical(cchar *,char *,char *,char *) noex ;


EXTERNC_end


#endif /* GETCANONICAL_INCLUDE */


