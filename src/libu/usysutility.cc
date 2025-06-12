/* usysutility SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* utility subroutines */
/* version %I% last-modified %G% */


/* revision history:

	= 2023-10-08, David A-D- Morano
	This is written as some interim hack for some code that
	cannot otherwise be linked with the main LIBUC code library.

*/

/* Copyright © 2023 David A-D- Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	libu::ctdecf

	Description:
	Convert a |double| to a decimal digit-string.

	Synopsis:
	int libu::ctdecf(char *rbuf,int rlen,int prec,double v) noex

	Arguments:
	rbuf		result buffer pointer
	rlen		result buffer length
	prec		precision (places after decimal point)
	v		value to convert

	Returns:
	>=0		OK
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>
#include	<unistd.h>		/* |sleep(3c)| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstdio>		/* |vsnprintf(3c)| */
#include	<cstring>		/* |strlcpy(3c)| */
#include	<new>			/* |nothrow(3c++)| */
#include	<algorithm>		/* |min(3c++)| + |max(3c++)|  */
#include	<string>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<usyscallbase.hh>	/* |usyscallbase(3u)| */
#include	<utimeout.h>
#include	<errtimer.hh>
#include	<localmisc.h>		/* |DIGBUFLEN| */

#include	"usysutility.hh"


/* local defines */


/* imported namespaces */

using std::nullptr_t ;			/* type */
using std::string ;			/* type */
using libu::usyscallbase ;		/* type */
using std::min ;			/* subroutine-template */
using std::max ;			/* subroutine-template */
using std::to_string ;			/* subroutine-template */
using std::nothrow ;			/* constant */


/* local typedefs */


/* external subroutines */

extern "C" {
    extern int	msleep(int) noex ;
}

namespace libu {
    extern int	snwcpy(char *,int,cchar *,int = -1) noex ;
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
} /* end namespace */


/* forward references */

static int std_vsnprintf(char *,int,cc *,va_list) noex ;


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
    int ctdecf(char *rbuf,int rlen,int prec,double v) noex {
	int		rs = SR_FAULT ;
	int		len = 0 ;
	if (rbuf) {
	    rs = SR_INVALID ;
	    if (prec >= 0) {
		rs = snprintf(rbuf,rlen,"%.*f",prec,v) ;
		len = rs ;
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
    } /* end subroutine (ctdecf) */
    int snvprintf(char *rbuf,int rlen,cchar *fmt,va_list ap) noex {
	int		rs = SR_FAULT ;
	if (rbuf && fmt && ap) {
	    rs = SR_INVALID ;
	    if ((rlen >= 0) && fmt[0]) {
		errtimer	to_again	= utimeout[uto_again] ;
		errtimer	to_nomem	= utimeout[uto_nomem] ;
		errtimer	to_nosr		= utimeout[uto_nosr] ;
		errtimer	to_nobufs	= utimeout[uto_nobufs] ;
		errtimer	to_nospc	= utimeout[uto_nospc] ;
		reterr		r ;
		repeat {
		    if ((rs = std_vsnprintf(rbuf,rlen,fmt,ap)) < 0) {
			r(rs) ;
			switch (rs) {
			case SR_AGAIN:
			    r = to_again(rs) ;
			    break ;
			case SR_NOMEM:
			    r = to_nomem(rs) ;
			    break ;
			case SR_NOSR:
			    r = to_nosr(rs) ;
			    break ;
			case SR_NOBUFS:
			    r = to_nobufs(rs) ;
			    break ;
			case SR_NOSPC:
			    r = to_nospc(rs) ;
			    break ;
			case SR_INTR:
			    r(false) ;
			    break ;
			} /* end switch */
			rs = r ;
		    } /* end if (error) */
		} until ((rs >= 0) || r.fexit) ;
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
    } /* end subroutine (snvprintf) */
    int snprintf(char *rbuf,int rlen,cchar *fmt,...) noex {
	va_list		ap ;
	int		rs = SR_FAULT ;
	if (rbuf && fmt) {
	    rs = SR_INVALID ;
	    if ((rlen >= 0) && fmt[0]) {
		va_begin(ap,fmt) ;
		rs = snvprintf(rbuf,rlen,fmt,ap) ;
		va_end(ap) ;
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
    } /* end subroutine (snprintf) */
    int	snloadavgd(char *rbuf,int rlen,int prec,double *dlap,int dlan) noex {
	int		rs = SR_FAULT ;
	int		len = 0 ;
	if (rbuf && dlap) {
	    rs = SR_INVALID ;
	    rbuf[0] = '\0' ;
	    if ((rlen >= 0) && (prec >= 0) && (dlan >= 0)) {
		try {
		    string	s ;
		    cint	dlen = DECBUFLEN ;
		    char	dbuf[DECBUFLEN + 1] ;
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
    } /* end subroutine (snloadavgd) */
} /* end namespace (libu) */


/* local subroutines */

int syscaller::std_getloadavg() noex {
	int		rs ;
	if ((rs = getloadavg(dlap,dlan)) < 0) {
	    rs = (- errno) ;
	}
	return rs ;
}		
/* end method (syscaller::std_getloadavg) */

static int std_vsnprintf(char *rbuf,int rlen,cc *fmt,va_list ap) noex {
	csize		rsize = size_t(rlen + 1) ;
	int		rs ;
	if ((rs = vsnprintf(rbuf,rsize,fmt,ap)) >= 0) {
	    if (rs > rlen) {
	        rs = SR_OVERFLOW ;
	    }
	} else {
	    rs = (- errno) ;
	} /* end if */
	return rs ;
}
/* end subroutine (std_vsnprintf) */


