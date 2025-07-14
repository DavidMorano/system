/* ucrealpath SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* resolve a path without symbolic or relative components */
/* version %I% last-modified %G% */

#define	CF_MKPATH	0		/* use |mkpath(3uc)| */

/* revision history:

	= 1998-04-13, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	uc_realpath

	Description:
	This subroutine takes an existing path and creates a new path
	that does not contain either symbolic or relative components.

	Synopsis:
	int uc_realpath(cchar *fname,char *rbuf) noex

	Arguments:
	fname		file-name to resolve
	rbuf		result buffer pointer (must be MAXPATHLEN in length)

	Returns:
	>=0		length of resuling resolved file-name
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/param.h>		/* standard says this is necessary */
#include	<cerrno>
#include	<climits>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>		/* |realpath(3c)| */
#include	<cstring>		/* |strcpy(3c)| */
#include	<usystem.h>
#include	<bufsizevar.hh>
#include	<errtimer.hh>
#include	<mkpathx.h>
#include	<localmisc.h>

import libutil ;

/* local defines */

#ifndef	CF_MKPATH
#define	CF_MKPATH	0		/* use |mkpath(3uc)| */
#endif


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

sysret_t std_realpath(cchar *,char *,char **) noex ;


/* local variables */

static bufsizevar	maxpathlen(getbufsize_mp) ;

constexpr bool		f_mkpath = CF_MKPATH ;


/* exported variables */


/* external subroutines */

int uc_realpath(cchar *fname,char *rbuf) noex {
	int		rs = SR_FAULT ;
	int		rl = 0 ; /* return-value */
	if (fname && rbuf) {
	    cnullptr	np{} ;
	    rs = SR_INVALID ;
	    rbuf[0] = '\0' ;
	    if (fname[0]) {
		char	*rp{} ;
		if_constexpr (f_mkpath) {
	            if ((rs = std_realpath(fname,np,&rp)) >= 0) {
			rs = mkpath(rbuf,rp) ;
			rl = rs ;
			free(rp) ;
		    } /* end if (std_realpath) */
		} else {
		    if ((rs = maxpathlen) >= 0) {
		        cint	rlen = rs ;
	                if ((rs = std_realpath(fname,np,&rp)) >= 0) {
		            if ((rl = lenstr(rp)) <= rlen) {
		                strcpy(rbuf,rp) ;
			        rs = SR_OK ;
		            } else {
			        rs = SR_OVERFLOW ;
		            }
			    free(rp) ;
		        } /* end if (std_realpath) */
		    } /* end if (maxpathlen) */
		} /* end if_constexpr (f_mkpath) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? rl : rs ;
}
/* end subroutine (uc_realpath) */


/* local subroutines */

sysret_t std_realpath(cchar *fname,char *rbuf,char **rpp) noex {
	errtimer	to_mfile = utimeout[uto_mfile] ;
	errtimer	to_nfile = utimeout[uto_nfile] ;
	errtimer	to_nomem = utimeout[uto_nomem] ;
	errtimer	to_nospc = utimeout[uto_nospc] ;
	errtimer	to_again = utimeout[uto_again] ;
        reterr          r ;
	int		rs ;
	char		*rp = nullptr ;
	repeat {
	    rs = SR_OK ;
	    if ((rp = realpath(fname,rbuf)) == nullptr) {
		rs = (- errno) ;
	    }
	    if (rs < 0) {
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
	*rpp = (rs >= 0) ? rp : nullptr ;
	return rs ;
}
/* end method (std_realpath) */


