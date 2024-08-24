/* uexec SUPPORT */
/* lang=C++20 */

/* translation layer interface for UNIX® equivalents */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services
	(RNS).

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Coding note:
	The Solaris® (from Sun Microsystems) OS is messed up if
	|execvp(2)| is called with a rooted program file.  Why is
	it hosed? Who can fathom the screw-up known as Solaris?  So
	we check if the program file is rooted ourselves and if it
	is, we change and call |execv(2)| instead.  Did I mention
	that Solaris is a messed up OS?

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>
#include	<unistd.h>
#include	<cerrno>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysrets.h>
#include	<usyscalls.h>
#include	<errtimer.hh>
#include	<localmisc.h>

#include	"uexec.h"


/* local defines */


/* local namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct uexecflags {
	uint		fintr:1 ;	/* request interrupts to return */
    } ; /* end struct (uexecflags) */
    struct uexecbase ;
    typedef int (uexecbase::*uexecbase_m)(cchar *,mainv,mainv) noex ;
    struct uexecbase {
	uexecbase_m	m{} ;
	uexecflags	f{} ;
	int operator () (cchar *,mainv,mainv = nullptr) noex ;
	int callstd(cchar *,mainv,mainv) noex ;
	int iexecve(cchar *,mainv,mainv) noex ;
	int iexecv(cchar *,mainv,mainv) noex ;
	int iexecvp(cchar *,mainv,mainv) noex ;
    } ; /* end struct (uexecbase) */
}


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int u_execve(cchar *fn,mainv argv,mainv envv) noex {
	uexecbase	eo ;
	eo.m = &uexecbase::iexecve ;
	return eo(fn,argv,envv) ;
}
/* end subroutine (uexecve) */

int u_execv(cchar *fn,mainv argv) noex {
	uexecbase	eo ;
	eo.m = &uexecbase::iexecv ;
	return eo(fn,argv) ;
}
/* end subroutine (u_execv) */

int u_execvp(cchar *fn,mainv argv) noex {
	uexecbase	eo ;
	eo.m = &uexecbase::iexecvp ;
	return eo(fn,argv) ;
}
/* end subroutine (u_execvp) */


/* local subroutines */

int uexecbase::callstd(cchar *fn,mainv argv,mainv envv) noex {
	return (this->*m)(fn,argv,envv) ;
}

int uexecbase::iexecve(cchar *fn,mainv argv,mainv envv) noex {
	int		rs ;
	char		*const *eav = (char *const *) argv ;
	char		*const *eev = (char *const *) envv ;
	if ((rs = execve(fn,eav,eev)) < 0) {
	    rs = (- errno) ;
	}
	return rs ;
}
/* end method (uexecbase::iexecve) */

int uexecbase::iexecv(cchar *fn,mainv argv,mainv) noex {
	int		rs ;
	char		*const	*eav = (char *const *) argv ;
	if ((rs = execv(fn,eav)) < 0) {
	    rs = (- errno) ;
	}
	return rs ;
}
/* end method (uexecbase::iexecv) */

int uexecbase::iexecvp(cchar *fn,mainv argv,mainv) noex {
	int		rs ;
	char		*const	*eav = (char *const *) argv ;
	if (fn[0] != '/') {
	    if ((rs = execvp(fn,eav)) < 0) {
		rs = (- errno) ;
	    }
	} else {
	    if ((rs = execv(fn,eav)) < 0) {
		rs = (- errno) ;
	    }
	}
	return rs ;
}
/* end method (uexecbase::iexecvp) */

int uexecbase::operator () (cchar *fn,mainv argv,mainv envv) noex {
	int		rs = SR_FAULT ;
	if (fn && argv) {
	    rs = SR_INVALID ;
	    if (fn[0]) {
	        errtimer	to_again	= utimeout[uto_again] ;
	        errtimer	to_busy		= utimeout[uto_busy] ;
	        errtimer	to_nomem	= utimeout[uto_nomem] ;
	        errtimer	to_nosr		= utimeout[uto_nosr] ;
	        errtimer	to_nobufs	= utimeout[uto_nobufs] ;
	        errtimer	to_mfile	= utimeout[uto_mfile] ;
	        errtimer	to_nfile	= utimeout[uto_nfile] ;
	        errtimer	to_nolck	= utimeout[uto_nolck] ;
	        errtimer	to_nospc	= utimeout[uto_nospc] ;
	        errtimer	to_dquot	= utimeout[uto_dquot] ;
	        errtimer	to_io		= utimeout[uto_io] ;
	        reterr		r ;
	        repeat {
	            if ((rs = callstd(fn,argv,envv)) < 0) {
		        r(rs) ;			/* <- default causes exit */
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
	                case SR_NOSR:
                            r = to_nosr(rs) ;
		            break ;
	                case SR_NOBUFS:
	                    r = to_nobufs(rs) ;
	                    break ;
                        case SR_MFILE:
                            r = to_mfile(rs) ;
                            break ;
                        case SR_NFILE:
                            r = to_nfile(rs) ;
                            break ;
	                case SR_NOLCK:
                            r = to_nolck(rs) ;
		            break ;
                        case SR_NOSPC:
                            r = to_nospc(rs) ;
		            break ;
	                case SR_DQUOT:
                            r = to_dquot(rs) ;
		            break ;
	                case SR_IO:
                            r = to_io(rs) ;
		            break ;
		        case SR_INPROGRESS:
			    r(false) ;
		            break ;
                        case SR_INTR:
			    if (! f.fintr) {
			        r(false) ;
		            }
                            break ;
                        } /* end switch */
		        rs = r ;
                    } /* end if (error) */
	        } until ((rs >= 0) || r.fexit) ;
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
} /* end method (uexecbase::operator) */


