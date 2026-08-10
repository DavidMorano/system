/* cmi SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* read or audit a ComMandment Index (CMI) database */
/* version %I% last-modified %G% */

#define	CF_SEARCH	1		/* use |bsearch(3c)| */

/* revision history:

	= 1998-03-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	cmi

	Descriptiom:
	This subroutine opens and allows for reading or auditing
	of a CMI (ComMandment Index) database.

	Synopsis:
	int cmi_open(CMI *op,cchar *dbname) noex

	Arguments:
	- op		object pointer
	- dbname	name of (path-to) DB

	Returns:
	>=0		OK
	<0		error code (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<sys/mman.h>		/* POSIX */
#include	<ctime>			/* CSTD */
#include	<climits>		/* CSTD |USHORT_MAX| + |UINT_MAX| */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstring>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<endian.h>		/* LIBU */
#include	<intsat.h>		/* LIBU */
#include	<ucmem.h>		/* LIBUC */
#include	<mkfname.h>		/* LIBUC */
#include	<char.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */
#include	<bvcitekey.h>		/* LIBDAM */

#include	"cmi.h"
#include	"cmihdr.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |memclear(3u)| */

/* local defines */

#define	CMI_KA		szof(cmi_line)
#define	CMI_BO(v)	((CMI_KA - ((v) % CMI_KA)) % CMI_KA)

#define	SHIFTINT	(6 * 60)	/* possible time-shift */

#define	TO_CHECK	4

#ifndef	CF_SEARCH
#define	CF_SEARCH	1		/* use |bsearch(3c)| */
#endif


/* imported namespaces */

using libuc::mem ;			/* variable */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

template<typename ... Args>
local int cmi_ctor(cmi *op,Args ... args) noex {
	CMI		*hop = op ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    rs = memclear(hop) ;
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (cmi_ctor) */

local int cmi_dtor(cmi *op) noex {
	int		rs = SR_FAULT ;
	if (op) ylikely {
	    rs = SR_OK ;
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (cmi_dtor) */

template<typename ... Args>
local inline int cmi_magic(cmi *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    rs = (op->magval == CMI_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
} /* end subroutine (cmi_magic) */

local int	cmi_loadbegin	(cmi *,time_t) noex ;
local int	cmi_loadend	(cmi *) noex ;
local int	cmi_mapcreate	(cmi *,time_t) noex ;
local int	cmi_mapdestroy	(cmi *) noex ;
local int	cmi_proc	(cmi *,time_t) noex ;
local int	cmi_verify	(cmi *,time_t) noex ;
local int	cmi_auditvt	(cmi *) noex ;
local int	cmi_checkupdate	(cmi *,time_t) noex ;
local int	cmi_search	(cmi *,uint) noex ;
local int	cmi_loadcmi	(cmi *,cmi_ent *,char *,int,int) noex ;

extern "C" {
    local int	vtecmp(cvoid *,cvoid *) noex ;
}


/* local variables */

constexpr bool		f_search = CF_SEARCH ;


/* exported variables */

const cmi_obj		cmi_modinfo = {
	"cmi",
	szof(cmi),
	szof(cmi_cur)
} ; /* end array */


/* exported subroutines */

local int	cmi_opener(cmi *,cchar *) noex ;

int cmi_open(cmi *op,cchar *dbname) noex {
	int		rs ;
	int		nents = 0 ;
	if ((rs = cmi_ctor(op,dbname)) >= 0) ylikely {
	    rs = SR_INVALID ;
	    if (dbname[0]) {
		rs = cmi_opener(op,dbname) ;
		nents = rs ;
	    } /* end if (valid) */
	    if (rs < 0) {
		cmi_dtor(op) ;
	    } /* end if (error) */
	} /* end if (cmi_ctor) */
	return (rs >= 0) ? nents : rs ;
} /* end subroutine (cmi_open) */

local int cmi_opener(cmi *op,cchar *dbn) noex {
    	custime		dt = getustime ;
    	int		rs ;
	int		rs1 ;
	int		nents = 0 ; /* return-value */
	if (cchar *cp ; (rs = mem.strw(dbn,-1,&cp)) >= 0) ylikely {
	    op->dbname = cp ;
	    if (char *tbuf ; (rs = mem.mp(&tbuf)) >= 0) ylikely {
	        cchar	*es = ENDIANSTR ;
	        cchar	*suf = CMI_SUF ;
	        if ((rs = mkfnamesuf2(tbuf,op->dbname,suf,es)) >= 0) ylikely {
	            cint	tl = rs ;
	            if ((rs = mem.strw(tbuf,tl,&cp)) >= 0) ylikely {
	                op->fname = cp ;
	                if ((rs = cmi_loadbegin(op,dt)) >= 0) ylikely {
	                    nents = rs ;
	                    op->ti_lastcheck = dt ;
	                    op->magval = CMI_MAGIC ;
	                } /* end if (loadbegin) */
	                if (rs < 0) {
	                    if (op->fname) {
	                        voidp vp = voidp(op->fname) ;
	                        mem.free(vp) ;
	                        op->fname = nullptr ;
	                    } /* end if (memory-release) */
	                } /* end if (error) */
	            } /* end if (memory-allocation) */
	        } /* end if (mkfnamesuf2) */
	        if (rs < 0) {
	            if (op->dbname) {
	                voidp vp = voidp(op->dbname) ;
	                mem.free(vp) ;
	                op->dbname = nullptr ;
	            } /* end if (memory-release) */
	        } /* end if (error) */
		rs1 = mem.free(tbuf) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (m-a-f) */
	} /* end if (memory-allocation) */
	return (rs >= 0) ? nents : rs ;
} /* end subroutine (cmi_opener) */

int cmi_close(cmi *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = cmi_magic(op)) >= 0) ylikely {
	    {
	        rs1 = cmi_loadend(op) ;
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
		rs1 = cmi_dtor(op) ;
		if (rs >= 0) rs = rs1 ;
	    }
	    op->magval = 0 ;
	} /* end if (magic) */
	return rs ;
} /* end subroutine (cmi_close) */

int cmi_audit(cmi *op) noex {
	int		rs ;
	if ((rs = cmi_magic(op)) >= 0) ylikely {
	    rs = cmi_auditvt(op) ;
	} /* end if (magic) */
	return rs ;
} /* end subroutine (cmi_audit) */

int cmi_count(cmi *op) noex {
	int		rs ;
	int		ne = 0 ;
	if ((rs = cmi_magic(op)) >= 0) {
	    cmihdr	*hip = &op->fhi ;
	    ne = hip->nents ;
	} /* end if (magic) */
	return (rs >= 0) ? ne : rs ;
} /* end subroutine (cmi_count) */

int cmi_getinfo(cmi *op,cmi_info *ip) noex {
	int		rs ;
	int		ne = 0 ;
	if ((rs = cmi_magic(op)) >= 0) ylikely {
	    if (ip) {
	        cmihdr	*hip = &op->fhi ;
	        rs = memclear(ip) ;
	        ip->idxmtime = op->fmi.ti_mod ;
	        ip->idxctime = (time_t) hip->idxtime ;
	        ip->dbtime = (time_t) hip->dbtime ;
	        ip->dbsize = (size_t) hip->dbsize ;
	        ip->idxsize = (size_t) hip->idxsize ;
	        ip->nents = hip->nents ;
	        ip->maxent = hip->maxent ;
		ne = hip->nents ;
	    } /* end if (non-null) */
	} /* end if (magic) */
	return (rs >= 0) ? ne : rs ;
} /* end subroutine (cmi_getinfo) */

int cmi_read(cmi *op,cmi_ent *bvep,char *vbuf,int vlen,uint cn) noex {
	int		rs ;
	int		vi = 0 ;
	if ((rs = cmi_magic(op,bvep,vnif)) >= 0) ylikely {
	    if ((rs >= 0) && (op->ncursors == 0)) {
	        rs = cmi_checkupdate(op,0) ;
	    }
	    if (rs >= 0) ylikely {
	        if ((rs = cmi_search(op,cn)) >= 0) {
	            vi = rs ;
	            rs = cmi_loadcmi(op,bvep,vbuf,vlen,vi) ;
	        }
	    } /* end if (ok) */
	} /* end if (magic) */
	return (rs >= 0) ? vi : rs ;
} /* end subroutine (cmi_read) */

int cmi_curbegin(cmi *op,cmi_cur *curp) noex {
	int		rs ;
	if ((rs = cmi_magic(op,curp)) >= 0) {
	    curp->i = 0 ;
	    op->ncursors += 1 ;
	} /* end if (magic) */
	return rs ;
} /* end subroutine (cmi_curbegin) */

int cmi_curend(cmi *op,cmi_cur *curp) noex {
	int		rs ;
	if ((rs = cmi_magic(op,curp)) >= 0) ylikely {
	    curp->i = 0 ;
	    if (op->ncursors > 0) {
	        op->ncursors -= 1 ;
	    }
	} /* end if (magic) */
	return rs ;
} /* end subroutine (cmi_curend) */

int cmi_curenum(cmi *op,cmi_cur *curp,cmi_ent *bvep,char *vbuf,int vlen) noex {
	int		rs ;
	int		nlines = 0 ; /* return-value */
	if ((rs = cmi_magic(op,curp,bvep,vbuf)) >= 0) ylikely {
	    rs = SR_INVALID ;
	    if (op->ncursors > 0) {
	        cmihdr	*hip = &op->fhi ;
	        int	vi = (curp->i < 0) ? 0 : (curp->i + 1) ;
	        if (vi < hip->vilen) {
	            if ((rs = cmi_loadcmi(op,bvep,vbuf,vlen,vi)) >= 0) {
		        nlines = rs ;
	                curp->i = vi ;
	            }
	        } else {
	            rs = SR_NOTFOUND ;
	        }
	    } /* end if (valid) */
	} /* end if (cmi_magic) */
	return (rs >= 0) ? nlines : rs ;
} /* end subroutine (cmi_curenum) */


/* private subroutines */

local int cmi_loadbegin(cmi *op,time_t dt) noex {
	int		rs ;
	int		nents = 0 ;
	if ((rs = cmi_mapcreate(op,dt)) >= 0) ylikely {
	    rs = cmi_proc(op,dt) ;
	    nents = rs ;
	    if (rs < 0) {
	        cmi_mapdestroy(op) ;
	    } /* end if (error) */
	} /* end if */
	return (rs >= 0) ? nents : rs ;
} /* end subroutine (cmi_loadbegin) */

local int cmi_loadend(cmi *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	{
	    rs1 = cmi_mapdestroy(op) ;
	    if (rs >= 0) rs = rs1 ;
	}
	{
	    cmi_fmi	*mip = &op->fmi ;
	    mip->vt = nullptr ;
	    mip->lt = nullptr ;
	}
	return rs ;
} /* end subroutine (cmi_loadend) */

local int cmi_mapcreate(cmi *op,time_t dt) noex {
	cmi_fmi		*mip = &op->fmi ;
	cnullptr	np{} ;
	int		rs = SR_BUGCHECK ;
	int		rs1 ;
	if (op->fname) ylikely {
	    cint	of = O_RDONLY ;
	    cmode	om = 0666 ;
	    if ((rs = u_open(op->fname,of,om)) >= 0) ylikely {
	        cint	fd = rs ;
	        if (ustat sb ; (rs = u_fstat(fd,&sb)) >= 0) {
	            csize	fsize = size_t(sb.st_size) {
	            if (fsize > 0) {
	                csize	ms = fsize ;
	                cint	mp = PROT_READ ;
	                cint	mf = MAP_SHARED ;
	                void	*md ;
	                if ((rs = u_mmapbegin(np,ms,mp,mf,fd,0z,&md)) >= 0) {
	                    mip->mapdata = md ;
	                    mip->mapsize = ms ;
	                    mip->ti_mod = sb.st_mtime ;
	                    mip->ti_map = dt ;
	                    rs = intsat(fsize) ;
	                } /* end if (mmap) */
	            } else {
	                rs = SR_UNATCH ;
	            }
	        } /* end if (stat) */
	        rs1 = u_close(fd) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (open) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (cmi_mapcreate) */

local int cmi_mapdestroy(cmi *op) noex {
	cmi_fmi		*mip = &op->fmi ;
	int		rs = SR_OK ;
	int		rs1 ;
	if (mip->mapdata) {
	    rs1 = u_munmap(mip->mapdata,mip->mapsize) ;
	    if (rs >= 0) rs = rs1 ;
	    mip->mapdata = nullptr ;
	    mip->mapsize = 0 ;
	    mip->ti_map = 0 ;
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (cmi_mapdestroy) */

local int cmi_checkupdate(cmi *op,time_t dt) noex {
	int		rs = SR_OK ;
	int		f = false ;
	if (op->ncursors == 0) {
	    if (dt <= 0) dt = getustime ;
	    if ((dt - op->ti_lastcheck) >= TO_CHECK) {
	        cmi_fmi		*mip = &op->fmi ;
	        op->ti_lastcheck = dt ;
	        if (ustat sb ; (rs = u_stat(op->fname,&sb)) >= 0) {
	            f = f || (sb.st_mtime > mip->ti_mod) ;
	            f = f || (sb.st_mtime > mip->ti_map) ;
	            if (f) {
	                cmi_loadend(op) ;
	                rs = cmi_loadbegin(op,dt) ;
	            } /* end if (update) */
	        } else if (isNotPresent(rs)) {
	            rs = SR_OK ;
	        }
	    } /* end if (time-checked out) */
	} /* end if (no cursors out) */
	return (rs >= 0) ? f : rs ;
} /* end subroutine (cmi_checkupdate) */

local int cmi_proc(cmi *op,time_t dt) noex {
	cmi_fmi		*mip = &op->fmi ;
	cmihdr		*hip = &op->fhi ;
	int		rs ;
	int		nents = 0 ;
	if ((rs = cmihdr(hip,1,mip->mapdata,mip->mapsize)) >= 0) {
	    if ((rs = cmi_verify(op,dt)) >= 0) {
	        mip->vt = (uint (*)[4]) (mip->mapdata + hip->vioff) ;
	        mip->lt = (uint (*)[2]) (mip->mapdata + hip->vloff) ;
	    }
	}
	return (rs >= 0) ? nents : rs ;
} /* end subroutine (cmi_proc) */

local int cmi_verify(cmi *op,time_t dt) noex {
	cmi_fmi		*mip = &op->fmi ;
	cmihdr		*hip = &op->fhi ;
	int		rs = SR_OK ;
	int		sz ;
	int		f = true ;
	f = f && (hip->idxsize == mip->mapsize) ;
	f = f && (hip->idxtime > 0) ;
	if (f) {
	    time_t	tt = (time_t) hip->idxtime ;
	    f = (dt >= tt) ;
	}
#ifdef	COMMENT
	{
	    const uint	utime = (uint) dt ;
	    f = f && (hip->idxtime <= (utime + SHIFTINT)) ;
	}
#endif
	/* alignment restriction */
	{
	f = f && ((hip->vioff & (szof(int) - 1)) == 0) ;
	}
	/* size restrictions */
	{
	f = f && (hip->vioff <= mip->mapsize) ;
	sz = hip->vilen * 4 * szof(uint) ;
	f = f && ((hip->vioff + sz) <= mip->mapsize) ;
	}
	/* alignment restriction */
	{
	f = f && ((hip->vloff & (szof(int) - 1)) == 0) ;
	}
	/* size restrictions */
	{
	f = f && (hip->vloff <= mip->mapsize) ;
	size = (hip->vllen * 2 * szof(uint)) ;
	f = f && ((hip->vloff + sz) <= mip->mapsize) ;
	}
	/* size restrictions */
	{
	f = f && (hip->vilen == hip->nents) ;
	}
	/* get out */
	if (! f) {
	    rs = SR_BADFMT ;
	}
	return rs ;
} /* end subroutine (cmi_verify) */

local int cmi_auditvt(cmi *op) noex {
	cmi_fmi		*mip = &op->fmi ;
	cmihdr		*hip = &op->fhi ;
	uint		(*vt)[4] ;
	uint		pcitcmpval = 0 ;
	uint		citcmpval ;
	int		rs = SR_OK ;
	int		li ;
	vt = mip->vt ;
	/* "verses" table */
	for (int i = 1 ; (rs >= 0) && (i < hip->vilen) ; i += 1) {
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
} /* end subroutine (cmi_auditvt) */

local int cmi_search(cmi *op,uint cn) noex {
	cmi_fmi		*mip = &op->fmi ;
	cmihdr		*hip = &op->fhi ;
	uint		(*vt)[4] ;
	uint		vte[4] ;
	uint		citekey = (uint) (cn & USHORT_MAX) ;
	int		rs = SR_OK ;
	int		vtlen ;
	int		vi = 0 ;
	vt = mip->vt ;
	vtlen = hip->vilen ;
	/* search for entry */
	vte[3] = citekey ;
	if_constexpr (f_search) {
	    uint	*vtep ;
	    csize	vtesize = (4 * szof(uint)) ;
	    vtep = (uint *) bsearch(vte,vt,vtlen,vtesize,vtecmp) ;
	    rs = (vtep != nullptr) ? ((vtep - vt[0]) >> 2) : SR_NOTFOUND ;
	    vi = rs ;
	} else {
	    for (vi = 0 ; vi < vtlen ; vi += 1) {
	        const ushort	vkey = ((vt[vi][3] >> 16) & USHORT_MAX) ;
	        if (vkey == citekey) break ;
	    } /* end for */
	    rs = (vi < vtlen) ? vi : SR_NOTFOUND ;
	} /* end if_constexpr (f_search) */
	return (rs >= 0) ? vi : rs ;
} /* end subroutine (cmi_search) */

local int cmi_loadcmi(cmi *op,cmi_ent *bvep,char *ebuf,int elen,int vi) noex {
	int		rs = SR_BUGCHECK ;
	int		nlines = 0 ; /* return-value */
	if (op && bvep && ebuf) ylikely {
	    rs = SR_OVERFLOW ;
	    if (elen > 0) {
	        cmi_line	*lines ;
	        cmi_fmi		*mip ;
	        cmihdr		*hip ;
	        ulong		uebuf = (ulong) ebuf ;
	        uint		*vte ;
	        uint		(*lt)[2] ;
	        uint		li ;
	        int		bo ;
	        int		linesz ;
	        rs = SR_OK ;
	        mip = &op->fmi ;
	        hip = &op->fhi ;
	        vte = mip->vt[vi] ;
	        /* load the basic stuff */
	        memclear(bvep) ;
	        bvep->eoff	= vte[0] ;
	        bvep->elen	= vte[1] ;
	        bvep->nlines	= conv<ushort>((vte[3] >> 16) & USHORT_MAX) ;
	        bvep->cn	= conv<ushort>((vte[3] >> 00) & USHORT_MAX) ;
	        /* load the lines */
	        li = vte[2] ;
	        nlines = bvep->nlines ;
	        if (li < hip->vllen) {
	            bo = CMI_BO(uebuf) ;
	            linesz = ((nlines + 1) * szof(cmi_line)) ;
	            if (linesz <= (elen - intconv(bo - uebuf))) {
		        int	i ; /* used-afterwards */
	                lt = (uint (*)[2]) (mip->mapdata + hip->vloff) ;
	                lines = (cmi_line *) (uebuf + bo) ;
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
} /* end subroutine (cmi_loadcmi) */

local int vtecmp(cvoid *v1p,cvoid *v2p) noex {
	uint		*vte1 = (uint *) v1p ;
	uint		*vte2 = (uint *) v2p ;
	int		rc = 0 ;
	{
	    int cn1 = ((vte1[3] >> 0) & USHORT_MAX) ;
	    int cn2 = ((vte2[3] >> 0) & USHORT_MAX) ;
	    rc = (cn1 - cn2) ;
	}
	return rc ;
} /* end subroutine (vtecmp) */


