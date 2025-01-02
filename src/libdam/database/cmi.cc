/* cmi SUPPORT */
/* encoding=ISO8859-1 */
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
#include	<sys/param.h>
#include	<sys/mman.h>
#include	<climits>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<char.h>
#include	<endian.h>
#include	<localmisc.h>

#include	"cmi.h"
#include	"cmihdr.h"
#include	"bvcitekey.h"


/* local defines */

#define	CMI_KA		szof(cmi_line)
#define	CMI_BO(v)	((CMI_KA - ((v) % CMI_KA)) % CMI_KA)

#define	SHIFTINT	(6 * 60)	/* possible time-shift */

#define	MODP2(v,n)	((v) & ((n) - 1))

#define	TO_CHECK	4

#ifndef	CF_SEARCH
#define	CF_SEARCH	1		/* use |bsearch(3c)| */
#endif


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

template<typename ... Args>
static int cmi_ctor(cmi *op,Args ... args) noex {
	CMI		*hup = op ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = memclear(hop) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (cmi_ctor) */

static int cmi_dtor(cmi *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (cmi_dtor) */

template<typename ... Args>
static inline int cmi_magic(cmi *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magic == CMI_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}
/* end subroutine (cmi_magic) */

static int	cmi_loadbegin(cmi *,time_t) noex ;
static int	cmi_loadend(cmi *) noex ;
static int	cmi_mapcreate(cmi *,time_t) noex ;
static int	cmi_mapdestroy(cmi *) noex ;
static int	cmi_proc(cmi *,time_t) noex ;
static int	cmi_verify(cmi *,time_t) noex ;
static int	cmi_auditvt(cmi *) noex ;
static int	cmi_checkupdate(cmi *,time_t) noex ;
static int	cmi_search(cmi *,uint) noex ;
static int	cmi_loadcmd(cmi *,cmi_ent *,char *,int,int) noex ;

static int	vtecmp(cvoid *,cvoid *) noex ;


/* local variables */

constexpr bool		f_search = CF_SEARCH ;


/* exported variables */

extern const cmi_obj	cmi_modinfo = {
	"cmi",
	szof(cmi),
	szof(cmi_cur)
} ;


/* exported subroutines */

int cmi_open(cmi *op,cchar *dbname) noex {
	custime		dt = getustime ;
	int		rs ;
	int		nents = 0 ;
	if ((rs = ci_ctor(op,dbname)) >= 0) {
	    rs = SR_INVALID ;
	    if (dbname[0]) {
	cchar	*cp ;
	if ((rs = uc_mallocstrw(dbname,-1,&cp)) >= 0) {
	    cchar	*es = ENDIANSTR ;
	    char	tmpfname[MAXPATHLEN + 1] ;
	    op->dbname = cp ;
	    if ((rs = mkfnamesuf2(tmpfname,op->dbname,CMI_SUF,es)) >= 0) {
	        cint	tl = rs ;
	        if ((rs = uc_mallocstrw(tmpfname,tl,&cp)) >= 0) {
	            op->fname = cp ;
	            if ((rs = cmi_loadbegin(op,dt)) >= 0) {
	                nents = rs ;
	                op->ti_lastcheck = dt ;
	                op->magic = CMI_MAGIC ;
	            } /* end if (loadbegin) */
	            if (rs < 0) {
	                if (op->fname != nullptr) {
	                    uc_free(op->fname) ;
	                    op->fname = nullptr ;
	                }
	            }
	        } /* end if (memory-allocation) */
	    } /* end if (mkfnamesuf2) */
	    if (rs < 0) {
	        if (op->dbname != nullptr) {
	            uc_free(op->dbname) ;
	            op->dbname = nullptr ;
	        }
	    }
	} /* end if (memory-allocation) */

	    } /* end if (valid) */
	    if (rs < 0) {
		cmi_dtor(op) ;
	    }
	} /* end if (cmi_ctor) */
	return (rs >= 0) ? nents : rs ;
}
/* end subroutine (cmi_open) */

int cmi_close(cmi *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = cmi_magic(op)) >= 0) {
	    {
	        rs1 = cmi_loadend(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    if (op->fname) {
	        rs1 = uc_free(op->fname) ;
	        if (rs >= 0) rs = rs1 ;
	        op->fname = nullptr ;
	    }
	    if (op->dbname) {
	        rs1 = uc_free(op->dbname) ;
	        if (rs >= 0) rs = rs1 ;
	        op->dbname = nullptr ;
	    }
	    {
		rs1 = cmi_dtor(op) ;
		if (rs >= 0) rs = rs1 ;
	    }
	    op->magic = 0 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (cmi_close) */

int cmi_audit(cmi *op) noex {
	int		rs ;
	if ((rs = cmi_magic(op)) >= 0) {
	    rs = cmi_auditvt(op) ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (cmi_audit) */

int cmi_count(cmi *op) noex {
	int		rs ;
	int		ne = 0 ;
	if ((rs = cmi_magic(op)) >= 0) {
	    CMIHDR	*hip = &op->fhi ;
	    ne = hip->nents ;
	} /* end if (magic) */
	return (rs >= 0) ? ne : rs ;
}
/* end subroutine (cmi_count) */

int cmi_getinfo(cmi *op,cmi_info *ip) noex {
	int		rs ;
	int		ne = 0 ;
	if ((rs = cmi_magic(op)) >= 0) {
	    if (ip) {
	        CMIHDR	*hip = &op->fhi ;
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
}
/* end subroutine (cmi_getinfo) */

int cmi_read(cmi *op,cmi_ent *bvep,char *vbuf,int vlen,uint cn) noex {
	int		rs ;
	int		vi = 0 ;
	if ((rs = cmi_magic(op,bvep,vnif)) >= 0) {
	    if ((rs >= 0) && (op->ncursors == 0)) {
	        rs = cmi_checkupdate(op,0) ;
	    }
	    if (rs >= 0) {
	        if ((rs = cmi_search(op,cn)) >= 0) {
	            vi = rs ;
	            rs = cmi_loadcmd(op,bvep,vbuf,vlen,vi) ;
	        }
	    } /* end if (ok) */
	} /* end if (magic) */
	return (rs >= 0) ? vi : rs ;
}
/* end subroutine (cmi_read) */

int cmi_curbegin(cmi *op,cmi_cur *curp) noex {
	int		rs ;
	if ((rs = cmi_magic(op,curp)) >= 0) {
	    curp->i = 0 ;
	    op->ncursors += 1 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (cmi_curbegin) */

int cmi_curend(cmi *op,cmi_cur *curp) noex {
	int		rs ;
	if ((rs = cmi_magic(op,curp)) >= 0) {
	    curp->i = 0 ;
	    if (op->ncursors > 0) {
	        op->ncursors -= 1 ;
	    }
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (cmi_curend) */

int cmi_enum(cmi *op,cmi_cur *curp,cmi_ent *bvep,char *vbuf,int vlen) noex {
	CMIHDR		*hip ;
	int		rs = SR_OK ;
	int		vi ;
	int		nlines = 0 ;

	if (op == nullptr) return SR_FAULT ;
	if (curp == nullptr) return SR_FAULT ;
	if (bvep == nullptr) return SR_FAULT ;
	if (vbuf == nullptr) return SR_FAULT ;

	if (op->magic != CMI_MAGIC) return SR_NOTOPEN ;

	if (op->ncursors == 0) return SR_INVALID ;

	vi = (curp->i < 0) ? 0 : (curp->i + 1) ;
	hip = &op->fhi ;

	if (vi < hip->vilen) {
	    if ((rs = cmi_loadcmd(op,bvep,vbuf,vlen,vi)) >= 0) {
		nlines = rs ;
	        curp->i = vi ;
	    }
	} else {
	    rs = SR_NOTFOUND ;
	}

	return (rs >= 0) ? nlines : rs ;
}
/* end subroutine (cmi_enum) */


/* private subroutines */

static int cmi_loadbegin(cmi *op,time_t dt) noex {
	int		rs ;
	int		nents = 0 ;
	if ((rs = cmi_mapcreate(op,dt)) >= 0) {
	    rs = cmi_proc(op,dt) ;
	    nents = rs ;
	    if (rs < 0) {
	        cmi_mapdestroy(op) ;
	    }
	} /* end if */
	return (rs >= 0) ? nents : rs ;
}
/* end subroutine (cmi_loadbegin) */

static int cmi_loadend(cmi *op) noex {
	cmi_fmi		*mip ;
	int		rs = SR_OK ;
	int		rs1 ;
	{
	rs1 = cmi_mapdestroy(op) ;
	if (rs >= 0) rs = rs1 ;
	}
	mip = &op->fmi ;
	mip->vt = nullptr ;
	mip->lt = nullptr ;
	return rs ;
}
/* end subroutine (cmi_loadend) */

static int cmi_mapcreate(cmi *op,time_t dt) noex {
	cmi_fmi		*mip = &op->fmi ;
	int		rs ;

	if (op->fname == nullptr) return SR_BUGCHECK ;

	if ((rs = u_open(op->fname,O_RDONLY,0666)) >= 0) {
	    struct ustat	sb ;
	    int			fd = rs ;
	    if ((rs = u_fstat(fd,&sb)) >= 0) {
	        size_t	fsize = (sb.st_size & UINT_MAX) ;
	        if (fsize > 0) {
	            size_t	ms = fsize ;
	            int		mp = PROT_READ ;
	            int		mf = MAP_SHARED ;
	            void	*md ;
	            if ((rs = u_mmap(nullptr,ms,mp,mf,fd,0L,&md)) >= 0) {
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
	    u_close(fd) ;
	} /* end if (open) */

	return rs ;
}
/* end subroutine (cmi_mapcreate) */

static int cmi_mapdestroy(cmi *op) noex {
	cmi_fmi		*mip = &op->fmi ;
	int		rs = SR_OK ;
	int		rs1 ;

	if (mip->mapdata != nullptr) {
	    rs1 = u_munmap(mip->mapdata,mip->mapsize) ;
	    if (rs >= 0) rs = rs1 ;
	    mip->mapdata = nullptr ;
	    mip->mapsize = 0 ;
	    mip->ti_map = 0 ;
	}

	return rs ;
}
/* end subroutine (cmi_mapdestroy) */

static int cmi_checkupdate(cmi *op,time_t dt) noex {
	int		rs = SR_OK ;
	int		f = FALSE ;

	if (op->ncursors == 0) {
	    if (dt <= 0) dt = time(nullptr) ;
	    if ((dt - op->ti_lastcheck) >= TO_CHECK) {
	        struct ustat	sb ;
	        cmi_fmi		*mip = &op->fmi ;
	        op->ti_lastcheck = dt ;
	        if ((rs = u_stat(op->fname,&sb)) >= 0) {
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
}
/* end subroutine (cmi_checkupdate) */

static int cmi_proc(cmi *op,time_t dt) noex {
	cmi_fmi		*mip = &op->fmi ;
	CMIHDR		*hip = &op->fhi ;
	int		rs ;
	int		nents = 0 ;

	if ((rs = cmihdr(hip,1,mip->mapdata,mip->mapsize)) >= 0) {
	    if ((rs = cmi_verify(op,dt)) >= 0) {
	        mip->vt = (uint (*)[4]) (mip->mapdata + hip->vioff) ;
	        mip->lt = (uint (*)[2]) (mip->mapdata + hip->vloff) ;
	    }
	}

	return (rs >= 0) ? nents : rs ;
}
/* end subroutine (cmi_proc) */

static int cmi_verify(cmi *op,time_t dt) noex {
	cmi_fmi		*mip = &op->fmi ;
	CMIHDR		*hip = &op->fhi ;
	int		rs = SR_OK ;
	int		size ;
	int		f = TRUE ;

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

	f = f && ((hip->vioff & (szof(int)-1)) == 0) ;

/* size restrictions */

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
	f = f && (hip->vilen == hip->nents) ;

/* get out */

	if (! f)
	    rs = SR_BADFMT ;

	return rs ;
}
/* end subroutine (cmi_verify) */

static int cmi_auditvt(cmi *op) noex {
	cmi_fmi		*mip = &op->fmi ;
	CMIHDR		*hip = &op->fhi ;
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

}
/* end subroutine (cmi_auditvt) */

static int cmi_search(cmi *op,uint cn) noex {
	cmi_fmi		*mip = &op->fmi ;
	CMIHDR		*hip = &op->fhi ;
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
	    int		vtesize = (4 * szof(uint)) ;
	    vtep = (uint *) bsearch(vte,vt,vtlen,vtesize,vtecmp) ;
	    rs = (vtep != nullptr) ? ((vtep - vt[0]) >> 2) : SR_NOTFOUND ;
	    vi = rs ;
	} else {
	    for (vi = 0 ; vi < vtlen ; vi += 1) {
	        const ushort	vkey = ((vt[vi][3] >> 16) & USHORT_MAX) ;
	        if (vkey == citekey) break ;
	    }
	    rs = (vi < vtlen) ? vi : SR_NOTFOUND ;
	} /* end if_constexpr (f_search) */
	return (rs >= 0) ? vi : rs ;
}
/* end subroutine (cmi_search) */

static int cmi_loadcmd(cmi *op,cmi_ent *bvep,char *ebuf,int elen,int vi) noex {
	cmi_line	*lines ;
	cmi_fmi		*mip ;
	CMIHDR		*hip ;
	ulong		uebuf = (ulong) ebuf ;
	uint		*vte ;
	uint		(*lt)[2] ;
	uint		li ;
	int		rs = SR_OK ;
	int		bo, i ;
	int		linesize ;
	int		nlines ;

	if (bvep == nullptr) return SR_FAULT ;
	if (ebuf == nullptr) return SR_FAULT ;

	if (elen <= 0) return SR_OVERFLOW ;

	mip = &op->fmi ;
	hip = &op->fhi ;

	vte = mip->vt[vi] ;

/* load the basic stuff */

	memclear(bvep) ;
	bvep->eoff = vte[0] ;
	bvep->elen = vte[1] ;
	bvep->nlines = ((vte[3] >> 16) & USHORT_MAX) ;
	bvep->cn = ((vte[3] >> 0) & USHORT_MAX) ;

/* load the lines */

	li = vte[2] ;
	nlines = bvep->nlines ;

	if (li < hip->vllen) {

	    bo = CMI_BO(uebuf) ;

	    linesize = ((nlines + 1) * szof(cmi_line)) ;
	    if (linesize <= (elen - (bo-uebuf))) {

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
	        }

	    } else {
	        rs = SR_OVERFLOW ;
	    }
	} else {
	    rs = SR_BADFMT ;
	}

	return (rs >= 0) ? nlines : rs ;
}
/* end subroutine (cmi_loadcmd) */

#if	CF_SEARCH
static int vtecmp(cvoid *v1p,cvoid *v2p) noex {
	uint		*vte1 = (uint *) v1p ;
	uint		*vte2 = (uint *) v2p ;
	uint		cn1, cn2 ;
	cn1 = ((vte1[3] >> 0) & USHORT_MAX) ;
	cn2 = ((vte2[3] >> 0) & USHORT_MAX) ;
	return (cn1 - cn2) ;
}
/* end subroutine (vtecmp) */
#endif /* CF_SEARCH */


