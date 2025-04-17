/* cmimk SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* make a CMI database */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	cmimk

	Description:
	This module creates a CMI database file.

	Synopsis:
	int cmimk_open(cmimk *op,cchar *dbname) noex

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
#include	<climits>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>		/* |UINT_MAX| */
#include	<new>			/* |nothrow(3c++)| */
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<usystem.h>
#include	<endian.h>
#include	<estrings.h>
#include	<vecobj.h>
#include	<filer.h>
#include	<opentmp.h>
#include	<mktmp.h>
#include	<mkfnamesuf.h>
#include	<char.h>
#include	<isnot.h>
#include	<localmisc.h>

#include	"cmimk.h"
#include	"cmihdr.h"


/* local defines */

#define	CMIMK_NENTS	(19 * 1024)
#define	CMIMK_NSKIP	5
#define	HDRBUFLEN	(szof(cmihdr) + 128)

#define	BUFLEN		(szof(cmihdr) + 128)

#define	FSUF_IDX	"cmi"

#define	TO_OLDFILE	(5 * 60)

#define	MODP2(v,n)	((v) & ((n) - 1))

#define	BLENTRY		struct blentry


/* local namespaces */


/* local typedefs */

using std::nullptr_t ;			/* type */
using std::min ;			/* subroutine-template */
using std::max ;			/* subroutine-template */
using std::nothrow ;			/* constant */


/* external subroutines */


/* external variables */


/* exported variables */

extern const cmimk_obj	cmimk_modinfo = {
	"cmimk",
	szof(cmimk),
	0
} ;


/* local structures */

struct cmentry {
	uint		eoff ;
	uint		elen ;
	uint		li ;		/* index-number of first line-entry */
	ushort		nlines ;
	ushort		cn ;		/* command-number */
} ;

struct blentry {
	uint		loff ;
	uint		llen ;
} ;


/* forward references */

template<typename ... Args>
static int cmimk_ctor(cmimk *op,Args ... args) noex {
    	CMIMK		*hop = op ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    cnullptr	np{} ;
	    rs = SR_NOMEM ;
	    memclear(hop) ;
	    if ((op->elp = new(nothrow) vecobj) != np) {
	        if ((op->llp = new(nothrow) vecobj) != np) {
		    rs = SR_OK ;
		}
		if (rs < 0) {
		    delete op->elp ;
		    op->elp = nullptr ;
		}
	    } /* end if (new-vecobj) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (cmimk_ctor) */

static int cmimk_dtor(cmimk *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	    if (op->llp) {
		delete op->llp ;
		op->llp = nullptr ;
	    }
	    if (op->elp) {
		delete op->elp ;
		op->elp = nullptr ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (cmimk_dtor) */

template<typename ... Args>
static inline int cmimk_magic(cmimk *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magic == CMIMK_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}
/* end subroutine (cmimk_magic) */

static int	cmimk_filesbegin(cmimk *) noex ;
static int	cmimk_filesbeginc(cmimk *) noex ;
static int	cmimk_filesbeginwait(cmimk *) noex ;
static int	cmimk_filesbegincreate(cmimk *,cchar *,int,mode_t) noex ;
static int	cmimk_filesend(cmimk *) noex ;
static int	cmimk_listbegin(cmimk *,int) noex ;
static int	cmimk_listend(cmimk *) noex ;
static int	cmimk_mkidx(cmimk *) noex ;
static int	cmimk_mkidxwrmain(cmimk *,cmihdr *) noex ;
static int	cmimk_mkidxwrhdr(cmimk *,cmihdr *,filer *) noex ;
static int	cmimk_mkidxwrents(cmimk *,cmihdr *,filer *,int) noex ;
static int	cmimk_mkidxwrlines(cmimk *,cmihdr *,filer *,int) noex ;
static int	cmimk_nidxopen(cmimk *) noex ;
static int	cmimk_nidxclose(cmimk *) noex ;
static int	cmimk_renamefiles(cmimk *) noex ;

static int	mknewfname(char *,int,cchar *,cchar *) noex ;
static int	unlinkstale(cchar *,int) noex ;

extern "C" {
    static int	vvecmp(cvoid **,cvoid **) noex ;
}


/* local variables */


/* exported variables */


/* exported subroutines */

int cmimk_open(cmimk *op,cchar *dbname,int of,mode_t om) noex {
	int		rs ;
	int		c = 0 ;
	if ((rs = cmimk_ctor(op,dbname)) >= 0) {
	    rs = SR_INVALID ;
	    if (dbname[0]) {
	        cint	n = CMIMK_NENTS ;
	        op->om = (om|0600) ;
	        op->nfd = -1 ;
	        op->fl.ofcreat = !!(of & O_CREAT) ;
	        op->fl.ofexcl = !!(of & O_EXCL) ;
	        if (cchar *cp ; (rs = uc_mallocstrw(dbname,-1,&cp)) >= 0) {
	            op->dbname = cp ;
	            if ((rs = cmimk_filesbegin(op)) >= 0) {
	                c = rs ;
	                if ((rs = cmimk_listbegin(op,n)) >= 0) {
	                    op->magic = CMIMK_MAGIC ;
	                }
	                if (rs < 0) {
	                    cmimk_filesend(op) ;
		        }
	            } /* end if (nvimk) */
	            if (rs < 0) {
	                uc_free(op->dbname) ;
	                op->dbname = nullptr ;
	            }
	        } /* end if (m-a) */
	    } /* end if (valid) */
	    if (rs < 0) {
		cmimk_dtor(op) ;
	    }
	} /* end if (cmimk_ctor) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (cmimk_open) */

int cmimk_close(cmimk *op) noex {
	int		rs ;
	int		rs1 ;
	int		nents = 0 ;
	if ((rs = cmimk_magic(op)) >= 0) {
	int		f_go = false ;
	f_go = (! op->fl.abort) ;
	if (op->fl.notsorted) {
	    vecobj_sort(op->elp,vvecmp) ;
	}
	nents = op->nents ;
	if (nents > 0) {
	    rs1 = cmimk_mkidx(op) ;
	    if (rs >= 0) rs = rs1 ;
	    f_go = f_go && (rs1 >= 0) ;
	}
	if (op->nfd >= 0) {
	    rs1 = u_close(op->nfd) ;
	    if (rs >= 0) rs = rs1 ;
	    op->nfd = -1 ;
	}
	{
	rs1 = cmimk_listend(op) ;
	if (rs >= 0) rs = rs1 ;
	f_go = f_go && (rs1 >= 0) ;
	}
	if ((nents > 0) && f_go) {
	    rs1 = cmimk_renamefiles(op) ;
	    if (rs >= 0) rs = rs1 ;
	}
	{
	    rs1 = cmimk_filesend(op) ;
	    if (rs >= 0) rs = rs1 ;
	}
	if (op->dbname != nullptr) {
	    rs1 = uc_free(op->dbname) ;
	    if (rs >= 0) rs = rs1 ;
	    op->dbname = nullptr ;
	}
	op->magic = 0 ;
	} /* end if (magic) */
	return (rs >= 0) ? nents : rs ;
}
/* end subroutine (cmimk_close) */

int cmimk_setdb(cmimk *op,size_t size_db,time_t ti_db) noex {
    	int		rs ;
	if ((rs = cmimk_magic(op)) >= 0) {
	    op->size_db = size_db ;
	    op->ti_db = ti_db ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (cmimk_setdb) */

int cmimk_add(cmimk *op,CMIMK_ENT *bvp) noex {
	int		rs ;
	if ((rs = cmimk_magic(op,bvp)) >= 0) {
	uint		li = UINT_MAX ;
	if ((bvp->lines != nullptr) && (bvp->nlines > 0)) {
	    BLENTRY	ble ;
	    for (int i = 0 ; i < bvp->nlines ; i += 1) {
	        ble.loff = bvp->lines[i].loff ;
	        ble.llen = bvp->lines[i].llen ;
	        rs = vecobj_add(op->llp,&ble) ;
	        if (i == 0) li = rs ;
	        if (rs < 0) break ;
	    } /* end for */
	} /* end if (ok) */
	if (rs >= 0) {
	    cmentry	cme{} ;
	    cme.eoff = bvp->eoff ;
	    cme.elen = bvp->elen ;
	    cme.li = li ;
	    cme.nlines = bvp->nlines ;
	    cme.cn = bvp->cn ;
	    {
		uint	cn = bvp->cn ;
	        if (cn < op->pcn) op->fl.notsorted = true ;
	        op->pcn = cn ;
	    }
	    if ((rs = vecobj_add(op->elp,&cme)) >= 0) {
	        op->nents += 1 ;
	        if (cme.cn > op->maxent) op->maxent = cme.cn ;
	    } /* end if (add) */
	} /* end if (ok) */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (cmimk_add) */

int cmimk_abort(cmimk *op,int f) noex {
    	int		rs ;
	if ((rs = cmimk_magic(op)) >= 0) {
	    op->fl.abort = !!f ;
	}
	return rs ;
}
/* end subroutine (cmimk_abort) */

int cmimk_getinfo(cmimk *op,cmimk_info *bip) noex {
	int		rs ;
	int		nents = 0 ;
	if ((rs = cmimk_magic(op)) >= 0) {
	    nents = op->nents ;
	    if (bip) {
	        bip->maxent = op->maxent ;
	        bip->nents = op->nents ;
	    }
	} /* end if (magic) */
	return (rs >= 0) ? nents : rs ;
}
/* end subroutine (cmimk_getinfo) */


/* private subroutines */

static int cmimk_filesbegin(cmimk *op) noex {
	int		rs = SR_OK ;
	int		c = 0 ;
	if (op->fl.ofcreat) {
	    rs = cmimk_filesbeginc(op) ;
	} else {
	    rs = cmimk_filesbeginwait(op) ;
	    c = rs ;
	}
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (cmimk_filesbegin) */

static int cmimk_filesbeginc(cmimk *op) noex {
	cint	type = (op->fl.ofcreat && (! op->fl.ofexcl)) ;
	int		rs ;
	cchar		*dbn = op->dbname ;
	cchar		*suf = FSUF_IDX	 ;
	char		tbuf[MAXPATHLEN+1] ;
	if ((rs = mknewfname(tbuf,type,dbn,suf)) >= 0) {
	    cchar	*tfn = tbuf ;
	    cmode	om = op->om ;
	    char	rbuf[MAXPATHLEN+1] ;
	    if (type) {
	        if ((rs = mktmpfile(rbuf,tbuf,om)) >= 0) {
	            op->fl.created = true ;
	            tfn = rbuf ;
	        }
	    }
	    if (rs >= 0) {
	        int	of = O_CREAT ;
	        if (op->fl.ofexcl) of |= O_EXCL ;
	        rs = cmimk_filesbegincreate(op,tfn,of,om) ;
		if ((rs < 0) && type) {
		    uc_unlink(rbuf) ;
		}
	    } /* end if (ok) */
	} /* end if (mknewfname) */
	return rs ;
}
/* end subroutine (cmimk_filesbeginc) */

static int cmimk_filesbeginwait(cmimk *op) noex {
	int		rs ;
	int		c = 0 ;
	cchar		*dbn = op->dbname ;
	cchar		*suf = FSUF_IDX	 ;
	char		tbuf[MAXPATHLEN+1] ;
	if ((rs = mknewfname(tbuf,false,dbn,suf)) >= 0) {
	    const mode_t	om = op->om ;
	    cint		to_stale = CMIMK_INTSTALE ;
	    cint		nrs = SR_EXISTS ;
	    cint		of = (O_CREAT|O_WRONLY|O_EXCL) ;
	    int			to = CMIMK_INTOPEN ;
	    while ((rs = cmimk_filesbegincreate(op,tbuf,of,om)) == nrs) {
	        c = 1 ;
	        sleep(1) ;
	        unlinkstale(tbuf,to_stale) ;
	        if (to-- == 0) break ;
	    } /* end while (db exists) */
	    if (rs == nrs) {
	        op->fl.ofcreat = false ;
	        c = 0 ;
	        rs = cmimk_filesbeginc(op) ;
	    }
	} /* end if (mknewfname) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (cmimk_filesbeginwait) */

static int cmimk_filesbegincreate(cmimk *op,cchar *tfn,int of,mode_t om) noex {
	int		rs ;
	if ((rs = uc_open(tfn,of,om)) >= 0) {
	    cint	fd = rs ;
	    cchar	*cp ;
	    op->fl.created = true ;
	    if ((rs = uc_mallocstrw(tfn,-1,&cp)) >= 0) {
	        op->nidxfname = (char *) cp ;
	    }
	    u_close(fd) ;
	} /* end if (create) */

	return rs ;
}
/* end subroutine (cmimk_filesbegincreate) */

static int cmimk_filesend(cmimk *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;

	if (op->nidxfname != nullptr) {
	    if (op->fl.created && (op->nidxfname[0] != '\0')) {
	        u_unlink(op->nidxfname) ;
	    }
	    rs1 = uc_free(op->nidxfname) ;
	    if (rs >= 0) rs = rs1 ;
	    op->nidxfname = nullptr ;
	}

	if (op->idname != nullptr) {
	    rs1 = uc_free(op->idname) ;
	    if (rs >= 0) rs = rs1 ;
	    op->idname = nullptr ;
	}

	return rs ;
}
/* end subroutine (cmimk_filesend) */

static int cmimk_listbegin(cmimk *op,int n) noex {
	int		rs ;
	int		size ;
	int		opts ;

	opts = 0 ;
	opts |= VECOBJ_OCOMPACT ;
	opts |= VECOBJ_OORDERED ;
	opts |= VECOBJ_OSTATIONARY ;
	size = szof(struct cmentry) ;
	if ((rs = vecobj_start(op->elp,size,n,opts)) >= 0) {
	    rs = vecobj_start(op->llp,size,(n * 2),opts) ;
	    if (rs < 0) {
	        vecobj_finish(op->elp) ;
	    }
	}

	return rs ;
}
/* end subroutine (cmimk_listbegin) */

static int cmimk_listend(cmimk *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	{
	    rs1 = vecobj_finish(op->llp) ;
	    if (rs >= 0) rs = rs1 ;
	}
	{
	    rs1 = vecobj_finish(op->elp) ;
	    if (rs >= 0) rs = rs1 ;
	}
	return rs ;
}
/* end subroutine (cmimk_listend) */

static int cmimk_mkidx(cmimk *op) noex {
	int		rs ;
	int		rs1 ;
	int		wlen = 0 ;

	if ((rs = cmimk_nidxopen(op)) >= 0) {
	    cmihdr	hdr{} ;
	    hdr.vetu[0] = CMIHDR_VERSION ;
	    hdr.vetu[1] = uchar(ENDIAN) ;
	    hdr.vetu[2] = 0 ;
	    hdr.vetu[3] = 0 ;
	    hdr.dbsize = (uint) op->size_db ;
	    hdr.dbtime = (uint) op->ti_db ;
	    hdr.nents = op->nents ;
	    hdr.maxent = op->maxent ;

	    if ((rs = cmimk_mkidxwrmain(op,&hdr)) >= 0) {
	        cint	hlen = HDRBUFLEN ;
	        char	hbuf[HDRBUFLEN+1] ;

	        hdr.idxtime = (uint) time(nullptr) ;
	        hdr.idxsize = (uint) rs ;
	        wlen = rs ;
	        if ((rs = cmihdr_rd(&hdr,hbuf,hlen)) >= 0) {
	            cint	bl = rs ;
	            if ((rs = u_pwrite(op->nfd,hbuf,bl,0L)) >= 0) {
	                cmode	om = op->om ;
	                rs = uc_fminmod(op->nfd,om) ;
	            }
	        }

	    } /* end if (cmimk_mkidxwrmain) */

	    rs1 = cmimk_nidxclose(op) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (cmimk_nidx) */

	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (cmimk_mkidx) */

static int cmimk_mkidxwrmain(cmimk *op,cmihdr *hdrp) noex {
	filer		hf, *hfp = &hf ;
	cint	nfd = op->nfd ;
	cint	ps = getpagesize() ;
	int		bsize ;
	int		rs ;
	int		rs1 ;
	int		off = 0 ;
	bsize = (ps * 4) ;
	if ((rs = filer_start(hfp,nfd,0,bsize,0)) >= 0) {
	    if ((rs = cmimk_mkidxwrhdr(op,hdrp,hfp)) >= 0) {
	        off += rs ;
	        if (rs >= 0) {
	            rs = cmimk_mkidxwrents(op,hdrp,hfp,off) ;
	            off += rs ;
	        }
	        if (rs >= 0) {
	            rs = cmimk_mkidxwrlines(op,hdrp,hfp,off) ;
	            off += rs ;
	        }
	    } /* end if (cmimk_mkidxwrhdr) */
	    rs1 = filer_finish(hfp) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (filer) */
	return (rs >= 0) ? off : rs ;
}
/* end subroutine (cmimk_mkidxwrmain) */

static int cmimk_mkidxwrhdr(cmimk *op,cmihdr *hdrp,filer *hfp) noex {
	int		rs = SR_FAULT ;
	int		wlen = 0 ;
	if (op) {
	    cint	hlen = HDRBUFLEN ;
	    char	hbuf[HDRBUFLEN+1] ;
	    if ((rs = cmihdr_rd(hdrp,hbuf,hlen)) >= 0) {
	        rs = filer_writefill(hfp,hbuf,rs) ;
	        wlen += rs ;
	    }
	} /* end if (non-null) */
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (cmimk_mkidxwrhdr) */

static int cmimk_mkidxwrents(cmimk *op,cmihdr *hdrp,filer *hfp,int off) noex {
	uint		a[4] ;
	cint		sz = (4 * szof(uint)) ;
	int		rs = SR_OK ;
	int		wlen = 0 ;
	int		n = 0 ;
	hdrp->vioff = off ;
	void	*vp{} ;
	for (int i = 0 ; vecobj_get(op->elp,i,&vp) >= 0 ; i += 1) {
	    cmentry	*cmep = (cmentry *) vp ;
	    if (vp) {
	        a[0] = cmep->eoff ;
	        a[1] = cmep->elen ;
	        a[2] = cmep->li ;
	        a[3] = ((cmep->nlines << 16) | (cmep->cn & UINT_MAX)) ;
	        n += 1 ;
	        rs = filer_write(hfp,a,sz) ;
	        wlen += rs ;
	    }
	    if (rs < 0) break ;
	} /* end for */
	hdrp->vilen = n ;
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (cmimk_mkidxwrents) */

static int cmimk_mkidxwrlines(cmimk *op,cmihdr *hdrp,filer *hfp,int off) noex {
	uint		a[4] ;
	cint		sz = (2 * szof(uint)) ;
	int		rs = SR_OK ;
	int		wlen = 0 ;
	int		n = 0 ;
	hdrp->vloff = off ;
	void	*vp{} ;
	for (int i = 0 ; vecobj_get(op->llp,i,&vp) >= 0 ; i += 1) {
	    BLENTRY	*blep = (BLENTRY *) vp ;;
	    if (vp) {
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
/* end subroutine (cmimk_mkidxwrlines) */

static int cmimk_nidxopen(cmimk *op) noex {
	int		rs ;
	int		fd = -1 ;
	int		of = (O_CREAT|O_WRONLY) ;
	cmode		om = op->om ;
	if (op->nidxfname == nullptr) {
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
	            if (cchar *cp ; (rs = uc_mallocstrw(tfn,-1,&cp)) >= 0) {
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
/* end subroutine (cmimk_nidxopen) */

static int cmimk_nidxclose(cmimk *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (op->nfd >= 0) {
	    rs1 = u_close(op->nfd) ;
	    if (rs >= 0) rs = rs1 ;
	    op->nfd = -1 ;
	}
	return rs ;
}
/* end subroutine (cmimk_nidxclose) */

static int cmimk_renamefiles(cmimk *op) noex {
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
/* end subroutine (cmimk_renamefiles) */

static int mknewfname(char *tbuf,int type,cchar *dbn,cchar *suf) noex {
	cchar		*end = ENDIANSTR ;
	cchar		*fin = (type) ? "xXXXX" : "n" ;
	return mkfnamesuf3(tbuf,dbn,suf,end,fin) ;
}
/* end subroutine (mknewfname) */

static int unlinkstale(cchar *fn,int to) noex {
	custime		dt = getustime ;
	int		rs ;
	if (USTAT sb ; (rs = uc_stat(fn,&sb)) >= 0) {
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

static int entcmp(cmentry *e1p,cmentry *e2p) noex {
	int	c1 = int(e1p->cn) ;
	int	c2 = int(e2p->cn) ;
	return (c1 - c2) ;
}

static int vvecmp(cvoid **v1p,cvoid **v2p) noex {
	cmentry	**e1pp = (cmentry **) v1p ;
	cmentry	**e2pp = (cmentry **) v2p ;
	int		rc = 0 ;
	{
	    cmentry	*e1p = *e1pp ;
	    cmentry	*e2p = *e2pp ;
	    if (e1p || e2p) {
	        if (e1p) {
	            if (e2p) {
			rc = entcmp(e1p,e2p) ;
	            } else {
	                rc = -1 ;
	            }
	        } else {
	            rc = +1 ;
	        }
	    }
	} /* end block */
	return rc ;
}
/* end subroutine (vvecmp) */


