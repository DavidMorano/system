/* ucfsync SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* interface component for UNIX® library-3c */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-13, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* POSIX */
#include	<unistd.h>		/* POSIX */
#include	<fcntl.h>		/* POSIX */
#include	<cerrno>		/* CSTD */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<usupport.h>		/* LIBU */
#include	<utimeout.h>		/* LIBU */
#include	<ucdesc.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */

#include	"ucfsync.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int uc_fsync(int fd) noex {
	int		rs = SR_BADFD ;
	if (fd >= 0) {
	    int		to_again = utimeout[uto_again] ;
	    int		to_nospc = utimeout[uto_nospc] ;
	    bool	fexit = false ;
	    repeat {
	        if ((rs = fsync(fd)) < 0) {
		    rs = (neg errno) ;
	            switch (rs) {
	            case SR_AGAIN:
	                if (to_again-- > 0) {
	                    msleep(1000) ;
		        } else {
	                    fexit = true ;
	                }
		        break ;
	            case SR_NOSPC:
	                if (to_nospc-- > 0) {
	                    msleep(1000) ;
		        } else {
	                    fexit = true ;
	                }
		        break ;
	            case SR_INTR:
	                break ;
		    default:
		        fexit = true ;
		        break ;
	            } /* end switch */
	        } /* end if (error) */
	    } until ((rs >= 0) || fexit) ;
	} /* end if (valid) */
	return rs ;
} /* end subroutine (uc_fsync) */

int uc_fsyncdata(int fd) noex {
	int		rs = SR_BADFD ;
	if (fd >= 0) {
	    int		to_again = utimeout[uto_again] ;
	    int		to_nospc = utimeout[uto_nospc] ;
	    bool	fexit = false ;
	    repeat {
	        if ((rs = fdatasync(fd)) < 0) {
		    rs = (neg errno) ;
	            switch (rs) {
	            case SR_AGAIN:
	                if (to_again-- > 0) {
	                    msleep(1000) ;
		        } else {
	                    fexit = true ;
	                }
		        break ;
	            case SR_NOSPC:
	                if (to_nospc-- > 0) {
	                    msleep(1000) ;
		        } else {
	                    fexit = true ;
	                }
		        break ;
	            case SR_INTR:
	                break ;
	            default:
		        fexit = true ;
	                break ;
	            } /* end switch */
	        } /* end if (error) */
	    } until ((rs >= 0) || fexit) ;
	} /* end if (valid) */
	return rs ;
} /* end subroutine (uc_fsyncdata) */


