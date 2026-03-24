/* uc_truncate SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* interface component for UNIX® library-3c */
/* version %I% last-modified %G% */

#define	CF_DEBUGS	0

/* revision history:

	= 1998-04-13, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	uc_truncate

	Description:
	Truncate a file (as specified by a file-nam).

	Notes: 
	1.  No one says that |truncate(2)| can return EAGAIN, but
	we think that several OSes can indeed return this; so we
	handle it.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/uio.h>
#include	<sys/stat.h>
#include	<unistd.h>
#include	<cerrno>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>		/* |getenv(3c)| */
#include	<clanguage.h>
#include	<usysbase.h>
#include	<usyscalls.h>
#include	<utimeout.h>
#include	<localmisc.h>

#include	"uctruncate.h"


/* local defines */


/* external subroutines */


/* external variables */


/* forward references */


/* exported variables */


/* exported subroutines */

int uc_truncate(cchar *fn,off_t len) noex {
	int		rs  = SR_FAULT ;
	if (fn) {
	    rs = SR_INVALID ;
	    if (fn[0]) {
	        int	to_again = utimeout[uto_again] ;
	        bool	fexit = false ;
	        repeat {
	            if ((rs = truncate(fn,len)) < 0) {
		        rs = (- errno) ;
	                switch (rs) {
	                case SR_AGAIN:
	                    if (to_again-- > 0) {
			        msleep(1000) ;
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
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (uc_truncate) */


