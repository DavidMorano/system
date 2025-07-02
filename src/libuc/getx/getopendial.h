/* getopendial HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* get the type of operating system we are on */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-07-10, David A­D­ Morano
	This was written from scratch.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	getopendial

	Description:
	Get the open-dial code (an integer value) for an open-dial
	name string.

	Synopsis:
	int getopendial(cchar *name) noex

	Arguments:
	name		name of dialer

	Returns:
	>=0		OK
	<0		error (system-return)

*******************************************************************************/

#ifndef	GETOPENDIAL_INCLUDE
#define	GETOPENDIAL_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>

#include	<opendial.h>


EXTERNC_begin

extern int	getopendial(cchar *) noex ;

EXTERNC_end


#endif /* GETOPENDIAL_INCLUDE */


