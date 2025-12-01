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

  	Description:
	This file contains the UNIX® system types that the brain-damaged
	MacOS operating system does NOT have.  We are trying in a very
	small way to make up for some of the immense brain-damage within
	the Apple Darwin operating system.

*******************************************************************************/

#ifndef	USYSPLOCK_INCLUDE
#define	USYSPLOCK_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/time.h>		/* for all others |gettimeofday(3c)| */
#include	<unistd.h>		/* for Solaris® */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>

/* MEMPLOCK begin */
#if	defined(SYSHAS_MEMPLOCK) && (SYSHAS_MEMPLOCK > 0)

/* SunOS (Solaris®) already has the |plock(2)| system call */

#else /* defined(SYSHAS_MEMPLOCK) && (SYSHAS_MEMPLOCK > 0) */
#ifdef	__cplusplus /* C++ only! */

#ifndef	SUBROUTINE_STIME
#define	SUBROUTINE_STIME
namespace usys {
    extern unixret_t usys_plock(int) noex ;
}
#endif /* SUBROUTINE_STIME */

#endif /* __cplusplus (C++ only) */
#endif /* defined(SYSHAS_MEMPLOCK) && (SYSHAS_MEMPLOCK > 0) */
/* MEMPLOCK end */


#endif /* USYSPLOCK_INCLUDE */


