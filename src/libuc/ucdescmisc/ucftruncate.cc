/* ucftruncate SUPPORT */
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
#include	<sys/types.h>		/* POSIX® */
#include	<sys/uio.h>		/* POSIX® */
#include	<sys/stat.h>		/* POSIX® */
#include	<unistd.h>		/* POSIX® */
#include	<fcntl.h>		/* POSIX® */
#include	<poll.h>		/* POSIX® */
#include	<cerrno>		/* CSTD */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<usupport.h>		/* LIBU */
#include	<utimeout.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

#include	"ucftruncate.h"

/* local defines */

#define	TO_AGAIN	5


/* external subroutines */


/* forward references */

local int	ucftruncater(int,off_t) noex ;
local sysret_t	std_ftruncate(int,off_t) noex ;


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
	        rs = ucftruncater(fd,len) ;
	    }
	} /* end if (valid) */
	return rs ;
} /* end suboutine (uc_ftruncate) */


/* local subroutines */

local int ucftruncater(int fd,off_t len) noex {
	int		rs ;
	int		to_again = utimeout[uto_again] ;
	bool		fexit = false ;
	repeat {
	    if ((rs = std_ftruncate(fd,len)) < 0) {
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
} /* end subroutine (ucftruncater) */

local sysret_t std_ftruncate(int fd,off_t len) noex {
	int		rs ;
	if ((rs = ftruncate(fd,len)) < 0) {
	    rs = (neg errno) ;
	}
	return rs ;
} /* end subroutine (std_ftruncate) */


