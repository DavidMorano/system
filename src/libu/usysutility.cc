/* usysutility SUPPORT */
/* lang=C++20 */

/* utility subroutines */
/* version %I% last-modified %G% */


/* revision history:

	= 2023-10-08, David A.D. Morano
	This is a new thing, because, why now?
	
*/

/* Copyright © 2023 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	ctdecf

	Description:
	Convert a |double| to a string.

	Synopsis:
	int ctdecf(char *rbuf,int rlen,cchar *fmt,...) noex

	Arguments:
	rbuf		result buffer pointer
	rlen		result buffer length
	fmt		format string
	...		value to convert

	Returns:
	>=0		OK
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>
#include	<unistd.h>		/* |sleep(3c)| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstdio>		/* |snprintf(3c)| */
#include	<cstring>		/* |strlcpy(3c)| */
#include	<algorithm>		/* |min(3c++)| + |max(3c++)|  */
#include	<string>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<utimeout.h>
#include	<usysrets.h>
#include	<usyscallbase.hh>
#include	<localmisc.h>		/* |DIGBUFLEN| */

#include	"usysutility.hh"


/* local defines */


/* imported namespaces */

using std::nullptr_t ;		/* type */
using std::string ;		/* type */
using libu::usyscallbase ;	/* type */
using std::min ;		/* subroutine-template */
using std::max ;		/* subroutine-template */
using std::to_string ;		/* subroutine */
using std::nothrow ;		/* constant */


/* local typedefs */


/* external subroutines */

extern "C" {
    extern int	msleep(int) noex ;
}

namespace libu {
    extern int	snwcpy(char *,int,cchar *,int) noex ;
}


/* external variables */


/* local structures */

namespace {
    struct syscaller ;
    typedef int (syscaller::*syscaller_m)() noex ;
    struct syscaller : usyscallbase {
	syscaller_m	m = nullptr ;
	double		*dlap ;
	int		dlan ;
	int operator () (double *dp,int n) noex {
	    dlap = dp ;
	    dlan = n ;
	    return handler() ;
	} ;
        int callstd() noex override {
            int         rs = SR_BUGCHECK ;
            if (m) {
                rs = (this->*m)() ;
            }
            return rs ;
        } ;
	int std_getloadavg() noex ;
    } ; /* end struct (syscaller) */
}


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

namespace libu {
    int uloadavgd(double *dla,int n) noex {
	int		rs = SR_FAULT ;
	if (dla) {
	    rs = SR_INVALID ;
	    if (n >= 0) {
		syscaller	sc ;
		sc.m = &syscaller::std_getloadavg ;
		rs = sc(dla,n) ;
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
    } /* end subroutine (uloadavgd) */
}

namespace libu {
    int snuvprintf(char *rbuf,int rlen,cchar *fmt,va_list ap) noex {
	int		rs = SR_FAULT ;
	int		len = 0 ;
	if (rbuf && fmt && ap) {
	    rs = SR_INVALID ;
	    if ((rlen >= 0) && fmt[0]) {
		csize	rsz = size_t(rlen + 1) ;
		int	to_nomem = utimeout[uto_nomem] ;
		bool	fexit = false ;
		repeat {
		    if ((rs = vsnprintf(rbuf,rsz,fmt,ap)) >= 0) {
			if (rs > rlen) {
			    rs = SR_OVERFLOW ;
			} else {
		            len = rs ;
			}
		    } else {
		        rs = SR_BADFMT ;
		    } /* end if */
		    if (rs < 0) {
			switch (rs) {
			case SR_NOMEM:
			    if (to_nomem-- > 0) {
				sleep(1) ;
			    } else {
				fexit = true ;
			    }
			    break ;
			case SR_AGAIN:
			case SR_INTR:
			    break ;
			} /* end switch */
		    } /* end if (error) */
		} until ((rs >= 0) || fexit) ;
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
    } /* end subroutine (snuvprintf) */
    int snuprintf(char *rbuf,int rlen,cchar *fmt,...) noex {
	int		rs = SR_FAULT ;
	if (rbuf && fmt) {
	    rs = SR_INVALID ;
	    if ((rlen >= 0) && fmt[0]) {
		va_list	ap ;
		va_begin(ap,fmt) ;
		rs = snuvprintf(rbuf,rlen,fmt,ap) ;
		va_end(ap) ;
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
    } /* end subroutine (snuprintf) */
    int	snuloadavg(char *rbuf,int rlen,int prec,double *dlap,int dlan) noex {
	int		rs = SR_FAULT ;
	int		len = 0 ;
	if (rbuf && dlap) {
	    rs = SR_INVALID ;
	    rbuf[0] = '\0' ;
	    if ((rlen >= 0) && (prec >= 0) && (dlan >= 0)) {
		try {
		    string	s ;
		    cint	dlen = DIGBUFLEN ;
		    char	dbuf[DIGBUFLEN + 1] ;
		    rs = SR_OK ;
		    for (int i = 0 ; (rs >= SR_OK) && (i < dlan) ; i += 1) {
			if (i > 0) s += ' ' ;
			if ((rs = ctdecf(dbuf,dlen,prec,dlap[i])) >= 0) {
			    s += dbuf ;
			} /* end if (ctdecf) */
		    } /* end for */
		    if (rs >= 0) {
			cint	dl = int(s.length()) ;
			cchar	*dp = s.data() ;
			rs = libu::snwcpy(rbuf,rlen,dp,dl) ;
			len = rs ;
		    } /* end if (ok) */
		} catch (...) {
		    rs = SR_NOMEM ;
		}
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
    }
    int ctdecf(char *rbuf,int rlen,int prec,double v) noex {
	int		rs = SR_FAULT ;
	int		len = 0 ;
	if (rbuf) {
	    rs = SR_INVALID ;
	    if (prec >= 0) {
		try {
		    string	s = "%." + to_string(prec) + "f" ;
		    {
			cchar	*fmt = s.c_str() ;
	        	rs = snuprintf(rbuf,rlen,fmt,prec,v) ;
			len = rs ;
		    }
		} catch (...) {
		    rs = SR_NOMEM ;
		}
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
    } /* end subroutine (ctdecf) */
}


/* local subroutines */

int syscaller::std_getloadavg() noex {
	int		rs ;
	if ((rs = getloadavg(dlap,dlan)) < 0) {
	    rs = (- errno) ;
	}
	return rs ;
}		
/* end method (syscaller::std_getloadavg) */


