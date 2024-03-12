/* dstr SUPPORT */
/* lang=C++20 */

/* string object */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This little object tries to mimic a dynamic-length string.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<cstdlib>
#include	<cstring>		/* for |strlen(3c)| */
#include	<usystem.h>
#include	<strwcpy.h>
#include	<localmisc.h>

#include	"dstr.h"


/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* exported subroutines */

int dstr_start(dstr *sop,cchar *s,int slen) noex {
	int		rs = SR_NOEXIST ;
	sop->sbuf = nullptr ;
	sop->slen = 0 ;
	if (s != nullptr) {
	    void	*vp{} ;
	    if (slen < 0) slen = strlen(s) ;
	    sop->slen = slen ;
	    if ((rs = uc_malloc(sop->slen,&vp)) >= 0) {
		sop->sbuf = charp(vp) ;
	        rs = sop->slen ;
	        strwcpy(sop->sbuf,s,sop->slen) ;
	    }
	} /* end if */
	return rs ;
}
/* end subroutine (dstr_start) */

int dstr_finish(dstr *sop) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (sop->sbuf != nullptr) {
	    rs1 = uc_free(sop->sbuf) ;
	    if (rs >= 0) rs = rs1 ;
	    sop->sbuf = nullptr ;
	}
	sop->slen = 0 ;
	return rs ;
}
/* end subroutine (dstr_finish) */

int dstr_assign(dstr *sop,dstr *sop2) noex {
	int		rs ;
	if ((rs = dstr_finish(sop)) >= 0) {
	    rs = dstr_start(sop,sop2->sbuf,sop2->slen) ;
	}
	return rs ;
}
/* end subroutine (dstr_assign) */


