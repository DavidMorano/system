/* ucstrtod */
/* lang=C++20 */

/* interface component for UNIX® library-3c */


/* revision history:

	= 1998-04-13, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/uio.h>
#include	<sys/stat.h>
#include	<unistd.h>
#include	<stdlib.h>
#include	<errno.h>
#include	<usystem.h>

#include	"ucstrtod.h"


/* exported subroutines */

int uc_strtod(cchar *startp,char **endpp,double *rp) noex {
	int	rs = SR_FAULT ;
	if (startp && rp) {
	    errno = 0 ;
	    *rp = strtod(startp,endpp) ;
	    rs = (errno != 0) ? (- errno) : 0 ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (uc_strtod) */


