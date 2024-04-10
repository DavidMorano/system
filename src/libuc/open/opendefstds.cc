/* opendefstds SUPPORT */
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

	This subroutine opens the default standard files up to a certain
	file-descriptor number (generally should be '3').


*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/param.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<climits>
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<localmisc.h>


/* local defines */

#ifndef	NULLFNAME
#define	NULLFNAME	"/dev/null"
#endif


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
	cchar		nfn = NULLFNAME ;
	for (int i = 0 ; (rs >= 0) && (i < n) ; i += 1) {
	    if ((rs = u_fcntl(i,F_GETFD,0)) == SR_BADF) {
	        cint	of = (i == 0) ? O_RDONLY : O_WRONLY ;
	        rs = u_open(nfn,of,om) ;
	    }
	} /* end if */
	return rs ;
}
/* end subroutine (opendefstds) */


