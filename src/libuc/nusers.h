/* nusers HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* retrieve the number of users (logged-in) on system */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-18, David A­D­ Morano
	I calculate the number of users logged into the system.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	NUSERS_INCLUDE
#define	NUSERS_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */


EXTERNC_begin

extern int	nusers() noex ;

EXTERNC_end


#endif /* NUSERS_INCLUDE */


