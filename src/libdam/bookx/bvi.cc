/* bvi SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* read or audit a BVI database */
/* version %I% last-modified %G% */

#define	CF_SEARCH	1		/* use 'bsearch(3c)' */

/* revision history:

	= 2008-10-01, David A­D­ Morano
	This module was originally written.

*/

/* Copyright © 2008 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	bvi

	Description:
	This subroutine opens and allows for reading or auditing
	of a BVI (Bible Verse Index) database.

	Synopsis:
	int bvi_open(BVI *op,cchar *dbname) noex

	Arguments:
	- op		object pointer
	- dbname	name of (path-to) DB

	Returns:
	>=0		OK
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>		/* POSIX */
#include	<sys/param.h>		/* POSIX */
#include	<sys/mman.h>		/* POSIX */
#include	<climits>		/* CSTD |UCHAR_MAX| */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstring>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<ucmem.h>		/* LIBUC */
#include	<char.h>		/* LIBUC */
#include	<endian.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */

#include	"bvihdr.h"
#include	"bvcitekey.h"
#include	"bvi.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |memclear(3u)| */

/* local defines */

#define	bvi_fmi		struct bvi_fmi
#define	BVI_KA		szof(bvi_line)
#define	BVI_BO(v)	((BVI_KA - ((v) % BVI_KA)) % BVI_KA)

#define	SHIFTINT	(6 * 60)	/* possible time-shift */

#define	MODP2(v,n)	((v) & ((n) - 1))

#define	TO_CHECK	4


/* imported namespaces */

using libuc::mem ;			/* variable */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

chapters {
	uchar		*ap ;
	int		al ;
	int		ci ;
} ; /* end struct (chapters) */


/* forward references */

template<typename ... Args>
local inline int bvi_ctor(bvi *op,Args ... args) noex {
    	cnullptr	np{} ;
	cnothrow	nt{} ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    rs = SR_NOMEM ;
	    op->dbname	= np ;
	    op->fname	= np ;
	    op->fhip	= np ;
	    op->magval	= 0 ;
	    if ((op->fmip = new(nt) bvi_fmi) != np) {
	        if ((op->fhip = new(nt) bvihdr) != np) {
		    rs = SR_OK ;
		} /* end if (new-bvshdr) */
		if (rs < 0) {
		    delete op->fmip ;
		    op->fmip = nullptr ;
		} /* end if (error) */
	    } /* end if (new-fmi) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (bvi_ctor) */

local int bvi_dtor(bvi *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	    if (op->fhip) {
		delete op->fhip ;
		op->fhip = nullptr ;
	    } /* end if (memory-release) */
	    if (op->fmip) {
		delete op->fmip ;
		op->fmip = nullptr ;
	    } /* end if (memory-release) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (bvi_dtor) */

template<typename ... Args>
local inline int bvi_magic(bvi *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    rs = (op->magval == BVI_MAGIC) ? SR_OK : SR_NOTOPEN ;
	} /* end if */
	return rs ;
} /* end subroutine (bvi_magic) */

local int	bvi_loadbegin		(bvi *,time_t) noex ;
local int	bvi_loadend		(bvi *) noex ;
local int	bvi_mapcreate		(bvi *,time_t) noex ;
local int	bvi_mapdestroy		(bvi *) noex ;
local int	bvi_proc		(bvi *,time_t) noex ;
local int	bvi_verify		(bvi *,time_t) noex ;
local int	bvi_auditvt		(bvi *) noex ;
local int	bvi_checkup		(bvi *,time_t) noex ;
local int	bvi_search		(bvi *,bvi_q *) noex ;
local int	bvi_loadbve		(bvi *,bvi_v *,char *,int,int) noex ;
local int	bvi_loadchapters	(bvi *,int,uchar *,int) noex ;

local int	chapters_start		(chapters *,uchar *,int) noex ;
local int	chapters_set		(chapters *,int,int) noex ;
local int	chapters_finish		(chapters *) noex ;

local int	mkcitekey(bvi_q *,uint *) noex ;

#if	CF_SEARCH
local int	vtecmp(cvoid **,cvoid **) noex ;
#endif


/* local variables */


/* exported variables */

const bvi_obj		bvi_modinfo = {
	"bvi",
	szof(bvi),
	szof(bvi_cur)
} ; /* end initialization */


/* exported variables */


/* exported subroutines */

local int bvi_opens(bvi *op,cchar *dbn) noex {
    	int		rs ;
	int		rs1 ;
	int		nv = 0 ; /* return-value */
	cchar		*es = ENDIANSTR ;
	if (char *tbuf ; (rs = mem.mp(&tbuf)) >= 0) {
	    if ((rs = mkfnamesuf2(tbuf,op->dbn,BVI_SUF,es)) >= 0) {
	        custime	dt = time(nullptr) ;
	        cint	tl = rs ;
	        if ((rs = mem.strw(tbuf,tl,&cp)) >= 0) {
	            op->fname = cp ;
	            if ((rs = bvi_loadbegin(op,dt)) >= 0) {
	                nv = rs ;
	                op->ti_lastcheck = dt ;
	                op->magval = BVI_MAGIC ;
	            } /* end if (loadbegin) */
	            if (rs < 0) {
	                if (op->fname) {
	                    void *vp = voidp(op->fname) ;
	                    mem.free(vp) ;
	                    op->fname = nullptr ;
	                } /* end if (memory-release) */
	            } /* end if (error) */
	        } /* end if (memory-acquire) */
	    } /* end if (mkfnamesuf2) */
	} /* end if (m-a-f) */
	return (rs >= 0) ? nv : rs ;
} /* end subroutine (bvi_opens) */

int bvi_open(bvi *op,cchar *dbn) noex {
	int		rs ;
	int		nv = 0 ; /* return-value */
	if ((rs = bvi_ctor(op,dbn)) >= 0) {
	    rs = SR_INVALID ;
	    if (dbn[0]) {
	        if (cchar *cp ; (rs = mem.strw(dbn,-1,&cp)) >= 0) {
	            op->dbname = cp ;
	            if ((rs = bv_opens(op)) >= 0) {
	                op->magval = BVI_MAGIC ;
	                nv = rs ;
	            } /* end if */
	            if (rs < 0) {
	                if (op->dbname) {
	                    void *vp = voidp(op->dbname) ;
	                    mem.free(vp) ;
	                    op->dbname = nullptr ;
	                } /* end if (memory-release) */
	            } /* end if (error) */
	        } /* end if (memory-acquire) */
	    } /* end if (valid) */
	    if (rs < 0) {
		bvi_dtor(op) ;
	    } /* end if (error) */
	} /* end if (bvi_ctor) */
	return (rs >= 0) ? nv : rs ;
}
/* end subroutine (bvi_open) */

int bvi_close(bvi *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = bvi_magic(op)) >= 0) {
	    {
	        rs1 = bvi_loadend(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    if (op->fname) {
	        void *vp = voidp(op->fname) ;
	        rs1 = mem.free(vp) ;
	        if (rs >= 0) rs = rs1 ;
	        op->fname = nullptr ;
	    } /* end if (memory-release) */
	    if (op->dbname) {
	        void *vp = voidp(op->dname) ;
	        rs1 = mem.free(vp) ;
	        if (rs >= 0) rs = rs1 ;
	        op->dbname = nullptr ;
	    } /* end if (memory-release) */
	    {
		rs1 = bvi_dtor(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    op->magval = 0 ;
	} /* end if (bvi_magic) */
	return rs ;
}
/* end subroutine (bvi_close) */

int bvi_audit(bvi *op) noex {
	int		rs ;
	if ((rs = bvi_magic(op)) >= 0) {
	    /* verify that all list pointers and list entries are valid */
	    rs = bvi_auditvt(op) ;
	} /* end if (bvi_magic) */
	return rs ;
}
/* end subroutine (bvi_audit) */

int bvi_count(bvi *op) noex {
	int		rs ;
	int		nv = 0 ; /* return-value */
	if ((rs = bvi_magic(op)) >= 0) {
	    bvihdr	*hip = op->fhip ;
	    nv = hip->nverses ;
	} /* end if (bvi_magic) */
	return (rs >= 0) ? rv : rs ;
}
/* end subroutine (bvi_count) */

int bvi_info(bvi *op,bvi_info *ip) noex {
	int		rs ;
	int		nv = 0 ; /* return-value */
	if ((rs = bvi_magic(op)) >= 0) {
	    bvihdr	*hip = op->fhip ;
	    nv = hip->nverses ;
	    if (ip) {
	        memclear(ip) ;
	        ip->mtime	= op->fmi.ti_mod ;
	        ip->ctime	= (time_t) hip->wtime ;
	        ip->maxbook	= hip->maxbook ;
	        ip->maxchapter	= hip->maxchapter ;
	        ip->count	= hip->nverses ;
	        ip->nzverses	= hip->nzverses ;
	    } /* end if (non-null) */
	} /* end if (bvi_magic) */
	return (rs >= 0) ? nv : rs ;
}
/* end subroutine (bvi_info) */

int bvi_read(bvi *op,bvi_v *bvep,char *vbuf,int vlen,bvi_q *qp) noex {
	int		rs ;
	int		vi = 0 ; /* return-value */
	if ((rs = bvi_magic(op,bvep,vbuf,gp)) >= 0) {
	    /* check for update */
	    if ((rs >= 0) && (op->ncursors == 0)) {
	        rs = bvi_checkup(op,0) ;
	    } /* end if */
	    if (rs >= 0) {
	        if ((rs = bvi_search(op,qp)) >= 0) {
	            vi = rs ;
	            rs = bvi_loadbve(op,bvep,vbuf,vlen,vi) ;
	        }
	    } /* end if (ok) */
	} /* end if (bvi_magic) */
	return (rs >= 0) ? vi : rs ;
}
/* end subroutine (bvi_read) */

int bvi_get(bvi *op,bvi_q *qp,bvi_v *bvep,char *vbuf,int vlen) noex {
	return bvi_read(op,bvep,vbuf,vlen,qp) ;
}
/* end subroutine (bvi_get) */

int bvi_curbegin(bvi *op,bvi_cur *curp) noex {
    	int		rs ;
	if ((rs = bvi_magic(op,curp)) >= 0) {
	    curp->i = 0 ;
	    op->ncursors += 1 ;
	} /* end if (bvi_magic) */
	return rs ;
}
/* end subroutine (bvi_curbegin) */

int bvi_curend(bvi *op,bvi_cur *curp) noex {
    	int		rs ;
	if ((rs = bvi_magic(op,curp)) >= 0) {
	    curp->i = 0 ;
	    if (op->ncursors > 0) {
	        op->ncursors -= 1 ;
	    }
	} /* end if (bvi_magic) */
	return rs ;
}
/* end subroutine (bvi_curend) */

int bvi_curenum(bvi *op,bvi_cur *curp,bvi_v *bvep,char *vbuf,int vlen) noex {
	int		rs ;
	int		nlines = 0 ; /* return-value */
	if ((rs = bvi_magic(op,curp,bvep,vbuf)) >= 0) {
	    rs = SR_INVALID ;
	    if (op->ncursors > 0) {
	        bvihdr	*hip = op->fhip ;
	        cint	vi = (curp->i < 0) ? 0 : (curp->i + 1) ;
	        if (vi < hip->vilen) {
	            if ((rs = bvi_loadbve(op,bvep,vbuf,vlen,vi)) >= 0) {
		        nlines = rs ;
	                curp->i = vi ;
	            }
	        } else {
	            rs = SR_NOTFOUND ;
	        }
	    } /* end if (valid) */
	} /* end if (bvi_magic) */
	return (rs >= 0) ? nlines : rs ;
}
/* end subroutine (bvi_curenum) */

int bvi_chapters(bvi *op,int book,uchar *ap,int al) noex {
	bvi_q	q ;
	int		rs ;
	int		n = 0 ;

	if (op == nullptr) return SR_FAULT ;

	if (op->magval != BVI_MAGIC) return SR_NOTOPEN ;

	if (book < 0) return SR_INVALID ;

	q.b = (book & UCHAR_MAX) ;
	q.c = 1 ;
	q.v = 1 ;
	if ((rs = bvi_search(op,&q)) >= 0) {
	    int	vi = rs ;
	    rs = bvi_loadchapters(op,vi,ap,al) ;
	    n += rs ;
	}

	return (rs >= 0) ? n : rs ;
}
/* end subroutine (bvi_chapters) */


/* private subroutines */

local int bvi_loadbegin(bvi *op,time_t dt) noex {
	int		rs ;
	int		nv = 0 ; /* return-value */
	if ((rs = bvi_mapcreate(op,dt)) >= 0) {
	    rs = bvi_proc(op,dt) ;
	    nv = rs ;
	    if (rs < 0) {
	        bvi_mapdestroy(op) ;
	    } /* end if (error) */
	} /* end if */
	return (rs >= 0) ? nv : rs ;
} /* end subroutine (bvi_loadbegin) */

local int bvi_loadend(bvi *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	{
	    rs1 = bvi_mapdestroy(op) ;
	    if (rs >= 0) rs = rs1 ;
	}
	{
	    bvi_fmi	*mip = op->fmip ;
	    mip->vt = nullptr ;
	    mip->lt = nullptr ;
	}
	return rs ;
} /* end subroutine (bvi_loadend) */

local int bvi_mapcreate(bvi *op,time_t dt) noex {
	bvi_fmi		*mip = op->fmip ;
	cnullptr	np{} ;
	int		rs ;
	int		rs1 ;

	if (op->fname == nullptr) return SR_BUGCHECK ;

	if ((rs = u_open(op->fname,O_RDONLY,0666)) >= 0) {
	    int		fd = rs ;
	    if (USTAT sb ; (rs = u_fstat(fd,&sb)) >= 0) {
	        csize	fsize = (sb.st_size & UINT_MAX) ;
	        if (fsize > 0) {
	            csize	ms = fsize ;
	            cint	mp = PROT_READ ;
	            cint	mf = MAP_SHARED ;
	            void	*md ;
	            if ((rs = u_mmap(np,ms,mp,mf,fd,0z,&md)) >= 0) {
	                mip->mapdata = md ;
	                mip->mapsize = ms ;
	                mip->ti_mod = sb.st_mtime ;
	                mip->ti_map = dt ;
	                rs = fsize ;
	            } /* end if (mmap) */
	        } else {
	            rs = SR_UNATCH ;
		}
	    } /* end if (stat) */
	    rs1 = u_close(fd) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (open) */

	return rs ;
} /* end subroutine (bvi_mapcreate) */

local int bvi_mapdestroy(bvi *op) noex {
	bvi_fmi		*mip = op->fmip ;
	int		rs = SR_OK ;
	int		rs1 ;
	if (mip->mapdata) {
	    rs1 = u_munmap(mip->mapdata,mip->mapsize) ;
	    if (rs >= 0) rs = rs1 ;
	    mip->mapdata = nullptr ;
	    mip->mapsize = 0 ;
	    mip->ti_map = 0 ;
	} /* end if (un-map) */
	return rs ;
} /* end subroutine (bvi_mapdestroy) */

local int bvi_checkup(bvi *op,time_t dt) noex {
	int		rs = SR_OK ;
	int		f = false ;

	if (op->ncursors == 0) {
	    if (dt <= 0) dt = time(nullptr) ;
	    if ((dt - op->ti_lastcheck) >= TO_CHECK) {
	        USTAT		sb ;
	        bvi_fmi		*mip = op->fmip ;
	        op->ti_lastcheck = dt ;
	        if ((rs = u_stat(op->fname,&sb)) >= 0) {
	            f = f || (sb.st_mtime > mip->ti_mod) ;
	            f = f || (sb.st_mtime > mip->ti_map) ;
	            if (f) {
	                bvi_loadend(op) ;
	                rs = bvi_loadbegin(op,dt) ;
	            } /* end if (update) */
	        } else if (isNotPresent(rs)) {
	            rs = SR_OK ;
	        }
	    } /* end if (time-checked out) */
	} /* end if (no cursors out) */

	return (rs >= 0) ? f : rs ;
} /* end subroutine (bvi_checkup) */

local int bvi_proc(bvi *op,time_t dt) noex {
	bvi_fmi		*mip = op->fmip ;
	bvihdr		*hip = op->fhip ;
	int		rs ;
	int		nv = 0 ;
	if ((rs = bvihdr(hip,1,mip->mapdata,mip->mapsize)) >= 0) {
	    nverses = hip->nverses ;
	    if ((rs = bvi_verify(op,dt)) >= 0) {
	        mip->vt = (uint (*)[4]) (mip->mapdata + hip->vioff) ;
	        mip->lt = (uint (*)[2]) (mip->mapdata + hip->vloff) ;
	    }
	}
	return (rs >= 0) ? nverses : rs ;
} /* end subroutine (bvi_proc) */

local int bvi_verify(bvi *op,time_t dt) noex {
	bvi_fmi		*mip = op->fmip ;
	bvihdr		*hip = op->fhip ;
	int		rs = SR_OK ;
	int		size ;
	int		f = true ;

	f = f && (hip->fsize == mip->mapsize) ;
	f = f && (hip->wtime > 0) ;
	if (f) {
	    time_t	tt = (time_t) hip->wtime ;
	    f = (dt >= tt) ;
	}

#ifdef	COMMENT
	{
	    const uint	utime = (uint) dt ;
	    f = f && (hip->wtime <= (utime + SHIFTINT)) ;
	}
#endif

/* alignment restriction */

	f = f && ((hip->vioff & (szof(int)-1)) == 0) ;
	f = f && (hip->vioff <= mip->mapsize) ;
	size = hip->vilen * 4 * szof(uint) ;
	f = f && ((hip->vioff + size) <= mip->mapsize) ;
	/* alignment restriction */
	f = f && ((hip->vloff & (szof(int)-1)) == 0) ;
	/* size restrictions */
	f = f && (hip->vloff <= mip->mapsize) ;
	size = (hip->vllen * 2 * szof(uint)) ;
	f = f && ((hip->vloff + size) <= mip->mapsize) ;
	/* size restrictions */
	f = f && (hip->vilen == hip->nverses) ;
	/* get out */
	if (! f) {
	    rs = SR_BADFMT ;
	}

	return rs ;
} /* end subroutine (bvi_verify) */

local int bvi_auditvt(bvi *op) noex {
	bvi_fmi		*mip = op->fmip ;
	bvihdr		*hip = op->fhip ;
	uint		(*vt)[4] ;
	uint		pcitcmpval = 0 ;
	uint		citcmpval ;
	int		rs = SR_OK ;
	int		i, li ;

	vt = mip->vt ;

/* "verses" table */

	for (i = 1 ; (rs >= 0) && (i < hip->vilen) ; i += 1) {

/* verify no line-index is longer than the "lines" table itself */

	    li = vt[i][2] ;
	    if (li >= hip->vllen) {
	        rs = SR_BADFMT ;
	        break ;
	    }

/* verify all entries are ordered w/ increasing citations */

	    citcmpval = vt[i][3] & 0x00FFFFFF ;
	    if (citcmpval < pcitcmpval) {
	        rs = SR_BADFMT ;
	        break ;
	    }
	    pcitcmpval = citcmpval ;

	} /* end for (record table entries) */

	return rs ;
} /* end subroutine (bvi_auditvt) */

local int bvi_search(bvi *op,bvi_q *qp) noex {
	bvi_fmi		*mip ;
	bvihdr		*hip ;
	uint		(*vt)[4] ;
	uint		citekey ;
	uint		vte[4] ;
	int		rs = SR_OK ;
	int		vtlen ;
	int		vi = 0 ;

	mip = op->fmip ;
	hip = op->fhip ;

	vt = mip->vt ;
	vtlen = hip->vilen ;

/* search for entry */

	mkcitekey(qp,&citekey) ;

	vte[3] = citekey ;

#if	CF_SEARCH
	{
	    uint	*vtep ;
	    int		vtesize = (4 * szof(uint)) ;

	    vtep = (uint *) bsearch(vte,vt,vtlen,vtesize,vtecmp) ;

	    rs = (vtep != nullptr) ? ((vtep - vt[0]) >> 2) : SR_NOTFOUND ;
	    vi = rs ;
	}
#else /* CF_SEARCH */
	{
	    for (vi = 0 ; vi < vtlen ; vi += 1) {
	        if ((vt[vi][3] & 0x00FFFFFF) == citekey) {
	            break ;
	        }
	    }
	    rs = (vi < vtlen) ? vi : SR_NOTFOUND ;
	}
#endif /* CF_SEARCH */

	return (rs >= 0) ? vi : rs ;
} /* end subroutine (bvi_search) */

local int bvi_loadbve(bvi *op,bvi_v *bvep,char *ebuf,int elen,int vi) noex {
	BVI_LINE	*lines ;
	bvi_fmi		*mip ;
	bvihdr		*hip ;
	ulong		uebuf = (ulong) ebuf ;
	uint		*vte ;
	uint		(*lt)[2] ;
	uint		li ;
	uint		bo ;
	int		rs = SR_OK ;
	int		i ;
	int		linesize ;
	int		nlines ;

	if (bvep == nullptr) return SR_FAULT ;
	if (ebuf == nullptr) return SR_FAULT ;

	if (elen <= 0) return SR_OVERFLOW ;

	mip = op->fmip ;
	hip = op->fhip ;

	vte = mip->vt[vi] ;

/* load the basic stuff */

	memclear(bvep) ;
	bvep->voff = vte[0] ;
	bvep->vlen = vte[1] ;
	bvep->nlines = (vte[3] >> 24) & 0xFF ;
	bvep->b = (vte[3] >> 16) & 0xFF ;
	bvep->c = (vte[3] >> 8) & 0xFF ;
	bvep->v = (vte[3] >> 0) & 0xFF ;

/* load the lines */

	li = vte[2] ;
	nlines = bvep->nlines ;

	if (li < hip->vllen) {

	    bo = BVI_BO(uebuf) ;
	    linesize = ((nlines + 1) * szof(BVI_LINE)) ;
	    if (linesize <= (elen - (bo-uebuf))) {

	        lt = (uint (*)[2]) (mip->mapdata + hip->vloff) ;
	        lines = (BVI_LINE *) (uebuf + bo) ;
	        bvep->lines = lines ;

	        for (i = 0 ; i < nlines ; i += 1) {
	            lines[i].loff = lt[li+i][0] ;
	            lines[i].llen = lt[li+i][1] ;
	        } /* end for */

	        if (rs >= 0) {
	            lines[i].loff = 0 ;
	            lines[i].llen = 0 ;
	        }

	    } else {
	        rs = SR_OVERFLOW ;
	    }

	} else {
	    rs = SR_BADFMT ;
	}

	return (rs >= 0) ? nlines : rs ;
} /* end subroutine (bvi_loadbve) */

local int bvi_loadchapters(bvi *op,int vi,uchar *ap,int al) noex {
	chapters	cm ;
	BVCITEKEY	ck ;
	bvi_fmi		*mip ;
	bvihdr		*hip ;
	uint		(*vt)[4] ;
	uint		b, c, v ;
	int		rs = SR_OK ;
	int		i ;
	int		vtlen ;
	int		n = 0 ;

	mip = op->fmip ;
	hip = op->fhip ;

	vt = mip->vt ;
	vtlen = hip->vilen ;

	if (vi < vtlen) {

	    bvcitekey_get(&ck,(vt[vi]+3)) ;
	    b = ck.b ;
	    c = ck.c ;

	    if (ap != nullptr) rs = chapters_start(&cm,ap,al) ;

	    if (rs >= 0) {

	        v = 0 ;
	        for (i = vi ; (rs >= 0) && (i < vtlen) ; i += 1) {
	            bvcitekey_get(&ck,(vt[i]+3)) ;
	            if (b != ck.b)
	                break ;
	            if (c != ck.c) {
	                if (ap != nullptr) rs = chapters_set(&cm,c,v) ;
	                c = ck.c ;
	            }
	            v = ck.v ;
	        } /* end for */

	        if (rs >= 0) {
	            if (ap != nullptr) rs = chapters_set(&cm,c,v) ;
	            c += 1 ;
	        }

	        n = c ;
	        if (ap != nullptr) chapters_finish(&cm) ;

	    } /* end if (ok) */

	} /* end if */

	return (rs >= 0) ? n : rs ;
} /* end subroutine (bvi_loadchapters) */

local int chapters_start(chapters *cp,uchar *ap,int al) noex {
    	int		rs = SR_FAULT ;
	if (op && ap) {
	    rs = SR_OK ;
	    cp->ap = ap ;
	    cp->al = al ;
	    cp->ci = 0 ;
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (chapters_start) */

local int chapters_set(chapters *cp,int ci,int nv) noex {
    	int		rs = SR_OK ;
	int		ci = 0 ; /* return-value */
	if (nv <= UCHAR_MAX) {
	    if (ci < cp->al) {
		rs = SR_OK ;
	        while (cp->ci < ci) {
	            cp->ap[cp->ci++] = 0 ;
	        } /* end while */
	        if (cp->ci == ci) {
	            cp->ap[cp->ci++] = nv ;
	        }
	        ci = cp-ci ;
	    } else {
	        rs = SR_OVERFLOW ;
	    }
	} else {
	    rs = SR_RANGE ;
	}
	return (rs >= 0) ? ci : rs ;
} /* end subroutine (chapters_set) */

local int chapters_finish(chapters *cp) noex {
    	int		rs = SR_FAULT ;
	int		ci = 0 ; /* return-value */
	if (op) {
	    rs = SR_OK ;
	    ci = cp->ci ;
	} /* end if (non-null) */
	return (rs >= 0) ? ci : rs ;
} /* end subroutine (chapters_finish) */

local int mkcitekey(bvi_q *bvp,uint *cip) noex {
    	int		rs = SR_FAULT ;
	if (bvp && cip) {
	    uint	ci = 0 ;
	    rs = SR_OK ;
	    ci |= (bvp->b & UCHAR_MAX) ;
	    ci = (ci << 8) ;
	    ci |= (bvp->c & UCHAR_MAX) ;
	    ci = (ci << 8) ;
	    ci |= (bvp->v & UCHAR_MAX) ;
	    *cip = ci ;
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (mkcitekey) */

#if	CF_SEARCH
local int vtecmp(cvoid *v1p,cvoid *v2p) noex {
	uint		*vte1 = (uint *) v1p ;
	uint		*vte2 = (uint *) v2p ;
	uint		c1, c2 ;
	c1 = vte1[3] & 0x00FFFFFF ;
	c2 = vte2[3] & 0x00FFFFFF ;
	return (c1 - c2) ;
} /* end subroutine (vtecmp) */
#endif /* CF_SEARCH */


