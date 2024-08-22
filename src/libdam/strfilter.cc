/* strfilter SUPPORT */
/* lang=C++20 */

/* filter a string of text against some criteria */
/* version %I% last-modified %G% */


/* revision history:

	= 2009-04-10, David A­D­ Morano
	This subroutine was originally written.

*/

/* Copyright © 2009 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	strfilter

	Description:
	This object module filters a line of text against one or
	both of a "select" list and an "exclude" list.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<vecstr.h>
#include	<ascii.h>
#include	<sfx.h>
#include	<six.h>
#include	<localmisc.h>

#include	"strfilter.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int strfilter_start(strfilter *op,cchar *ssfname,cchar *sxfname) noex {
	int		rs = SR_FAULT ;
	if (op ) {
	    cint	vn = 10 ;
	    cint	vo = 0 ;
	    rs = memclear(op) ;
	    if ((ssfname != nullptr) && (ssfname[0] != '\0')) {
	        op->f.sslist = true ;
	        if ((rs = vecstr_start(&op->sslist,vn,vo)) >= 0) {
	            rs = vecstr_loadfile(&op->sslist,true,ssfname) ;
	            if (rs < 0) {
	                vecstr_finish(&op->sslist) ;
		    }
	        }
	    }
	    if (rs >= 0) {
	        if ((sxfname != nullptr) && (sxfname[0] != '\0')) {
	            op->f.sxlist = true ;
	            if ((rs = vecstr_start(&op->sxlist,vn,vo)) >= 0) {
	                rs = vecstr_loadfile(&op->sxlist,true,sxfname) ;
	                if (rs < 0) {
	                    vecstr_finish(&op->sxlist) ;
		        }
	            }
	        }
	        if (rs < 0) {
	            vecstr_finish(&op->sslist) ;
	        }
	    } /* end if (ok) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (strfilter_start) */

int strfilter_finish(strfilter *op) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (op) {
	    rs = SR_OK ;
	    if (op->f.sxlist) {
	        op->f.sxlist = false ;
	        rs1 = vecstr_finish(&op->sxlist) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    if (op->f.sslist) {
	        op->f.sslist = false ;
	        rs1 = vecstr_finish(&op->sslist) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (strfilter_finish) */

int strfilter_check(strfilter *op,cchar *lbuf,int llen) noex {
	int		rs = SR_FAULT ;
	int		f = true ;
	if (op && lbuf) {
	    int		si ;
	    cchar	*cp ;
	    rs = SR_OK ;
	    if (op->f.sslist) {
		vecstr	*sslp = &op->sslist ;
	        f = false ;
	        for (int i = 0 ; vecstr_get(sslp,i,&cp) >= 0 ; i += 1) {
	            if (cp) {
	                if ((si = sisub(lbuf,llen,cp)) >= 0) {
	                    f = true ;
	                    break ;
	                }
		    }
	        }
	    } /* end if */
	    if (f && op->f.sxlist) {
		vecstr	*sxlp = &op->sslist ;
	        for (int i = 0 ; vecstr_get(sxlp,i,&cp) >= 0 ; i += 1) {
	            if (cp) {
	                if ((si = sisub(lbuf,llen,cp)) >= 0) {
	                    f = false ;
	                    break ;
	                }
		    }
	        }
	    } /* end if */
	} /* end if (non-null) */
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (strfilter_check) */


