/* u_getdents */
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
	int u_getdents(int fd,struct dirent *dbuf,int dsize) noex

	Returns:
	>=0		size of data returned
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>
#include	<sys/types.h>
#include	<sys/dirent.h>
#include	<errno.h>
#include	<usystem.h>
#include	<localmisc.h>


/* local defines */


/* external subroutines */


/* exported subroutines */

int u_getdents(int fd,DIRENT *dbuf,int dsize) noex {
	int		rs ;
	repeat {
	    if ((rs = getdents(fd,dbuf,dsize)) < 0) rs = (- errno) ;
	} until (rs != SR_INTR) ;
	return rs ;
}
/* end subroutine (u_getdents) */


