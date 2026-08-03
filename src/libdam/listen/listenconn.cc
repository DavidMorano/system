/* listenconn SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* subroutine to listen on a mounted FIFO for passed file-descriptors */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-10, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	listenconn

	Description:
	This little subroutine checks for or establishes (if possible)
	a mounted FIFO for listening for passed file descriptors.
	This is a common method for standing servers to receive new
	client connections.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* POSIX® */
#include	<sys/stat.h>		/* POSIX® */
#include	<unistd.h>		/* POSIX® */
#include	<fcntl.h>		/* POSIX® */
#include	<utime.h>		/* POSIX® |utime(2)| */
#include	<netdb.h>		/* POSIX® */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<ucmem.h>		/* LIBUC */
#include	<ucopen.h>		/* LIBUC */
#include	<ucdesc.h>		/* LIBUC */
#include	<ucfileop.h>		/* LIBUC */
#include	<isnot.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */

#include	"listenconn.h"


/* local defines */

#define	O_FLAGS1	(O_RDWR | O_NONBLOCK)


/* imported namespaces */

using libuc::mem ;			/* variable */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

local int	mntcheck(cchar *,mode_t) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int listenconn(cchar *mntfname,mode_t om,int opts) noex {
	int		rs = SR_FAULT ;
	int		fd = -1 ;
	(void) opts ;
	if (mntfname) ylikely {
	    rs = SR_INVALID ;
	    if (mntfname[0]) ylikely {
	        if ((rs = mntcheck(mntfname,om)) >= 0) ylikely {
	            if (int pipes[2] ; (rs = u_pipe(pipes)) >= 0) ylikely {
	                int	cfd = pipes[1] ;	/* client-side */
	                fd = pipes[0] ;			/* server-side */
	                if ((rs = u_ioctl(cfd,I_PUSH,"connld")) >= 0) {
	                    if ((rs = uc_fattach(cfd,mntfname)) >= 0) {
	                        u_close(cfd) ;
	                        cfd = -1 ;
	                        rs = uc_closeonexec(fd,true) ;
			        if (rs < 0) {
	            	            uc_detach(mntfname) ;
			        } /* end if (error) */
		            } /* end if (uc_fattach) */
	                } /* end if (u_ioctl) */
		        if (rs < 0) {
		            u_close(fd) ;
		            fd = -1 ;
		            if (cfd >= 0) {
			        u_close(cfd) ;
			        cfd = -1 ;
		            }
		        } /* end if (error) */
	            } /* end if (u_pipe) */
	        } /* end if (mntcheck) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? fd : rs ;
} /* end subroutine (listenconn) */


/* local subroutines */

local int mntcheck(cchar *mntfname,mode_t om) noex {
	cint		am = (R_OK|W_OK) ;
	int		rs ;
	int		rs1 ;
	int		f = false ; /* return-value */
	if ((rs = uc_access(mntfname,am)) >= 0) {
	    f = true ;
	} else if (rs == SR_NOENT) {
	    cint	of = (O_CREAT|O_RDWR) ;
	    if ((rs = uc_open(mntfname,of,om)) >= 0) {
		cint	fd = rs ;
	        if (ustat sb ; (rs = u_fstat(fd,&sb)) >= 0) {
	            if ((sb.st_mode & S_IWOTH) == 0) {
	                u_fchmod(fd,om) ;
		    }
	        }
		rs1 = uc_close(fd) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (uc_open) */
	} /* end if */
	return (rs >= 0) ? f : rs ;
} /* end subroutine (mntcheck) */


