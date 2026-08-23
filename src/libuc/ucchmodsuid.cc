/* ucchmodsuid */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

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

int uc_chmodsuid(cchar *fname,int f) noex {
	int		rs = SR_FAULT ;
	int		fprev = false ;
	if (fname) ylikely {
	    rs = SR_INVALID ;
	    if (fname[0]) ylikely {
	        if (ustat sb ; (rs = u_stat(fname,&sb)) >= 0) ylikely {
	            mode_t mperm = sb.st_mode ;
	            fprev = ((mperm & S_IXSUID) == S_IXSUID) ;
	            if (! LEQUIV(fprev,f)) {
	                if (f) {
	                    mperm |= S_IXSUID ;
	                } else {
	                    mperm &= (compl S_ISUID) ;
		        }
	                rs = u_chmod(fname,mperm) ;
	            } /* end if (needed a change) */
	        } /* end if (stat) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? fprev : rs ;
} /* end subroutine (uc_chmodsuid) */


