/* ucwritedesc SUPPORT */
/* charset=ISO8859-1 */
/* version %I% last-modified %G% */

/* interface component for UNIX® library-3c */
/* copy from one file descriptor to another */


/* revision history:

	= 1998-01-10, David A­D­ Morano
	The subroutine was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

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

	Notes:
	1. I am intentionally cheap on the initialization of the
	'var' variable.  There is no initialization lock on this
	variable below (intentionally).  So each thread might have
	to initialize the variable 'pagesz' itself.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/stat.h>
#include	<sys/mman.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<climits>		/* |INT_MAX| + |SIZE_MAX| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<usystem.h>
#include	<getbufsize.h>
#include	<filetype.h>		/* |filetype(3u)| */
#include	<localmisc.h>


/* local defines */


/* local namespaces */

using std::min ;			/* subroutine-template */
using std::max ;			/* subroutine-template */


/* local typedefs */


/* external subroutines */

extern "C" {
    int uc_mmapbegin(void *,size_t,int,int,int,off_t,void **) noex ;
    int uc_mmapend(void *,size_t) noex ;
    int uc_madvise(void *,size_t,int) noex ;
}


/* external variables */


/* local structures */

namespace {
    struct writer {
	int		dfd ;
	int		sfd ;
	int		ulen ;
	writer(int d,int s,int l) noex : dfd(d), sfd(s), ulen(l) { } ;
	operator int() noex ;
	int copymap(size_t) noex ;
	int copybuf(int) noex ;
	int copymaper(off_t,size_t) noex ;
	int bufsize(int) noex ;
    } ; /* end struct (writer) */
    struct vars {
	int		pagesz ;
	int		msglen ;
	int		maxnamelen ;
	operator int () noex ;
    } ; /* end struct (vars) */
} /* end namespace */


/* forward references */


/* local variables */

static vars		var ;


/* exported variables */


/* exported subroutines */

int uc_writedesc(int dfd,int sfd,int ulen) noex {
    	int		rs = SR_BADF ;
	int		tlen = 0 ;
	if ((dfd >= 0) && (sfd >= 0)) {
	    if ((rs = var) >= 0) {
	        writer wo(dfd,sfd,ulen) ;
	        rs = wo ;
		tlen = rs ;
	    } /* end if (vars) */
	} /* end if (valid) */
	return (rs >= 0) ? tlen : rs ;
} /* end subroutine (uc_writedesc) */


/* local subroutines */

writer::operator int () noex {
    	int		rs ;
	int		tlen = 0 ; /* return-value */
	if (ustat sb ; (rs = uc_fstat(sfd,&sb)) >= 0) ylikely {
	    cint	ft = filetype(sb.st_mode) ;
	    if (S_ISREG(sb.st_mode) || S_ISBLK(sb.st_mode)) {
	        if (csize fsize = size_t(sb.st_size) ; fsize > 0) {
		    rs = copymap(fsize) ;
		    tlen = rs ;
	        } /* end if (non-zero positive) */
	    } else {
		rs = copybuf(ft) ;
		tlen = rs ;
	    } /* end if */
	} /* end if (uc_fstat) */
	return (rs >= 0) ? tlen : rs ;
} /* end method (writer::operator) */

int writer::copymap(size_t fsize) noex {
    	int		rs ;
	int		tlen = 0 ; /* return-value */
	if (off_t fo ; (rs = uc_tell(sfd,&fo)) >= 0) ylikely {
	    csize	osize = size_t(fo) ;
	    if (csize rsize = (fsize - osize) ; rsize > 0) ylikely {
		size_t ms = rsize ;
		if (ulen > 0) {
		    csize usize = size_t(ulen) ;
		    ms = min(rsize,usize) ;
		}
		if (ms > 0) ylikely {
	            rs = copymaper(fo,ms) ;
		    tlen = rs ;
		} /* end if (non-zero positive) */
	    } /* end if (non-zero positive) */
	} /* end if (uc_tell) */
    	return (rs >= 0) ? tlen : rs ;
} /* end method (writer::copymap) */

int writer::copymaper(off_t fo,size_t ms) noex {
        cnullptr        np{} ;
    	csize		ps = size_t(4 * var.pagesz) ;
    	cint		fd = sfd ;
        cint            mp = PROT_READ ;
        cint            mf = MAP_SHARED ;
        int             rs ;
	int		rs1 ;
        int             tlen = 0 ; /* return-value */
        if (void *md ; (rs = uc_mmapbegin(np,ms,mp,mf,fd,fo,&md)) >= 0) {
            cint        cmd = MADV_SEQUENTIAL ;
            if ((rs = uc_madvise(md,ms,cmd)) >= 0) ylikely {
		size_t	lsize = 0 ;
                cchar   *lp = charp(md) ;
                for (size_t ll = ms ; ll > 0 ; ) {
		    csize llen = min(ll,ps) ;
		    {
		        cint wlen = int(llen) ;
		       	rs = uc_write(dfd,lp,wlen) ;
		        lsize += rs ;
		    }
		    ll -= rs ;
		    lp += rs ;
		    if (rs < 0) break ;
		} /* end for */
		tlen = intsat(lsize) ;
            } /* end if (memory-advise) */
            rs1 = uc_mmapend(md,ms) ;
            if (rs >= 0) rs = rs1 ;
        } /* end if (map-file) */
	return (rs >= 0) ? tlen : rs ;
} /* end method (writer::copymaper) */

int writer::copybuf(int ft) noex {
    	int		rs ;
	int		rs1 ;
	int		tlen = 0 ;
	if ((rs = bufsize(ft)) >= 0) {
	    size_t	ll = (ulen > 0) ? size_t(ulen) : SIZE_MAX ;
	    char	*bbuf ;
	    if (cint blen = rs ; (rs = uc_libvalloc(blen,&bbuf)) >= 0) {
		csize	bsize = size_t(blen) ;
		size_t	lsize = 0 ;
		cauto read = [&,this] () noex {
		    int		lrs = SR_OK ;
		    if (csize len = min(ll,bsize) ; len > 0) {
		        cint rsz = int(len) ;
		        lrs = uc_read(sfd,bbuf,rsz) ;
		    }
		    return lrs ; 
		} ; /* end lambda (read) */
		while ((rs = read()) > 0) {
		    rs = uc_write(dfd,bbuf,rs) ;
		    ll -= rs ;
		    lsize += rs ;
		    if (rs < 0) break ;
		} /* end for */
		tlen = intsat(lsize) ;
		rs1 = uc_libfree(bbuf) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (m-a-f) */
	} /* end if (bufsize) */
	return (rs >= 0) ? tlen : rs ;
} /* end method (writer::copymaper) */

int writer::bufsize(int ft) noex {
    	int		rs = SR_OK ;
	int		bsz = var.pagesz ;
	switch (ft) {
	case DT_CHR:
	case DT_FIFO:
	    bsz = var.msglen ;
	    break ;
	case DT_SOCK:
	    bsz = (2 * var.pagesz) ;
	    break ;
	case DT_LNK:
	    bsz = var.maxnamelen ;
	    break ;
	default:
	    bsz = var.pagesz ;
	    break ;
	} /* end switch */
	return (rs >= 0) ? bsz : rs ;
} /* end method (writer::bufsize) */

vars::operator int () noex {
    	int		rs ;
	if ((rs = pagesz) == 0) {
	    if ((rs = ucpagesize) >= 0) {
	        pagesz = rs ;
		if ((rs = getbufsize(getbufsize_mm)) >= 0) {
		    msglen = rs ;
		    if ((rs = getbufsize(getbufsize_mn)) >= 0) {
			maxnamelen = rs ;
		    }
		}
	    }
	}
	return rs ;
} /* end method (vars::operator) */


