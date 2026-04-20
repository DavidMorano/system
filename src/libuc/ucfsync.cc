/* ucfsync */
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
#include	<sys/types.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<cerrno>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>		/* |getenv(3c)| */
#include	<clanguage.h>
#include	<usysbase.h>
#include	<usyscalls.h>
#include	<usupport.h>
#include	<utimeout.h>
#include	<localmisc.h>


/* local defines */


/* external subroutines */


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
		    rs = (- errno) ;
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
}
/* end subroutine (uc_fsync) */

int uc_fsyncdata(int fd) noex {
	int		rs = SR_BADFD ;
	if (fd >= 0) {
	    int		to_again = utimeout[uto_again] ;
	    int		to_nospc = utimeout[uto_nospc] ;
	    bool	fexit = false ;
	    repeat {
	        if ((rs = fdatasync(fd)) < 0) {
		    rs = (- errno) ;
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
}
/* end subroutine (uc_fsyncdata) */


