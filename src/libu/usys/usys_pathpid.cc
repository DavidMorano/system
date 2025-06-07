/* usys_pathpid SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* define various sytem (global) variables */
/* version %I% last-modified %G% */


/* revision history:

	= 2001-04-11, David A-D- Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 2001 David A-D- Morano.  All rights reserved. */

/*******************************************************************************

  	Group:
	usys_pathpid

	Description:
	Retrieve the full path-name of the executable file associated
	with a given process PID.

	Synopsis:
	int usys_pathpid(char *rbuf,int rlen,pid_t pid) noex

	Arguments:
	rbuf		result buffer pointer
	rlen		result buffer length
	pid		PID of process to retrieve execuatable path for

	Returns:
	>=0		exec-file found and this is its length in result buffer
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */

#include	<unistd.h>		/* |getpid(2)| */
#include	<cerrno>
#include	<climits>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>		/* |getenv(3c)| + |getprogname(3c)| */
#include	<cstring>		/* |strchr(3c)| */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<localmisc.h>

#include	"usys_pathpid.h"

/* USYS_PATHPID start */
#if	defined(SYSHAS_LIBPROC) && (SYSHAS_LIBPROC > 0)

#include	<libproc.h>		/* |proc_pidpath(3proc)| */

namespace usys {
    sysret_t usys_pathpid(char *obuf,int olen,pid_t pid) noex {
	int		rs = SR_FAULT ;
	if (obuf) {
	    rs = SR_INVALID ;
	    if (olen > 0) {
		if ((rs = proc_pidpath(pid,obuf,olen)) < 0) {
		    rs = (- errno) ;
		}
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
    } /* end subroutine (darwin_pathpid) */
    sysret_t usys_namepid(char *obuf,int olen,pid_t pid) noex {
	int		rs = SR_FAULT ;
	if (obuf) {
	    rs = SR_INVALID ;
	    if (olen > 0) {
		if ((rs = proc_name(pid,obuf,olen)) < 0) {
		    rs = (- errno) ;
		}
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
    } /* end subroutine (darwin_namepid) */
} /* end namespace */

#else

namespace usys {
    sysret_t usys_pathpid(char *obuf,int olen,pid_t) noex {
	int		rs = SR_FAULT ;
	if (obuf) {
	    rs = SR_INVALID ;
	    if (olen > 0) {
		rs = SR_NOSYS ;
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
    } /* end subroutine (darwin_pathpid) */
    sysret_t usys_namehpid(char *obuf,int olen,pid_t) noex {
	int		rs = SR_FAULT ;
	if (obuf) {
	    rs = SR_INVALID ;
	    if (olen > 0) {
		rs = SR_NOSYS ;
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
    } /* end subroutine (darwin_namepid) */
} /* end namespace */

#endif /* defined(SYSHAS_LIBPROC) && (SYSHAS_LIBPROC > 0) */
/* USYS_PATHPID finish */


