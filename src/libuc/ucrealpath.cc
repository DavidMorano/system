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
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>		/* |realpath(3c)| */
#include	<cstring>		/* |strcpy(3c)| */
#include	<usystem.h>
#include	<bufsizevar.hh>
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


/* local variables */

static bufsizevar	maxpathlen(getbufsize_mp) ;

constexpr bool		f_mkpath = CF_MKPATH ;


/* exported variables */


/* external subroutines */

int uc_realpath(cchar *fname,char *rbuf) noex {
	cnullptr	np{} ;
	int		rs = SR_FAULT ;
	int		rl = 0 ; /* return-value */
	if (fname && rbuf) ylikely {
	    rs = SR_INVALID ;
	    rbuf[0] = '\0' ;
	    if (fname[0]) ylikely {
		char	*rp{} ; /* used-multiple */
		if_constexpr (f_mkpath) {
	            if ((rs = u_realpath(fname,np,&rp)) >= 0) ylikely {
			rs = mkpath(rbuf,rp) ;
			rl = rs ;
			free(rp) ;
		    } /* end if (u_realpath) */
		} else {
		    if ((rs = maxpathlen) >= 0) ylikely {
		        cint	rlen = rs ;
	                if ((rs = u_realpath(fname,np,&rp)) >= 0) ylikely {
		            if ((rl = lenstr(rp)) <= rlen) ylikely {
		                strcpy(rbuf,rp) ;
			        rs = SR_OK ;
		            } else {
			        rs = SR_OVERFLOW ;
		            }
			    free(rp) ;
		        } /* end if (u_realpath) */
		    } /* end if (maxpathlen) */
		} /* end if_constexpr (f_mkpath) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? rl : rs ;
}
/* end subroutine (uc_realpath) */


