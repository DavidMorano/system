/* uc_realpath SUPPORT */
/* encoding=ISO8859-1 */
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

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<climits>
#include	<cerrno>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>		/* |realpath(3c)| */
#include	<cstring>		/* |strlen(3c)| + |strcpy(3c)| */
#include	<usystem.h>
#include	<bufsizevar.hh>
#include	<mkpathx.h>
#include	<localmisc.h>


/* local defines */

#ifndef	CF_MKPATH
#define	CF_MKPATH	0		/* use |mkpath(3uc)| */
#endif


/* external subroutines */


/* external variables */


/* local variables */

static bufsizevar	maxpathlen(getbufsize_mp) ;

constexpr bool		f_mkpath = CF_MKPATH ;


/* exported variables */


/* external subroutines */

int uc_realpath(cchar *fname,char *rbuf) noex {
	int		rs = SR_FAULT ;
	int		rl = 0 ;
	if (fname && rbuf) {
	    cnullptr	np{} ;
	    rs = SR_INVALID ;
	    if (fname[0]) {
		if_constexpr (f_mkpath) {
	            if (char *rp ; (rp = realpath(fname,np)) != np) {
			rs = mkpath(rbuf,rp) ;
			rl = rs ;
		    } else {
		        rs = (- errno) ;
		    } /* end if (realpath) */
		} else {
		    if ((rs = maxpathlen) >= 0) {
		        cint	rlen = rs ;
	                if (char *rp ; (rp = realpath(fname,np)) != np) {
		            if ((rl = strlen(rp)) <= rlen) {
		                strcpy(rbuf,rp) ;
			        rs = SR_OK ;
		            } else {
			        rs = SR_OVERFLOW ;
		            }
		        } else {
		            rs = (- errno) ;
		        } /* end if (realpath) */
		    } /* end if (maxpathlen) */
		} /* end if_constexpr (f_mkpath) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? rl : rs ;
}
/* end subroutine (uc_realpath) */


