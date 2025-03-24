/* opendefstds SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* open default standard files */
/* version %I% last-modified %G% */


/* revision history:

	= 2008-09-01, David A­D­ Morano
	This subroutine was borrowed and modified from previous
	generic front-end 'main' subroutines!

*/

/* Copyright © 2008 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	opendefstds

	Description:
	This subroutine opens the default standard files up to a
	certain file-descriptor number (generally should be '3').

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<unistd.h>
#include	<fcntl.h>
#include	<climits>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<syswords.hh>
#include	<localmisc.h>

#include	"open.h"


/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int opendefstds(int n) noex {
	int		rs = SR_OK ;
	cmode		om = 0666 ;
	cchar		*nfn = sysword.w_devnull ;
	for (int i = 0 ; (rs >= 0) && (i < n) ; i += 1) {
	    if ((rs = u_fcntl(i,F_GETFD,0)) == SR_BADF) {
	        cint	of = (i == 0) ? O_RDONLY : O_WRONLY ;
	        rs = u_open(nfn,of,om) ;
	    }
	} /* end if */
	return rs ;
}
/* end subroutine (opendefstds) */


