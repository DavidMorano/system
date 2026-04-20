/* uccreatefile */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* interface component for UNIX® library-3c */
/* version %I% last-modified %G% */

#define	CF_DEBUGS	0		/* compile-time debugging */

/* revision history:

	= 1998-04-13, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>		/* |getenv(3c)| */
#include	<clanguage.h>
#include	<usysbase.h>
#include	<usyscalls.h>
#include	<localmisc.h>


/* external subroutines */

extern "C" {
    extern int uc_create(cchar *,mode_t) noex ;
}


/* exported variables */


/* exported subroutines */

int uc_createfile(cchar *fname,mode_t om) noex {
	int		rs = SR_FAULT ;
	if (fname) {
	    rs = SR_INVALID ;
	    if (fname[0]) {
	        if ((rs = uc_create(fname,om)) >= 0) {
	            rs = u_close(rs) ;
	        }
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (uc_createfile) */


