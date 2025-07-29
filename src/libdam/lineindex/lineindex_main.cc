/* lineindex_main SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* line indexing object */
/* version %I% last-modified %G% */


/* revision history:

	= 2003-06-11, David A­D­ Morano
	I snarfed this object (flavor) from the SS-Hammock processing
	and lookup (access management) code.  We will see how it
	works out!

*/

/* Copyright © 2003 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Object:
	lineindex

	Description:
	This object module manages a line-index database.  It can
	also create such a database if it is opened with the O_CREAT
	option.  Note that line indexing is so fast (relatively)
	that trying to super-optimize anything here is not really
	worth it.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/stat.h>
#include	<sys/mman.h>		/* Memory Management */
#include	<unistd.h>
#include	<fcntl.h>		/* open-flags */
#include	<climits>		/* |INT_MAX| */
#include	<ctime>			/* |time_t| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<usystem.h>
#include	<sysval.hh>
#include	<endian.h>
#include	<intceil.h>
#include	<intsat.h>
#include	<isfiledesc.h>
#include	<isnot.h>
#include	<localmisc.h>		/* |MODP2| */

#include	"lineindex.h"
#include	"lineindexhdr.h"

import libutil ;

/* local defines */

#define	LI		lineindex

#define	TO_FILECOME	2
#define	TO_OPEN		(60 * 60)
#define	TO_MAP		(1 * 60)
#define	TO_CHECK	4
#define	TO_ACCESS	(1 * 60)


/* imported namespaces */

using std::nullptr_t ;			/* type */
using std::min ;			/* subroutine-template */
using std::max ;			/* subroutine-template */
using std::nothrow ;			/* constant */


/* local typedefs */

typedef mode_t		om_t ;		/* open-mode */
typedef uint *		uintp ;


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

template<typename ... Args>
static int lineindex_ctor(lineindex *op,Args ... args) noex {
    	LINEINDEX	*hop = op ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    cnullptr	np{} ;
	    memclear(hop) ;
	    rs = SR_NOMEM ;
	    if ((op->sbp = new(nothrow) USTAT) != np) {
		rs = SR_OK ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (lineindex_ctor) */

static int lineindex_dtor(lineindex *op) noex {
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
/* end subroutine (lineindex_dtor) */

template<typename ... Args>
static inline int lineindex_magic(lineindex *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magic == LINEINDEX_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}
/* end subroutine (lineindex_magic) */

static int	lineindex_stbegin(LI *,cc *,cc *) noex ;
static int	lineindex_stend(LI *) noex ;
static int	lineindex_opener(LI *) noex ;

static int	lineindex_idxmbegin(LI *,time_t) noex ;
static int	lineindex_idxmend(LI *) noex ;
static int	lineindex_idxmap(LI *,time_t) noex ;

static int	lineindex_idxopen(LI *,time_t) noex ;
static int	lineindex_idxopener(LI *) noex ;
static int	lineindex_idxclose(LI *) noex ;
static int	lineindex_idxverify(LI *) noex ;

namespace lineindex_ns {
    extern int	lineindex_mkidx(LI *) noex ;
}

/* local variables */

static sysval	pagesize(sysval_ps) ;

constexpr int	magicsize = LINEINDEXHDR_MAGICSIZE ;

constexpr off_t	maxfoff = INT_MAX ;


/* exported variables */


/* exported subroutines */

int lineindex_open(LI *op,cc *ifn,int of,om_t om,cc *tfn) noex {
	int		rs ;
	int		c = 0 ;
	if ((rs = lineindex_ctor(op,ifn)) >= 0) {
	    rs = SR_INVALID ;
	    if (ifn[0] && (of >= 0)) {
	        op->fd = -1 ;
	        op->of = of ;
	        op->om = om ;
		if ((rs = pagesize) >= 0) {
		    cint	am = (of & O_ACCMODE) ;
		    op->pagesize = rs ;
		    op->fl.wantwrite = ((am == O_WRONLY) || (am == O_RDWR)) ;
		    if ((rs = lineindex_stbegin(op,ifn,tfn)) >= 0) {
		    	{
		    	    rs = lineindex_opener(op) ;
			    c = rs ;
			}
			if (rs < 0) {
			    lineindex_stend(op) ;
			}
		    } /* end if (store-files) */
		} /* end if (pagesize) */
	    } /* end if (valid) */
	    if (rs < 0) {
		lineindex_dtor(op) ;
	    }
	} /* end if (lineindex_ctor) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (lineindex_opne) */

static int lineindex_opener(LI *op) noex {
	custime		dt = getustime ;
	int		rs ;
	if ((rs = lineindex_idxmbegin(op,dt)) >= 0) {
	    if ((rs = lineindex_idxverify(op)) >= 0) {
		op->tiaccess = dt ;
		op->magic = LINEINDEX_MAGIC ;
	    }
	    if (rs < 0) {
		lineindex_idxmend(op) ;
	    }
	} /* end if (lineindex_idxm-) */
	return rs ;
}
/* end subroutine (lineindex_opener) */

int lineindex_close(LI *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = lineindex_magic(op)) >= 0) {
	    if (op->mapdata) {
		rs1 = lineindex_idxmend(op) ;
		if (rs >= 0) rs = rs1 ;
	    }
	    {
		rs1 = lineindex_stend(op) ;
		if (rs >= 0) rs = rs1 ;
	    }
	    {
		rs1 = lineindex_dtor(op) ;
		if (rs >= 0) rs = rs1 ;
	    }
	    op->magic = 0 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (lineindex_close) */

int lineindex_count(LI *op) noex {
    	int		rs ;
	if ((rs = lineindex_magic(op)) >= 0) {
	    rs = op->lines ;
	}
	return rs ;
}
/* end subroutine (lineindex_count) */

int lineindex_curbegin(LI *op,lineindex_cur *curp) noex {
    	int		rs ;
	if ((rs = lineindex_magic(op,curp)) >= 0) {
	    op->cursors += 1 ;
	    curp->i = 0 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (lineindex_curbegin) */

int lineindex_curend(LI *op,lineindex_cur *curp) noex {
    	int		rs ;
	if ((rs = lineindex_magic(op,curp)) >= 0) {
	    if (op->cursors > 0) {
	        op->cursors -= 1 ;
	    }
	    curp->i = 0 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (lineindex_curend) */

int lineindex_curenum(LI *op,lineindex_cur *curp,off_t *rop) noex {
	int		rs ;
	int		ri = 0 ;
	if ((rs = lineindex_magic(op,curp,rop)) >= 0) {
	    rs = SR_INVALID ;
	    if (op->cursors > 0) {
		rs = SR_NOTFOUND ;
		if ((ri = curp->i) < op->lines) {
		    rs = SR_OK ;
	            *rop = off_t(op->rectab[ri++]) ;
	            curp->i = ri ;
		} else {
		    *rop = 0z ;
	        } /* end if (ok) */
	    } /* end if (valid) */
	} /* end if (magic) */
	return (rs >= 0) ? ri : rs ;
}
/* end subroutine (lineindex_curenum) */

int lineindex_lookup(LI *op,int ri,off_t *rop) noex {
	int		rs ;
	if ((rs = lineindex_magic(op,rop)) >= 0) {
	    rs = SR_NOTFOUND ;
	    if (ri < op->lines) {
		rs = SR_OK ;
	    	*rop = off_t(op->rectab[ri]) ;
	    } else {
		*rop = 0z ;
	    } /* end if (valid) */
	} /* end if (magic) */
	return (rs >= 0) ? ri : rs ;
}
/* end subroutine (lineindex_lookup) */

int lineindex_check(LI *op,time_t dt) noex {
	int		rs ;
	if ((rs = lineindex_magic(op)) >= 0) {
	    (void) dt ;
	    rs = SR_NOSYS ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (lineindex_check) */


/* private subroutines */

static int lineindex_stbegin(LI *op,cc *ifn,cc *tfn) noex {
    	int		rs ;
	if (cchar *cp{} ; (rs = uc_mallocstrw(ifn,-1,&cp)) >= 0) {
	    op->ifn = cp ;
	    if (tfn) {
	        rs = SR_INVALID ;
		if (tfn[0]) {
		    if ((rs = uc_mallocstrw(tfn,-1,&cp)) >= 0) {
	    		op->tfn = cp ;
		    } /* end if (memory-alloation) */
		}
	    } /* end if (non-null) */
	    if (rs < 0) {
		uc_free(op->ifn) ;
		op->ifn = nullptr ;
	    }
	} /* end if (memory-allocation) */
	return rs ;
}
/* end subroutine (lineindex_stbegin) */

static int lineindex_stend(LI *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (op->tfn) {
	    rs1 = uc_free(op->tfn) ;
	    if (rs >= 0) rs = rs1 ;
	    op->tfn = nullptr ;
	}
	if (op->ifn) {
	    rs1 = uc_free(op->ifn) ;
	    if (rs >= 0) rs = rs1 ;
	    op->ifn = nullptr ;
	}
	return rs ;
}
/* end subroutine (lineindex_stend) */

static int lineindex_idxmap(LI *op,time_t dt) noex {
	int		rs = SR_OK ;
	if (op->mapdata == nullptr) {
	    cnullptr	np{} ;
	    csize	ms = size_t(uceil(op->filesize,op->pagesize)) ;
	    cint 	mp = PROT_READ ;
	    cint 	mf = MAP_SHARED ;
	    cint	fd = op->fd ;
	    void	*md{} ; 
	    if ((rs = u_mmapbegin(np,ms,mp,mf,fd,0z,&md)) >= 0) {
		op->mapdata = caddr_t(md) ;
		op->mapsize = ms ;
	        op->timap = dt ;
	    } /* end if (u_mmapbegin) */
	} /* end if (mapping needed) */
	return rs ;
}
/* end subroutine (lineindex_idxmap) */

static int lineindex_idxmbegin(LI *op,custime dt) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (op->mapdata == nullptr) {
	    if ((rs = lineindex_idxopen(op,dt)) >= 0) {
		{
		    rs = lineindex_idxmap(op,dt) ;
		}
	        rs1 = lineindex_idxclose(op) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (opened the file) */
	} /* end if (mapping file) */
	return rs ;
}
/* end subroutine (lineindex_idxmbegin) */

static int lineindex_idxmend(LI *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (op->mapdata) {
	    csize	ms = op->mapsize ;
	    void	*md = op->mapdata ;
	    rs1 = u_mmapend(md,ms) ;
	    if (rs >= 0) rs = rs1 ;
	    op->mapdata = nullptr ;
	    op->mapsize = 0 ;
	} /* end if (checking existing map) */
	return rs ;
}
/* end subroutine (lineindex_idxmend) */

static int lineindex_idxopener(LI *op) noex {
	cint		of = (op->of & (~ (O_TRUNC | O_CREAT))) ;
    	int		rs ;
	cmode		om = op->om ;
	if ((rs = u_open(op->ifn,of,om)) >= 0) {
	    op->fd = rs ;
	} else if (isNotPresent(rs)) {
	    cbool	fc = bool(op->of & O_CREAT) ;
	    if (fc && (op->tfn != nullptr)) {
	        if ((rs = lineindex_ns::lineindex_mkidx(op)) >= 0) {
	            rs = u_open(op->ifn,of,om) ;
	    	    op->fd = rs ;
	        }
	    }
	}
	return rs ;
}
/* end subroutine (lineindex_idxopener) */

static int lineindex_idxopen(LI *op,time_t dt) noex {
	int		rs ;
	if ((rs = lineindex_idxopener(op)) >= 0) {
	    cint	fd = op->fd ;
	    if ((rs = isfsremote(fd)) >= 0) {
	  	op->fl.remote = (rs > 0) ;
		if (USTAT *sbp = op->sbp ; (rs = u_fstat(fd,sbp)) >= 0) {
		    if (sbp->st_size <= maxfoff) {
			if (dt == 0) dt = getustime ;
	                op->filesize = int(sbp->st_size) ;
	                op->tifmod = sbp->st_mtime ;
	    	        op->tiopen = dt ;
		    } else {
			rs = SR_TOOBIG ;
		    }
		} /* end if (fstat) */
	    } /* end if (isfsremote) */
	    if (rs < 0) {
		u_close(op->fd) ;
		op->fd = -1 ;
	    }
	} /* end if (lineindex_idxopener) */
	return rs ;
}
/* end subroutine (lineindex_idxopen) */

static int lineindex_idxclose(LI *op) noex {
	int		rs = SR_OK ;
	if (op->fd >= 0) {
	    rs = u_close(op->fd) ;
	    op->fd = -1 ;
	    op->tifmod = 0 ;
	    op->tiopen = 0 ;
	}
	return rs ;
}
/* end subroutine (lineindex_idxclose) */

static int lineindex_idxverify(LI *op) noex {
    	int		rs = SR_BUGCHECK ;
	if (op->mapdata) {
	    cint	ml = intsat(op->mapsize) ;
	    char	*mp = charp(op->mapdata) ;
	    if (lineindexhdr hdr{} ; (rs = hdr.wr(mp,ml)) >= 0) {
		cint	ver = hdr.vetu[0] ;
		cint	end = hdr.vetu[1] ;
		cuint	typ = hdr.vetu[2] ;
		bool	fbad = false ;
		op->rectab = uintp(mp + hdr.rectab) ;
		op->tiwrite = time_t(hdr.wrtime) ;
		op->lines = int(hdr.lines) ;
		fbad = fbad || (ver != LINEINDEX_FILEVERSION) ;
		fbad = fbad || (end != ENDIAN) ;
		fbad = fbad || (typ != LINEINDEX_FILETYPE) ;
		if (fbad) rs = SR_BADFMT ;
	    } /* end if (lineindexhdr_wr) */
	}
	return rs ;
}
/* end subroutine (lineindex_idxverify) */


