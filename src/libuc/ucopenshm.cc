/* ucopenshm SUPPORT */
/* lang=C++20 */

/* interface component for UNIX® library-3c */
/* open POSIX share-memory (w/ time-out) */


/* revision history:

	= 1998-07-10, David A­D­ Morano
	This subroutine was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	uc_openshmto

	Description:
	This subroutine opens a POSIXÂ® shared-memory (|sem(3rt)|)
	object but with a time-out. What does this mean to have a
	time-out while trying to open a shared memory segment? It
	means that if the segment is access protected, we continue
	to try until the time-out value has expired.

	Synopsis:
	int uc_openshmto(cchar *shmname,int of,mode_t om,int to) noex

	Arguments:
	shmname		string representing the name of the shared memory
	of		open flags
	om		open mode
	to		time-out

	Returns:
	>=0		OK (file-descriptor)
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<unistd.h>
#include	<climits>		/* |INT_MAX| */
#include	<cstring>		/* |strlen(3c)| */
#include	<cerrno>
#include	<usystem.h>
#include	<usupport.h>
#include	<strwcpy.h>
#include	<localmisc.h>


/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* local structures */


/* forward references */


/* local variables */


/* exported subroutines */

int uc_openshmto(cchar *shmname,int of,mode_t om,int to) noex {
	int		rs = SR_FAULT ;
	int		fd = -1 ;
	if (to < 0) to = INT_MAX ;
	if (shmname) {
	    rs = SR_INVALID ;
	    if (shmname[0]) {
	        for (int i = 0 ; to-- >= 0 ; i += 1) {
	            if (i > 0) msleep(1000) ;
	            rs = uc_openshm(shmname,of,om) ;
	            fd = rs ;
	            if (rs != SR_ACCESS) break ;
	        } /* end while */
	        if (rs == SR_ACCESS) rs = SR_TIMEDOUT ;
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? fd : rs ;
}
/* end subroutine (uc_openshmto) */

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
	        if (nbuf) uc_libfree(nbuf) ;
	    } /* end if (ok) */
	} /* end if (non-null) */
	return (rs >= 0) ? fd : rs ;
}
/* end subroutine (uc_openshm) */


