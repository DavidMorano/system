/* usys_nprocs SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* UNIX® system emulated support */
/* version %I% last-modified %G% */


/* revision history:

	= 2001-04-11, David A-D- Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 2001 David A-D- Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	usys_nprocs

	Description:
	I think that there is no standard way (at least not in the
	POSIX® standard) to retrieve the number of processes currently
	on the system (either computing, sleeping or otherwise).
	So I procide something for some operating systems.

	Synopsis:
	int usys_nprocs() noex

	Arguments:
	-

	Returns:
	>=0		number of processes on the system
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>		/* POSIX® |pid_t| */
#include	<climits>		/* CSTD |PATH_MAX| + |INT_MAX| */
#include	<cerrno>		/* CSTD */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */
#include	<usysflag.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

#include	"usys_nprocs.h"

#if	defined(SYSHAS_LIBPROC) && (SYSHAS_LIBPROC > 0) 

#include	<libproc.h>		/* LIBPROC */

#ifdef	COMMENT





#endif /* COMMENT */

int usys_nprocs() noex {
	int	rs = SR_OK ;
	int	n = 0 ; /* return-value */
	errno = 0 ;
	{
	    cint bytes = proc_listallpids(nullptr,0) ;
	    if (errno) {
		rs = (- errno) ;
	    } else {
		n = (bytes / szof(pid_t)) ;
	    }
	} /* end block */
	return (rs >= 0) ? n : rs ;
} /* end subroutine (usys_nprocs) */

#else /* defined(SYSHAS_LIBPROC) && (SYSHAS_LIBPROC > 0) */

int usys_nprocs() noex {
	return SR_NOSYS ;
} /* end subroutine (usys_nprocs) */

#endif /* defined(SYSHAS_LIBPROC) && (SYSHAS_LIBPROC > 0) */


