/* bvses SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* read or audit a bvses (Bible Verse Structure) database */
/* version %I% last-modified %G% */

#define	CF_DEBUG	0		/* compile-time debugging */

/* revision history:

	= 2008-10-01, David A­D­ Morano
	This module was originally written.

*/

/* Copyright © 2008 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	bvses

	Description:
	This subroutine opens and allows for reading or auditing
	of a bvses database.

	Synopsis:
	int bvses_open(bvses *op,cchar *pr,cchar *dbname) noex

	Arguments:
	- op		object pointer
	- pr		program root
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
#include	<climits>		/* CSTD */
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
#include	<vecstr.h>		/* LIBUC */
#include	<mkpathx.h>		/* LIBUC */
#include	<sncpyx.h>		/* LIBUC */
#include	<char.h>		/* LIBUC */
#include	<isnot.h>		/* LIBUC */
#include	<mkchar.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */
#include	<libdebug.h>		/* LIBDEBUG |DEBUGPRINTF(3debug)| */

#include	"bvshdr.h"
#include	"bvsbook.h"
#include	"bvses.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |memclear(3u)| */

/* local defines */

#define	BVSES_IDNAME	"var/bvses"

#define	SHIFTINT	(6 * 60)	/* possible time-shift */

#define	TO_CHECK	4

#ifndef	CF_DEBUG
#define	CF_DEBUG	0		/* compile-time debugging */
#endif


/* imported namespaces */
using libuc::mem ;			/* variable */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

struct bventry {
	uint	voff ;
	uint	vlen ;
	uint	li ;			/* index-number of first line-entry */
	uint	citation ;		/* (nlines, b, c, v) */
} ; /* end struct */

struct blentry {
	uint	loff ;
	uint	llen ;
} ; /* end struct */


/* forward references */

template<typename ... Args>
local inline int bvses_ctor(bvses *op,Args ... args) noex {
    	cnullptr	np{} ;
	cnothrow	nt{} ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    rs = SR_NOMEM ;
	    op->pr	= np ;
	    op->dbname	= np ;
	    op->fname	= np ;
	    op->fhip	= np ;
	    op->magval	= 0 ;
	    if ((op->fmip = new(nt) bvses_fmi) != np) {
	        if ((op->fhip = new(nt) bvshdr) != np) {
		    rs = SR_OK ;
		} /* end if (new_bvshdr) */
		if (rs < 0) {
		    delete op->fmip ;
		    op->fmip = nullptr ;
		} /* end if (error) */
	    } /* end if (new-fmi) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (bvses_ctor) */

local int bvses_dtor(bvses *op) noex {
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
} /* end subroutine (bvses_dtor) */

template<typename ... Args>
local inline int bvses_magic(bvses *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    rs = (op->magval == BVSES_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
} /* end subroutine (bvses_magic) */

local int	bvses_loadbegin	(bvses *,time_t) noex ;
local int	bvses_loadend	(bvses *) noex ;
local int	bvses_mapbegin	(bvses *,time_t) noex ;
local int	bvses_mapend	(bvses *) noex ;
local int	bvses_proc	(bvses *,time_t) noex ;
local int	bvses_verify	(bvses *,time_t) noex ;
local int	bvses_auditbt	(bvses *) noex ;
local int	bvses_auditct	(bvses *) noex ;
local int	bvses_checkup	(bvses *,time_t) noex ;


/* local variables */


/* exported variables */

const bvses_obj		vcses_modinfo = {
	"bvses",
	szof(bvses),
	0
} ; /* end initializætion */

cbool			f_debug		= CF_DEBUG ;


/* exported subroutines */

local int bvses_opens(bvses *op) noex {
    	int		rs ;
	int		rs1 ;
	if (char *cbuf ; (rs = mem.mn(&cbuf)) >= 0) {
	    cint	clen = rs ;
	    cchar	*suf = BVSES_SUF ;
	    cchar	*end = ENDIANSTR ;
	    if ((rs = sncpy4(cbuf,clen,op->dbname,".",suf,end)) >= 0) {
	        cchar	*ind = BVSES_IDNAME ;
		if (char *tbuf ; (rs = mem.mp(&tbuf)) >= 0) {
	            if ((rs = mkpath3(tbuf,op->pr,ind,cbuf)) >= 0) {
	                int	tl = rs ;
	                if (cchar *cp ; (rs = mem.strw(tbuf,tl,&cp)) >= 0) {
	                    custime	dt = time(nullptr) ;
	                    op->fname = cp ;
	                    if ((rs = bvses_loadbegin(op,dt)) >= 0) {
	                        op->ti_lastcheck = dt ;
	                        op->magval = BVSES_MAGIC ;
	                    } /* end if (bvses_loadbegin) */
	                    if (rs < 0) {
	                        voidp vp = voidp(op->fname) ;
	                        mem.free(vp) ;
	                        op->fname = nullptr ;
	                    } /* end if (error) */
	                } /* end if (memory-allocation) */
	            } /* end if (mkpath) */
		    rs1 = mem.free(tbuf) ;
	            if (rs >= 0) rs = rs1 ;
		} /* end if (m-a-f) */
	    } /* end if (sncpy) */
	    rs1 = mem.free(cbuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return rs ;
} /* end subroutine (bvses_opens) */

int bvses_open(bvses *op,cchar *pr,cchar *dbname) noex {
	int		rs ;
	if ((rs = bvses_ctor(op,pr,dbname)) >= 0) {
	    rs = SR_INVALID ;
	    if (pr[0] && dbname[0]) {
	        if (cchar *cp ; (rs = mem.strw(pr,-1,&cp)) >= 0) {
	            op->pr = cp ;
	            if ((rs = mem.strw(dbname,-1,&cp)) >= 0) {
	                op->dbname = cp ;
		        {
		            rs = bvses_opens(op) ;
		        }
	                if (rs < 0) {
	                    voidp vp = voidp(op->dbname) ;
	                    mem.free(vp) ;
	                    op->dbname = nullptr ;
	                } /* end if (error) */
	            } /* end if (memory-allocation) */
	            if (rs < 0) {
	                voidp vp = voidp(op->pr) ;
	                mem.free(vp) ;
	                op->pr = nullptr ;
	            } /* end if (error) */
	        } /* end if (memory-allocation) */
	    } /* end if (valid) */
	    if (rs < 0) {
		bvses_dtor(op) ;
	    } /* end if (error) */
	} /* end if (bvses_ctor) */
	return rs ;
} /* end subroutine (bvses_open) */

int bvses_close(bvses *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = bvses_magic(op)) >= 0) {
	    {
	        rs1 = bvses_loadend(op) ;
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
	    if (op->pr) {
	        voidp vp = voidp(op->pr) ;
	        rs1 = mem.free(vp) ;
	        if (rs >= 0) rs = rs1 ;
	        op->pr = nullptr ;
	    } /* end if (memory-release) */
	    {
		rs1 = bvses_dtor(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    op->magval = 0 ;
	} /* end if (magic) */
	return rs ;
} /* end subroutine (bvses_close) */

int bvses_audit(bvses *op) noex {
	int		rs ;
	if ((rs = bvses_magic(op)) >= 0) {
	    /* verify that all list pointers and list entries are valid */
	    if ((rs = bvses_auditbt(op)) >= 0) {
	        rs = bvses_auditct(op) ;
	    }
	} /* end if (magic) */
	return rs ;
} /* end subroutine (bvses_audit) */

int bvses_count(bvses *op) noex {
	int		rs ;
	int		nv = 0 ; /* return-value */
	if ((rs = bvses_magic(op)) >= 0) {
	    bvshdr	*hip = op->fhip ;
	    nv = hip->nverses ;
	} /* end if (magic) */
	return (rs >= 0) ? nv : rs ;
} /* end subroutine (bvses_count) */

int bvses_getinfo(bvses *op,BVSES_INFO *ip) noex {
	int		rs ;
	int		nv = 0 ; /* return-value */
	if ((rs = bvses_magic(op)) >= 0) {
	    bvshdr	*hip = op->fhip ;
	    if (ip) {
		bvses_fmi	*fmip = op->fmip ;
	        memclear(ip) ;
	        ip->mtime	= fmip->ti_mod ;
	        ip->ctime	= time_t(hip->wtime) ;
	        ip->nzbooks	= hip->nzbooks ;
	        ip->nbooks	= hip->btlen ;
	        ip->nchapters	= hip->ctlen ;
	        ip->nverses	= hip->nverses ;
	        ip->nzverses	= hip->nzverses ;
	    } /* end if */
	    nv = hip->nverses ;
	} /* end if (non-null) */
	return (rs >= 0) ? nv : rs ;
} /* end subroutine (bvses_getinfo) */

local int bvses_mkmodqs(bvses *op,bvses_v *bvep,int mjd) noex {
	bvses_fmi	*mip = op->fmip ;
	bvshdr		*hip = op->fhip ;
	ushort		(*bt)[4] ;
	uint		nzverses ;	/* non-zero verses */
	uint		vc ;		/* verse-count */
	uchar		*ct ;
	int		rs = SR_OK ;
	int		vi = 0 ; /* return-value */
	uchar		b, c ;		/* book-chapter */
	uchar		ci ;		/* chapter-index */
	uchar		cl ;		/* chapter-length */
	bt = mip->bt ;
	/* perform the calculation */
	vi = uchar(mjd % hip->nzverses) ;
	vc = vi ;
	for (b = 1 ; b < hip->btlen ; b += 1) {
	    if (bvsbook be ; (rs = bvsbook_get(&be,bt[b])) >= 0) {
	        if (be.al > 0) {
	            ci = be.ci ;
	            ct = (mip->ct + ci) ;
	            nzverses = (be.nverses - ct[0]) ;
	            if (vc < nzverses) break ;
	            vc -= nzverses ;
	        } /* end if (non-zero positive) */
	    } /* end if (bvsbook_get) */
	    if (rs < 0) break ;
	} /* end for */
	DEBUGPRINTF("mid rs=%d b=%u vc=%u\n",rs,b,vc) ;
	if (rs >= 0) {
	    /* loop reducing 'v' to as close to *zero* as possible */
	    if (bvsbook be ; (rs = bvsbook_get(&be,bt[b])) >= 0) {
		cuchar al = be.al ;
	        ci = be.ci ;
	        cl = uchar(hip->ctlen) ;
	        ct = (mip->ct + ci) ;
	        for (c = 1 ; (c < cl) && (c < al) && (vc >= ct[c]) ; c += 1) {
	            vc -= ct[c] ;
	        } /* end for */
	        memclear(bvep) ;
	        bvep->b = uchar(b) ;
	        bvep->c = uchar(c) ;
	        bvep->v = uchar(vc + 1) ;
	    } /* end if (bvsbook_get) */
	} /* end if (ok) */
	return (rs >= 0) ? vi : rs ;
} /* end subroutine (bvses_mkmodqs) */

int bvses_mkmodq(bvses *op,bvses_v *bvep,int mjd) noex {
	int		rs ;
	int		vi = 0 ; /* return-value */
	DEBUGPRINTF("ent mjd=%u\n",mjd) ;
	if ((rs = bvses_magic(op,bvep)) >= 0) {
	    rs = SR_INVALID ;
	    if (mjd >= 0) {
		rs = SR_OK ;
	        /* check for update */
	        if (op->ncursors == 0) {
	            rs = bvses_checkup(op,0) ;
	        }
	        if (rs >= 0) {
		    rs = bvses_mkmodqs(op,bvep,mjd) ;
		    vi = rs ;
	        } /* end if (ok ) */
	    } /* end if (valid) */
	} /* end if (magic) */
	return (rs >= 0) ? vi : rs ;
} /* end subroutine (bvses_mkmodq) */


/* private subroutines */

local int bvses_loadbegin(bvses *op,time_t dt) noex {
	int		rs ;
	int		fsz = 0 ;
	if ((rs = bvses_mapbegin(op,dt)) >= 0) {
	    fsz = rs ;
	    rs = bvses_proc(op,dt) ;
	    if (rs < 0) {
	        bvses_mapend(op) ;
	    } /* end if (error) */
	} /* end if (loadbegin) */
	return (rs >= 0) ? fsz : rs ;
} /* end subroutine (bvses_loadbegin) */

local int bvses_loadend(bvses *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	{
	    rs1 = bvses_mapend(op) ;
	    if (rs >= 0) rs = rs1 ;
	}
	{
	    bvses_fmi	*mip = op->fmip ;
	    mip->bt = nullptr ;
	    mip->ct = nullptr ;
	}
	return rs ;
} /* end subroutine (bvses_loadend) */

local int bvses_mapbeginer(bvses *op,time_t dt) noex {
	cnullptr	np{} ;
	int		rs ;
	int		rs1 ;
	int		fsz = 0 ; /* return-value */
	if ((rs = u_open(op->fname,O_RDONLY,0666)) >= 0) {
	    cint	fd = rs ;
	    if (ustat sb ; (rs = u_fstat(fd,&sb)) >= 0) {
	        if (csize fsize = sb.st_size ; fsize > 0) {
	            csize	ms = fsize ;
	            cint	mp = PROT_READ ;
	            cint	mf = MAP_SHARED ;
	            void	*md ;
	            if ((rs = u_mmapbegin(np,ms,mp,mf,fd,0z,&md)) >= 0) {
			bvses_fmi *mip = op->fmip ;
	                mip->mapdata	= charp(md) ;
	                mip->mapsize	= ms ;
	                mip->ti_mod	= sb.st_mtime ;
	                mip->ti_map	= dt ;
			fsz = intconv(fsize) ;
	            } /* end if (u_mamp) */
	        } else {
	            rs = SR_UNATCH ;
		}
	    } /* end if (fstat) */
	    rs1 = u_close(fd) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (file-process) */
	return (rs >= 0) ? fsz : rs ;
} /* end subroutine (bvses_mapbeginer) */

local int bvses_mapbegin(bvses *op,time_t dt) noex {
	int		rs = SR_FAULT ;
	int		fsz = 0 ; /* return-value */
	if (op->fname) {
	    rs = SR_INVALID ;
	    if (op->fname[0]) {
		rs = bvses_mapbeginer(op,dt) ;
		fsz = rs ;
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? fsz : rs ;
} /* end subroutine (bvses_mapbegin) */

local int bvses_mapend(bvses *op) noex {
	bvses_fmi	*mip = op->fmip ;
	int		rs = SR_OK ;
	int		rs1 ;
	if (mip->mapdata) {
	    voidp	md = mip->mapdata ;
	    csize	ms = mip->mapsize ;
	    rs1 = u_mmapend(md,ms) ;
	    if (rs >= 0) rs = rs1 ;
	    mip->mapdata = nullptr ;
	    mip->mapsize = 0 ;
	    mip->ti_map = 0 ;
	} /* end if (un-map) */
	return rs ;
} /* end subroutine (bvses_mapend) */

local int bvses_checkup(bvses *op,time_t dt) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	int		f = false ; /* return-value */
	if (op->ncursors == 0) {
	    if (dt <= 0) dt = time(nullptr) ;
	    if ((dt - op->ti_lastcheck) >= TO_CHECK) {
	        bvses_fmi	*mip = op->fmip ;
	        op->ti_lastcheck = dt ;
	        if (ustat sb ; (rs = u_stat(op->fname,&sb)) >= 0) {
	            f = false ;
	            f = f || (sb.st_mtime > mip->ti_mod) ;
	            f = f || (sb.st_mtime > mip->ti_map) ;
	            if (f) {
			{
	                    rs1 = bvses_loadend(op) ;
	                    if (rs >= 0) rs = rs1 ;
			}
	                if (rs >= 0) {
	                    rs = bvses_loadbegin(op,dt) ;
	  	        }
	            } /* end if (update) */
		} else if(isNotPresent(rs)) {
		    rs = SR_OK ;
	        } /* end if (stat) */
	    } /* end if (time-out) */
	} /* end if (no cursors out) */
	return (rs >= 0) ? f : rs ;
} /* end subroutine (bvses_checkup) */

local int bvses_proc(bvses *op,time_t dt) noex {
	int		rs = SR_BUGCHECK ;
	if (bvses_fmi *mip = op->fmip ; mip) {
	    cint msz = intconv(mip->mapsize) ;
	    if (bvshdr *hip = op->fhip ; hip) {
	        if ((rs = hip->wr(mip->mapdata,msz)) >= 0) {
	            if ((rs = bvses_verify(op,dt)) >= 0) {
	                mip->bt = (ushort (*)[4])(mip->mapdata + hip->btoff) ;
	                mip->ct = (uchar *)	(mip->mapdata + hip->ctoff) ;
	            } /* end if */
	        } /* end if */
	    } /* end if (bug-check) */
	} /* end if (bug-check) */
	return rs ;
} /* end subroutine (bvses_proc) */

local int bvses_verify(bvses *op,time_t dt) noex {
	int		rs = SR_BUGCHECK ;
	if (bvses_fmi *mip = op->fmip ; mip) {
	    if (bvshdr *hip = op->fhip ; hip) {
	        cuint	dtime = uintconv(dt) ;
	        cuint	msz = uintconv(mip->mapsize) ;
	        cint	si = SHIFTINT ;
	        int	sz{} ;
	        bool	f = true ;
	        DEBUGPRINTF("ent nverses=%u\n",hip->nverses) ;
	        f = f && (hip->nverses > 0) ;
	        f = f && (hip->fsz == msz) ;
	        f = f && (hip->wtime > 0) && (hip->wtime <= (dtime + si)) ;
	        /* alignment restriction */
	        f = f && ((hip->btoff & (szof(uint) - 1)) == 0) ;
	        /* size restrictions */
	        f = f && (hip->btoff <= msz) ;
	        sz = hip->btlen * 4 * szof(ushort) ;
	        f = f && ((hip->btoff + sz) <= msz) ;
	        /* size restrictions */
	        f = f && (hip->ctoff <= msz) ;
	        sz = hip->ctlen * 1 * szof(uchar) ;
	        /* size restrictions */
	        f = f && (hip->btlen <= hip->ctlen) ;
	        /* get out */
	        if (! f) {
	            rs = SR_BADFMT ;
	        }
	    } /* end if (bug-check) */
	} /* end if (bug-check) */
	DEBUGPRINTF("ret rs=%d\n",rs) ;
	return rs ;
} /* end subroutine (bvses_verify) */

local int bvses_auditbt(bvses *op) noex {
	bvses_fmi	*mip = op->fmip ;
	bvshdr		*hip = op->fhip ;
	int		rs = SR_OK ;
	{
	    ushort	(*bt)[4] = mip->bt ;
	    /* "book" table */
	    for (uint i = 0 ; (rs >= 0) && (i < hip->btlen) ; i += 1) {
	        if (bvsbook be ; (rs = bvsbook_get(&be,bt[i])) >= 0) {
	            if ((rs >= 0) && (be.al >= hip->ctlen)) {
	                rs = SR_BADFMT ;
	            }
	            if ((rs >= 0) && (be.ci >= hip->ctlen)) {
	                rs = SR_BADFMT ;
	            }
	            if ((rs >= 0) && (be.nverses >= hip->nverses)) {
	                rs = SR_BADFMT ;
	            }
	        } /* end if */
	    } /* end for (record table entries) */
	} /* end block */
	return rs ;
} /* end subroutine (bvses_auditbt) */

local int bvses_auditct(bvses *op) noex {
	bvses_fmi	*mip = op->fmip ;
	bvshdr		*hip = op->fhip ;
	int		rs = SR_OK ;
	{
	    uchar	*ct = mip->ct ;
	    uint	nv = 0 ;
	    for (uint i = 0 ; (rs >= 0) && (i < hip->ctlen) ; i += 1) {
	        nv += int(ct[i] & UCHAR_MAX) ;
	    } /* end for (record table entries) */
	    if ((rs >= 0) && (nv > hip->nverses)) {
	        rs = SR_BADFMT ;
	    }
	} /* end block */
	return rs ;
} /* end subroutine (bvses_auditct) */


