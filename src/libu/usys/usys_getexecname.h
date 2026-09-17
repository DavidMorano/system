/* usys_getexecname HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* Apple Darwin operating system support */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-21, David A­D­ Morano
	This module was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	usys_getexecname

	Description:
	This file contains the UNIX® system types or subroutines
	that the brain-damaged MacOS operating system does NOT have.
	We are trying in a very small way to make up for some of
	the immense brain-damage within the Apple Darwin operating
	system.

*******************************************************************************/

#ifndef	USYSGETEXECNAME_INCLUDE
#define	USYSGETEXECNAME_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>		/* POSIX® system-types */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */
#include	<usysrets.h>		/* LIBU */


#if	defined(SYSHAS_GETEXECNAME) && (SYSHAS_GETEXECNAME > 0)
/******************************************************************************/



/******************************************************************************/
#else /* defined(SYSHAS_GETEXECNAME) && (SYSHAS_GETEXECNAME > 0) */
/******************************************************************************/

#ifndef	GETEXECNAME_DECLARATION
#define	GETEXECNAME_DECLARATION
EXTERNC_begin
extern cchar	*getexecname() noex ;
EXTERNC_end
#endif /* GETEXECNAME_DECLARATION */

/******************************************************************************/
#endif /* defined(SYSHAS_GETEXECNAME) && (SYSHAS_GETEXECNAME > 0) */


#endif /* USYSGETEXECNAME_INCLUDE */


