/* filemap SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* read-file funtion through file-mapping */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	filemap

	Description:
        This little object supports some buffered file operations for
        low-overhead buffered I-O operations (read-only).

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<sys/mman.h>
#include	<unistd.h>		/* |off_t| */
#include	<fcntl.h>
#include	<climits>		/* |ULONG_MAX| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<usystem.h>
#include	<sysval.hh>
#include	<localmisc.h>

#include	"filemap.h"


/* local defines */


/* imported namespaces */

using std::nullptr_t ;			/* type */
using std::min ;			/* subroutine-template */
using std::max ;			/* subroutine-template */
using std::nothrow ;			/* constant */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

template<typename ... Args>
static int filemap_ctor(filemap *op,Args ... args) noex {
    	FILEMAP		*hop = op ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    cnullptr	np{} ;
	    rs = SR_NOMEM ;
	    memclear(hop) ;
	    if ((op->sbp = new(nothrow) USTAT) != np) {
		rs = SR_OK ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (filemap_ctor) */

static int filemap_dtor(filemap *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	    if (op->sbp) {
		delete op->sbp ;
		op->sbp = nullptr ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (filemap_dtor) */

static int filemap_opener(filemap *,cchar *) noex ;
static int filemap_openmap(filemap *,int,size_t) noex ;


/* local variables */

static sysval		pagesize(sysval_ps) ;


/* exported variables */


/* exported subroutines */

int filemap_open(filemap *op,cchar *fname,size_t nmax) noex {
	int		rs ;
	if ((rs = filemap_ctor(op,fname)) >= 0) {
	    rs = SR_INVALID ;
	    if (fname[0]) {
	        if (nmax <= 0) nmax = ULONG_MAX ;
	        op->maxsize = nmax ;
		rs = filemap_opener(op,fname) ;
	    } /* end if (valid) */
	    if (rs < 0) {
		filemap_dtor(op) ;
	    }
	} /* end if (filemap_ctor) */
	return rs ;
}
/* end subroutine (filemap_open) */

int filemap_close(filemap *op) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (op) {
	    rs = SR_OK ;
	    if (op->mapdata) {
	        csize	ms = op->mapsize ;
	        void	*ma = op->mapdata ;
	        rs1 = u_mmapend(ma,ms) ;
		if (rs >= 0) rs = rs1 ;
	        op->mapdata = nullptr ;
	        op->mapsize = 0 ;
	    }
	    {
		rs1 = filemap_dtor(op) ;
		if (rs >= 0) rs = rs1 ;
	    }
	    op->bp = nullptr ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (filemap_close) */

int filemap_stat(filemap *op,USTAT *sbp) noex {
	int		rs = SR_FAULT ;
	if (op && sbp) {
	    rs = SR_NOTOPEN ;
	    if (op->mapdata) {
		*sbp = *op->sbp ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (filemap_stat) */

int filemap_read(filemap *op,int rlen,void *vp) noex {
	int		rs = SR_FAULT ;
	if (op && vp) {
	    USTAT	*sbp = op->sbp ;
	    size_t	fsize = size_t(sbp->st_size) ;
	    rs = SR_NOTOPEN ;
	    if (op->mapdata) {
		cchar	*bdata = charp(op->mapdata) ;
	        cvoid	**bpp = (cvoid **) vp ;
		cchar	*sbp = op->bp ;
		if (fsize > op->maxsize) fsize = op->maxsize ;
		{
		    cchar	*ebp = (bdata + fsize) ;
		    op->bp = min(ebp,(sbp+rlen)) ;
		    *bpp = voidp(sbp) ;
		    rs = (op->bp - sbp) ;
		} /* end block */
	    } /* end if (open) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (filemap_read) */

int filemap_getln(filemap *op,cchar **bpp) noex {
	size_t		fsize ;
	int		rs = SR_FAULT ;
	if (op && bpp) {
	    USTAT	*sbp = op->sbp ;
	    fsize = size_t(sbp->st_size) ;
	    rs = SR_NOTOPEN ;
	    if (op->mapdata) {
	        cchar	*bdata = charp(op->mapdata) ;
		cchar	*sbp = op->bp ;
		if (fsize > op->maxsize) fsize = op->maxsize ;
		{
		    cchar	*ebp = (bdata + fsize) ;
		    while (op->bp < ebp) {
	    	        if (*op->bp++ == '\n') break ;
		    } /* end while */
		    *bpp = sbp ;
		    rs = (op->bp - sbp) ;
		} /* end block */
	    } /* end if (open) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (filemap_getln) */

int filemap_seek(filemap *op,off_t off,int w) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_NOTOPEN ;
	    if (op->mapdata) {
	        cchar	*bdata = charp(op->mapdata) ;
		off_t	noff = 0 ;
	        switch (w) {
	        case SEEK_SET:
		    noff = off ;
	            break ;
	        case SEEK_END:
		    {
			USTAT	*sbp = op->sbp ;
	                noff = (sbp->st_size + off) ;
		    }
	            break ;
	        case SEEK_CUR:
	            noff = ((op->bp - bdata) + off) ;
	            break ;
	        default:
	            rs = SR_INVALID ;
	            break ;
	        } /* end switch */
	        if (rs >= 0) {
	            if (noff < 0) {
	                rs = SR_INVALID ;
	            } else {
			off_t	moff = off_t(op->mapsize) ;
			if (noff > moff) {
	                    noff = moff ;
			}
		    }
	            op->bp = (bdata + noff) ;
	        } /* end if */
	    } /* end if (open) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (filemap_seek) */

int filemap_tell(filemap *op,off_t *offp) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_NOTOPEN ;
	    if (op->mapdata) {
	        cchar	*bdata = charp(op->mapdata) ;
	        if (offp) *offp = (op->bp - bdata) ;
	        rs = intsat(op->bp - bdata) ;
	    } /* end if (open) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (filemap_tell) */

int filemap_rewind(filemap *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_NOTOPEN ;
	    if (op->mapdata) {
		rs = SR_OK ;
		op->bp = charp(op->mapdata) ;
	    } /* end if (open) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (filemap_rewind) */


/* local subroutines */

static int filemap_opener(filemap *op,cchar *fn) noex {
	csize		nmax = op->maxsize ;
	cint		of = (O_RDONLY | O_CLOEXEC) ;
	int		rs ;
	int		rs1 ;
	if ((rs = uc_open(fn,of,0666)) >= 0) {
	    cint	fd = rs ;
	    if (USTAT *sbp = op->sbp ; (rs = uc_fstat(fd,sbp)) >= 0) {
		if (S_ISREG(sbp->st_mode)) {
		    csize	fsize = size_t(sbp->st_size) ;
		    rs = SR_TOOBIG ;
		    if ((nmax > 0) && (fsize <= nmax)) {
			rs = filemap_openmap(op,fd,fsize) ;
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

static int filemap_openmap(filemap *op,int fd,size_t fsize) noex {
	cnullptr	np{} ;
	size_t		ms ;
	int		rs ;
	if ((rs = pagesize) >= 0) {
	    csize	ps = size_t(rs) ;
	    cint	mp = PROT_READ ;
	    cint	mf = MAP_SHARED ;
	    void	*md{} ;
	    ms = max(ps,fsize) ;
	    if ((rs = u_mmapbegin(np,ms,mp,mf,fd,0z,&md)) >= 0) {
	        cint	madv = MADV_SEQUENTIAL ;
	        if ((rs = u_madvise(md,ms,madv)) >= 0) {
	            op->mapdata = md ;
	            op->mapsize = ms ;
	            op->bp = charp(md) ;
	        }
	        if (rs < 0) {
	            u_mmapend(md,ms) ;
	        }
	    } /* end if (mmap) */
	} /* end if (pagesize) */
	return rs ;
}
/* end subroutine (filemap_openmap) */

filemap_co::operator int () noex {
	int		rs = SR_BUGCHECK ;
	if (op) {
	    switch (w) {
	    case filemapmem_rewind:
	        rs = filemap_rewind(op) ;
	        break ;
	    case filemapmem_close:
	        rs = filemap_close(op) ;
	        break ;
	    } /* end switch */
	} /* end if (non-null) */
	return rs ;
}
/* end method (filemap_co::operator) */

int filemap_teller::operator () (off_t *fop) noex {
	int		rs = SR_BUGCHECK ;
	if (op) {
	    switch (w) {
	    case 0:
	        rs = filemap_tell(op,fop) ;
	        break ;
	    } /* end switch */
	} /* end if (non-null) */
	return rs ;
}
/* end method (filemap_teller::operator) */

int filemap::open(cchar *fn,size_t fsz) noex {
    	return filemap_open(this,fn,fsz) ;
}

int filemap::stat(USTAT *sbp) noex {
    	return filemap_stat(this,sbp) ;
}

int filemap::read(int rlen,void *rbuf) noex {
    	return filemap_read(this,rlen,rbuf) ;
}

int filemap::getln(cchar **cpp) noex {
    	return filemap_getln(this,cpp) ;
}

int filemap::seek(off_t fo,int w) noex {
    	return filemap_seek(this,fo,w) ;
}

void filemap::dtor() noex {
	if (cint rs = close ; rs < 0) {
	    ulogerror("filemap",rs,"fini-close") ;
	}
}


