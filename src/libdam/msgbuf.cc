/* msgbuf SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* message buffering */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-06-01, David A­D­ Morano
	This program was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	msgbuf

	Description:
	This object performs some simple message buffering.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<unistd.h>
#include	<fcntl.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<localmisc.h>

#include	"msgbuf.h"

import libutil ;

/* local defines */

#ifndef	TO_READ
#define	TO_READ		4
#endif

#define	NEOF		3


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int msgbuf_start(msgbuf *mbp,int fd,int bufsz,int to) noex {
	int		rs = SR_FAULT ;
	if (mbp) {
	    rs = SR_INVALID ;
	    memclear(mbp) ;
	    if (fd >= 0) {
		if ((rs = ucpagesize) >= 0) {
	            if (bufsz <= 0) bufsz = rs ;
	            if (to < 1) to = TO_READ ;
	            mbp->fd = fd ;
	            mbp->mlen = bufsz ;
	            mbp->to = to ;
	            if (char *bp ; (rs = uc_malloc(bufsz,&bp)) >= 0) {
	                mbp->mbuf = bp ;
	            }
		} /* end if (ucpagesize) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (msgbuf_start) */

int msgbuf_finish(msgbuf *mbp) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (mbp) {
	    rs = SR_OK ;
	    if (mbp->mbuf) {
	        rs1 = uc_free(mbp->mbuf) ;
	        if (rs >= 0) rs = rs1 ;
	        mbp->mbuf = nullptr ;
	    }
	    mbp->bp = nullptr ;
	    mbp->bl = 0 ;
	    mbp->fd = -1 ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (msgbuf_finish) */

int msgbuf_read(msgbuf *mbp,cchar **rpp) noex {
	int		rs = SR_FAULT ;
	int		len = 0 ;
	if (mbp && rpp) {
	    rs = SR_OK ;
	    if ((mbp->bl == 0) && (mbp->neof < NEOF)) {
	        int	ro = 0 ;
	        rs = uc_reade(mbp->fd,mbp->mbuf,mbp->mlen,mbp->to,ro) ;
	        mbp->bp = mbp->mbuf ;
	        mbp->bl = rs ;
	        mbp->neof = (rs == 0) ? (mbp->neof+1) : 0 ;
	    }
	    if (rs >= 0) {
	        *rpp = mbp->bp ;
	        len = mbp->bl ;
	    } else {
	        *rpp = nullptr ;
	    }
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (msgbuf_read) */

int msgbuf_adv(msgbuf *mbp,int mlen) noex {
	int		rs = SR_FAULT ;
	int		len = 0 ;
	if (mbp) {
	    int		rlen ;
	    int		opts = 0 ;
	    char	*rbuf ;
	    rs = SR_OK ;
	    if (mlen < 0) {
	        if ((mbp->bl > 0) && (mbp->bp != mbp->mbuf)) {
	            for (int i = 0 ; i < mbp->bl ; i += 1) {
	                mbp->mbuf[i] = *mbp->bp++ ;
		    }
	            mbp->bp = mbp->mbuf ;
	        }
	    } else if (mlen <= mbp->bl) {
	        mbp->bp += mlen ;
	        mbp->bl -= mlen ;
	        if (mbp->bl == 0) {
		    mbp->bp = mbp->mbuf ;
		}
	    } else if (mlen > mbp->bl) {
	        rs = SR_RANGE ;
	    }
	    if ((rs >= 0) && (mlen < 0)) {
	        rbuf = (mbp->mbuf + mbp->bl) ;
	        rlen = (mbp->mlen - mbp->bl) ;
	        if (rlen > 0) {
	            rs = uc_reade(mbp->fd,rbuf,rlen,mbp->to,opts) ;
	            len = rs ;
	        }
	        if (rs >= 0) {
	            mbp->neof = (rs == 0) ? (mbp->neof+1) : 0 ;
	            mbp->bl += len ;
	            len = mbp->bl ;
	        }
	    } /* end if */
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (msgbuf_adv) */

int msgbuf_update(msgbuf *mbp,int mlen) noex {
	return msgbuf_adv(mbp,mlen) ;
}
/* end subroutine (msgbuf_update) */


