/* usys_getprocuid SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* UNIX® system emulated support */
/* version %I% last-modified %G% */

#define	CF_DEBUG	1	/* debugging */

/* revision history:

	= 2001-04-11, David A-D- Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 2001 David A-D- Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	usys_getprocuid

	Description:
	Retrieve a process user-ID (UID) given its process-ID (PID).

	Synopsis:
	sysret_t getprocuid(pid_t pid noex

	Arguments:
	pid		PID of process to retrieve UID from

	Returns:
	>=0		UID of the specificed process
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>		/* POSIX® |pid_t| */
#include	<cerrno>		/* CSTD */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<new>			/* C++STD */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */
#include	<usysflag.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */
#include	<dprint.hh>		/* LIBU |DPRINTF(3u)| */

#include	"usys_getprocuid.h"

#ifndef	CF_DEBUG
#define	CF_DEBUG	0		/* debugging */
#endif

cbool		f_debug		= CF_DEBUG ;

#if	defined(SYSHAS_LIBPROC) && (SYSHAS_LIBPROC > 0)

#include	<libproc.h>		/* LIBPROC */

sysret_t getprocuid(pid_t pid) noex {
    	cnothrow	nt{} ;
    	int		rs = SR_INVALID ;
	int		uid = 0 ; /* return-value */
	DPRINTF("ent pid=%u\n",pid) ;
	if (pid >= 0) {
	    cint psz = szof(proc_bsdshortinfo) ;
	    rs = SR_BUGCHECK ;
	    if (psz == PROC_PIDT_SHORTBSDINFO_SIZE) {
	        rs = SR_NOMEM ;
	        if (proc_bsdshortinfo *pp = new(nt) proc_bsdshortinfo ; pp) {
		    cint cmd = PROC_PIDT_SHORTBSDINFO ;
		    if ((rs = proc_pidinfo(pid,cmd,0,pp,psz)) >= 0) {
			uid = pp->pbsi_uid ;
			DPRINTF("got rs=%d uid=%u\n",rs,uid) ;
		    } else {
			rs = (neg errno) ;
		    }
		    delete pp ;
	        } /* end if (new-proc_bsdshortinfo) */
	    } /* end if (bug-check) */
	} /* end if (valid) */
	DPRINTF("ret rs=%d uid=%u\n",rs,uid) ;
	return (rs >= 0) ? uid : rs ;
} /* end subroutine (usys_getprocuid) */

#else /* defined(SYSHAS_LIBPROC) && (SYSHAS_LIBPROC > 0) */

sysret_t getprocuid(pid_t pid) noex {
    	int	rs = SR_INVALID ;
	if (pid >= 0) {
	    rs = SR_NOSYS ;
	} /* end if (valid) */
	return rs ;
} /* end subroutine (getprocuid) */

#endif /* defined(SYSHAS_LIBPROC) && (SYSHAS_LIBPROC > 0) */


