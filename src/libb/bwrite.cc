/* bwrite SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* routine to write bytes */
/* version %I% last-modified %G% */

#define	CF_CHUNKCPY	0		/* try chunk copy */
#define	CF_FLUSHPART	1		/* do partial flushes */

/* revision history:

	= 1998-07-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Unlike the standard I-O library, all writes of this library
	are atomic in that the entire portion of each write request
	is actually written to the file as a whole. Each write block
	is either written to the file as a single block or in
	conjunction with previous write requests, but in no way
	will any single write request be broken up and written
	separately to the file.

	Note that this library can also freely intermix reads and
	writes to a file with the data ending up where it should
	without getting scrambled as in the standard library.

	Both of the above features, as well as some other features
	unique to this library, would normally make this package
	slower than the standard I-O library, but this package is
	normally faster than most versions of the standard package
	and probably close in performance with some of the latest
	implemtations which use some of the buffer management
	strategies used here.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/param.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<algorithm>
#include	<usystem.h>
#include	<strn.h>
#include	<localmisc.h>

#include	"bfile.h"

import libutil ;

/* local defines */

#define	MEMCPYLEN	100		/* minimum length for 'memcpy(3c)' */

#ifndef	CF_CHUNKCPY
#define	CF_CHUNKCPY	0		/* try chunk copy */
#endif
#ifndef	CF_FLUSHPART
#define	CF_FLUSHPART	1		/* do partial flushes */
#endif


/* imported namespaces */

using std::min ;			/* subroutine-template */
using std::max ;			/* subroutine-template */


/* external subroutines */

extern "C" {
    int		bfile_write(bfile *,cvoid *,int) noex ;
}


/* external variables */


/* local structures */


/* forward references */

static int	bfile_wbig(bfile *,cvoid *,int) noex ;
static int	bfile_wreg(bfile *,cvoid *,int) noex ;
static int	bfile_bufcpy(bfile *,cchar *,int) noex ;


/* local variables */

constexpr bool		f_chunkcpy = CF_CHUNKCPY ;
constexpr bool		f_flushpart = CF_FLUSHPART ;


/* exported variables */


/* exported subroutines */

int bwrite(bfile *op,cvoid *abuf,int alen) noex {
	int		rs ;
	if ((rs = bfile_magic(op,abuf)) > 0) {
	    if ((rs = bfile_ckwr(op)) >= 0) {
		rs = bfile_write(op,abuf,alen) ;
	    } /* end if (access) */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (bwrite) */

int bfile_write(bfile *op,cvoid *abuf,int alen) noex {
	int		rs ;
	cchar		*abp = charp(abuf) ;
	if (alen < 0) alen = lenstr(abp) ;
	if (op->bm == bfilebm_none) {
	    rs = bfile_wbig(op,abuf,alen) ;
	} else {
	    rs = bfile_wreg(op,abuf,alen) ;
	}
	return rs ;
}
/* end subroutine (bfile_write) */


/* local subroutines */

static int bfile_wbig(bfile *op,cvoid *abuf,int alen) noex {
	int		rs ;
	if ((rs = bfile_flush(op)) >= 0) {
	    int		abl = alen ;
	    cchar	*abp = charp(abuf) ;
	    while ((rs >= 0) && (abl > 0)) {
	        if ((rs = uc_writen(op->fd,abp,abl)) >= 0) {
	            cint	len = rs ;
	            op->offset += len ;
	            abp += len ;
	            abl -= len ;
	        }
	    } /* end while */
	} /* end if (flush) */
	return (rs >= 0) ? alen : rs ;
}
/* end subroutine (bfile_wbig) */

static int bfile_wreg(bfile *op,cvoid *abuf,int alen) noex {
	int		rs = SR_OK ;
	int		alenr = alen ;
	int		mlen ;
	int		len ;
	bool		f_bufline = (op->bm == bfilebm_line) ;
	cchar		*abp = (cchar *) abuf ;
	while ((rs >= 0) && (alenr > 0)) {
	    if_constexpr (f_chunkcpy) {
	        if ((rs >= 0) && (op->len == 0) && (alenr >= op->bsize)) {
		    while ((rs >= 0) && (alenr >= op->bsize)) {
	                mlen = op->bsize ;
		        rs = bfile_wbig(op,abuf,mlen) ;
		        alenr -= mlen ;
		    }
	        } /* end if */
	    } /* end if_constexpr (f_chunkcpy) */
	    if ((rs >= 0) && (alenr > 0)) {
	        int	n = 0 ;
	        int	blenr = intconv(op->bdata + op->bsize - op->bp) ;
	        mlen = min(alenr,blenr) ;
	        if (f_bufline && (mlen > 0)) {
	            cchar	*tp ;
	            if ((tp = strnrchr(abp,mlen,'\n')) != nullptr) {
	                n = intconv(op->len + ((tp+1) - abp)) ;
	            }
	        }
	        len = bfile_bufcpy(op,abp,mlen) ;
	        abp += len ;
	        alenr -= len ;
		if_constexpr (f_flushpart) {
	            if (op->bp == (op->bdata + op->bsize)) {
	                rs = bfile_flush(op) ;
	            } else if (f_bufline && (n > 0)) {
	                rs = bfile_flushn(op,n) ;
	            }
		} else {
	            if (op->bp == (op->bdata + op->bsize)) {
	                rs = bfile_flush(op) ;
		    }
		} /* end if_constexpr (f_flushpart) */
	    } /* end if */
	} /* end while */
	return (rs >= 0) ? alen : rs ;
}
/* end subroutine (bfile_wreg) */

static int bfile_bufcpy(bfile *op,cchar *abp,int mlen) noex {
	if (mlen > MEMCPYLEN) {
	    memcpy(op->bp,abp,mlen) ;
	} else {
	    char	*bp = op->bp ;
	    for (int i = 0 ; i < mlen ; i += 1) {
	        *bp++ = *abp++ ;
	    }
	}
	op->bp += mlen ;
	op->len += mlen ;
	op->offset += mlen ;
	return mlen ;
}
/* end subroutine (bfile_bufcpy) */


