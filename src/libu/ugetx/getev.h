/* getev HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* UNIX® Environment-Variable support */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-21, David A­D­ Morano
	This module was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	getev

	Description:
	This file contains some support for the UNIX® environment
	variable operations.

*******************************************************************************/

#ifndef	GETEV_INCLUDE
#define	GETEV_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */


EXTERNC_begin

extern int	getev(con mainv,cchar *,int,cchar **) noex ;

EXTERNC_end


#endif /* GETEV_INCLUDE */


