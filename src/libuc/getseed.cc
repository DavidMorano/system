/* getseed SUPPORT */
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

#include	<envstandards.h>
#include	<sys/types.h>
#include	<sys/time.h>		/* |gettimeofday(3c)| */
#include	<unistd.h>
#include	<climits>
#include	<cstdlib>
#include	<usystem.h>
#include	<cfdec.h>
#include	<ranlc.h>
#include	<localmisc.h>


/* local defines */

#ifndef	VARRANDOM
#define	VARRANDOM	"RANDOM"
#endif


/* external subroutines */


/* local structures */


/* forward references */


/* local variables */


/* exported subroutines */

int getseed(int seed) noex {
	TIMEVAL		tv ;
	const pid_t	pid = getpid() ;
	const uid_t	uid = getuid() ;
	uint		rv = 0 ;
	int		rs ;
	int		v1, v2 ;
	int		v3 = 0 ;
	cchar		*cp ;

	    gettimeofday(&tv,NULL) ;

	    v1 = getppid() ;

	    v2 = getpgrp() ;

	    if ((cp = getenv(VARRANDOM)) != NULL) {
		cfdeci(cp,-1,&v3) ;
	    }

	rv += randlc(tv.tv_usec) ;
	rv += randlc((int) pid) ;
	rv += randlc(v1) ;
	rv += randlc(v2) ;
	rv += randlc(v3) ;
	rv += randlc(tv.tv_sec) ;
	rv += randlc(uid) ;
	rv += randlc(seed) ;

#if	CF_GETHRTIME
	{
	    hrtime_t	h = gethrtime() ;
	    rv += (uint) h ;
	    h >>= sizeof(uint) ;
	    rv += (uint) h ;
	}
#endif /* CF_GETHRTIME */

	rs = (rv & INT_MAX) ;
	return rs ;
}
/* end subroutine (getseed) */


