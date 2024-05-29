/* ugetdents SUPPORT */
/* lang=C20 */

/* translation layer interface for UNIX® equivalents */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services (RNS).

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	u_getdents

	Description:
	This subroutine is provided by some operating systems (like
	Solaris®, IllumOs®, and others) to retieve directory entries
	given a file-descriptor open on a directory. Not all operating
	systems support the used of this subroutine when the calling
	process is running in the ULP32 execution model (I will
	give no names at this time: Apple Darwin). So the use of
	this subroutine on wirdo systems (like Apple Darwin) should
	be avoided.

	Synopsis:
	int u_getdents(int fd,struct dirent *dbuf,int dsz) noex

	Returns:
	>=0		size of data returned
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<sys/types.h>
#include	<sys/dirent.h>
#include	<errno.h>
#include	<usystem.h>
#include	<usysflag.h>
#include	<localmisc.h>

#include	"usyscalls.h"


/* local defines */


/* external subroutines */


/* exported subroutines */

#if	defined(SYSHAS_GETDENTS) && (SYSHAS_GETDENTS > 0) 

int u_getdents(int fd,DIRENT *dbuf,int dsz) noex {
	csize		ssz = size_t(dsz) ;
	int		rs = SR_FAULT ;
	if (dbuf) {
	    rs = SR_INVALID ;
	    if (dsz >= 0) {
		rs = SR_BADF ;
		if (fd >= 0) {
	            repeat {
	                if ((rs = getdents(fd,dbuf,ssz)) < 0) {
		            rs = (- errno) ;
	                }
	            } until (rs != SR_INTR) ;
	        } /* end if (good FD) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (u_getdents) */

#else /* defined(SYSHAS_GETDENTS) && (SYSHAS_GETDENTS > 0) */

int u_getdents(int fd,DIRENT *dbuf,int dsz) noex {
	int		rs = SR_FAULT ;
	if (dbuf) {
	    rs = SR_INVALID ;
	    if (dsz >= 0) {
	        rs = SR_BADF ;
	        if (fd >= 0) {
	            rs = SR_NOSYS ;
	        } /* end if (not BADFD) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}

#endif /* defined(SYSHAS_GETDENTS) && (SYSHAS_GETDENTS > 0) */

