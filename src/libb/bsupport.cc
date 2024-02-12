/* bsupport SUPPORT */
/* lang=C++20 */

/* "Basic I/O" package (BFILE) */
/* last modifed %G% version %I% */


/* revision history:

	= 1998-07-01, David A­D­ Morano
	This subroutine was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Internal (BFILE) flush support subroutines.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<unistd.h>
#include	<fcntl.h>
#include	<usystem.h>
#include	<localmisc.h>

#include	"bfile.h"


/* local defines */


/* external subroutines */


/* external variables */


/* exported subroutines */

int bfile_flushn(bfile *fp,int n) noex {
	int		rs = SR_OK ;
	int		len = 0 ;
	if (n != 0) {
	    int	f_sa = (! fp->f.notseek) && (fp->oflags & O_APPEND) ;
	    if (f_sa) {
	        off_t	o{} ;
	        rs = u_seeko(fp->fd,0LL,SEEK_END,&o) ;
	        fp->offset = o ;
	    } /* end if (sa) */
	    if ((rs >= 0) && (fp->len > 0)) {
	        int	mlen = (fp->bp - fp->bbp) ;
	        if ((n > 0) && (n < mlen)) mlen = n ;
	        if ((rs = uc_writen(fp->fd,fp->bbp,mlen)) >= 0) {
	           len = rs ;
	            fp->bbp += len ;
	            fp->len -= len ;
	            if (fp->len == 0) {
		        fp->bbp = fp->bdata ;
		        fp->bp = fp->bdata ;
	            }
	            if (f_sa) {
	                fp->offset += len ;
		    }
	        }
	    } /* end if (flush needed) */
	} /* end if (not-zero) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (bfile_flushn) */

int bfile_flush(bfile *fp) noex {
	return bfile_flushn(fp,-1) ;
}
/* end subroutine (bfile_flush) */

int bfile_pagein(bfile *fp,off_t off,int i) noex {
	int		rs = SR_FAULT ;
	if (fp) {
	    size_t	ms = fp->pagesize ;
	    cint	mp = PROT_READ ;
	    cint	mf = MAP_SHARED ;
	    void	*vp ;
	    off &= (fp->pagesize - 1) ;
	    if ((rs = u_mmap(NULL,ms,mp,mf,fp->fd,off,&vp)) >= 0) {
	        fp->maps[i].buf = (char *) vp ;
	        fp->maps[i].f.valid = true ;
	        fp->maps[i].offset = off ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (bfile_pagein) */


