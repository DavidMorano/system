/* uiname SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* try to divine the best real name from a USERINFO object */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	uiname

	Description:
	We try to divine the best name that we can for the current
	user.  We do this simply by looking through some options
	contained in the USERINFO object.

	Synopsis:
	int uiname(char *rbuf,int rlen,usernfo *uip) noex

	Arguments:
	rbuf		supplied result buffer
	rlen		supplied result buffer length
	uip		pointer to USERINFO object

	Returns:
	>=0		length of resulting name
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<userinfo.h>
#include	<sncpyx.h>
#include	<localmisc.h>


/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* local structures */


/* local variables */


/* exported variables */


/* exported subroutines */

int uiname(USERINFO *uip,char *rbuf,int rlen) noex {
	int		rs = SR_OK ;
	if (rbuf && uip) {
	    rs = SR_INVALID ;
	    rbuf[0] = '\0' ;
	    if (rlen >= 0) {
		cchar	*np = nullptr ;
		rs = SR_NOTFOUND ;
	        if (uip->fullname && (uip->fullname[0] != '\0')) {
	            np = uip->fullname ;
	        } else if (uip->name && (uip->name[0] != '\0')) {
	            np = uip->name ;
	        } else if (uip->mailname && (uip->mailname[0] != '\0')) {
	            np = uip->mailname ;
	        } else if (uip->username && (uip->username[0] != '\0')) {
	            np = uip->username ;
	        }
	        if (np) {
	            rs = sncpy1(rbuf,rlen,np) ;
	        }
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (uiname) */


