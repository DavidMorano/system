/* psem SUPPORT */
/* lang=C++20 */

/* POSIX® unnamed-Semaphore (PSEM) */
/* version %I% last-modified %G% */


/* revision history:

	= 1999-07-23, David A­D­ Morano
	This module was originally written.

*/

/* Copyright © 1999 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This module provides the POSIX® standard (unnamed) semaphore
	facility one way or another.  On normal operating systems,
	the whole POSIX semaphore facility (both named and unnamed)
	is readily available. But on the Apple Darwin operating
	system, it is not.  Apple Darwin only supports the named
	POSIX semaphore capability. Why? Because Apple really does
	not give a f*ck number one to POSIX standards.  They (Apple)
	do not have to.  They can just f*ck everyone up the *ss and
	get away with it because ... after all ...  they are Apple!
	So below we have a compile time switch.  If we are compiling
	on a system that supports the the POSIX unnamed semaphores,
	we just used a cleaned up wrapper version of them. But if
	we are on a system (like Apple Darwin) that does not support
	unnamed POSIX semaphores, then we substitute our own
	implemetation of POSIX unnamed semaphores.  Happily, we
	already had a version of counting semaphores that supported
	arbitrary count values on each operation, so we just adjust
	that to emulate the normal POSIX unnamed semaphore interface.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */


#define	PSEM_REDIRECT	1


/* are POSIX unnamed semaphores supported on this platform or not? */
#if	defined(SYSHAS_PSEM) && (SYSHAS_PSEM > 0)

#include	"rpsem.cc"		/* "real" POSIX® semaphores */

#else /* defined(SYSHAS_PSEM) && (SYSHAS_PSEM > 0) */

#include	"epsem.cc"		/* "emulated" POSIX® semaphores */

#endif /* defined(SYSHAS_PSEM) && (SYSHAS_PSEM > 0) */


