/* cyimk SUPPORT */
/* charset=ISO8859-1 */
/* lang=C20 */

/* Calendar-Year-Index DB-make (make a CYI database) */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-01, David A≠D≠ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A≠D≠ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	cyimk

	Description:
	This module creates a CYI database file.

	Synopsis:
	int cyimk_open(op,year,dname,cname,of,om)
	CYIMK		*op ;
	int		year ;
	const char	dname[] ;
	const char	cname[] ;
	int		of ;
	mode_t		om ;

	Arguments:
	op		object pointer
	dname		directory path
	cname		name of calendar
	of		open-flags
	om		open (create) file permissions 
	year		year

	Returns:
	>=0		OK
	<0		error code (system-return)

	Notes:

	= possible returns to an open attempt
	- OK (creating)
	- already exists
	- doesn't exist but is in progress
	- exists and is in progress

	= open-flags

			if DB exits	if NDB exists	returns
	___________________________________________________________________

	-		no		no		SR_OK (created)
	-		no		yes		SR_INPROGRESS
	-		yes		no		SR_OK
	-		yes		yes		SR_INPROGRESS

	O_CREAT|O_EXCL	no		no		SR_OK (created)
	O_CREAT|O_EXCL	no		yes		SR_INPROGRESS
	O_CREAT|O_EXCL	yes		no		SR_EXIST
	O_CREAT|O_EXCL	yes		yes		SR_INPROGRESS

	O_CREAT		x		x		SR_OK (created)

*******************************************************************************/

#include	<envstandards.h>	/* must be before others */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<climits>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<tzfile.h>		/* for TM_YEAR_BASE */
#include	<usystem.h>
#include	<endian.h>
#include	<estrings.h>
#include	<ids.h>
#include	<storebuf.h>
#include	<filer.h>
#include	<tmtime.hh>
#include	<opentmp.h>
#include	<char.h>
#include	<isnot.h>
#include	<localmisc.h>

#include	"cyimk.h"
#include	"cyihdr.h"


/* local defines */

#define	CYIMK_DEFENTS	1024
#define	CYIMK_NSKIP	5

#define	HDRBUFLEN	(szof(CYIHDR) + 128)
#define	BUFLEN		(szof(CYIHDR) + 128)

#define	FSUF_IDX	"cyi"

#define	TO_OLDFILE	(5 * 60)

#define	MODP2(v,n)	((v) & ((n) - 1))


/* imported nameﬂpaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

struct bventry {
	uint		voff ;
	uint		vlen ;
	uint		li ;		/* index-number of first line-entry */
	uint		hash ;
	uint		citation ;	/* (nlines, m, d) */
} ;

struct blentry {
	uint		loff ;
	uint		llen ;
} ;


/* forward references */

static int	cyimk_idbegin(CYIMK *,cchar *,int) ;
static int	cyimk_idend(CYIMK *) ;
static int	cyimk_idxdir(CYIMK *,IDS *,cchar *) ;
static int	cyimk_minown(CYIMK *,cchar *,mode_t) ;

static int	cyimk_filesbegin(CYIMK *) ;
static int	cyimk_filesbeginc(CYIMK *) ;
static int	cyimk_filesbeginwait(CYIMK *) ;
static int	cyimk_filesbegincreate(CYIMK *,cchar *,int,mode_t) ;
static int	cyimk_filesend(CYIMK *) ;

static int	cyimk_listbegin(CYIMK *,int) ;
static int	cyimk_listend(CYIMK *) ;

static int	cyimk_mkidx(CYIMK *) ;
static int	cyimk_mkidxmain(CYIMK *,CYIHDR *) ;
static int	cyimk_mkidxhdr(CYIMK *,CYIHDR *,FILER *) ;
static int	cyimk_mkidxstrs(CYIMK *,CYIHDR *,FILER *,int) ;
static int	cyimk_mkidxents(CYIMK *,CYIHDR *,FILER *,int) ;
static int	cyimk_mkidxlines(CYIMK *,CYIHDR *,FILER *,int) ;
static int	cyimk_nidxopen(CYIMK *) ;
static int	cyimk_nidxclose(CYIMK *) ;
static int	cyimk_renamefiles(CYIMK *) ;

static int	mkydname(char *,cchar *,int) ;
static int	mkcitation(uint *,CYIMK_ENT *) ;
static int	mknewfname(char *,int,cchar *,cchar *) ;
static int	unlinkstale(cchar *,int) ;

static int	vvecmp(const void *,const void *) ;


/* local variables */


/* exported variables */

extern const cyimk_obj	cyimk_modinfo = {
	"cyimk",
	szof(cyimk)
} ;


/* exported subroutines */

int cyimk_open(CYIMK *op,int year,cc *dname,cc *cname,int of,mode_t om) noex {
	cint		n = CYIMK_DEFENTS ;
	int		rs = SR_OK ;
	int		c = 0 ;

	if (op == NULL) return SR_FAULT ;
	if (dname == NULL) return SR_FAULT ;
	if (cname == NULL) return SR_FAULT ;

	if (dname[0] == '\0') return SR_INVALID ;
	if (cname[0] == '\0') return SR_INVALID ;

	if (year <= 0) {
	    TMTIME	tm ;
	    time_t	dt = time(NULL) ;
	    rs = tmtime_localtime(&tm,dt) ;
	    year = (tm.year + TM_YEAR_BASE) ;
	} /* end if */

	memclear(op) ;
	op->om = (om|0600) ;
	op->nfd = -1 ;
	op->year = year ;

	op->fl.ofcreat = MKBOOL(of & O_CREAT) ;
	op->fl.ofexcl = MKBOOL(of & O_EXCL) ;
	op->fl.none = (! op->fl.ofcreat) && (! op->fl.ofexcl) ;

	if (rs >= 0) {
	    if ((rs = cyimk_idbegin(op,dname,year)) >= 0) {
	        cchar	*cp ;
	        if ((rs = uc_mallocstrw(cname,-1,&cp)) >= 0) {
	            op->cname = cp ;
	            if ((rs = cyimk_filesbegin(op)) >= 0) {
	                c = rs ;
	                if ((rs = cyimk_listbegin(op,n)) >= 0) {
	                    op->magic = CYIMK_MAGIC ;
	                }
	                if (rs < 0)
	                    cyimk_filesend(op) ;
	            }
	            if (rs < 0) {
	                if (op->cname != NULL) {
	                    uc_free(op->cname) ;
	                    op->cname = NULL ;
	                }
	            }
	        } /* end if (memory-allocation) */
	        if (rs < 0)
	            cyimk_idend(op) ;
	    } /* end if (cyim_idbegin) */
	} /* end if (ok) */

	return (rs >= 0) ? c : rs ;
}
/* end subroutine (cyimk_open) */


int cyimk_close(CYIMK *op)
{
	int		rs = SR_OK ;
	int		rs1 ;
	int		n = 0 ;
	int		f_go = FALSE ;

	if (op == NULL) return SR_FAULT ;

	if (op->magic != CYIMK_MAGIC) return SR_NOTOPEN ;

	f_go = (! op->fl.abort) ;
	n = op->nentries ;
	if (n > 0) {
	    if (op->fl.notsorted) {
	        vecobj_sort(&op->verses,vvecmp) ;
	    }
	    rs1 = cyimk_mkidx(op) ;
	    if (rs >= 0) rs = rs1 ;
	    f_go = f_go && (rs1 >= 0) ;
	}

	rs1 = cyimk_listend(op) ;
	if (rs >= 0) rs = rs1 ;
	f_go = f_go && (rs1 >= 0) ;

	if ((rs >= 0) && f_go) {
	    rs1 = cyimk_renamefiles(op) ;
	    if (rs >= 0) rs = rs1 ;
	}

	rs1 = cyimk_filesend(op) ;
	if (rs >= 0) rs = rs1 ;

	if (op->cname != NULL) {
	    rs1 = uc_free(op->cname) ;
	    if (rs >= 0) rs = rs1 ;
	    op->cname = NULL ;
	}

	rs1 = cyimk_idend(op) ;
	if (rs >= 0) rs = rs1 ;

	op->magic = 0 ;
	return (rs >= 0) ? n : rs ;
}
/* end subroutine (cyimk_close) */


int cyimk_add(CYIMK *op,CYIMK_ENT *bvp)
{
	uint		li = UINT_MAX ;
	int		rs = SR_OK ;

	if (op == NULL) return SR_FAULT ;
	if (bvp == NULL) return SR_FAULT ;

	if (op->magic != CYIMK_MAGIC) return SR_NOTOPEN ;

	if ((bvp->lines != NULL) && (bvp->nlines > 0)) {
	    struct blentry	ble ;
	    cint		imax = UCHAR_MAX ;
	    int			i ;
	    for (i = 0 ; (i < bvp->nlines) && (i < imax) ; i += 1) {

	        ble.loff = bvp->lines[i].loff ;
	        ble.llen = bvp->lines[i].llen ;
	        rs = vecobj_add(&op->lines,&ble) ;
	        if (i == 0) li = rs ;

	        if (rs < 0) break ;
	    } /* end for */
	} /* end if */

	if (rs >= 0) {
	    struct bventry	bve ;
	    uint		citcmpval ;
	    bve.voff = bvp->voff ;
	    bve.vlen = bvp->vlen ;
	    bve.li = li ;
	    bve.hash = bvp->hash ;
	    mkcitation(&bve.citation,bvp) ;
	    citcmpval = (bve.citation & 0x0000FFFF) ;
	    if (citcmpval < op->pcitation) op->fl.notsorted = TRUE ;
	    op->pcitation = citcmpval ;
	    rs = vecobj_add(&op->verses,&bve) ;
	    op->nentries += 1 ;
	}

	return rs ;
}
/* end subroutine (cyimk_add) */


int cyimk_abort(CYIMK *op,int f)
{
	op->fl.abort = f ;
	return SR_OK ;
}
/* end subroutine (cyimk_abort) */


/* private subroutines */

static int cyimk_idbegin(CYIMK *op,cchar *dname,int year) noex {
	int		rs ;
	if (USTAT sb ; (rs = uc_stat(dname,&sb)) >= 0) {
	    cint	am = (W_OK|X_OK) ;
	    op->gid = sb.st_gid ;
	    op->uid = sb.st_uid ;
	    if (ids id ; (rs = ids_load(&id)) >= 0) {
	        if ((rs = permid(&id,&sb,am)) >= 0) {
	            char	ydname[MAXPATHLEN+1] ;
	            if ((rs = mkydname(ydname,dname,year)) >= 0) {
		        if ((rs = cyimk_idxdir(op,&id,ydname)) >= 0) {
	                    cchar	*cp ;
	                    if ((rs = uc_mallocstrw(ydname,-1,&cp)) >= 0) {
	                        op->idname = cp ;
	                    }
		        } /* end if (cyimk_idxdir) */
	            } /* end if (mkydname) */
		} /* end if (permid) */
		ids_release(&id) ;
	    } /* end if (ids) */
	} /* end if */
	return rs ;
}
/* end subroutine (cyimk_idbegin) */


static int cyimk_idxdir(CYIMK *op,IDS *idp,cchar *ydname)
{
	ustat	sb ;
	const mode_t	dm = 0777 ;
	cint	nrs = SR_NOENT ;
	int		rs = SR_OK ;
	if ((rs = uc_stat(ydname,&sb)) >= 0) {
	    cint	am = (W_OK|X_OK) ;
	    uid_t	uid_yd = sb.st_uid ;
	    gid_t	gid_yd = sb.st_gid ;
	    if ((rs = permid(idp,&sb,am)) >= 0) {
		uid_t	uid = geteuid() ;
		if (uid == uid_yd) {
		    if ((uid_yd != op->uid) || (gid_yd != op->gid)) {
			rs = cyimk_minown(op,ydname,dm) ;
		    }
		}
	    } /* end if (permid) */
	} else if (rs == nrs) {
	    if ((rs = mkdirs(ydname,dm)) >= 0) {
		rs = cyimk_minown(op,ydname,dm) ;
	    } /* end if (mkdirs) */
	}
	return rs ;
}
/* end subroutine (cyimk_idxdir) */


static int cyimk_minown(CYIMK *op,cchar *dname,mode_t dm)
{
	int		rs ;
		if ((rs = uc_minmod(dname,dm)) >= 0) {
		    gid_t	gid = op->gid ;
		    uid_t	uid = op->uid ;
		    uc_chown(dname,uid,gid) ;
		}
	return rs ;
}
/* end subroutine (cyimk_minown) */


static int cyimk_idend(CYIMK *op)
{
	int		rs = SR_OK ;
	int		rs1 ;
	if (op->idname != NULL) {
	    rs1 = uc_free(op->idname) ;
	    if (rs >= 0) rs = rs1 ;
	    op->idname = NULL ;
	}
	return rs ;
}
/* end subroutine (cyimk_idend) */


static int cyimk_filesbegin(CYIMK *op)
{
	int		rs = SR_OK ;
	int		c = 0 ;
	if (op->fl.ofcreat) {
	    rs = cyimk_filesbeginc(op) ;
	} else {
	    rs = cyimk_filesbeginwait(op) ;
	    c = rs ;
	}
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (cyimk_filesbegin) */


static int cyimk_filesbeginc(CYIMK *op)
{
	cint	type = (op->fl.ofcreat && (! op->fl.ofexcl)) ;
	int		rs ;
	char		dbn[MAXPATHLEN+1] ;
	if ((rs = mkpath2(dbn,op->idname,op->cname)) >= 0) {
	cchar		*suf = FSUF_IDX	 ;
	char		tbuf[MAXPATHLEN+1] ;
	if ((rs = mknewfname(tbuf,type,dbn,suf)) >= 0) {
	    const mode_t	om = op->om ;
	    cchar		*tfn = tbuf ;
	    char		rbuf[MAXPATHLEN+1] ;
	    if (type) {
	        if ((rs = mktmpfile(rbuf,om,tbuf)) >= 0) {
	            op->fl.created = TRUE ;
	            tfn = rbuf ;
	        }
	    }
	    if (rs >= 0) {
	        mode_t	om = op->om ;
	        int	of = O_CREAT ;
	        if (op->fl.ofexcl) of |= O_EXCL ;
	        rs = cyimk_filesbegincreate(op,tfn,of,om) ;
	        if ((rs < 0) && type) {
	            uc_unlink(rbuf) ;
	        }
	    } /* end if (ok) */
	} /* end if (mknewfname) */
	} /* end if (mkpath) */
	return rs ;
}
/* end subroutine (cyimk_filesbeginc) */


static int cyimk_filesbeginwait(CYIMK *op)
{
	int		rs ;
	int		c = 0 ;
	char		dbn[MAXPATHLEN+1] ;
	if ((rs = mkpath2(dbn,op->idname,op->cname)) >= 0) {
	cchar		*suf = FSUF_IDX	 ;
	char		tbuf[MAXPATHLEN+1] ;
	if ((rs = mknewfname(tbuf,FALSE,dbn,suf)) >= 0) {
	    const mode_t	om = op->om ;
	    cint		to_stale = CYIMK_INTSTALE ;
	    cint		nrs = SR_EXISTS ;
	    cint		of = (O_CREAT|O_WRONLY|O_EXCL) ;
	    int			to = CYIMK_INTOPEN ;
	    while ((rs = cyimk_filesbegincreate(op,tbuf,of,om)) == nrs) {
	        c = 1 ;
	        sleep(1) ;
	        unlinkstale(tbuf,to_stale) ;
	        if (to-- == 0) break ;
	    } /* end while (db exists) */
	    if (rs == nrs) {
	        op->fl.ofcreat = FALSE ;
	        c = 0 ;
	        rs = cyimk_filesbeginc(op) ;
	    }
	} /* end if (mknewfname) */
	} /* end if (mkpath) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (cyimk_filesbeginwait) */


static int cyimk_filesbegincreate(CYIMK *op,cchar *tfn,int of,mode_t om)
{
	int		rs ;
	if ((rs = uc_open(tfn,of,om)) >= 0) {
	    cint	fd = rs ;
	    cchar	*cp ;
	    op->fl.created = TRUE ;
	    if ((rs = uc_mallocstrw(tfn,-1,&cp)) >= 0) {
	        op->nidxfname = (char *) cp ;
	    }
	    u_close(fd) ;
	} /* end if (create) */

	return rs ;
}
/* end subroutine (cyimk_filesbegincreate) */


static int cyimk_filesend(CYIMK *op)
{
	int		rs = SR_OK ;
	int		rs1 ;

	if (op->nfd >= 0) {
	    rs1 = u_close(op->nfd) ;
	    if (rs >= 0) rs = rs1 ;
	    op->nfd = -1 ;
	}

	if (op->nidxfname != NULL) {
	    if (op->fl.created && (op->nidxfname[0] != '\0')) {
	        rs1 = u_unlink(op->nidxfname) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    rs1 = uc_free(op->nidxfname) ;
	    if (rs >= 0) rs = rs1 ;
	    op->nidxfname = NULL ;
	}

	return rs ;
}
/* end subroutine (cyimk_filesend) */


static int cyimk_listbegin(CYIMK *op,int n)
{
	int		rs ;
	int		sz ;
	int		opts ;

	opts = 0 ;
	opts |= VECOBJ_OSTATIONARY ;
	opts |= VECOBJ_OORDERED ;
	opts |= VECOBJ_OCOMPACT ;
	sz = szof(struct bventry) ;
	if ((rs = vecobj_start(&op->verses,sz,n,opts)) >= 0) {
	    rs = vecobj_start(&op->lines,sz,(n * 2),opts) ;
	    if (rs < 0) {
	        vecobj_finish(&op->verses) ;
	    }
	}

	return rs ;
}
/* end subroutine (cyimk_listbegin) */


static int cyimk_listend(CYIMK *op)
{
	int		rs = SR_OK ;
	int		rs1 ;

	rs1 = vecobj_finish(&op->lines) ;
	if (rs >= 0) rs = rs1 ;

	rs1 = vecobj_finish(&op->verses) ;
	if (rs >= 0) rs = rs1 ;

	return rs ;
}
/* end subroutine (cyimk_listend) */


static int cyimk_mkidx(CYIMK *op)
{
	int		rs ;
	int		rs1 ;
	int		wlen = 0 ;

	if ((rs = cyimk_nidxopen(op)) >= 0) {
	    CYIHDR	hdr{} ;
	    hdr.vetu[0] = CYIHDR_VERSION ;
	    hdr.vetu[1] = ENDIAN ;
	    hdr.vetu[2] = 0 ;
	    hdr.vetu[3] = 0 ;
	    hdr.wtime = (uint) time(NULL) ;
	    hdr.nentries = op->nentries ;
	    hdr.nskip = CYIMK_NSKIP ;
	    hdr.year = op->year ;

	    if ((rs = cyimk_mkidxmain(op,&hdr)) >= 0) {
	        cint	hlen = HDRBUFLEN ;
	        char		hbuf[HDRBUFLEN+1] ;
	        hdr.fsize = rs ;
	        wlen = rs ;

	        if ((rs = cyihdr(&hdr,0,hbuf,hlen)) >= 0) {
	            cint	bl = rs ;
	            if ((rs = u_pwrite(op->nfd,hbuf,bl,0L)) >= 0) {
	                const mode_t	om = op->om ;
	                rs = uc_fminmod(op->nfd,om) ;
	            }
	        }

	    } /* end if (cyimk_mkidxmain) */

	    rs1 = cyimk_nidxclose(op) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (cyimk_nidx) */

	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (cyimk_mkidx) */


static int cyimk_mkidxmain(CYIMK *op,CYIHDR *hdrp)
{
	FILER		hf, *hfp = &hf ;
	cint	nfd = op->nfd ;
	cint	ps = getpagesize() ;
	int		bsz ;
	int		rs ;
	int		rs1 ;
	int		off = 0 ;
	bsz = (ps * 4) ;
	if ((rs = filer_start(hfp,nfd,0,bsz,0)) >= 0) {
	    if ((rs = cyimk_mkidxhdr(op,hdrp,hfp)) >= 0) {
	        off += rs ;
	        if (rs >= 0) {
	            rs = cyimk_mkidxstrs(op,hdrp,hfp,off) ;
	            off += rs ;
	        }
	        if (rs >= 0) {
	            rs = cyimk_mkidxents(op,hdrp,hfp,off) ;
	            off += rs ;
	        }
	        if (rs >= 0) {
	            rs = cyimk_mkidxlines(op,hdrp,hfp,off) ;
	            off += rs ;
	        }
	    } /* end if (cyimk_mkidxhdr) */
	    rs1 = filer_finish(hfp) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (filer) */
	return (rs >= 0) ? off : rs ;
}
/* end subroutine (cyimk_mkidxmain) */


/* ARGSUSED */
static int cyimk_mkidxhdr(CYIMK *op,CYIHDR *hdrp,FILER *hfp)
{
	cint	hlen = HDRBUFLEN ;
	int		rs ;
	int		wlen = 0 ;
	char		hbuf[HDRBUFLEN+1] ;
	if (op == NULL) return SR_FAULT ; /* LINT */
	if ((rs = cyihdr(hdrp,0,hbuf,hlen)) >= 0) {
	    rs = filer_writefill(hfp,hbuf,rs) ;
	    wlen += rs ;
	}
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (cyimk_mkidxhdr) */


static int cyimk_mkidxstrs(CYIMK *op,CYIHDR *hdrp,FILER *hfp,int off)
{
	int		rs ;
	int		wlen = 0 ;
	char		tbuf[MAXPATHLEN + 1] ;

	hdrp->diroff = off ;
	if ((rs = pathclean(tbuf,op->idname,-1)) >= 0) {
	    int	tl = rs ;
	    if ((rs = filer_writefill(hfp,tbuf,(tl+1))) >= 0) {
	        tl = lenstr(op->cname) ;
	        off += rs ;
	        wlen += rs ;
	        hdrp->caloff = off ;
	        rs = filer_writefill(hfp,op->cname,(tl+1)) ;
	        off += rs ;
	        wlen += rs ;
	    } /* end if (filer_writefill) */
	} /* end if (pathclean) */

	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (cyimk_mkidxstrs) */


static int cyimk_mkidxents(CYIMK *op,CYIHDR *hdrp,FILER *hfp,int off)
{
	struct bventry	*bvep ;
	vecobj		*elp = &op->verses ;
	uint		a[5] ;
	cint	size = (5 * szof(uint)) ;
	int		rs = SR_OK ;
	int		i ;
	int		n = 0 ;
	int		wlen = 0 ;

	hdrp->vioff = off ;
	for (i = 0 ; vecobj_get(elp,i,&bvep) >= 0 ; i += 1) {
	    if (bvep != NULL) {
	        a[0] = bvep->voff ;
	        a[1] = bvep->vlen ;
	        a[2] = bvep->li ;
	        a[3] = bvep->citation ;
	        a[4] = bvep->hash ;
	        n += 1 ;
	        rs = filer_write(hfp,a,size) ;
	        off += rs ;
	        wlen += rs ;
	    }
	    if (rs < 0) break ;
	} /* end for */
	hdrp->vilen = n ;

	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (cyimk_mkidxents) */


static int cyimk_mkidxlines(CYIMK *op,CYIHDR *hdrp,FILER *hfp,int off)
{
	struct blentry	*blep ;
	vecobj		*llp = &op->lines ;
	uint		a[2] ;
	cint	size = (2 * szof(uint)) ;
	int		rs = SR_OK ;
	int		n = 0 ;
	int		i ;
	int		wlen = 0 ;

	hdrp->vloff = off ;
	for (i = 0 ; vecobj_get(llp,i,&blep) >= 0 ; i += 1) {
	    if (blep != NULL) {
	        a[0] = blep->loff ;
	        a[1] = blep->llen ;
	        n += 1 ;
	        rs = filer_write(hfp,a,size) ;
	        off += rs ;
	        wlen += rs ;
	    }
	    if (rs < 0) break ;
	} /* end for */
	hdrp->vllen = n ;

	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (cyimk_mkidxlines) */


static int cyimk_nidxopen(CYIMK *op)
{
	const mode_t	om = op->om ;
	int		rs ;
	int		fd = -1 ;
	int		of = (O_CREAT|O_WRONLY) ;
	if (op->nidxfname == NULL) {
	    cint	type = (op->fl.ofcreat && (! op->fl.ofexcl)) ;
	    cchar	*dbn = op->cname ;
	    cchar	*suf = FSUF_IDX ;
	    char	tbuf[MAXPATHLEN+1] ;
	    if ((rs = mknewfname(tbuf,type,dbn,suf)) >= 0) {
	        cchar	*tfn = tbuf ;
	        char	rbuf[MAXPATHLEN+1] ;
	        if (type) {
	            rs = opentmpfile(tbuf,of,om,rbuf) ;
	            op->nfd = rs ;
	            fd = rs ;
	            tfn = rbuf ;
	        } else {
	            if (op->fl.ofexcl) of |= O_EXCL ;
	            rs = uc_open(tbuf,of,om) ;
	            op->nfd = rs ;
	            fd = rs ;
	        }
	        if (rs >= 0) {
	            cchar	*cp ;
	            if ((rs = uc_mallocstrw(tfn,-1,&cp)) >= 0) {
	                op->nidxfname = (char *) cp ;
	            }
	        } /* end if (ok) */
	    } /* end if (mknewfname) */
	} else {
	    if (op->fl.ofexcl) of |= O_EXCL ;
	    rs = uc_open(op->nidxfname,of,om) ;
	    op->nfd = rs ;
	    fd = rs ;
	}
	return (rs >= 0) ? fd : rs ;
}
/* end subroutine (cyimk_nidxopen) */


static int cyimk_nidxclose(CYIMK *op)
{
	int		rs = SR_OK ;
	int		rs1 ;
	if (op->nfd >= 0) {
	    rs1 = u_close(op->nfd) ;
	    if (rs >= 0) rs = rs1 ;
	    op->nfd = -1 ;
	}
	return rs ;
}
/* end subroutine (cyimk_nidxclose) */


static int cyimk_renamefiles(CYIMK *op)
{
	int		rs ;
	cchar		*suf = FSUF_IDX ;
	cchar		*end = ENDIANSTR ;
	char		dbn[MAXPATHLEN+1] ;
	if ((rs = mkpath2(dbn,op->idname,op->cname)) >= 0) {
	    char	idxfname[MAXPATHLEN + 1] ;
	if ((rs = mkfnamesuf2(idxfname,dbn,suf,end)) >= 0) {
	    if ((rs = u_rename(op->nidxfname,idxfname)) >= 0) {
	        op->nidxfname[0] = '\0' ;
	    } else {
	        u_unlink(op->nidxfname) ;
	        op->nidxfname[0] = '\0' ;
	    }
	} /* end if (mkfnamesuf) */
	} /* end if (mkpath) */
	return rs ;
}
/* end subroutine (cyimk_renamefiles) */


static int mkcitation(uint *cip,CYIMK_ENT *bvp)
{
	uint		ci = 0 ;
	uint		nlines ;

	nlines = (bvp->lines != NULL) ? bvp->nlines : 0 ;

	ci |= ((nlines & UCHAR_MAX) << 24) ;
	ci |= ((bvp->m & UCHAR_MAX) << 8) ;
	ci |= ((bvp->d & UCHAR_MAX) << 0) ;

	*cip = ci ;
	return SR_OK ;
}
/* end subroutine (mkcitation) */

static int mkydname(char *rbuf,cchar *dname,int year) noex {
	cint	rlen = MAXPATHLEN ;
	int		rs = SR_OK ;
	int		i = 0 ;
	if (rs >= 0) {
	    rs = storebuf_strw(rbuf,rlen,i,dname,-1) ;
	    i += rs ;
	}
	if (rs >= 0) {
	    rs = storebuf_chr(rbuf,rlen,i,'/') ;
	    i += rs ;
	}
	if (rs >= 0) {
	    rs = storebuf_chr(rbuf,rlen,i,'y') ;
	    i += rs ;
	}
	if (rs >= 0) {
	    rs = storebuf_deci(rbuf,rlen,i,year) ;
	    i += rs ;
	}
	return (rs >= 0) ? i : rs ;
}
/* end subroutine (mkydname) */


static int mknewfname(char *tbuf,int type,cchar *dbn,cchar *suf)
{
	cchar		*end = ENDIANSTR ;
	cchar		*fin = (type) ? "xXXXX" : "n" ;
	return mkfnamesuf3(tbuf,dbn,suf,end,fin) ;
}
/* end subroutine (mknewfname) */


static int unlinkstale(cchar *fn,int to)
{
	ustat	sb ;
	const time_t	dt = time(NULL) ;
	int		rs ;
	if ((rs = uc_stat(fn,&sb)) >= 0) {
	    if ((dt-sb.st_mtime) >= to) {
	        uc_unlink(fn) ;
	        rs = 1 ;
	    } else {
	        rs = 0 ;
	    }
	} else if (isNotPresent(rs)) {
	    rs = SR_OK ;
	}
	return rs ;
}
/* end subroutine (unlinkstale) */


static int vvecmp(const void *v1p,const void *v2p)
{
	struct bventry	**e1pp = (struct bventry **) v1p ;
	struct bventry	**e2pp = (struct bventry **) v2p ;
	int		rc = 0 ;

	if (*e1pp != NULL) {
	    if (*e2pp != NULL) {
	        uint	vc1 = (*e1pp)->citation & 0x0000FFFF ;
	        uint	vc2 = (*e2pp)->citation & 0x0000FFFF ;
	        rc = (vc1 - vc2) ;
	    } else
	        rc = -1 ;
	} else
	    rc = 1 ;

	return rc ;
}
/* end subroutine (vvecmp) */


