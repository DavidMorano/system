/* ucstrtof SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* interface component for UNIX® library-3c */
/* floating string-to-value functions */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-13, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cerrno>		/* CSTD */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD |strto{x}(3c)| */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

#include	"ucstrtof.h"


/* local defines */


/* local namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

template<typename T> local T strtox(cchar *,char **) noex {
    	return T(0.0) ;
} /* end subroutine-template */

template<> float strtox(cchar *stp,char **epp) noex {
	return strtof(stp,epp) ;
}

template<> double strtox(cchar *stp,char **epp) noex {
	return strtod(stp,epp) ;
}

template<> longdouble strtox(cchar *stp,char **epp) noex {
	return strtold(stp,epp) ;
}

template<typename T> local int strtoxx(cchar *startp,cchar **epp,T *rp) noex {
	int		rs = SR_FAULT ;
	if (startp && rp) ylikely {
	    char	*ep{} ;
	    errno = 0 ;
	    *rp = strtox<T>(startp,&ep) ;
	    if (errno) {
		rs = (neg errno) ;
	    } else {
		rs = intconv(ep - startp) ;
		if (epp) *epp = ep ;
	    }
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (strtoxx) */


/* local variables */


/* exported variables */


/* exported subroutines */

int uc_strtof(cchar *startp,cchar **endpp,float *rp) noex {
    	return strtoxx(startp,endpp,rp) ;
} /* end subroutine (uc_strtof) */

int uc_strtod(cchar *startp,cchar **endpp,double *rp) noex {
    	return strtoxx(startp,endpp,rp) ;
} /* end subroutine (uc_strtod) */

int uc_strtold(cchar *startp,cchar **endpp,longdouble *rp) noex {
    	return strtoxx(startp,endpp,rp) ;
} /* end subroutine (uc_strtold) */


