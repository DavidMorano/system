/* fdliner SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* file-read line hanlding */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	fdliner

	Description:
	This object is a line-reading helper manager.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/stat.h>		/* POSIX® */
#include	<unistd.h>		/* POSIX® */
#include	<fcntl.h>		/* POSIX® */
#include	<climits>		/* CSTD |INT_MAX| */
#include	<cstdlib>		/* CSTD */
#include	<cstring>		/* CSTD */
#include	<algorithm>		/* C++STD |min(3c++)| + |max(3c++)| */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<intsat.h>		/* LIBU */
#include	<uclibmem.h>		/* LIBUC */
#include	<filer.h>		/* LIBUC */
#include	<char.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */

#include	"fdliner.h"


/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

local int	fdliner_starter(fdliner *,int) noex ;
local int	fdliner_bufsize(fdliner *,int) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int fdliner_start(fdliner *op,int mfd,off_t foff,int to) noex {
	int		rs = SR_FAULT ;
	if (op) ylikely {
	    rs = SR_BADF ;
	    if (mfd >= 0) ylikely {
		cint	osz = szof(filer) ;
	        op->poff = 0 ;
	        op->foff = foff ;
	        op->fbo = 0 ;
	        op->to = to ;
	        op->llen = 0 ;
	        op->lbuf = nullptr ;
		if (void *vp ; (rs = lm_mall(osz,&vp)) >= 0) ylikely {
		    op->fbp = (filer *) vp ;
		    if (char *lp ; (rs = lm_ml(&lp)) >= 0) ylikely {
			op->lbuf = lp ;
			op->llen = rs ;
			rs = fdliner_starter(op,mfd) ;
			if (rs < 0) {
			    lm_free(op->lbuf) ;
			    op->lbuf = nullptr ;
			    op->llen = 0 ;
			} /* end if (error) */
		    } /* end if (m-a) */
		    if (rs < 0) {
			lm_free(op->fbp) ;
			op->fbp = nullptr ;
		    } /* end if (error) */
		} /* end if (m-a) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (fdliner_start) */

int fdliner_finish(fdliner *op) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (op) ylikely {
	    rs = SR_OK ;
	    if (op->lbuf) {
	        op->lbuf[0] = '\0' ;
	        rs1 = lm_free(op->lbuf) ;
		if (rs >= 0) rs = rs1 ;
		op->lbuf = nullptr ;
	        op->llen = 0 ;
	    } /* end if (memory-release) */
	    if (op->fbp) {
		{
		    rs1 = filer_finish(op->fbp) ;
		    if (rs >= 0) rs = rs1 ;
		}
		{
	            rs1 = lm_free(op->fbp) ;
		    if (rs >= 0) rs = rs1 ;
		    op->fbp = nullptr ;
		} /* end if (memory-release) */
	    }
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (fdliner_finish) */

int fdliner_getln(fdliner *op,cchar **lpp) noex {
	int		rs = SR_FAULT ;
	int		len = 0 ;
	if (op && lpp) ylikely {
	    filer	*fbp = op->fbp ;
	    rs = SR_OK ;
	    if (op->llen < 0) {
		cint	to = op->to ;
	        cint	ll = op->llen ;
	        char	*lp = op->lbuf ;
	        op->poff = op->foff ;
	        if ((rs = filer_readln(fbp,lp,ll,to)) > 0) {
	            len = rs ;
	            op->foff += len ;
	        }
	    } /* end if (needed a new line) */
	    if (lpp) {
	        *lpp = (rs >= 0) ? op->lbuf : nullptr ;
	    }
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
} /* end subroutine (fdliner_getln) */

int fdliner_done(fdliner *op) noex {
	int		rs = SR_FAULT ;
	if (op) ylikely {
	    rs = SR_OK ;
	    op->llen = 0 ;
	    op->lbuf[0] = '\0' ;
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (fdliner_done) */

int fdliner_adv(fdliner *lsp,int inc) noex {
    	int		rs = SR_FAULT ;
	if (lsp) ylikely {
	    rs = SR_OK ;
	    lsp->poff = lsp->foff ;
	    if (inc > 0) {
	        lsp->llen = -1 ;
	        lsp->lbuf[0] = '\0' ;
	        lsp->poff += inc ;
	        lsp->foff += inc ;
	        rs = filer_adv(lsp->fbp,inc) ;
	    } /* end if */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (fdliner_adv) */

/* local subroutines */

local int fdliner_starter(fdliner *op,int mfd) noex {
	int		rs ;
	if ((rs = fdliner_bufsize(op,mfd)) >= 0) ylikely {
	    coff	foff = op->foff ;
	    cint	bs = rs ;
	    cint	fbo = op->fbo ;
	    rs = filer_start(op->fbp,mfd,foff,bs,fbo) ;
	}
	return rs ;
} /* end subroutine (fdliner_starter) */

local int fdliner_bufsize(fdliner *op,int mfd) noex {
	int		rs ;
	int		bs = 0 ;
	if (ustat sb ; (rs = u_fstat(mfd,&sb)) >= 0) ylikely {
	    csize	im(INT_MAX) ;
	    csize	fsize = size_t(sb.st_size) ;
	    rs = SR_TOOBIG ;
	    if (fsize <= im) {
		cint	fs = intsat(fsize) ;
	        cmode	m = sb.st_mode ;
	        bs = FDLINER_BUFSIZEDEF ;
	        op->fl.fnet = S_ISCHR(m) || S_ISSOCK(m) || S_ISFIFO(m) ;
	        if (op->fl.fnet) {
	            op->fbo |= FILER_ONET ;
	        } else {
	            bs = BCEIL(fs,FDLINER_BUFSIZEBLOCK) ;
	        }
	    } /* end if (can-handle) */
	} /* end if (uc_fstat) */
	return (rs >= 0) ? bs : rs ;
} /* end subroutine (fdliner_bufsize) */

int fdliner::start(int mfd,off_t off,int ato) noex {
	return fdliner_start(this,mfd,off,ato) ;
}

int fdliner::getln(cchar **lpp) noex {
	return fdliner_getln(this,lpp) ;
}

void fdliner::dtor() noex {
	if (cint rs = finish ; rs < 0) {
	    ulogerror("fdliner",rs,"fini-finish") ;
	}
} /* end method (fdliner::dtor) */

int fdliner_co::operator () (int a) noex {
	int		rs = SR_BUGCHECK ;
	if (op) ylikely {
	    switch (w) {
	    case fdlinermem_done:
	        rs = fdliner_done(op) ;
	        break ;
	    case fdlinermem_adv:
	        rs = fdliner_adv(op,a) ;
	        break ;
	    case fdlinermem_finish:
	        rs = fdliner_finish(op) ;
	        break ;
	    } /* end switch */
	} /* end if (non-null) */
	return rs ;
} /* end method (fdliner_co::operator) */


