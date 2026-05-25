/* bufsizeget HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* interface for getting system buffer sizes */
/* version %I% last-modified %G% */


/* revision history:

	= 2001-04-11, David A-D- Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2001 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	bufsizeget

	Description:
	This file provides the interface for retrieving system
	buffer sizes.  These sizes are determined dynamically at
	run-time but are otherwise expected to remain constant for
	the life of any process.

*******************************************************************************/

#ifndef	BUFSIZEGET_INCLUDE
#define	BUFSIZEGET_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<bufsizes.h>		/* LIBUC */


#define	BUFSIZEGET_CONF		"/etc/default/bufsize"
#define	BUFSIZEGET_DEFVAL	(4*1024)


EXTERNC_begin

extern int	bufsizeget(int) noex ;

EXTERNC_end


#endif /* BUFSIZEGET_INCLUDE */


