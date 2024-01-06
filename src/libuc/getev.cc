/* getev SUPPORT */
/* lang=C20 */

/* get an environment variable by name */
/* version %I% last-modified %G% */

#define	CF_DEBUGS	0		/* compile-time debugging */

/* revision history:

	= 1998-09-10, David A­D­ Morano
	This program was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	getev

	Description:
	This subroutine is similar to 'getenv(3c)' except that the
	variable array (an array of pointers to strings) is explicitly
	passed by the caller.

	Synopsis:
	int getev(mainv envv,cchar *kp,int kl,cchar **rpp) noex

	Arguments:
	envv		array of pointers to strings
	kp		name c-string pointer to search for
	kl		name c-string length
	rpp		if not nullptr a pointer to the found is stored

	Returns:
	>=0		length of found string (if any)
	SR_NOTFOUND	name was not found
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>
#include	<sys/types.h>
#include	<cstring>		/* for |strlen(3c)| */
#include	<usystem.h>
#include	<matkeystr.h>
#include	<localmisc.h>


/* local defines */


/* local namespaces */


/* local typedefs */


/* external subroutines */


/* local variables */


/* exported subroutines */

int getev(cchar **envv,cchar *np,int nl,cchar **rpp) noex {
	int		rs = SR_FAULT ;
	int		vl = 0 ;
	cchar		*vp = nullptr ;
	if (envv && np) {
	    rs = SR_INVALID ;
	    if (np[0]) {
	        int	ei ;
	        if ((ei = matkeystr(envv,np,nl)) >= 0) {
	            if ((vp = strchr(envv[ei],'=')) != nullptr) {
	                vp += 1 ;
	                vl = strlen(vp) ;
	            }
	        } else {
	            rs = SR_NOTFOUND ;
	        }
	    } /* end if (valid) */
	} /* end if (non-null) */
	if (rpp) {
	    *rpp = (rs >= 0) ? vp : nullptr ;
	}
	return (rs >= 0) ? vl : rs ;
}
/* end subroutine (getev) */


