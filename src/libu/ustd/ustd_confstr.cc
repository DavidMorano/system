/* ustd_confstr SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* interface component for UNIX® library-3c */
/* get system configuration information */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-13, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	std_constr

	Description:
	This subroutine is NOT the same as the UNIX®-System version.
	This subroutine returns "OVERFLOW" when the user supplied
	buffer is not big enough to hold the associated value.

	Synopsis:
	int ustd_confstr(char *rbuf,int rlen,int req) noex

	Arguments:
	rbuf		user supplied buffer to hold result
	rlen		length of user supplied value
	req		configuration value to request

	Returns:
	>0		valid and the value is returned with this length
	0		valid but there was no value associated
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<unistd.h>		/* |confstr(2)| */
#include	<cerrno>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<usystem.h>
#include	<errtimer.hh>
#include	<sysconfcmds.h>
#include	<localmisc.h>

#include	"ustd.h"


/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */

constexpr size_t	minusone = -1uz ;


/* exported variables */


/* exported subroutines */


/* local subroutines */

namespace ustd {
    int ustd_confstr(int req,char *rbuf,int rlen) noex {
    	size_t		res ; /* used-multiple */
	int		rs = SR_OK ;
	int		len = 0 ;
	if (rlen > 0) {
	    rs = SR_FAULT ;
	    errno = 0 ;
	    if (rbuf) {
	        csize	rsize = size_t(rlen + 1) ;
		rs = SR_OK ;
		if ((res = confstr(req,rbuf,rsize)) == 0uz) {
	            if (res <= rsize) {
	                len = intsat(res - 1) ;
	            } else {
		        rs = SR_OVERFLOW ;
		    }
	        } else if (res == 0uz) {
	            rs = (errno) ? (- errno) : SR_NOTSUP ;
		} else if (res == minusone) {
	            rs = (errno) ? (- errno) : SR_NOTSUP ;
		} else {
		    rs = SR_NOSYS ;	/* not defined in documentation */
	        } /* end if */
	    } /* end if (non-null) */
	} else if (rlen == 0) {
	    cnullptr	np{} ;
	    if ((res = confstr(req,np,0uz)) > 0uz) {
	        len = intsat(res - 1) ;
	    } else if (res == 0uz) {
	        rs = (errno) ? (- errno) : SR_NOTSUP ;
	    } else if (res == minusone) {
	        rs = (errno) ? (- errno) : SR_NOTSUP ;
	    } else {
		rs = SR_NOSYS ;
	    }
	} else {
	    rs = SR_INVALID ;
	} /* end if */
	return (rs >= 0) ? len : rs ;
    } /* end subroutine (ustd_confstr) */
} /* end namespace (ustd) */


