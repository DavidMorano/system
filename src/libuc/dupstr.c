/* dupstr SUPPORT */
/* lang=C++20 */

/* create a (writable) duplicate of a string */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-01, David A­D­ Morano
	This subroutine was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This object module (DUPSTR) creates a writable duplicate
	of the given string.  Note: for a simple non-writable
	(constant) copy of a string, check out the NULSTR object.

	Object:
	dupstr

	Methods:
	int dupstr_start(DUPSTR *ssp,cchar *sp,int sl,char **rpp)
	int dupstr_finish(DUPSTR *ssp)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstring>		/* <- for |strlen(3c)| */
#include	<usystem.h>
#include	<strwcpy.h>
#include	<localmisc.h>

#include	"dupstr.h"


/* local defines */


/* external subroutines */


/* exported subroutines */

int dupstr_start(dupstr *ssp,cchar *sp,int sl,char **rpp) noex {
	int		rs = SR_FAULT ;
	int		cl = 0 ;
	if (ssp && sp && rpp) {
	    rs = SR_OK ;
	    ssp->as = nullptr ;
	    if (sl < 0) sl = strlen(sp) ;
	    if (sl > DUPSTR_SHORTLEN) {
	        cint	ssize = (sl+1) ;
	        char	*bp ;
	        if ((rs = uc_malloc(ssize,&bp)) >= 0) {
	            cl = strwcpy(bp,sp,sl) - bp ;
	            *rpp = bp ;
	            ssp->as = bp ;
	        } /* end if (m-a) */
	    } else {
	        *rpp = ssp->buf ;
	        cl = strwcpy(ssp->buf,sp,sl) - ssp->buf ;
	    } /* end if */
	} /* end if (non-null) */
	return (rs >= 0) ? cl : rs ;
}
/* end subroutine (dupstr_start) */

int dupstr_finish(dupstr *ssp) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (ssp) {
	    rs = SR_OK ;
	    if (ssp->as) {
	        rs1 = uc_free(ssp->as) ;
	        if (rs >= 0) rs = rs1 ;
	        ssp->as = nullptr ;
	    }
	    ssp->buf[0] = '\0' ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (dupstr_finish) */


