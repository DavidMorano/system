/* fsdir SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* File-System-Direcroy (FSDIR) */
/* version %I% last-modified %G% */


/* revision history:

	= 1999-07-23, David A­D­ Morano
	This module was originally written.

*/

/* Copyright © 1999 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This object facilitates reading directory entries.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */


#if	defined(SYSHAS_GETDENTS) && (SYSHAS_GETDENTS > 0)

#include	"sunos_fsdir.cc"

#else /* defined(SYSHAS_GETDENTS) && (SYSHAS_GETDENTS > 0) */

#include	"darwin_fsdir.cc"

#endif /* defined(SYSHAS_GETDENTS) && (SYSHAS_GETDENTS > 0) */


