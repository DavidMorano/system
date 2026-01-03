/* usys_isaexec HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* miscelllaneous (XXX) operating system support */
/* version %I% last-modified %G% */


/* revision history:

	= 2025-11-21, David A­D­ Morano
	This module was originally written.

*/

/* Copyright © 2025 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	usys_isaexec

  	Description:
	This file contains the UNIX® system types that the brain-damaged
	MacOS operating system does NOT have.  We are trying in a very
	small way to make up for some of the immense brain-damage within
	the Apple Darwin operating system.

*******************************************************************************/

#ifndef	USYSISAEXEC_INCLUDE
#define	USYSISAEXEC_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<unistd.h>		/* for Solaris® */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysflag.h>

/* ISAEXEC begin */
#if	F_SUNOS
/******************************************************************************/


/* SunOS (Solaris®) already has the |isaexec(2)| system call */


/******************************************************************************/
#else /* F_SUNOS */
/******************************************************************************/


#ifndef	SUBROUTINE_ISAEXEC
#define	SUBROUTINE_ISAEXEC
EXTERNC_begin
    extern unixret_t isaexec(cchar *,char *const *,char *const *) noex ;
EXTERNC_end
#endif /* SUBROUTINE_ISAEXEC */


/******************************************************************************/
#endif /* F_SUNOS */
/* ISAEXEC end */


#endif /* USYSISAEXEC_INCLUDE */


