/* ut_free */

/* UNIX� XTI subroutine */
/* XTI free */


#define	CF_DEBUGS	0		/* compile-time debugging */


/* revision history:

	= 1998-11-01, David A�D� Morano

	This subroutine was written for Rightcore Network Services.


*/

/* Copyright � 1998 David A�D� Morano.  All rights reserved. */


#define	LIBUT_MASTER	0


#include	<envstandards.h>	/* MUST be first to configure */

#include	<sys/types.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<poll.h>
#include	<xti.h>
#include	<errno.h>

#include	<usystem.h>


/* external subroutines */

extern int	msleep(int) ;


/* exported subroutines */


int ut_free(void *p,int stype)
{
	int		rs = SR_OK ;

	if (p == NULL) return SR_FAULT ;

again:
	rs = t_free(p,stype) ;

	if (rs < 0) {
	    switch (t_errno) {
	    case TSYSERR:
	        rs = (- errno) ;
		break ;
	    default:
		rs = SR_NOANODE ;
	    } /* end switch */
	} /* end if */

	if (rs < 0) {
	    switch (rs) {
	    case SR_NOMEM:
	    case SR_NOSR:
	        msleep(1000) ;
/* FALLTHROUGH */
	    case SR_INTR:
	    case SR_AGAIN:
	        goto again ;
	    } /* end switch */
	} /* end if */

	return rs ;
}
/* end subroutine (ut_free) */


