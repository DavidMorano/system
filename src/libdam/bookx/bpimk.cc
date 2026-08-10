/* bpimk SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* make a BPI database */
/* version %I% last-modified %G% */

#define	CF_DEBUG	0		/* compile-time debugging */

/* revision history:

	= 1998-03-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	vpimk

	Description:
	This module creates a BPI database file.

	Synopsis:
	int bpimk_open(bpimk*op,cchar *dbname,...) noex

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
#include	<climits>		/* CSTD */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<endian.h>		/* LIBU */
#include	<ucmem.h>		/* LIBUC */
#include	<ucsysmisc.h>		/* LIBUC */
#include	<ucsysconf.h>		/* LIBUC */
#include	<ucsysauxinfo.h>	/* LIBUC */
#include	<ucopen.h>		/* LIBUC */
#include	<ucdesc.h>		/* LIBUC */
#include	<ucfileop.h>		/* LIBUC */
#include	<bufsizeget.h>		/* LIBUC */
#include	<estrings.h>		/* LIBUC */
#include	<vecobj.h>		/* LIBUC */
#include	<filer.h>		/* LIBUC */
#include	<opentmp.h>		/* LIBUC */
#include	<mktmp.h>		/* LIBUC */
#include	<char.h>		/* LIBUC */
#include	<isnot.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */
#include	<libdebug.h>		/* LIBDEBUG |DPRINTF(3debug)| */

#include	"bvcitekey.h"
#include	"bpihdr.h"
#include	"bpimk.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |memclear(3u)| */

/* local defines */

#define	BPIMK_NENTRIES	(19 * 1024)
#define	BPIMK_NSKIP	5
#define	HDRBUFLEN	(szof(bpihdr) + 128)

#define	FSUF_IDX	"bpi"

#define	TO_OLDFILE	(5 * 60)

#define	BVENT		bventry

#ifndef	CF_DEBUG
#define	CF_DEBUG	0		/* compile-time debugging */
#endif


/* imported namespaces */

using libuc::mem ;			/* variable */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

struct bventry {
	uint	citation ;		/* (nlines, b, c, v) */
} ; /* end struct (bventry) */

namespace {
    struct vars {
	int	maxpathlen ;
	operator int () noex ;
    } ; /* end struct (vars) */
} /* end namespace */


/* forward references */

template<typename ... Args>
local int bpimk_ctor(bpimk *op,Args ... args) noex {
	BPIMK		*hop = op ;
	cnullptr	np{} ;
	cnothrow	nt{} ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    memclear(hop) ;
	    rs = SR_NOMEM ;
	    if ((op->vlp = new(nt) vecobj) != np) ylikely {
		rs = SR_OK ;
	    } /* end if (new-verobj) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (bpimk_ctor) */

local int bpimk_dtor(bpimk *op) noex {
	int		rs = SR_FAULT ;
	if (op) ylikely {
	    rs = SR_OK ;
	    if (op->vlp) ylikely {
		delete op->vlp ;
		op->vlp = nullptr ;
	    } /* end if (memory-release) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (bpimk_dtor) */

template<typename ... Args>
local inline int bpimk_magic(bpimk *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    rs = (op->magval == BPIMK_MAGIC) ? SR_OK : SR_NOTOPEN ;
	} /* end if */
	return rs ;
} /* end subroutine (bpimk_magic) */

local int	bpimk_filesbegin	(bpimk *) noex ;
local int	bpimk_filesbeginc	(bpimk *) noex ;
local int	bpimk_filesbeginwait	(bpimk *) noex ;
local int	bpimk_filesbegincreate	(bpimk *,cchar *,int,mode_t) noex ;
local int	bpimk_filesend		(bpimk *) noex ;
local int	bpimk_listbegin		(bpimk *,int) noex ;
local int	bpimk_listend		(bpimk *) noex ;
local int	bpimk_mkidx		(bpimk *) noex ;
local int	bpimk_mkidxwrmain	(bpimk *,bpihdr *) noex ;
local int	bpimk_mkidxwrhdr	(bpimk *,bpihdr *,filer *,int) noex ;
local int	bpimk_mkidxwrtab	(bpimk *,bpihdr *,filer *,int) noex ;
local int	bpimk_nidxopen		(bpimk *) noex ;
local int	bpimk_nidxclose		(bpimk *) noex ;
local int	bpimk_renamefiles	(bpimk *) noex ;

local int	mkcitation		(uint *,bpimk_v *) noex ;
local int	mknewfname		(char *,int,cchar *,cchar *) noex ;
local int	unlinkstale		(cchar *,int) noex ;

local int	vvecmp			(cvoid **,cvoid **) noex ;


/* local variables */

constexpr uint		bvcitemask	= bvcitekey_vmask ;
static vars		var ;
static int		maxpathlen	(bufsize_mp) ;
cbool			f_debug		= CF_DEBUG ;


/* exported variables */

const bpimk_obj		bpimk_modinfo = {
	"bpimk",
	szof(bpimk),
	0
} ; /* end initialization */


/* exported subroutines */

int bpimk_open(bpimk *op,ccharp dbn,int of,mode_t om) noex {
	cint		n = BPIMK_NENTRIES ;
	int		rs ;
	if ((rs = bpimk_ctor(op,dbn)) >= 0) ylikely {
	    rs = SR_INVALID ;
	    if (dbn[0]) ylikely {
		if (static cint rsv = var ; (rs = rsv) >= 0) ylikely {
	            op->om = (om | 0600) ;
	            op->nfd = -1 ;
	            op->fl.ofcreat	= MKBOOL(of & O_CREAT) ;
	            op->fl.ofexcl	= MKBOOL(of & O_EXCL) ;
	            if (cchar *cp ; (rs = mem.strw(dbn,-1,&cp)) >= 0) ylikely {
	                op->dbname = cp ;
	                if ((rs = bpimk_filesbegin(op)) >= 0) ylikely {
	                    if ((rs = bpimk_listbegin(op,n)) >= 0) ylikely {
	                        op->magval = BPIMK_MAGIC ;
	                    } /* end if */
	                    if (rs < 0) {
	                        bpimk_filesend(op) ;
		            } /* end if (error) */
	                } /* end if (files) */
	                if (rs < 0) {
	                    if (op->dbname) {
			        voidp vp = voidp(op->dbname) ;
	                        mem.free(vp) ;
	                        op->dbname = nullptr ;
	                    } /* end if (memory-release) */
	                } /* end if (error) */
	            } /* end if (memory-acquire) */
		} /* end if (vars) */
	    } /* end if (valid) */
	    if (rs < 0) {
	        bpimk_dtor(op) ;
	    } /* end if (error) */
	} /* end if (bpimk_ctor) */
	DEBUGPRINTF("ret rs=%d\n",rs) ;
	return rs ;
} /* end subroutine (bpimk_open) */

int bpimk_close(bpimk *op) noex {
	int		rs ;
	int		rs1 ;
	int		nv = 0 ; /* return-value */
	if ((rs = bpimk_magic(op)) >= 0) ylikely {
	    int		f_go{} ;
	    DEBUGPRINTF("nverses=%u\n",op->nverses) ;
	    f_go = (! op->fl.abort) ;
	    if (vecobj *vlp = op->vlp ; vlp) {
	        if (op->fl.notsorted) {
	            rs1 = vlp->sort(vvecmp) ;
	            if (rs >= 0) rs = rs1 ;
	        }
	    }
	    nv = op->nverses ;
	    if (nv > 0) {
	        rs1 = bpimk_mkidx(op) ;
	        if (rs >= 0) rs = rs1 ;
	        f_go = f_go && (rs1 >= 0) ;
	    }
	    DEBUGPRINTF("bpimk_mkidx() rs=%d\n",rs) ;
	    if (op->nfd >= 0) {
	        rs1 = u_close(op->nfd) ;
	        if (rs >= 0) rs = rs1 ;
	        op->nfd = -1 ;
	    }
	    {
	        rs1 = bpimk_listend(op) ;
	        if (rs >= 0) rs = rs1 ;
	        f_go = f_go && (rs1 >= 0) ;
	    }
	    if ((rs >= 0) && (nv > 0) && f_go) {
	        rs1 = bpimk_renamefiles(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    DEBUGPRINTF("bpimk_renamefiles() rs=%d\n",rs) ;
	    {
	        rs1 = bpimk_filesend(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    if (op->dbname) {
	        voidp vp = voidp(op->dbname) ;
	        rs1 = mem.free(vp) ;
	        if (rs >= 0) rs = rs1 ;
	        op->dbname = nullptr ;
	    } /* end if (memort-release) */
	    {
	        rs1 = bpimk_dtor(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    op->magval = 0 ;
	} /* end if (bpi_magic) */
	return (rs >= 0) ? nv : rs ;
} /* end subroutine (bpimk_close) */

int bpimk_add(bpimk *op,bpimk_v *bvp) noex {
	int		rs ;
	if ((rs = bpimk_magic(op,bvp)) >= 0) ylikely {
	    DEBUGPRINTF("q=%u:%u:%u\n", bvp->b,bvp->c,bvp->v) ;
	    if (bventry bve ; (rs = mkcitation(&bve.citation,bvp)) >= 0) {
	        uint	citcmpval = (bve.citation & bvcitemask) ;
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
	            if (v > op->maxbook)	op->maxbook = v ;
	            v = bvp->c ;
	            if (v > op->maxchap)	op->maxchap = v ;
	            v = bvp->v ;
	            if (v > op->maxvers)	op->maxvers = v ;
	        } /* end if (vecobj_add) */
	    } /* end if (mkcitation) */
	} /* end if (bpi_magic) */
	DEBUGPRINTF("ret=%d\n",rs) ;
	return rs ;
} /* end subroutine (bpimk_add) */

int bpimk_abort(bpimk *op,int f) noex {
    	int		rs ;
	if ((rs = bpimk_magic(op)) >= 0) ylikely {
	    op->fl.abort = !!f ;
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (bpimk_abort) */

int bpimk_getinfo(bpimk *op,bpimk_info *bip) noex {
	int		rs ;
	int		nv = 0 ; /* return-value*/
	if ((rs = bpimk_magic(op,bip)) >= 0) ylikely {
	    nv = op->nverses ;
	    if (bip) {
		memclear(bip) ;
	        bip->maxbook	= op->maxbook ;
	        bip->maxchap	= op->maxchap ;
	        bip->maxvers	= op->maxvers ;
	        bip->nverses	= op->nverses ;
	        bip->nzverses	= op->nzverses ;
	    } /* end if (non-null) */
	} /* end if (bpi_magic) */
	return (rs >= 0) ? nv : rs ;
} /* end subroutine (bpimk_getinfo) */


/* private subroutines */

local int bpimk_filesbegin(bpimk *op) noex {
	int		rs = SR_OK ;
	int		c = 0 ;
	if (op->fl.ofcreat) {
	    rs = bpimk_filesbeginc(op) ;
	} else {
	    rs = bpimk_filesbeginwait(op) ;
	    c = rs ;
	}
	DEBUGPRINTF("ret rs=%d c=%u\n",rs,c) ;
	return (rs >= 0) ? c : rs ;
} /* end subroutine (bpimk_filesbegin) */

local int bpimk_filesbeginc(bpimk *op) noex {
	int		rs ;
	int		rs1 ;
	int		ai = 2 ; /* two path buffers */
	if ((rs = maxpathlen) >= 0) ylikely {
	    cint	sz = (ai * (rs + 1)) ;
	    cint	maxpath = rs ;
	    cint	type = (op->fl.ofcreat && (! op->fl.ofexcl)) ;
	    cchar	*dbn = op->dbname ;
	    cchar	*suf = FSUF_IDX	 ;
	    if (char *a ; (rs = mem.mall(sz,&a)) >= 0) ylikely {
		char	*tbuf = (a + (--ai * (maxpath + 1))) ;
	        if ((rs = mknewfname(tbuf,type,dbn,suf)) >= 0) ylikely {
	            cchar	*tfn = tbuf ;
	            char	*rbuf = (a + (--ai * (maxpath + 1))) ;
	            cmode	om = op->om ;
	            if (type) {
	                if ((rs = mktmpfile(rbuf,tbuf,om)) >= 0) {
	                    op->fl.created = true ;
	                    tfn = rbuf ;
	                } /* end if (mktmpfile) */
	            } /* end if (type) */
	            if (rs >= 0) {
	                int	of = O_CREAT ;
	                if (op->fl.ofexcl) of |= O_EXCL ;
	                rs = bpimk_filesbegincreate(op,tfn,of,om) ;
		        if ((rs < 0) && type) {
		            uc_unlink(rbuf) ;
		        } /* end if (error) */
	            } /* end if (ok) */
	        } /* end if (mknewfname) */
	        rs1 = mem.free(a) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (m-a-f) */
	} /* end if (maxpathlen) */
	return rs ;
} /* end subroutine (bpimk_filesbeginc) */

local int bpimk_filesbeginwait(bpimk *op) noex {
	cint		nrs = SR_EXISTS ;
	int		rs ;
	int		rs1 ;
	int		c = 0 ; /* return-value */
	if (char *tbuf ; (rs = mem.mp(&tbuf)) >= 0) ylikely {
	    cchar	*dbn = op->dbname ;
	    cchar	*suf = FSUF_IDX	 ;
	    if ((rs = mknewfname(tbuf,false,dbn,suf)) >= 0) ylikely {
	        cint	to_stale = BPIMK_INTSTALE ;
	        cint	of = (O_CREAT|O_WRONLY|O_EXCL) ;
	        int	to = BPIMK_INTOPEN ;
	        cmode	om = op->om ;
	        while ((rs = bpimk_filesbegincreate(op,tbuf,of,om)) == nrs) {
	            c = 1 ;
	            msleep(200) ;
	            unlinkstale(tbuf,to_stale) ;
	            if (to-- == 0) break ;
	        } /* end while (db exists) */
	        if (rs == nrs) {
	            op->fl.ofcreat = false ;
	            c = 0 ;
	            rs = bpimk_filesbeginc(op) ;
	        } /* end if */
	    } /* end if (mknewfname) */
	    rs1 = mem.free(tbuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return (rs >= 0) ? c : rs ;
} /* end subroutine (bpimk_filesbeginwait) */

local int bpimk_filesbegincreate(bpimk *op,cchar *tfn,int of,mode_t om) noex {
	int		rs ;
	int		rs1 ;
#if	CF_DEBUG
	{
	    char	obuf[100+1] ;
	    snflagsopen(obuf,100,of) ;
	    DEBUGPRINTF("ent of=%s\n",obuf) ;
	    DEBUGPRINTF("om=%05o\n",om) ;
	}
#endif
	if ((rs = uc_open(tfn,of,om)) >= 0) ylikely {
	    cint	fd = rs ;
	    op->fl.created = true ;
	    if (cchar *cp ; (rs = mem.strw(tfn,-1,&cp)) >= 0) ylikely {
	        op->nidxfname = charp(cp) ;
	    } /* end if (memory-acquire) */
	    rs1 = u_close(fd) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (create) */
	DEBUGPRINTF("ret rs=%d\n",rs) ;
	return rs ;
} /* end subroutine (bpimk_filesbegincreate) */

local int bpimk_filesend(bpimk *op) noex {
	int		rs = SR_OK ; 
	int		rs1 ;
	if (op->nidxfname) {
	    if (op->fl.created && (op->nidxfname[0] != '\0')) {
	        u_unlink(op->nidxfname) ;
	    }
	    {
	        rs1 = mem.free(op->nidxfname) ;
	        if (rs >= 0) rs = rs1 ;
	        op->nidxfname = nullptr ;
	    } /* end if (memory-release) */
	} /* end if */
	if (op->idname) {
	    voidp vp = voidp(op->idname) ;
	    rs1 = mem.free(vp) ;
	    if (rs >= 0) rs = rs1 ;
	    op->idname = nullptr ;
	} /* end if (memory-release) */
	DEBUGPRINTF("ret rs=%d\n",rs) ;
	return rs ;
} /* end subroutine (bpimk_filesend) */

local int bpimk_listbegin(bpimk *op,int n) noex {
    	vecobj		*vlp = op->vlp ;
	int		rs ;
	{
	    int		sz = szof(bventry) ;
    	    cint	vn = n ;
	    int		vo = 0 ;
	    vo |= vecobjm.compact ;
	    vo |= vecobjm.ordered ;
	    vo |= vecobjm.stationary ;
	    rs = vlp->start(sz,vn,vo) ;
	} /* end block */
	return rs ;
} /* end subroutine (bpimk_listbegin) */

local int bpimk_listend(bpimk *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	{
	    rs1 = vecobj_finish(op->vlp) ;
	    if (rs >= 0) rs = rs1 ;
	}
	return rs ;
} /* end subroutine (bpimk_listend) */

local int bpimk_mkidx(bpimk *op) noex {
	int		rs ;
	int		rs1 ;
	int		wlen = 0 ;
	if ((rs = bpimk_nidxopen(op)) >= 0) ylikely {
	    bpihdr	hdr{} ;
	    hdr.vetu[0] = uchar(BPIHDR_VERSION) ;
	    hdr.vetu[1] = uchar(ENDIAN) ;
	    hdr.vetu[2] = 0 ;
	    hdr.vetu[3] = 0 ;
	    hdr.wtime		= (uint) time(nullptr) ;
	    hdr.nverses		= op->nverses ;
	    hdr.nzverses	= op->nzverses ;
	    hdr.maxbook		= op->maxbook ;
	    hdr.maxchap		= op->maxchap ;
	    if ((rs = bpimk_mkidxwrmain(op,&hdr)) >= 0) {
	        cint	hlen = HDRBUFLEN ;
	        char	hbuf[HDRBUFLEN+1] ;
	        hdr.fsz = rs ;
	        wlen = rs ;
	        if ((rs = hdr.rd(hbuf,hlen)) >= 0) {
	            cint	bl = rs ;
	            if ((rs = u_writep(op->nfd,hbuf,bl,0L)) >= 0) {
	                cmode	om = op->om ;
	                rs = uc_fminmod(op->nfd,om) ;
	            } /* end if (u_writep) */
	        } /* end if (bpihdr_rd) */
	    } /* end if (bpimk_mkidxwrmain) */
	    rs1 = bpimk_nidxclose(op) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (bpimk_nidx) */
	DEBUGPRINTF("ret rs=%d wlen=%u\n",rs,wlen) ;
	return (rs >= 0) ? wlen : rs ;
} /* end subroutine (bpimk_mkidx) */

local int bpimk_mkidxwrmain(bpimk *op,bpihdr *hdrp) noex {
	int		rs ;
	int		rs1 ;
	int		off = 0 ; /* return-value */
	if ((rs = ucpagesize) >= 0) ylikely {
	    cint bsz = (rs * 4) ;
	    cint nfd = op->nfd ;
	    if (filer hf ; (rs = hf.start(nfd,0,bsz,0)) >= 0) ylikely {
	        if ((rs = bpimk_mkidxwrhdr(op,hdrp,&hf,off)) >= 0) ylikely {
	            off += rs ;
	            if (rs >= 0) ylikely {
	                rs = bpimk_mkidxwrtab(op,hdrp,&hf,off) ;
	                off += rs ;
	            } /* end if (ok) */
	        } /* end if (bpimk_mkidxwrhdr) */
	        rs1 = hf.finish ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (filer) */
	} /* end if (ucpagesize) */
	return (rs >= 0) ? off : rs ;
} /* end subroutine (bpimk_mkidxwrmain) */

/* ARGSUSED */
local int bpimk_mkidxwrhdr(bpimk *op,bpihdr *hdrp,filer *hfp,int off) noex {
	int		rs = SR_FAULT ;
	int		wlen = 0 ;
	(void) off ;
	if (op) ylikely {
	    cint	hlen = HDRBUFLEN ;
	    char	hbuf[HDRBUFLEN+1] ;
	    if ((rs = hdrp->rd(hbuf,hlen)) >= 0) {
	        cint	bl = rs ;
	        rs = hfp->writefill(hbuf,bl) ;
	        wlen += rs ;
	    } /* end if (bpihdr_rd) */
	} /* end if (non-null) */
	return (rs >= 0) ? wlen : rs ;
} /* end subroutine (bpimk_mkidxwrhdr) */

local int bpimk_mkidxwrtab(bpimk *op,bpihdr *hdrp,filer *hfp,int off) noex {
    	vecobj		*vlp = op->vlp ;
	uint		a[4] = {} ;
	cint		sz = (1 * szof(uint)) ;
	int		rs = SR_OK ;
	int		wlen = 0 ; /* return-value */
	int		n = 0 ;
	hdrp->vioff = off ;
	void *vp ;
	for (int i = 0 ; vlp->get(i,&vp) >= 0 ; i += 1) {
	    bventry	*bvep = resumelife<bventry>(vp) ;
	    if (vp) {
	        a[0] = bvep->citation ;
	        rs = hfp->write(a,sz) ;
	        wlen += rs ;
	        n += 1 ;
	    }
	    if (rs < 0) break ;
	} /* end for */
	hdrp->vilen = n ;
	return (rs >= 0) ? wlen : rs ;
} /* end subroutine (bpimk_mkidxwrtab) */

local int bpimk_nidxopen(bpimk *op) noex {
	int		rs ;
	int		rs1 ;
	int		fd = -1 ;
	int		of = (O_CREAT|O_WRONLY) ;
	int		ai = 2 ; /* two path buffes */
	cmode		om = op->om ;
	if (op->nidxfname == nullptr) {
	    cint psz = (ai * (var.maxpathlen + 1)) ;
	    cint maxpath = var.maxpathlen ;
	    if (char *a ; (rs = mem.mall(psz,&a)) >= 0) ylikely {
	        cint	type = (op->fl.ofcreat && (! op->fl.ofexcl)) ;
	        cchar	*dbn = op->dbname ;
	        cchar	*suf = FSUF_IDX ;
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
	                    op->nidxfname = cast_const<charp>(cp) ;
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
} /* end subroutine (bpimk_nidxopen) */

local int bpimk_nidxclose(bpimk *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (op->nfd >= 0) {
	    rs1 = u_close(op->nfd) ;
	    if (rs >= 0) rs = rs1 ;
	    op->nfd = -1 ;
	} /* end if */
	return rs ;
} /* end subroutine (bpimk_nidxclose) */

local int bpimk_renamefiles(bpimk *op) noex {
	int		rs ;
	int		rs1 ;
	if (char *idxfname ; (rs = mem.mp(&idxfname)) >= 0) ylikely {
	    cchar	*suf = FSUF_IDX ;
	    cchar	*end = ENDIANSTR ;
	    if ((rs = mkfnamesuf2(idxfname,op->dbname,suf,end)) >= 0) ylikely {
	        if ((rs = u_rename(op->nidxfname,idxfname)) >= 0) {
	            op->nidxfname[0] = '\0' ;
	        } else {
	            u_unlink(op->nidxfname) ;
	            op->nidxfname[0] = '\0' ;
	        }
	    } /* end if (mkfnamesuf) */
	    rs1 = mem.free(idxfname) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return rs ;
} /* end subroutine (bpimk_renamefiles) */

local uint mkciteload(uint ci,uchar item) noex {
	ci = (ci << UCHAR_BIT) ;
	ci |= uint(item) ;
	return ci ; 
} /* end subroutine (mkciteload) */

local int mkcitation(uint *cip,bpimk_v *bvp) noex {
    	int		rs = SR_FAULT ;
	if (cip && bvp) ylikely {
	    uint	ci = 0 ;
	    rs = SR_OK ;
	    ci = mkciteload(ci,0) ;
	    ci = mkciteload(ci,bvp->b) ;
	    ci = mkciteload(ci,bvp->c) ;
	    ci = mkciteload(ci,bvp->v) ;
	    *cip = ci ;
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (mkcitation) */

local int mknewfname(char *tbuf,int type,cchar *dbn,cchar *suf) noex {
	cchar		*end = ENDIANSTR ;
	cchar		*fin = (type) ? "xXXXX" : "n" ;
	return mkfnamesuf3(tbuf,dbn,suf,end,fin) ;
} /* end subroutine (mknewfname) */

local int unlinkstale(cchar *fn,int to) noex {
	custime		dt = time(nullptr) ;
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

local int entcmp(BVENT *e1p,BVENT *e2p) noex {
    	int		rc = 0 ;
	if (e1p || e2p) ylikely {
	    if (e1p) {
	        if (e2p) {
	    	    cint vc1 = intconv(e1p->citation & bvcitemask) ;
	    	    cint vc2 = intconv(e2p->citation & bvcitemask) ;
	    	    rc = (vc1 - vc2) ;
	        } else {
	            rc = -1 ;
	        }
	    } else {
	        rc = +1 ;
	    }
	} /* end if (non-null) */
	return rc ;
} /* end subroutine (entcmp) */

local int vvecmp(cvoid **v1pp,cvoid **v2pp) noex {
	bventry		**e1pp = (bventry **) v1pp ;
	bventry		**e2pp = (bventry **) v2pp ;
	int		rc = 0 ;
	if (e1pp && e2pp) ylikely {
	    bventry	*e1p = *e1pp ;
	    bventry	*e2p = *e2pp ;
	    rc = entcmp(e1p,e2p) ;
	} /* end block */
	return rc ;
} /* end subroutine (vvecmp) */

vars::operator int () noex {
    	int		rs ;
	if ((rs = bufsizeget(bufsize_mp)) >= 0) ylikely {
	    maxpathlen = rs ;
	} /* end if (bufsizeget) */
    	return rs ;
} /* end if (vars::operator) */


