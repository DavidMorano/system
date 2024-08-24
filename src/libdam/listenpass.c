/* listenpass SUPPORT */
/* lang=C++20 */

/* subroutine to listen on a FIFO for pass-FD requests */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-10, David A­D­ Morano
	This subroutine was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This little subroutine checks for or establishes (if possible)
	a FIFO for listening for passed file descriptors.  This is
	a common method for standing servers to receive new client
	connections.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/stat.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<netdb.h>
#include	<usystem.h>
#include	<isnot.h>
#include	<localmisc.h>

#include	"listenpass.h"


/* local defines */

#define	O_FLAGS1	(O_RDWR | O_NONBLOCK)


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int listenpass(cchar *passfname,mode_t om,int opts) noex {
	int		rs = SR_FAULT ;
	int		fd = -1 ;
	(void) opts ;
	if (passfname) {
	    rs = SR_INVALID ;
	    if (passfname[0]) {
	        if ((rs = uc_open(passfname,O_FLAGS1,om)) >= 0) {
	            USTAT	sb ;
	            fd = rs ;
	            if ((rs = u_fstat(fd,&sb)) >= 0) {
	                if ((sb.st_mode & S_IWOTH) == 0) {
	                    u_fchmod(fd,om) ;
		        }
	            } else {
	                u_close(fd) ;
	            }
	        } else if (isNotPresent(rs)) {
	            u_unlink(passfname) ;
	            if ((rs = uc_mkfifo(passfname,om)) >= 0) {
	                rs = u_open(passfname,O_FLAGS1,om) ;
	                fd = rs ;
	            }
	        } /* end if */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? fd : rs ;
}
/* end subroutine (listenpass) */


