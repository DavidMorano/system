/* bsupport SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* "Basic I-O" package (BFILE) */
/* last modifed %G% version %I% */


/* revision history:

	= 1998-07-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	bsupport

	Description:
	Internal (BFILE) flush support subroutines.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<unistd.h>
#include	<fcntl.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<usystem.h>
#include	<intfloor.h>
#include	<localmisc.h>

#include	"bfile.h"


/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* exported subroutines */

int bfile_bufreset(bfile *op) noex {
	op->bp = op->bdata ;
	op->bbp = op->bdata ;
	op->len = 0 ;
	return SR_OK ;
}

int bfile_acc(bfile *op,bool fwr) noex {
	int		rs ;
	if (fwr) {
	    rs = bfile_ckrd(op) ;
	} else {
	    rs = bfile_ckwr(op) ;
	}
	return rs ;
}
/* end subroutine (bfile_acc) */

int bfile_ckrd(bfile *op) noex {	
	int		rs = SR_BADF ;
	if (op->fl.rd) {
	    rs = SR_OK ;
	    if (op->fl.writing) {
		rs = bfile_flush(op) ;
		op->fl.writing = false ;
	    }
	} /* end if (access allowed) */
	return rs ;
}
/* end subroutine (bfile_ckrd) */

int bfile_ckwr(bfile *op) noex {	
	int		rs = SR_BADF ;
	if (op->fl.wr) {
	    rs = SR_OK ;
	    if (! op->fl.writing) {
		if ((rs = bfile_bufreset(op)) >= 0) {
	            if ((! op->fl.notseek) && (! op->fl.append)) {
	                rs = u_seek(op->fd,op->offset,SEEK_SET) ;
	            }
		}
		op->fl.writing = true ;
	    } /* end if (not previously writing */
	} /* end if (access allowed) */
	return rs ;
}
/* end subroutine (bfile_ckwr) */

int bfile_flushn(bfile *op,int n) noex {
	int		rs = SR_OK ;
	int		len = 0 ;
	if (n != 0) {
	    bool	f_sa = (! op->fl.notseek) && (op->of & O_APPEND) ;
	    if (f_sa) {
	        off_t	o{} ;
	        rs = u_seeko(op->fd,0LL,SEEK_END,&o) ;
	        op->offset = o ;
	    } /* end if (sa) */
	    if ((rs >= 0) && (op->len > 0)) {
	        int	mlen = intconv(op->bp - op->bbp) ;
	        if ((n > 0) && (n < mlen)) mlen = n ;
	        if ((rs = uc_writen(op->fd,op->bbp,mlen)) >= 0) {
	            len = rs ;
	            op->bbp += len ;
	            op->len -= len ;
	            if (op->len == 0) {
		        op->bbp = op->bdata ;
		        op->bp = op->bdata ;
	            }
	            if (f_sa) {
	                op->offset += len ;
		    }
	        }
	    } /* end if (flush needed) */
	} /* end if (not-zero) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (bfile_flushn) */

int bfile_flush(bfile *op) noex {
	return bfile_flushn(op,-1) ;
}
/* end subroutine (bfile_flush) */

int bfile_pagein(bfile *op,off_t off,int i) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    cnullptr	np{} ;
	    csize	ms = size_t(op->pagesize) ;
	    coff	of = floor(off,op->pagesize) ;
	    cint	mp = PROT_READ ;
	    cint	mf = MAP_SHARED ;
	    cint	fd = op->fd ;
	    void	*vp ;
	    if ((rs = u_mmapbegin(np,ms,mp,mf,fd,of,&vp)) >= 0) {
	        op->maps[i].bdata = charp(vp) ;
	        op->maps[i].bsize = ms ;
	        op->maps[i].offset = of ;
	        op->maps[i].fl.valid = true ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (bfile_pagein) */


