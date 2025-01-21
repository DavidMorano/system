/* uc_fchmodsuid SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C20 */

/* interface component for UNIX� library-3c */
/* set or clear the SUID bit on the file permissions mode */

#define	CF_DEBUGS	0		/* compile-time debugging */

/* revision history:

	= 1998-04-13, David A�D� Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright � 1998 David A�D� Morano.  All rights reserved. */

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/uio.h>
#include	<sys/stat.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<poll.h>
#include	<usystem.h>
#include	<localmisc.h>


/* local defines */

#ifndef	S_IXSUID
#define	S_IXSUID	(S_ISUID | S_IXUSR)
#endif


/* forward references */


/* exported subroutines */

int uc_fchmodsuid(int fd,int f) noex {
	USTAT		sb ;
	int		rs ;
	int		fperm ;
	int		f_previous = false ;
	if ((rs = u_fstat(fd,&sb)) >= 0) {
	    fperm = sb.st_mode ;
	    f_previous = ((fperm & S_IXSUID) == S_IXSUID) ? 1 : 0 ;
	    if (! LEQUIV(f_previous,f)) {
	        if (f) {
	            fperm |= S_IXSUID ;
	        } else {
	            fperm &= (~ S_ISUID) ;
		}
	        rs = u_fchmod(fd,fperm) ;
	    } /* end if (needed a change) */
	} /* end if (stat) */
	return (rs >= 0) ? f_previous : rs ;
}
/* end subroutine (uc_fchmodsuid) */


