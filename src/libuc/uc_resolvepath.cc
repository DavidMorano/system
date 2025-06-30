/* uc_resolvepath SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* interface component for UNIX�® library-3c */
/* resolve a path without symbolic components */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-13, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	uc_resolvepath

	Description:
	This subroutine takes an existing path and creates a new
	path that does not contain any symbolic components.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<unistd.h>
#include	<cerrno>
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

int uc_resolvepath(cchar *input,char *rbuf,int rlen) noex {
    	int		rs = SR_FAULT ;
	if (input && rbuf) {
	    rs = SR_INVALID ;
	    rbuf[0] = '\0' ;
	    if (input[0] && (rlen > 0)) {
		rs = u_resolvepath(input,rbuf,rlen) ;
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (uc_resolvepath) */


