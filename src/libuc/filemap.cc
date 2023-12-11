/* filemap */
/* lang=C++20 */

/* support low-overhead file bufferring requirements */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-01, David A­D­ Morano
	This subroutine was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

        This little object supports some buffered file operations for
        low-overhead buffered I/O requirements.

*******************************************************************************/

#include	<envstandards.h>
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<sys/mman.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<climits>
#include	<cstdlib>
#include	<cstring>
#include	<algorithm>
#include	<usystem.h>
#include	<usupport.h>
#include	<sysval.hh>
#include	<localmisc.h>

#include	"filemap.h"


/* local defines */


/* local namespaces */

using std::min ;
using std::max ;


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

static int filemap_opener(filemap *,cchar *,int) noex ;
static int filemap_openmap(filemap *,int,size_t) noex ;


/* local variables */

static sysval		pagesize(sysval_ps) ;


/* exported subroutines */

int filemap_open(filemap *fmp,cchar *fname,int oflags,size_t max) noex {
	int		rs = SR_FAULT ;
	if (fmp && fname) {
	    rs = SR_INVALID ;
	    memclear(fmp) ;
	    if (fname[0]) {
	        if (max <= 0) max = ULONG_MAX ;
	        fmp->maxsize = max ;
		rs = filemap_opener(fmp,fname,oflags) ;
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (filemap_open) */

int filemap_close(filemap *fmp) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (fmp) {
	    rs = SR_OK ;
	    if (fmp->mapdata) {
	        size_t	ms = fmp->mapsize ;
	        void	*ma = fmp->mapdata ;
	        rs1 = u_munmap(ma,ms) ;
		if (rs >= 0) rs = rs1 ;
	        fmp->mapdata = nullptr ;
	        fmp->mapsize = 0 ;
	    }
	    fmp->bp = nullptr ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (filemap_close) */

int filemap_stat(filemap *fmp,USTAT *sbp) noex {
	int		rs = SR_FAULT ;
	if (fmp && sbp) {
	    rs = SR_NOTOPEN ;
	    if (fmp->mapdata) {
		*sbp = fmp->sb ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (filemap_stat) */

int filemap_read(filemap *fmp,int rlen,void *vp) noex {
	int		rs = SR_FAULT ;
	if (fmp && vp) {
	    size_t	fsize = size_t(fmp->sb.st_size) ;
	    rs = SR_NOTOPEN ;
	    if (fmp->mapdata) {
		cchar	*bdata = (cchar *) fmp->mapdata ;
	        cvoid	**bpp = (cvoid **) vp ;
		cchar	*sbp = fmp->bp ;
		if (fsize > fmp->maxsize) fsize = fmp->maxsize ;
		{
		    cchar	*ebp = (bdata + fsize) ;
		    fmp->bp = min(ebp,(sbp+rlen)) ;
		    *bpp = (void *) sbp ;
		    rs = (fmp->bp - sbp) ;
		} /* end block */
	    } /* end if (open) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (filemap_read) */

int filemap_getline(filemap *fmp,cchar **bpp) noex {
	int		rs = SR_FAULT ;
	if (fmp && bpp) {
	    size_t	fsize = size_t(fmp->sb.st_size) ;
	    rs = SR_NOTOPEN ;
	    if (fmp->mapdata) {
	        cchar	*bdata = (cchar *) fmp->mapdata ;
		cchar	*sbp = fmp->bp ;
		if (fsize > fmp->maxsize) fsize = fmp->maxsize ;
		{
		    cchar	*ebp = (bdata + fsize) ;
		    while (fmp->bp < ebp) {
	    	        if (*fmp->bp++ == '\n') break ;
		    } /* end while */
		    *bpp = sbp ;
		    rs = (fmp->bp - sbp) ;
		} /* end block */
	    } /* end if (open) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (filemap_getline) */

int filemap_seek(filemap *fmp,off_t off,int w) noex {
	int		rs = SR_FAULT ;
	if (fmp) {
	    rs = SR_NOTOPEN ;
	    if (fmp->mapdata) {
		off_t	noff = 0 ;
	        switch (w) {
	        case SEEK_SET:
		    noff = off ;
	            break ;
	        case SEEK_END:
	            noff = (fmp->sb.st_size + off) ;
	            break ;
	        case SEEK_CUR:
	            {
		        cchar	*bdata = (cchar *) fmp->mapdata ;
	                noff = ((fmp->bp - bdata) + off) ;
	            }
	            break ;
	        default:
	            rs = SR_INVALID ;
	            break ;
	        } /* end switch */
	        if (rs >= 0) {
	            cchar	*bdata = (cchar *) fmp->mapdata ;
	            if (noff < 0) {
	                rs = SR_INVALID ;
	            } else {
			off_t	moff = off_t(fmp->mapsize) ;
			if (noff > moff) {
	                    noff = moff ;
			}
		    }
	            fmp->bp = (bdata + noff) ;
	        } /* end if */
	    } /* end if (open) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (filemap_seek) */

int filemap_tell(filemap *fmp,off_t *offp) noex {
	int		rs = SR_FAULT ;
	if (fmp) {
	    rs = SR_NOTOPEN ;
	    if (fmp->mapdata) {
	        cchar	*bdata = (cchar *) fmp->mapdata ;
	        if (offp) *offp = (fmp->bp - bdata) ;
	        rs = int((fmp->bp - bdata) & INT_MAX) ;
	    } /* end if (open) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (filemap_tell) */

int filemap_rewind(filemap *fmp) noex {
	int		rs = SR_FAULT ;
	if (fmp) {
	    rs = SR_NOTOPEN ;
	    if (fmp->mapdata) {
		rs = SR_OK ;
		fmp->bp = (char *) fmp->mapdata ;
	    } /* end if (open) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (filemap_rewind) */


/* local subroutines */

static int filemap_opener(filemap *fmp,cchar *fn,int of) noex {
	csize		max = fmp->maxsize ;
	int		rs ;
	int		rs1 ;
	if ((rs = uc_open(fn,of,0666)) >= 0) {
	    USTAT	*sbp = &fmp->sb ;
	    cint	fd = rs ;
	    if ((rs = uc_fstat(fd,sbp)) >= 0) {
		if (S_ISREG(sbp->st_mode)) {
		    csize	fsize = size_t(sbp->st_size) ;
		    rs = SR_TOOBIG ;
		    if ((max > 0) && (fsize <= max)) {
			rs = filemap_openmap(fmp,fd,fsize) ;
		    } /* end if (size appropriate) */
	        } else {
	            rs = SR_PROTO ;
		}
	    } /* end if (stat) */
	    rs1 = uc_close(fd) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (file-open) */
	return rs ;
}
/* end subroutine (filemap_opener) */

static int filemap_openmap(filemap *fmp,int fd,size_t fsize) noex {
	size_t		ms ;
	int		rs ;
	if ((rs = pagesize) >= 0) {
	    nullptr_t	n{} ;
	    csize	ps = size_t(rs) ;
	    cint	mp = PROT_READ ;
	    cint	mf = MAP_SHARED ;
	    void	*md{} ;
	    ms = max(ps,fsize) ;
	    if ((rs = u_mmap(n,ms,mp,mf,fd,0L,&md)) >= 0) {
	        cint		madv = MADV_SEQUENTIAL ;
	        if ((rs = uc_madvise(md,ms,madv)) >= 0) {
	            fmp->mapdata = md ;
	            fmp->mapsize = ms ;
	            fmp->bp = (cchar *) md ;
	        }
	        if (rs < 0) {
	            u_munmap(md,ms) ;
	        }
	    } /* end if (mmap) */
	} /* end if (pagesize) */
	return rs ;
}
/* end subroutine (filemap_openmap) */


