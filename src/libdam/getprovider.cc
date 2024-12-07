/* getprovider SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* get the machine provider string */
/* version %I% last-modified %G% */


/* revision history:

	= 2001-04-11, David A­D­ Morano
	This was originally written.

*/

/* Copyright © 2001 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	getprovider

	Description:
	Get and return the machine provider string.

	Synopsis:
	int getprovider(char *rbuf,int rlen)

	Arguments:
	rbuf		result buffer
	rlen		result buffer length

	Returns:
	>=0		number of characters returned
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<sys/types.h>
#include	<sys/systeminfo.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<usystem.h>
#include	<uinfo.h>
#include	<sncpyx.h>
#include	<sfx.h>
#include	<localmisc.h>


/* local defines */

#ifndef	VARPROVIDER
#define	VARPROVIDER	"PROVIDER"
#endif

#ifndef	PROVIDER
#define	PROVIDER	"Rightcore Network Services"
#endif


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int getprovider(char *rbuf,int rlen) noex {
	int		rs = SR_OK ;
	cchar		*vp ;
	rbuf[0] = '\0' ;
	if ((vp = getenv(VARPROVIDER)) != nullptr) { /* environment */
	    cchar	*cp ;
	    int		cl ;
	    if ((cl = sfshrink(vp,-1,&cp)) > 0) {
	        rs = sncpy1w(rbuf,rlen,cp,cl) ;
	    }
	}

	if ((rs >= 0) && (rbuf[0] == '\0')) { /* process cache */
	    UINFO_AUX	aux ;
	    if ((rs = uinfo_aux(&aux)) >= 0) {
		if (aux.provider != nullptr) {
	            rs = sncpy1(rbuf,rlen,aux.provider) ;
		}
	    }
	}

	if ((rs >= 0) && (rbuf[0] == '\0')) { /* otherwise */
	    rs = sncpy1(rbuf,rlen,PROVIDER) ;
	}

	return rs ;
}
/* end subroutine (getprovider) */


