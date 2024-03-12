/* ugetloadavg SUPPORT */
/* lang=C++20 */

/* get the standard load averages maintained by the kernel */
/* version %I% last-modified %G% */


/* revision history:

	= 2001-04-23, David A­D­ Morano
	Well Solaris-8 supplied this new capability. I have no idea
	about any status as a standard of any sort but this is a
	lot easier than using 'kstat(3kstat)'! I continued to use
	(and still do) 'kstat(3kstat)' on Solaris-8 since it first
	came out. Originally I resisted investigating how to get
	the direct load averages out of the kernel besides using
	'kstat(3kstat)'. A little investigation (easy) showed that
	they had implemented a new system call to do it. I guess
	that they were also getting tired of using 'kstat(3kstat)'
	for kernel load averages!

*/

/* Copyright © 2001 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Yes, this grabs (rather directly and easily), the load
	averages maintained by the kernel. Solaris-8 (I think) was
	the first version to allow these kernel load averages to
	be grabbed without using |kstat(3kstat)| (but I am not
	sure). Anyway, this little subroutine uses the UNDOCUMENTED
	new system call in Solaris-8 to grab these load averages!

	Let me repeat myself for those in the audience who didn't
	quite hear me correctly the first time. This subroutine
	uses the secret and UNDOCUMENTED new system call to grab
	the load averages from the kernel.  This could break at any
	time! If you do not believe me, go back to using the draft
	version of some of the stupid POSIX reentrant calls and see
	how long they will be continued to be supported!

	Synopsis:
	int u_getloadavg(uint la,int n) noex ;

	Arguments:
	la	array of unsigned integers of at least 'n' elements
	n	number of elements in the array 'la'

	Returns:
	>=0	OK
	<0	error (system-return)

	CAUTION:
	This subroutine uses a secret and undocumented new system
	call introduced maybe with Solaris-8. This function could
	go away at any time so don't get to comfortable with using
	it! Don't give up your day job but rather instead keep your
	old 'kstat(3kstat)' stuff around for getting load averages
	when this new thing suddenly "goes away"!

	Extra information:
	If you can stand to get the load averages converted to the
	type double (an array of doubles rather than an array of
	ints), then check out |getloadavg(3c)|.


*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>
#include	<cerrno>
#include	<cstdlib>
#include	<usystem.h>
#include	<usysflag.h>
#include	<localmisc.h>

#include	"ugetloadavg.h"


/* local defines */

#ifndef	LOADAVG_NSTATS
#define LOADAVG_NSTATS	3	/* maximum load-averages avilable */
#endif


/* local namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

static int	ugetloadavg(uint *,int) noex ;


/* local variables */

constexpr int		maxloadavgs = LOADAVG_NSTATS ;

constexpr bool		f_sunos = F_SUNOS ;


/* exported variables */


/* exported subroutines */

int u_getloadavg(uint *la,int n) noex {
	int		rs = SR_FAULT ;
	if (la) {
	    rs = SR_INVALID ;
	    if (n > 0) {
	        if (n > maxloadavgs) n = maxloadavgs ;
		if constexpr (f_sunos) {
		    int		*ila = (int *) la ;
	            if ((rs = __getloadavg(ila,n)) < 0) {
	                rs = (- errno) ;
	            }
		} else {
		    rs = ugetloadavg(la,n) ;
		}
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (u_getloadavg) */

namespace libuc {
    int ucgetloadavg(double *dla,int n) noex {
	int		to_again = utimeout[uto_again] ;
	int		rs ;
	bool		f_exit = false ;
	repeat {
	    if ((rs = getloadavg(dla,n)) < 0) rs = (- errno) ;
	    if (rs < 0) {
	        switch (rs) {
	        case SR_AGAIN:
	            if (to_again-- > 0) {
			msleep(100) ;
		    } else {
			f_exit = true ;
		    }
	            break ;
	        case SR_INTR:
	            break ;
		default:
		    f_exit = true ;
		    break ;
	        } /* end switch */
	    } /* end if (error) */
	} until ((rs >= 0) || f_exit) ;
	return rs ;
    } /* end subroutine (ucgetloadavg) */
}


/* local subroutines */

static int ugetloadavg(uint *la,int n) noex {
	cint		nmax = maxloadavgs ;
	int		rs = SR_FAULT ;
	int		rn = 0 ;
	if (la) {
	    rs = SR_INVALID ;
	    if (n > 0) {
	        double	d[nmax] ;
	        if (n > nmax) n = nmax ;
	        if ((rs = libuc::ucgetloadavg(d,n)) >= 0) {
		    rn = rs ;
	            for (int i = 0 ; i < n ; i += 1) {
	                la[i] = uint(d[i] * FSCALE) ;
		    }
		}
	    } /* end if (greater-than) */
	} /* end if (non-null) */
	return (rs >= 0) ? rn : rs ;
}
/* end subroutine (ugetloadavg) */


