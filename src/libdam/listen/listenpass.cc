/* listenpass SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* subroutine to listen on a FIFO for pass-FD requests */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-10, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	listenpass

	Description:
	This little subroutine checks for or establishes (if possible)
	a FIFO for listening for passed file descriptors.  This is
	a common method for standing servers to receive new client
	connections.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* POSIX® */
#include	<sys/stat.h>		/* POSIX® */
#include	<unistd.h>		/* POSIX® */
#include	<fcntl.h>		/* POSIX® */
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
	if (passfname) ylikely {
	    rs = SR_INVALID ;
	    if (passfname[0]) ylikely {
	        if ((rs = uc_open(passfname,O_FLAGS1,om)) >= 0) {
	            fd = rs ;
	            if (ustat sb ; (rs = u_fstat(fd,&sb)) >= 0) {
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
} /* end subroutine (listenpass) */


