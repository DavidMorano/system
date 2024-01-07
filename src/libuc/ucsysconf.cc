/* ucsysconf */
/* lang=C++20 */

/* interface component for UNIX® library-3c */
/* get system configuration information */


/* revision history:

	= 1998-04-13, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	uc_confsys

	Description:
	This subroutine returns system-related values (as opposed
	to strings as |uc_confstr(3uc)| does).

	Synopsis:
	int uc_confsys(int req,long *rp) noex

	Arguments:
	req		requested value to return
	rp		pointer to value-type |long| for received value


	Name:
	uc_confstr

	Description:
	This subroutine is NOT the same as the UNIX®-System version.
	This subroutine returns "OVERFLOW" when the user supplied
	buffer is not big enough to hold the associated value.

	Synopsis:
	int uc_confstr(char *rbuf,int rlen,int req) noex

	Arguments:
	rbuf		user supplied buffer to hold result
	rlen		length of user supplied value
	req		configuration value to request

	Returns:
	>0		valid and the value is returned with this length
	0		valid but there was no value associated
	<0		the requested configuration value was invalid

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<unistd.h>
#include	<cerrno>
#include	<climits>
#include	<usystem.h>
#include	<usupport.h>
#include	<localmisc.h>


/* local defines */


/* local namespaces */


/* local typedefs */


/* external subroutines */

extern "C" {
    extern int uc_sysconf(int,long *) noex ;
}


/* local structures */

namespace {
    struct ucsysconf ;
    typedef int (ucsysconf::*mem_f)(int) ;
    struct ucsysconf {
	mem_f		m ;
	char		*rbuf ;
	long		*lp ;
	int		rlen ;
	ucsysconf(char *rp,int rl) noex : rbuf(rp), rlen(rl) { } ;
	ucsysconf(long *p) noex : lp(p) { } ;
	int mconfsys(int) noex ;
	int mconfstr(int) noex ;
	int operator () (int) noex ;
    } ; /* end struct (ucsysconf) */
}


/* forward references */


/* exported subroutines */

int uc_confsys(int req,long *rp) noex {
	ucsysconf	sco(rp) ;
	sco.m = &ucsysconf::mconfsys ;
	return sco(req) ;
}
/* end subroutine (uc_confsys) */

int uc_confstr(char *rbuf,int rlen,int req) noex {
	ucsysconf	sco(rbuf,rlen) ;
	sco.m = &ucsysconf::mconfstr ;
	return sco(req) ;
}
/* end subroutine (uc_confstr) */

int uc_sysconf(int req,long *rp) noex {
	return uc_confsys(req,rp) ;
}


/* local subroutines */

int ucsysconf::operator () (int req) noex {
	int		to_nomem = utimeout[uto_nomem] ;
	int		rs ;
	bool		f_exit = false ;
	repeat {
	    if ((rs = (this->*m)(req)) < 0) {
	        switch (rs) {
	        case SR_NOMEM:
		    if (to_nomem-- > 0) {
	                msleep(1000) ;
	 	    } else {
		        f_exit = true ;
		    }
		    break ;
	        case SR_AGAIN:
	        case SR_INTR:
	            break ;
		default:
		    f_exit = true ;
		    break ;
	        } /* end switch */
	    } /* end if (error) */
	} until ((rs >= 0) || f_exit) ;
	return rs ;
}
/* end subroutine (ucsysconf::operator) */

int ucsysconf::mconfsys(int req) noex {
	long		result = 0 ;
	int		rs = SR_OK ;
	errno = 0 ;
	if ((result = sysconf(req)) < 0) {
	    rs = (errno) ? (- errno) : SR_NOTSUP ;
	} else {
	    if (lp) *lp = result ;
	    rs = int(result & INT_MAX) ;
	}
	return rs ;
}
/* end subroutine (ucsysconf::mconfsys) */

int ucsysconf::mconfstr(int req) noex {
	int		rs = SR_OVERFLOW ;
	int		len = 0 ;
	if (rbuf == nullptr) rlen = 0 ; /* indicate return length only */
	if ((rbuf == nullptr) || (rlen >= 1)) {
	    const size_t	res_bad = size_t(-1) ;
	    size_t		result ;
	    size_t		llen = (rlen+1) ;
	    rs = SR_OK ;
	    errno = 0 ;
	    if ((result = confstr(req,rbuf,llen)) == res_bad) {
	        rs = (errno) ? (- errno) : SR_NOTSUP ;
	    } else {
	        if (result <= llen) {
	            len = int(result-1) ;
	        } else {
		    rs = SR_OVERFLOW ;
		}
	    } /* end if */
	} /* end if (try) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (ucsysconf::mconfstr) */


