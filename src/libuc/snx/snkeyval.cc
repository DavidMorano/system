/* snkeyval SUPPORT */
/* lang=C++20 */

/* string formatting (key-value pair) */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was originally written.

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
	vp		value-pointer
	kl		key length
	vl		value-length

	Returns:
	>=0		length of created string
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<usystem.h>
#include	<storebuf.h>
#include	<sncpyx.h>


/* local defines */

#define	CHX_MIDDLE	'='


/* external subroutines */


/* external variables */


/* local variables */


/* exported variables */


/* exported subroutines */

int snkeyval(char *dbuf,int dlen,cchar *kp,int kl,cchar *vp,int vl) noex {
	int		rs = SR_FAULT ;
	int		i = 0 ;
	if (dbuf && kp) {
	    rs = SR_OK ;
	    if (rs >= 0) {
	        rs = storebuf_strw(dbuf,dlen,i,kp,kl) ;
	        i += rs ;
	    }
	    if (rs >= 0) {
	        rs = storebuf_char(dbuf,dlen,i,CHX_MIDDLE) ;
	        i += rs ;
	    }
	    if ((rs >= 0) && vp) {
	        rs = storebuf_strw(dbuf,dlen,i,vp,vl) ;
	        i += rs ;
	    }
	} /* end if (non-null) */
	return (rs >= 0) ? i : rs ;
}
/* end subroutine (snkeyval) */


