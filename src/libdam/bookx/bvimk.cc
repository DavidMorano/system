/* bvimk SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* make a BVI database */
/* version %I% last-modified %G% */


/* revision history:

	= 2008-10-01, David A­D­ Morano
	This module was originally written.

*/

/* Copyright © 2008 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	bvimk

	Description:
	This module creates a BVI database file.

	Synopsis:
	int bvimk_open(bvimk *op,cchar *dbname,...) noex

	Arguments:
	- op		object pointer
	- dbname	name of (path-to) DB

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
#include	<ctime>
#include	<climits>		/* |UCHAR_MAX| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<endian.h>
#include	<estrings.h>
#include	<vecobj.h>
#include	<filer.h>
#include	<opentmp.h>
#include	<char.h>
#include	<localmisc.h>

#include	"bvimk.h"
#include	"bvihdr.h"


/* local defines */

#define	BVIMK_NENTS	(19 * 1024)
#define	BVIMK_NSKIP	5
#define	HDRBUFLEN	(szof(bvihdr) + 128)

#define	BUFLEN		(szof(bvihdr) + 128)

#define	FSUF_IDX	"bvi"

#define	TO_OLDFILE	(5 * 60)

#define	MODP2(v,n)	((v) & ((n) - 1))


/* external subroutines */


/* external variables */


/* local structures */

struct bventry {
	uint	voff ;
	uint	vlen ;
	uint	li ;			/* index-number of first line-entry */
	uint	citation ;		/* (nlines, b, c, v) */
} ;

struct blentry {
	uint	loff ;
	uint	llen ;
} ;


/* forward references */

static int	bvimk_filesbegin(bvimk *) noex ;
static int	bvimk_filesbeginc(bvimk *) noex ;
static int	bvimk_filesbeginwait(bvimk *) noex ;
static int	bvimk_filesbegincreate(bvimk *,cchar *,int,mode_t) noex ;
static int	bvimk_filesend(bvimk *) noex ;
static int	bvimk_listbegin(bvimk *,int) noex ;
static int	bvimk_listend(bvimk *) noex ;
static int	bvimk_mkidx(bvimk *) noex ;
static int	bvimk_mkidxwrmain(bvimk *,bvihdr *) noex ;
static int	bvimk_mkidxwrhdr(bvimk *,bvihdr *,filer *) noex ;
static int	bvimk_mkidxwrverses(bvimk *,bvihdr *,filer *,int) noex ;
static int	bvimk_mkidxwrlines(bvimk *,bvihdr *,filer *,int) noex ;
static int	bvimk_nidxopen(bvimk *) noex ;
static int	bvimk_nidxclose(bvimk *) noex ;
static int	bvimk_renamefiles(bvimk *) noex ;

static int	mkcitation(uint *,bvimk_v *) noex ;
static int	mknewfname(char *,int,cchar *,cchar *) noex ;
static int	unlinkstale(cchar *,int) noex ;

static int	vvecmp(cvoid *,cvoid *) noex ;


/* local variables */


/* exported variables */

bvimk_obj bvimk_modinfo = {
	"bvimk",
	szof(bvimk)
} ;


/* exported subroutines */

int bvimk_open(bvimk *op,cchar *dbname,int of,mode_t om) noex {
	cint		n = BVIMK_NENTS ;
	int		rs ;
	int		c = 0 ;

	if (op == NULL) return SR_FAULT ;
	if (dbname == NULL) return SR_FAULT ;
	if (dbname[0] == '\0') return SR_INVALID ;

	memclear(op) ;
	op->om = (om|0600) ;
	op->nfd = -1 ;

	op->fl.ofcreat = MKBOOL(of & O_CREAT) ;
	op->fl.ofexcl = MKBOOL(of & O_EXCL) ;

	if (cchar *cp{} ; (rs = uc_mallocstrw(dbname,-1,&cp)) >= 0) {
	    op->dbname = cp ;
	    if ((rs = bvimk_filesbegin(op)) >= 0) {
	        c = rs ;
	        if ((rs = bvimk_listbegin(op,n)) >= 0) {
	            op->magic = BVIMK_MAGIC ;
	        }
	        if (rs < 0) {
	            bvimk_filesend(op) ;
		}
	    } /* end if (nvimk) */
	    if (rs < 0) {
	        uc_free(op->dbname) ;
	        op->dbname = NULL ;
	    }
	} /* end if (m-a) */

	return (rs >= 0) ? c : rs ;
}
/* end subroutine (bvimk_open) */

int bvimk_close(bvimk *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	int		nverses = 0 ;
	int		f_go = FALSE ;

	if (op == NULL) return SR_FAULT ;

	if (op->magic != BVIMK_MAGIC) return SR_NOTOPEN ;

	f_go = (! op->fl.abort) ;
	if (op->fl.notsorted) {
	    vecobj_sort(&op->verses,vvecmp) ;
	}

	nverses = op->nverses ;
	if (nverses > 0) {
	    rs1 = bvimk_mkidx(op) ;
	    if (rs >= 0) rs = rs1 ;
	    f_go = f_go && (rs1 >= 0) ;
	}

	if (op->nfd >= 0) {
	    rs1 = u_close(op->nfd) ;
	    if (rs >= 0) rs = rs1 ;
	    op->nfd = -1 ;
	}

	rs1 = bvimk_listend(op) ;
	if (rs >= 0) rs = rs1 ;
	f_go = f_go && (rs1 >= 0) ;

	if ((rs >= 0) && (nverses > 0) && f_go) {
	    rs1 = bvimk_renamefiles(op) ;
	    if (rs >= 0) rs = rs1 ;
	}

	rs1 = bvimk_filesend(op) ;
	if (rs >= 0) rs = rs1 ;

	if (op->dbname != NULL) {
	    rs1 = uc_free(op->dbname) ;
	    if (rs >= 0) rs = rs1 ;
	    op->dbname = NULL ;
	}

	op->magic = 0 ;
	return (rs >= 0) ? nverses : rs ;
}
/* end subroutine (bvimk_close) */

int bvimk_add(bvimk *op,bvimk_v *bvp) noex {
	struct bventry	bve ;
	struct blentry	ble ;
	uint		li = UINT_MAX ;
	int		rs = SR_OK ;

	if (op == NULL) return SR_FAULT ;
	if (bvp == NULL) return SR_FAULT ;

	if (op->magic != BVIMK_MAGIC) return SR_NOTOPEN ;

	if ((bvp->lines != NULL) && (bvp->nlines > 0)) {
	    int	i ;
	    for (i = 0 ; i < bvp->nlines ; i += 1) {

	        ble.loff = bvp->lines[i].loff ;
	        ble.llen = bvp->lines[i].llen ;
	        rs = vecobj_add(&op->lines,&ble) ;
	        if (i == 0) li = rs ;

	        if (rs < 0) break ;
	    } /* end for */
	} /* end if */

	if (rs >= 0) {
	    uint	citcmpval ;

	    bve.voff = bvp->voff ;
	    bve.vlen = bvp->vlen ;
	    bve.li = li ;
	    mkcitation(&bve.citation,bvp) ;

	    citcmpval = (bve.citation & 0x00FFFFFF) ;
	    if (citcmpval < op->pcitation) {
	        op->fl.notsorted = TRUE ;
	    }

	    op->pcitation = citcmpval ;

	    if ((rs = vecobj_add(&op->verses,&bve)) >= 0) {
		uint	v ;
	        op->nverses += 1 ;
	        if ((bvp->b > 0) && (bvp->c > 0) && (bvp->v > 0)) {
	            op->nzverses += 1 ;
	        }
	        v = bvp->b ;
	        if (v > op->maxbook) op->maxbook = v ;
	        v = bvp->c ;
	        if (v > op->maxchapter) op->maxchapter = v ;
	        v = bvp->v ;
	        if (v > op->maxverse) op->maxverse = v ;
	    } /* end if (add) */

	} /* end if (ok) */

	return rs ;
}
/* end subroutine (bvimk_add) */

int bvimk_abort(bvimk *op,int f) noex {
	op->fl.abort = f ;
	return SR_OK ;
}
/* end subroutine (bvimk_abort) */

int bvimk_info(bvimk *op,BVIMK_INFO *bip) noex {
	int		rs = SR_OK ;
	int		nverses ;

	if (op == NULL) return SR_FAULT ;

	if (op->magic != BVIMK_MAGIC) return SR_NOTOPEN ;

	nverses = op->nverses ;
	if (bip != NULL) {
	    bip->maxbook = op->maxbook ;
	    bip->maxchapter = op->maxchapter ;
	    bip->maxverse = op->maxverse ;
	    bip->nverses = op->nverses ;
	    bip->nzverses = op->nzverses ;
	}

	return (rs >= 0) ? nverses : rs ;
}
/* end subroutine (bvimk_info) */


/* private subroutines */

static int bvimk_filesbegin(bvimk *op) noex {
	int		rs = SR_OK ;
	int		c = 0 ;
	if (op->fl.ofcreat) {
	    rs = bvimk_filesbeginc(op) ;
	} else {
	    rs = bvimk_filesbeginwait(op) ;
	    c = rs ;
	}
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (bvimk_filesbegin) */

static int bvimk_filesbeginc(bvimk *op) noex {
	cint		type = (op->fl.ofcreat && (! op->fl.ofexcl)) ;
	int		rs ;
	cchar		*dbn = op->dbname ;
	cchar		*suf = FSUF_IDX	 ;
	char		tbuf[MAXPATHLEN+1] ;
	if ((rs = mknewfname(tbuf,type,dbn,suf)) >= 0) {
	    cchar	*tfn = tbuf ;
	    char	rbuf[MAXPATHLEN+1] ;
	    cmode	om = op->om ;
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
	        rs = bvimk_filesbegincreate(op,tfn,of,om) ;
		if ((rs < 0) && type) {
		    uc_unlink(rbuf) ;
		}
	    } /* end if (ok) */
	} /* end if (mknewfname) */
	return rs ;
}
/* end subroutine (bvimk_filesbeginc) */

static int bvimk_filesbeginwait(bvimk *op) noex {
	int		rs ;
	int		c = 0 ;
	cchar		*dbn = op->dbname ;
	cchar		*suf = FSUF_IDX	 ;
	char		tbuf[MAXPATHLEN+1] ;
	if ((rs = mknewfname(tbuf,FALSE,dbn,suf)) >= 0) {
	    cint	to_stale = BVIMK_INTSTALE ;
	    cint	rsn = SR_EXISTS ;
	    cint	of = (O_CREAT|O_WRONLY|O_EXCL) ;
	    int		to = BVIMK_INTOPEN ;
	    cmode	om = op->om ;
	    while ((rs = bvimk_filesbegincreate(op,tbuf,of,om)) == rsn) {
	        c = 1 ;
	        sleep(1) ;
	        unlinkstale(tbuf,to_stale) ;
	        if (to-- == 0) break ;
	    } /* end while (db exists) */
	    if (rs == rsn) {
	        op->fl.ofcreat = FALSE ;
	        c = 0 ;
	        rs = bvimk_filesbeginc(op) ;
	    }
	} /* end if (mknewfname) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (bvimk_filesbeginwait) */

static int bvimk_filesbegincreate(bvimk *op,cchar *tfn,int of,mode_t om) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = uc_open(tfn,of,om)) >= 0) {
	    cint	fd = rs ;
	    op->fl.created = TRUE ;
	    if (cchar *cp{} ; (rs = uc_mallocstrw(tfn,-1,&cp)) >= 0) {
	        op->nidxfname = charp(cp) ;
	    }
	    rs1 = u_close(fd) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (create) */

	return rs ;
}
/* end subroutine (bvimk_filesbegincreate) */

static int bvimk_filesend(bvimk *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (op->nidxfname != NULL) {
	    if (op->fl.created && (op->nidxfname[0] != '\0')) {
	        u_unlink(op->nidxfname) ;
	    }
	    rs1 = uc_free(op->nidxfname) ;
	    if (rs >= 0) rs = rs1 ;
	    op->nidxfname = NULL ;
	}
	if (op->idname != NULL) {
	    rs1 = uc_free(op->idname) ;
	    if (rs >= 0) rs = rs1 ;
	    op->idname = NULL ;
	}
	return rs ;
}
/* end subroutine (bvimk_filesend) */

static int bvimk_listbegin(bvimk *op,int n) noex {
	int		rs ;
	int		sz ;
	int		opts = 0 ;
	opts |= VECOBJ_OCOMPACT ;
	opts |= VECOBJ_OORDERED ;
	opts |= VECOBJ_OSTATIONARY ;
	sz = szof(struct bventry) ;
	if ((rs = vecobj_start(&op->verses,sz,n,opts)) >= 0) {
	    rs = vecobj_start(&op->lines,sz,(n * 2),opts) ;
	    if (rs < 0)
	        vecobj_finish(&op->verses) ;
	}

	return rs ;
}
/* end subroutine (bvimk_listbegin) */

static int bvimk_listend(bvimk *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	{
	rs1 = vecobj_finish(&op->lines) ;
	if (rs >= 0) rs = rs1 ;
	}
	{
	rs1 = vecobj_finish(&op->verses) ;
	if (rs >= 0) rs = rs1 ;
	}
	return rs ;
}
/* end subroutine (bvimk_listend) */

static int bvimk_mkidx(bvimk *op) noex {
	int		rs ;
	int		rs1 ;
	int		wlen = 0 ;

	if ((rs = bvimk_nidxopen(op)) >= 0) {
	    bvihdr	hdr{} ;
	    hdr.vetu[0] = bvihdr_VERSION ;
	    hdr.vetu[1] = ENDIAN ;
	    hdr.vetu[2] = 0 ;
	    hdr.vetu[3] = 0 ;
	    hdr.wtime = (uint) time(NULL) ;
	    hdr.nverses = op->nverses ;
	    hdr.nzverses = op->nzverses ;
	    hdr.maxbook = op->maxbook ;
	    hdr.maxchapter = op->maxchapter ;

	    if ((rs = bvimk_mkidxwrmain(op,&hdr)) >= 0) {
	        cint	hlen = HDRBUFLEN ;
	        char		hbuf[HDRBUFLEN+1] ;
	        hdr.fsize = rs ;
	        wlen = rs ;

	        if ((rs = bvihdr(&hdr,0,hbuf,hlen)) >= 0) {
	            cint	bl = rs ;
	            if ((rs = u_pwrite(op->nfd,hbuf,bl,0L)) >= 0) {
	                const mode_t	om = op->om ;
	                rs = uc_fminmod(op->nfd,om) ;
	            }
	        }

	    } /* end if (bvimk_mkidxwrmain) */

	    rs1 = bvimk_nidxclose(op) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (bvimk_nidx) */

	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (bvimk_mkidx) */

static int bvimk_mkidxwrmain(bvimk *op,bvihdr *hdrp) noex {
	filer		hf, *hfp = &hf ;
	cint	nfd = op->nfd ;
	cint	ps = getpagesize() ;
	int		bsize ;
	int		rs ;
	int		rs1 ;
	int		off = 0 ;
	bsize = (ps * 4) ;
	if ((rs = filer_start(hfp,nfd,0,bsize,0)) >= 0) {
	    if ((rs = bvimk_mkidxwrhdr(op,hdrp,hfp)) >= 0) {
	        off += rs ;
	        if (rs >= 0) {
	            rs = bvimk_mkidxwrverses(op,hdrp,hfp,off) ;
	            off += rs ;
	        }
	        if (rs >= 0) {
	            rs = bvimk_mkidxwrlines(op,hdrp,hfp,off) ;
	            off += rs ;
	        }
	    } /* end if (bvimk_mkidxwrhdr) */
	    rs1 = filer_finish(hfp) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (filer) */
	return (rs >= 0) ? off : rs ;
}
/* end subroutine (bvimk_mkidxwrmain) */


/* ARGSUSED */
static int bvimk_mkidxwrhdr(bvimk *op,bvihdr *hdrp,filer *hfp)
{
	cint	hlen = HDRBUFLEN ;
	int		rs ;
	int		wlen = 0 ;
	char		hbuf[HDRBUFLEN+1] ;
	if (op == NULL) return SR_FAULT ; /* LINT */
	if ((rs = bvihdr(hdrp,0,hbuf,hlen)) >= 0) {
	    rs = filer_writefill(hfp,hbuf,rs) ;
	    wlen += rs ;
	}
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (bvimk_mkidxwrhdr) */


static int bvimk_mkidxwrverses(bvimk *op,bvihdr *hdrp,filer *hfp,int off)
{
	struct bventry	*bvep ;
	uint		a[4] ;
	cint	sz = (4 * szof(uint)) ;
	int		rs = SR_OK ;
	int		wlen = 0 ;
	int		n = 0 ;
	int		i ;
	hdrp->vioff = off ;
	for (i = 0 ; vecobj_get(&op->verses,i,&bvep) >= 0 ; i += 1) {
	    if (bvep != NULL) {
	        a[0] = bvep->voff ;
	        a[1] = bvep->vlen ;
	        a[2] = bvep->li ;
	        a[3] = bvep->citation ;
	        n += 1 ;
	        rs = filer_write(hfp,a,sz) ;
	        wlen += rs ;
	    }
	    if (rs < 0) break ;
	} /* end for */
	hdrp->vilen = n ;
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (bvimk_mkidxwrverses) */

static int bvimk_mkidxwrlines(bvimk *op,bvihdr *hdrp,filer *hfp,int off) noex {
	struct blentry	*blep ;
	uint		a[4] ;
	cint	sz = (2 * szof(uint)) ;
	int		rs = SR_OK ;
	int		wlen = 0 ;
	int		n = 0 ;
	int		i ;
	hdrp->vloff = off ;
	for (i = 0 ; vecobj_get(&op->lines,i,&blep) >= 0 ; i += 1) {
	    if (blep != NULL) {
	        a[0] = blep->loff ;
	        a[1] = blep->llen ;
	        n += 1 ;
	        rs = filer_write(hfp,a,sz) ;
	        wlen += rs ;
	    }
	    if (rs < 0) break ;
	} /* end for */
	hdrp->vllen = n ;
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (bvimk_mkidxwrlines) */

static int bvimk_nidxopen(bvimk *op) noex {
	int		rs ;
	int		fd = -1 ;
	int		of = (O_CREAT|O_WRONLY) ;
	cmode		om = op->om ;
	if (op->nidxfname == NULL) {
	    cint	type = (op->fl.ofcreat && (! op->fl.ofexcl)) ;
	    cchar	*dbn = op->dbname ;
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
/* end subroutine (bvimk_nidxopen) */

static int bvimk_nidxclose(bvimk *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (op->nfd >= 0) {
	    rs1 = u_close(op->nfd) ;
	    if (rs >= 0) rs = rs1 ;
	    op->nfd = -1 ;
	}
	return rs ;
}
/* end subroutine (bvimk_nidxclose) */

static int bvimk_renamefiles(bvimk *op) noex {
	int		rs ;
	cchar		*suf = FSUF_IDX ;
	cchar		*end = ENDIANSTR ;
	char		idxfname[MAXPATHLEN + 1] ;

	if ((rs = mkfnamesuf2(idxfname,op->dbname,suf,end)) >= 0) {
	    if ((rs = u_rename(op->nidxfname,idxfname)) >= 0) {
	        op->nidxfname[0] = '\0' ;
	    } else {
	        u_unlink(op->nidxfname) ;
	        op->nidxfname[0] = '\0' ;
	    } /* end if (rename) */
	} /* end if (mkfnamesuf) */

	return rs ;
}
/* end subroutine (bvimk_renamefiles) */


static int mkcitation(uint *cip,bvimk_v *bvp)
{
	uint		ci = 0 ;
	uint		nlines = 0 ;
	if (bvp->lines != NULL) {
	    nlines = bvp->nlines ;
	}
	ci |= (nlines & UCHAR_MAX) ;
	ci = (ci << 8) ;
	ci |= (bvp->b & UCHAR_MAX) ;
	ci = (ci << 8) ;
	ci |= (bvp->c & UCHAR_MAX) ;
	ci = (ci << 8) ;
	ci |= (bvp->v & UCHAR_MAX) ;
	*cip = ci ;
	return SR_OK ;
}
/* end subroutine (mkcitation) */


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

static int vvecmp(cvoid *v1p,cvoid *v2p) noex {
	struct bventry	**e1pp = (struct bventry **) v1p ;
	struct bventry	**e2pp = (struct bventry **) v2p ;
	int		rc = 0 ;
	if (*e1pp != NULL) {
	    if (*e2pp != NULL) {
	        uint	vc1 = (*e1pp)->citation & 0x00FFFFFF ;
	        uint	vc2 = (*e2pp)->citation & 0x00FFFFFF ;
	        rc = (vc1 - vc2) ;
	    } else {
	        rc = -1 ;
	    }
	} else {
	    rc = 1 ;
	}
	return rc ;
}
/* end subroutine (vvecmp) */


