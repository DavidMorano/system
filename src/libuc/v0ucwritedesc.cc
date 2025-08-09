/* ucwritedesc SUPPORT */
/* charset=ISO8859-1 */
/* version %I% last-modified %G% */

/* interface component for UNIX® library-3c */
/* copy from one file descriptor to another */

#define	CF_VALLOC	0		/* only use 'valloc(3c)' */

/* revision history:

	= 1998-01-10, David A­D­ Morano
	The subroutine was originally written.

*/

/* Copyright Â© 1998 David AÂ­DÂ­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	uc_writedesc

	Description:
	This subroutine copies data from one file descriptor to
	another (source to destination) for the length specified
	as an argument.  The idea of having a separate subroutine
	for this sort of (usually) trivial function, is that we can
	perform optimizations that a typical user would find tiresome
	for such a "simple" function.

	Synopsis:
	int uc_writedesc(int dfd,int sfd,int len) noex

	Arguments:
	sfd		source file descriptor
	dfd		destination file descriptor
	len		length to copy

	Returns:
	>=0		length copied
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/stat.h>
#include	<sys/mman.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<climits>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<usystem.h>
#include	<localmisc.h>


/* local defines */

#define	PIPEBUFLEN	2048

#if	CF_VALLOC
#define	MAXBUFLEN	(16*1024*1024)
#else
#define	MAXBUFLEN	(2*1024*1024)
#endif

#ifndef	BCEIL
#define	BCEIL(v,m)	(((v) + ((m) - 1)) & (~ ((m) - 1)))
#endif


/* local defines */


/* local namespaces */

using std::min ;			/* subroutine-template */
using std::max ;			/* subroutine-template */
using std::nothrow ;			/* constant */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int uc_writedesc(int dfd,int sfd,int ulen) noex {
	int		rs = SR_OK ;
	int		tlen = 0 ;

	if (sfd == dfd) return SR_INVALID ;

	if (ulen != 0) {
	    if ((rs = ucpagesize) >= 0) {
	    size_t	fsize = SIZE_MAX ;
	    cint	ps = rs ;
	    int		readlen ;

/* calculate the size of a buffer to allocate */

	    if ((ulen < 0) || (ulen > ps)) {
	        if (ulen >= 0) {
	            readlen = BCEIL(ulen,ps) ;
	        } else {
	            int		bsize ;
	            if (ustat sb ; (rs = u_fstat(sfd,&sb)) >= 0) {
	                cmode	m = sb.st_mode ;
	                if (S_ISREG(m) || S_ISBLK(m)) {
	                    fsize = (size_t) (sb.st_size & SIZE_MAX) ;
	                    bsize = (MAX(sb.st_size,1) & INT_MAX) ;
	                    readlen = BCEIL(bsize,ps) ;
	                } else if (S_ISFIFO(m)) {
	                    readlen = PIPEBUFLEN ;
	                } else {
	                    readlen = ps ;
	                }
	            } /* end if (u_fstat) */
	        } /* end if */
	        if (readlen > MAXBUFLEN) readlen = MAXBUFLEN ;
	    } else {
	        readlen = ps ;
	    }

	    if ((rs >= 0) && (fsize > 0)) {
	        char		*mdata = NULL ;

#if	CF_VALLOC
#else
	        size_t		msize = 0 ;
#endif /* CF_VALLOC */

/* allocate the buffer */

#if	CF_VALLOC
	        rs = uc_libvalloc(readlen,&mdata) ;
#else
	        if (readlen <= ps) {
	            rs = uc_libmalloc(readlen,&mdata) ;
	        } else {
	            cint	mprot = (PROT_READ|PROT_WRITE) ;
	            cint	mflag = (MAP_PRIVATE|MAP_ANON) ;
	            msize = readlen ;
	            rs = u_mmap(NULL,msize,mprot,mflag,-1,0L,&mdata) ;
	        }
#endif /* CF_VALLOC */

	        if (rs >= 0) {
	            int	wlen ;

/* perform the copy (using the allocated buffer) */

	            while ((ulen < 0) || (tlen < ulen)) {
	                int	rlen = readlen ;

	                if (ulen >= 0) rlen = MIN((ulen - tlen),readlen) ;

	                rs = u_read(sfd,mdata,rlen) ;
	                wlen = rs ;
	                if (rs <= 0) break ;

	                rs = uc_writen(dfd,mdata,wlen) ;
	                tlen += rs ;

	                if (rs < 0) break ;
	            } /* end while */

#if	CF_VALLOC
	            uc_libfree(mdata) ;
#else
	            if (msize == 0) {
	                uc_libfree(mdata) ;
	            } else {
	                u_munmap(mdata,msize) ;
	            }
#endif /* CF_VALLOC */
	        } /* end if (memory allocation) */

	        tlen &= INT_MAX ;

	    } /* end if (positive) */

	    } /* ed if (uc_pagesize) */
	} /* end if (non-zero) */

	return (rs >= 0) ? tlen : rs ;
}
/* end subroutine (uc_writedesc) */

int uc_copy(int ifd,int ofd,int wlen) noex {
	return uc_writedesc(ofd,ifd,wlen) ;
}
/* end subroutine (uc_writedesc) */


