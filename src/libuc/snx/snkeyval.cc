/* snkeyval SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* string formatting (key-value pair) */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	snkeyval

	Description:
	We make something that looks like an environment vaiable
	string (with the key included) from a key and value
	specification (consisting of a string pointer and a string
	length).

	Synopsis:
	int snkeyval(char *dbuf,int dlen,cc *kp,int kl,cc *vp,int vl) noex

	Arguments:
	dbuf		destination buffer
	dlen		destination buffer length
	kp		key-pointer
	kl		key length
	vp		value-pointer
	vl		value-length

	Returns:
	>=0		length of created string
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<usystem.h>
#include	<storebuf.h>
#include	<localmisc.h>

#include	"snx.h"


/* local defines */

#define	CHX_MIDDLE	'='


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int snkeyval(char *dbuf,int dlen,cchar *kp,int kl,cchar *vp,int vl) noex {
	int		rs = SR_FAULT ;
	if (dbuf && kp) {
	    rs = SR_INVALID ;
	    if (kp[0]) {
    	        if (storebuf sb(dbuf,dlen) ; (rs = sb.strw(kp,kl)) >= 0) {
	            if ((rs = sb.chr(CHX_MIDDLE)) >= 0) {
	                if (vp) rs = sb.strw(vp,vl) ;
	                if (rs >= 0) rs = sb.idx ;
		    }
		} /* end if (storebuf) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (snkeyval) */


