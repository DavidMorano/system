/* u_mknod */
/* lang=C++23 */

/* translation layer interface for UNIX® equivalents */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services
	(RNS).

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#include	<envstandards.h>
#include	<sys/types.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<errno.h>
#include	<usystem.h>
#include	<localmisc.h>


/* local defines */


/* external subroutines */


/* exported subroutines */

int u_mknod(cchar *path,mode_t m,dev_t dev) noex {
	int		rs = SR_FAULT ;
	if (path) {
	    rs = SR_INVALID ;
	    if (path[0]) {
		int	to_dquot = utimeout[uto_dquot] ;
		int	to_nospc = utimeout[uto_nospc] ;
		bool	f_exit = false ;
	        repeat {
	            if ((rs = mknod(path,m,dev)) < 0) rs = (- errno) ;
	            if (rs < 0) {
	                switch (rs) {
	                case SR_DQUOT:
	                    if (to_dquot -- > 0) {
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
/* end subroutine (u_mknod) */


