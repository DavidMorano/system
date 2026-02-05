/* pcsgetfacility HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* get the "facility" name of the for the PCS distribution */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-06-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	PCSGETFACILITY_INCLUDE
#define	PCSGETFACILITY_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<usysbase.h>


EXTERNC_begin

extern int	pcsgetfacility(cchar *,char *,int,cchar *) noex ;

EXTERNC_end


#endif /* PCSGETFACILITY_INCLUDE */


