/* prqotd_util SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* open a channel (file-descriptor) to the quote-of-the-day (QOTD) */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:

	Description:

	Synopsis:

	Arguments:

	Returns:

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>
#include	<usysbase.h>
#include	<usyscalls.h>
#include	<uclibmem.h>
#include	<ucgetpid.h>
#include	<bufsizeget.h>
#include	<getmjd.h>
#include	<ids.h>
#include	<tmtime.hh>
#include	<storebuf.h>
#include	<strn.h>		/* |strnchr(3uc)| */
#include	<mkpathxw.h>
#include	<mkfnamesuf.h>
#include	<localmisc.h>		/* |DIGBUFLEN| */

#include	"prqotd_util.hh"

#pragma		GCC dependency		"mod/libutil.ccm"
#pragma		GCC dependency		"mod/uconstants.ccm"

import libutil ;			/* |lenstr(3u)| */
import uconstants ;			/* |varname(3u)| */

/* local defines */

#define	EBUFLENMULT		3	/* e-buffer-length multiplier */
#define	VBUFLENMULT		2	/* e-buffer-length multiplier */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

namespace prqotd {
    initer::operator int () noex {
	static cint rsv = var ;
	return rsv ;
    } ; /* end method (initer::operator) */
} /* end namespace (prqotd) */

namespace prqotd {
    vars::operator int () noex {
	int	rs ;
	if ((rs = u_getenviron(&envv)) >= 0) {
	    if ((rs = ucpid) >= 0) {
		pid = rs ;
	        if ((rs = bufsizeget(bufsize_mn)) >= 0) {
		    maxnamelen = rs ;
	            if ((rs = bufsizeget(bufsize_mp)) >= 0) {
		        maxpathlen = rs ;
			if ((rs = bufsizeget(bufsize_nn)) >= 0) {
			    nodenamelen = rs ;
			    if ((rs = bufsizeget(bufsize_hostname)) >= 0) {
			        hostnamelen = rs ;
			        {
		                    ebuflen = (maxpathlen * EBUFLENMULT) ;
		                    vbuflen = (maxpathlen * VBUFLENMULT) ;
			        }
			    } /* end if (bufsizeget) */
			} /* end if (bufsizeget) */
	            }
	        }
	    } /* end if (ucpid) */
	} /* end if (u_getenviron) */
	return rs ;
    } /* end method (vars::operator) */
} /* end namespace (prqotd) */

namespace prqotd {
    int getdefmjd(time_t dt) noex {
	int		rs ;
	if (dt == 0) dt = getustime ;
	if (tmtime ct ; (rs = tmtime_timelocal(&ct,dt)) >= 0) {
	    cint	y = (ct.year + TMTIME_YEARBASE) ;
	    cint	m = ct.mon ;
	    cint	d = ct.mday ;
	    rs = getmjd(y,m,d) ;
	} /* end if */
	return rs ;
    } /* end subroutine (getdefmjd) */
} /* end namespace (prqotd) */

namespace prqotd {
    int mkqfname(char *rbuf,cchar *qdname,int mjd) noex {
	int		rlen = var.maxpathlen ;
	int		rs = SR_OK ;
	int		i = 0 ; /* return-value */
	if (rs >= 0) {
	    rs = storebuf_strw(rbuf,rlen,i,qdname,-1) ;
	    i += rs ;
	}
	if ((rs >= 0) && (i > 0) && (rbuf[i-1] != '/')) {
	    rs = storebuf_chr(rbuf,rlen,i,'/') ;
	    i += rs ;
	}
	if (rs >= 0) {
	    rs = storebuf_chr(rbuf,rlen,i,'q') ;
	    i += rs ;
	}
	if (rs >= 0) {
	    rs = storebuf_deci(rbuf,rlen,i,mjd) ;
	    i += rs ;
	}
	return (rs >= 0) ? i : rs ;
    } /* end subroutine (mkqfname) */
} /* end namespace (prqotd) */

namespace prqotd {
    int mkourname(char *rbuf,cc *pr,cc *inter,cc *sp,int sl) noex {
	int		rs = SR_OK ;
	if (strnchr(sp,sl,'/') != nullptr) {
	    if (sp[0] != '/') {
	        rs = mkpath2w(rbuf,pr,sp,sl) ;
	    } else {
	        rs = mkpath1w(rbuf,sp,sl) ;
	    }
	} else {
	    rs = mkpath3w(rbuf,pr,inter,sp,sl) ;
	}
	return rs ;
    } /* end subroutine (mkourname) */
} /* end namespace (prqotd) */

namespace prqotd {
    initer		init ;
    vars		var ;
} /* end namespace (prqotd) */


