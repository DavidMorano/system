/* u_seekable */

/* translation layer interface for UNIX� equivalents */


#define	CF_DEBUGS	0		/* compile-time debugging */


/* revision history:

	= 1998-11-01, David A�D� Morano
	This subroutine was written for Rightcore Network Services (RNS).

*/

/* Copyright � 1998 David A�D� Morano.  All rights reserved. */


#include	<envstandards.h>

#include	<sys/types.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<errno.h>

#include	<usystem.h>
#include	<localmisc.h>


/* exported subroutines */


int u_seekable(nt fd)
{

	return u_seek(fd,0L,SEEK_CUR) ;
}
/* end subroutine (u_seekable) */


