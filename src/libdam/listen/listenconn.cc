/* listenconn SUPPORT */
/* lang=C++20 */

/* subroutine to listen on a mounted FIFO for passed file-descriptors */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-10, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This little subroutine checks for or establishes (if possible)
	a mounted FIFO for listening for passed file descriptors.
	This is a common method for standing servers to receive new
	client connections.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/stat.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<utime.h>		/* for |utime(2)| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<netdb.h>
#include	<usystem.h>
#include	<isnot.h>
#include	<localmisc.h>

#include	"listenconn.h"


/* local defines */

#define	O_FLAGS1	(O_RDWR | O_NONBLOCK)


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

static int	mntcheck(cchar *,mode_t) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int listenconn(cchar *mntfname,mode_t om,int opts) noex {
	int		rs = SR_FAULT ;
	int		fd = -1 ;
	(void) opts ;
	if (mntfname) {
	    rs = SR_INVALID ;
	    if (mntfname[0]) {
	        if ((rs = mntcheck(mntfname,om)) >= 0) {
	            int		pipes[2] ;
	            if ((rs = u_pipe(pipes)) >= 0) {
	                int	cfd = pipes[1] ;	/* client-side */
	                fd = pipes[0] ;			/* server-side */
	                if ((rs = u_ioctl(cfd,I_PUSH,"connld")) >= 0) {
	                    if ((rs = uc_fattach(cfd,mntfname)) >= 0) {
	                        u_close(cfd) ;
	                        cfd = -1 ;
	                        rs = uc_closeonexec(fd,true) ;
			        if (rs < 0) {
	            	            uc_fdetach(mntfname) ;
			        }
		            } /* end if (uc_fattach) */
	                } /* end if (u_ioctl) */
		        if (rs < 0) {
		            u_close(fd) ;
		            fd = -1 ;
		            if (cfd >= 0) {
			        u_close(cfd) ;
			        cfd = -1 ;
		            }
		        }
	            } /* end if (u_pipe) */
	        } /* end if (mntcheck) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? fd : rs ;
}
/* end subroutine (listenconn) */


/* local subroutines */

static int mntcheck(cchar *mntfname,mode_t om) noex {
	cint		am = (R_OK|W_OK) ;
	int		rs ;
	int		f = FALSE ;
	if ((rs = uc_access(mntfname,am)) >= 0) {
	    f = true ;
	} else if (rs == SR_NOENT) {
	    cint	of = (O_CREAT|O_RDWR) ;
	    if ((rs = uc_open(mntfname,of,om)) >= 0) {
	        USTAT	sb ;
		cint	fd = rs ;
	        if ((rs = u_fstat(fd,&sb)) >= 0) {
	            if ((sb.st_mode & S_IWOTH) == 0) {
	                u_fchmod(fd,om) ;
		    }
	        }
		u_close(fd) ;
	    } /* end if (uc_open) */
	} /* end if */
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (mntcheck) */


