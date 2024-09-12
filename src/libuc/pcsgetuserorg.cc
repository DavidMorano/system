/* pcsgetuserorg SUPPORT */
/* lang=C++20 */

/* get the organization name (string) for a specified user-name */
/* version %I% last-modified %G% */

#define	CF_ORGSYS	0		/* get from system? */

/* revision history:

	= 1998-07-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	pcsgetuserorg

	Description:
	This subroutine retrieves the organization name (string) for a
	specified user-name.

	Synopsis:
	int pcsgetuserorg(cc *pr,char *rbuf,int rlen,cc *un) noex

	Arguments:
	pr		program-root
	rbuf		user supplied buffer to hold result
	rlen		length of user supplied buffer
	un		username to look up

	Returns:
	>=0		length of return organization string
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<usystem.h>
#include	<getusername.h>
#include	<getuserorg.h>
#include	<isnot.h>
#include	<localmisc.h>

#include	"pcsgetorg.h"
#include	"pcsgetuserorg.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int pcsgetuserorg(cchar *pr,char *rbuf,int rlen,cchar *un) noex {
	int		rs = SR_FAULT ;
	if (pr && rbuf && un) {
	    rs = SR_INVALID ;
	    rbuf[0] = '\0' ;
	    if (pr[0] && un[0]) {
	        rs = getuserorg(rbuf,rlen,un) ;
	        if (isNotPresent(rs) || (rs == 0)) {
	            rs = pcsgetorg(pr,rbuf,rlen,un) ;
	        }
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (pcsgetuserorg) */


