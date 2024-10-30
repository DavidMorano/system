/* usys_pipes HEADER */
/* encoding=ISO8859-1 */
/* lang=C20 */

/* miscelllaneous (XXX) operating system support */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-21, David A­D­ Morano
	This module was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	System:
	usys_linux

	Description:
	This file contains the UNIX® system types that the brain-damaged
	MacOS operating system does NOT have.  We are trying in a
	very small way to make up for some of the immense brain-damage
	within the Apple Darwin operating system.

*******************************************************************************/

#ifndef	USYSPIPES_INCLUDE
#define	USYSPIPES_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */

/* PIPES begin */
#if	(!defined(SYSHAS_PIPES)) || (SYSHAS_PIPES == 0)


#include	<sys/random.h>		/* |getentropy(2)| */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>


/* the call is actually named |pipe2(2)| */
#ifndef	SUBROUTINE_PIPES
#define	SUBROUTINE_PIPES
EXTERNC_begin

extern unixret_t	pipe2(int *,int) noex ;

EXTERNC_end
#endif /* SUBROUTINE_PIPES */


#endif /* (!defined(SYSHAS_PIPES)) || (SYSHAS_PIPES == 0) */
/* PIPES end */


#endif /* USYSPIPES_INCLUDE */


