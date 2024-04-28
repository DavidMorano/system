/* ufiledescbase SUPPORT (UNIX® System V IPC) */
/* lang=C++20 */

/* translation layer interface for UNIX® equivalents */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services (RNS).

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Names:

	Description:
	UNIX® File-Descriptor system calls.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<sys/types.h>
#include	<unistd.h>
#include	<climits>		/* |INT_MAX| */
#include	<cerrno>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstdint>		/* |intptr_t| */
#include	<cstring>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysrets.h>
#include	<usyscalls.h>
#include	<usupport.h>
#include	<clanguage.h>
#include	<localmisc.h>

#include	"ufiledesc.h"


/* local defines */


/* imported namespaces */

using namespace	ufiledesc ;		/* namespace */

using std::nullptr_t ;			/* type */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local variables */


/* exported variables */


/* exported subroutines */

int ufiledescbase::operator () (int fd) noex {
	int		rs = SR_FAULT ;
	int		to_nomem = utimeout[uto_nomem] ;
	int		to_nospc = utimeout[uto_nospc] ;
	int		to_nobufs = utime[uto_nobufs] ;
	int		to_mfile = utimeout[uto_mfile] ;
	bool		f_exit = false ;
	repeat {
	    if ((rs = (this->*m)(fd)) < 0) {
                switch (rs) {
                case SR_NOMEM:
                    if (to_nomem-- > 0) {
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
	        case SR_NOBUFS:
	            if (to_nobufs-- > 0) {
			msleep(1000) ;
		    } else {
			f_exit = true ;
		    }
	            break ;
                case SR_MFILE:
                    if (to_mfile-- > 0) {
                        msleep(1000) ;
                    } else {
                        f_exit = true ;
                    }
                    break ;
                case SR_INTR:
                    break ;
                default:
                    f_exit = true ;
                    break ;
                } /* end switch */
            } /* end if (error) */
	} until ((rs >= 0) || f_exit) ;
	return rs ;
}
/* end method (ufiledescbase::operator) */


