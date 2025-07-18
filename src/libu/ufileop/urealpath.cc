/* urealpath SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* resolve a path without symbolic or relative components */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-13, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	u_realpath

	Description:
	This subroutine takes an existing path and creates a new path
	that does not contain either symbolic or relative components.

	Synopsis:
	int u_realpath(cchar *fname,char *rbuf,char **rpp) noex

	Arguments:
	fname		file-name to resolve
	rbuf		result buffer pointer (must be MAXPATHLEN in length)
	rpp		result pointer pointer

	Returns:
	>=0		length of resuling resolved file-name
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/param.h>		/* standard says this is necessary */
#include	<cerrno>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>		/* |realpath(3c)| */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<utimeout.h>
#include	<errtimer.hh>
#include	<localmisc.h>

#include	"ufileop.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* external subroutines */

static sysret_t sr_realpath(cchar *fn,char *rbuf,char **rpp) noex {
	cnullptr	np{} ;
	int		rs = SR_OK ;
	if (char *rp ; (rp = realpath(fn,rbuf)) != np) ylikely {
	    *rpp = rp ;
	} else {
	    rs = (- errno) ;
	}
	return rs ;
} /* end subroutine (sr_realpath) */

sysret_t u_realpath(cchar *fname,char *rbuf,char **rpp) noex {
	errtimer	to_mfile = utimeout[uto_mfile] ;
	errtimer	to_nfile = utimeout[uto_nfile] ;
	errtimer	to_nomem = utimeout[uto_nomem] ;
	errtimer	to_nospc = utimeout[uto_nospc] ;
	errtimer	to_again = utimeout[uto_again] ;
        reterr          r ;
	int		rs ;
	char		*rp = nullptr ;
	repeat {
	    if ((rs = sr_realpath(fname,rbuf,&rp)) < 0) nlikely {
                r(rs) ;                 /* <- default causes exit */
	        switch (rs) {
	        case SR_MFILE:
                    r = to_mfile(rs) ;
		    break ;
	        case SR_NFILE:
                    r = to_nfile(rs) ;
		    break ;
		case SR_NOMEM:
                    r = to_nomem(rs) ;
		    break ;
	        case SR_NOSPC:
                    r = to_nospc(rs) ;
		    break ;
	        case SR_AGAIN:
                    r = to_again(rs) ;
		    break ;
	        case SR_INTR:
		    r(false) ;
	            break ;
	        } /* end switch */
		rs = r ;
	    } /* end if (ok) */
	} until ((rs >= 0) || r.fexit) ;
	if (rpp) *rpp = (rs >= 0) ? rp : nullptr ;
	return rs ;
}
/* end method (u_realpath) */


