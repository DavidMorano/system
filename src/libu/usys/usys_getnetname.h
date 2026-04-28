/* usys_getnetname HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* miscelllaneous (XXX) operating system support */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-21, David A­D­ Morano
	This module was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This file contains the UNIX® system types that the brain-damaged
	MacOS operating system does NOT have.  We are trying in a very
	small way to make up for some of the immense brain-damage within
	the Apple Darwin operating system.

*******************************************************************************/

#ifndef	USYSGETNETNAME_INCLUDE
#define	USYSGETNETNAME_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>

/* GETNETNAME begin */
#if	(!defined(SYSHAS_ONC)) || (SYSHAS_ONC == 0)


#ifndef	SUBROUTINE_GETNETNAME
#define	SUBROUTINE_GETNETNAME
EXTERNC_begin
/* 0=not-found, 1=found */
extern int	getnetname(char *) noex ;

EXTERNC_end
#endif /* SUBROUTINE_GETNETNAME */


#endif /* (!defined(SYSHAS_ONC)) || (SYSHAS_ONC == 0) */
/* GETNETNAME end */


#endif /* USYSGETNETNAME_INCLUDE */


