/* bwrite SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* routine to write bytes */
/* version %I% last-modified %G% */

#define	CF_DEBUG	1		/* debugging */
#define	CF_CHUNKCPY	0		/* try chunk copy */
#define	CF_FLUSHPART	1		/* do partial flushes */

/* revision history:

	= 1998-07-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	bwrite

	Description:
	Unlike the standard I-O library, all writes of this library
	are atomic in that the entire portion of each write request
	is actually written to the file as a whole. Each write block
	is either written to the file as a single block or in
	conjunction with previous write requests, but in no way
	will any single write request be broken up and written
	separately to the file.  Note that this library can also
	freely intermix reads and writes to a file with the data
	ending up where it should without getting scrambled as in
	the standard library.  Both of the above features, as well
	as some other features unique to this library, would normally
	make this package slower than the standard I-O library, but
	this package is normally faster than most versions of the
	standard package and probably close in performance with
	some of the latest implemtations which use some of the
	buffer management strategies used here.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/param.h>		/* POSIX */
#include	<unistd.h>		/* POSIX */
#include	<fcntl.h>		/* POSIX */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<algorithm>		/* C++STD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<ucdesc.h>		/* LIBUC */
#include	<strn.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */
#include	<libdebug.h>		/* LIBDEUG |DEBUGPRINTF(3debug)| */

#include	"bfile.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| */

/* local defines */

#define	MEMCPYLEN	100		/* minimum length for |memcpy(3c)| */

#ifndef	CF_DEBUG
#define	CF_DEBUG	0		/* debugging */
#endif
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

namespace bfile_ns {
    int		bfile_write(bfile *,cvoid *,int) noex ;
} /* end namespace (bfile_ns) */


/* external variables */


/* local structures */


/* forward references */

local int	bfile_wbig	(bfile *,cvoid *,int) noex ;
local int	bfile_wreg	(bfile *,cvoid *,int) noex ;
local int	bfile_bufcpy	(bfile *,cchar *,int) noex ;


/* local variables */

cbool		f_debug		= CF_DEBUG ;
cbool		f_chunkcpy	= CF_CHUNKCPY ;
cbool		f_flushpart	= CF_FLUSHPART ;


/* exported variables */


/* exported subroutines */

int bwrite(bfile *op,cvoid *abuf,int alen) noex {
	int		rs ;
	DEBUGPRINTF("ent\n") ;
	if ((rs = bfile_magic(op,abuf)) > 0) {
	    DEBUGPRINTF("-> ckwr\n") ;
	    if ((rs = bfile_ckwr(op)) >= 0) {
	        DEBUGPRINTF("-> bfile_write\n") ;
		rs = bfile_ns::bfile_write(op,abuf,alen) ;
	    } /* end if (access) */
	} /* end if (magic) */
	DEBUGPRINTF("ret rs=%d\n",rs) ;
	return rs ;
} /* end subroutine (bwrite) */

namespace bfile_ns {
    int bfile_write(bfile *op,cvoid *abuf,int alen) noex {
	int		rs ;
	cchar		*abp = charp(abuf) ;
	if (alen < 0) alen = lenstr(abp) ;
	DEBUGPRINTF("ent alen=%d\n",alen) ;
	if (op->bm == bfilebm_none) {
	    DEBUGPRINTF("wbig\n") ;
	    rs = bfile_wbig(op,abuf,alen) ;
	} else {
	    DEBUGPRINTF("wreg\n") ;
	    rs = bfile_wreg(op,abuf,alen) ;
	} /* end if */
	DEBUGPRINTF("ret rs=%d\n",rs) ;
	return rs ;
    } /* end subroutine (bfile_write) */
} /* end namespace (bfile_ns) */


/* local subroutines */

local int bfile_wbig(bfile *op,cvoid *abuf,int alen) noex {
	int		rs ;
	if ((rs = bfile_flush(op)) >= 0) {
	    int		abl = alen ;
	    cchar	*abp = charp(abuf) ;
	    while ((rs >= 0) && (abl > 0)) {
	        if ((rs = uc_writen(op->fd,abp,abl)) > 0) {
	            op->offset += rs ;
	            abp += rs ;
	            abl -= rs ;
	        } /* end if (uc_writen) */
	    } /* end while */
	} /* end if (flush) */
	return (rs >= 0) ? alen : rs ;
} /* end subroutine (bfile_wbig) */

local int bfile_wreg(bfile *op,cvoid *abuf,int alen) noex {
	int		rs = SR_OK ;
	int		alenr = alen ;
	int		mlen ;
	int		len ;
	bool		f_bufline = (op->bm == bfilebm_line) ;
	cchar		*abp = (cchar *) abuf ;
	while ((rs >= 0) && (alenr > 0)) {
	    if_constexpr (f_chunkcpy) {
	        if ((rs >= 0) && (op->len == 0) && (alenr >= op->bsz)) {
		    while ((rs >= 0) && (alenr >= op->bsz)) {
	                mlen = op->bsz ;
		        rs = bfile_wbig(op,abuf,mlen) ;
		        alenr -= mlen ;
		    }
	        } /* end if */
	    } /* end if_constexpr (f_chunkcpy) */
	    if ((rs >= 0) && (alenr > 0)) {
	        int	n = 0 ;
	        int	blenr = intconv(op->bdata + op->bsz - op->bp) ;
	        mlen = min(alenr,blenr) ;
	        if (f_bufline && (mlen > 0)) {
	            if (cchar *tp = strnrchr(abp,mlen,'\n') ; tp) {
	                n = intconv(op->len + ((tp+1) - abp)) ;
	            }
	        } /* end if */
	        len = bfile_bufcpy(op,abp,mlen) ;
	        abp += len ;
	        alenr -= len ;
		if_constexpr (f_flushpart) {
	            if (op->bp == (op->bdata + op->bsz)) {
	                rs = bfile_flush(op) ;
	            } else if (f_bufline && (n > 0)) {
	                rs = bfile_flushn(op,n) ;
	            }
		} else {
	            if (op->bp == (op->bdata + op->bsz)) {
	                rs = bfile_flush(op) ;
		    }
		} /* end if_constexpr (f_flushpart) */
	    } /* end if */
	} /* end while */
	return (rs >= 0) ? alen : rs ;
} /* end subroutine (bfile_wreg) */

local int bfile_bufcpy(bfile *op,cchar *abp,int mlen) noex {
	if (mlen > MEMCPYLEN) {
	    memcopy(op->bp,abp,mlen) ;
	} else {
	    char	*bp = op->bp ;
	    for (int i = 0 ; i < mlen ; i += 1) {
	        *bp++ = *abp++ ;
	    }
	} /* end if */
	op->bp += mlen ;
	op->len += mlen ;
	op->offset += mlen ;
	return mlen ;
} /* end subroutine (bfile_bufcpy) */


