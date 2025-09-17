/* breadln SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++11 */

/* "Basic I-O" package similiar to "stdio" */
/* last modifed %G% version %I% */

#define	CF_MEMCCPY	0		/* faster than |memccpy(3c)|! */

/* revision history:

	= 1998-07-01, David A­D­ Morano
	This code was originally written.

	= 1999-01-10, David A­D­ Morano
	I added the little extra code to allow for memory mapped
	I-O. It is all a waste because it is way slower than without
	it! This should teach me to leave old programs alone!!!

*/

/* Copyright © 1998,1999 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This subroutine reads a single line from the buffer (or
	where ever) and returns that line to the caller. Any remaining
	data is left for subsequent reads (of any kind).

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/stat.h>
#include	<sys/mman.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>		/* |memccpy(3c)| */
#include	<usystem.h>
#include	<ascii.h>
#include	<localmisc.h>

#include	"bfile.h"


/* local defines */

#ifndef	CF_MEMCCPY
#define	CF_MEMCCPY	0		/* faster than |memccpy(3c)|! */
#endif


/* imported namespaces */

using std::nullptr_t ;			/* type */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

static int	breadlnmap(bfile *,char *,int) noex ;
static int	breadlnreg(bfile *,char *,int,int) noex ;
static int	breload(bfile *,int,int) noex ;

static inline int	isoureol(int) noex ;

static inline char *stpccpy(char *dp,cchar *sp,int ch,int n) noex {
	return charp(memccpy(dp,sp,ch,n)) ;
}


/* local variables */

constexpr bool		f_memcpy = CF_MEMCCPY ;


/* exported variables */


/* exported subroutines */

int breadlnto(bfile *op,char *ubuf,int ulen,int to) noex {
	int		rs ;
	int		rlen = 0 ;
	if ((rs = bfile_magic(op,ubuf)) > 0) {
	    if ((rs = bfile_ckrd(op)) >= 0) {
		if (op->fl.mapinit) {
		    rs = breadlnmap(op,ubuf,ulen) ;
		    rlen = rs ;
		} else {
		    rs = breadlnreg(op,ubuf,ulen,to) ;
		    rlen = rs ;
		} /* end if (read map or not) */
	    } /* end if (reading) */
	} /* end if (non-null) */
	return (rs >= 0) ? rlen : rs ;
}
/* end subroutine (breadlnto) */


/* local subroutines */

static int breadlnmap(bfile *op,char *ubuf,int ulen) noex {
	USTAT		sb ;
	cnullptr	np{} ;
	size_t		baseoff, runoff ;
	int		rs = SR_OK ;
	int		mlen ;
	int		i ;
	int		pagemask = (op->pagesize - 1) ;
	int		tlen = 0 ;
	int		f_partial = false ;
	char		*dbp = ubuf ;

	runoff = op->offset ;
	while ((rs >= 0) && (tlen < ulen)) {

/* is there more data in the file and are we at a map page boundary? */

	    mlen = intconv(op->fsize - runoff) ;

	    if ((mlen > 0) &&
	        ((op->bp == nullptr) || (op->len == op->pagesize))) {

	        i = (runoff / op->pagesize) & (BFILE_NMAPS - 1) ;
	        baseoff = runoff & (~ pagemask) ;
	        if ((! op->maps[i].fl.valid) || (op->maps[i].bdata == np)
	            || (op->maps[i].offset != baseoff)) {

	            bfile_pagein(op,runoff,i) ;
	        }

	        op->len = intconv(runoff & pagemask) ;
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
	        char	*bp ;
	        char	*lastp ;
		if_constexpr (f_memcpy) {
	            if ((lastp = stpccpy(dbp,op->bp,'\n',mlen)) == np) {
	                lastp = dbp + mlen ;
	            }
	            i = lastp - dbp ;
	            dbp += i ;
	            op->bp += i ;
		} else {
	            bp = op->bp ;
	            lastp = op->bp + mlen ;
	            while (bp < lastp) {
	                if (isoureol(*dbp++ = *bp++)) break ;
	            }
	            i = intconv(bp - op->bp) ;
	            op->bp += i ;
		} /* end if_constexpr (f_memcpy) */
	        op->len += i ;
	        runoff += i ;
	        tlen += i ;
	        if ((i > 0) && isoureol(dbp[-1])) break ;
	    } /* end if (move it) */

/* if we were file size limited */

	    if ((rs >= 0) && (runoff >= op->fsize)) {
	        if (f_partial) break ;

	        rs = u_fstat(op->fd,&sb) ;

	        op->fsize = sb.st_size ;
	        f_partial = true ;
	    } /* end if (file size limited) */

	} /* end while (reading) */

	if (rs >= 0) {
	    op->offset += tlen ;
	}

	return (rs >= 0) ? tlen : rs ;
}
/* end subroutine (breadlnmap) */

static int breadlnreg(bfile *op,char *ubuf,int ulen,int to) noex {
	cnullptr	np{} ;
	cint		opts = FM_TIMED ;
	int		rs = SR_OK ;
	int		mlen ;
	int		i ;
	int		tlen = 0 ;
	int		f_partial = false ;
	char		*dbp = ubuf ;

	while ((rs >= 0) && (ulen > 0)) {

	    if (op->len == 0) {
	        if (f_partial && op->fl.inpartline) break ;
	        rs = breload(op,to,opts) ;
	        if (rs <= 0) break ;
	        if (op->len < op->bsize) f_partial = true ;
	    } /* end if (refilling up buffer) */

	    mlen = (op->len < ulen) ? op->len : ulen ;

	    if ((rs >= 0) && (mlen > 0)) {
	        char	*bp ;
	        char	*lastp ;
		if_constexpr (f_memcpy) {
	            if ((lastp = stpccpy(dbp,op->bp,'\n',mlen)) == np) {
	                lastp = dbp + mlen ;
	            }
	            i = lastp - dbp ;
	            dbp += i ;
	            op->bp += i ;
		} else {
	            bp = op->bp ;
	            lastp = op->bp + mlen ;
	            while (bp < lastp) {
	                if (isoureol(*dbp++ = *bp++)) break ;
	            } /* end while */
	            i = intconv(bp - op->bp) ;
	            op->bp += i ;
		} /* end if_constexpr (f_memcpy) */
	        op->len -= i ;
	        tlen += i ;
	        if ((i > 0) && isoureol(dbp[-1])) break ;
	        ulen -= mlen ;
	    } /* end if (move it) */

	} /* end while (trying to satisfy request) */

	if (rs >= 0) {
	    op->offset += tlen ;
	}

	return (rs >= 0) ? tlen : rs ;
}
/* end subroutine (breadlnreg) */

static int breload(bfile *op,int to,int opts) noex {
	int		rs = SR_OK ;
	int		maxeof ;
	int		neof = 0 ;
	int		len = 0 ;
	maxeof = (op->fl.network && (to < 0)) ? BFILE_MAXNEOF : 1 ;
	while ((rs >= 0) && (len == 0) && (neof < maxeof)) {
	    if (to >= 0) {
	        rs = uc_reade(op->fd,op->bdata,op->bsize,to,opts) ;
	        len = rs ;
	    } else {
	        rs = u_read(op->fd,op->bdata,op->bsize) ;
	        len = rs ;
	    }
	    if (rs >= 0) {
	        neof = (len == 0) ? (neof+1) : 0 ;
	    }
	} /* end while */
	if (rs >= 0) {
	    op->len = len ;
	    op->bp = op->bdata ;
	}
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (breload) */

static inline int isoureol(int ch) noex {
	return (ch == '\n') ;
}
/* end subroutine (isoureol) */


