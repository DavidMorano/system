/* uclibmemx SUPPORT (3uc) */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* interface component for UNIX® library-3c */
/* memory allocation facility (for library use) */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-85, David A­D­ Morano
	This code was originally written.

	= 2018-09-26, David A-D- Morano
	I brought |uc_libcalloc(3uc)| in line w/ the standard for
	|calloc(3c)|.  I never used this myself in 40 years, so I
	never missed it!

*/

/* Copyright © 1998,2018 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Group:
	uc_lbmemsys
	uc_lbmemallocsys

	Description:
	This is the low-level component of the MEMALLOC facility.
	These subroutines need to be able to be interposed upon,
	so they have to be in their own compilation (object) image.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<usyscalls.h>
#include	<ucmem.h>		/* |libuc::mem(3uc)| */
#include	<getbufsize.h>
#include	<localmisc.h>

#include	"uclibmemx.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| + |getlenstr(3u)| */

/* local defines */


/* imported namespaces */

using libuc::mem ;			/* variable */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local vaiables */


/* exported variables */


/* exported subroutines */

int uc_libmemsys(int w,char **rpp) noex {
	int		rs = SR_FAULT ;
	int		rl = 0 ;
	if (rpp) {
	    *rpp = nullptr ;
	    if ((rs = getbufsize(w)) >= 0) {
		rl = rs ;
	        if (char *bp ; (rs = mem.mall((rl + 1),&bp)) >= 0) {
	            *rpp = bp ;
	        } /* end if */
	    } /* end if (getbufsize) */
	} /* end if (non-null) */
	return (rs >= 0) ? rl : rs ;
}
/* end subroutine (uc_libmemsys) */


