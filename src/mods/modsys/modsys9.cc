/* modsys9 MODULE (implementation) */
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
	modsys9

	Description:
	Retrieve values from the system.

	Synopsis:
    	int ustd_confval(int req,long *lp = nullptr) noex
	int ustd_confstr(int req,char *rbuf,int rlen) noex

	Arguments:
	req		configuration value to request
	lp		optional pointer to longword to receive result
	rbuf		result buffer pointer
	rlen		result buffer length

	Returns:
	>0		valid and the value is returned with this length
	0		valid but there was no value associated
	<0		error (system-return)

*******************************************************************************/

module ;

#include	<envstandards.h>	/* MUST be first to configure */
#include	<unistd.h>		/* POSIX |confstr(2)| */
#include	<cerrno>		/* CSTD */
#include	<climits>		/* CSTD |INT_MAX| */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */
#include	<usysrets.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */
#include	<sysconfcmds.h>		/* LIBU commands for |sysconf(3c)| */

#include	"modsys.hh"

module modsys ;

/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

local bool isnolimit(int) noex ;


/* local variables */

constexpr int		nolimits[] = {
	_SC_AIO_MAX,
	_SC_ATEXIT_MAX,
	_SC_MQ_OPEN_MAX,
	_SC_THREAD_THREADS_MAX,
	_SC_THREAD_KEYS_MAX,
	_SC_THREAD_DESTRUCTOR_ITERATIONS,
	_SC_TZNAME_MAX,
	-1
} ; /* end array (nolimits) */

constexpr size_t	minusone = -1uz ;


/* exported variables */


/* exported subroutines */

int ustd_confval(int req,long *lp) noex {
	int		rs ;
	errno = 0 ;
	if (long res ; (res = sysconf(req)) >= 0L) {
	    if (lp) *lp = res ;
	    rs = intsat(res) ;
	} else {
	    if (errno) {
	        rs = (neg errno) ;
		if (lp) *lp = 0 ;
	    } else {
		if (isnolimit(req)) {
		    rs = INT_MAX ;
		    if (lp) *lp = INT_MAX ;
		} else {
		    rs = SR_NOTSUP ;
		    if (lp) *lp = 0 ;
		}
	    } /* end if */
	} /* end if */
	return rs ;
} /* end subroutine (ustd_confval) */

int ustd_confstr(int req,char *rbuf,int rlen) noex {
	cnullptr	np{} ;
    	size_t		res ; /* used-multiple */
	int		rs = SR_OK ;
	int		len = 0 ;
	if (rlen > 0) {
	    rs = SR_FAULT ;
	    errno = 0 ;
	    if (rbuf) {
	        csize	rsize = size_t(rlen + 1) ;
		rs = SR_OK ;
		if ((res = confstr(req,rbuf,rsize)) > 0uz) {
	            if (res <= rsize) {
	                len = intsat(res - 1) ;
	            } else {
		        rs = SR_OVERFLOW ;
		    }
	        } else if (res == 0uz) {
	            rs = (errno) ? (neg errno) : SR_NOTSUP ;
		} else if (res == minusone) {
	            rs = (errno) ? (neg errno) : SR_NOTSUP ;
		} else {
		    rs = SR_NOSYS ;	/* not defined in documentation */
	        } /* end if */
	    } /* end if (non-null) */
	} else if (rlen == 0) {
	    if ((res = confstr(req,np,0uz)) > 0uz) {
	        len = intsat(res - 1) ;
	    } else if (res == 0uz) {
	        rs = (errno) ? (neg errno) : SR_NOTSUP ;
	    } else if (res == minusone) {
	        rs = (errno) ? (neg errno) : SR_NOTSUP ;
	    } else {
		rs = SR_NOSYS ;
	    }
	} else {
	    rs = SR_INVALID ;
	} /* end if */
	return (rs >= 0) ? len : rs ;
} /* end subroutine (ustd_confstr) */


/* local subroutines */

local bool isnolimit(int name) noex {
    	bool f = false ;
	for (int i = 0 ; (! f) && (nolimits[i] >= 0) ; i += 1) {
	    f = (name == nolimits[i]) ;
	} /* end for */
	return f ;
} /* end subroutine (isnolimit) */


