/* getpf HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* get a protocol family from an address family */
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

#ifndef	GETPF_INCLUDE
#define	GETPF_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */


EXTERNC_begin

extern int	getpf(int) noex ;

EXTERNC_end


#endif /* GETPF_INCLUDE */


