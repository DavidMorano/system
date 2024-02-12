/* breadln SUPPORT */
/* lang=C++11 */

/* "Basic I/O" package similiar to "stdio" */
/* last modifed %G% version %I% */

#define	CF_MEMCCPY	0		/* faster than |memccpy(3c)|! */

/* revision history:

	= 1998-07-01, David A­D­ Morano
	This subroutine was originally written.

	= 1999-01-10, David A­D­ Morano
	I added the little extra code to allow for memory mapped
	I/O. It is all a waste because it is way slower than without
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
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<ascii.h>
#include	<localmisc.h>

#include	"bfile.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

static int	breadlnmap(bfile *,char *,int) noex ;
static int	breadlnreg(bfile *,char *,int,int) noex ;
static int	breload(bfile *,int,int) noex ;

static inline int	isoureol(int) noex ;


/* local variables */


/* exported subroutines */

int breadlnto(bfile *fp,char *ubuf,int ulen,int to) noex {
	int		rs = SR_FAULT ;
	if (fp && ubuf) {
	    rs = SR_NOTOPEN ;
	    if (fp->magic == BFILE_MAGIC) {
		rs = SR_WRONLY ;
	        if (! (fp->oflags & O_WRONLY)) {
		    rs = SR_OK ;
	            if (! fp->f.nullfile) {
	                if (fp->f.write) {
	                    fp->f.write = FALSE ;
	                    if (fp->len > 0) rs = bfile_flush(fp) ;
	                } /* end if (switching) */
	                if (rs >= 0) {
	                    if (fp->f.mapinit) {
	                        rs = breadlnmap(fp,ubuf,ulen) ;
	                    } else {
	                        rs = breadlnreg(fp,ubuf,ulen,to) ;
	                    } /* end if (read map or not) */
	                } /* end if */
		    } /* end if (not-nullfile) */
	        } /* end if (valid) */
	    } /* end if (open) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (breadlnto) */


/* local subroutines */

static int breadlnmap(bfile *fp,char *ubuf,int ulen) noex {
	USTAT		sb ;
	size_t		baseoff, runoff ;
	int		rs = SR_OK ;
	int		mlen ;
	int		i ;
	int		pagemask = (fp->pagesize - 1) ;
	int		tlen = 0 ;
	int		f_partial = FALSE ;
	char		*dbp = ubuf ;

	runoff = fp->offset ;
	while ((rs >= 0) && (tlen < ulen)) {

/* is there more data in the file and are we at a map page boundary? */

	    mlen = (fp->fsize - runoff) ;

	    if ((mlen > 0) &&
	        ((fp->bp == NULL) || (fp->len == fp->pagesize))) {

	        i = (runoff / fp->pagesize) & (BFILE_NMAPS - 1) ;
	        baseoff = runoff & (~ pagemask) ;
	        if ((! fp->maps[i].f.valid) || (fp->maps[i].buf == NULL)
	            || (fp->maps[i].offset != baseoff)) {

	            bfile_pagein(fp,runoff,i) ;
	        }

	        fp->len = runoff & pagemask ;
	        fp->bp = fp->maps[i].buf + fp->len ;

	    } /* end if (initializing memory mapping) */

/* prepare to move data */

	    if ((fp->pagesize - fp->len) < mlen) {
	        mlen = (fp->pagesize - fp->len) ;
	    }

	    if ((ulen - tlen) < mlen) {
	        mlen = (ulen - tlen) ;
	    }

	    if (mlen > 0) {
	        char	*bp ;
	        char	*lastp ;

#if	CF_MEMCCPY
	        if ((lastp = memccpy(dbp,fp->bp,'\n',mlen)) == NULL) {
	            lastp = dbp + mlen ;
	        }
	        i = lastp - dbp ;
	        dbp += i ;
	        fp->bp += i ;
#else
	        bp = fp->bp ;
	        lastp = fp->bp + mlen ;
	        while (bp < lastp) {
	            if (isoureol(*dbp++ = *bp++)) break ;
	        }
	        i = bp - fp->bp ;
	        fp->bp += i ;
#endif /* CF_MEMCCPY */

	        fp->len += i ;
	        runoff += i ;
	        tlen += i ;
	        if ((i > 0) && isoureol(dbp[-1])) break ;

	    } /* end if (move it) */

/* if we were file size limited */

	    if ((rs >= 0) && (runoff >= fp->fsize)) {
	        if (f_partial) break ;

	        rs = bfilefstat(fp->fd,&sb) ;

	        fp->fsize = sb.st_size ;
	        f_partial = TRUE ;
	    } /* end if (file size limited) */

	} /* end while (reading) */

	if (rs >= 0) {
	    fp->offset += tlen ;
	}

	return (rs >= 0) ? tlen : rs ;
}
/* end subroutine (breadlnmap) */

static int breadlnreg(bfile *fp,char *ubuf,int ulen,int to) noex {
	cint		opts = FM_TIMED ;
	int		rs = SR_OK ;
	int		mlen ;
	int		i ;
	int		tlen = 0 ;
	int		f_partial = FALSE ;
	char		*dbp = ubuf ;

	while ((rs >= 0) && (ulen > 0)) {

	    if (fp->len == 0) {
	        if (f_partial && fp->f.inpartline) break ;
	        rs = breload(fp,to,opts) ;
	        if (rs <= 0) break ;
	        if (fp->len < fp->bsize) f_partial = TRUE ;
	    } /* end if (refilling up buffer) */

	    mlen = (fp->len < ulen) ? fp->len : ulen ;

	    if ((rs >= 0) && (mlen > 0)) {
	        char	*bp ;
	        char	*lastp ;

#if	CF_MEMCCPY
	        if ((lastp = memccpy(dbp,fp->bp,'\n',mlen)) == NULL) {
	            lastp = dbp + mlen ;
	        }
	        i = lastp - dbp ;
	        dbp += i ;
	        fp->bp += i ;
#else
	        bp = fp->bp ;
	        lastp = fp->bp + mlen ;
	        while (bp < lastp) {
	            if (isoureol(*dbp++ = *bp++)) break ;
	        } /* end while */
	        i = bp - fp->bp ;
	        fp->bp += i ;
#endif /* CF_MEMCCPY */

	        fp->len -= i ;
	        tlen += i ;
	        if ((i > 0) && isoureol(dbp[-1])) break ;

	        ulen -= mlen ;

	    } /* end if (move it) */

	} /* end while (trying to satisfy request) */

	if (rs >= 0) {
	    fp->offset += tlen ;
	}

	return (rs >= 0) ? tlen : rs ;
}
/* end subroutine (breadlnreg) */

static int breload(bfile *fp,int to,int opts) noex {
	int		rs = SR_OK ;
	int		maxeof ;
	int		neof = 0 ;
	int		len = 0 ;
	maxeof = (fp->f.network && (to < 0)) ? BFILE_MAXNEOF : 1 ;
	while ((rs >= 0) && (len == 0) && (neof < maxeof)) {
	    if (to >= 0) {
	        rs = uc_reade(fp->fd,fp->bdata,fp->bsize,to,opts) ;
	        len = rs ;
	    } else {
	        rs = u_read(fp->fd,fp->bdata,fp->bsize) ;
	        len = rs ;
	    }
	    if (rs >= 0) {
	        neof = (len == 0) ? (neof+1) : 0 ;
	    }
	} /* end while */
	if (rs >= 0) {
	    fp->len = len ;
	    fp->bp = fp->bdata ;
	}
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (breload) */

static int isoureol(int ch) noex {
	return (ch == '\n') ;
}
/* end subroutine (isoureol) */


