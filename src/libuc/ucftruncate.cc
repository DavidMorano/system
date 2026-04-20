/* ucftruncate */
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
#include	<sys/uio.h>
#include	<sys/stat.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<poll.h>
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

#define	TO_AGAIN	5


/* external subroutines */


/* forward references */

local int	uc_ftruncater(int,off_t) noex ;


/* exported variables */


/* exported subroutines */

int uc_ftruncate(int fd,off_t len) noex {
	int		rs = SR_BADFD ;
	if (fd >= 0) {
	    rs = SR_OK ;
	    if (len < 0) {
	        rs = u_seeko(fd,0L,SEEK_CUR,&len) ;
	    }
	    if (rs >= 0) {
	        rs = uc_ftruncater(fd,len) ;
	    }
	} /* end if (valid) */
	return rs ;
}
/* end suboutine (uc_ftruncate) */


/* local subroutines */

local int uc_ftruncater(int fd,off_t len) noex {
	int		rs ;
	int		to_again = utimeout[uto_again] ;
	bool		fexit = false ;
	repeat {
	    if ((rs = ftruncate(fd,len)) < 0) {
		rs = (- errno) ;
	        switch (rs) {
	        case SR_AGAIN:
	            if (to_again-- > 0) {
			msleep(10) ;
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
	return rs ;
}
/* end subroutine (uc_ftruncater) */


