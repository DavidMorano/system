/* getaf HEADER */
/* lang=C20 */

/* UNIX® Address-Family (AF) support */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-21, David A­D­ Morano
	This module was originally written.

	= 2017-08-01, David A­D­ Morano
	Updated for lack of interfaces in Apple-Darwin

*/

/* Copyright © 1998,2017 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This file contains some support for the UNIX® Address-Familt
	(AF) related operations.

*******************************************************************************/

#ifndef	GETAF_INCLUDE
#define	GETAF_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<clanguage.h>


EXTERNC_begin

extern int	getaf(cchar *,int) noex ;
extern int	getaflen(int) noex ;
extern cchar	*strafname(int) noex ;

EXTERNC_end


#endif /* GETAF_INCLUDE */


