/* ucinproj SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* interface component for UNIX® library-3c */
/* project DB access */


/* revision history:

	= 1998-06-16, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	uc_inproj

	Description:
	This subroutine was written so that we could use a single
	interface to access the 'passwd' database on all UNIX®
	platforms.  This code module provides a platform independent
	implementation of UNIX® 'passwd' database access subroutines.

	Symopsis:
	int uc_inproj(cchar *un,cchar *pn,char *rbuf,int rlen) noex

	Arguments:
	un		user-name to check
	pn		project-name to check
	rbuf		buffer to hold internal project entry data
	rlen		length of supplied buffer

	Returns:
	>0		YES	(user is in project)
	==0		NO	(user not in project)
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cerrno>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<usysbase.h>
#include        <errtimer.hh>
#include	<localmisc.h>

#include	"ucids.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

#if	defined(SYSHAS_PROJECT) && (SYSHAS_PROJECT > 0)
/******************************************************************************/

local sysret_t std_inproj(cchar *un,cchar *pn,char *rbuf,int rlen) noex {
    	csize		rsize = size_t(rlen) ;
	int		rs = SR_OK ;
	int		finproj{} ; /* return-value */
	errno = 0 ;
	finproj = inproj(username,projname,rbuf,rsize) ;
	if (errno != 0) {
	    rs = (- errno) ;
	}
	return (rs >= 0) ? finproj : rs ;
} /* end subroutine (std_inproj) */

int uc_inproj(cchar *username,cchar *projname,char *rbuf,int rlen) noex {
	int		rs = SR_FAULT ;
	int		finproj = false ;
	if (username && projname && rbuf) {
	    rs = SR_INVALID ;
	    if (rlen >= 0) {
		errtimer	to_mfile = utimeout[uto_mfile] ;
		errtimer	to_nfile = utimeout[uto_nfile] ;
		errtimer	to_nomem = utimeout[uto_nomem] ;
		errtimer	to_nospc = utimeout[uto_nospc] ;
		errtimer	to_again = utimeout[uto_again] ;
        	reterr          r ;
	        repeat {
		    if ((rs = std_inproj(un,pn,rbuf,rlen)) < 0) {
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
		    } else {
			finproj = rs ;
	            } /* end if (error) */
		} until ((rs >= 0) || r.fexit) ;
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? finproj : rs ;
}
/* end subroutine (uc_inproj) */

/******************************************************************************/
#else /* defined(SYSHAS_PROJECT) && (SYSHAS_PROJECT > 0) */
/******************************************************************************/

int uc_inproj(cchar *name,cc *projname,char *rbuf,int rlen) noex {
    	int		rs = SR_FAULT ;
	if (name && projname && rbuf) {
	    rs = SR_INVALID ;
	    if (rlen >= 0) {
	        rs = SR_NOSYS ;
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (uc_inproj) */

/******************************************************************************/
#endif /* defined(SYSHAS_PROJECT) && (SYSHAS_PROJECT > 0) */


