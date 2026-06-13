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

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>		/* POSIX */
#include	<sys/param.h>		/* POSIX */
#include	<sys/stat.h>		/* POSIX */
#include	<unistd.h>		/* POSIX */
#include	<fcntl.h>		/* POSIX */
#include	<ctime>			/* CSTD */
#include	<climits>		/* CSTD |UCHAR_MAX| */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstring>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<endian.h>		/* LIBU */
#include	<estrings.h>		/* LIBU */
#include	<ucmem.h>		/* LIBUC */
#include	<ucsysmisc.h>		/* LIBUC */
#include	<ucsysconf.h>		/* LIBUC */
#include	<ucsysauxinfo.h>	/* LIBUC */
#include	<ucopen.h>		/* LIBUC */
#include	<ucdesc.h>		/* LIBUC */
#include	<ucfileop.h>		/* LIBUC */
#include	<bufsizeget.h>		/* LIBUC */
#include	<vecobj.h>		/* LIBUC */
#include	<opentmp.h>		/* LIBUC */
#include	<filer.h>		/* LIBUC */
#include	<mktmp.h>		/* LIBUC */
#include	<mkfname.h>		/* LIBUC */
#include	<char.h>		/* LIBUC */
#include	<ischarx.h>		/* LIBUC */
#include	<isnot.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */

#include	"bvimk.h"
#include	"bvihdr.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |memclear(3u)| */

/* local defines */

#define	BVIMK_NENTS	(19 * 1024)
#define	BVIMK_NSKIP	5
#define	HDRBUFLEN	(szof(bvihdr) + 128)
#define	BUFLEN		(szof(bvihdr) + 128)

#define	FSUF_IDX	"bvi"

#define	TO_OLDFILE	(5 * 60)

#define	MODP2(v,n)	((v) & ((n) - 1))


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

namespace {
    struct vars {
	int	maxpathlen ;
	operator int () noex ;
    } ; /* end struct (vars) */
} /* end namespace */


/* forward references */

template<typename ... Args>
local int bvimk_ctor(bvimk *op,Args ... args) noex {
	BVIMK		*hop = op ;
	cnullptr	np{} ;
	cnothrow	nt{} ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    memclear(hop) ;
	    rs = SR_NOMEM ;
	    if ((op->vlp = new(nt) vecobj) != np) ylikely {
                if ((op->llp = new(nt) vecobj) != np) ylikely {
                    rs = SR_OK ;
                } /* end if (new-vecobj) */
                if (rs < 0) {
                    delete op->vlp ;
                    op->vlp = nullptr ;
                } /* end if (error) */
	    } /* end if (new-vecobj) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (bvimk_ctor) */

local int bvimk_dtor(bvimk *op) noex {
	int		rs = SR_FAULT ;
	if (op) ylikely {
	    rs = SR_OK ;
            if (op->llp) ylikely {
                delete op->llp ;
                op->llp = nullptr ;
            }
	    if (op->vlp) ylikely {
		delete op->vlp ;
		op->vlp = nullptr ;
	    }
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (bvimk_dtor) */

template<typename ... Args>
local inline int bvimk_magic(bvimk *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    rs = (op->magval == BVIMK_MAGIC) ? SR_OK : SR_NOTOPEN ;
	} /* end if */
	return rs ;
} /* end subroutine (bvimk_magic) */

local int	bvimk_filesbegin(bvimk *) noex ;
local int	bvimk_filesbeginc(bvimk *) noex ;
local int	bvimk_filesbeginwait(bvimk *) noex ;
local int	bvimk_filesbegincreate(bvimk *,cchar *,int,mode_t) noex ;
local int	bvimk_filesend(bvimk *) noex ;
local int	bvimk_listbegin(bvimk *,int) noex ;
local int	bvimk_listend(bvimk *) noex ;
local int	bvimk_mkidx(bvimk *) noex ;
local int	bvimk_mkidxwrmain(bvimk *,bvihdr *) noex ;
local int	bvimk_mkidxwrhdr(bvimk *,bvihdr *,filer *) noex ;
local int	bvimk_mkidxwrverses(bvimk *,bvihdr *,filer *,int) noex ;
local int	bvimk_mkidxwrlines(bvimk *,bvihdr *,filer *,int) noex ;
local int	bvimk_nidxopen(bvimk *) noex ;
local int	bvimk_nidxclose(bvimk *) noex ;
local int	bvimk_renamefiles(bvimk *) noex ;

local int	mkcitation(uint *,bvimk_v *) noex ;
local int	mknewfname(char *,int,cchar *,cchar *) noex ;
local int	unlinkstale(cchar *,int) noex ;

local int	ventcmp(cvoid **,cvoid **) noex ;


/* local variables */


/* exported variables */

const bvimk_obj		bvimk_modinfo = {
	"bvimk",
	szof(bvimk),
	0
} ; /* end initialization */

static vars		var ;


/* exported variables */


/* exported subroutines */

int bvimk_open(bvimk *op,cchar *dbn,int of,mode_t om) noex {
	int		rs ;
	int		c = 0 ; /* return-value */
	if ((rs = bvimk_ctor(op,dbn)) >= 0) {
	    rs = SR_INVALID ;
	    if (dbn[0]) {
		if (static cint rsv = var ; (rs = rsv) >= 0) {
	            cint	n = BVIMK_NENTS ;
	            memclear(op) ;
	            op->om = (om|0600) ;
	            op->nfd = -1 ;
	            op->fl.ofcreat	= MKBOOL(of & O_CREAT) ;
	            op->fl.ofexcl	= MKBOOL(of & O_EXCL) ;
		    if (cchar *cp ; (rs = mem.strw(dbn,-1,&cp)) >= 0) {
	                op->dbname = cp ;
	                if ((rs = bvimk_filesbegin(op)) >= 0) {
	                    c = rs ;
	                    if ((rs = bvimk_listbegin(op,n)) >= 0) {
	                        op->magval = BVIMK_MAGIC ;
	                    }
	                    if (rs < 0) {
	                        bvimk_filesend(op) ;
		            } /* end if (error) */
	                } /* end if (nvimk) */
	                if (rs < 0) {
	                    void *vp = voidp(op->dbname) ;
	                    mem.free(vp) ;
	                    op->dbname = nullptr ;
	                } /* end if (error) */
	            } /* end if (memory-acquire) */
		} /* end if (vars) */
	    } /* end if (valid) */
	    if (rs < 0) {
		bvimk_dtor(op) ;
	    } /* end if (error) */
	} /* end if (bvimk_ctor) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (bvimk_open) */

int bvimk_close(bvimk *op) noex {
	int		rs ;
	int		rs1 ;
	int		nv = 0 ; /* return-value */
	if ((rs = bvimk_magic(op)) >= 0) {
	    bool	f_go = false ;
	    f_go = (! op->fl.abort) ;
	    if (op->fl.notsorted) {
	        vecobj_sort(op->vlp,ventcmp) ;
	    }
	    nv = op->nverses ;
	    if (nv > 0) {
	        rs1 = bvimk_mkidx(op) ;
	        if (rs >= 0) rs = rs1 ;
	        f_go = f_go && (rs1 >= 0) ;
	    }
	    if (op->nfd >= 0) {
	        rs1 = u_close(op->nfd) ;
	        if (rs >= 0) rs = rs1 ;
	        op->nfd = -1 ;
	    }
	    {
	        rs1 = bvimk_listend(op) ;
	        if (rs >= 0) rs = rs1 ;
	        f_go = f_go && (rs1 >= 0) ;
	    }
	    if ((rs >= 0) && (nv > 0) && f_go) {
	        rs1 = bvimk_renamefiles(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = bvimk_filesend(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    if (op->dbname) {
		void *vp = voidp(op->dbname) ;
	        rs1 = mem.free(vp) ;
	        if (rs >= 0) rs = rs1 ;
	        op->dbname = nullptr ;
	    } /* end if (memory-release) */
	    {
	        rs1 = bvimk_dtor(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    op->magval = 0 ;
	} /* end if (bvimk_magic) */
	return (rs >= 0) ? nv : rs ;
}
/* end subroutine (bvimk_close) */

int bvimk_add(bvimk *op,bvimk_v *bvp) noex {
	int		rs = SR_OK ;
	if ((rs = bvimk_magic(op,bvp)) >= 0) {
	    bventry	bve{} ;
	    blentry	ble{} ;
	    uint	li = UINT_MAX ;
	    if (bvp->lines && (bvp->nlines > 0)) {
	        for (int i = 0 ; i < bvp->nlines ; i += 1) {
	            ble.loff = bvp->lines[i].loff ;
	            ble.llen = bvp->lines[i].llen ;
	            rs = vecobj_add(op->llp,&ble) ;
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
	            op->fl.notsorted = true ;
	        }
	        op->pcitation = citcmpval ;
	        if ((rs = vecobj_add(op->vlp,&bve)) >= 0) {
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
	} /* end if (bvimk_magic) */
	return rs ;
} /* end subroutine (bvimk_add) */

int bvimk_abort(bvimk *op,int f) noex {
    	int		rs ;
	if ((rs = bvimk_magic(op)) >= 0) {
	    op->fl.abort = !!f ;
	} /* end if (bvimk_magic) */
	return rs ;
} /* end subroutine (bvimk_abort) */

int bvimk_getinfo(bvimk *op,BVIMK_INFO *bip) noex {
	int		rs ;
	int		nv = 0 ; /* return-value */
	if ((rs = bvimk_magic(op)) >= 0) {
	    nv = op->nverses ;
	    if (bip) {
		memclear(bip) ;
	        bip->maxbook = op->maxbook ;
	        bip->maxchapter = op->maxchapter ;
	        bip->maxverse = op->maxverse ;
	        bip->nverses = op->nverses ;
	        bip->nzverses = op->nzverses ;
	    } /* end if (non-null) */
	} /* end if (bvimk_magic) */
	return (rs >= 0) ? nv : rs ;
} /* end subroutine (bvimk_getinfo) */


/* private subroutines */

local int bvimk_filesbegin(bvimk *op) noex {
	int		rs = SR_OK ;
	int		c = 0 ;
	if (op->fl.ofcreat) {
	    rs = bvimk_filesbeginc(op) ;
	} else {
	    rs = bvimk_filesbeginwait(op) ;
	    c = rs ;
	}
	return (rs >= 0) ? c : rs ;
} /* end subroutine (bvimk_filesbegin) */

local int bvimk_filesbeginc(bvimk *op) noex {
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
	        if ((rs = mktmpfile(rbuf,tbuf,om)) >= 0) {
	            op->fl.created = true ;
	            tfn = rbuf ;
	        } /* end if (mktmpfile) */
	    }
	    if (rs >= 0) {
	        int	of = O_CREAT ;
	        if (op->fl.ofexcl) of |= O_EXCL ;
	        rs = bvimk_filesbegincreate(op,tfn,of,om) ;
		if ((rs < 0) && type) {
		    uc_unlink(rbuf) ;
		} /* end if (error) */
	    } /* end if (ok) */
	} /* end if (mknewfname) */
	return rs ;
} /* end subroutine (bvimk_filesbeginc) */

local int bvimk_filesbeginwait(bvimk *op) noex {
	int		rs ;
	int		c = 0 ;
	cchar		*dbn = op->dbname ;
	cchar		*suf = FSUF_IDX	 ;
	char		tbuf[MAXPATHLEN+1] ;
	if ((rs = mknewfname(tbuf,false,dbn,suf)) >= 0) {
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
	        op->fl.ofcreat = false ;
	        c = 0 ;
	        rs = bvimk_filesbeginc(op) ;
	    } /* end if (not-found) */
	} /* end if (mknewfname) */
	return (rs >= 0) ? c : rs ;
} /* end subroutine (bvimk_filesbeginwait) */

local int bvimk_filesbegincreate(bvimk *op,cchar *tfn,int of,mode_t om) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = uc_open(tfn,of,om)) >= 0) {
	    cint	fd = rs ;
	    op->fl.created = true ;
	    if (cchar *cp ; (rs = mem.strw(tfn,-1,&cp)) >= 0) {
	        op->nidxfname = charp(cp) ;
	    } /* end if (memory-acquire) */
	    rs1 = u_close(fd) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (create) */
	return rs ;
} /* end subroutine (bvimk_filesbegincreate) */

local int bvimk_filesend(bvimk *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (op->nidxfname) {
	    if (op->fl.created && (op->nidxfname[0] != '\0')) {
	        u_unlink(op->nidxfname) ;
	    }
	    {
	        void *vp = voidp(op->nidxfname) ;
	        rs1 = mem.free(vp) ;
	        if (rs >= 0) rs = rs1 ;
	        op->nidxfname = nullptr ;
	    } /* end if (memory-release) */
	}
	if (op->idname) {
	    void *vp = voidp(op->idname) ;
	    rs1 = mem.free(vp) ;
	    if (rs >= 0) rs = rs1 ;
	    op->idname = nullptr ;
	} /* end if (memory-release) */
	return rs ;
} /* end subroutine (bvimk_filesend) */

local int bvimk_listbegin(bvimk *op,int n) noex {
	int		rs ;
	int		sz ;
	int		opts = 0 ;
	opts |= VECOBJ_OCOMPACT ;
	opts |= VECOBJ_OORDERED ;
	opts |= VECOBJ_OSTATIONARY ;
	sz = szof(struct bventry) ;
	if ((rs = vecobj_start(op->vlp,sz,n,opts)) >= 0) {
	    rs = vecobj_start(op->llp,sz,(n * 2),opts) ;
	    if (rs < 0) {
	        vecobj_finish(op->vlp) ;
	    } /* end if (error) */
	}

	return rs ;
} /* end subroutine (bvimk_listbegin) */

local int bvimk_listend(bvimk *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	{
	    rs1 = vecobj_finish(op->llp) ;
	    if (rs >= 0) rs = rs1 ;
	}
	{
	    rs1 = vecobj_finish(op->vlp) ;
	    if (rs >= 0) rs = rs1 ;
	}
	return rs ;
} /* end subroutine (bvimk_listend) */

local int bvimk_mkidx(bvimk *op) noex {
	int		rs ;
	int		rs1 ;
	int		wlen = 0 ; /* return-value */
	if ((rs = bvimk_nidxopen(op)) >= 0) {
	    bvihdr	hdr{} ;
	    hdr.vetu[0] = uchar(BVIHDR_VERSION) ;
	    hdr.vetu[1] = uchar(ENDIAN) ;
	    hdr.vetu[2] = 0 ;
	    hdr.vetu[3] = 0 ;
	    hdr.wtime		= (uint) time(nullptr) ;
	    hdr.nverses		= op->nverses ;
	    hdr.nzverses	= op->nzverses ;
	    hdr.maxbook		= op->maxbook ;
	    hdr.maxchapter	= op->maxchapter ;
	    if ((rs = bvimk_mkidxwrmain(op,&hdr)) >= 0) {
	        cint	hlen = HDRBUFLEN ;
	        char	hbuf[HDRBUFLEN+1] ;
	        hdr.fsz = rs ;
	        wlen = rs ;
	        if ((rs = hdr.rd(hbuf,hlen)) >= 0) {
	            cint	bl = rs ;
	            if ((rs = u_writep(op->nfd,hbuf,bl,0L)) >= 0) {
	                cmode	om = op->om ;
	                rs = uc_fminmod(op->nfd,om) ;
	            } /* end if (uc_writep) */
	        } /* end if (bvihdr) */
	    } /* end if (bvimk_mkidxwrmain) */
	    rs1 = bvimk_nidxclose(op) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (bvimk_nidx) */
	return (rs >= 0) ? wlen : rs ;
} /* end subroutine (bvimk_mkidx) */

local int bvimk_mkidxwrmain(bvimk *op,bvihdr *hdrp) noex {
	int		rs ;
	int		rs1 ;
	int		off = 0 ; /* return-value */
	if ((rs = ucpagesize) >= 0) {
	    cint bsz = (rs * 4) ;
	    cint nfd = op->nfd ;
	    if (filer hf ; (rs = hf.start(nfd,0,bsz,0)) >= 0) {
	        if ((rs = bvimk_mkidxwrhdr(op,hdrp,&hf)) >= 0) {
	            off += rs ;
	            if (rs >= 0) {
	                rs = bvimk_mkidxwrverses(op,hdrp,&hf,off) ;
	                off += rs ;
	            } /* end if (ok) */
	            if (rs >= 0) {
	                rs = bvimk_mkidxwrlines(op,hdrp,&hf,off) ;
	                off += rs ;
	            } /* end if (ok) */
	        } /* end if (bvimk_mkidxwrhdr) */
	        rs1 = hf.finish ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (filer) */
	} /* end if (ucpagesize) */
	return (rs >= 0) ? off : rs ;
} /* end subroutine (bvimk_mkidxwrmain) */

local int bvimk_mkidxwrhdr(bvimk *op,bvihdr *hdrp,filer *hfp) noex {
	int		rs = SR_FAULT ;
	int		wlen = 0 ; /* return-value */
	if (op) {
	    cint	hlen = HDRBUFLEN ;
	    char	hbuf[HDRBUFLEN+1] ;
	    if ((rs = hdrp->rd(hbuf,hlen)) >= 0) {
	        rs = filer_writefill(hfp,hbuf,rs) ;
	        wlen += rs ;
	    } /* end if (bvihdr) */
	} /* end if (non-null) */
	return (rs >= 0) ? wlen : rs ;
} /* end subroutine (bvimk_mkidxwrhdr) */

local int bvimk_mkidxwrverses(bvimk *op,bvihdr *hdrp,filer *hfp,int off) noex {
	uint		a[4] ;
	cint		sz = (4 * szof(uint)) ;
	int		rs = SR_OK ;
	int		wlen = 0 ; /* return-value */
	int		n = 0 ;
	hdrp->vioff = off ;
	void *vp ;
	for (int i = 0 ; vecobj_get(op->vlp,i,&vp) >= 0 ; i += 1) {
	    bventry	*bvep = resumelife<bventry>(vp) ;
	    if (vp) {
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
} /* end subroutine (bvimk_mkidxwrverses) */

local int bvimk_mkidxwrlines(bvimk *op,bvihdr *hdrp,filer *hfp,int off) noex {
	uint		a[4] ;
	cint		sz = (2 * szof(uint)) ;
	int		rs = SR_OK ;
	int		wlen = 0 ; /* return-value */
	int		n = 0 ;
	hdrp->vloff = off ;
	void *vp ;
	for (int i = 0 ; vecobj_get(op->llp,i,&vp) >= 0 ; i += 1) {
	    blentry	*blep = resumelife<blentry>(vp) ;
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
} /* end subroutine (bvimk_mkidxwrlines) */

local int bvimk_nidxopen(bvimk *op) noex {
	int		rs ;
	int		rs1 ;
	int		fd = -1 ;/* return-value */
	int		of = (O_CREAT|O_WRONLY) ;
	int		ai = 2 ;
	cmode		om = op->om ;
	if (op->nidxfname == nullptr) {
	    cint	maxpath = var.maxpathlen ;
	    cint	type = (op->fl.ofcreat && (! op->fl.ofexcl)) ;
	    cint	psz = (ai + var.maxpathlen) ;
	    cchar	*dbn = op->dbname ;
	    cchar	*suf = FSUF_IDX ;
	    if (char *a ; (rs = mem.mall(psz,&a)) >= 0) {
	        char	*tbuf = (a + (--ai * (maxpath + 1))) ;
	        if ((rs = mknewfname(tbuf,type,dbn,suf)) >= 0) {
	            cchar	*tfn = tbuf ;
	            char	*rbuf = (a + (--ai * (maxpath + 1))) ;
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
	            } /* end if */
	            if (rs >= 0) {
	                if (cchar *cp ; (rs = mem.strw(tfn,-1,&cp)) >= 0) {
	                    op->nidxfname = charp(cp) ;
	                } /* end if (memory-acquire) */
	            } /* end if (ok) */
	        } /* end if (mknewfname) */
	        rs1 = mem.free(a) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (m-a-f) */
	} else {
	    if (op->fl.ofexcl) of |= O_EXCL ;
	    rs = uc_open(op->nidxfname,of,om) ;
	    op->nfd = rs ;
	    fd = rs ;
	} /* end if */
	return (rs >= 0) ? fd : rs ;
} /* end subroutine (bvimk_nidxopen) */

local int bvimk_nidxclose(bvimk *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (op->nfd >= 0) {
	    rs1 = u_close(op->nfd) ;
	    if (rs >= 0) rs = rs1 ;
	    op->nfd = -1 ;
	} /* end if */
	return rs ;
} /* end subroutine (bvimk_nidxclose) */

local int bvimk_renamefiles(bvimk *op) noex {
	int		rs ;
	int		rs1 ;
	cchar		*suf = FSUF_IDX ;
	cchar		*end = ENDIANSTR ;
	if (char *idxfname ; (rs = mem.mp(&idxfname)) >= 0) {
	    if ((rs = mkfnamesuf2(idxfname,op->dbname,suf,end)) >= 0) {
	        if ((rs = u_rename(op->nidxfname,idxfname)) >= 0) {
	            op->nidxfname[0] = '\0' ;
	        } else {
	            u_unlink(op->nidxfname) ;
	            op->nidxfname[0] = '\0' ;
	        } /* end if (rename) */
	    } /* end if (mkfnamesuf) */
	    rs1 = mem.free(idxfname) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return rs ;
} /* end subroutine (bvimk_renamefiles) */

local int mkcitation(uint *cip,bvimk_v *bvp) noex {
	uint		ci = 0 ;
	uint		nlines = 0 ;
	if (bvp->lines) {
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
} /* end subroutine (mkcitation) */

local int mknewfname(char *tbuf,int type,cchar *dbn,cchar *suf) noex {
	cchar		*end = ENDIANSTR ;
	cchar		*fin = (type) ? "xXXXX" : "n" ;
	return mkfnamesuf3(tbuf,dbn,suf,end,fin) ;
} /* end subroutine (mknewfname) */

local int unlinkstale(cchar *fn,int to) noex {
	custime		dt = getustime ;
	int		rs ;
	if (ustat sb ; (rs = uc_stat(fn,&sb)) >= 0) {
	    if ((dt - sb.st_mtime) >= to) {
	        uc_unlink(fn) ;
	        rs = 1 ;
	    } else {
	        rs = 0 ;
	    }
	} else if (isNotPresent(rs)) {
	    rs = SR_OK ;
	}
	return rs ;
} /* end subroutine (unlinkstale) */

local int entcmp(bventry *e1p,bventry *e2p) noex {
	uint	vc1 = e1p->citation & 0x00FFFFFF ;
	uint	vc2 = e2p->citation & 0x00FFFFFF ;
	int	rc = 0 ;
	{
	rc = (vc1 - vc2) ;
	}
	return rc ;
} /* end subroutine (entcmp) */

local int ventcmp(cvoid **v1pp,cvoid **v2pp) noex {
	bventry		**e1pp = (bventry **) v1pp ;
	bventry		**e2pp = (bventry **) v2pp ;
	int		rc = 0 ;
	if (e1pp && e2pp) {
	    bventry	*e1p = *e1pp ;
	    bventry	*e2p = *e2pp ;
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
	    } /* end if */
	} /* end block */
	return rc ;
} /* end subroutine (ventcmp) */

vars::operator int () noex {
    	int		rs ;
	if ((rs = bufsizeget(bufsize_mp)) >= 0) {
	    maxpathlen = rs ;
	} /* end if (bufsizeget) */
    	return rs ;
} /* end if (vars::operator) */


