/* mkexpandpath SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* make an expanded path */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-07-10, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/******************************************************************************

	Name:
	mkexpandpath

	Description:
	This subroutine creates a resolved filename path from the
	coded form.

	Synopsis:
	int mkexpandpath(char *rbuf,cchar *pp,int pl) noex

	Arguments:
	rbuf		result buffer (should be MAXPATHLEN+1 long)
	pp		source path pointer
	pl		source path length

	Returns:
	>0		expansion
	==0		no expansion
	<0		error (system-return)

******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<usystem.h>
#include	<localmisc.h>

#include	"mkx.h"

import libutil ;

/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */

extern "C" {
    extern int	mkvarpath(char *,cchar *,int) noex ;
    extern int	mkuserpath(char *,cchar *,cchar *,int) noex ;
    extern int	mkcdpath(char *,cchar *,int) noex ;
}


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int mkexpandpath(char *rbuf,cchar *pp,int pl) noex {
	int		rs = SR_FAULT ;
	int		rl = 0 ;
	if (rbuf && pp) ylikely {
	    if (pl < 0) pl = lenstr(pp) ;
	    rbuf[0] = '\0' ;
	    if ((rs = mkuserpath(rbuf,nullptr,pp,pl)) == 0) ylikely {
	        if ((rs = mkvarpath(rbuf,pp,pl)) == 0) ylikely {
	            rs = mkcdpath(rbuf,pp,pl) ;
		    rl = rs ;
	        }
	    }
	} /* end if (non-null) */
	return (rs >= 0) ? rl : rs ;
}
/* end subroutine (mkexpandpath) */


