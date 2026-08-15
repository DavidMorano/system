/* cmimk SUPPORT */
/* charset=ISO8859-1 */
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

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>		/* POSIX® */
#include	<sys/param.h>		/* POSIX® */
#include	<sys/stat.h>		/* POSIX® */
#include	<unistd.h>		/* POSIX® */
#include	<fcntl.h>		/* POSIX® */
#include	<ctime>			/* CSTD */
#include	<climits>		/* CSTD |UINT_MAX| */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstring>		/* CSTD */
#include	<new>			/* C++SYD plaement-new */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<endian.h>		/* LIBU */
#include	<ucmem.h>		/* LIBUC */
#include	<ucopen.h>		/* LIBUC */
#include	<ucdesc.h>		/* LIBUC */
#include	<ucfileop.h>		/* LIBUC */
#include	<sysval.hh>		/* LIBUC */
#include	<bufsizeget.h>		/* LIBUC */
#include	<estrings.h>		/* LIBUC */
#include	<vecobj.h>		/* LIBUC */
#include	<filer.h>		/* LIBUC */
#include	<opentmp.h>		/* LIBUC */
#include	<mktmp.h>		/* LIBUC */
#include	<mkfnamesuf.h>		/* LIBUC */
#include	<char.h>		/* LIBUC */
#include	<isnot.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */

#include	"cmihdr.h"
#include	"cmimk.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |memclear(3u)| */

/* local defines */

#define	CMIMK_NENTS	(19 * 1024)
#define	CMIMK_NSKIP	5

#define	HDRBUFLEN	(szof(cmihdr) + 128)
#define	BUFLEN		(szof(cmihdr) + 128)

#define	FSUF_IDX	"cmi"

#define	TO_OLDFILE	(5 * 60)

#define	BLENTRY		blentry


/* imported namespaces */

using libuc::mem ;			/* variable */


/* local typedefs */


/* local namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

struct cmentry {
	uint		eoff ;
	uint		elen ;
	uint		li ;		/* index-number of first line-entry */
	ushort		nlines ;
	ushort		cn ;		/* command-number */
} ; /* end struct */

struct blentry {
	uint		loff ;
	uint		llen ;
} ; /* end struct */

namespace {
    struct vars {
	int		maxpathlen ;
	operator int () noex ;
    } ; /* end struct (vars) */
} /* end namespace */


/* forward references */

template<typename ... Args>
local int cmimk_ctor(cmimk *op,Args ... args) noex {
    	CMIMK		*hop = op ;
	cnullptr	np{} ;
	cnothrow	nt{} ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    rs = SR_NOMEM ;
	    memclear(hop) ;
	    if ((op->elp = new(nt) vecobj) != np) ylikely {
	        if ((op->llp = new(nt) vecobj) != np) ylikely {
		    rs = SR_OK ;
		} /* end if (new-vecobj) */
		if (rs < 0) {
		    delete op->elp ;
		    op->elp = nullptr ;
		} /* end if (error) */
	    } /* end if (new-vecobj) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (cmimk_ctor) */

local int cmimk_dtor(cmimk *op) noex {
	int		rs = SR_FAULT ;
	if (op) ylikely {
	    rs = SR_OK ;
	    if (op->llp) {
		delete op->llp ;
		op->llp = nullptr ;
	    } /* end if (delete-vecobj) */
	    if (op->elp) {
		delete op->elp ;
		op->elp = nullptr ;
	    } /* end if (delete-vecobj) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (cmimk_dtor) */

template<typename ... Args>
local inline int cmimk_magic(cmimk *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    rs = (op->magval == CMIMK_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
} /* end subroutine (cmimk_magic) */

local int	cmimk_filesbeg(cmimk *) noex ;
local int	cmimk_filesbegc(cmimk *) noex ;
local int	cmimk_filesbegwait(cmimk *) noex ;
local int	cmimk_filesbegcreate(cmimk *,cchar *,int,mode_t) noex ;
local int	cmimk_filesend(cmimk *) noex ;
local int	cmimk_listbegin(cmimk *,int) noex ;
local int	cmimk_listend(cmimk *) noex ;
local int	cmimk_mkidx(cmimk *) noex ;
local int	cmimk_mkidxwrmain(cmimk *,cmihdr *) noex ;
local int	cmimk_mkidxwrhdr(cmimk *,cmihdr *,filer *) noex ;
local int	cmimk_mkidxwrents(cmimk *,cmihdr *,filer *,int) noex ;
local int	cmimk_mkidxwrlines(cmimk *,cmihdr *,filer *,int) noex ;
local int	cmimk_nidxopen(cmimk *) noex ;
local int	cmimk_nidxclose(cmimk *) noex ;
local int	cmimk_renamefiles(cmimk *) noex ;

local int	mknewfname(char *,int,cchar *,cchar *) noex ;
local int	unlinkstale(cchar *,int) noex ;

extern "C" {
    local int	vvecmp(cvoid **,cvoid **) noex ;
}


/* local variables */

static sysval		pagesz		(sysval_ps) ;
static vars		var ;


/* exported variables */

const cmimk_obj		cmimk_modinfo = {
	"cmimk",
	szof(cmimk),
	0
} ; /* end initialization */


/* exported subroutines */

int cmimk_open(cmimk *op,cchar *dbname,int of,mode_t om) noex {
	int		rs ;
	int		c = 0 ;
	if ((rs = cmimk_ctor(op,dbname)) >= 0) {
	    rs = SR_INVALID ;
	    if (dbname[0]) {
		if (static cint rsv = var ; (rs = rsv) >= 0) ylikely {
	            cint		n = CMIMK_NENTS ;
	            op->om		= (om | 0600) ;
	            op->nfd		= -1 ;
	            op->fl.ofcreat	= !!(of & O_CREAT) ;
	            op->fl.ofexcl	= !!(of & O_EXCL) ;
	            if (cchar *cp ; (rs = mem.strw(dbname,-1,&cp)) >= 0) {
	                op->dbname = cp ;
	                if ((rs = cmimk_filesbeg(op)) >= 0) {
	                    c = rs ;
	                    if ((rs = cmimk_listbegin(op,n)) >= 0) {
	                        op->magval = CMIMK_MAGIC ;
	                    }
	                    if (rs < 0) {
	                        cmimk_filesend(op) ;
		            } /* end if (error) */
	                } /* end if (nvimk) */
	                if (rs < 0) {
	                    voidp vp = voidp(op->dbname) ;
	                    mem.free(vp) ;
	                    op->dbname = nullptr ;
	                } /* end if (error) */
	            } /* end if (memory-acquire) */
		} /* end if (vars) */
	    } /* end if (valid) */
	    if (rs < 0) {
		cmimk_dtor(op) ;
	    } /* end if (error) */
	} /* end if (cmimk_ctor) */
	return (rs >= 0) ? c : rs ;
} /* end subroutine (cmimk_open) */

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
	    if (op->dbname) {
	        void *vp = voidp(op->dbname) ;
	        rs1 = mem.free(vp) ;
	        if (rs >= 0) rs = rs1 ;
	        op->dbname = nullptr ;
	    } /* end if (memory-release) */
	    {
	        rs1 = cmimk_dtor(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    op->magval = 0 ;
	} /* end if (magic) */
	return (rs >= 0) ? nents : rs ;
} /* end subroutine (cmimk_close) */

int cmimk_setdb(cmimk *op,size_t size_db,time_t ti_db) noex {
    	int		rs ;
	if ((rs = cmimk_magic(op)) >= 0) {
	    op->size_db = size_db ;
	    op->ti_db = ti_db ;
	} /* end if (magic) */
	return rs ;
} /* end subroutine (cmimk_setdb) */

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
} /* end subroutine (cmimk_add) */

int cmimk_abort(cmimk *op,int f) noex {
    	int		rs ;
	if ((rs = cmimk_magic(op)) >= 0) {
	    op->fl.abort = !!f ;
	}
	return rs ;
} /* end subroutine (cmimk_abort) */

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
} /* end subroutine (cmimk_getinfo) */


/* private subroutines */

local int cmimk_filesbeg(cmimk *op) noex {
	int		rs = SR_OK ;
	int		c = 0 ;
	if (op->fl.ofcreat) {
	    rs = cmimk_filesbegc(op) ;
	} else {
	    rs = cmimk_filesbegwait(op) ;
	    c = rs ;
	}
	return (rs >= 0) ? c : rs ;
} /* end subroutine (cmimk_filesbeg) */

local int cmimk_filesbegc(cmimk *op) noex {
	cint		type = (op->fl.ofcreat && (! op->fl.ofexcl)) ;
	cint		maxpath = var.maxpathlen ;
	cint		sz = (2 * (var.maxpathlen + 1)) ;
	int		rs ;
	int		rs1 ;
	int		ai = 2 ; /* double allocation */
	cchar		*dbn = op->dbname ;
	cchar		*suf = FSUF_IDX	 ;
	if (char *a ; (rs = mem.mall(sz,&a)) >= 0) {
	    char	*tbuf = (a + (--ai * (maxpath + 1))) ;
	    if ((rs = mknewfname(tbuf,type,dbn,suf)) >= 0) {
	        cchar	*tfn = tbuf ;
	        cmode	om = op->om ;
	        char	*rbuf = (a + (--ai * (maxpath + 1))) ;
	        if (type) {
	            if ((rs = mktmpfile(rbuf,tbuf,om)) >= 0) {
	                op->fl.created = true ;
	                tfn = rbuf ;
	            }
	        }
	        if (rs >= 0) {
	            int	of = O_CREAT ;
	            if (op->fl.ofexcl) of |= O_EXCL ;
	            rs = cmimk_filesbegcreate(op,tfn,of,om) ;
		    if ((rs < 0) && type) {
		        uc_unlink(rbuf) ;
		    } /* end if (error) */
	        } /* end if (ok) */
	    } /* end if (mknewfname) */
	    rs1 = mem.free(a) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return rs ;
} /* end subroutine (cmimk_filesbegc) */

local int cmimk_filesbegwait(cmimk *op) noex {
	int		rs ;
	int		rs1 ;
	int		c = 0 ; /* return-value */
	cchar		*dbn = op->dbname ;
	cchar		*suf = FSUF_IDX	 ;
	if (char *tbuf ; (rs = mem.mp(&tbuf)) >= 0) {
	    if ((rs = mknewfname(tbuf,false,dbn,suf)) >= 0) {
	        cint	to_stale = CMIMK_INTSTALE ;
	        cint	nrs = SR_EXISTS ;
	        cint	of = (O_CREAT|O_WRONLY|O_EXCL) ;
	        int		to = CMIMK_INTOPEN ;
	        cmode	om = op->om ;
	        while ((rs = cmimk_filesbegcreate(op,tbuf,of,om)) == nrs) {
	            c = 1 ;
	            sleep(1) ;
	            unlinkstale(tbuf,to_stale) ;
	            if (to-- == 0) break ;
	        } /* end while (db exists) */
	        if (rs == nrs) {
	            op->fl.ofcreat = false ;
	            c = 0 ;
	            rs = cmimk_filesbegc(op) ;
	        } /* end if */
	    } /* end if (mknewfname) */
	    rs1 = mem.free(tbuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return (rs >= 0) ? c : rs ;
} /* end subroutine (cmimk_filesbegwait) */

local int cmimk_filesbegcreate(cmimk *op,cchar *tfn,int of,mode_t om) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = uc_open(tfn,of,om)) >= 0) {
	    cint	fd = rs ;
	    op->fl.created = true ;
	    if (cchar *cp ; (rs = mem.strw(tfn,-1,&cp)) >= 0) {
	        op->nidxfname = charp(cp) ;
	    } /* end if (memory-acquire) */
	    rs1 = uc_close(fd) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (create) */
	return rs ;
} /* end subroutine (cmimk_filesbegcreate) */

local int cmimk_filesend(cmimk *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (op->nidxfname) {
	    if (op->fl.created && (op->nidxfname[0] != '\0')) {
	        u_unlink(op->nidxfname) ;
	    }
	    {
	        voidp vp = voidp(op->nidxfname) ;
	        rs1 = mem.free(vp) ;
	        if (rs >= 0) rs = rs1 ;
	        op->nidxfname = nullptr ;
	    } /* end if (memory-release) */
	}
	if (op->idname) {
	    voidp vp = voidp(op->idname) ;
	    rs1 = mem.free(vp) ;
	    if (rs >= 0) rs = rs1 ;
	    op->idname = nullptr ;
	} /* end if (memory-release) */
	return rs ;
} /* end subroutine (cmimk_filesend) */

local int cmimk_listbegin(cmimk *op,int vn) noex {
	int		rs ;
	int		sz = szof(cmentry) ;
	int		vo = 0 ;
	vo |= vecobjm.compact ;
	vo |= vecobjm.ordered ;
	vo |= vecobjm.stationary ;
	if ((rs = vecobj_start(op->elp,sz,vn,vo)) >= 0) {
	    rs = vecobj_start(op->llp,sz,(vn * 2),vo) ;
	    if (rs < 0) {
	        vecobj_finish(op->elp) ;
	    } /* end if (error) */
	} /* end if (vecobj_start) */
	return rs ;
} /* end subroutine (cmimk_listbegin) */

local int cmimk_listend(cmimk *op) noex {
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
} /* end subroutine (cmimk_listend) */

local int cmimk_mkidx(cmimk *op) noex {
	int		rs ;
	int		rs1 ;
	int		wlen = 0 ;
	if ((rs = cmimk_nidxopen(op)) >= 0) {
	    cmihdr	hdr{} ;
	    hdr.vetu[0] = CMIHDR_VERSION ;
	    hdr.vetu[1] = uchar(ENDIAN) ;
	    hdr.vetu[2] = 0 ;
	    hdr.vetu[3] = 0 ;
	    hdr.dbsz = (uint) op->size_db ;
	    hdr.dbtime = (uint) op->ti_db ;
	    hdr.nents = op->nents ;
	    hdr.maxent = op->maxent ;
	    if ((rs = cmimk_mkidxwrmain(op,&hdr)) >= 0) {
	        cint	hlen = HDRBUFLEN ;
	        char	hbuf[HDRBUFLEN+1] ;
	        hdr.idxtime = (uint) time(nullptr) ;
	        hdr.idxsz = (uint) rs ;
	        wlen = rs ;
	        if ((rs = cmihdr_rd(&hdr,hbuf,hlen)) >= 0) {
	            cint	bl = rs ;
	            if ((rs = u_writep(op->nfd,hbuf,bl,0L)) >= 0) {
	                cmode	om = op->om ;
	                rs = uc_fminmod(op->nfd,om) ;
	            }
	        }
	    } /* end if (cmimk_mkidxwrmain) */
	    rs1 = cmimk_nidxclose(op) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (cmimk_nidx) */
	return (rs >= 0) ? wlen : rs ;
} /* end subroutine (cmimk_mkidx) */

local int cmimk_mkidxwrmain(cmimk *op,cmihdr *hdrp) noex {
	cint		nfd = op->nfd ;
	cint		ps = pagesz ;
	int		bsz ;
	int		rs ;
	int		rs1 ;
	int		off = 0 ; /* return-value */
	bsz = (ps * 4) ;
	if (filer hf ; (rs = hf.start(nfd,0z,bsz,0)) >= 0) {
	    if ((rs = cmimk_mkidxwrhdr(op,hdrp,&hf)) >= 0) {
	        off += rs ;
	        if (rs >= 0) {
	            rs = cmimk_mkidxwrents(op,hdrp,&hf,off) ;
	            off += rs ;
	        }
	        if (rs >= 0) {
	            rs = cmimk_mkidxwrlines(op,hdrp,&hf,off) ;
	            off += rs ;
	        }
	    } /* end if (cmimk_mkidxwrhdr) */
	    rs1 = hf.finish ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (filer) */
	return (rs >= 0) ? off : rs ;
} /* end subroutine (cmimk_mkidxwrmain) */

local int cmimk_mkidxwrhdr(cmimk *op,cmihdr *hdrp,filer *hfp) noex {
	int		rs = SR_FAULT ;
	int		wlen = 0 ;
	if (op) ylikely {
	    cint	hlen = HDRBUFLEN ;
	    char	hbuf[HDRBUFLEN+1] ;
	    if ((rs = cmihdr_rd(hdrp,hbuf,hlen)) >= 0) {
	        rs = hfp->writefill(hbuf,rs) ;
	        wlen += rs ;
	    }
	} /* end if (non-null) */
	return (rs >= 0) ? wlen : rs ;
} /* end subroutine (cmimk_mkidxwrhdr) */

local int cmimk_mkidxwrents(cmimk *op,cmihdr *hdrp,filer *hfp,int off) noex {
	uint		a[4] ;
	cint		sz = (4 * szof(uint)) ;
	int		rs = SR_OK ;
	int		wlen = 0 ; /* return-value */
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
	        rs = hfp->write(a,sz) ;
	        wlen += rs ;
	    }
	    if (rs < 0) break ;
	} /* end for */
	hdrp->vilen = n ;
	return (rs >= 0) ? wlen : rs ;
} /* end subroutine (cmimk_mkidxwrents) */

local int cmimk_mkidxwrlines(cmimk *op,cmihdr *hdrp,filer *hfp,int off) noex {
	uint		a[4] ;
	cint		sz = (2 * szof(uint)) ;
	int		rs = SR_OK ;
	int		wlen = 0 ; /* return-value */
	int		n = 0 ;
	hdrp->vloff = off ;
	void	*vp{} ;
	for (int i = 0 ; vecobj_get(op->llp,i,&vp) >= 0 ; i += 1) {
	    BLENTRY	*blep = (BLENTRY *) vp ;
	    if (vp) {
	        a[0] = blep->loff ;
	        a[1] = blep->llen ;
	        n += 1 ;
	        rs = hfp->write(a,sz) ;
	        wlen += rs ;
	    }
	    if (rs < 0) break ;
	} /* end for */
	hdrp->vllen = n ;
	return (rs >= 0) ? wlen : rs ;
} /* end subroutine (cmimk_mkidxwrlines) */

local int cmimk_nidxopen(cmimk *op) noex {
    	cint		maxpath = var.maxpathlen ;
	int		rs ;
	int		rs1 ;
	int		fd = -1 ;
	int		of = (O_CREAT|O_WRONLY) ;
	cmode		om = op->om ;
	if (op->nidxfname == nullptr) {
	    cint	sz = (2 * (var.maxpathlen + 1)) ;
	    cint	type = (op->fl.ofcreat && (! op->fl.ofexcl)) ;
	    int		ai = 2 ; /* double allocation */
	    cchar	*dbn = op->dbname ;
	    cchar	*suf = FSUF_IDX ;
	    if (char *a ; (rs = mem.mall(sz,&a)) >= 0) {
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
} /* end subroutine (cmimk_nidxopen) */

local int cmimk_nidxclose(cmimk *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (op->nfd >= 0) {
	    rs1 = u_close(op->nfd) ;
	    if (rs >= 0) rs = rs1 ;
	    op->nfd = -1 ;
	}
	return rs ;
} /* end subroutine (cmimk_nidxclose) */

local int cmimk_renamefiles(cmimk *op) noex {
	int		rs ;
	int		rs1 ;
	cchar		*suf = FSUF_IDX ;
	cchar		*end = ENDIANSTR ;
	if (char *ibuf ; (rs = mem.mp(&ibuf)) >= 0) {
	    if ((rs = mkfnamesuf(ibuf,op->dbname,suf,end)) >= 0) {
	        if ((rs = u_rename(op->nidxfname,ibuf)) >= 0) {
	            op->nidxfname[0] = '\0' ;
	        } else {
	            u_unlink(op->nidxfname) ;
	            op->nidxfname[0] = '\0' ;
	        } /* end if (rename) */
	    } /* end if (mkfnamesuf) */
	    rs1 = mem.free(ibuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return rs ;
} /* end subroutine (cmimk_renamefiles) */

local int mknewfname(char *rbuf,int type,cchar *dbn,cchar *suf) noex {
	cchar		*s_end = ENDIANSTR ;
	cchar		*s_fin = (type) ? "xXXXX" : "n" ;
	return mkfnamesuf(rbuf,dbn,suf,s_end,s_fin) ;
} /* end subroutine (mknewfname) */

local int unlinkstale(cchar *fn,int to) noex {
	custime		dt = getustime ;
	int		rs ;
	if (ustat sb ; (rs = uc_stat(fn,&sb)) >= 0) {
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
} /* end subroutine (unlinkstale) */

vars::operator int () noex {
    	int		rs ;
	if ((rs = pagesz) >= 0) {
	    if ((rs = bufsizeget(bufsize_mp)) >= 0) {
	        maxpathlen = rs ;
	    } /* end if (maxpathlen) */
	} /* end if (pagesz) */
	return rs ;
} /* end method (vars::operator) */

local int entcmp(cmentry *e1p,cmentry *e2p) noex {
	int	c1 = int(e1p->cn) ;
	int	c2 = int(e2p->cn) ;
	return (c1 - c2) ;
} /* end subroutine */

local int vvecmp(cvoid **v1p,cvoid **v2p) noex {
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
} /* end subroutine (vvecmp) */


