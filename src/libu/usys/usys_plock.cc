/* usys_plock SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */


/* define some missing or special subroutines for Apple-Darwin */
/* version %I% last-modified %G% */


/* revision history:

	= 2025-11-21, David A-D- Morano
	I wrote this from and using a suggestion from Google Gemini
	AI thing.

*/

/* Copyright © 1998 David A-D- Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	usys_plock

	Description:
	This subroutine is an emulation of the Solaris® subroutine
	(system call) |plock2)|.

	Synopsis:
	int usys_plock(int cmd) noex

	Arguments:
	cmd		one of:
    				MLOCKP_NON
    				MLOCKP_ALL
    				MLOCKP_TXT
    				MLOCKP_DAT

	Returns:
	!=-1		OK
	==-1		error (error number in |errno|)

  	Notes:

	1. This subroutine (below) emulates the Solaris® |plock(2)|
	system call using the standard POSIX® (and Linux) |mlockall(2)|
	and |munlockall(2)| system calls.

	2. The Solaris® PLOCK_DAEMON feature associated with the
	|fork(2)| behavior is NOT emulated, as that is application
	logic.

	3. Original |plock(PLOCK_DAEMON)| also calls |fork(2)|,
	but that is application logic and not part of the
	memory lock emulation.  The locking part is handled above.

	@param op The operation to perform:
		PLOCK_TEXT, PLOCK_DAEMON, PLOCK_INIT, 
		PLOCK_PROT (lock all current+future),
		PLOCK_UNL (unlock all).
	@return 0 on success, -1 on failure (errno is set).

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/mman.h>
#include	<unistd.h>
#include	<cerrno>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<usysflag.h>
#include	<localmisc.h>

#include	"usys_plock.h"


/* local defines */


/* local namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

namespace usys {
    unixret_t usys_plock(int op) noex {
        int rc = 0 ; /* return-value */
        int flags = 0 ;
        switch (op) {
    	case MLOCKP_ALL:
    	case MLOCKP_TXT:
    	case MLOCKP_DAT:
            /****
    		For locking, calling:
    			mlockall(MCL_CURRENT | MCL_FUTURE) 
    		is the standard way to achieve the comprehensive
    		locking that |plock()| provides.
	    ****/
            flags = (MCL_CURRENT | MCL_FUTURE) ;
            if (mlockall(flags) == -1) {
                rc = -1 ; /* ERRNO is set by |mlockall()| */
            }
            break ;
        case MLOCKP_NON:
            /* Unlock all currently locked pages. */
            if (munlockall() == -1) {
                rc = -1 ; /* ERRNO is set by |munlockall()| */
            }
            break;
        default:
            errno = EINVAL ; // Invalid argument
            rc = -1 ;
    	    break ;
        } /* end switch */
        return rc ;
    } /* end subroutine (usys_plock) */
} /* end namespace (usys) */


