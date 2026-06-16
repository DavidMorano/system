/* bvi SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* read or audit a BVI database */
/* version %I% last-modified %G% */

#define	CF_DEBUG	0		/* debugging */
#define	CF_SEARCH	1		/* use |bsearch(3c)| */

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
#include	<usyscalls.h>		/* LIBU */
#include	<endian.h>		/* LIBU */
#include	<ucmem.h>		/* LIBUC */
#include	<mkfname.h>		/* LIBUC */
#include	<char.h>		/* LIBUC */
#include	<isnot.h>		/* LIBUC */
#include	<bvchapters.hh>		/* LIBDAM */
#include	<localmisc.h>		/* LIBU */

#include	"bvihdr.h"
#include	"bvcitekey.h"
#include	"bvi.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |memclear(3u)| */

/* local defines */

#define	BVI_KA		szof(bvi_line)
#define	BVI_BO(v)	((BVI_KA - ((v) % BVI_KA)) % BVI_KA)

#define	SHIFTINT	(6 * 60)	/* possible time-shift */

#define	MODP2(v,n)	((v) & ((n) - 1))

#define	TO_CHECK	4

#ifndef	CF_DEBUG
#define	CF_DEBUG	0		/* debugging */
#endif
#ifndef	CF_SEARCH
#define	CF_SEARCH	0		/* use |bsearch(3c)| */
#endif


/* imported namespaces */

using libuc::mem ;			/* variable */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


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

local int	mkcitekey	(uint *,bvi_q *) noex ;
local int	vtecmp		(cvoid *,cvoid *) noex ;


/* local variables */

cuint		vmask			= bvcitekey_vmask ;
cbool		f_debug			= CF_DEBUG ;
cbool		f_search		= CF_SEARCH ;


/* exported variables */

const bvi_obj		bvi_modinfo = {
	"bvi",
	szof(bvi),
	szof(bvi_cur)
} ; /* end initialization */


/* exported subroutines */

local int bvi_opens(bvi *op) noex {
    	int		rs ;
	int		rs1 ;
	int		nv = 0 ; /* return-value */
	cchar		*es = ENDIANSTR ;
	if (char *tbuf ; (rs = mem.mp(&tbuf)) >= 0) {
	    if ((rs = mkfnamesuf2(tbuf,op->dbname,BVI_SUF,es)) >= 0) {
	        custime	dt = time(nullptr) ;
	        cint	tl = rs ;
	        if (cchar *cp ; (rs = mem.strw(tbuf,tl,&cp)) >= 0) {
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
	    rs1 = mem.free(tbuf) ;
	    if (rs >= 0) rs = rs1 ;
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
	            if ((rs = bvi_opens(op)) >= 0) {
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
} /* end subroutine (bvi_open) */

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
	        void *vp = voidp(op->dbname) ;
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
} /* end subroutine (bvi_close) */

int bvi_audit(bvi *op) noex {
	int		rs ;
	if ((rs = bvi_magic(op)) >= 0) {
	    /* verify that all list pointers and list entries are valid */
	    rs = bvi_auditvt(op) ;
	} /* end if (bvi_magic) */
	return rs ;
} /* end subroutine (bvi_audit) */

int bvi_count(bvi *op) noex {
	int		rs ;
	int		nv = 0 ; /* return-value */
	if ((rs = bvi_magic(op)) >= 0) {
	    bvihdr	*hip = op->fhip ;
	    nv = hip->nverses ;
	} /* end if (bvi_magic) */
	return (rs >= 0) ? nv : rs ;
} /* end subroutine (bvi_count) */

int bvi_getinfo(bvi *op,bvi_info *ip) noex {
	int		rs ;
	int		nv = 0 ; /* return-value */
	if ((rs = bvi_magic(op)) >= 0) {
	    bvi_fmi	*fip = op->fmip ;
	    bvihdr	*hip = op->fhip ;
	    nv = hip->nverses ;
	    if (ip) {
	        memclear(ip) ;
	        ip->mtime	= fip->ti_mod ;
	        ip->ctime	= (time_t) hip->wtime ;
	        ip->maxbook	= hip->maxbook ;
	        ip->maxchap	= hip->maxchap ;
	        ip->count	= hip->nverses ;
	        ip->nzverses	= hip->nzverses ;
	    } /* end if (non-null) */
	} /* end if (bvi_magic) */
	return (rs >= 0) ? nv : rs ;
} /* end subroutine (bvi_getinfo) */

int bvi_read(bvi *op,bvi_v *bvep,char *vbuf,int vlen,bvi_q *qp) noex {
	int		rs ;
	int		vi = 0 ; /* return-value */
	if ((rs = bvi_magic(op,bvep,vbuf,qp)) >= 0) {
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
} /* end subroutine (bvi_read) */

int bvi_get(bvi *op,bvi_q *qp,bvi_v *bvep,char *vbuf,int vlen) noex {
	return bvi_read(op,bvep,vbuf,vlen,qp) ;
} /* end subroutine (bvi_get) */

int bvi_curbegin(bvi *op,bvi_cur *curp) noex {
    	int		rs ;
	if ((rs = bvi_magic(op,curp)) >= 0) {
	    curp->i = 0 ;
	    op->ncursors += 1 ;
	} /* end if (bvi_magic) */
	return rs ;
} /* end subroutine (bvi_curbegin) */

int bvi_curend(bvi *op,bvi_cur *curp) noex {
    	int		rs ;
	if ((rs = bvi_magic(op,curp)) >= 0) {
	    curp->i = 0 ;
	    if (op->ncursors > 0) {
	        op->ncursors -= 1 ;
	    }
	} /* end if (bvi_magic) */
	return rs ;
} /* end subroutine (bvi_curend) */

int bvi_curenum(bvi *op,bvi_cur *curp,bvi_v *bvep,char *vbuf,int vlen) noex {
	int		rs ;
	int		nlines = 0 ; /* return-value */
	if ((rs = bvi_magic(op,curp,bvep,vbuf)) >= 0) {
	    bvihdr	*hip = op->fhip ;
	    rs = SR_INVALID ;
	    if (op->ncursors > 0) {
	        cint	vi = (curp->i < 0) ? 0 : (curp->i + 1) ;
	        rs = SR_NOTFOUND ;
		if (cint vilen = int(hip->vilen) ; vi < vilen) {
	            if ((rs = bvi_loadbve(op,bvep,vbuf,vlen,vi)) >= 0) {
		        nlines = rs ;
	                curp->i = vi ;
	            } /* end if (bvi_loadbve) */
	        } /* end if (valid) */
	    } /* end if (valid) */
	} /* end if (bvi_magic) */
	return (rs >= 0) ? nlines : rs ;
} /* end subroutine (bvi_curenum) */

int bvi_chapters(bvi *op,int book,uchar *ap,int al) noex {
	int		rs ;
	int		n = 0 ; /* return-value */
	if ((rs = bvi_magic(op,ap)) >= 0) {
	    rs = SR_INVALID ;
	    if (book >= 0) {
	        bvi_q	q{} ;
	        q.b = uchar(book & UCHAR_MAX) ;
	        q.c = 1 ;
	        q.v = 1 ;
	        if ((rs = bvi_search(op,&q)) >= 0) {
	            cint	vi = rs ;
	            rs = bvi_loadchapters(op,vi,ap,al) ;
	            n += rs ;
	        }
	    } /* end if (valid) */
	} /* end if (bvi_magic) */
	return (rs >= 0) ? n : rs ;
} /* end subroutine (bvi_chapters) */


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
	int		rs = SR_BUGCHECK ;
	int		rs1 ;
	int		fsz = 0 ; /* return-value */
	if (op->fname) {
	    if ((rs = u_open(op->fname,O_RDONLY,0666)) >= 0) {
	        cint	fd = rs ;
	        if (ustat sb ; (rs = u_fstat(fd,&sb)) >= 0) {
	            csize	fsize = size_t(sb.st_size) ;
	            if (fsize > 0) {
	                csize	ms = fsize ;
	                cint	mp = PROT_READ ;
	                cint	mf = MAP_SHARED ;
	                void	*md ;
	                if ((rs = u_mmapbegin(np,ms,mp,mf,fd,0z,&md)) >= 0) {
	                    mip->mapdata = charp(md) ;
	                    mip->mapsize = ms ;
	                    mip->ti_mod = sb.st_mtime ;
	                    mip->ti_map = dt ;
	                    fsz = intconv(fsize) ;
	                } /* end if (mmap) */
	            } else {
	                rs = SR_UNATCH ;
		    }
	        } /* end if (stat) */
	        rs1 = u_close(fd) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (open) */
	} /* end if (non-null) */
	return (rs >= 0) ? fsz : rs ;
} /* end subroutine (bvi_mapcreate) */

local int bvi_mapdestroy(bvi *op) noex {
	bvi_fmi		*mip = op->fmip ;
	int		rs = SR_OK ;
	int		rs1 ;
	if (mip->mapdata) {
	    csize ms = mip->mapsize ;
	    voidp md = mip->mapdata ;
	    rs1 = u_mmapend(md,ms) ;
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
	        bvi_fmi		*mip = op->fmip ;
	        op->ti_lastcheck = dt ;
	        if (ustat sb ; (rs = u_stat(op->fname,&sb)) >= 0) {
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
	int		rs = SR_BUGCHECK ;
	int		nv = 0 ;
	if (bvi_fmi *mip = op->fmip ; mip) {
	    cint msz = intconv(mip->mapsize) ;
	    if (bvihdr *hip = op->fhip ; hip) {
	        if ((rs = hip->wr(mip->mapdata,msz)) >= 0) {
	            nv = hip->nverses ;
	            if ((rs = bvi_verify(op,dt)) >= 0) {
	                mip->vt = (uint (*)[4]) (mip->mapdata + hip->vioff) ;
	                mip->lt = (uint (*)[2]) (mip->mapdata + hip->vloff) ;
	            } /* end if (bvi_verify) */
	        } /* end if (bvihdr_wr) */
	    } /* end if (bug-check) */
	} /* end if (bug-check) */
	return (rs >= 0) ? nv : rs ;
} /* end subroutine (bvi_proc) */

local int bvi_verify(bvi *op,time_t dt) noex {
	int		rs = SR_BUGCHECK ;
	if (bvi_fmi *mip = op->fmip ; mip) {
	    if (bvihdr *hip = op->fhip ; hip) {
		cuint	msz = uintconv(mip->mapsize) ;
		int	sz{} ;
		bool	f = true ;
		rs = SR_OK ;
	        f = f && (hip->fsz == mip->mapsize) ;
	        f = f && (hip->wtime > 0) ;
	        if (f) {
	            custime tt = time_t(hip->wtime) ;
	            f = (dt >= tt) ;
	        }
	        /* alignment restriction */
	        f = f && ((hip->vioff & (szof(int)-1)) == 0) ;
	        f = f && (hip->vioff <= msz) ;
	        sz = hip->vilen * 4 * szof(uint) ;
	        f = f && ((hip->vioff + sz) <= msz) ;
	        /* alignment restriction */
	        f = f && ((hip->vloff & (szof(int) - 1)) == 0) ;
	        /* size restrictions */
	        f = f && (hip->vloff <= msz) ;
	        sz = (hip->vllen * 2 * szof(uint)) ;
	        f = f && ((hip->vloff + sz) <= msz) ;
	        /* size restrictions */
	        f = f && (hip->vilen == hip->nverses) ;
	        /* get out */
	        if (! f) {
	            rs = SR_BADFMT ;
	        }
	    } /* end if (bug-check) */
	} /* end if (bug-check) */
	return rs ;
} /* end subroutine (bvi_verify) */

local int bvi_auditvt(bvi *op) noex {
	bvi_fmi		*mip = op->fmip ;
	bvihdr		*hip = op->fhip ;
	int		rs = SR_OK ;
	{
	   uint		(*vt)[4] = mip->vt ;
	   uint		pcitcmpval = 0 ;
	   uint		citcmpval ;
	   /* "verses" table */
	   cint n = int(hip->vilen) ;
	   for (int i = 1 ; (rs >= 0) && (i < n) ; i += 1) {
	       /* verify no line-index is LT the "lines" table itself */
	       if (cuint li = vt[i][2] ; li >= hip->vllen) {
	           rs = SR_BADFMT ;
	           break ;
	       }
	       /* verify all entries are ordered w/ increasing citations */
	       citcmpval = vt[i][3] & vmask ;
	       if (citcmpval < pcitcmpval) {
	           rs = SR_BADFMT ;
	           break ;
	       }
	       pcitcmpval = citcmpval ;
	   } /* end for (record table entries) */
	} /* end block */
	return rs ;
} /* end subroutine (bvi_auditvt) */

local int bvi_search(bvi *op,bvi_q *qp) noex {
	bvi_fmi		*mip = op->fmip ;
	bvihdr		*hip = op->fhip ;
	int		rs = SR_OK ;
	int		vi = 0 ; /* return-value */
	{
	    int		vtlen = hip->vilen ;
	    uint	(*vt)[4] = mip->vt ;
	    uint	vte[4] ;
	    /* search for entry */
	    if (uint citekey ; (rs = mkcitekey(&citekey,qp)) >= 0) {
	        vte[3] = citekey ;
	        if_constexpr (f_search) {
	            uint	*vtep ;
	            csize	vtesize = (4 * szof(uint)) ;
	            {
	                vtep = (uint *) bsearch(vte,vt,vtlen,vtesize,vtecmp) ;
	            }
	            rs = (vtep) ? intconv((vtep - vt[0]) >> 2) : SR_NOTFOUND ;
	            vi = rs ;
	        } else {
	            for (vi = 0 ; vi < vtlen ; vi += 1) {
	                if ((vt[vi][3] & vmask) == citekey) {
	                    break ;
	                }
	            } /* end if */
	            rs = (vi < vtlen) ? vi : SR_NOTFOUND ;
	        } /* end if_constexpr (f_search) */
	    } /* end if (mkcitekey) */
	} /* end block */
	return (rs >= 0) ? vi : rs ;
} /* end subroutine (bvi_search) */

local int bvi_loadbve(bvi *op,bvi_v *bvep,char *ebuf,int elen,int vi) noex {
	bvi_fmi		*mip = op->fmip ;
	bvihdr		*hip = op->fhip ;
	int		rs = SR_FAULT ;
	int		nlines = 0 ; /* return-value */
	if (bvep && ebuf) {
	    rs = SR_OVERFLOW ;
	    if (elen > 0) {
	        uintptr_t	uebuf = uintptr_t(ebuf) ;
	        bvi_line	*lines ;
	        uint		*vte = mip->vt[vi] ;
	        uint		(*lt)[2] ;
	        uint		li ;
	        /* load the basic stuff */
	        memclear(bvep) ;
	        bvep->voff	= vte[0] ;
	        bvep->vlen	= vte[1] ;
	        bvep->nlines	= getbyte(vte[3],3) ;
	        bvep->b		= getbyte(vte[3],2) ;
	        bvep->c		= getbyte(vte[3],1) ;
	        bvep->v		= getbyte(vte[3],0) ;
	        /* load the lines */
	        li = vte[2] ;
	        nlines = bvep->nlines ; /* return-value */
	        if (li < hip->vllen) {
	            cint	linesz = ((nlines + 1) * szof(bvi_line)) ;
	            uint	bo = BVI_BO(uebuf) ;
	            if (linesz <= (elen - intconv(bo - uebuf))) {
		        int i ; /* used-afterwards */
			rs = SR_OK ;
	                lt = (uint (*)[2]) (mip->mapdata + hip->vloff) ;
	                lines = (bvi_line *) (uebuf + bo) ;
	                bvep->lines = lines ;
	                for (i = 0 ; i < nlines ; i += 1) {
	                    lines[i].loff = lt[li+i][0] ;
	                    lines[i].llen = lt[li+i][1] ;
	                } /* end for */
	                if (rs >= 0) {
	                    lines[i].loff = 0 ;
	                    lines[i].llen = 0 ;
	                } /* end if (ok) */
	            } else {
	                rs = SR_OVERFLOW ;
	            }
	        } else {
	            rs = SR_BADFMT ;
	        }
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? nlines : rs ;
} /* end subroutine (bvi_loadbve) */

local int bvi_loadchapters(bvi *op,int vi,uchar *ap,int al) noex {
	bvi_fmi		*mip = op->fmip ;
	bvihdr		*hip = op->fhip ;
	uint		(*vt)[4] ;
	uint		b, c, v ;
	int		rs = SR_OK ;
	int		rs1 ;
	int		vtlen ;
	int		n = 0 ; /* return-value */
	{
	    vt = mip->vt ;
	    vtlen = hip->vilen ;
	}
	if (vi < vtlen) {
	    bvcitekey	ck ;
	    bvchapters	cm ;
	    bvcitekey_get(&ck,(vt[vi]+3)) ;
	    b = ck.b ;
	    c = ck.c ;
	    if (ap) {
		rs = cm.start(ap,al) ;
	    }
	    if (rs >= 0) {
	        v = 0 ;
	        for (int i = vi ; (rs >= 0) && (i < vtlen) ; i += 1) {
	            bvcitekey_get(&ck,(vt[i]+3)) ;
	            if (b != ck.b)
	                break ;
	            if (c != ck.c) {
	                if (ap) {
			    rs = cm.load(c,v) ;
			}
	                c = ck.c ;
	            }
	            v = ck.v ;
	        } /* end for */
	        if (rs >= 0) {
	            if (ap) {
			rs = cm.load(c,v) ;
		    }
	            c += 1 ;
	        } /* end if (ok) */
	        n = c ; /* return-value */
	        if (ap) {
		    rs1 = cm.finish() ;
		    if (rs >= 0) rs = rs1 ;
		}
	    } /* end if (ok) */
	} /* end if */
	return (rs >= 0) ? n : rs ;
} /* end subroutine (bvi_loadchapters) */

local uint mkciteload(uint ci,uchar item) noex {
	ci = (ci << UCHAR_BIT) ;
	ci |= uint(item) ;
	return ci ; 
} /* end subroutine (mkciteload) */

local int mkcitekey(uint *cip,bvi_q *bvp) noex {
    	int		rs = SR_BUGCHECK ;
	if (cip && bvp) {
	    uint	ci = 0 ;
	    rs = SR_OK ;
	    ci = mkciteload(ci,bvp->b) ;
	    ci = mkciteload(ci,bvp->c) ;
	    ci = mkciteload(ci,bvp->v) ;
	    *cip = ci ;
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (mkcitekey) */

local int entcmp(cuint *vte1,cuint *vte2) noex {
	int		rc = 0 ;
	{
	    int	c1 = int(vte1[3] & vmask) ;
	    int	c2 = int(vte2[3] & vmask) ;
	    rc = (c1 - c2) ;
	}
	return rc ;
} /* end subroutine (entcmp) */

local int vtecmp(cvoid *v1p,cvoid *v2p) noex {
	uint		*vte1 = uintp(v1p) ;
	uint		*vte2 = uintp(v2p) ;
	return entcmp(vte1,vte2) ;
} /* end subroutine (vtecmp) */


