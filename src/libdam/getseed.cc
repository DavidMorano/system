/* getseed SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* get random data from the UNIX® kernel */
/* version %I% last-modified %G% */

#define	CF_GETHRTIME	1		/* use |gethrtime(3c)| */

/* revision history:

	= 2001-04-11, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 2001 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	getseed
	
	Synopsis:
	int getseed(int seed) noex

	Arguments:
	seed		given value to add to the mix

	Returns:
	>0		returned number of bytes
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<sys/types.h>
#include	<sys/time.h>		/* |gettimeofday(3c)| */
#include	<unistd.h>
#include	<climits>		/* |CHAR_BIT| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>		/* |getenv(3c)| */
#include	<usystem.h>
#include	<cfdec.h>
#include	<randlc.h>
#include	<localmisc.h>

#include	"getseed.h"

import uconstants ;

/* local defines */

#ifndef	CF_GETHRTIME
#define	CF_GETHRTIME	0
#endif


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */

constexpr bool		f_gethrtime = CF_GETHRTIME ;


/* exported variables */


/* exported subroutines */

int getseed(int seed) noex {
	cchar		*vn = varname.random ;
	TIMEVAL		tv ;
	int		rs ;
	if ((rs = uc_gettimeofday(&tv,nullptr)) >= 0) {
	    static cchar	*val = getenv(vn) ;
	    uint	rv = 0 ;
	    cint	uid = getuid() ;
	    cint	pid = getpid() ;
	    cint	v1 = getppid() ;
	    cint	v2 = getpgrp() ;
	    if (val) {
		if (int v ; cfdeci(val,-1,&v) >= 0) {
	    	    rv += randlc(v) ;
		}
	    }
	    rv += randlc(tv.tv_usec) ;
	    rv += randlc(uid) ;
	    rv += randlc(pid) ;
	    rv += randlc(v1) ;
	    rv += randlc(v2) ;
	    rv += randlc(tv.tv_sec) ;
	    rv += randlc(seed) ;
	    if_constexpr (f_gethrtime) {
	        hrtime_t	h = gethrtime() ;
	        rv += uint(h) ;
	        h >>= (szof(uint) * CHAR_BIT) ;
	        rv += uint(h) ;
	    } /* end if_constexpr (f_gethrtime) */
	    rs = int(rv & INT_MAX) ;
	} /* end if (uc_gettimeofday) */
	return rs ;
}
/* end subroutine (getseed) */


