/* ucfdmanip SUPPORT */
/* lang=C++20 */

/* manipulation of file-descriptors */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-07-10, David A­D­ Morano
	This subroutine was originally written.

	= 2023-11-27, , David A­D­ Morano
	Code review. I included an update note in the notes below.
	No code changes.

*/

/* Copyright © 1998,2023 David A­D­ Morano.  All rights reserved. */

/******************************************************************************

	Name:
	uc_duper

	Description:
	This subroutine will duplicate a file desciptor but the new
	file descriptor is guaranteed to be at or above some minimum
	number. This sort of thing is required in a number of
	situations where file descriptors need to be juggled around
	(like around when programs do forks!).

	Synopsis:
	int dupup(int fd,int min) noex

	Arguments:
	fd	file desciptor to duplicate
	min	minimum level to get the duplicate at

	Returns:
	>=0	the new file descriptor
	<0	error


	Name:
	uc_moveup

	Description:
	This subroutine *moves* file descriptors up to some minimum
	number that is specified. This comes in most handy when
	forking child processes with pipes open and which need to
	be duplicated to some place above FD=2 (to a minumum FD=3).

	Synopsis:
	int uc_moveup(int fd,int minfd) noex

	Arguments:
	fd		file descriptor to move up (possibly gets closed)
	minfd		minimum number where new FD will move to

	Returns:
	>=0		the new FD
	<0		error (original is *not* closed in this case)

	Implementation note:
	This subroutine needs to be able to execute after a |fork(2)|
	but before any corresponding |exit(2)|, so we need it to
	be fork-safe (as much as possible). So we do not use
	|malloc(3c)| for any memory allocation. The reason for this
	is because |malloc(3c)| is *not* fork-safe on many (most)
	UNIX® platforms. One exception is that |malloc(3c)| does
	appear to be fork-safe on the Solaris® platform since they
	(AT&T and Sun Microsystems) seemed to be much more concerned
	with thread and fork safety than most of the other UNIX®
	systems out there. But we cannot rely on always executing
	on the Solaris® system, so we need to accommodate poorer
	UNIX® cousins.

	Notes:
	OK, I am laughing a little bit.  It is always -- for some
	strange unknown reason -- to laugh a little bit at what I
	wrote years (or decades) ago. It is sort of a time-warp thing.
	Anyway, I just want to note for posterity that I think that
	most UNIXi now-a-days fully have a |malloc(3c)| that is
	entirely fork-safe.  I remember when fork-safety was even
	a thing being invented (at the dawn of POSIX threads),
	the central example of fork-safety problems used |malloc(3c)|
	as the example problem. Enjoy.

******************************************************************************/

#include	<envstandards.h>
#include	<sys/types.h>
#include	<sys/param.h>
#include	<unistd.h>		/* <- for |getdtablesize(3c)| */
#include	<fcntl.h>
#include	<climits>
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<localmisc.h>

#include	"ucfdmanip.h"


/* local defines */


/* local namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* forward references */

static int	movethem(int,int,int) noex ;


/* exported variables */


/* exported subroutines */

int uc_duper(int fd,int min) noex {
	int		rs = SR_INVALID ;
	int		ufd = -1 ;
	if ((fd >= 0) && (min >= 0)) {
	    if ((rs = u_dup(fd)) >= 0) {
	        ufd = rs ;
	        if (ufd < min) {
	            rs = uc_moveup(ufd,min) ;
	            ufd = rs ;
	            if (rs < 0) uc_close(ufd) ;
	        } /* end if (move needed) */
	    } /* end if (dup) */
	} /* end if (non-null) */
	return (rs >= 0) ? ufd : rs ;
}
/* end subroutine (uc_duper) */

int uc_moveup(int fd,int minfd) noex {
	int		rs = SR_INVALID ;
	if ((fd >= 0) && (minfd >= 0)) {
	    rs = SR_OK ;
	    if (fd < minfd) {
	        cint	nfds = getdtablesize() ;
	        if (minfd < nfds) {
	            rs = movethem(nfds,fd,minfd) ;
	            fd = rs ;
	        } else {
	            rs = SR_MFILE ;
	        }
	    } /* end if (move needed) */
	} /* end if (valid) */
	return (rs >= 0) ? fd : rs ;
}
/* end subroutine (uc_moveup) */


/* local subroutines */

int movethem(int nfds,int fd,int minfd) noex {
	int		rs = SR_OK ;
	int		i = 0 ;
	int		fds[nfds+1] ;	/* <- variable-length-array (VLA) */
	while ((rs >= 0) && (fd < minfd) && (i < nfds)) {
	    fds[i++] = fd ;
	    rs = u_dup(fd) ;
	    fd = rs ;
	} /* end while */
	for (int j = (((rs < 0) || (i == nfds)) ? 1 : 0) ; j < i ; j += 1) {
	    u_close(fds[j]) ;
	}
	if ((rs >= 0) && (i == nfds)) {
	    rs = SR_MFILE ;
	}
	return (rs >= 0) ? fd : rs ;
}
/* end subroutine (movethem) */


