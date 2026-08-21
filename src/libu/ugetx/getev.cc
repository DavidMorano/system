/* getev SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* get the value of an environment variable */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-21, David A­D­ Morano
	This program was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	getev

	Description:
	This subroutine is similar to |getenv(3c)| except that the
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

#include	<envstandards.h>	/* ordered first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstring>		/* CSTD |strchr(3c)| */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usupport.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

#include	"getev.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| */

/* local defines */


/* imported namespaces */

using libu::matkeystr ;			/* subroutine */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int getev(con mainv envv,cchar *kp,int kl,cchar **rpp) noex {
	int		rs = SR_FAULT ;
	int		vl = 0 ; /* return-value */
	cchar		*vp = nullptr ;
	if (envv && kp) ylikely {
	    rs = SR_INVALID ;
	    if (kp[0]) ylikely {
		rs = SR_OK ;
	        if (int ei ; (ei = matkeystr(envv,kp,kl)) >= 0) {
	            if ((vp = strchr(envv[ei],'=')) != nullptr) {
	                vp += 1 ;
	                vl = lenstr(vp) ;
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
} /* end subroutine (getev) */


