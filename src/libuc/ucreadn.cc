/* ucreadn SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* interface component for UNIX® library-3c */
/* read a fixed number of bytes */

#define	CF_DEBUGS	0		/* compile-time debugging */

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
#include	<poll.h>
#include	<cerrno>
#include	<usystem.h>
#include	<localmisc.h>


/* local defines */

#define	MAXEOF		4		/* maximum consecutive EOFs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int uc_readn(int fd,void *abuf,int alen) noex {
	int	rs = SR_FAULT ;
	int	rlen = 0 ; /* return-value */
	if (abuf) ylikely {
	    rs = SR_BADF ;
	    if (fd >= 0) ylikely {
	        rs = SR_INVALID ;
	        if (alen >= 0) ylikely {
	            int		c = 0 ;
	            int		len ;
	            int		alenr = alen ;
	            char	*abp = (char *) abuf ;
	            while ((rs >= 0) && (alenr > 0)) {
	                rs = u_read(fd,abp,alenr) ;
	                len = rs ;
	                if (rs > 0) {
		            abp += len ;
		            rlen += len ;
		            alenr -= len ;
	                } else if (rs == 0) {
		            c += 1 ;
		            if (c >= MAXEOF) break ;
	                }
	            } /* end while */
	        } /* end if (valid) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? rlen : rs ;
}
/* end subroutine (uc_readn) */


