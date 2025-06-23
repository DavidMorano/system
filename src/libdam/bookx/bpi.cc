/* bpi SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* read or audit a BPI (Bible Paragraph Index) database */
/* version %I% last-modified %G% */

#define	CF_SEARCH	1		/* use 'bsearch(3c)' */

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

#include	<envstandards.h>	/* must be before others */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<sys/mman.h>
#include	<unistd.h>
#include	<climits>		/* |UINT_MAX| */
#include	<ctime>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<storebuf.h>
#include	<char.h>
#include	<endian.h>
#include	<localmisc.h>

#include	"bpi.h"
#include	"bpihdr.h"
#include	"bvcitekey.h"


/* local defines */

#define	BPI_MAGIC	0x88773422
#define	BPI_KA		szof(BPI_LINE)
#define	BPI_BO(v)	((BPI_KA - ((v) % BPI_KA)) % BPI_KA)

#define	SHIFTINT	(6 * 60)	/* possible time-shift */

#ifndef	MODP2
#define	MODP2(v,n)	((v) & ((n) - 1))
#endif

#define	TO_CHECK	4


/* external subroutines */


/* external variables */


/* exported variables */

bpi_obj bpi_modinfo = {
	"bpi",
	szof(bpi),
	szof(bpi_cir)
} ;


/* local structures */


/* forward references */

static int	bpi_loadbegin(bpi *,time_t) noex ;
static int	bpi_loadend(bpi *) noex ;
static int	bpi_mapcreate(bpi *,time_t) noex ;
static int	bpi_mapdestroy(bpi *) noex ;
static int	bpi_proc(bpi *,time_t) noex ;
static int	bpi_verify(bpi *,time_t) noex ;
static int	bpi_auditvt(bpi *) noex ;
static int	bpi_checkupdate(bpi *,time_t) noex ;
static int	bpi_search(bpi *,bpi_q *) noex ;
static int	bpi_loadbve(bpi *,bpi_v *,int) noex ;

static int	mkcitekey(bpi_q *,uint *) noex ;

#if	CF_SEARCH
static int	vtecmp(cvoid *,cvoid *) noex ;
#endif


/* local variables */


/* exported variables */


/* exported subroutines */

int bpi_open(bpi *op,cchar *dbname) noex {
	ctime_t		dt = time(nullptr) ;
	int		rs = SR_OK ;
	int		tl ;
	int		nverses = 0 ;
	cchar		*cp ;

	if (op == nullptr) return SR_FAULT ;
	if (dbname == nullptr) return SR_FAULT ;

	if (dbname[0] == '\0') return SR_INVALID ;

	memclear(op) ;

	if ((rs = uc_mallocstrw(dbname,-1,&cp)) >= 0) {
	    cchar	*suf = BPI_SUF ;
	    cchar	*end = ENDIANSTR ;
	    char	tbuf[MAXPATHLEN + 1] ;
	    op->dbname = cp ;
	    if ((rs = mkfnamesuf2(tbuf,op->dbname,suf,end)) >= 0) {
	        tl = rs ;
	        if ((rs = uc_mallocstrw(tbuf,tl,&cp)) >= 0) {
	            op->fname = cp ;
	            if ((rs = bpi_loadbegin(op,dt)) >= 0) {
	                op->ti_lastcheck = dt ;
	                op->magic = BPI_MAGIC ;
	            }
	            if (rs < 0) {
	                uc_free(op->fname) ;
	                op->fname = nullptr ;
	            }
	        } /* end if (m-a) */
	    } /* end if (mkfnamesuf) */
	    if (rs < 0) {
	        uc_free(op->dbname) ;
	        op->dbname = nullptr ;
	    }
	} /* end if (m-a) */

	return (rs >= 0) ? nverses : rs ;
}
/* end subroutine (bpi_open) */

int bpi_close(bpi *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;

	if (op == nullptr) return SR_FAULT ;

	if (op->magic != BPI_MAGIC) return SR_NOTOPEN ;

	rs1 = bpi_loadend(op) ;
	if (rs >= 0) rs = rs1 ;

	if (op->fname != nullptr) {
	    rs1 = uc_free(op->fname) ;
	    if (rs >= 0) rs = rs1 ;
	    op->fname = nullptr ;
	}

	if (op->dbname != nullptr) {
	    rs1 = uc_free(op->dbname) ;
	    if (rs >= 0) rs = rs1 ;
	    op->dbname = nullptr ;
	}

	op->magic = 0 ;
	return rs ;
}
/* end subroutine (bpi_close) */

int bpi_audit(bpi *op) noex {
	int		rs ;

	if (op == nullptr) return SR_FAULT ;

	if (op->magic != BPI_MAGIC) return SR_NOTOPEN ;

/* verify that all list pointers and list entries are valid */

	rs = bpi_auditvt(op) ;

	return rs ;
}
/* end subroutine (bpi_audit) */

int bpi_count(bpi *op) noex {
	bpihdr		*hip ;
	int		rs = SR_OK ;

	if (op == nullptr) return SR_FAULT ;

	if (op->magic != BPI_MAGIC) return SR_NOTOPEN ;

	hip = &op->fhi ;
	return (rs >= 0) ? hip->nverses : rs ;
}
/* end subroutine (bpi_count) */

int bpi_info(bpi *op,bpi_i *ip) noex {
	bpihdr		*hip ;
	int		rs = SR_OK ;

	if (op == nullptr) return SR_FAULT ;

	if (op->magic != BPI_MAGIC) return SR_NOTOPEN ;

	hip = &op->fhi ;

	if (ip != nullptr) {
	    rs = memclear(ip) ;
	    ip->mtime = op->fmi.ti_mod ;
	    ip->ctime = time_t(hip->wtime) ;
	    ip->maxbook = hip->maxbook ;
	    ip->maxchapter = hip->maxchapter ;
	    ip->count = hip->nverses ;
	    ip->nzverses = hip->nzverses ;
	}

	return (rs >= 0) ? hip->nverses : rs ;
}
/* end subroutine (bpi_info) */

int bpi_get(bpi *op,bpi_q *qp) noex {
	int		rs = SR_OK ;
	int		vi = 0 ;

	if (op == nullptr) return SR_FAULT ;
	if (qp == nullptr) return SR_FAULT ;

	if (op->magic != BPI_MAGIC) return SR_NOTOPEN ;

/* check for update */

	if (op->ncursors == 0) {
	    rs = bpi_checkupdate(op,0) ;
	}

	if (rs >= 0) {
	    rs = bpi_search(op,qp) ;
	    vi = rs ;
	}

	return (rs >= 0) ? vi : rs ;
}
/* end subroutine (bpi_get) */

int bpi_curbegin(bpi *op,bpi_cur *curp) noex {
	if (op == nullptr) return SR_FAULT ;
	if (curp == nullptr) return SR_FAULT ;

	if (op->magic != BPI_MAGIC) return SR_NOTOPEN ;

	curp->i = 0 ;
	op->ncursors += 1 ;

	return SR_OK ;
}
/* end subroutine (bpi_curbegin) */

int bpi_curend(bpi *op,bpi_cur *curp) noex {
	if (op == nullptr) return SR_FAULT ;
	if (curp == nullptr) return SR_FAULT ;
	if (op->magic != BPI_MAGIC) return SR_NOTOPEN ;

	curp->i = 0 ;
	if (op->ncursors > 0)
	    op->ncursors -= 1 ;

	return SR_OK ;
}
/* end subroutine (bpi_curend) */

int bpi_enum(bpi *op,bpi_cur *curp,bpi_v *bvep) noex {
	bpihdr		*hip ;
	int		rs = SR_OK ;
	int		vi ;
	int		vtlen ;

	if (op == nullptr) return SR_FAULT ;
	if (curp == nullptr) return SR_FAULT ;
	if (bvep == nullptr) return SR_FAULT ;

	if (op->magic != BPI_MAGIC) return SR_NOTOPEN ;

	if (op->ncursors == 0) return SR_INVALID ;

	vi = (curp->i < 0) ? 0 : (curp->i + 1) ;

	hip = &op->fhi ;
	vtlen = hip->vilen ;
	if (vi >= vtlen) rs = SR_NOTFOUND ;

	if (rs >= 0) {
	    if ((rs = bpi_loadbve(op,bvep,vi)) >= 0) {
	    	curp->i = vi ;
	    }
	}

	return rs ;
}
/* end subroutine (bpi_enum) */


/* private subroutines */

static int bpi_loadbegin(bpi *op,time_t daytime) noex {
	int		rs ;
	int		nverses = 0 ;

	if ((rs = bpi_mapcreate(op,daytime)) >= 0) {
	    rs = bpi_proc(op,daytime) ;
	    nverses = rs ;
	    if (rs < 0) {
		bpi_mapdestroy(op) ;
	    }
	} /* end if (map) */

	return (rs >= 0) ? nverses : rs ;
}
/* end subroutine (bpi_loadbegin) */

static int bpi_loadend(bpi *op) noex {
	bpi_fmi		*mip ;
	int		rs = SR_OK ;
	int		rs1 ;
	{
	    rs1 = bpi_mapdestroy(op) ;
	    if (rs >= 0) rs = rs1 ;
	}
	mip = &op->fmi ;
	mip->vt = nullptr ;
	return rs ;
}
/* end subroutine (bpi_loadend) */

static int bpi_mapcreate(bpi *op,time_t daytime) noex {
	bpi_fmi		*mip = &op->fmi ;
	int		rs ;

	if (op->fname == nullptr) return SR_FAULT ;

	if ((rs = u_open(op->fname,O_RDONLY,0666)) >= 0) {
	    USTAT	sb ;
	    cint	fd = rs ;
	    if ((rs = u_fstat(fd,&sb)) >= 0) {
	        csize	fsize = (sb.st_size & UINT_MAX) ;
	        if (fsize > 0) {
	            size_t	ms = (size_t) fsize ;
	            int		mp = PROT_READ ;
	            int		mf = MAP_SHARED ;
	            void	*md ;
	            if ((rs = u_mmap(nullptr,ms,mp,mf,fd,0L,&md)) >= 0) {
	                mip->mapdata = md ;
	                mip->mapsize = ms ;
	                mip->ti_mod = sb.st_mtime ;
	                mip->ti_map = daytime ;
	            } /* end if (u_mmap) */
	        } else {
		    rs = SR_UNATCH ;
		}
	    } /* end if (stat) */
	    u_close(fd) ;
	} /* end if (file-open) */

	return rs ;
}
/* end subroutine (bpi_mapcreate) */

static int bpi_mapdestroy(bpi *op) noex {
	bpi_fmi		*mip = &op->fmi ;
	int		rs = SR_OK ;

	if (mip->mapdata != nullptr) {
	    rs = u_munmap(mip->mapdata,mip->mapsize) ;
	    mip->mapdata = nullptr ;
	    mip->mapsize = 0 ;
	    mip->ti_map = 0 ;
	}

	return rs ;
}
/* end subroutine (bpi_mapdestroy) */

static int bpi_checkupdate(bpi *op,time_t dt) noex {
	int		rs = SR_OK ;
	int		f = false ;

	if (op->ncursors == 0) {
	    if (dt <= 0) dt = time(nullptr) ;
	    if ((dt - op->ti_lastcheck) >= TO_CHECK) {
	        USTAT		sb ;
	        bpi_fmi		*mip = &op->fmi ;
	        op->ti_lastcheck = dt ;
	        if ((rs = u_stat(op->fname,&sb)) >= 0) {
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
	}

	return (rs >= 0) ? f : rs ;
}
/* end subroutine (bpi_checkupdate) */

static int bpi_proc(bpi *op,time_t daytime) noex {
	bpi_fmi		*mip = &op->fmi ;
	bpihdr		*hip = &op->fhi ;
	int		rs ;
	int		nverses = 0 ;

	rs = bpihdr(hip,1,mip->mapdata,mip->mapsize) ;

	if (rs >= 0) {
	    rs = bpi_verify(op,daytime) ;
	    nverses = hip->nverses ;
	}

	if (rs >= 0) {
	    mip->vt = (uint (*)[1]) (mip->mapdata + hip->vioff) ;
	}

	return (rs >= 0) ? nverses : rs ;
}
/* end subroutine (bpi_proc) */

static int bpi_verify(bpi *op,time_t daytime) noex {
	bpi_fmi		*mip = &op->fmi ;
	bpihdr		*hip = &op->fhi ;
	uint		utime = (uint) daytime ;
	int		rs = SR_OK ;
	int		size ;
	int		f = true ;

	f = f && (hip->fsize == mip->mapsize) ;
	f = f && (hip->wtime > 0) && (hip->wtime <= (utime + SHIFTINT)) ;

/* alignment restriction */

	f = f && ((hip->vioff & (szof(int)-1)) == 0) ;

/* size restrictions */

	f = f && (hip->vioff <= mip->mapsize) ;
	size = (hip->vilen * 1) * szof(uint) ;
	f = f && ((hip->vioff + size) <= mip->mapsize) ;

/* something restriction? */

	f = f && (hip->vilen == hip->nverses) ;

/* get out */

	if (! f)
	    rs = SR_BADFMT ;

	return rs ;
}
/* end subroutine (bpi_verify) */

static int bpi_auditvt(bpi *op) noex {
	bpi_fmi		*mip = &op->fmi ;
	bpihdr		*hip = &op->fhi ;
	uint		(*vt)[1] ;
	uint		pcitcmpval = 0 ;
	uint		citcmpval ;
	int		rs = SR_OK ;
	vt = mip->vt ;
/* "verses" table */
	for (int i = 1 ; (rs >= 0) && (i < hip->vilen) ; i += 1) {
/* verify all entries are ordered w/ increasing citations */
	    citcmpval = vt[i][0] & 0x00FFFFFF ;
	    if (citcmpval < pcitcmpval) {
	        rs = SR_BADFMT ;
	        break ;
	    }
	    pcitcmpval = citcmpval ;
	} /* end for (record table entries) */
	return rs ;
}
/* end subroutine (bpi_auditvt) */

static int bpi_search(bpi *op,bpi_q *qp) noex {
	bpi_fmi		*mip = &op->fmi ;
	bpihdr		*hip = &op->fhi ;
	uint		(*vt)[1] ;
	uint		citekey ;
	uint		vte[1] ;
	int		rs = SR_OK ;
	int		vtlen ;
	int		vi = 0 ;

	vt = mip->vt ;
	vtlen = hip->vilen ;

/* search for entry */

	mkcitekey(qp,&citekey) ;

	vte[0] = citekey ;

#if	CF_SEARCH
	{
	    uint	*vtep ;
	    int		vtesize = (1 * szof(uint)) ;
	    vtep = (uint *) bsearch(vte,vt,vtlen,vtesize,vtecmp) ;
	    rs = (vtep != nullptr) ? ((vtep - vt[0]) >> 2) : SR_NOTFOUND ;
	    vi = rs ;
	}
#else /* CF_SEARCH */
	{
	    for (vi = 0 ; vi < vtlen ; vi += 1) {
	        if ((vt[vi][0] & 0x00FFFFFF) == citekey)
		    break ;
	    }
	    rs = (vi < vtlen) ? vi : SR_NOTFOUND ;
	}
#endif /* CF_SEARCH */

	return (rs >= 0) ? vi : rs ;
}
/* end subroutine (bpi_search) */

static int bpi_loadbve(bpi *op,bpi_v *bvep,int vi) noex {
	bpi_fmi		*mip = &op->fmi ;
	bpihdr		*hip = &op->fhi ;
	uint		*vte ;
	int		rs = SR_OK ;
	int		vtlen ;

	if (bvep == nullptr) return SR_FAULT ;

	memclear(bvep) ;

	vtlen = hip->vilen ;
	if (vi >= vtlen) rs = SR_NOANODE ;

/* load the basic stuff */

	if (rs >= 0) {
	    vte = mip->vt[vi] ;
	    bvep->nlines = (vte[0] >> 24) & 0xFF ;
	    bvep->b = (vte[0] >> 16) & 0xFF ;
	    bvep->c = (vte[0] >> 8) & 0xFF ;
	    bvep->v = (vte[0] >> 0) & 0xFF ;
	} /* end if */

	return rs ;
}
/* end subroutine (bpi_loadbve) */

static int mkcitekey(bpi_q *bvp,uint *cip) noex {
	uint		ci = 0 ;
	ci |= (bvp->b & UCHAR_MAX) ;
	ci = (ci << 8) ;
	ci |= (bvp->c & UCHAR_MAX) ;
	ci = (ci << 8) ;
	ci |= (bvp->v & UCHAR_MAX) ;
	*cip = ci ;
	return SR_OK ;
}
/* end subroutine (mkcitekey) */

#if	CF_SEARCH
static int vtecmp(cvoid *v1p,cvoid *v2p) noex {
	uint		*vte1 = (uint *) v1p ;
	uint		*vte2 = (uint *) v2p ;
	uint		c1, c2 ;
	c1 = vte1[0] & 0x00FFFFFF ;
	c2 = vte2[0] & 0x00FFFFFF ;
	return (c1 - c2) ;
}
/* end subroutine (vtecmp) */
#endif /* CF_SEARCH */


