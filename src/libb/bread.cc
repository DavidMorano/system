/* bread SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* "Basic I-O" package similiar to some other thing whose initials is "stdio" */
/* version %I% last-modified %G% */

#define	CF_MEMCPY	1		/* use |memcpy(3c)| */

/* revision history:

	= 1998-07-01, David A­D­ Morano
	This code was originally written.

	= 1999-01-10, David A­D­ Morano
	Wow, I finally got around to adding memory mapping to this
	thing! Other subroutines of mine have been using memory
	mapped I-O for years but this is one of those routines where
	it should have been applied a long time ago because of its
	big performance benefits! It is all a waste because it is
	way slower than without it! This should teach me to leave
	old programs alone!

*/

/* Copyright © 1998,1999 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	bread

	Description:
	We do the reading.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/mman.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<usystem.h>
#include	<localmisc.h>

#include	"bfile.h"


/* local defines */

#ifndef	CF_MEMCPY
#define	CF_MEMCPY	1		/* use |memcpy(3c)| */
#endif


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* forward references */

static int bfile_rdmap(bfile *,void *,int,int,int) noex ;
static int bfile_rdreg(bfile *,void *,int,int,int) noex ;


/* local variables */

constexpr bool		f_memcpy = CF_MEMCPY ;


/* exported variables */


/* exported subroutines */

int breade(bfile *op,void *ubuf,int ulen,int to,int opts) noex {
	int		rs ;
	if ((rs = bfile_magic(op,ubuf)) > 0) {
	    if ((rs = bfile_ckrd(op)) >= 0) {
	        if (op->fl.mapinit) {
	            rs = bfile_rdmap(op,ubuf,ulen,to,opts) ;
	        } else {
	            rs = bfile_rdreg(op,ubuf,ulen,to,opts) ;
	        }
	    } /* end if (reading) */
	} /* end if (magic) */
	return rs ;
}
/* end routine (breade) */

int bread(bfile *op,void *ubuf,int ulen) noex {
	return breade(op,ubuf,ulen,-1,0) ;
}
/* end routine (bread) */


/* local subroutines */

static int bfile_rdmap(bfile *op,void *ubuf,int ulen,int,int) noex {
	USTAT		sb ;
	int		rs ;
	int		tlen = 0 ;
	int		pagemask = op->pagesize - 1 ;
	int		i, mlen ;
	int		f_already ;

	f_already = false ;
	while (tlen < ulen) {
	    mlen = intconv(op->fsize - op->offset) ;
	    if ((mlen > 0) &&
	        ((op->bp == nullptr) || (op->len == op->pagesize))) {

	        i = (op->offset / op->pagesize) & (BFILE_NMAPS - 1) ;
	        if ((! op->maps[i].fl.valid) || (op->maps[i].bdata == nullptr)
	            || (op->maps[i].offset != (op->offset & (~ pagemask))))
	            bfile_pagein(op,op->offset,i) ;

	        op->len = intconv(op->offset & pagemask) ;
	        op->bp = op->maps[i].bdata + op->len ;

	    } /* end if (initializing memory mapping) */

/* prepare to move data */

	    if ((op->pagesize - op->len) < mlen) {
	        mlen = (op->pagesize - op->len) ;
	    }
	    if ((ulen - tlen) < mlen) {
	        mlen = (ulen - tlen) ;
	    }
	    if (mlen > 0) {
		char	*dp = charp(ubuf) ;
	        memcpy((dp + tlen),op->bp,mlen) ;
	        op->bp += mlen ;
	        op->len += mlen ;
	        op->offset += mlen ;
	        tlen += mlen ;
	    } /* end if (move it) */

/* if we were file-size limited */

	    if (op->offset >= op->fsize) {
	        if (f_already) break ;

	        rs = u_fstat(op->fd,&sb) ;
	        if (rs < 0) break ;

	        op->fsize = sb.st_size ;
	        f_already = true ;

	    } /* end if (file size limited) */

	} /* end while (reading) */

	return (rs >= 0) ? tlen : rs ;
}
/* end subroutine (bfile_rdmap) */

static int bfile_rdreg(bfile *op,void *ubuf,int ulen,int to,int opts) noex {
	int		rs = SR_OK ;
	int		maxeof ;
	int		neof = 0 ;
	int		len ;
	int		mlen ;
	int		tlen = 0 ;
	int		f_already = false ;
	char		*dbp ;

	maxeof = (op->fl.network && (to < 0)) ? BFILE_MAXNEOF : 1 ;
	dbp = charp(ubuf) ;
	while ((rs >= 0) && (ulen > 0) && (neof < maxeof)) {

	    if (op->len <= 0) {
	        if (f_already) break ;
		if (to >= 0) {
	            rs = uc_reade(op->fd,op->bdata,op->bsize,to,opts) ;
	            len = rs ;
		} else {
	            rs = u_read(op->fd,op->bdata,op->bsize) ;
	            len = rs ;
		}
	        if (rs < 0) break ;
		if (len == 0) {
		    neof += 1 ;
		} else {
		    neof = 0 ;
		}
	        if (op->len < op->bsize) {
	            f_already = true ;
		}
	        op->bp = op->bdata ;
		op->len = len ;
	    } /* end if (refill buffer) */

	    if ((rs >= 0) && (op->len > 0)) {
	        mlen = (op->len < ulen) ? op->len : ulen ;
		if_constexpr (f_memcpy) {
	            memcpy(dbp,op->bp,mlen) ;
	            op->bp += mlen ;
	            dbp += mlen ;
		} else {
	            for (int i = 0 ; i < mlen ; i += 1) {
	                *dbp++ = *(op->bp)++ ;
	 	    }
		} /* end if_constexpr (f_memcpy) */
	        op->offset += mlen ;
	        op->len -= mlen ;
	        tlen += mlen ;
	        ulen -= mlen ;
	    } /* end if */

	} /* end while */

	return (rs >= 0) ? tlen : rs ;
}
/* end subroutine (bfile_rdreg) */


