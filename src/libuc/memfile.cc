/* memfile SUPPORT */
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
#include	<intsat.h>
#include	<intceil.h>
#include	<localmisc.h>

#include	"memfile.h"


/* local defines */

#define	ZEROBUFLEN	1024


/* local namespaces */

using std::min ;			/* subroutine-template */
using std::max ;			/* subroutine-template */


/* external subroutines */


/* external variables */


/* local typedefs */


/* local structures */


/* forward references */

template<typename ... Args>
static inline int memfile_ctor(memfile *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = SR_OK ;
	    op->magic = 0 ;
	    op->dbuf = nullptr ;
	    op->bp = nullptr ;
	    op->off = 0 ;
	    op->dlen = 0 ;
	    op->fsize = 0 ;
	    op->fd = -1 ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (memfile_ctor) */

static int memfile_dtor(memfile *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	}
	return rs ;
}
/* end subroutine (memfile_dtor) */

template<typename ... Args>
static inline int memfile_magic(memfile *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magic == MEMFILE_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}
/* end subroutine (memfile_magic) */

static int	memfile_opener(memfile *,cchar *,int,mode_t) noex ;
static int	memfile_openmap(memfile *,int,size_t) noex ;
static int	memfile_extend(memfile *) noex ;
static int	memfile_mapextend(memfile *,size_t) noex ;
static int	memfile_ismemfree(memfile *,caddr_t,size_t) noex ;


/* local variables */

static sysval		pagesize(sysval_ps) ;


/* exported subroutines */

int memfile_open(memfile *op,cchar *fname,int of,mode_t om) noex {
	int		rs = SR_FAULT ;
	if ((rs = memfile_ctor(op,fname)) >= 0) {
	    rs = SR_INVALID ;
	    if (fname[0]) {
	 	if ((rs = memfile_opener(op,fname,of,om)) >= 0) {
		    op->magic = MEMFILE_MAGIC ;
		}
	    } /* end if (valid) */
	    if (rs < 0) {
		memfile_dtor(op) ;
	    }
	} /* end if (memfile_ctor) */
	return rs ;
}
/* end subroutine (memfile_open) */

int memfile_close(memfile *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = memfile_magic(op)) >= 0) {
	        if (op->fd >= 0) {
	            rs1 = uc_close(op->fd) ;
	            if (rs >= 0) rs = rs1 ;
	            op->fd = -1 ;
	        }
	        if (op->dbuf) {
	            rs1 = u_munmap(op->dbuf,op->dlen) ;
	            if (rs >= 0) rs = rs1 ;
	            op->dbuf = nullptr ;
	            op->dlen = 0 ;
	        }
		{
		    rs1 = memfile_dtor(op) ;
	            if (rs >= 0) rs = rs1 ;
		}
	    op->magic = 0 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (memfile_close) */

int memfile_write(memfile *op,cvoid *wbuf,int wlen) noex {
	int		rs ;
	if ((rs = memfile_magic(op,wbuf)) >= 0) {
	    csize	fsize = op->fsize ;
	    cint	ps = op->pagesize ;
	    rs = SR_NOTOPEN ;
	    if (op->dbuf) {
	        csize	p = (4 * ps) ;
		uint	pmo = (ps - 1) ;
		rs = SR_OK ;
		if ((op->off + wlen) > fsize) {
		    csize	a = szceil(fsize,ps) ;
	    	    if ((op->off + wlen) > a) {
	        	csize e = max(((op->off + wlen) - a),p) ;
	                if ((rs = memfile_mapextend(op,e)) >= 0) {
		            char	zbuf[2] = {} ;
	                    for (size_t off = a ; off < (a + e) ; off += ps) {
	                        rs = u_pwrite(op->fd,zbuf,1,(off + pmo)) ;
	                        if (rs < 0) break ;
	                    } /* end for */
	                } /* end if (extending file) */
	            } /* end if (extending map and file) */
	            if (rs >= 0) {
	                csize	extra = ((op->off + wlen) - fsize) ;
	                op->fsize += extra ;
	            }
	        } /* end if (writing beyond file end) */
	        if (rs >= 0) {
	            if ((rs = u_pwrite(op->fd,wbuf,wlen,op->off)) >= 0) {
	                if ((op->off + wlen) > fsize) {
	                    op->fsize = (op->off + wlen) ;
			}
	            }
	            op->off += wlen ;
	        } /* end if (ok) */
	    } /* end if (open) */
	} /* end if (magic) */
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (memfile_write) */

int memfile_len(memfile *op) noex {
	int		rs ;
	if ((rs = memfile_magic(op)) >= 0) {
	    rs = intsat(op->fsize) ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (memfile_len) */

int memfile_allocation(memfile *op) noex {
	int		rs ;
	if ((rs = memfile_magic(op)) >= 0) {
	    rs = intsat(op->dlen) ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (memfile_allocation) */

int memfile_tell(memfile *op,off_t *offp) noex {
	int		rs ;
	if ((rs = memfile_magic(op)) >= 0) {
	    rs = intsat(op->off) ;
	    if (offp) *offp = op->off ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (memfile_tell) */

int memfile_buf(memfile *op,void *vp) noex {
	caddr_t		*rpp = (caddr_t *) vp ;
	int		rs ;
	if ((rs = memfile_magic(op,vp)) >= 0) {
	    *rpp = caddr_t(op->dbuf) ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (memfile_buf) */


/* private subroutines */

static int memfile_opener(memfile *op,cchar *fname,int of,mode_t om) noex {
	int		rs ;
	of &= (~ (O_RDONLY | O_WRONLY)) ;
	of |= O_RDWR ;
        if ((rs = uc_open(fname,of,om)) >= 0) {
            USTAT       sb ;
            cint        fd = rs ;
            if ((rs = uc_fstat(fd,&sb)) >= 0) {
                if (S_ISREG(sb.st_mode)) {
                    csize       fsize = size_t(sb.st_size) ;
                    if ((rs = pagesize) >= 0) {
                        op->pagesize = rs ;
                        if ((rs = uc_closeonexec(fd,true)) >= 0) {
                            rs = memfile_openmap(op,fd,fsize) ;
                        }
                    } /* end if (pagesize) */
                } else {
                    rs = SR_PROTO ;
                }
            } /* end if (stat) */
            if (rs < 0) {
                uc_close(fd) ;
                op->fd = -1 ;
            }
        } /* end if (file-open) */
	return rs ;
}
/* end subroutine (memfile_opener) */

static int memfile_openmap(memfile *op,int fd,size_t fsize) noex {
	csize		ms = szceil(fsize,op->pagesize) ;
	cint		mp = (PROT_READ | PROT_WRITE) ;
	cint		mf = MAP_SHARED ;
	int		rs ;
	void		*md{} ;
	op->off = 0 ;
	op->fd = fd ;
	if ((rs = u_mmapbegin(nullptr,ms,mp,mf,fd,0L,&md)) >= 0) {
	    op->fsize = fsize ;
	    op->dbuf = (char *) md ;
	    op->dlen = ms ;
	    if ((rs = memfile_extend(op)) >= 0) {
	        op->bp = op->dbuf ;
	    }
	    if (rs < 0) {
		u_munmap(op->dbuf,op->dlen) ;
		op->dbuf = nullptr ;
		op->dlen = 0 ;
	    } /* end if (error) */
	} /* end if (u_mmapbegin) */
	return rs ;
}
/* end subroutine (memfile_openmap) */

static int memfile_extend(memfile *op) noex {
	size_t		off = op->fsize ;
	cint		zlen = ZEROBUFLEN ;
	int		rs = SR_OK ;
	char		zbuf[ZEROBUFLEN + 1] = {} ;
	while (off < op->dlen) {
	    csize	zsize = size_t(zlen) ;
	    size_t	clen, dlen ;
	    if ((off % zlen) == 0) {
	        clen = zsize ;
	    } else {
	        clen = szceil(off,zlen) ;
	    }
	    dlen = min(zsize,(clen - op->fsize)) ;
	    rs = u_pwrite(op->fd,zbuf,dlen,off) ;
	    off += dlen ;
	    if (rs < 0) break ;
	} /* end for */
	return rs ;
}
/* end subroutine (memfile_extend) */

static int memfile_mapextend(memfile *op,size_t ext) noex {
	off_t		mo = 0 ;
	caddr_t		addr = (op->dbuf + op->dlen) ;
	size_t		ms ;
	cint		mp = (PROT_READ | PROT_WRITE) ;
	cint		mf = MAP_SHARED ;
	int		rs = SR_INVALID ;
/* first we try to extend our existing map */
	if (memfile_ismemfree(op,addr,ext) > 0) {
	    int		fd = op->fd ;
	    void	*md{} ;
	    mo = op->dlen ;
	    ms = ext ;
	    if ((rs = u_mmap(addr,ms,mp,mf,fd,mo,&md)) >= 0) {
	        op->dbuf = (char *) md ;
	        op->dlen += ms ;
	    }
	} /* end if */
/* do we need to remap entirely? */
	if (rs < 0) {
	    if ((rs = uc_fdatasync(op->fd)) >= 0) {
	        ms = op->dlen ;
	        rs = u_munmap(op->dbuf,ms) ;
	        op->dbuf = nullptr ;
	    }
	    if (rs >= 0) {
	        cint	fd = op->fd ;
	        void	*md{} ;
	        ms = (op->dlen + ext) ;
	        op->dlen = 0 ;
	        if ((rs = u_mmap(nullptr,ms,mp,mf,fd,0L,&md)) >= 0) {
	            op->dbuf = (char *) md ;
	            op->dlen = ms ;
	        }
	    } /* end if */
	} /* end if */
	return rs ;
}
/* end subroutine (memfile_mapextend) */

static int memfile_ismemfree(memfile *op,caddr_t addr,size_t ext) noex {
	size_t		tlen = op->pagesize ;
	cint		ps = op->pagesize ;
	int		rs = SR_NOMEM ;
	char		vec[10] ;
	for (caddr_t ta = addr ; ta < (addr + ext) ; ta += ps) {
	    rs = u_mincore(ta,tlen,vec) ;
	    if (rs != SR_NOMEM) break ;
	} /* end for */
	return (rs == SR_NOMEM) ? true : SR_EXIST ;
}
/* end subroutine (memfile_ismemfree) */


