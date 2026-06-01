/* usys_isaexec SUPPORT */
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
	usys_isaexec

	Description:
	This subroutine is an emulation of the Solaris® subroutine
	(system call) |isaexec2)|.

	Synopsis:
	int usys_isaexec(cchar *fn,char *const *av,char *const *ev) noex

	Arguments:
	fn		file-name of program to execute
	av		array argument
	ev		array environment

	Returns:
	!=-1		OK
	==-1		error (error number in |errno|)

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<unistd.h>		/* POSIX */
#include	<cerrno>		/* CSTD */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<localmisc.h>

#include	"usys_isaexec.h"

/* ISAEXEC start */
#if	F_SUNOS
/******************************************************************************/


#include	<unistd.h>		/* for SunOS */


/******************************************************************************/
#else /* defined(SYSHAS_ISAEXEC) && (SYSHAS_ISAEXEC > 0) */
/******************************************************************************/


unixret_t isaexec(cchar *fn,char *const *av,char *const *ev) noex {
    	return execve(fn,av,ev) ;
} /* end subroutine (isaexec) */


/******************************************************************************/
#endif /* (!defined(SYSHAS_ISAEXEC)) || (SYSHAS_ISAEXEC == 0) */
/* ISAEXEC end */


