/* getproto HEADER */
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
	getproto_name

	Description:
	Retrieve a protocol number given a protocol name.

*******************************************************************************/

#ifndef	GETPROTO_INCLUDE
#define	GETPROTO_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */


EXTERNC_begin

extern int	getproto_name(cchar *,int) noex ;

EXTERNC_end


#endif /* GETPROTO_INCLUDE */


