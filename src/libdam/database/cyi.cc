/* cyi SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* Calendar-Year-Index DB (read or audit a CYI database) */
/* version %I% last-modified %G% */

#define	CF_SEARCH	1		/* use 'bsearch(3c)' */
#define	CF_ISOUR	0		/* isOur */

/* revision history:

	= 1998-03-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	cyi

	Description:
	This subroutine opens and allows for reading or auditing
	of a CYI database (which currently consists of two files).

	Synopsis:
	int cyi_open(cyi *op,int year,cc *dname,cc *cname) noex

	Arguments:
	op		CYI object pointer
	year		year
	dnames		list of (pointers to) directories to search
	cnames		list of (pointers to) calendar files to use

	Returns:
	>=0		OK
	<0		error code (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* must be before others */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<sys/mman.h>
#include	<unistd.h>
#include	<climits>		/* |INT_MAX| */
#include	<ctime>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>		/* |strncmp(3c)| */
#include	<usystem.h>
#include	<getbufsize.h>
#include	<mallocxx.h>
#include	<endian.h>
#include	<ids.h>
#include	<storebuf.h>
#include	<mkpathx.h>
#include	<snx.h>
#include	<sncpyx.h>
#include	<strwcpy.h>
#include	<xperm.h>
#include	<char.h>
#include	<isnot.h>
#include	<localmisc.h>

#include	"cyi.h"
#include	"cyihdr.h"


/* local defines */

#define	CYI_KA		szof(cyi_line)
#define	CYI_FSUF	"cyi"
#define	CYI_FSUFLEN	10
#define	CYI_BO(v)	((CYI_KA - ((v) % CYI_KA)) % CYI_KA)

#define	FE_CYI		"cyi"		/* variable-index */

#define	SHIFTINT	(6 * 60)	/* possible time-shift */

#define	TO_CHECK	4

#ifndef	CF_SEARCH
#define	CF_SEARCH	1		/* use 'bsearch(3c)' */
#endif


/* external subroutines */


/* external variables */


/* local structures */

struct bventry {
	uint	voff ;
	uint	vlen ;
	uint	li ;			/* index-number of first line-entry */
	uint	hash ;
	uint	citation ;		/* (nlines, m, d) */
} ;

struct blentry {
	uint	loff ;
	uint	llen ;
} ;

namespace {
    struct vars {
	int		maxpathlen ;
	int mkvars() noex ;
    } ; /* end struct (vars) */
}


/* forward references */

template<typename ... Args>
static int cyi_ctor(cyi *op,Args ... args) noex {
    	CYI		*hop = op ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = memclear(hop) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (cyi_ctor) */

static int cyi_dtor(cyi *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (cyi_dtor) */

template<typename ... Args>
static inline int cyi_magic(cyi *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magic == CYI_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}
/* end subroutine (cyi_magic) */

static int	cyi_dbfind(cyi *,time_t,cchar *,cchar *,int) noex ;
static int	cyi_dbfindname(cyi *,ids *,time_t,char *,cc *,cc *,int) noex ;
static int	cyi_dbfindone(cyi *,time_t,cchar *,cchar *) noex ;
static int	cyi_dblose(cyi *) noex ;

static int	cyi_loadbegin(cyi *,time_t,cchar *) noex ;
static int	cyi_loadend(cyi *) noex ;
static int	cyi_mapcreate(cyi *,time_t,cchar *) noex ;
static int	cyi_mapdestroy(cyi *) noex ;
static int	cyi_proc(cyi *,time_t) noex ;
static int	cyi_verify(cyi *,time_t) noex ;
static int	cyi_auditvt(cyi *) noex ;
static int	cyi_checkupdate(cyi *,time_t) noex ;
static int	cyi_loadbve(cyi *,cyi_ent *,char *,int,uint *) noex ;

static int	cyi_bsearch(cyi *,uint (*)[5],int,uint *) noex ;
static int	cyi_lsearch(cyi *,uint (*)[5],int,uint *) noex ;

static int	mkydname(char *,cchar *,int) noex ;
static int	mkcitekey(uint *,cyi_q *) noex ;
static int	vtecmp(cvoid *,cvoid *) noex ;

#if	CF_ISOUR
static bool	isOurSuffix(cchar *,cchar *) noex ;
static bool	isNotOurFile(int) noex ;
#endif


/* local variables */

static vars		var ;

constexpr bool		f_search = CF_SEARCH ;


/* exported variables */

extern const cyi_obj	cyi_modinfo = {
	"cyi",
	szof(cyi),
	szof(cyi_cur)
} ;


/* exported subroutines */

int cyi_open(cyi *op,int year,cchar *dname,cchar *cname) noex {
	int		rs ;
	if ((rs = cyi_ctor(op,dname,cname)) >= 0) {
	    rs = SR_INVALID ;
	    if (dname[0] && cname[0]) {
		static cint	rsv = var.mkvars() ;
		if ((rs = rsv) >= 0) {
	            custime	dt = getustime ;
	            if ((rs = cyi_dbfind(op,dt,dname,cname,year)) >= 0) {
	                op->ti_lastcheck = dt ;
	                op->year = year ;
	                op->magic = CYI_MAGIC ;
	            }
		} /* end if (vars::mkvars) */
	    } /* end if (valid) */
	    if (rs < 0) {
		cyi_dtor(op) ;
	    }
	} /* end if (cyi_ctor) */
	return rs ;
}
/* end subroutine (cyi_open) */

int cyi_close(cyi *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = cyi_magic(op)) >= 0) {
	    {
	        rs1 = cyi_dblose(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = cyi_dtor(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    op->magic = 0 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (cyi_close) */

int cyi_audit(cyi *op) noex {
	int		rs ;
	if ((rs = cyi_magic(op)) >= 0) {
	    rs = cyi_auditvt(op) ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (cyi_audit) */

int cyi_count(cyi *op) noex {
	int		rs = SR_OK ;
	int		nent = 0 ;
	if ((rs = cyi_magic(op)) >= 0) {
	    cyihdr	*hip = &op->fhi ;
	    nent = hip->nentries ;
	} /* end if (magic) */
	return (rs >= 0) ? nent : rs ;
}
/* end subroutine (cyi_count) */

int cyi_getinfo(cyi *op,cyi_info *ip) noex {
	int		rs ;
	int		nent = 0 ;
	if ((rs = cyi_magic(op)) >= 0) {
	    cyihdr	*hip = &op->fhi ;
	    if (ip) {
	        memclear(ip) ;
	        ip->mtime = op->fmi.ti_mod ;
	        ip->ctime = time_t(hip->wtime) ;
	        ip->count = hip->nentries ;
	        ip->year = hip->year ;
	    }
	    nent = hip->nentries ;
	} /* end if (magic) */
	return (rs >= 0) ? nent : rs ;
}
/* end subroutine (cyi_info) */

int cyi_curbegin(cyi *op,cyi_cur *curp) noex {
    	int		rs ;
	if ((rs = cyi_magic(op,curp)) >= 0) {
	    memclear(curp) ;
	    curp->citekey = UINT_MAX ;
	    curp->i = -1 ;
	    curp->magic = CYI_MAGIC ;
	    op->ncursors += 1 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (cyi_curbegin) */

int cyi_curend(cyi *op,cyi_cur *curp) noex {
    	int		rs ;
	if ((rs = cyi_magic(op,curp)) >= 0) {
	    rs = SR_NOTOPEN ;
	    if (curp->magic == CYI_MAGIC) {
	        if (op->ncursors > 0) {
	            op->ncursors -= 1 ;
	        }
	        curp->magic = 0 ;
	    } /* end if (valid) */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (cyi_curend) */

int cyi_curcite(cyi *op,cyi_cur *curp,cyi_q *qp) noex {
	int		rs ;
	int		vi = 0 ;
	if ((rs = cyi_magic(op,curp,qp)) >= 0) {
	    cyi_fmi	*mip = &op->fmi ;
	    cyihdr	*hip = &op->fhi ;
	    rs = SR_NOTOPEN ;
	    if (curp->magic == CYI_MAGIC) {
	        uint		(*vt)[5] ;
	        uint		vte[5] ;
	        uint		citekey ;
	        int		vtlen = hip->vilen ;
	        vt = mip->vt ;
	        /* check for update */
	        if (op->ncursors == 0) {
	            rs = cyi_checkupdate(op,0) ;
	        }
	        if (rs >= 0) {
	            /* search for entry */
	            mkcitekey(&citekey,qp) ;
		    vte[3] = citekey ;
		    if_constexpr (f_search) {
	    	        rs = cyi_bsearch(op,vt,vtlen,vte) ;
	                vi = rs ;
	            } else {
	                rs = cyi_lsearch(op,vt,vtlen,vte) ;
	                vi = rs ;
	            } /* end if_constexpr (f_search) */
	            if (rs >= 0) {
	                curp->citekey = citekey ;
	                curp->i = vi ;
	            }
	        } /* end if (ok) */
	    } /* end if (valid) */
	} /* end if (magic) */
	return (rs >= 0) ? vi : rs ;
}
/* end subroutine (cyi_lookcite) */

int cyi_curread(cyi *op,cyi_cur *curp,cyi_ent *ep,char *ebuf,int elen) noex {
	int		rs ;
	int		rl = 0 ;
	if ((rs = cyi_magic(op,curp,ep,ebuf)) >= 0) {
	    cyi_fmi	*mip = &op->fmi ;
	    cyihdr	*hip = &op->fhi ;
	    rs = SR_NOTOPEN ;
	    if (curp->magic == CYI_MAGIC) {
		rs = SR_INVALID ;
		if (op->ncursors > 0) {
		    rs = SR_NOTFOUND ;
		    if (curp->i >= 0) {
			uint	vi = curp->i ;
			uint	(*vt)[5] ;
			uint	*vtep ;
			if (vi < hip->vllen) {
	    		    vt = mip->vt ;
	    		    vtep = vt[vi] ;
	    		    if (curp->citekey != UINT_MAX) {
				if (curp->citekey != (vtep[3] & 0xffff)) {
		    		    rs = SR_NOTFOUND ;
				}
	    		    } /* end if */
			} else {
	    		    rs = SR_NOTFOUND ;
			}
			if (rs >= 0) {
	    		    if ((rs = cyi_loadbve(op,ep,ebuf,elen,vtep)) >= 0) {
				rl = rs ;
	        		curp->i = (vi + 1) ;
	    		    }
			}
		    } /* end if (cursor-plausible) */
	        } /* end if (have-cursor) */
	    } /* end if (valid) */
	} /* end if (magic) */
	return (rs >= 0) ? rl : rs ;
}
/* end subroutine (cyi_curread) */

int cyi_curenum(cyi *op,cyi_cur *curp,cyi_ent *bvep,char *ebuf,int elen) noex {
	return cyi_curread(op,curp,bvep,ebuf,elen) ;
}
/* end subroutine (cyi_curenum) */


/* private subroutines */

static int cyi_dbfind(cyi *op,time_t dt,cchar *dname,cchar *cname,int y) noex {
	int		rs ;
	int		rs1 ;
	int		tl = 0 ;
	if (ids id ; (rs = id.load) >= 0) {
	    if (char *tbuf{} ; (rs = malloc_mp(&tbuf)) >= 0) {
	        if ((rs = cyi_dbfindname(op,&id,dt,tbuf,dname,cname,y)) >= 0) {
	            tl = rs ;
	            if (cchar *cp{} ; (rs = uc_mallocstrw(tbuf,tl,&cp)) >= 0) {
	                op->fname = cp ;
		    }
	        }
	    	rs = rsfree(rs,tbuf) ;
	    } /* end if (m-a-f) */
	    rs1 = id.release ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (ids) */
	return (rs >= 0) ? tl : rs ;
}
/* end subroutine (cyi_dbfind) */

static int cyi_dbfindname(cyi *op,ids *idp,time_t dt,char *tbuf,
		cchar *dname,cchar *cal,int y) noex {
	int		rs ;
	int		tl = 0 ;
	int		ai = 0 ;
	cint		sz = ((var.maxpathlen + 1) * 2) ;
	if (char *a{} ; (rs = uc_malloc(sz,&a)) >= 0) {
	    char	*ydname = (a + ((var.maxpathlen + 1) * ai++)) ;
	    if ((rs = mkydname(ydname,dname,y)) >= 0) {
	        cint	flen = CYI_FSUFLEN ;
	        cchar	*suf = CYI_FSUF ;
	        cchar	*end = ENDIANSTR ;
	        char	fsuf[CYI_FSUFLEN + 1] ;
	        if ((rs = sncpy2(fsuf,flen,suf,end)) >= 0) {
		    cint	clen = var.maxpathlen ;
	            char	*cbuf = (a + ((var.maxpathlen + 1) * ai++)) ;
	            if ((rs = snsds(cbuf,clen,cal,fsuf)) >= 0) {
		        if ((rs = mkpath2(tbuf,ydname,cbuf)) >= 0) {
			    if (USTAT sb ; (rs = uc_stat(tbuf,&sb)) >= 0) {
			        cint	am = (R_OK) ;
			        if ((rs = sperm(idp,&sb,am)) >= 0) {
	                            tl = rs ;
	                            rs = cyi_dbfindone(op,dt,cal,tbuf) ;
			        }
			    }
		        } /* end if (mkpath) */
	            } /* end if (snsds) */
	        } /* end if (sncpy) */
	    } /* end if (mkydname) */
	    rs = rsfree(rs,a) ;
	} /* end if (m-a-f) */
	return (rs >= 0) ? tl : rs ;
}
/* end subroutine (cyi_dbfindname) */

static int cyi_dbfindone(cyi *op,time_t dt,cchar *cal,cchar *tmpfname) noex {
	cyi_fmi		*mip = &op->fmi ;
	cyihdr		*hip = &op->fhi ;
	int		rs ;
	if ((rs = cyi_loadbegin(op,dt,tmpfname)) >= 0) {
	    caddr_t	md = caddr_t(mip->mapdata) ;
	    cint	mnl = var.maxpathlen ;
	    cchar	*cp ;
	    cp = (md + hip->caloff) ;
	    if (strncmp(cp,cal,mnl) != 0) rs = SR_NOMSG ;
	    if (rs < 0) {
		cyi_loadend(op) ;
	    }
	} /* end if */
	return rs ;
}
/* end subroutine (cyi_dbfindone) */

static int cyi_dblose(cyi *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	{
	    rs1 = cyi_loadend(op) ;
	    if (rs >= 0) rs = rs1 ;
	}
	if (op->fname) {
	    rs1 = uc_free(op->fname) ;
	    if (rs >= 0) rs = rs1 ;
	    op->fname = nullptr ;
	}
	return rs ;
}
/* end subroutine (cyi_dblose) */

static int cyi_loadbegin(cyi *op,time_t dt,cchar *fname) noex {
	int		rs ;
	int		fsize = 0 ;
	if ((rs = cyi_mapcreate(op,dt,fname)) >= 0) {
	    fsize = rs ;
	    rs = cyi_proc(op,dt) ;
	    if (rs < 0) {
		cyi_mapdestroy(op) ;
	    }
	}
	return (rs >= 0) ? fsize : rs ;
}
/* end subroutine (cyi_loadbegin) */

static int cyi_loadend(cyi *op) noex {
	cyi_fmi		*mip ;
	int		rs = SR_OK ;
	int		rs1 ;
	{
	    rs1 = cyi_mapdestroy(op) ;
	    if (rs >= 0) rs = rs1 ;
	}
	{
	    mip = &op->fmi ;
	    mip->vt = nullptr ;
	    mip->lt = nullptr ;
	}
	return rs ;
}
/* end subroutine (cyi_loadend) */

static int cyi_mapcreate(cyi *op,time_t dt,cchar *fname) noex {
    	cnullptr	np{} ;
	int		rs ;
	int		rs1 ;
	int		fsize = 0 ;	/* subroutine return value */
	if ((rs = u_open(fname,O_RDONLY,0666)) >= 0) {
	    USTAT	sb ;
	    cint	fd = rs ;
	    if ((rs = u_fstat(fd,&sb)) >= 0) {
		fsize = (sb.st_size & INT_MAX) ;
	        if (fsize > 0) {
	    	    csize	ms = size_t(sb.st_size) ;
	    	    cint	mp = PROT_READ ;
	    	    cint	mf = MAP_SHARED ;
	    	    void	*md{} ;
	    	    if ((rs = u_mmapbegin(np,ms,mp,mf,fd,0z,&md)) >= 0) {
			cyi_fmi		*mip = &op->fmi ;
			mip->mapdata = md ;
	        	mip->mapsize = ms ;
	        	mip->ti_mod = sb.st_mtime ;
	        	mip->ti_map = dt ;
	    	    } /* end if (u_mmapbegin) */
		} else {
	    	    rs = SR_NOCSI ;
		}
	    } /* end if (stat) */
	    rs1 = u_close(fd) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (file) */
	return (rs >= 0) ? fsize : rs ;
}
/* end subroutine (cyi_mapcreate) */

static int cyi_mapdestroy(cyi *op) noex {
	cyi_fmi		*mip = &op->fmi ;
	int		rs = SR_OK ;
	int		rs1 ;
	if (mip->mapdata) {
	    csize	ms = mip->mapsize ;
	    void	*md = mip->mapdata ;
	    rs1 = u_mmapend(md,ms) ;
	    if (rs >= 0) rs = rs1 ;
	    mip->mapdata = nullptr ;
	    mip->mapsize = 0 ;
	    mip->ti_map = 0 ;
	}
	return rs ;
}
/* end subroutine (cyi_mapdestroy) */

static int cyi_checkupdate(cyi *op,time_t dt) noex {
	int		rs = SR_OK ;
	int		f = false ;
	if (op->ncursors == 0) {
	    if (dt == 0) dt = getustime ;
	    if ((dt - op->ti_lastcheck) >= TO_CHECK) {
	        cyi_fmi		*mip = &op->fmi ;
	        op->ti_lastcheck = dt ;
	        if (USTAT sb ; (rs = u_stat(op->fname,&sb)) >= 0) {
	            f = f || (sb.st_mtime > mip->ti_mod) ;
	            f = f || (sb.st_mtime > mip->ti_map) ;
	            if (f) {
	                cyi_loadend(op) ;
	                rs = cyi_loadbegin(op,dt,op->fname) ;
	            } /* end if (update) */
	        } else if (isNotPresent(rs)) {
	            rs = SR_OK ;
	        }
	    } /* end if (timed out) */
	} /* end if (no cursors out) */
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (cyi_checkupdate) */

static int cyi_proc(cyi *op,time_t dt) noex {
	cyi_fmi		*mip = &op->fmi ;
	cyihdr		*hip = &op->fhi ;
	int		rs ;
	int		hlen = int(mip->mapsize) ;
	cchar		*hbuf = charp(mip->mapdata) ;
	if ((rs = cyihdr_wr(hip,hbuf,hlen)) >= 0) {
	    if ((rs = cyi_verify(op,dt)) >= 0) {
	        caddr_t		ma = caddr_t(mip->mapdata) ;
	        mip->vt = (uint (*)[5]) (ma + hip->vioff) ;
	        mip->lt = (uint (*)[2]) (ma + hip->vloff) ;
	    }
	}
	return rs ;
}
/* end subroutine (cyi_proc) */

static int cyi_verify(cyi *op,time_t dt) noex {
	cyi_fmi		*mip = &op->fmi ;
	cyihdr		*hip = &op->fhi ;
	uint		utime = (uint) dt ;
	int		rs = SR_OK ;
	int		size ;
	int		f = true ;
	{
	    f = f && (hip->fsize == mip->mapsize) ;
	    f = f && (hip->wtime > 0) && (hip->wtime <= (utime + SHIFTINT)) ;
	}
	{
	    f = f && (hip->vioff <= mip->mapsize) ;
	    size = hip->vilen * 5 * szof(uint) ;
	    f = f && ((hip->vioff + size) <= mip->mapsize) ;
	}
	{
	    f = f && (hip->vloff <= mip->mapsize) ;
	    size = hip->vllen * 2 * szof(uint) ;
	    f = f && ((hip->vloff + size) <= mip->mapsize) ;
	}
	{
	    f = f && (hip->vilen == hip->nentries) ;
	}
	/* get out */
	if (! f) {
	    rs = SR_BADFMT ;
	}
	return rs ;
}
/* end subroutine (cyi_verify) */

static int cyi_auditvt(cyi *op) noex {
	cyi_fmi		*mip = &op->fmi ;
	cyihdr		*hip = &op->fhi ;
	uint		(*vt)[5] ;
	uint		pcitcmpval = 0 ;
	uint		citcmpval ;
	int		rs = SR_OK ;
	int		li ;
	vt = mip->vt ;
	/* "verses" table */
	cint	vilen = int(hip->vilen) ;
	for (int i = 1 ; (rs >= 0) && (i < vilen) ; i += 1) {
	    /* verify no line-index is longer than the "lines" table itself */
	    li = vt[i][2] ;
	    if (li < vilen) {
	        /* verify all entries are ordered w/ increasing citations */
	        citcmpval = vt[i][3] & 0x0000FFFF ;
	        if (citcmpval >= pcitcmpval) {
	            pcitcmpval = citcmpval ;
	        } else {
		    rs = SR_BADFMT ;
	        }
	    } else {
		rs = SR_BADFMT ;
	    }
	} /* end for (record table entries) */
	return rs ;
}
/* end subroutine (cyi_auditvt) */

static int cyi_bsearch(cyi *op,uint (*vt)[5],int vtlen,uint vte[5]) noex {
	int		rs = SR_FAULT ;
	int		vi = 0 ;
	if (op) {
	    uint	citekey = (vte[3] & 0xffff) ;
	    uint	(*vtep)[5] ;
	    int		vtesize = (5 * szof(uint)) ;
	    vtep = (uint (*)[5]) bsearch(vte,vt,vtlen,vtesize,vtecmp) ;
	    rs = (vtep) ? (vtep - vt) : SR_NOTFOUND ;
	    vi = rs ;
	    if (rs >= 0) {
	        while ((vi > 0) && ((vt[vi-1][3] & 0x0000FFFF) == citekey)) {
		    vi -= 1 ;
	        }
	    } /* end while */
	} /* end if (non-null) */
	return (rs >= 0) ? vi : rs ;
}
/* end subroutine (cyi_bsearch) */

static int cyi_lsearch(cyi *op,uint (*vt)[5],int vtlen,uint vte[5]) noex {
	int		rs = SR_FAULT ;
	int		vi = 0 ; /* used-afterwards */
	if (op) {
	    uint	citekey = (vte[3] & 0xffff) ;
	    for (vi = 0 ; vi < vtlen ; vi += 1) {
	        if ((vt[vi][3] & 0x0000FFFF) == citekey) break ;
	    }
	    rs = (vi < vtlen) ? vi : SR_NOTFOUND ;
	} /* end if (non-null) */
	return (rs >= 0) ? vi : rs ;
}
/* end subroutine (cyi_lsearch) */

static int cyi_loadbve(cyi *op,cyi_ent *bvep,char *ebuf,int ebuflen,
		uint vte[5]) noex {
	int		rs = SR_OVERFLOW ;
	int		rlen = 0 ;
	if (ebuflen > 0) {
	    cyihdr	*hip = &op->fhi ;
	    uint	li = vte[2] ;
	    int		nlines ;
	    /* load the basic stuff */
	    memclear(bvep) ;
	    bvep->voff = vte[0] ;
	    bvep->vlen = vte[1] ;
	    bvep->nlines = (vte[3] >> 24) & UCHAR_MAX ;
	    bvep->m = (vte[3] >> 8) & UCHAR_MAX ;
	    bvep->d = (vte[3] >> 0) & UCHAR_MAX ;
	    bvep->hash = vte[4] ;
	    /* load the lines */
	    nlines = bvep->nlines ;
	    if (li < hip->vllen) {
	        cyi_fmi		*mip = &op->fmi ;
	        cint		bo = CYI_BO((ulong) ebuf) ;
	        cint		linesize = ((nlines + 1) * szof(cyi_line)) ;
	        if (linesize <= (ebuflen - bo)) {
	            cyi_line	*lines ;
	            caddr_t	ma = caddr_t(mip->mapdata) ;
	            uint	(*lt)[2] ;
	            int		i ; /* used-afterwards */
	            lt = (uint (*)[2]) (ma + hip->vloff) ;
	            lines = (cyi_line *) (ebuf + bo) ;
	            bvep->lines = lines ;
	            for (i = 0 ; i < nlines ; i += 1) {
	                lines[i].loff = lt[li+i][0] ;
	                lines[i].llen = lt[li+i][1] ;
	            } /* end for */
	            if (rs >= 0) {
	                lines[i].loff = 0 ;
	                lines[i].llen = 0 ;
	                rlen = (linesize + bo) ;
	            }
	        } else {
	            rs = SR_OVERFLOW ;
	        }
	    } else {
	        rs = SR_BADFMT ;
	    }
	} /* end if (valid) */
	return (rs >= 0) ? rlen : rs ;
}
/* end subroutine (cyi_loadbve) */

static int mkydname(char *rbuf,cchar *dname,int year) noex {
	cint		rlen = var.maxpathlen ;
	int		rs ;
	int		len = 0 ;
	if (storebuf sb(rbuf,rlen) ; (rs = sb.str(dname)) >= 0) {
	    if (rs >= 0) rs = sb.chr('/') ;
	    if (rs >= 0) rs = sb.chr('y') ;
	    if (rs >= 0) rs = sb.dec(year) ;
	    len = sb.idx ;
	} /* end if (storebuf) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (mkydname) */

static int mkcitekey(uint *cip,cyi_q *bvp) noex {
	uint		ci = 0 ;
	ci |= ((bvp->m & UCHAR_MAX) << 8) ;
	ci |= ((bvp->d & UCHAR_MAX) << 0) ;
	*cip = ci ;
	return SR_OK ;
}
/* end subroutine (mkcitekey) */

static int vtecmp(cvoid *v1p,cvoid *v2p) noex {
	uint		*vte1 = (uint *) v1p ;
	uint		*vte2 = (uint *) v2p ;
	int		c1, c2 ;
	c1 = vte1[3] & 0x0000FFFF ;
	c2 = vte2[3] & 0x0000FFFF ;
	return (c1 - c2) ;
}
/* end subroutine (vtecmp) */

#if	CF_ISOUR
static bool isOurSuffix(cchar *name,cchar *fsuf) noex {
    	cnullptr	np{} ;
	bool		f = false ;
	cchar		*tp ;
	if (((tp = strchr(name,'.')) != np) && (strcmp((tp+1),fsuf) == 0)) {
	    f = true ;
	}
	return f ;
}
/* end subroutine (isOurSuffix) */
#endif /* CF_ISOUR */

#if	CF_ISOUR
static bool isNotOurFile(int rs) noex {
	bool		f = false ;
	f = f || isNotPresent(rs) ;
	f = f || (rs == SR_NOMSG) ;
	f = f || (rs == SR_NOCSI) ;
	return f ;
}
/* end subroutine (isNotOurFile) */
#endif /* CF_ISOUR */

int vars::mkvars() noex {
    	int		rs ;
	if ((rs = getbufsize(getbufsize_mp)) >= 0) {
	    maxpathlen = rs ;
	}
    	return rs ;
}
/* end method (vars::mkvars) */


