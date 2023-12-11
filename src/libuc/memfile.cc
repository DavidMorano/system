/* memfile */
/* lang=C++20 */

/* support low-overhead file bufferring requirements */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-4-01, David A­D­ Morano
	This subroutine was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/******************************************************************************

        This little object supports some buffered file operations for
        low-overhead buffered I-O requirements.

******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<sys/mman.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<climits>
#include	<cstring>
#include	<algorithm>
#include	<usystem.h>
#include	<usupport.h>
#include	<sysval.hh>
#include	<intceil.h>
#include	<localmisc.h>

#include	"memfile.h"


/* local defines */

#define	ZEROBUFLEN	1024


/* local namespaces */

using std::min ;
using std::max ;


/* external subroutines */


/* external variables */


/* local typedefs */


/* local structures */


/* forward references */

static int	memfile_openmap(memfile *,int,size_t) noex ;
static int	memfile_extend(memfile *) noex ;
static int	memfile_mapextend(memfile *,uint) noex ;
static int	memfile_ismemfree(memfile *,caddr_t,uint) noex ;


/* local variables */

static sysval		pagesize(sysval_ps) ;


/* exported subroutines */

int memfile_open(memfile *fbp,cchar *fname,int of,mode_t om) noex {
	int		rs = SR_FAULT ;
	if (fbp && fname) {
	    rs = SR_INVALID ;
	    memclear(fbp) ;
	    if (fname[0]) {
	        of &= (~ (O_RDONLY | O_WRONLY)) ;
	        of |= O_RDWR ;
	        if ((rs = uc_open(fname,of,om)) >= 0) {
	            USTAT	sb ;
	            cint	fd = rs ;
	            if ((rs = u_fstat(fd,&sb)) >= 0) {
	                if (S_ISREG(sb.st_mode)) {
		            csize	fsize = size_t(sb.st_size) ;
		            if ((rs = pagesize) >= 0) {
			        fbp->pagesize = rs ;
	                        if ((rs = uc_closeonexec(fd,true)) >= 0) {
		                    rs = memfile_openmap(fbp,fd,fsize) ;
			        }
		            } /* end if (pagesize) */
	                } else {
	                    rs = SR_PROTO ;
		        }
	            } /* end if (stat) */
	            if (rs < 0) {
	                u_close(fd) ;
		        fbp->fd = -1 ;
	            }
	        } /* end if (file-open) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (memfile_open) */

int memfile_close(memfile *fbp) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (fbp) {
	    rs = SR_NOTOPEN ;
	    if (fbp->magic == MEMFILE_MAGIC) {
		rs = SR_OK ;
	        if (fbp->fd >= 0) {
	            rs1 = u_close(fbp->fd) ;
	            if (rs >= 0) rs = rs1 ;
	            fbp->fd = -1 ;
	        }
	        if (fbp->dbuf) {
	            rs1 = u_munmap(fbp->dbuf,fbp->dlen) ;
	            if (rs >= 0) rs = rs1 ;
	            fbp->dbuf = nullptr ;
	            fbp->dlen = 0 ;
	        }
	        fbp->magic = 0 ;
	    } /* end if (open) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (memfile_close) */

int memfile_write(memfile *fbp,cvoid *wbuf,int wlen) noex {
	int		rs = SR_FAULT ;
	if (fbp && wbuf) {
	    csize	fsize = fbp->fsize ;
	    cint	ps = fbp->pagesize ;
	    rs = SR_NOTOPEN ;
	    if ((fbp->magic == MEMFILE_MAGIC) && fbp->dbuf) {
	        csize	p = (4 * ps) ;
		uint	pmo = (ps - 1) ;
		rs = SR_OK ;
		if ((fbp->off + wlen) > fsize) {
		    csize	a = szceil(fsize,ps) ;
	    	    if ((fbp->off + wlen) > a) {
	        	csize e = max(((fbp->off + wlen) - a),p) ;
	                if ((rs = memfile_mapextend(fbp,e)) >= 0) {
		            char	zbuf[2] = {} ;
	                    for (size_t off = a ; off < (a + e) ; off += ps) {
	                        rs = u_pwrite(fbp->fd,zbuf,1,(off + pmo)) ;
	                        if (rs < 0) break ;
	                    } /* end for */
	                } /* end if (extending file) */
	            } /* end if (extending map and file) */
	            if (rs >= 0) {
	                csize	extra = ((fbp->off + wlen) - fsize) ;
	                fbp->fsize += extra ;
	            }
	        } /* end if (writing beyond file end) */
	        if (rs >= 0) {
	            if ((rs = u_pwrite(fbp->fd,wbuf,wlen,fbp->off)) >= 0) {
	                if ((fbp->off + wlen) > fsize) {
	                    fbp->fsize = (fbp->off + wlen) ;
			}
	            }
	            fbp->off += wlen ;
	        } /* end if (ok) */
	    } /* end if (open) */
	} /* end if (non-null) */
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (memfile_write) */

int memfile_len(memfile *fbp) noex {
	int		rs = SR_FAULT ;
	if (fbp) {
	    rs = SR_NOTOPEN ;
	    if (fbp->magic == MEMFILE_MAGIC) {
		rs = fbp->fsize ;
	    } /* end if (open) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (memfile_len) */

int memfile_allocation(memfile *fbp) noex {
	int		rs = SR_FAULT ;
	if (fbp) {
	    rs = SR_NOTOPEN ;
	    if (fbp->magic == MEMFILE_MAGIC) {
		rs = fbp->dlen ;
	    } /* end if (open) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (memfile_allocation) */

int memfile_tell(memfile *fbp,off_t *offp) noex {
	int		rs = SR_FAULT ;
	if (fbp) {
	    rs = SR_NOTOPEN ;
	    if (fbp->magic == MEMFILE_MAGIC) {
		rs = int(fbp->off) ;
		if (offp) *offp = fbp->off ;
	    } /* end if (open) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (memfile_tell) */

int memfile_buf(memfile *fbp,void *vp) noex {
	caddr_t		*rpp = (caddr_t *) vp ;
	int		rs = SR_FAULT ;
	if (fbp && vp) {
	    rs = SR_NOTOPEN ;
	    if (fbp->magic == MEMFILE_MAGIC) {
		*rpp = caddr_t(fbp->dbuf) ;
	    } /* end if (open) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (memfile_buf) */


/* private subroutines */

static int memfile_openmap(memfile *fbp,int fd,size_t fsize) noex {
	csize		ms = szceil(fsize,fbp->pagesize) ;
	cint		mp = (PROT_READ | PROT_WRITE) ;
	cint		mf = MAP_SHARED ;
	int		rs ;
	void		*md ;
	fbp->off = 0 ;
	fbp->fd = fd ;
	if ((rs = u_mmapbegin(nullptr,ms,mp,mf,fd,0L,&md)) >= 0) {
	    fbp->fsize = fsize ;
	    fbp->dbuf = (char *) md ;
	    fbp->dlen = ms ;
	    if ((rs = memfile_extend(fbp)) >= 0) {
	        fbp->bp = fbp->dbuf ;
		fbp->magic = MEMFILE_MAGIC ;
	    }
	    if (rs < 0) {
		u_munmap(fbp->dbuf,fbp->dlen) ;
		fbp->dbuf = nullptr ;
		fbp->dlen = 0 ;
	    } /* end if (error) */
	} /* end if (u_mmapbegin) */
	return rs ;
}
/* end subroutine (memfile_openmap) */

static int memfile_extend(memfile *fbp) noex {
	size_t		off = fbp->fsize ;
	cint		zlen = ZEROBUFLEN ;
	int		rs = SR_OK ;
	char		zbuf[ZEROBUFLEN + 1] = {} ;
	while (off < fbp->dlen) {
	    csize	zsize = size_t(zlen) ;
	    size_t	clen, dlen ;
	    if ((off % zlen) == 0) {
	        clen = zsize ;
	    } else {
	        clen = szceil(off,zlen) ;
	    }
	    dlen = min(zsize,(clen - fbp->fsize)) ;
	    rs = u_pwrite(fbp->fd,zbuf,dlen,off) ;
	    off += dlen ;
	    if (rs < 0) break ;
	} /* end for */
	return rs ;
}
/* end subroutine (memfile_extend) */

static int memfile_mapextend(memfile *fbp,uint extension) noex {
	off_t		moff ;
	caddr_t		addr ;
	size_t		msize ;
	cint		mprot = (PROT_READ | PROT_WRITE) ;
	cint		mflags = MAP_SHARED ;
	int		rs = SR_INVALID ;

/* first we try to extend our existing map */

	addr = (fbp->dbuf + fbp->dlen) ;
	if (memfile_ismemfree(fbp,addr,extension) > 0) {
	    int		fd = fbp->fd ;
	    void	*p ;

	    moff = fbp->dlen ;
	    msize = extension ;
	    if ((rs = u_mmap(addr,msize,mprot,mflags,fd,moff,&p)) >= 0) {
	        fbp->dbuf = (char *) p ;
	        fbp->dlen += extension ;
	    }

	} /* end if */

/* do we need to remap entirely? */

	if (rs < 0) {

	    if ((rs = uc_fdatasync(fbp->fd)) >= 0) {
	        msize = fbp->dlen ;
	        rs = u_munmap(fbp->dbuf,msize) ;
	        fbp->dbuf = nullptr ;
	    }

	    if (rs >= 0) {
	        cint	fd = fbp->fd ;
	        void	*p ;

	        msize = (fbp->dlen + extension) ;
	        fbp->dlen = 0 ;
	        if ((rs = u_mmap(nullptr,msize,mprot,mflags,fd,0L,&p)) >= 0) {
	            fbp->dbuf = (char *) p ;
	            fbp->dlen = msize ;
	        }

	    } /* end if */

	} /* end if */

	return rs ;
}
/* end subroutine (memfile_mapextend) */

static int memfile_ismemfree(memfile *fbp,caddr_t addr,uint len) noex {
	size_t		tlen = fbp->pagesize ;
	cint		ps = dbp->pagesize ;
	int		rs = SR_NOMEM ;
	char		vec[10] ;
	for (caddr_t ta = addr ; ta < (addr + len) ; ta += ps) {
	    rs = u_mincore(ta,tlen,vec) ;
	    if (rs != SR_NOMEM) break ;
	} /* end for */
	return (rs == SR_NOMEM) ? true : SR_EXIST ;
}
/* end subroutine (memfile_ismemfree) */


