/* usys_plock HEADER */
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
	usys_plock

  	Description:
	This file contains the UNIX® system types that the brain-damaged
	MacOS operating system does NOT have.  We are trying in a very
	small way to make up for some of the immense brain-damage within
	the Apple Darwin operating system.

	Commands:
	MLOCKP_NON		unlock everything
	MLOCKP_ALL		lock-all
	MLOCKP_TXT		lock-text-segment
	MLOCKP_DAT		lock_data-segment

*******************************************************************************/

#ifndef	USYSPLOCK_INCLUDE
#define	USYSPLOCK_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/mman.h>
#include	<unistd.h>		/* for Solaris® */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>

/* MEMPLOCK begin */
#if	defined(SYSHAS_MEMPLOCK) && (SYSHAS_MEMPLOCK > 0)
/******************************************************************************/


/* SunOS (Solaris®) already has the |plock(2)| system call */


/******************************************************************************/
#else /* defined(SYSHAS_MEMPLOCK) && (SYSHAS_MEMPLOCK > 0) */
#ifdef	__cplusplus /* C++ only */
/******************************************************************************/


#ifndef	SUBROUTINE_PLOCK
#define	SUBROUTINE_PLOCK
namespace usys {
    extern unixret_t usys_plock(int) noex ;
}
#endif /* SUBROUTINE_PLOCK */


/******************************************************************************/
#endif /* __cplusplus (C++ only) */
#endif /* defined(SYSHAS_MEMPLOCK) && (SYSHAS_MEMPLOCK > 0) */
/* MEMPLOCK end */


#endif /* USYSPLOCK_INCLUDE */


