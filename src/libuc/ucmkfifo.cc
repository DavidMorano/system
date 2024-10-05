/* ucmkfifo SUPPORT */
/* lang=C++20 */

/* interface component for UNIX® library-3c */
/* file-system FIFO creation */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-13, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#include	<envstandards.h>	/* MUST be first to configure */
#include	<unistd.h>
#include	<fcntl.h>
#include	<cerrno>
#include	<usystem.h>
#include	<usupport.h>
#include	<localmisc.h>


/* local defines */


/* external subroutines */


/* exported variables */


/* exported subroutines */

int uc_mkfifo(cchar *path,mode_t om) noex {
	int		rs = SR_FAULT ;
	if (path) {
	    rs = SR_INVALID ;
	    if (path[0]) {
	        int		to_io = utimeout[uto_io] ;
	        int		to_dquot = utimeout[uto_dquot] ;
	        int		to_nospc = utimeout[uto_nospc] ;
	        int		f_exit = false ;
	        repeat {
	            if ((rs = mkfifo(path,om)) < 0) rs = (- errno) ;
	            if (rs < 0) {
	                switch (rs) {
	                case SR_IO:
	                    if (to_io-- > 0) {
		                msleep(1000) ;
		            } else {
			        f_exit = true ;
		            }
		            break ;
	                case SR_DQUOT:
	                    if (to_dquot-- > 0) {
		                msleep(1000) ;
		            } else {
			        f_exit = true ;
		            }
		            break ;
	                case SR_NOSPC:
	                    if (to_nospc-- > 0) {
		                msleep(1000) ;
		            } else {
			        f_exit = true ;
		            }
		            break ;
	                case SR_AGAIN:
	                case SR_INTR:
	                    break ;
		        default:
		            f_exit = true ;
		            break ;
	               } /* end switch */
	            } /* end if (error) */
	        } until ((rs >= 0) || f_exit) ;
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (uc_mkfifo) */


