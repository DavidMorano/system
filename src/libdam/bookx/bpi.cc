/* bpi SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* read or audit a BPI (Bible Paragraph Index) database */
/* version %I% last-modified %G% */

#define	CF_DEBUG	0		/* debugging */
#define	CF_SEARCH	1		/* use |bsearch(3c)| */

/* revision history:

	= 1998-03-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	bpi

	Description:
	This subroutine opens and allows for reading or auditing
	of a BPI (Bible Paragraph Index) database.

	Synopsis:
	int bpi_open(bpi *op,cchar *dbname) noex

	Arguments:
	- op		object pointer
	- dbname	name of (path-to) DB

	Returns:
	>=0		OK
	<0		error code (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>		/* POSIX */
#include	<sys/param.h>		/* POSIX */
#include	<sys/stat.h>		/* POSIX */
#include	<sys/mman.h>		/* POSIX */
#include	<unistd.h>		/* POSIX */
#include	<ctime>			/* CSTD */
#include	<climits>		/* CSTD |UINT_MAX| */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstring>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<endian.h>		/* LIBU */
#include	<ucmem.h>		/* LIBUC */
#include	<ucopen.h>		/* LIBUC */
#include	<ucdesc.h>		/* LIBUC */
#include	<ucfileop.h>		/* LIBUC */
#include	<storebuf.h>		/* LIBUC */
#include	<mkpath.h>		/* LIBUC */
#include	<mkfname.h>		/* LIBUC */
#include	<char.h>		/* LIBUC */
#include	<isnot.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */

#include	"bpihdr.h"
#include	"bvcitekey.h"
#include	"bpi.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |memclear(3u)| */

/* local defines */

#define	BPI_KA		szof(BPI_LINE)
#define	BPI_BO(v)	((BPI_KA - ((v) % BPI_KA)) % BPI_KA)

#define	SHIFTINT	(6 * 60)	/* possible time-shift */

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
local inline int bpi_ctor(bpi *op,Args ... args) noex {
    	cnullptr	np{} ;
	cnothrow	nt{} ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    rs = SR_NOMEM ;
	    op->dbname	= np ;
	    op->fname	= np ;
	    op->fhip	= np ;
	    op->magval	= 0 ;
	    if ((op->fmip = new(nt) bpi_fmi) != np) {
	        if ((op->fhip = new(nt) bpihdr) != np) {
		    rs = SR_OK ;
		} /* end if (new-bvshdr) */
		if (rs < 0) {
		    delete op->fmip ;
		    op->fmip = nullptr ;
		} /* end if (error) */
	    } /* end if (new-fmi) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (bpi_ctor) */

local int bpi_dtor(bpi *op) noex {
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
} /* end subroutine (bpi_dtor) */

template<typename ... Args>
local inline int bpi_magic(bpi *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    rs = (op->magval == BPI_MAGIC) ? SR_OK : SR_NOTOPEN ;
	} /* end if */
	return rs ;
} /* end subroutine (bpi_magic) */

local int	bpi_loadbegin	(bpi *,time_t) noex ;
local int	bpi_loadend	(bpi *) noex ;
local int	bpi_mapcreate	(bpi *,time_t) noex ;
local int	bpi_mapdestroy	(bpi *) noex ;
local int	bpi_proc	(bpi *,time_t) noex ;
local int	bpi_verify	(bpi *,time_t) noex ;
local int	bpi_auditvt	(bpi *) noex ;
local int	bpi_checkupdate	(bpi *,time_t) noex ;
local int	bpi_search	(bpi *,bpi_q *) noex ;
local int	bpi_loadbve	(bpi *,bpi_v *,int) noex ;

local int	mkcitekey	(uint *,bpi_q *) noex ;
local int	vtecmp		(cvoid *,cvoid *) noex ;


/* local variables */

cuint		vmask		= bvcitekey_vmask ;
cbool		f_debug 	= CF_DEBUG ;
cbool		f_search	= CF_SEARCH ;


/* exported variables */

const bpi_obj bpi_modinfo = {
	"bpi",
	szof(bpi),
	szof(bpi_cur)
} ; /* end initialization */


/* exported subroutines */

int bpi_open(bpi *op,cchar *dbn) noex {
	int		rs ;
	int		rs1 ;
	int		nv = 0 ; /* return-value */
	if ((rs = bpi_ctor(op,dbn)) >= 0) {
	    rs = SR_INVALID ;
	    if (dbn[0]) {
	        custime		dt = time(nullptr) ;
	        if (cchar *cp ; (rs = mem.strw(dbn,-1,&cp)) >= 0) {
	            cchar	*suf = BPI_SUF ;
	            cchar	*end = ENDIANSTR ;
		    if (char *tbuf ; (rs = mem.mp(&tbuf)) >= 0) {
	    	        op->dbname = cp ;
	    		if ((rs = mkfnamesuf2(tbuf,cp,suf,end)) >= 0) {
	                    cint tl = rs ;
	                    if ((rs = mem.strw(tbuf,tl,&cp)) >= 0) {
	            		op->fname = cp ;
	            		if ((rs = bpi_loadbegin(op,dt)) >= 0) {
	                	    op->ti_lastcheck = dt ;
	                	    op->magval = BPI_MAGIC ;
	            		} /* end if (bpi_loadbegin) */
	            		if (rs < 0) {
				    voidp vp = voidp(op->fname) ;
	              		    mem.free(vp) ;
	              		    op->fname = nullptr ;
	            		} /* end if (error) */
	        	    } /* end if (memory-acquire) */
	    		} /* end if (mkfnamesuf) */
		        rs1 = mem.free(tbuf) ;
		        if (rs >= 0) rs = rs1 ;
		    } /* end if (m-a-f) */
	   	    if (rs < 0) {
			voidp vp = voidp(op->dbname) ;
	        	mem.free(vp) ;
	        	op->dbname = nullptr ;
	   	    } /* end if (error) */
		} /* end if (memory-acquire) */
	    } /* end if (valid) */
	    if (rs < 0) {
		bpi_dtor(op) ;
	    } /* end if (error) */
	} /* end if (bpi_ctor) */
	return (rs >= 0) ? nv : rs ;
} /* end subroutine (bpi_open) */

int bpi_close(bpi *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = bpi_magic(op)) >= 0) ylikely {
	    {
	        rs1 = bpi_loadend(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    if (op->fname) {
	        voidp vp = voidp(op->fname) ;
	        rs1 = mem.free(vp) ;
	        if (rs >= 0) rs = rs1 ;
	        op->fname = nullptr ;
	    } /* end if (memory-release) */
	    if (op->dbname) {
	        voidp vp = voidp(op->dbname) ;
	        rs1 = mem.free(vp) ;
	        if (rs >= 0) rs = rs1 ;
	        op->dbname = nullptr ;
	    } /* end if (memory-release) */
	    {
	        rs1 = bpi_dtor(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    op->magval = 0 ;
	} /* end if (bpi_magic) */
	return rs ;
} /* end subroutine (bpi_close) */

int bpi_audit(bpi *op) noex {
	int		rs ;
	if ((rs = bpi_magic(op)) >= 0) ylikely {
	    /* verify that all list pointers and list entries are valid */
	    rs = bpi_auditvt(op) ;
	} /* end if (bpi_magic) */
	return rs ;
} /* end subroutine (bpi_audit) */

int bpi_count(bpi *op) noex {
	int		rs ;
	int		nv = 0 ; /* return-value */
	if ((rs = bpi_magic(op)) >= 0) ylikely {
	    bpihdr	*hip = op->fhip ;
	    nv = hip->nverses ;
	} /* end if (bpi_magic) */
	return (rs >= 0) ? nv : rs ;
} /* end subroutine (bpi_count) */

int bpi_getinfo(bpi *op,bpi_info *ip) noex {
	int		rs ;
	int		nv = 0 ; /* return-value */
	if ((rs = bpi_magic(op)) >= 0) ylikely {
	    bpihdr	*hip = op->fhip ;
	    if (ip) {
		bpi_fmi *fip = op->fmip ;
	        memclear(ip) ;
	        ip->mtime	= fip->ti_mod ;
	        ip->ctime	= time_t(hip->wtime) ;
	        ip->maxbook	= hip->maxbook ;
	        ip->maxchap	= hip->maxchap ;
	        ip->count	= hip->nverses ;
	        ip->nzverses	= hip->nzverses ;
	    } /* end if (non-null) */
	    nv = hip->nverses ;
	} /* end if (bpi_magic) */
	return (rs >= 0) ? nv : rs ;
} /* end subroutine (bpi_getinfo) */

int bpi_get(bpi *op,bpi_q *qp) noex {
	int		rs ;
	int		vi = 0 ; /* return-value */
	if ((rs = bpi_magic(op,qp)) >= 0) ylikely {
	    /* check for update */
	    if (op->ncursors == 0) {
	        rs = bpi_checkupdate(op,0) ;
	    }
	    if (rs >= 0) {
	        rs = bpi_search(op,qp) ;
	        vi = rs ;
	    } /* end if (ok) */
	} /* end if (bpi_magic) */
	return (rs >= 0) ? vi : rs ;
} /* end subroutine (bpi_get) */

int bpi_curbegin(bpi *op,bpi_cur *curp) noex {
    	int		rs ;
	if ((rs = bpi_magic(op,curp)) >= 0) ylikely {
	    curp->i = 0 ;
	    op->ncursors += 1 ;
	} /* end if (bpi_magic) */
	return rs ;
} /* end subroutine (bpi_curbegin) */

int bpi_curend(bpi *op,bpi_cur *curp) noex {
    	int		rs ;
	if ((rs = bpi_magic(op,curp)) >= 0) ylikely {
	    curp->i = 0 ;
	    if (op->ncursors > 0) {
	        op->ncursors -= 1 ;
	    }
	} /* end if (bpi_magic) */
	return rs ;
} /* end subroutine (bpi_curend) */

int bpi_curenum(bpi *op,bpi_cur *curp,bpi_v *bvep) noex {
	int		rs ;
	if ((rs = bpi_magic(op,curp,bvep)) >= 0) ylikely {
	    bpihdr	*hip = op->fhip ;
	    rs = SR_INVALID ;
	    if (op->ncursors > 0) {
		int	vi = (curp->i < 0) ? 0 : (curp->i + 1) ;
		int	vtlen = hip->vilen ;
		rs = SR_NOTFOUND ;
		if (vi < vtlen) {
	            if ((rs = bpi_loadbve(op,bvep,vi)) >= 0) {
	    	        curp->i = vi ;
	            }
	        } /* end if (valid) */
	    } /* end if (valid) */
	} /* end if (bpi_magic) */
	return rs ;
} /* end subroutine (bpi_curenum) */


/* private subroutines */

local int bpi_loadbegin(bpi *op,time_t daytime) noex {
	int		rs ;
	int		nv = 0 ; /* return-value */
	if ((rs = bpi_mapcreate(op,daytime)) >= 0) ylikely {
	    rs = bpi_proc(op,daytime) ;
	    nv = rs ;
	    if (rs < 0) {
		bpi_mapdestroy(op) ;
	    } /* end if (error) */
	} /* end if (map) */
	return (rs >= 0) ? nv : rs ;
} /* end subroutine (bpi_loadbegin) */

local int bpi_loadend(bpi *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	{
	    rs1 = bpi_mapdestroy(op) ;
	    if (rs >= 0) rs = rs1 ;
	}
	{
	    bpi_fmi	*mip = op->fmip ;
	    mip->vt = nullptr ;
	}
	return rs ;
} /* end subroutine (bpi_loadend) */

local int bpi_mapcreate(bpi *op,time_t daytime) noex {
    	cnullptr	np{} ;
	int		rs = SR_BUGCHECK ;
	int		rs1 ;
	if (op->fname) ylikely {
	    bpi_fmi	*mip = op->fmip ;
	    if ((rs = u_open(op->fname,O_RDONLY,0666)) >= 0) ylikely {
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
	                    mip->ti_map = daytime ;
	                } /* end if (u_mmap) */
	            } else {
		        rs = SR_UNATCH ;
		    }
	        } /* end if (stat) */
	        rs1 = u_close(fd) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (file-open) */
	} /* end if (bug-check) */
	return rs ;
} /* end subroutine (bpi_mapcreate) */

local int bpi_mapdestroy(bpi *op) noex {
	bpi_fmi		*mip = op->fmip ;
	int		rs = SR_OK ;
	if (mip->mapdata) {
	    csize ms = mip->mapsize ;
	    voidp md = mip->mapdata ;
	    rs = u_munmap(md,ms) ;
	    mip->mapdata = nullptr ;
	    mip->mapsize = 0 ;
	    mip->ti_map = 0 ;
	} /* end if (un-map) */
	return rs ;
} /* end subroutine (bpi_mapdestroy) */

local int bpi_checkupdate(bpi *op,time_t dt) noex {
	int		rs = SR_OK ;
	int		f = false ; /* return-value */
	if (op->ncursors == 0) {
	    if (dt <= 0) dt = time(nullptr) ;
	    if ((dt - op->ti_lastcheck) >= TO_CHECK) {
	        bpi_fmi		*mip = op->fmip ;
	        op->ti_lastcheck = dt ;
	        if (ustat sb ; (rs = u_stat(op->fname,&sb)) >= 0) {
	            f = f || (sb.st_mtime > mip->ti_mod) ;
	            f = f || (sb.st_mtime > mip->ti_map) ;
	            if (f) {
	                bpi_loadend(op) ;
	                rs = bpi_loadbegin(op,dt) ;
	            } /* end if (update) */
	        } else if (isNotPresent(rs)) {
	            rs = SR_OK ;
	        }
	    } /* end if (needed checking) */
	} /* end if (cursor-open) */
	return (rs >= 0) ? f : rs ;
} /* end subroutine (bpi_checkupdate) */

local int bpi_proc(bpi *op,time_t dt) noex {
	int		rs = SR_BUGCHECK ;
	int		nv = 0 ; /* return-value */
	if (bpi_fmi *mip = op->fmip ; mip) ylikely {
	    cint msz = intconv(mip->mapsize) ;
	    if (bpihdr *hip = op->fhip ; hip) ylikely {
	        if ((rs = hip->wr(mip->mapdata,msz)) >= 0) {
	            if ((rs = bpi_verify(op,dt)) >= 0) {
	                nv = hip->nverses ;
	                mip->vt = (uint (*)[1]) (mip->mapdata + hip->vioff) ;
	            }
	        } /* end if (bpihdr_wr) */
	    } /* end if (bug-check) */
	} /* end if (bug-check) */
	return (rs >= 0) ? nv : rs ;
} /* end subroutine (bpi_proc) */

local int bpi_verify(bpi *op,time_t dt) noex {
	int		rs = SR_BUGCHECK ;
	if (bpi_fmi *mip = op->fmip ; mip) ylikely {
	    if (bpihdr *hip = op->fhip ; hip) ylikely {
		cuint	utime = uint(dt) ;
		cuint	msz = uintconv(mip->mapsize) ;
		cint	si = SHIFTINT ;
		int	sz{} ;
		bool	f = true ;
		rs = SR_OK ;
	        f = f && (hip->fsz == msz) ;
	        f = f && (hip->wtime > 0) && (hip->wtime <= (utime + si)) ;
	        /* alignment restriction */
	        f = f && ((hip->vioff & (szof(int) - 1)) == 0) ;
	        /* size restrictions */
	        f = f && (hip->vioff <= msz) ;
	        sz = (hip->vilen * 1) * szof(uint) ;
	        f = f && ((hip->vioff + sz) <= msz) ;
	        /* something restriction? */
	        f = f && (hip->vilen == hip->nverses) ;
	        /* get out */
	        if (! f) {
	            rs = SR_BADFMT ;
	        }
	    } /* end if (bug-check) */
	} /* end if (bug-check) */
	return rs ;
} /* end subroutine (bpi_verify) */

local int bpi_auditvt(bpi *op) noex {
	int		rs = SR_BUGCHECK ;
	if (bpi_fmi *mip = op->fmip ; mip) ylikely {
	    if (bpihdr *hip = op->fhip ; hip) ylikely {
	        uint	(*vt)[1] = mip->vt ;
	        uint	pcitcmpval = 0 ;
		int	vilen = int(hip->vilen) ;
		rs = SR_OK ;
	        /* "verses" table */
	        for (int i = 1 ; (rs >= 0) && (i < vilen) ; i += 1) {
	            cuint citcmpval = vt[i][0] & vmask ;
	            if (citcmpval < pcitcmpval) {
	                rs = SR_BADFMT ;
	                break ;
	            }
	            pcitcmpval = citcmpval ;
	        } /* end for (record table entries) */
	    } /* end if (bug-check) */
	} /* end if (bug-check) */
	return rs ;
} /* end subroutine (bpi_auditvt) */

local int bpi_search(bpi *op,bpi_q *qp) noex {
	bpi_fmi		*mip = op->fmip ;
	bpihdr		*hip = op->fhip ;
	uint		(*vt)[1] ;
	uint		citekey ;
	uint		vte[1] ;
	int		rs = SR_OK ;
	int		vtlen ;
	int		vi = 0 ; /* return-value */
	vt = mip->vt ;
	vtlen = hip->vilen ;
	/* search for entry */
	mkcitekey(&citekey,qp) ;
	vte[0] = citekey ;
	if_constexpr (f_search) {
	    uint	*vtep ;
	    int		vtesize = (1 * szof(uint)) ;
	    vtep = (uint *) bsearch(vte,vt,vtlen,vtesize,vtecmp) ;
	    rs = (vtep) ? intconv((vtep - vt[0]) >> 2) : SR_NOTFOUND ;
	    vi = rs ;
	} else {
	    for (vi = 0 ; vi < vtlen ; vi += 1) {
	        if ((vt[vi][0] & vmask) == citekey)
		    break ;
	    } /* end for */
	    rs = (vi < vtlen) ? vi : SR_NOTFOUND ;
	} /* end if_constexpr (f_search) */
	return (rs >= 0) ? vi : rs ;
} /* end subroutine (bpi_search) */

local int bpi_loadbve(bpi *op,bpi_v *bvep,int vi) noex {
	bpi_fmi		*mip = op->fmip ;
	bpihdr		*hip = op->fhip ;
	int		rs = SR_FAULT ;
	if (op && bvep) {
	    rs = memclear(bvep) ;
	    if (cint vtlen = int(hip->vilen) ; vi >= vtlen) {
	        rs = SR_NOANODE ;
	    }
	    /* load the basic stuff */
	    if (rs >= 0) {
	        uint *vte = mip->vt[vi] ;
	        bvep->nlines	= getbyte(vte[0],3) ;
	        bvep->b		= getbyte(vte[0],2) ;
	        bvep->c		= getbyte(vte[0],1) ;
	        bvep->v		= getbyte(vte[0],0) ;
	    } /* end if (ok) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (bpi_loadbve) */

local uint mkciteload(uint ci,uchar item) noex {
	ci = (ci << UCHAR_BIT) ;
	ci |= uint(item) ;
	return ci ; 
} /* end subroutine (mkciteload) */

local int mkcitekey(uint *cip,bpi_q *bvp) noex {
    	int		rs = SR_BUGCHECK ;
	if (cip && bvp) ylikely {
	    uint	ci = 0 ;
	    rs = SR_OK ;
	    ci = mkciteload(ci,bvp->b) ;
	    ci = mkciteload(ci,bvp->c) ;
	    ci = mkciteload(ci,bvp->v) ;
	    *cip = ci ;
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (mkcitekey) */

local int vtecmp(cvoid *v1p,cvoid *v2p) noex {
	uint		*vte1 = uintp(v1p) ;
	uint		*vte2 = uintp(v2p) ;
	int		rc = 0 ;
	{
	    cint c1 = int(vte1[0] & vmask) ;
	    cint c2 = int(vte2[0] & vmask) ;
	    rc = (c1 - c2) ;
	}
	return rc ;
} /* end subroutine (vtecmp) */


