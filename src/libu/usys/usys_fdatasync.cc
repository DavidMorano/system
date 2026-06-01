/* usys_fdatasync SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */


/* define some missing or special subroutines for Apple-Darwin */
/* version %I% last-modified %G% */


/* revision history:

	= 2025-11-21, David A-D- Morano
	I wrote this from and using a suggestion from Google Gemini
	AI thing.

*/

/* Copyright © 1998 David A-D- Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	usys_fdatasync

	Description:
	This subroutine is an emulation of the Solaris® subroutine
	(system call) |fdatasync2)|.

	Synopsis:
	int usys_fdatasync(cchar *fn,char *const *av,char *const *ev) noex

	Arguments:
	fn		file-name of program to execute
	av		array argument
	ev		array environment

	Returns:
	!=-1		OK
	==-1		error (error number in |errno|)

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<unistd.h>
#include	<cerrno>		/* CSTD */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<localmisc.h>

#include	"usys_fdatasync.h"

/* FDATASYNC start */
#if	defined(SYSHAS_FDATASYNC) && (SYSHAS_FDATASYNC > 0)
/******************************************************************************/


#include	<unistd.h>		/* for SunOS */


/******************************************************************************/
#else /* defined(SYSHAS_FDATASYNC) && (SYSHAS_FDATASYNC > 0) */
/******************************************************************************/


unixret_t fdatasync(int fd) noex {
    	return fsync(fd) ;
} /* end subroutine (fdatasync) */


/******************************************************************************/
#endif /* (!defined(SYSHAS_FDATASYNC)) || (SYSHAS_FDATASYNC == 0) */
/* FDATASYNC end */


