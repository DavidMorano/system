/* ucgetrusage SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* interface components for UNIX® library-3c */
/* retrieve process usage information */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-13, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	uc_getrusage

	Description:
	This subroutine retrieves process usage data from the kernel.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/resource.h>	/* POSIX® */
#include	<cerrno>		/* CSTD */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include        <errtimer.hh>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

#include	"ucgetx.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

local int	std_getrusage(int,RUSAGE *) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int uc_getrusage(int who,RUSAGE *rp) noex {
	errtimer        to_mfile	= utimeout[uto_mfile] ;
	errtimer        to_nfile	= utimeout[uto_nfile] ;
	errtimer        to_nomem	= utimeout[uto_nomem] ;
	errtimer        to_nospc	= utimeout[uto_nospc] ;
	errtimer        to_again	= utimeout[uto_again] ;
	errtimer        to_busy		= utimeout[uto_busy] ;
	reterr          r ;
	int		rs ;
	repeat {
	    if ((rs = std_getrusage(who,rp)) < 0) {
                r(rs) ;                 /* <- default causes exit */
                switch (rs) {
                case SR_MFILE:
                    r = to_mfile(rs) ;
                    break ;
                case SR_NFILE:
                    r = to_nfile(rs) ;
                    break ;
                case SR_NOMEM:
                    r = to_nomem(rs) ;
                    break ;
                case SR_NOSPC:
                    r = to_nospc(rs) ;
                    break ;
                case SR_AGAIN:
                    r = to_again(rs) ;
                    break ;
                case SR_BUSY:
                    r = to_busy(rs) ;
                    break ;
                case SR_INTR:
                    r(false) ;
                    break ;
                } /* end switch */
                rs = r ;
	    } /* end if (error) */
	} until ((rs >= 0) || r.fexit) ;
	return rs ;
} /* end subroutine (uc_getrusage) */


/* local subroutines */

local int std_getrusage(int who,RUSAGE *rp) noex {
    	int		rs ;
	if ((rs = getrusage(who,rp)) < 0) {
	    rs = (neg errno) ;
	}
	return rs ;
} /* end subroutine (std_getrusage) */


