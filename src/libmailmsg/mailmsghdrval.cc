/* mailmsghdrval SUPPORT */
/* lang=C++20 */

/* message header-value handling */
/* version %I% last-modified %G% */


/* revision history:

	= 2002-01-10, David A�D� Morano
	This was written to support the MAILBOX object.

*/

/* Copyright � 2002 David A�D� Morano.  All rights reserved. */

/*******************************************************************************

	This is a *simple* message header-value object.  

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/param.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<mallocxx.h>
#include	<sfx.h>
#include	<strwcpy.h>
#include	<localmisc.h>		/* |REALNAMELEN| */

#include	"mailmsghdrval.h"


/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

static int mailmsghdrval_loadadd(MAILMSGHDRVAL *,cchar *,int) noex ;


/* local variables */

constexpr int		extlen = REALNAMELEN ;


/* exported variables */


/* exported subroutines */

int mailmsghdrval_start(mailmsghdrval *op,int i,cchar *hp,int hl) noex {
	int		rs = SR_FAULT ;
	if (op && hp) {
	    char	*cp{} ;
	    op->idx = i ;
	    op->vl = 0 ;
	    if ((rs = malloc_ml(&cp)) >= 0) {
		op->vlen = rs ;
	        op->vbuf = cp ;
		op->vbuf[0] = '\0' ;
	        rs = mailmsghdrval_loadadd(op,hp,hl) ;
		if (rs < 0) {
		    uc_free(cp) ;
		    op->vbuf = nullptr ;
		    op->vlen = 0 ;
		}
	    } /* end if (m-a) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (mailmsghdrval_start) */

int mailmsghdrval_finish(mailmsghdrval *op) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (op) {
	    rs = SR_OK ;
	    op->vbuf[0] = '\0' ;
	    if (op->vbuf) {
	        rs1 = uc_free(op->vbuf) ;
	        if (rs >= 0) rs = rs1 ;
	        op->vbuf = nullptr ;
	    }
	    op->vlen = 0 ;
	    op->vl = 0 ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (mailmsghdrval_finish) */

int mailmsghdrval_add(mailmsghdrval *op,cchar *hp,int hl) noex {
	int		rs = SR_FAULT ;
	if (op && hp) {
	    rs = mailmsghdrval_loadadd(op,hp,hl) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (mailmsghdrval_add) */

int mailmsghdrval_get(mailmsghdrval *op,cchar **vpp,int *vlp) noex {
	int		rs = SR_FAULT ;
	int		idx = 0 ;
	if (op) {
	    if (vpp) *vpp = op->vbuf ;
	    if (vlp) *vlp = op->vl ;
	    idx = op->idx ;
	} /* end if (non-null) */
	return (rs >= 0) ? idx : rs ;
}
/* end subroutine (mailmsghdrval_get) */

int mailmsghdrval_clr(mailmsghdrval *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    op->vbuf[0] = '\0' ;
	    op->vl = 0 ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (mailmsghdrval_clr) */


/* private subroutines */

static int mailmsghdrval_loadadd(mailmsghdrval *op,cchar *hp,int hl) noex {
	int		rs = SR_OK ;
	int		hlen = 0 ;
	cchar		*sp ;
	if (int sl ; (sl = sfshrink(hp,hl,&sp)) > 0) {
	    cint	remlen = (op->vlen - op->vl) ;
	    cint	reqlen = (sl + 1) ;
	    if (reqlen > remlen) {
		cint	sz = (op->vlen + reqlen + extlen) ;
	        char	*nvp ;
	        if ((rs = uc_realloc(op->vbuf,(sz+1),&nvp)) >= 0) {
		    op->vbuf = nvp ;
		    op->vlen = sz ;
		}
	    }  /* end if (buffer extension) */
	    if (rs >= 0) {
		char	*vp = (op->vbuf + op->vl) ;
	        *vp++ = ' ' ;
	        strwcpy(vp,sp,sl) ;
	        op->vl += sl ;
		hlen = op->vl ;
	    } /* end if */
	} /* end if (sfshrink) */
	return (rs >= 0) ? hlen : rs ;
}
/* end subroutine (mailmsghdrval_loadadd) */


