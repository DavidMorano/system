/* getprotofamily HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* UNIX® Protocol-Family support */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-21, David A­D­ Morano
	This module was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	getprotofamily

	Description:
	This subroutine will find a protocol family (if one exists)
	that corresponds with a specified address family.

*******************************************************************************/

#ifndef	GETPROTOFAMILY_INCLUDE
#define	GETPROTOFAMILY_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>
#include	<usysbase.h>


EXTERNC_begin

extern int	getprotofamily(int) noex ;

EXTERNC_end


#endif /* GETPROTOFAMILY_INCLUDE */


