/* uc_openshm SUPPORT */
/* lang=C++20 */

/* interface component for UNIX�® library-3c */
/* open POSIX share-memory */


/* revision history:

	= 1998-07-10, David A­D­ Morano
	This subroutine was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This subroutine opens a POSIX shared-memory ('sem(3rt)')
	object.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<unistd.h>
#include	<fcntl.h>
#include	<cstring>
#include	<usystem.h>
#include	<snx.h>
#include	<strwcpy.h>
#include	<localmisc.h>


/* local defines */


/* external subroutines */


/* local structures */


/* forward references */


/* local variables */


/* exported subroutines */

#if	defined(SYSHAS_PSHM) && (SYSHAS_PSHM > 0)

int uc_openshm(cchar *shmname,int of,mode_t om) noex {
	int		rs = SR_FAULT ;
	int		fd = -1 ;
	if (shmname) {
	    char	*nbuf = nullptr ;
	    rs = SR_OK ;
	    if (shmname[0] != '/') {
	        cint	ns = (strlen(shmname)+2) ;
	        char	*bp ;
	        if ((rs = uc_libmalloc(ns,&bp)) >= 0) {
		    cchar	*a = bp ;
		    *bp++ = '/' ;
		    strwcpy(bp,shmname,-1) ;
		    shmname = a ;
	        }
	    }
	    if (rs >= 0) {
	        repeat {
	            errno = 0 ;
	            if ((rs = shm_open(shmname,of,om)) < 0) {
			rs = (- errno) ;
		    }
		    fd = rs ;
	        } until (rs != SR_INTR) ;
	        if (nbuf != nullptr) uc_libfree(nbuf) ;
	    } /* end if (ok) */
	} /* end if (non-null) */
	return (rs >= 0) ? fd : rs ;
}
/* end subroutine (uc_openshm) */

#else /* defined(SYSHAS_PSHM) && (SYSHAS_PSHM > 0) */

int uc_openshm(cchar *shmname,int,mode_t) noex {
	int		rs = SR_FAULT ;
	if (shmname) {
	    rs = SR_NOSYS ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (uc_openshm) */

#endif /* defined(SYSHAS_PSHM) && (SYSHAS_PSHM > 0) */


