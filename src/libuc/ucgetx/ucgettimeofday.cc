/* ucgettimeofday SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* interface component for UNIX® library-3c */
/* miscellaneous system information retrieval functions */
/* version %I% last-modified %G% */


/* revision history:

	= 2000-05-14, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2000 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	uc_gettimeofday

	Description:
	This routine calls the system's (library) 'gettimeofday'
	subroutine.

	Synopsis:
	int uc_gettimeofday(struct timeval *tvp,void *np) noex
	
	Arguments:
	tvp		pointer to TIMEVAL object to hold result
	np		NULL pointer (currently required)

	Returns:
	>=0		success
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* POSIX */
#include	<sys/param.h>		/* POSIX */
#include	<sys/timeb.h>		/* POSIX */
#include	<unistd.h>		/* POSIX */
#include	<fcntl.h>		/* POSIX */
#include	<climits>		/* CSTD */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<ucsysconf.h>		/* LIBUC */
#include	<tmtime.hh>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */

#include	"ucgetx.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |memclear(3u)| */

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

int uc_gettimeofday(TIMEVAL *tvp,void *dp) noex {
	int		rs = SR_FAULT ;
	if (tvp) {
	    rs = SR_OK ;
	    if (gettimeofday(tvp,dp) == -1) {
		rs = (neg errno) ;
	    }
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (uc_gettimeofday) */


/* local subrouties */


