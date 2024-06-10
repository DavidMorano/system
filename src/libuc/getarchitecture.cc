/* getarchitecture SUPPORT */
/* version %I% last-modified %G% */

/* get the machine architecture string */
/* version %I% last-modified %G% */


/* revision history:

	= 2001-04-11, David A­D­ Morano
	This was originally written.

*/

/* Copyright © 2001 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	getarcitecture

	Description:
	Get and return the machine architecture string.

	Synopsis:
	int getarchitecture(char *rbuf,int rlen) noex

	Arguments:
	rbuf		result buffer
	rlen		result buffer length

	Returns:
	>=0		number of characters returned
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<usystem.h>
#include	<uvariables.hh>
#include	<ucsysauxinfo.h>	/* |usysauxinforeq_architecture| */
#include	<sfx.h>
#include	<snwcpy.h>
#include	<localmisc.h>


/* local defines */

#ifndef	ARCHITECTURE
#define	ARCHITECTURE		"Armv9"
#endif


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int getarchitecture(char *rbuf,int rlen) noex {
	int		rs = SR_FAULT ;
	if (rbuf) {
	    static cchar	*vp = getenv(varname.architecture) ;
	    rs = SR_OK ;
	    rbuf[0] = '\0' ;
	    /* try first */
	    if (vp) {
	        cchar	*cp ;
	        if (int cl ; (cl = sfshrink(vp,-1,&cp)) > 0) {
	            rs = snwcpy(rbuf,rlen,cp,cl) ;
	        }
	    }
	    /* try second */
	    if ((rs >= 0) && (rbuf[0] == '\0')) {
	        cint	cmd = usysauxinforeq_architecture ;
	        rs = uc_sysauxinfo(rbuf,rlen,cmd) ;
	    }
	    /* try third */
	    if ((rs >= 0) && (rbuf[0] == '\0')) {
	        rs = snwcpy(rbuf,rlen,ARCHITECTURE) ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (getarchitecture) */


