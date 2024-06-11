/* ucopenpt SUPPORT */
/* lang=C++20 */

/* open a PTS-style pseudo-terminal */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-01, David A­D­ Morano
	This subroutine was adapted from a previously related subroutine.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	uc_openpt

	Description:
	This subroutine opens a PTS-type pseudo-terminal for use
	on System V Release 3 (SVR3) UNIX® OS type systems.

	Synopsis:
	int uc_openpt(int oflags) noex

	Arguments:
	oflags		open-flags

	Returns:
	>=0		file-descriptor
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<cstdlib>
#include	<usystem.h>


/* local defines */

#ifndef	PTMXFNAME
#define	PTMXFNAME	"/dev/ptmx"
#endif

#if	defined(SYSHAS_OPENPT) && (SYSHAS_OPENPT > 0)
#define	F_OPENPT	1
#else
#define	F_OPENPT	0
#endif


/* external subroutines */


/* external variables */


/* forward references */


/* local variables */

constexpr bool		f_openpt = F_OPENPT ;


/* exported variables */


/* exported subroutines */

int uc_openpt(int oflags) noex {
	int		rs ;
	oflags &= (~ O_ACCMODE) ;
	oflags |= O_RDWR ;
	if_constexpr (f_openpt) {
	    errno = 0 ;
	    if ((rs = posix_openpt(oflags)) < 0) {
	        rs = (- errno) ;
	    }
	} else {
	    rs = u_open(PTMXFNAME,oflags,0662) ;
	} /* end if_constexpr (f_openpt) */
	return rs ;
}
/* end subroutine (uc_openpt) */


