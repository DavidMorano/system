/* psem INCLUDE */
/* lang=C20 */


/* revision history:

	= 1999-07-28, David A-D- Morano
	Originally written for Rightcore Network Services.

*/

/*******************************************************************************

	These are POSIX semaphores.
	Sss the corresponding main C-language source file for more
	information on what is going on here.

*******************************************************************************/

/* Copyright � 1999 David A�D� Morano.  All rights reserved. */


#if	defined(SYSHAS_PSEM) && (SYSHAS_PSEM > 0)

#include	<rpsem.h>

#else /* defined(SYSHAS_PSEM) && (SYSHAS_PSEM > 0) */

#include	<epsem.h>

#endif /* defined(SYSHAS_PSEM) && (SYSHAS_PSEM > 0) */


