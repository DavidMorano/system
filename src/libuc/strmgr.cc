/* strmgr SUPPORT */
/* lang=C++20 */

/* string management */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-08-10 David A.D. Morano
	This was written from scratch.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	We perform some simple string management.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstring>
#include	<usystem.h>
#include	<localmisc.h>

#include	"strmgr.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported subroutines */

int strmgr_start(strmgr *op,char *dbuf,int dlen) noex {
	int		rs = SR_FAULT ;
	if (op && dbuf) {
	    rs = SR_OK ;
	    op->dp = dbuf ;
	    op->dlen = dlen ;
	    op->dl = 0 ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (strmgr_start) */

int strmgr_avail(strmgr *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = (op->dlen - op->dl) ;
	}
	return rs ;
}
/* end subroutine (strmgr_avail) */

int strmgr_rem(strmgr *op) noex {
	return strmgr_avail(op) ;
}
/* end subroutine (strmgr_rem) */

int strmgr_str(strmgr *op,cchar *sp,int sl) noex {
	int		rs = SR_FAULT ;
	int		tl = 0 ;
	if (op && sp) {
	    cint	rlen = (op->dlen-op->dl) ;
	    rs = SR_OK ;
	    while ((rs >= 0) && sl-- && *sp) {
	        if (tl < rlen) {
	            op->dp[tl++] = *sp++ ;
	        } else {
	            rs = SR_OVERFLOW ;
	        }
	    } /* end for */
	    if (rs >= 0) {
	        op->dp += tl ;
	        op->dl += tl ;
	    }
	} /* end if (non-null) */
	return (rs >= 0) ? tl : rs ;
}
/* end subroutine (strmgr_str) */

int strmgr_char(strmgr *op,int ch) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	    if (op->dl < op->dlen) {
	        *op->dp++ = ch ;
	        op->dl += 1 ;
	    } else {
	        rs = SR_OVERFLOW ;
	    }
	} /* end if (non-null) */
	return (rs >= 0) ? 1 : rs ;
}
/* end subroutine (strmgr_char) */

int strmgr_finish(strmgr *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = op->dl ;
	    op->dp[0] = '\0' ;
	    op->dp = nullptr ;
	    op->dl = 0 ;
	    op->dlen = 0 ;
	}
	return rs ;
}
/* end subroutine (strmgr_finish) */


