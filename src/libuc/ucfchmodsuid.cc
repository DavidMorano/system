/* ucfchmodsuid SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* interface component for UNIX® library-3c */
/* set or clear the SUID bit on the file permissions mode */

#define	CF_DEBUGS	0		/* compile-time debugging */

/* revision history:

	= 1998-04-13, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* POSIX® */
#include	<sys/uio.h>		/* POSIX® */
#include	<sys/stat.h>		/* POSIX® */
#include	<unistd.h>		/* POSIX® */
#include	<fcntl.h>		/* POSIX® */
#include	<poll.h>		/* POSIX® */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */


/* local defines */

#ifndef	S_IXSUID
#define	S_IXSUID	(S_ISUID | S_IXUSR)
#endif


/* forward references */


/* exported subroutines */

int uc_fchmodsuid(int fd,int f) noex {
	int		rs ;
	int		fprev = false ;
	if (ustat sb ; (rs = u_fstat(fd,&sb)) >= 0) {
	    mode_t fmod = sb.st_mode ;
	    fprev = ((fmod & S_IXSUID) == S_IXSUID) ;
	    if (! LEQUIV(fprev,f)) {
	        if (f) {
	            fmod |= S_IXSUID ;
	        } else {
	            fmod &= (~ S_ISUID) ;
		}
	        rs = u_fchmod(fd,fmod) ;
	    } /* end if (needed a change) */
	} /* end if (stat) */
	return (rs >= 0) ? fprev : rs ;
} /* end subroutine (uc_fchmodsuid) */


