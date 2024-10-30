/* u_resolvepath SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* resolve a path without symbolic components */
/* translation layer interface for UNIX® equivalents */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services (RNS).

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	u_resolvepath

	Description:
	This subroutine takes an existing path and creates a new
	path that does not contain any symbolic components.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<unistd.h>
#include	<errno.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<usystem.h>
#include	<localmisc.h>


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

int u_resolvepath(char *rbuf,int rlen,cchar *input) noex {
	int		rs = SR_FAULT ;
	if (rbuf && input) {
	    rs = SR_INVALID ;
	    rbuf[0] = '\0' ;
	    if (input[0]) {
		csize	rsz = size_t(rlen) + 1 ;
	        repeat {
	            if ((rs = resolvepath(input,rbuf,rsz)) >= 0) {
		        if (rs <= rlen) {
			    rbuf[rs] = '\0' ;
		        } else {
			    rbuf[rlen] = '\0' ;
			    rs = SR_OVERFLOW ;
		        }
		    } else if (rs < 0) {
		        rs = (- errno) ;
	            }
	        } until (rs != SR_INTR) ;
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (u_resolvepath) */


