/* cmi SUPPORT */
/* lang=C++20 */

/* read or audit a ComMandment Index (CMI) database */
/* version %I% last-modified %G% */

#define	CF_SEARCH	1		/* use 'bsearch(3c)' */

/* revision history:

	= 1998-03-01, David A­D­ Morano
	This subroutine was originally written.

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

#define	CMI_KA		sizeof(CMI_LINE)
#define	CMI_BO(v)	((CMI_KA - ((v) % CMI_KA)) % CMI_KA)

#define	SHIFTINT	(6 * 60)	/* possible time-shift */

#define	MODP2(v,n)	((v) & ((n) - 1))

#define	TO_CHECK	4


/* external subroutines */

extern int	sncpy1(char *,int,cchar *) ;
extern int	sncpy2(char *,int,cchar *,cchar *) ;
extern int	sncpy3(char *,int,cchar *,cchar *,cchar *) ;
extern int	snwcpy(char *,int,cchar *,int) ;
extern int	mkpath2(char *,cchar *,cchar *) ;
extern int	mkfnamesuf1(char *,cchar *,cchar *) ;
extern int	mkfnamesuf2(char *,cchar *,cchar *,cchar *) ;
extern int	nleadstr(cchar *,cchar *,int) ;
extern int	cfdeci(cchar *,int,int *) ;
extern int	cfdecui(cchar *,int,uint *) ;
extern int	isNotPresent(int) ;

extern char	*strwcpy(char *,cchar *,int) ;
extern char	*strwcpylc(char *,cchar *,int) ;
extern char	*strnchr(cchar *,int,int) ;
extern char	*strnpbrk(cchar *,int,cchar *) ;


/* external variables */


/* exported variables */

CMI_OBJ	cmi_modinfo = {
	"cmi",
	sizeof(CMI),
	sizeof(CMI_CUR)
} ;


/* local structures */


/* forward references */

static int	cmi_loadbegin(CMI *,time_t) noex ;
static int	cmi_loadend(CMI *) noex ;
static int	cmi_mapcreate(CMI *,time_t) noex ;
static int	cmi_mapdestroy(CMI *) noex ;
static int	cmi_proc(CMI *,time_t) noex ;
static int	cmi_verify(CMI *,time_t) noex ;
static int	cmi_auditvt(CMI *) noex ;
static int	cmi_checkupdate(CMI *,time_t) noex ;
static int	cmi_search(CMI *,uint) noex ;
static int	cmi_loadcmd(CMI *,CMI_ENT *,char *,int,int) noex ;

#if	CF_SEARCH
static int	vtecmp(cvoid *,cvoid *) noex ;
#endif


/* local variables */


/* exported variables */


/* exported subroutines */

int cmi_open(CMI *op,cchar *dbname) noex {
	const time_t	dt = time(NULL) ;
	int		rs ;
	int		nents = 0 ;
	cchar	*cp ;

	if (op == NULL) return SR_FAULT ;
	if (dbname == NULL) return SR_FAULT ;

	if (dbname[0] == '\0') return SR_INVALID ;

	memset(op,0,sizeof(CMI)) ;

	if ((rs = uc_mallocstrw(dbname,-1,&cp)) >= 0) {
	    cchar	*es = ENDIANSTR ;
	    char	tmpfname[MAXPATHLEN + 1] ;
	    op->dbname = cp ;
	    if ((rs = mkfnamesuf2(tmpfname,op->dbname,CMI_SUF,es)) >= 0) {
	        const int	tl = rs ;
	        if ((rs = uc_mallocstrw(tmpfname,tl,&cp)) >= 0) {
	            op->fname = cp ;
	            if ((rs = cmi_loadbegin(op,dt)) >= 0) {
	                nents = rs ;
	                op->ti_lastcheck = dt ;
	                op->magic = CMI_MAGIC ;
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

	return (rs >= 0) ? nents : rs ;
}
/* end subroutine (cmi_open) */


int cmi_close(CMI *op)
{
	int		rs = SR_OK ;
	int		rs1 ;

	if (op == NULL) return SR_FAULT ;

	if (op->magic != CMI_MAGIC) return SR_NOTOPEN ;

	rs1 = cmi_loadend(op) ;
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
/* end subroutine (cmi_close) */


int cmi_audit(CMI *op)
{
	int		rs = SR_OK ;

	if (op == NULL) return SR_FAULT ;

	if (op->magic != CMI_MAGIC) return SR_NOTOPEN ;

/* verify that all list pointers and list entries are valid */

	if (rs >= 0) {
	    rs = cmi_auditvt(op) ;
	}

	return rs ;
}
/* end subroutine (cmi_audit) */


int cmi_count(CMI *op)
{
	CMIHDR		*hip ;
	int		rs = SR_OK ;

	if (op == NULL) return SR_FAULT ;

	if (op->magic != CMI_MAGIC) return SR_NOTOPEN ;

	hip = &op->fhi ;
	return (rs >= 0) ? hip->nents : rs ;
}
/* end subroutine (cmi_count) */


/* this is so vital to normal operation! (no joke) */
int cmi_getinfo(CMI *op,CMI_INFO *ip)
{
	CMIHDR		*hip ;
	int		rs = SR_OK ;

	if (op == NULL) return SR_FAULT ;

	if (op->magic != CMI_MAGIC) return SR_NOTOPEN ;

	hip = &op->fhi ;

	if (ip != NULL) {
	    memset(ip,0,sizeof(CMI_INFO)) ;
	    ip->idxmtime = op->fmi.ti_mod ;
	    ip->idxctime = (time_t) hip->idxtime ;
	    ip->dbtime = (time_t) hip->dbtime ;
	    ip->dbsize = (size_t) hip->dbsize ;
	    ip->idxsize = (size_t) hip->idxsize ;
	    ip->nents = hip->nents ;
	    ip->maxent = hip->maxent ;
	}

	return (rs >= 0) ? hip->nents : rs ;
}
/* end subroutine (cmi_getinfo) */


int cmi_read(CMI *op,CMI_ENT *bvep,char *vbuf,int vlen,uint cn)
{
	int		rs = SR_OK ;
	int		vi = 0 ;

	if (op == NULL) return SR_FAULT ;
	if (bvep == NULL) return SR_FAULT ;
	if (vbuf == NULL) return SR_FAULT ;

	if (op->magic != CMI_MAGIC) return SR_NOTOPEN ;

/* check for update */

	if ((rs >= 0) && (op->ncursors == 0)) {
	    rs = cmi_checkupdate(op,0) ;
	}
	if (rs >= 0) {
	    if ((rs = cmi_search(op,cn)) >= 0) {
	        vi = rs ;
	        rs = cmi_loadcmd(op,bvep,vbuf,vlen,vi) ;
	    }
	} /* end if (ok) */

	return (rs >= 0) ? vi : rs ;
}
/* end subroutine (cmi_read) */


int cmi_curbegin(CMI *op,CMI_CUR *curp)
{

	if (op == NULL) return SR_FAULT ;
	if (curp == NULL) return SR_FAULT ;

	if (op->magic != CMI_MAGIC) return SR_NOTOPEN ;

	curp->i = 0 ;
	op->ncursors += 1 ;

	return SR_OK ;
}
/* end subroutine (cmi_curbegin) */


int cmi_curend(CMI *op,CMI_CUR *curp)
{

	if (op == NULL) return SR_FAULT ;
	if (curp == NULL) return SR_FAULT ;

	if (op->magic != CMI_MAGIC) return SR_NOTOPEN ;

	curp->i = 0 ;
	if (op->ncursors > 0) {
	    op->ncursors -= 1 ;
	}

	return SR_OK ;
}
/* end subroutine (cmi_curend) */


int cmi_enum(CMI *op,CMI_CUR *curp,CMI_ENT *bvep,char *vbuf,int vlen)
{
	CMIHDR		*hip ;
	int		rs = SR_OK ;
	int		vi ;
	int		nlines = 0 ;

	if (op == NULL) return SR_FAULT ;
	if (curp == NULL) return SR_FAULT ;
	if (bvep == NULL) return SR_FAULT ;
	if (vbuf == NULL) return SR_FAULT ;

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

static int cmi_loadbegin(CMI *op,time_t dt) noex {
	int		rs ;
	int		nents = 0 ;

	if ((rs = cmi_mapcreate(op,dt)) >= 0) {
	    rs = cmi_proc(op,dt) ;
	    nents = rs ;
	    if (rs < 0)
	        cmi_mapdestroy(op) ;
	} /* end if */

	return (rs >= 0) ? nents : rs ;
}
/* end subroutine (cmi_loadbegin) */

static int cmi_loadend(CMI *op) noex {
	CMI_FMI		*mip ;
	int		rs = SR_OK ;
	int		rs1 ;

	rs1 = cmi_mapdestroy(op) ;
	if (rs >= 0) rs = rs1 ;

	mip = &op->fmi ;
	mip->vt = NULL ;
	mip->lt = NULL ;
	return rs ;
}
/* end subroutine (cmi_loadend) */

static int cmi_mapcreate(CMI *op,time_t dt) noex {
	CMI_FMI		*mip = &op->fmi ;
	int		rs ;

	if (op->fname == NULL) return SR_BUGCHECK ;

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
	            if ((rs = u_mmap(NULL,ms,mp,mf,fd,0L,&md)) >= 0) {
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

static int cmi_mapdestroy(CMI *op) noex {
	CMI_FMI		*mip = &op->fmi ;
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
/* end subroutine (cmi_mapdestroy) */

static int cmi_checkupdate(CMI *op,time_t dt) noex {
	int		rs = SR_OK ;
	int		f = FALSE ;

	if (op->ncursors == 0) {
	    if (dt <= 0) dt = time(NULL) ;
	    if ((dt - op->ti_lastcheck) >= TO_CHECK) {
	        struct ustat	sb ;
	        CMI_FMI		*mip = &op->fmi ;
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

static int cmi_proc(CMI *op,time_t dt) noex {
	CMI_FMI		*mip = &op->fmi ;
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

static int cmi_verify(CMI *op,time_t dt) noex {
	CMI_FMI		*mip = &op->fmi ;
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

	f = f && ((hip->vioff & (sizeof(int)-1)) == 0) ;

/* size restrictions */

	f = f && (hip->vioff <= mip->mapsize) ;
	size = hip->vilen * 4 * sizeof(uint) ;
	f = f && ((hip->vioff + size) <= mip->mapsize) ;

/* alignment restriction */

	f = f && ((hip->vloff & (sizeof(int)-1)) == 0) ;

/* size restrictions */

	f = f && (hip->vloff <= mip->mapsize) ;
	size = (hip->vllen * 2 * sizeof(uint)) ;
	f = f && ((hip->vloff + size) <= mip->mapsize) ;

/* size restrictions */
	f = f && (hip->vilen == hip->nents) ;

/* get out */

	if (! f)
	    rs = SR_BADFMT ;

	return rs ;
}
/* end subroutine (cmi_verify) */

static int cmi_auditvt(CMI *op) noex {
	CMI_FMI		*mip = &op->fmi ;
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

static int cmi_search(CMI *op,uint cn) noex {
	CMI_FMI		*mip = &op->fmi ;
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

#if	CF_SEARCH
	{
	    uint	*vtep ;
	    int		vtesize = (4 * sizeof(uint)) ;
	    vtep = (uint *) bsearch(vte,vt,vtlen,vtesize,vtecmp) ;
	    rs = (vtep != NULL) ? ((vtep - vt[0]) >> 2) : SR_NOTFOUND ;
	    vi = rs ;
	}
#else /* CF_SEARCH */
	{
	    for (vi = 0 ; vi < vtlen ; vi += 1) {
	        const ushort	vkey = ((vt[vi][3] >> 16) & USHORT_MAX) ;
	        if (vkey == citekey) break ;
	    }
	    rs = (vi < vtlen) ? vi : SR_NOTFOUND ;
	}
#endif /* CF_SEARCH */

	return (rs >= 0) ? vi : rs ;
}
/* end subroutine (cmi_search) */

static int cmi_loadcmd(CMI *op,CMI_ENT *bvep,char *ebuf,int elen,int vi) noex {
	CMI_LINE	*lines ;
	CMI_FMI		*mip ;
	CMIHDR		*hip ;
	ulong		uebuf = (ulong) ebuf ;
	uint		*vte ;
	uint		(*lt)[2] ;
	uint		li ;
	int		rs = SR_OK ;
	int		bo, i ;
	int		linesize ;
	int		nlines ;

	if (bvep == NULL) return SR_FAULT ;
	if (ebuf == NULL) return SR_FAULT ;

	if (elen <= 0) return SR_OVERFLOW ;

	mip = &op->fmi ;
	hip = &op->fhi ;

	vte = mip->vt[vi] ;

/* load the basic stuff */

	memset(bvep,0,sizeof(CMI_ENT)) ;
	bvep->eoff = vte[0] ;
	bvep->elen = vte[1] ;
	bvep->nlines = ((vte[3] >> 16) & USHORT_MAX) ;
	bvep->cn = ((vte[3] >> 0) & USHORT_MAX) ;

/* load the lines */

	li = vte[2] ;
	nlines = bvep->nlines ;

	if (li < hip->vllen) {

	    bo = CMI_BO(uebuf) ;

	    linesize = ((nlines + 1) * sizeof(CMI_LINE)) ;
	    if (linesize <= (elen - (bo-uebuf))) {

	        lt = (uint (*)[2]) (mip->mapdata + hip->vloff) ;
	        lines = (CMI_LINE *) (uebuf + bo) ;
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


