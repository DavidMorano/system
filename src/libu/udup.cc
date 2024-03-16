/* udup (UNIX® Memory-Management) */
/* lang=C++20 */

/* UNIX® Memory-Management interface */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-26, David A­D­ Morano
        This subroutine was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Names:
	u_dup
	u_dup2

	Description:
	Duplicate file-descriptors.

*******************************************************************************/

#include	<envstandards.h>
#include	<sys/types.h>
#include	<sys/mman.h>
#include	<unistd.h>
#include	<cerrno>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<usystem.h>
#include	<localmisc.h>

#include	"udup.h"


/* local defines */


/* imported namespaces */

using std::nullptr_t ;			/* type */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct udup ;
    typedef int (udup::*mem_f)(int) noex ;
    struct udup {
	mem_f		m ;
	int		fd2 ;
	udup() noex { } ;
	udup(int f) noex : fd2(f) { } ;
	int operator () (int) noex ;
	int dup1(int) noex ;
	int dup2(int) noex ;
    } ; /* end struct (udup) */
}


/* forward references */


/* local variables */


/* exported subroutines */

int u_dup(int fd) noex {
	udup		udupo ;
	udupo.m = &udup::dup1 ;
	return udupo(fd) ;
}
/* end subroutine (u_dup) */

int u_dup2(int fd,int fd2) noex {
	udup		udupo(fd2) ;
	udupo.m = &udup::dup2 ;
	return udupo(fd) ;
}
/* end subroutine (u_dup2) */


/* local subroutines */

int udup::operator () (int fd) noex {
	int		to_mfile = utimeout[uto_mfile] ;
	int		to_nfile = utimeout[uto_nfile] ;
	int		to_nomem = utimeout[uto_nomem] ;
	int		to_again = utimeout[uto_again] ;
	int		to_nosr = utimeout[uto_nosr] ;
	int		rs = SR_INVALID ;
	bool		f_exit = false ;
	if (fd >= 0) {
	    repeat {
	        if ((rs = (this->*m)(fd)) < 0) {
	            switch (rs) {
	            case SR_MFILE:
	                if (to_mfile-- > 0) {
			    msleep(1000) ;
		        } else {
			    f_exit = true ;
		        }
	                break ;
	            case SR_NFILE:
	                if (to_nfile-- > 0) {
			    msleep(1000) ;
		        } else {
			    f_exit = true ;
		        }
	                break ;
	            case SR_NOSR:
	                if (to_nosr-- > 0) {
			    msleep(1000) ;
		        } else {
			    f_exit = true ;
		        }
	                break ;
	            case SR_NOMEM:
	                if (to_nomem-- > 0) {
			    msleep(1000) ;
		        } else {
			    f_exit = true ;
		        }
	                break ;
	            case SR_AGAIN:
	                if (to_again-- > 0) {
			    msleep(1000) ;
		        } else {
			    f_exit = true ;
		        }
	                break ;
	            case SR_INTR:
		        break ;
	            } /* end switch */
	        } /* end if (some kind of error) */
	    } until ((rs >= 0) || f_exit) ;
	} /* end if (valid) */
	return rs ;
}
/* end subroutine (udup::operator) */

int udup::dup1(int fd) noex {
	int		rs ;
	if ((rs = dup(fd)) < 0) {
	    rs = (- errno) ;
	}
	return rs ;
}
/* end method (udup::dup1) */

int udup::dup2(int fd) noex {
	int		rs = SR_INVALID ;
	if (fd2 >= 0) {
	    if ((rs = dup2(fd2)) < 0) {
	        rs = (- errno) ;
	    }
	}
	return rs ;
}
/* end method (udup::dup2) */


