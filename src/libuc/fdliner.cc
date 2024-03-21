/* fdliner SUPPORT */
/* lang=C++20 */

/* file-read line hanlding */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-01, David A­D­ Morano
	This subroutine was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This object module processes one or more mail messages (in
	appropriate mailbox format if more than one) from a file
	descriptor passed in the 'init' call-method.  The input
	file descriptor can be a pipe as a standard mailbox file
	is not required (one of the whole reasons for this object
	as opposed to the MAILBOX object).

	Any mail messages found are effectively partially parsed
	and the object is then ready for queries by additional
	method calls.

	The parsed mail meta-data is stored in a manner such that
	it is optimized for repeated access.

	Use should proceed roughly as:

		if ((rs = mailmsgstage_start()) >= 0) {

		    mailmsgstage_xxxx() 

		    mailmegstage_finish()
		} end if (mailmsgstage)

	This object is pretty fast also!

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/stat.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<climits>		/* |INT_MAX| */
#include	<cstdlib>
#include	<cstring>
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<usystem.h>
#include	<mallocxx.h>
#include	<filebuf.h>
#include	<char.h>
#include	<localmisc.h>

#include	"fdliner.h"


/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

static int	fdliner_starter(fdliner *,int) noex ;
static int	fdliner_bufsize(fdliner *,int) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int fdliner_start(fdliner *op,int mfd,off_t foff,int to) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_BADF ;
	    if (mfd >= 0) {
		cint	osz = sizeof(filebuf) ;
		void	*vp{} ;
	        op->poff = 0 ;
	        op->foff = foff ;
	        op->fbo = 0 ;
	        op->to = to ;
	        op->llen = 0 ;
	        op->lbuf = nullptr ;
		if ((rs = uc_malloc(osz,&vp)) >= 0) {
		    char	*lp{} ;
		    op->fbp = (filebuf *) vp ;
		    if ((rs = malloc_ml(&lp)) >= 0) {
			op->lbuf = lp ;
			op->llen = rs ;
			rs = fdliner_starter(op,mfd) ;
			if (rs < 0) {
			    uc_free(op->lbuf) ;
			    op->lbuf = nullptr ;
			    op->llen = 0 ;
			}
		    } /* end if (m-a) */
		    if (rs < 0) {
			uc_free(op->fbp) ;
			op->fbp = nullptr ;
		    }
		} /* end if (m-a) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (fdliner_start) */

int fdliner_finish(fdliner *op) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (op) {
	    rs = SR_OK ;
	    if (op->lbuf) {
	        op->lbuf[0] = '\0' ;
	        rs1 = uc_free(op->lbuf) ;
		if (rs >= 0) rs = rs1 ;
		op->lbuf = nullptr ;
	        op->llen = 0 ;
	    }
	    if (op->fbp) {
		{
		    rs1 = filebuf_finish(op->fbp) ;
		    if (rs >= 0) rs = rs1 ;
		}
		{
	            rs1 = uc_free(op->fbp) ;
		    if (rs >= 0) rs = rs1 ;
		    op->fbp = nullptr ;
		}
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (fdliner_finish) */

int fdliner_read(fdliner *op,cchar **lpp) noex {
	int		rs = SR_FAULT ;
	int		len = 0 ;
	if (op && lpp) {
	    filebuf	*fbp = op->fbp ;
	    rs = SR_OK ;
	    if (op->llen < 0) {
		cint	to = op->to ;
	        cint	ll = op->llen ;
	        char	*lp = op->lbuf ;
	        op->poff = op->foff ;
	        if ((rs = filebuf_readln(fbp,lp,ll,to)) > 0) {
	            len = rs ;
	            op->foff += len ;
	        }
	    } /* end if (needed a new line) */
	    if (lpp) {
	        *lpp = (rs >= 0) ? op->lbuf : NULL ;
	    }
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (fdliner_read) */

int fdliner_done(fdliner *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	    op->llen = 0 ;
	    op->lbuf[0] = '\0' ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (fdliner_done) */


/* local subroutines */

static int fdliner_starter(fdliner *op,int mfd) noex {
	int		rs ;
	if ((rs = fdliner_bufsize(op,mfd)) >= 0) {
	    const off_t	foff = op->foff ;
	    cint	bs = rs ;
	    cint	fbo = op->fbo ;
	    rs = filebuf_start(op->fbp,mfd,foff,bs,fbo) ;
	}
	return rs ;
}
/* end subroutine (fdliner_starter) */

static int fdliner_bufsize(fdliner *op,int mfd) noex {
	USTAT		sb ;
	int		rs ;
	int		bs = 0 ;
	if ((rs = u_fstat(mfd,&sb)) >= 0) {
	    rs = SR_TOOBIG ;
	    if (sb.st_size <= INT_MAX) {
		cint	fs = int(sb.st_size) ;
	        cmode	m = sb.st_mode ;
	        bs = FDLINER_BUFSIZEDEF ;
	        op->f.fnet = S_ISCHR(m) || S_ISSOCK(m) || S_ISFIFO(m) ;
	        if (op->f.fnet) {
	            op->fbo |= FILEBUF_ONET ;
	        } else {
	            bs = BCEIL(fs,FDLINER_BUFSIZEBLOCK) ;
	        }
	    } /* end if (can-handle) */
	} /* end if (uc_fstat) */
	return (rs >= 0) ? bs : rs ;
}
/* end subroutine (fdliner_bufsize) */


