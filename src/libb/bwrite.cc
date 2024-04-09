/* bwrite SUPPORT */
/* lang=C++20 */

/* routine to write bytes */
/* version %I% last-modified %G% */

#define	CF_CHUNKCPY	0		/* try chunk copy */
#define	CF_FLUSHPART	1		/* do partial flushes */

/* revision history:

	= 1998-07-01, David A­D­ Morano
	This subroutine was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Unlike the standard I/O library, all writes of this library
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
	slower than the standard I/O library, but this package is
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
#include	<cstring>
#include	<algorithm>
#include	<usystem.h>
#include	<strn.h>
#include	<localmisc.h>

#include	"bfile.h"


/* local defines */

#define	MEMCPYLEN	100		/* minimum length for 'memcpy(3c)' */


/* imported namespaces */

using std::min ;			/* subroutine-template */
using std::max ;			/* subroutine-template */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

static int	bfile_wbig(bfile *,cvoid *,int) noex ;
static int	bfile_wreg(bfile *,cvoid *,int) noex ;
static int	bfile_bufcpy(bfile *,cchar *,int) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int bwrite(bfile *fp,cvoid *abuf,int alen) noex {
	int		rs ;
	if ((rs = bmagic(fp,abuf)) > 0) {
	cchar	*abp = charp(abuf) ;
	int		f_bufnone ;

	if ((fp->oflags & O_ACCMODE) == 0) return SR_RDONLY ;

	if (alen < 0) alen = strlen(abp) ;

	f_bufnone = (fp->bm == bfile_bmnone) ;

/* if we were not previously writing, seek to the proper place */

	if (! fp->f.write) {

	    if ((! fp->f.notseek) &&
	        (! (fp->oflags & O_APPEND))) {

	        rs = u_seek(fp->fd,fp->offset,SEEK_SET) ;

	    }

	    fp->len = 0 ;
	    fp->bp = fp->bdata ;
	    fp->f.write = true ;

	} /* end if (previously reading) */

/* check for a large 'write' */

	if (rs >= 0) {
	    if (f_bufnone) {
	        rs = bfile_wbig(fp,abuf,alen) ;
	    } else {
	        rs = bfile_wreg(fp,abuf,alen) ;
	    }
	}

	} /* end if (magic) */
	return rs ;
}
/* end subroutine (bwrite) */


/* local subroutines */

static int bfile_wbig(bfile *fp,cvoid *abuf,int alen) noex {
	int		rs ;
	if ((rs = bfile_flush(fp)) >= 0) {
	    int		abl = alen ;
	    cchar	*abp = charp(abuf) ;
	    while ((rs >= 0) && (abl > 0)) {
	        if ((rs = uc_writen(fp->fd,abp,abl)) >= 0) {
	            cint	len = rs ;
	            fp->offset += len ;
	            abp += len ;
	            abl -= len ;
	        }
	    } /* end while */
	} /* end if (flush) */
	return (rs >= 0) ? alen : rs ;
}
/* end subroutine (bfile_wbig) */

static int bfile_wreg(bfile *fp,cvoid *abuf,int alen) noex {
	int		rs = SR_OK ;
	int		alenr = alen ;
	int		mlen ;
	int		len ;
	int		f_bufline = (fp->bm == bfile_bmline) ;
	cchar	*abp = (cchar *) abuf ;

	while ((rs >= 0) && (alenr > 0)) {

#if	CF_CHUNKCPY
	    if ((rs >= 0) && (fp->len == 0) && (alenr >= fp->bsize)) {
		while ((rs >= 0) && (alenr >= fp->bsize)) {
	            mlen = fp->bsize ;
		    rs = bfile_wbig(fp,abuf,mlen)
		    alenr -= mlen ;
		}
	    } /* end if */
#endif /* CF_CHUNKCPY */

	    if ((rs >= 0) && (alenr > 0)) {
	        int	n = 0 ;
	        int	blenr = (fp->bdata + fp->bsize - fp->bp) ;

	        mlen = MIN(alenr,blenr) ;
	        if (f_bufline && (mlen > 0)) {
	            cchar	*tp ;
	            if ((tp = strnrchr(abp,mlen,'\n')) != NULL) {
	                n = (fp->len + ((tp+1) - abp)) ;
	            }
	        }

	        len = bfile_bufcpy(fp,abp,mlen) ;
	        abp += len ;
	        alenr -= len ;

#if	CF_FLUSHPART
	        if (fp->bp == (fp->bdata + fp->bsize)) {
	            rs = bfile_flush(fp) ;
	        } else if (f_bufline && (n > 0)) {
	            rs = bfile_flushn(fp,n) ;
	        }
#else /* CF_FLUSHPART */
	        if (fp->bp == (fp->bdata + fp->bsize)) {
	            rs = bfile_flush(fp) ;
		}
#endif /* CF_FLUSHPART */

	    } /* end if */

	} /* end while */

	return (rs >= 0) ? alen : rs ;
}
/* end subroutine (bfile_wreg) */

static int bfile_bufcpy(bfile *fp,cchar *abp,int mlen) noex {
	if (mlen > MEMCPYLEN) {
	    memcpy(fp->bp,abp,mlen) ;
	} else {
	    char	*bp = fp->bp ;
	    for (int i = 0 ; i < mlen ; i += 1) {
	        *bp++ = *abp++ ;
	    }
	}
	fp->bp += mlen ;
	fp->len += mlen ;
	fp->offset += mlen ;
	return mlen ;
}
/* end subroutine (bfile_bufcpy) */


