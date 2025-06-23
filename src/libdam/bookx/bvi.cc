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
	<0		error code (syhstem-return)

*******************************************************************************/

#include	<envstandards.h>	/* must be before others */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/mman.h>
#include	<climits>		/* |UCHAR_MAX| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<char.h>
#include	<endian.h>
#include	<localmisc.h>

#include	"bvi.h"
#include	"bvihdr.h"
#include	"bvcitekey.h"


/* local defines */

#define	BVI_FMI		struct bvi_fmi
#define	BVI_KA		szof(BVI_LINE)
#define	BVI_BO(v)	((BVI_KA - ((v) % BVI_KA)) % BVI_KA)

#define	SHIFTINT	(6 * 60)	/* possible time-shift */

#define	MODP2(v,n)	((v) & ((n) - 1))

#define	TO_CHECK	4


/* external subroutines */


/* external variables */


/* local structures */

struct chapters {
	uchar		*ap ;
	int		al ;
	int		ci ;
} ;


/* forward references */

static int	bvi_loadbegin(BVI *,time_t) noex ;
static int	bvi_loadend(BVI *) noex ;
static int	bvi_mapcreate(BVI *,time_t) noex ;
static int	bvi_mapdestroy(BVI *) noex ;
static int	bvi_proc(BVI *,time_t) noex ;
static int	bvi_verify(BVI *,time_t) noex ;
static int	bvi_auditvt(BVI *) noex ;
static int	bvi_checkupdate(BVI *,time_t) noex ;
static int	bvi_search(BVI *,BVI_QUERY *) noex ;
static int	bvi_loadbve(BVI *,bvi_v *,char *,int,int) noex ;
static int	bvi_loadchapters(BVI *,int,uchar *,int) noex ;

static int	chapters_start(struct chapters *,uchar *,int) noex ;
static int	chapters_set(struct chapters *,int,int) noex ;
static int	chapters_finish(struct chapters *) noex ;

static int	mkcitekey(BVI_QUERY *,uint *) noex ;

#if	CF_SEARCH
static int	vtecmp(cvoid *,cvoid *) noex ;
#endif


/* local variables */


/* exported variables */

bvi_obj bvi_modinfo = {
	"bvi",
	szof(bvi),
	szof(vci_cur)
} ;


/* exported variables */


/* exported subroutines */

int bvi_open(BVI *op,cchar *dbname) noex {
	const time_t	dt = time(NULL) ;
	int		rs ;
	int		nverses = 0 ;
	cchar	*cp ;

	if (op == NULL) return SR_FAULT ;
	if (dbname == NULL) return SR_FAULT ;

	if (dbname[0] == '\0') return SR_INVALID ;

	memclear(op) ;
	if ((rs = uc_mallocstrw(dbname,-1,&cp)) >= 0) {
	    cchar	*es = ENDIANSTR ;
	    char	tmpfname[MAXPATHLEN + 1] ;
	    op->dbname = cp ;
	    if ((rs = mkfnamesuf2(tmpfname,op->dbname,BVI_SUF,es)) >= 0) {
	        cint	tl = rs ;
	        if ((rs = uc_mallocstrw(tmpfname,tl,&cp)) >= 0) {
	            op->fname = cp ;
	            if ((rs = bvi_loadbegin(op,dt)) >= 0) {
	                nverses = rs ;
	                op->ti_lastcheck = dt ;
	                op->magic = BVI_MAGIC ;
	            } /* end if (loadbegin) */
	            if (rs < 0) {
	                if (op->fname != NULL) {
	                    uc_free(op->fname) ;
	                    op->fname = NULL ;
	                }
	            }
	        } /* end if (memory-allocation) */
	    } /* end if (mkfnamesuf2) */
	    if (rs < 0) {
	        if (op->dbname != NULL) {
	            uc_free(op->dbname) ;
	            op->dbname = NULL ;
	        }
	    }
	} /* end if (memory-allocation) */

	return (rs >= 0) ? nverses : rs ;
}
/* end subroutine (bvi_open) */

int bvi_close(BVI *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;

	if (op == NULL) return SR_FAULT ;

	if (op->magic != BVI_MAGIC) return SR_NOTOPEN ;

	rs1 = bvi_loadend(op) ;
	if (rs >= 0) rs = rs1 ;

	if (op->fname != NULL) {
	    rs1 = uc_free(op->fname) ;
	    if (rs >= 0) rs = rs1 ;
	    op->fname = NULL ;
	}

	if (op->dbname != NULL) {
	    rs1 = uc_free(op->dbname) ;
	    if (rs >= 0) rs = rs1 ;
	    op->dbname = NULL ;
	}

	op->magic = 0 ;
	return rs ;
}
/* end subroutine (bvi_close) */

int bvi_audit(BVI *op) noex {
	int		rs = SR_OK ;

	if (op == NULL) return SR_FAULT ;

	if (op->magic != BVI_MAGIC) return SR_NOTOPEN ;

/* verify that all list pointers and list entries are valid */

	if (rs >= 0) {
	    rs = bvi_auditvt(op) ;
	}

	return rs ;
}
/* end subroutine (bvi_audit) */

int bvi_count(BVI *op) noex {
	bvihdr		*hip ;
	int		rs = SR_OK ;

	if (op == NULL) return SR_FAULT ;

	if (op->magic != BVI_MAGIC) return SR_NOTOPEN ;

	hip = &op->fhi ;
	return (rs >= 0) ? hip->nverses : rs ;
}
/* end subroutine (bvi_count) */

int bvi_info(BVI *op,BVI_INFO *ip) noex {
	bvihdr		*hip ;
	int		rs = SR_OK ;
	int		nv = 0 ;

	if (op == NULL) return SR_FAULT ;

	if (op->magic != BVI_MAGIC) return SR_NOTOPEN ;

	hip = &op->fhi ;
	nv = hip->nverses ;

	if (ip != NULL) {
	    memclear(ip) ;
	    ip->mtime = op->fmi.ti_mod ;
	    ip->ctime = (time_t) hip->wtime ;
	    ip->maxbook = hip->maxbook ;
	    ip->maxchapter = hip->maxchapter ;
	    ip->count = hip->nverses ;
	    ip->nzverses = hip->nzverses ;
	}

	return (rs >= 0) ? nv : rs ;
}
/* end subroutine (bvi_info) */

int bvi_read(BVI *op,bvi_v *bvep,char *vbuf,int vlen,bvi_q *qp) noex {
	int		rs = SR_OK ;
	int		vi = 0 ;

	if (op == NULL) return SR_FAULT ;
	if (qp == NULL) return SR_FAULT ;
	if (bvep == NULL) return SR_FAULT ;
	if (vbuf == NULL) return SR_FAULT ;

	if (op->magic != BVI_MAGIC) return SR_NOTOPEN ;

/* check for update */

	if ((rs >= 0) && (op->ncursors == 0)) {
	    rs = bvi_checkupdate(op,0) ;
	}
	if (rs >= 0) {
	    if ((rs = bvi_search(op,qp)) >= 0) {
	        vi = rs ;
	        rs = bvi_loadbve(op,bvep,vbuf,vlen,vi) ;
	    }
	} /* end if (ok) */

	return (rs >= 0) ? vi : rs ;
}
/* end subroutine (bvi_read) */

int bvi_get(BVI *op,bvi_q *qp,bvi_v *bvep,char *vbuf,int vlen) noex {
	return bvi_read(op,bvep,vbuf,vlen,qp) ;
}
/* end subroutine (bvi_get) */

int bvi_curbegin(BVI *op,BVI_CUR *curp) noex {
	if (op == NULL) return SR_FAULT ;
	if (curp == NULL) return SR_FAULT ;

	if (op->magic != BVI_MAGIC) return SR_NOTOPEN ;

	curp->i = 0 ;
	op->ncursors += 1 ;

	return SR_OK ;
}
/* end subroutine (bvi_curbegin) */

int bvi_curend(BVI *op,BVI_CUR *curp) noex {
	if (op == NULL) return SR_FAULT ;
	if (curp == NULL) return SR_FAULT ;

	if (op->magic != BVI_MAGIC) return SR_NOTOPEN ;

	curp->i = 0 ;
	if (op->ncursors > 0) {
	    op->ncursors -= 1 ;
	}

	return SR_OK ;
}
/* end subroutine (bvi_curend) */

int bvi_enum(BVI *op,BVI_CUR *curp,bvi_v *bvep,char *vbuf,int vlen) noex {
	bvihdr		*hip ;
	int		rs = SR_OK ;
	int		vi ;
	int		nlines = 0 ;

	if (op == NULL) return SR_FAULT ;
	if (curp == NULL) return SR_FAULT ;
	if (bvep == NULL) return SR_FAULT ;
	if (vbuf == NULL) return SR_FAULT ;

	if (op->magic != BVI_MAGIC) return SR_NOTOPEN ;

	if (op->ncursors == 0) return SR_INVALID ;

	vi = (curp->i < 0) ? 0 : (curp->i + 1) ;

	hip = &op->fhi ;
	if (vi < hip->vilen) {
	    if ((rs = bvi_loadbve(op,bvep,vbuf,vlen,vi)) >= 0) {
		nlines = rs ;
	        curp->i = vi ;
	    }
	} else {
	    rs = SR_NOTFOUND ;
	}

	return (rs >= 0) ? nlines : rs ;
}
/* end subroutine (bvi_enum) */

int bvi_chapters(BVI *op,int book,uchar *ap,int al) noex {
	BVI_QUERY	q ;
	int		rs ;
	int		n = 0 ;

	if (op == NULL) return SR_FAULT ;

	if (op->magic != BVI_MAGIC) return SR_NOTOPEN ;

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

static int bvi_loadbegin(BVI *op,time_t dt) noex {
	int		rs ;
	int		nverses = 0 ;

	if ((rs = bvi_mapcreate(op,dt)) >= 0) {
	    rs = bvi_proc(op,dt) ;
	    nverses = rs ;
	    if (rs < 0) {
	        bvi_mapdestroy(op) ;
	    }
	} /* end if */

	return (rs >= 0) ? nverses : rs ;
}
/* end subroutine (bvi_loadbegin) */

static int bvi_loadend(BVI *op) noex {
	BVI_FMI		*mip ;
	int		rs = SR_OK ;
	int		rs1 ;
	{
	rs1 = bvi_mapdestroy(op) ;
	if (rs >= 0) rs = rs1 ;
	}
	mip = &op->fmi ;
	mip->vt = NULL ;
	mip->lt = NULL ;
	return rs ;
}
/* end subroutine (bvi_loadend) */

static int bvi_mapcreate(BVI *op,time_t dt) noex {
	BVI_FMI		*mip = &op->fmi ;
	cnullptr	np{} ;
	int		rs ;
	int		rs1 ;

	if (op->fname == NULL) return SR_BUGCHECK ;

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
}
/* end subroutine (bvi_mapcreate) */

static int bvi_mapdestroy(BVI *op) noex {
	BVI_FMI		*mip = &op->fmi ;
	int		rs = SR_OK ;
	int		rs1 ;

	if (mip->mapdata != NULL) {
	    rs1 = u_munmap(mip->mapdata,mip->mapsize) ;
	    if (rs >= 0) rs = rs1 ;
	    mip->mapdata = NULL ;
	    mip->mapsize = 0 ;
	    mip->ti_map = 0 ;
	}

	return rs ;
}
/* end subroutine (bvi_mapdestroy) */

static int bvi_checkupdate(BVI *op,time_t dt) noex {
	int		rs = SR_OK ;
	int		f = FALSE ;

	if (op->ncursors == 0) {
	    if (dt <= 0) dt = time(NULL) ;
	    if ((dt - op->ti_lastcheck) >= TO_CHECK) {
	        USTAT		sb ;
	        BVI_FMI		*mip = &op->fmi ;
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
}
/* end subroutine (bvi_checkupdate) */

static int bvi_proc(BVI *op,time_t dt) noex {
	BVI_FMI		*mip = &op->fmi ;
	bvihdr		*hip = &op->fhi ;
	int		rs ;
	int		nverses = 0 ;

	if ((rs = bvihdr(hip,1,mip->mapdata,mip->mapsize)) >= 0) {
	    nverses = hip->nverses ;
	    if ((rs = bvi_verify(op,dt)) >= 0) {
	        mip->vt = (uint (*)[4]) (mip->mapdata + hip->vioff) ;
	        mip->lt = (uint (*)[2]) (mip->mapdata + hip->vloff) ;
	    }
	}

	return (rs >= 0) ? nverses : rs ;
}
/* end subroutine (bvi_proc) */

static int bvi_verify(BVI *op,time_t dt) noex {
	BVI_FMI		*mip = &op->fmi ;
	bvihdr		*hip = &op->fhi ;
	int		rs = SR_OK ;
	int		size ;
	int		f = TRUE ;

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
}
/* end subroutine (bvi_verify) */

static int bvi_auditvt(BVI *op) noex {
	BVI_FMI		*mip = &op->fmi ;
	bvihdr		*hip = &op->fhi ;
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
}
/* end subroutine (bvi_auditvt) */

static int bvi_search(BVI *op,BVI_QUERY *qp) noex {
	BVI_FMI		*mip ;
	bvihdr		*hip ;
	uint		(*vt)[4] ;
	uint		citekey ;
	uint		vte[4] ;
	int		rs = SR_OK ;
	int		vtlen ;
	int		vi = 0 ;

	mip = &op->fmi ;
	hip = &op->fhi ;

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

	    rs = (vtep != NULL) ? ((vtep - vt[0]) >> 2) : SR_NOTFOUND ;
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
}
/* end subroutine (bvi_search) */

static int bvi_loadbve(BVI *op,bvi_v *bvep,char *ebuf,int elen,int vi) noex {
	BVI_LINE	*lines ;
	BVI_FMI		*mip ;
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

	if (bvep == NULL) return SR_FAULT ;
	if (ebuf == NULL) return SR_FAULT ;

	if (elen <= 0) return SR_OVERFLOW ;

	mip = &op->fmi ;
	hip = &op->fhi ;

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
}
/* end subroutine (bvi_loadbve) */

static int bvi_loadchapters(BVI *op,int vi,uchar *ap,int al) noex {
	struct chapters	cm ;
	BVCITEKEY	ck ;
	BVI_FMI		*mip ;
	bvihdr		*hip ;
	uint		(*vt)[4] ;
	uint		b, c, v ;
	int		rs = SR_OK ;
	int		i ;
	int		vtlen ;
	int		n = 0 ;

	mip = &op->fmi ;
	hip = &op->fhi ;

	vt = mip->vt ;
	vtlen = hip->vilen ;

	if (vi < vtlen) {

	    bvcitekey_get(&ck,(vt[vi]+3)) ;
	    b = ck.b ;
	    c = ck.c ;

	    if (ap != NULL) rs = chapters_start(&cm,ap,al) ;

	    if (rs >= 0) {

	        v = 0 ;
	        for (i = vi ; (rs >= 0) && (i < vtlen) ; i += 1) {
	            bvcitekey_get(&ck,(vt[i]+3)) ;
	            if (b != ck.b)
	                break ;
	            if (c != ck.c) {
	                if (ap != NULL) rs = chapters_set(&cm,c,v) ;
	                c = ck.c ;
	            }
	            v = ck.v ;
	        } /* end for */

	        if (rs >= 0) {
	            if (ap != NULL) rs = chapters_set(&cm,c,v) ;
	            c += 1 ;
	        }

	        n = c ;
	        if (ap != NULL) chapters_finish(&cm) ;

	    } /* end if (ok) */

	} /* end if */

	return (rs >= 0) ? n : rs ;
}
/* end subroutine (bvi_loadchapters) */

static int chapters_start(struct chapters *cp,uchar *ap,int al) noex {
	cp->ap = ap ;
	cp->al = al ;
	cp->ci = 0 ;
	return SR_OK ;
}
/* end subroutine (chapters_start) */

static int chapters_set(struct chapters *cp,int ci,int nv) noex {
	if (nv > UCHAR_MAX) return SR_RANGE ;
	if (ci >= cp->al) return SR_OVERFLOW ;

	while (cp->ci < ci) {
	    cp->ap[cp->ci++] = 0 ;
	}

	if (cp->ci == ci) {
	    cp->ap[cp->ci++] = nv ;
	}

	return cp->ci ;
}
/* end subroutine (chapters_set) */

static int chapters_finish(struct chapters *cp) noex {
	int		ci = cp->ci ;
	return ci ;
}
/* end subroutine (chapters_finish) */

static int mkcitekey(BVI_QUERY *bvp,uint *cip) noex {
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
	c1 = vte1[3] & 0x00FFFFFF ;
	c2 = vte2[3] & 0x00FFFFFF ;
	return (c1 - c2) ;
}
/* end subroutine (vtecmp) */
#endif /* CF_SEARCH */


