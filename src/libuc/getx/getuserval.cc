/* getuserval SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* get an attribute (a c-string) for a specified user-name */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-07-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	getuserval

	Descriptor:
	This subroutine retrieves an attribute for the given
	user and the given attribute key-name.

	Synopsis:
	int getuserval(char *rbuf,int rlen,ccp atp,cchar *un) noex

	Arguments:
	rbuf		user supplied buffer to hold result
	rlen		length of user supplied buffer
	atp		pointer to attribute key-name c-string
	un		username to look up

	Returns:
	>=0		length of return organization string
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<uclibmem.h>		/* LIBUC */
#include	<ucpwcache.h>		/* LIBUC */
#include	<getax.h>		/* LIBUC */
#include	<getpwx.h>		/* LIBUC */
#include	<getusername.h>		/* LIBUC */
#include	<filereadln.h>		/* LIBUC */
#include	<sncpyx.h>		/* LIBUC */
#include	<mkpathx.h>		/* LIBUC */
#include	<char.h>		/* LIBUC */
#include	<isnot.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */

#include	"getuserval.h"

#pragma		GCC dependency		"mod/libutil.ccm"
#pragma		GCC dependency		"mod/uconstants.ccm"

import libutil ;
import uconstants ;			/* |varname(3u)| */

/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int getuserval(char *rbuf,int rlen,cchar *atp,cchar *un) noex {
	int		rs = SR_FAULT ;
	int		len = 0 ; /* return-value */
	if (rbuf && atp) ylikely {
	    rs = SR_INVALID ;
	    rbuf[0] = eos ;
	    if ((rlen > 0) && un) {
		rs = SR_INVALID ;
		if (un[0] && atp[0]) {
		    rs = SR_OK ;
		} /* end if */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
} /* end subroutine (getuserval) */


/* local subroutines */


