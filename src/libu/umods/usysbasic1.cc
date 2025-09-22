/* usysbasic MODULE (System-Basic-Library) */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* interface components for UNIX® library-3c */
/* some basic system calls */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Names:
	uatfork
	uatæxit

	Description:
	I am providing a couple of basic library calls for use in
	these other low-level subroutines.  Unlike some other
	middleware calls to the C-language library system, these
	are quite straight-forward.

*******************************************************************************/

module ;

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/param.h>		/* |statfs| */
#include	<sys/mount.h>		/* |statfs| */
#include	<sys/statvfs.h>		/* |statvfs| */
#include	<sys/stat.h>		/* |stat(3c)| */
#include	<pthread.h>		/* |pthread_atfork(3c)| */
#include	<cerrno>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>		/* |atexit(3c)| */
#include	<cstring>		/* |strncpy(3c)| + |stpcpy(3c)| */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<usys.h>		/* |statfile(2)| + |statfilefs(2)| */
#include	<utimeout.h>
#include	<errtimer.hh>
#include	<localmisc.h>

#pragma		GCC dependency		"mod/libutil.ccm"

module usysbasic ;

import libutil ;			/* |lenstr(3u)| */

/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local stuctures */

namespace {
    struct ucaller ;
    typedef int (ucaller::*mem_m)() noex ;
    struct ucaller {
	mem_m		m ;
	void_f		func ;
	void_f		bf, apf, acf ;
	cchar		*fn ;
	void		*sbp ;		/* |stat{x}| */
	char		*rbuf ;
	int		rlen ;
	int		am ;
	ucaller(void_f abf,void_f aapf,void_f aacf) noex {
	    bf = abf ;
	    apf = aapf ;
	    acf = aacf ;
	} ;
	ucaller(void_f f) noex : func(f) { } ;
	ucaller(cchar *afn,void *asp) noex : fn(afn), sbp(asp) { } ;
	ucaller(cchar *afn,int aam) noex : fn(afn), am(aam) { } ;
	ucaller(char *abp,int abl) noex : rbuf(abp), rlen(abl) { } ;
	int callstd() noex {
            int         rs = SR_BUGCHECK ;
            if (m) {
                rs = (this->*m)() ;
            }
            return rs ;
        } ; /* end method (callstd) */
	int stdatfork() noex ;
	int stdatexit() noex ;
	int stdstat() noex ;
	int stdstatfs() noex ;
	int stdstatvfs() noex ;
	int stdgetcwd() noex ;
	int stdaccess() noex ;
	operator int () noex ;
    } ; /* end struct (ucaller) */
} /* end namespace */


/* local variables */


/* exported variables */


/* exported subroutines */

int uatfork(void_f b,void_f ap,void_f ac) noex {
	int		rs = SR_FAULT ;
	if (b || ap || ac) {
	    ucaller	po(b,ap,ac) ;
	    po.m = &ucaller::stdatfork ;
	    rs = po ;
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (uatfork) */
int uatexit(void_f func) noex {
	int		rs = SR_FAULT ;
	if (func) {
	    ucaller	aeo(func) ;
	    aeo.m = &ucaller::stdatexit ;
	    rs = aeo ;
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (uatexit) */
int ustatfile(cchar *afn,ustat *asp) noex {
	int		rs = SR_FAULT ;
	if (afn && asp) {
	    rs = SR_INVALID ;
	    if (afn[0]) {
	        ucaller	po(afn,asp) ;
	        po.m = &ucaller::stdstat ;
	        rs = po ;
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (ustatfile) */
int ustatfilefs(cchar *afn,ustatfs *asp) noex {
	int		rs = SR_FAULT ;
	if (afn && asp) {
	    rs = SR_INVALID ;
	    if (afn[0]) {
	        ucaller	po(afn,asp) ;
	        po.m = &ucaller::stdstatfs ;
	        rs = po ;
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (ustatfilefs) */
int ustatfilevfs(cchar *afn,ustatvfs *asp) noex {
	int		rs = SR_FAULT ;
	if (afn && asp) {
	    rs = SR_INVALID ;
	    if (afn[0]) {
	        ucaller	po(afn,asp) ;
	        po.m = &ucaller::stdstatvfs ;
	        rs = po ;
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (ustatfilevfs) */
int ugetcwd(char *rb,int rl) noex {
	int		rs = SR_FAULT ;
	if (rb) {
	    rs = SR_INVALID ;
	    if (rl > 0) {
	        ucaller	po(rb,rl) ;
	        po.m = &ucaller::stdgetcwd ;
	        rs = po ;
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (ugetcwd) */
int uaccess(cchar *afn,int am) noex {
	int		rs = SR_FAULT ;
	if (afn) {
	    rs = SR_INVALID ;
	    if (afn[0] && (am >= 0)) {
	        ucaller	po(afn,am) ;
	        po.m = &ucaller::stdaccess ;
	        rs = po ;
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (uaccess) */
int udiraccess(cchar *afn,int am) noex {
	int		rs = SR_FAULT ;
	if (afn) {
	    rs = SR_INVALID ;
	    if (afn[0] && (am >= 0)) {
		if (ustat sb ; (rs = ustatfile(afn,&sb)) >= 0) {
		    rs = SR_NOTDIR ;
		    if (S_ISDIR(sb.st_mode)) {
			rs = uaccess(afn,am) ;
		    }
		}
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (udiraccess) */


/* local subroutines */

ucaller::operator int () noex {
	errtimer	to_again	= utimeout[uto_again] ;
	errtimer	to_busy		= utimeout[uto_busy] ;
	errtimer	to_nomem	= utimeout[uto_nomem] ;
	errtimer	to_nospc	= utimeout[uto_nospc] ;
	errtimer	to_nosr		= utimeout[uto_nosr] ;
	errtimer	to_nobufs	= utimeout[uto_nobufs] ;
	reterr		r ;
	int		rs ;
	repeat {
            if ((rs = callstd()) < 0) {
		r(rs) ;
                switch (rs) {
                case SR_AGAIN:
		    r = to_again(rs) ;
                    break ;
                case SR_BUSY:
		    r = to_busy(rs) ;
                    break ;
                case SR_NOMEM:
		    r = to_nomem(rs) ;
                    break ;
                case SR_NOSPC:
		    r = to_nospc(rs) ;
                    break ;
                case SR_NOSR:
		    r = to_nosr(rs) ;
                    break ;
                case SR_NOBUFS:
		    r = to_nobufs(rs) ;
                    break ;
                case SR_INTR:
		    r(false) ;
                    break ;
                } /* end switch */
		rs = r ;
            } /* end if (error) */
	} until ((rs >= 0) || r.fexit) ;
	return rs ;
}
/* end subroutine (ucaller::operator) */

sysret_t ucaller::stdatfork() noex {
	int		rs ;
	if ((rs = pthread_atfork(bf,apf,acf)) < 0) {
	    rs = (- errno) ;
	}
	return rs ;
}
/* end method (ucaller::stdatfork) */

sysret_t ucaller::stdatexit() noex {
	int		rs ;
	if ((rs = atexit(func)) < 0) {
	    rs = (- errno) ;
	}
	return rs ;
}
/* end method (ucaller::stdatexit) */

sysret_t ucaller::stdstat() noex {
    	ustat		*p = cast_static<ustat *>(sbp) ;
	int		rs ;
	if ((rs = statfile(fn,p)) < 0) {
	    rs = (- errno) ;
	}
	return rs ;
}
/* end method (ucaller::stdstat) */

sysret_t ucaller::stdstatfs() noex {
    	ustatfs		*p = cast_static<ustatfs *>(sbp) ;
	int		rs ;
	if ((rs = statfilefs(fn,p)) < 0) {
	    rs = (- errno) ;
	}
	return rs ;
}
/* end method (ucaller::stdstatfs) */

sysret_t ucaller::stdstatvfs() noex {
    	ustatvfs	*p = cast_static<ustatvfs *>(sbp) ;
	int		rs ;
	if ((rs = statfilevfs(fn,p)) < 0) {
	    rs = (- errno) ;
	}
	return rs ;
}
/* end method (ucaller::stdstatvfs) */

sysret_t ucaller::stdgetcwd() noex {
    	cnullptr	np{} ;
    	csize		rsize = size_t(rlen) ;
	int		rs = SR_OK ;
	if (char *rp ; (rp = getcwd(rbuf,(rsize + 1))) != np) {
	    rs = lenstr(rp) ;
	} else {
	    rs = (- errno) ;
	}
	return rs ;
}
/* end method (ucaller::stdgetcwd) */

sysret_t ucaller::stdaccess() noex {
	int		rs ;
	if ((rs = access(fn,am)) < 0) {
	    rs = (- errno) ;
	}
	return rs ;
}
/* end method (ucaller::stdaccess) */


