/* igcd HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* write to a network (classic function) */
/* version %I% last-modified %G% */


/* revision history:

	= 1995-02-21, David A­D­ Morano
	This subroutine module was adopted for use from some previous
	code that performed the similar sorts of functions.

*/

/* Copyright © 1995 David A­D­ Morano.  All rights reserved. */

#ifndef	IGCD_INCLUDE
#define	IGCD_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */


EXTERNC_begin

extern int igcd(int,int) noex ;

EXTERNC_end


#endif /* IGCD_INCLUDE */


