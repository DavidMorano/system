/* pcsgetorg SUPPORT */
/* lang=C++20 */

/* get the PCS organization */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-05-01, David A­D­ Morano
	This subroutine is originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	pcsgetorg

	Description:
	This subroutine retrieves the PCS-specific organization string.

	Synopsis:
	int pcsgetorg(cc *pr,char *rbuf,int rlen,cc *un) noex

	Arguments:
	pr		PCS program root path
	rbuf		caller supplied buffer to place result in
	rlen		length of caller supplied buffer
	un		username

	Returns:
	>=0		length of returned ID
	<0		error in process of creating ID (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<usystem.h>
#include	<localmisc.h>

#include	"pcsgetorg.h"


/* local defines */


/* external subroutines */

extern "C" {
    extern int	localgetorg(cchar *,char *,int,cchar *) noex ;
}


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int pcsgetorg(cchar *pr,char *rbuf,int rlen,cchar *un) noex {
	int		rs = SR_FAULT ;
	if (pr && rbuf) {
	    rs = SR_INVALID ;
	    rbuf[0] = '\0' ;
	    if (pr[0]) {
	        rs = localgetorg(pr,rbuf,rlen,un) ;
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (pcsgetorg) */


