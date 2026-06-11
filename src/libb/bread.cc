/* bread SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* "Basic I-O" package similiar to some other thing whose initials is "stdio" */
/* version %I% last-modified %G% */

#define	CF_DEBUG	1		/* debugging */
#define	CF_MEMCOPY	1		/* use |memcopy(3c)| */

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
#include	<sys/mman.h>		/* POSIX */
#include	<unistd.h>		/* POSIX */
#include	<fcntl.h>		/* POSIX */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<ucdesc.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */
#include	<libdebug.h>		/* LIBDEBUG |DEBUGPRINTF(3debug)| */

#include	"bfile.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| */

/* local defines */

#ifndef	CF_DEBUG
#define	CF_DEBUG	0		/* debugging */
#endif
#ifndef	CF_MEMCOPY
#define	CF_MEMCOPY	1		/* use |memcopy(3c)| */
#endif


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* forward references */

local int bfile_rdmap(bfile *,void *,int,int,int) noex ;
local int bfile_rdreg(bfile *,void *,int,int,int) noex ;


/* local variables */

cbool		f_debug		= CF_DEBUG ;
cbool		f_memcopy	= CF_MEMCOPY ;


/* exported variables */


/* exported subroutines */

int breade(bfile *op,void *ubuf,int ulen,int to,int opts) noex {
	int		rs ;
	DEBUGPRINTF("ent\n") ;
	if ((rs = bfile_magic(op,ubuf)) > 0) {
	    if ((rs = bfile_ckrd(op)) >= 0) {
	        if (op->fl.mapinit) {
	            rs = bfile_rdmap(op,ubuf,ulen,to,opts) ;
	        } else {
	            rs = bfile_rdreg(op,ubuf,ulen,to,opts) ;
	        }
	    } /* end if (reading) */
	} /* end if (magic) */
	DEBUGPRINTF("ret rs=%d\n",rs) ;
	return rs ;
}
/* end routine (breade) */

int bread(bfile *op,void *ubuf,int ulen) noex {
	return breade(op,ubuf,ulen,-1,0) ;
}
/* end routine (bread) */


/* local subroutines */

local int bfile_rdreg(bfile *op,void *ubuf,int ulen,int to,int opts) noex {
	int		rs = SR_OK ;
	int		maxeof ;
	int		neof = 0 ;
	int		tlen = 0 ; /* return-value */
	bool		f_already = false ;
	char		*dbp ;
	DEBUGPRINTF("ent to=%d\n",to) ;
	maxeof = (op->fl.network && (to < 0)) ? BFILE_MAXNEOF : 1 ;
	dbp = charp(ubuf) ;
	while ((rs >= 0) && (ulen > 0) && (neof < maxeof)) {
	    int len ;
	    if (op->len == 0) {
	        if (f_already) break ;
		if (to >= 0) {
	            rs = uc_reade(op->fd,op->bdata,op->bsz,to,opts) ;
	            len = rs ;
		} else {
	            rs = u_read(op->fd,op->bdata,op->bsz) ;
	            len = rs ;
		} /* end if */
		DEBUGPRINTF("read() rs=%d\n",rs) ;
	        if (rs < 0) break ;
		if (len == 0) {
		    neof += 1 ;
		} else {
		    neof = 0 ; /* reset */
		}
	        if (op->len < op->bsz) {
	            f_already = true ;
		}
	        op->bp = op->bdata ;
		op->len = len ;
	    } /* end if (refill buffer) */
	    if ((rs >= 0) && (op->len > 0)) {
	        cint mlen = (op->len < ulen) ? op->len : ulen ;
		if_constexpr (f_memcopy) {
	            memcopy(dbp,op->bp,mlen) ;
	            op->bp += mlen ;
	            dbp += mlen ;
		} else {
	            for (int i = 0 ; i < mlen ; i += 1) {
	                *dbp++ = *(op->bp)++ ;
	 	    } /* end for */
		} /* end if_constexpr (f_memcopy) */
	        op->offset += mlen ;
	        op->len -= mlen ;
	        tlen += mlen ;
	        ulen -= mlen ;
	    } /* end if */
	} /* end while */
	DEBUGPRINTF("ret rs=%d tlen=%d\n",rs,tlen) ;
	return (rs >= 0) ? tlen : rs ;
} /* end subroutine (bfile_rdreg) */

local int bfile_rdmap(bfile *op,void *ubuf,int ulen,int,int) noex {
    	cnullptr	np{} ;
	ustat		sb ;
	int		rs ;
	int		tlen = 0 ;
	int		pagemask = (op->pagesz - 1) ;
	int		i, mlen ;
	bool		f_already = false ;
	while (tlen < ulen) {
	    bool f = false ;
	    mlen = intconv(op->fsize - op->offset) ;
	    if ((mlen > 0) && ((op->bp == np) || (op->len == op->pagesz))) {
	        i = (op->offset / op->pagesz) & (BFILE_NMAPS - 1) ;
	        f = f || (! op->maps[i].fl.valid) ;
		f = f || (op->maps[i].bdata == np) ;
		f = f || (op->maps[i].offset != (op->offset & (~ pagemask))) ;
		if (f) {
	            bfile_pagein(op,op->offset,i) ;
		}
	        op->len = intconv(op->offset & pagemask) ;
	        op->bp = op->maps[i].bdata + op->len ;
	    } /* end if (initializing memory mapping) */
	    /* prepare to move data */
	    if ((op->pagesz - op->len) < mlen) {
	        mlen = (op->pagesz - op->len) ;
	    }
	    if ((ulen - tlen) < mlen) {
	        mlen = (ulen - tlen) ;
	    }
	    if (mlen > 0) {
		char	*dp = charp(ubuf) ;
	        memcopy((dp + tlen),op->bp,mlen) ;
	        op->bp += mlen ;
	        op->len += mlen ;
	        op->offset += mlen ;
	        tlen += mlen ;
	    } /* end if (move it) */
	    /* if we were file-size limited */
	    if (op->offset >= op->fsize) {
	        if (f_already) break ;
		{
	        rs = u_fstat(op->fd,&sb) ;
	        if (rs < 0) break ;
		}
		{
	        op->fsize = sb.st_size ;
	        f_already = true ;
		}
	    } /* end if (file size limited) */
	} /* end while (reading) */
	return (rs >= 0) ? tlen : rs ;
} /* end subroutine (bfile_rdmap) */


