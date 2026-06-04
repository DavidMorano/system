/* inet4int HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* some INET-related subroutine */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-21, David A­D­ Morano
	This module was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	inet4int

	Description:
	This file contains some support for the UNIX® INET4INT related
	operations.

*******************************************************************************/

#ifndef	INET4INT_INCLUDE
#define	INET4INT_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */


EXTERNC_begin

extern uint	inet4int(cvoid *) noex ;

EXTERNC_end


#endif /* INET4INT_INCLUDE */


