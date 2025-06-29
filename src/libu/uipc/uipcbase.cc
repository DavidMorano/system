/* uipcbase SUPPORT (UNIX® System V IPC) */
/* charset=ISO8859-1 */
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
	UNIX® System V Inter-Process-Communication (IPC)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<sys/types.h>
#include	<sys/ipc.h>		/* UNIX® Sys-V IPC */
#include	<sys/shm.h>		/* UNIX® Sys-V IPC - SHM */
#include	<sys/msg.h>		/* UNIX® Sys-V IPC - MSG */
#include	<sys/sem.h>		/* UNIX® Sys-V IPC - SEM */
#include	<unistd.h>
#include	<climits>		/* |INT_MAX| */
#include	<cerrno>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdint>		/* |intptr_t| */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysrets.h>
#include	<usyscalls.h>
#include	<usupport.h>
#include	<localmisc.h>

#include	"uipc.h"


/* local defines */


/* imported namespaces */

using std::nullptr_t ;			/* type */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local variables */


/* exported variables */


/* exported subroutines */

namespace uipc {
    int uipcbase::operator () (int id,void *vp) noex {
	int		rs = SR_FAULT ;
	int		to_nomem = utimeout[uto_nomem] ;
	int		to_nospc = utimeout[uto_nospc] ;
	int		to_mfile = utimeout[uto_mfile] ;
	bool		f_exit = false ;
	repeat {
	    if ((rs = (this->*m)(id,vp)) < 0) {
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
    } /* end method (uipcbase::operator) */
}

