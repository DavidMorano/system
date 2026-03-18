/* memfile SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* provides a memory-mapped file for writing */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-4-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/******************************************************************************

  	Object:
	memfile

	Description:
	This little object provides a file writing facility for
	low-overhead raw writes (no separate user-space buffering).

	Notes:
	1. Comparison to other mapped-memory file facilities:
	This object allows for writing to the mapped file, while
	several other mapped-file facilities only allow for reading.

******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<sys/mman.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<clanguage.h>
#include	<usysbase.h>
#include	<usyscalls.h>
#include	<sysval.hh>
#include	<intsat.h>
#include	<intceil.h>
#include	<localmisc.h>

#include	"memfile.h"


/* local defines */

#define	ZEROBUFLEN	32
#define	PSZMULT		4


/* imported namespaces */

using std::min ;			/* subroutine-template */
using std::max ;			/* subroutine-template */


/* local typedefs */

typedef size_t		sz ;


/* external subroutines */

extern "C" {
    extern int uc_open(cchar *,int,mode_t) noex ;
    extern int uc_fstat(int,ustat *) noex ;
    extern int uc_fdatasync(int) noex ;
    extern int uc_close(int) ;
} /* end extern */


/* external variables */


/* local typedefs */


/* local structures */


/* forward references */

template<typename ... Args>
local inline int memfile_ctor(memfile *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
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

local inline int memfile_dtor(memfile *op) noex {
	int		rs = SR_FAULT ;
	if (op) ylikely {
	    rs = SR_OK ;
	}
	return rs ;
}
/* end subroutine (memfile_dtor) */

template<typename ... Args>
local inline int memfile_magic(memfile *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    rs = (op->magic == MEMFILE_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}
/* end subroutine (memfile_magic) */

local int	memfile_opener(memfile *,cchar *,int,mode_t) noex ;
local int	memfile_openmap(memfile *,size_t) noex ;
local int	memfile_mapbegin(memfile *,size_t,size_t) noex ;
local int	memfile_mapend(memfile *) noex ;
local int	memfile_extend(memfile *) noex ;
local int	memfile_mapextend(memfile *,size_t) noex ;
local int	memfile_memfree(memfile *,caddr_t,size_t) noex ;


/* local variables */

static sysval		pagesz(sysval_ps) ;


/* exported variables */


/* exported subroutines */

int memfile_open(memfile *op,cchar *fname,int of,mode_t om) noex {
	int		rs ;
	if ((rs = memfile_ctor(op,fname)) >= 0) ylikely {
	    rs = SR_INVALID ;
	    if (fname[0]) ylikely {
	 	if ((rs = memfile_opener(op,fname,of,om)) >= 0) ylikely {
		    op->magic = MEMFILE_MAGIC ;
		}
	    } /* end if (valid) */
	    if (rs < 0) {
		memfile_dtor(op) ;
	    } /* end if (error) */
	} /* end if (memfile_ctor) */
	return rs ;
}
/* end subroutine (memfile_open) */

int memfile_close(memfile *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = memfile_magic(op)) >= 0) ylikely {
            if (op->dbuf) ylikely {
		rs1 = memfile_mapend(op) ;
                if (rs >= 0) rs = rs1 ;
            }
            if (op->fd >= 0) ylikely {
                rs1 = uc_close(op->fd) ;
                if (rs >= 0) rs = rs1 ;
                op->fd = -1 ;
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
	if ((rs = memfile_magic(op,wbuf)) >= 0) ylikely {
	    csize	fsize = size_t(op->fsize) ;
	    cint	ps = op->pagesz ;
	    rs = SR_NOTOPEN ;
	    if (op->dbuf) ylikely {
	        csize	psz = (PSZMULT * ps) ;
		uint	pmo = (ps - 1) ;
		rs = SR_OK ;
		if ((op->off + wlen) > fsize) {
		    csize	a = szceil(fsize,ps) ;
	    	    if ((op->off + wlen) > a) {
	        	csize 	e = max(((op->off + wlen) - a),psz) ;
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
	        if (rs >= 0) ylikely {
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
	if ((rs = memfile_magic(op)) >= 0) ylikely {
	    rs = intsat(op->fsize) ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (memfile_len) */

int memfile_alloc(memfile *op) noex {
	int		rs ;
	if ((rs = memfile_magic(op)) >= 0) ylikely {
	    rs = intsat(op->dlen) ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (memfile_alloc) */

int memfile_tell(memfile *op,off_t *offp) noex {
	int		rs ;
	if ((rs = memfile_magic(op)) >= 0) ylikely {
	    rs = intsat(op->off) ;
	    if (offp) *offp = op->off ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (memfile_tell) */

int memfile_getbuf(memfile *op,void *vp) noex {
	caddr_t		*rpp = caddrp(vp) ;
	int		rs ;
	if ((rs = memfile_magic(op,vp)) >= 0) ylikely {
	    *rpp = caddr_t(op->dbuf) ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (memfile_getbuf) */


/* private subroutines */

local int memfile_opener(memfile *op,cchar *fname,int of,mode_t om) noex {
	int		rs ;
	of &= (~ (O_RDONLY | O_WRONLY)) ;
	of |= (O_RDWR | O_CLOEXEC) ;
        if ((rs = uc_open(fname,of,om)) >= 0) ylikely {
            cint        fd = rs ;
            if (ustat sb ; (rs = uc_fstat(fd,&sb)) >= 0) ylikely {
                if (S_ISREG(sb.st_mode)) {
                    csize       fsize = size_t(sb.st_size) ;
		    op->fd = fd ;
                    if ((rs = pagesz) >= 0) {
                        op->pagesz = rs ;
                        rs = memfile_openmap(op,fsize) ;
                    } /* end if (pagesz) */
                } else {
                    rs = SR_PROTO ;
                }
            } /* end if (stat) */
            if (rs < 0) {
                uc_close(fd) ;
                op->fd = -1 ;
            } /* end if (error) */
        } /* end if (file-open) */
	return rs ;
}
/* end subroutine (memfile_opener) */

local int memfile_openmap(memfile *op,size_t fsize) noex {
	int		rs ;
	if ((rs = memfile_mapbegin(op,0z,fsize)) >= 0) ylikely {
	    op->fsize = fsize ;
	    if ((rs = memfile_extend(op)) >= 0) ylikely {
	        op->bp = op->dbuf ;
	    }
	    if (rs < 0) {
		memfile_mapend(op) ;
	    } /* end if (error) */
	} /* end if (u_mmapbegin) */
	return rs ;
}
/* end subroutine (memfile_openmap) */

local int memfile_mapbegin(memfile *op,size_t of,size_t msz) noex {
	cnullptr	np{} ;
	csize		ms = szceil((of + msz),op->pagesz) ;
	cint		mp = (PROT_READ | PROT_WRITE) ;
	cint		mf = MAP_SHARED ;
	cint		fd = op->fd ;
	int		rs ;
	void		*md{} ;
	if ((rs = u_mmapbegin(np,ms,mp,mf,fd,of,&md)) >= 0) ylikely {
	    op->dbuf = charp(md) ;
	    op->dlen = msz ;
	}
	return rs ;
}
/* end subroutine (memfile_mepend) */

local int memfile_mapend(memfile *op) noex {
	int		rs ;
	void		*ma = op->dbuf ;
	csize		ms = op->dlen ;
	if ((rs = u_mmapend(ma,ms)) >= 0) ylikely {
	    op->dbuf = nullptr ;
	    op->dlen = 0 ;
	}
	return rs ;
}
/* end subroutine (memfile_mapend) */

local size_t mklen(sz soff,sz zsize) noex {
    	int		zl = intconv(zsize) ;
    	sz		clen ;
	if ((soff % zsize) == 0) {
	    clen = zsize ;
	} else {
	    clen = szceil(soff,zl) ;
	}
	return clen ;
} /* end subroutine (mklen) */

local int memfile_extend(memfile *op) noex {
	cint		zlen = ZEROBUFLEN ;
	int		rs = SR_OK ;
	char		zbuf[ZEROBUFLEN + 1] = {} ;
	for (size_t soff = op->fsize ; soff < op->dlen ; ) {
	    csize	zsize = size_t(zlen) ;
	    {
	        csize clen = mklen(soff,zsize) ;
	        {
	            csize	dsize = min(zsize,(clen - op->fsize)) ;
		    {
		        cint dl = intconv(dsize) ;
		        coff off = off_t(soff) ;
	                rs = u_pwrite(op->fd,zbuf,dl,off) ;
	                soff += dl ;
		    } /* end block (dsize) */
	        } /* end block (clen) */
	    } /* end block (zsize) */
	    if (rs < 0) break ;
	} /* end for */
	return rs ;
}
/* end subroutine (memfile_extend) */

local int memfile_mapextend(memfile *op,size_t ext) noex {
	cnullptr	np{} ;
	caddr_t		ma = (op->dbuf + op->dlen) ;
	size_t		ms ; /* used-multiple */
	cint		mp = (PROT_READ | PROT_WRITE) ;
	cint		mf = MAP_SHARED ;
	int		rs ;
	/* first we try to extend our existing map */
	if ((rs = memfile_memfree(op,ma,ext)) > 0) {
	    coff	mo = off_t(op->dlen) ;
	    int		fd = op->fd ;
	    void	*md{} ;
	    ms = ext ;
	    if ((rs = u_mmapbegin(ma,ms,mp,mf,fd,mo,&md)) >= 0) {
	        op->dbuf = charp(md) ;
	        op->dlen += ms ;
	    }
	} else if (rs == 0) { /* do we need to remap entirely? */
	    cint	fd = op->fd ;
	    if ((rs = uc_fdatasync(fd)) >= 0) ylikely {
		ma = op->dbuf ;
	        ms = op->dlen ;
	        rs = u_mmapend(ma,ms) ;
	        op->dbuf = nullptr ;
		op->dlen = 0 ;
	    }
	    if (rs >= 0) ylikely {
	        void	*md{} ;
	        ms = (op->dlen + ext) ;
	        if ((rs = u_mmapbegin(np,ms,mp,mf,fd,0z,&md)) >= 0) {
	            op->dbuf = charp(md) ;
	            op->dlen = ms ;
	        }
	    } /* end if */
	} /* end if (remap) */
	return rs ;
}
/* end subroutine (memfile_mapextend) */

local int memfile_memfree(memfile *op,caddr_t addr,size_t ext) noex {
	size_t		tsize = size_t(op->pagesz) ;
	cint		ps = op->pagesz ;
	int		rs = SR_OK ;
	int		f = true ; /* return-value */
	char		va[1] ;
	for (caddr_t ta = addr ; ta < (addr + ext) ; ta += ps) {
	    if ((rs = u_mincore(ta,tsize,va)) >= 0) {
		f = ((va[0] & MINCORE_INCORE) == 0) ;
		if (f) break ;
	    } /* end if (u_mincore) */
	    if (rs < 0) break ;
	} /* end for */
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (memfile_memfree) */

#ifdef	COMMENT
local int memfile_memfree(memfile *op,caddr_t addr,size_t ext) noex {
	size_t		tsize = size_t(op->pagesz) ;
	cint		ps = op->pagesz ;
	int		rs = SR_NOMEM ;
	char		vec[10] ;
	for (caddr_t ta = addr ; ta < (addr + ext) ; ta += ps) {
	    rs = u_mincore(ta,tsize,vec) ;
	    if (rs != SR_NOMEM) break ;
	} /* end for */
	return (rs == SR_NOMEM) ? true : SR_EXIST ;
}
/* end subroutine (memfile_memfree) */
#endif /* COMMENT */


