/* opendefstds SUPPORT */
/* charset=ISO8859-1 */
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

	Synopsis:
	int opendefstds(int n) noex

	Arguments:
	n		number of low file-descriptos to check

	Returns:
	>=0		OK, number of newly opened file-descriptors
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<unistd.h>
#include	<fcntl.h>
#include	<climits>		/* CSTD */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstring>		/* CSTD */
#include	<clanguage.h>		/* CSTD */
#include	<usysbase.h>		/* CSTD */
#include	<usyscalls.h>		/* CSTD */
#include	<ucopen.h>		/* LIBUC */
#include	<ucdesc.h>		/* LIBUC */
#include	<localmisc.h>		/* CSTD */

#include	"open.h"

#pragma		GCC dependency		"mod/uconstants.ccm"

import uconstants ;			/* |sysword(3u)| */

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
	int		c = 0 ;
	cmode		om = 0666 ;
	cchar		*nfn = sysword.w_devnull ;
	for (int i = 0 ; (rs >= 0) && (i < n) ; i += 1) {
	    if ((rs = u_fcntl(i,F_GETFD,0)) == SR_BADF) {
	        cint	of = (i == 0) ? O_RDONLY : O_WRONLY ;
	        rs = u_open(nfn,of,om) ;
		c += 1 ;
	    }
	} /* end if */
	return (rs >= 0) ? c : rs ;
} /* end subroutine (opendefstds) */


