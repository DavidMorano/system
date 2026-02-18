/* usys_task HEADER */
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
	usys_task

	Description:
	This file contains the UNIX® system types that the brain-damaged
	MacOS operating system does NOT have.  We are trying in a very
	small way to make up for some of the immense brain-damage within
	the Apple Darwin operating system.

*******************************************************************************/

#ifndef	USYSTASK_INCLUDE
#define	USYSTASK_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>		/* system-types + |pid_t| */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>


#if	defined(SYSHAS_EACCESS) && (SYSHAS_EACCESS > 0)
/******************************************************************************/


#include	<unistd.h>


/******************************************************************************/
#else /* defined(SYSHAS_EACCESS) && (SYSHAS_EACCESS > 0) */
/******************************************************************************/


#ifndef	SUBROUTINE_EACCESS
#define	SUBROUTINE_EACCESS
EXTERNC_begin
extern unixret_t	eaccess(const char *,int) noexcept ;
EXTERNC_end
#endif /* SUBROUTINE_EACCESS */


/******************************************************************************/
#endif /* defined(SYSHAS_EACCESS) && (SYSHAS_EACCESS > 0) */


#endif /* USYSTASK_INCLUDE */


