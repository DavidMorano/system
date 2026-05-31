/* usys_fdatasync HEADER */
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
	usys_fdatasync

  	Description:
	This file contains the UNIX® system types that the brain-damaged
	MacOS operating system does NOT have.  We are trying in a very
	small way to make up for some of the immense brain-damage within
	the Apple Darwin operating system.

*******************************************************************************/

#ifndef	USYSFDATASYNC_INCLUDE
#define	USYSFDATASYNC_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<unistd.h>		/* for Solaris® */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysflag.h>

/* FDATASYNC begin */
#if	defined(SYSHAS_FDATASYNC) && (SYSHAS_FDATASYNC > 0)
/******************************************************************************/


/* SunOS (Solaris®) already has the |fdatasync(2)| system call */


/******************************************************************************/
#else /* defined(SYSHAS_FDATASYNC) && (SYSHAS_FDATASYNC > 0) */
/******************************************************************************/


#ifndef	DECLARAION_FDATASYNC
#define	DECLARAION_FDATASYNC
EXTERNC_begin
    extern unixret_t fdatasync(int) noex ;
EXTERNC_end
#endif /* DECLARAION_FDATASYNC */


/******************************************************************************/
#endif /* defined(SYSHAS_FDATASYNC) && (SYSHAS_FDATASYNC > 0) */
/* FDATASYNC end */


#endif /* USYSFDATASYNC_INCLUDE */


