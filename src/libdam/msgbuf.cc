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
#include	<unistd.h>		/* POSIX® */
#include	<fcntl.h>		/* POSIX® */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstring>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<ucmem.h>		/* LIBUC */
#include	<ucsysmisc.h>		/* LIBUC |ucpagesize(3uc)| */
#include	<ucopen.h>		/* LIBUC */
#include	<ucdesc.h>		/* LIBUC */
#include	<ucfileop.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */

#include	"msgbuf.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| */

/* local defines */

#ifndef	TO_READ
#define	TO_READ		4
#endif

#define	NEOF		3


/* imported namespaces */

using libuc::mem ;			/* variable */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int msgbuf_start(msgbuf *mbp,int fd,int bufsz,int to) noex {
	int		rs = SR_FAULT ;
	if (mbp) ylikely {
	    rs = SR_INVALID ;
	    memclear(mbp) ;
	    if (fd >= 0) ylikely {
		if ((rs = ucpagesize) >= 0) ylikely {
	            if (bufsz <= 0) bufsz = rs ;
	            if (to < 1) to = TO_READ ;
	            mbp->fd = fd ;
	            mbp->mlen = bufsz ;
	            mbp->to = to ;
	            if (char *bp ; (rs = mem.mall(bufsz,&bp)) >= 0) ylikely {
	                mbp->mbuf = bp ;
	            }
		} /* end if (ucpagesize) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (msgbuf_start) */

int msgbuf_finish(msgbuf *mbp) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (mbp) ylikely {
	    rs = SR_OK ;
	    if (mbp->mbuf) {
	        rs1 = mem.free(mbp->mbuf) ;
	        if (rs >= 0) rs = rs1 ;
	        mbp->mbuf = nullptr ;
	    } /* end if (memory-release) */
	    mbp->bp = nullptr ;
	    mbp->bl = 0 ;
	    mbp->fd = -1 ;
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (msgbuf_finish) */

int msgbuf_read(msgbuf *mbp,cchar **rpp) noex {
	int		rs = SR_FAULT ;
	int		len = 0 ;
	if (mbp && rpp) ylikely {
	    rs = SR_OK ;
	    if ((mbp->bl == 0) && (mbp->neof < NEOF)) {
	        int	ro = 0 ;
	        rs = uc_reade(mbp->fd,mbp->mbuf,mbp->mlen,mbp->to,ro) ;
	        mbp->bp = mbp->mbuf ;
	        mbp->bl = rs ;
	        mbp->neof = (rs == 0) ? (mbp->neof+1) : 0 ;
	    } /* end if */
	    if (rs >= 0) {
	        *rpp = mbp->bp ;
	        len = mbp->bl ;
	    } else {
	        *rpp = nullptr ;
	    }
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
} /* end subroutine (msgbuf_read) */

int msgbuf_adv(msgbuf *mbp,int mlen) noex {
	int		rs = SR_FAULT ;
	int		len = 0 ;
	if (mbp) ylikely {
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
	        if (rs >= 0) ylikely {
	            mbp->neof = (rs == 0) ? (mbp->neof+1) : 0 ;
	            mbp->bl += len ;
	            len = mbp->bl ;
	        } /* end if (ok) */
	    } /* end if */
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
} /* end subroutine (msgbuf_adv) */

int msgbuf_update(msgbuf *mbp,int mlen) noex {
	return msgbuf_adv(mbp,mlen) ;
} /* end subroutine (msgbuf_update) */


