/* mailmsghdrval SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* message header-value handling */
/* version %I% last-modified %G% */


/* revision history:

	= 2002-01-10, David A­D­ Morano
	This was written to support the MAILBOX object.

*/

/* Copyright © 2002 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	mailmsghdrval

	Description:
	This is a (rather) simple message header-value object.  The
	purpose of this object is to accummulate pieces of an entire
	header-value, one piece of it at a time.  The reason that
	header-values have to be accummulated is due to the fact
	that they may be on different lines from each other.  Further,
	under circumstances where desired, different header values
	may be combinæd from entirely different headers themsleves,
	but sharing the same header key-name.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<usystem.h>
#include	<bufsizevar.hh>
#include	<sfx.h>
#include	<strwcpy.h>
#include	<localmisc.h>		/* |REALNAMELEN| */

#include	"mailmsghdrval.h"


/* local defines */

#define	MMHV	mailmsghdrval


/* imported namespaces */

using libuc::mem ;			/* variable */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

static int mailmsghdrval_loadadd(mailmsghdrval *,cchar *,int) noex ;


/* local variables */

static bufsizevar	maxlinelen(getbufsize_ml) ;

cint			extlen = REALNAMELEN ;


/* exported variables */


/* exported subroutines */

int mailmsghdrval_start(mailmsghdrval *op,int i,cchar *hp,int hl) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    if ((rs = maxlinelen) >= 0) {
	        cint	sz = (rs + 1) ;
	        op->idx = i ;
	        op->vl = 0 ;
	        if (char *bp ; (rs = mem.mall(sz,&bp)) >= 0) {
		    op->vlen = rs ;
	            op->vbuf = bp ;
		    op->vbuf[0] = '\0' ;
		    if (hp) {
	                rs = mailmsghdrval_loadadd(op,hp,hl) ;
		    }
		    if (rs < 0) {
		        mem.free(op->vbuf) ;
		        op->vbuf = nullptr ;
		        op->vlen = 0 ;
		    } /* end if (error) */
	        } /* end if (m-a) */
	    } /* end if (maxlinelen) */
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
	        rs1 = mem.free(op->vbuf) ;
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
	    rs = SR_OK ;
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
	        if ((rs = mem.rall(op->vbuf,(sz+1),&nvp)) >= 0) {
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


