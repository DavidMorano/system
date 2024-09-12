/* uatfork SUPPORT */
/* lang=C++20 */

/* interface component for UNIX® library-3c */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services (RNS).

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<sys/types.h>
#include	<unistd.h>
#include	<climits>
#include	<cerrno>
#include	<cstddef>		/* |nullptr_t| */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysrets.h>
#include	<usyscalls.h>


/* local defines */


/* local variables */


/* exported variables */


/* exported subroutines */

int u_atfork(void_f b,void_f ap,void_f ac) noex {
	int		rs ;
	repeat {
	    rs = SR_OK ;
	    if (errno_t ec ; (ec = pthread_atfork(b,ap,ac)) > 0) {
		 rs = (- ec) ;
	    }
	} until ((rs != SR_INTR) && (rs != SR_AGAIN)) ;
	return rs ;
}
/* end subroutine (u_atfork) */


