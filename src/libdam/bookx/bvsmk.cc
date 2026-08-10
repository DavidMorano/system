/* bvsmk SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* make a BVS database */
/* version %I% last-modified %G% */

#define	CF_DEBUG	0		/* compile-time debugging */

/* revision history:

	= 2008-10-01, David A­D­ Morano
	This module was originally written.

*/

/* Copyright © 2008 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	bvsmk

	Description:
	This module creates a BVS database file.

	Synopsis:
	int bvsmk_open(bvsmk op,cc *pr,cc *dbname,...) noex

	Arguments:
	- op		object pointer
	- pr		program-root
	- dbname	name of DB

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
	__________________________________

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
#include	<uclibmem.h>		/* LIBUC */
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
#include	<nulstr.h>		/* LIBUC */
#include	<opentmp.h>		/* LIBUC */
#include	<mkdirs.h>		/* LIBUC */
#include	<mktmp.h>		/* LIBUC */
#include	<cfdec.h>		/* LIBUC */
#include	<strwcpy.h>		/* LIBUC */
#include	<isnot.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */
#include	<libdebug.h>		/* LIBDEBUG |DEBUGPRINTF(3debug)| */

#include	"bvsmk.h"
#include	"bvshdr.h"
#include	"bvsbook.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |memclear(3u)| */

/* local defines */

#define	BVSMK_NENTRIES	(19 * 1024)
#define	BVSMK_IDNAME	"var/bvses"
#define	BVSMK_IDMODE	0777

#define	HDRBUFLEN	(szof(bvshdr) + 128)
#define	PAGEBUFMULT	4		/* page-size buffer multiplier */

#define	FSUF_IDX	"bvs"

#define	TO_OLDFILE	(5 * 60)

#ifndef	CF_DEBUG
#define	CF_DEBUG	0		/* compile-time debugging */
#endif


/* imported namespaces */

using libuc::mem ;			/* variable */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct vars {
	int	maxpathlen ;
	operator int () noex ;
    } ; /* end struct (vars) */
} /* end namespace */


/* forward references */

template<typename ... Args>
local int bvsmk_ctor(bvsmk *op,Args ... args) noex {
	BVSMK		*hop = op ;
	cnullptr	np{} ;
	cnothrow	nt{} ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    memclear(hop) ;
	    rs = SR_NOMEM ;
	    if ((op->blp = new(nt) vecobj) != np) ylikely {
		rs = SR_OK ;
	    } /* end if (new-verobj) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (bvsmk_ctor) */

local int bvsmk_dtor(bvsmk *op) noex {
	int		rs = SR_FAULT ;
	if (op) ylikely {
	    rs = SR_OK ;
	    if (op->blp) ylikely {
		delete op->blp ;
		op->blp = nullptr ;
	    } /* end if (memory-release) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (bvsmk_dtor) */

template<typename ... Args>
local inline int bvsmk_magic(bvsmk *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    rs = (op->magval == BVSMK_MAGIC) ? SR_OK : SR_NOTOPEN ;
	} /* end if */
	return rs ;
} /* end subroutine (bvsmk_magic) */

local int	bvsmk_filesbegin	(bvsmk *) noex ;
local int	bvsmk_filesbeginc	(bvsmk *) noex ;
local int	bvsmk_filesbeginwait	(bvsmk *) noex ;
local int	bvsmk_filesbegincreate	(bvsmk *,cchar *,int,mode_t) noex ;
local int	bvsmk_filesend		(bvsmk *) noex ;
local int	bvsmk_listbegin		(bvsmk *,int) noex ;
local int	bvsmk_listend		(bvsmk *) noex ;
local int	bvsmk_mkidx		(bvsmk *) noex ;
local int	bvsmk_mkidxwrmain	(bvsmk *,bvshdr *) noex ;
local int	bvsmk_mkidxwrhdr	(bvsmk *,bvshdr *,filer *) noex ;
local int	bvsmk_mkidxchaptab	(bvsmk *,bvshdr *,filer *,int) noex ;
local int	bvsmk_mkidxbooktab	(bvsmk *,bvshdr *,filer *,int) noex ;
local int	bvsmk_nidxopen		(bvsmk *) noex ;
local int	bvsmk_nidxclose		(bvsmk *) noex ;
local int	bvsmk_renamefiles	(bvsmk *) noex ;

local int	mkdname		(cchar *,mode_t) noex ;
local int	mknifname	(char *,int,cchar *,cchar *,cchar *) noex ;
local int	unlinkstale	(cchar *,int) noex ;


/* local variables */

static vars		var ;
cbool			f_debug		= CF_DEBUG ;


/* exported variables */

const bvsmk_obj		bvsmk_modinfo = {
	"bvsmk",
	szof(bvsmk),
	0
} ; /* end initializætion */


/* exported subroutines */

int bvsmk_open(bvsmk *op,cchar *pr,cchar *db,int of,mode_t µom) noex {
	int		rs ;
	int		c = 0 ;
	if ((rs = bvsmk_ctor(op,pr,db)) >= 0) ylikely {
	    rs = SR_INVALID ;
	    if (pr[0] && db[0]) {
	        DEBUGPRINTF("ent pr=%s\n",pr) ;
	        DEBUGPRINTF("db=%s\n",db) ;
		if (static cint rsv = var ; (rs = rsv) >= 0) ylikely {
	            cint	n = BVSMK_NENTRIES ;
	            int		sz = 0 ;
	            op->omode = µom ;
	            op->nfd = -1 ;
	            op->fl.ofcreat	= MKBOOL(of & O_CREAT) ;
	            op->fl.ofexcl	= MKBOOL(of & O_EXCL) ;
	            sz += (lenstr(pr) + 1) ;
	            sz += (lenstr(db) + 1) ;
	            if (char *bp ; (rs = mem.mall(sz,&bp)) >= 0) ylikely {
	                op->a = bp ;
	                op->pr = bp ;
	                bp = (strwcpy(bp,pr,-1)+1) ;
	                op->db = bp ;
	                bp = (strwcpy(bp,db,-1)+1) ;
	                if ((rs = bvsmk_filesbegin(op)) >= 0) ylikely {
		            c = rs ;
		            if ((rs = bvsmk_listbegin(op,n)) >= 0) ylikely {
			        op->magval = BVSMK_MAGIC ;
		            }
		            if (rs < 0) {
			        bvsmk_filesend(op) ;
		            } /* end if (error) */
		        } /* end if (files-begin) */
		        if (rs < 0) {
		            mem.free(op->a) ;
		            op->a = nullptr ;
		        } /* end if (error) */
	            } /* end if (memory-acquire) */
		} /* end if (vars) */
	    } /* end if (valid) */
	    if (rs < 0) {
		bvsmk_dtor(op) ;
	    } /* end if (error) */
	} /* end if (bvsk_ctor) */
	DEBUGPRINTF("ret rs=%d c=%d\n",rs,c) ;
	return (rs >= 0) ? c : rs ;
} /* end subroutine (bvsmk_open) */

int bvsmk_close(bvsmk *op) noex {
	int		rs ;
	int		rs1 ;
	int		nv = 0 ; /* return-value */
	if ((rs = bvsmk_magic(op)) >= 0) ylikely {
	    DEBUGPRINTF("ent nverses=%u\n",op->nverses) ;
	    DEBUGPRINTF("nzverses=%u\n",op->nzverses) ;
	    bool	f_go = (! op->fl.abort) ;
	    nv = op->nverses ;
	    if (nv > 0) {
	        rs1 = bvsmk_mkidx(op) ;
	        if (rs >= 0) rs = rs1 ;
	        f_go = f_go && (rs1 >= 0) ;
	    } /* end if */
	    if (op->nfd >= 0) {
	        rs1 = u_close(op->nfd) ;
	        if (rs >= 0) rs = rs1 ;
	        op->nfd = -1 ;
	    }
	    {
	       rs1 = bvsmk_listend(op) ;
	       if (rs >= 0) rs = rs1 ;
	       f_go = f_go && (rs1 >= 0) ;
	    }
	    if ((rs >= 0) && (nv > 0) && f_go) {
	        rs1 = bvsmk_renamefiles(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
	       rs1 = bvsmk_filesend(op) ;
	       if (rs >= 0) rs = rs1 ;
	    }
	    if (op->db) {
	        rs1 = mem.free(op->a) ;
	        if (rs >= 0) rs = rs1 ;
	        op->a = nullptr ;
	    } /* end if (memory-release) */
	    {
	        rs1 = bvsmk_dtor(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    op->magval = 0 ;
	} /* end if (bvsmk_magic) */
	DEBUGPRINTF("ret rs=%d\n",rs) ;
	return (rs >= 0) ? nv : rs ;
} /* end subroutine (bvsmk_close) */

int bvsmk_add(bvsmk *op,int book,uchar *ap,int al) noex {
	int		rs ;
	if ((rs = bvsmk_magic(op,ap)) >= 0) ylikely {
	    if ((book >= 0) && (al >= 0)) {
	        if (al > 0) {
		    cint	sz = (al * szof(uchar)) ;
		    if (uchar *bp ; (rs = mem.mall(sz,&bp)) >= 0) ylikely {
		        vecobj	*blp = op->blp ;
		        bvsbook	be{} ;
	                uint	nzverses ;
	                uint	nverses = 0 ;
	                for (int i = 0 ; i < al ; i += 1) {
	                    bp[i] = ap[i] ;
	                    nverses += ap[i] ;
	                } /* end for */
	                nzverses	= (nverses - ap[0]) ;
	                be.book		= uchar(book) ;
	                be.nverses	= uchar(nverses) ;
	                be.nzverses	= uchar(nzverses) ;
	                be.al		= uchar(al) ;
	                be.ap		= bp ;
	                if ((rs = blp->add(&be)) >= 0) {
		            op->nverses += nverses ;
		            if (book > 0) op->nzverses += nzverses ;
	                } /* end if (vecobj_add) */
	                if (rs < 0) {
	                    mem.free(bp) ;
		        } /* end if (error) */
	            } /* end if (memory-acquire) */
	        } /* end if (greater) */
	    } else {
	        rs = SR_INVALID ;
	    }
	} /* end if (bvsmk_magic) */
	DEBUGPRINTF("ret rs=%d\n",rs) ;
	return rs ;
} /* end subroutine (bvsmk_add) */

int bvsmk_abort(bvsmk *op,int f) noex {
    	int		rs ;
	if ((rs = bvsmk_magic(op)) >= 0) ylikely {
	    op->fl.abort = !!f ;
	} /* end if (bvsmk_magic) */
	return rs ;
} /* end subroutine (bvsmk_abort) */


/* private subroutines */

local int bvsmk_filesbegin(bvsmk *op) noex {
	int		rs ;
	int		rs1 ;
	int		c = 0 ;
	if (char *tbuf ; (rs = mem.mp(&tbuf)) >= 0) ylikely {
	    cchar	*idname = BVSMK_IDNAME ;
	    cchar	*dnp = tbuf ;
	    if ((rs = mkpath2(tbuf,op->pr,idname)) >= 0) ylikely {
	        cint	dnl = rs ;
	        cmode	dm = 0777 ;
	        if ((rs = mkdname(tbuf,dm)) >= 0) ylikely {
	            if (cchar *cp ; (rs = mem.strw(dnp,dnl,&cp)) >= 0) ylikely {
	                op->idname = cp ;
	                if (op->fl.ofcreat) {
	                    rs = bvsmk_filesbeginc(op) ;
	                } else {
	                    rs = bvsmk_filesbeginwait(op) ;
	                    c = rs ;
	                } /* end if */
	                if (rs < 0) {
	                    void *vp = voidp(op->idname) ;
	                    mem.free(vp) ;
	                    op->idname = nullptr ;
	                } /* end if (erro) */
	            } /* end if (memory-acquire) */
	        } /* end if (mkdname) */
	    } /* end if (mkpath) */
	    rs1 = mem.free(tbuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	DEBUGPRINTF("ret rs=%d c=%u\n",rs,c) ;
	return (rs >= 0) ? c : rs ;
} /* end subroutine (bvsmk_filesbegin) */

local int bvsmk_filesbeginc(bvsmk *op) noex {
	cint		type = (op->fl.ofcreat && (! op->fl.ofexcl)) ;
	cint		maxpath = var.maxpathlen ;
	int		rs ;
	int		rs1 ;
	int		ai = 2 ; /* two path buffers */
	{
	    cchar	*id = op->idname ;
	    cchar	*db = op->db ;
	    cchar	*suf = FSUF_IDX	;
	    cint sz = (ai * (var.maxpathlen + 1)) ;
	    if (char *a ; (rs = mem.mall(sz,&a)) >= 0) ylikely {
		char *tbuf = (a + (--ai * (maxpath + 1))) ;
	        if ((rs = mknifname(tbuf,type,id,db,suf)) >= 0) ylikely {
	            cmode	om = op->omode ;
	            cchar	*tfn = tbuf ;
	            char	*rbuf = (a + (--ai * (maxpath + 1))) ;
	            if (type) {
	                if ((rs = mktmpfile(rbuf,tbuf,om)) >= 0) {
	                    op->fl.created = true ;
	                    tfn = rbuf ;
	                }
	            } /* end if (type) */
	            if (rs >= 0) {
	                int	of = O_CREAT ;
	                if (op->fl.ofexcl) of |= O_EXCL ;
	                rs = bvsmk_filesbegincreate(op,tfn,of,om) ;
		        if ((rs < 0) && type) {
		            uc_unlink(rbuf) ;
		        } /* end if (error) */
	            } /* end if (ok) */
	        } /* end if (mknifname) */
	        rs1 = mem.free(a) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (m-a-f) */
	} /* end block */
	return rs ;
} /* end subroutine (bvsmk_filesbeginc) */

local int bvsmk_filesbeginwait(bvsmk *op) noex {
	cint		rse = SR_EXISTS ;
	int		rs ;
	int		rs1 ;
	int		c = 0 ;
	if (char *tbuf ; (rs = mem.mp(&tbuf)) >= 0) ylikely {
	    cchar	*id = op->idname ;
	    cchar	*db = op->db ;
	    cchar	*suf = FSUF_IDX	 ;
	    if ((rs = mknifname(tbuf,false,id,db,suf)) >= 0) ylikely {
	        cint	to_stale = BVSMK_INTSTALE ;
	        cint	of = (O_CREAT|O_WRONLY|O_EXCL) ;
	        int	to = BVSMK_INTOPEN ;
	        cmode	om = op->omode ;
	        while ((rs = bvsmk_filesbegincreate(op,tbuf,of,om)) == rse) {
	            c = 1 ;
	            msleep(200) ;
	            unlinkstale(tbuf,to_stale) ;
	            if (to-- == 0) break ;
	        } /* end while (db exists) */
	        if (rs == rse) {
	            op->fl.ofcreat = false ;
	            c = 0 ;
	            rs = bvsmk_filesbeginc(op) ;
	        } /* end if (not-found) */
	    } /* end if (mknifname) */
	    rs1 = mem.free(tbuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	DEBUGPRINTF("ret rs=%d c=%d\n",rs,c) ;
	return (rs >= 0) ? c : rs ;
} /* end subroutine (bvsmk_filesbeginwait) */

local int bvsmk_filesbegincreate(bvsmk *op,cchar *tfn,int of,mode_t om) noex {
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
	        op->nidxfname = (char *) cp ;
	    } /* end if (memory-acquire) */
	    rs1 = u_close(fd) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (create) */
	DEBUGPRINTF("ret rs=%d\n",rs) ;
	return rs ;
} /* end subroutine (bvsmk_filesbegincreate) */

local int bvsmk_filesend(bvsmk *op) noex {
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
	} /* end if (non-null) */
	if (op->idname) {
	    void *vp = voidp(op->idname) ;
	    rs1 = mem.free(vp) ;
	    if (rs >= 0) rs = rs1 ;
	    op->idname = nullptr ;
	} /* end if (memory-release) */
	DEBUGPRINTF("ret rs=%d\n",rs) ;
	return rs ;
} /* end subroutine (bvsmk_filesend) */

local int bvsmk_listbegin(bvsmk *op,int n) noex {
    	vecobj		*blp = op->blp ;
	int		rs ;
	int		vo = 0 ;
	{
	    cint sz = szof(bvsbook) ;
	    vo |= vecobjm.compact ;
	    vo |= vecobjm.ordered ;
	    vo |= vecobjm.stationary ;
	    rs = blp->start(sz,n,vo) ;
	}
	return rs ;
} /* end subroutine (bvsmk_listbegin) */

local int bvsmk_listend(bvsmk *op) noex {
    	vecobj		*blp = op->blp ;
	int		rs = SR_OK ;
	int		rs1 ;
	void *vp ;
	for (int i = 0 ; blp->get(i,&vp) >= 0 ; i += 1) {
	    bvsbook	*bep = resumelife<bvsbook>(vp) ;
	    if (bep) {
	        if (bep->ap) {
		    rs1 = mem.free(bep->ap) ;
		    if (rs >= 0) rs = rs1 ;
		} /* end if (memory-release) */
	    }
	} /* end for */
	DEBUGPRINTF("mid rs=%d\n",rs) ;
	{
	    rs1 = blp->finish ;
	    if (rs >= 0) rs = rs1 ;
	}
	DEBUGPRINTF("ret rs=%d\n",rs) ;
	return rs ;
} /* end subroutine (bvsmk_listend) */

local int bvsmk_mkidx(bvsmk *op) noex {
	int		rs ;
	int		rs1 ;
	int		wlen = 0 ;
	DEBUGPRINTF("ent\n") ;
	if ((rs = bvsmk_nidxopen(op)) >= 0) ylikely {
	    bvshdr	hdr{} ;
	    hdr.vetu[0]		= uchar(BVSHDR_VERSION) ;
	    hdr.vetu[1]		= uchar(ENDIAN) ;
	    hdr.vetu[2]		= 0 ;
	    hdr.vetu[3]		= 0 ;
	    hdr.wtime		= (uint) time(nullptr) ;
	    hdr.nverses		= op->nverses ;
	    hdr.nzverses	= op->nverses ;
	    if ((rs = bvsmk_mkidxwrmain(op,&hdr)) >= 0) ylikely {
	        cint	hlen = HDRBUFLEN ;
	        char	hbuf[HDRBUFLEN+1] ;
	        hdr.fsz = rs ;
	        wlen = rs ;
	        if ((rs = hdr.rd(hbuf,hlen)) >= 0) {
	            if ((rs = u_writep(op->nfd,hbuf,rs,0z)) >= 0) {
	                cmode	om = op->omode ;
	                rs = uc_fminmod(op->nfd,om) ;
	            } /* end if (u_writep) */
	        } /* end if (bvshdr_rd) */
	    } /* end if (bvsmk_mkidxwrmain) */
	    rs1 = bvsmk_nidxclose(op) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (bvsmk_nidx) */
	DEBUGPRINTF("ret rs=%d wlen=%u\n",rs,wlen) ;
	return (rs >= 0) ? wlen : rs ;
} /* end subroutine (bvsmk_mkidx) */

local int bvsmk_mkidxwrmain(bvsmk *op,bvshdr *hdrp) noex {
	int		rs ;
	int		rs1 ;
	int		foff = 0 ; /* return-value */
	if ((rs = ucpagesize) >= 0) ylikely {
	    cint bsz = (rs * PAGEBUFMULT) ;
	    cint nfd = op->nfd ;
	    if (filer hf ; (rs = hf.start(nfd,0,bsz,0)) >= 0) ylikely {
	        if ((rs = bvsmk_mkidxwrhdr(op,hdrp,&hf)) >= 0) ylikely {
	            foff += rs ;
		    op->maxbook = 0 ;
	            if (rs >= 0) {
	                rs = bvsmk_mkidxchaptab(op,hdrp,&hf,foff) ;
	                foff += rs ;
	            } /* end if (ok) */
	            if (rs >= 0) {
	                rs = bvsmk_mkidxbooktab(op,hdrp,&hf,foff) ;
	                foff += rs ;
	            } /* end if (ok) */
	        } /* end if (bvsmk_mkidxwrhdr) */
	        rs1 = hf.finish ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (filer) */
	} /* end if (ucpagesize) */
	return (rs >= 0) ? foff : rs ;
} /* end subroutine (bvsmk_mkidxwrmain) */

local int bvsmk_mkidxwrhdr(bvsmk *op,bvshdr *hdrp,filer *hfp) noex {
	int		rs = SR_FAULT ;
	int		wlen = 0 ;
	if (op) ylikely {
	    cint	hlen = HDRBUFLEN ;
	    char	hbuf[HDRBUFLEN+1] ;
	    if ((rs = hdrp->rd(hbuf,hlen)) >= 0) ylikely {
	        rs = hfp->writefill(hbuf,rs) ;
	        wlen += rs ;
	    } /* end if (bvshdr_rd) */
	} /* end if (non-null) */
	return (rs >= 0) ? wlen : rs ;
} /* end subroutine (bvsmk_mkidxwrhdr) */

local int bvsmk_mkidxchaptab(bvsmk *op,bvshdr *hdrp,filer *hfp,int foff) noex {
	vecobj		*blp = op->blp ;
	int		rs = SR_OK ;
	int		ctlen = 0 ;
	int		n = 0 ;
	int		wlen = 0 ; /* return-value */
	hdrp->ctoff = foff ;
	void *vp ;
	for (int i = 0 ; blp->get(i,&vp) >= 0 ; i += 1) {
	    bvsbook	*bep = resumelife<bvsbook>(vp) ;
	    if (bep) {
	        if (bep->book > 0) n += 1 ;
		if (bep->book > op->maxbook) {
		    op->maxbook = bep->book ;
		}
		ctlen += bep->al ;
	        bep->ci = ucharconv(foff - hdrp->ctoff) ;
	        rs = hfp->write(bep->ap,bep->al) ;
	        foff += rs ;
		wlen += rs ;
	        if (rs < 0) break ;
	    } /* end if */
	} /* end for */
	hdrp->nzbooks = n ;
	hdrp->ctlen = ctlen ;
	return (rs >= 0) ? wlen : rs ;
} /* end subroutine (bvsmk_mkidxchaptab) */

local int bvsmk_mkidxbooktab(bvsmk *op,bvshdr *hdrp,filer *hfp,int foff) noex {
	vecobj		*blp = op->blp ;
	cint		n = (op->maxbook + 1) ;
	cint		hsz = szof(ushort) ;
	cint		narrval = 4 ;
	int		rs ;
	int		rs1 ;
	int		wlen = 0 ; /* return-value */
	if ((rs = hfp->writealign(hsz)) >= 0) ylikely {
	    cint	sz = (n * narrval) * szof(ushort) ;
	    ushort	(*arrp)[narrval] = nullptr ;
	    wlen += rs ;
	    foff += rs ;
	    if ((rs = mem.call(1,sz,&arrp)) >= 0) ylikely {
		void *vp ;
	        for (int i = 0 ; blp->get(i,&vp) >= 0 ; i += 1) {
	            bvsbook	*bep = resumelife<bvsbook>(vp) ;
	            if (bep) {
	                rs = bvsbook_set(bep,arrp[bep->book]) ;
	            }
		    if (rs < 0) break ;
	        } /* end for */
	        if (rs >= 0) {
	            hdrp->btoff = foff ;
	            hdrp->btlen = n ;
	            rs = hfp->write(arrp,sz) ;
	            wlen += rs ;
	        } /* end if (ok) */
	        rs1 = mem.free(arrp) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (memory-acquire) */
	} /* end if (filer_writealign) */
	return (rs >= 0) ? wlen : rs ;
} /* end subroutine (bvsmk_mkidxbooktab) */

local int bvsmk_nidxopen(bvsmk *op) noex {
    	cint		maxpath = var.maxpathlen ;
	int		rs ;
	int		rs1 ;
	int		fd = -1 ;
	int		of = (O_CREAT|O_WRONLY) ;
	int		ai = 2 ; /* two path buffers */
	cmode		om = op->omode ;
	DEBUGPRINTF("ent nidxfname=%s\n",op->nidxfname) ;
	if (op->nidxfname == nullptr) {
	    cint	type = (op->fl.ofcreat && (! op->fl.ofexcl)) ;
	    cint	sz = (ai * (maxpath + 1)) ;
	    cchar	*id = op->idname ;
	    cchar	*db = op->db ;
	    cchar	*suf = FSUF_IDX ;
	    if (char *a ; (rs = mem.mall(sz,&a)) >= 0) ylikely {
	        char	*tbuf = (a + (--ai * (maxpath + 1))) ;
	        if ((rs = mknifname(tbuf,type,id,db,suf)) >= 0) ylikely {
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
	                    op->nidxfname = (char *) cp ;
	                } /* end if (memory-acquire) */
		        if (rs < 0) {
			    u_close(fd) ;
			    op->nfd = -1 ;
			    if (type) u_unlink(rbuf) ;
		        } /* end if (error) */
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
	DEBUGPRINTF("ret rs=%d\n",rs) ;
	return (rs >= 0) ? fd : rs ;
} /* end subroutine (bvsmk_nidxopen) */

local int bvsmk_nidxclose(bvsmk *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (op->nfd >= 0) {
	    rs1 = u_close(op->nfd) ;
	    if (rs >= 0) rs = rs1 ;
	    op->nfd = -1 ;
	} /* end if */
	return rs ;
} /* end subroutine (bvsmk_nidxclose) */

local int bvsmk_renamefiles(bvsmk *op) noex {
    	cint		maxpath = var.maxpathlen ;
	int		rs ;
	int		rs1 ;
	int		ai = 2 ; /* two path buffers */
	DEBUGPRINTF("ent\n") ;
	{
	    cint	psz = (ai * (maxpath + 1)) ;
	    if (char *a ; (rs = mem.mall(psz,&a)) >= 0) {
	        cint	clen = maxpath ;
	        cchar	*suf = FSUF_IDX ;
	        cchar	*end = ENDIANSTR ;
	        char	*cbuf = (a + (--ai * (maxpath + 1))) ;
	        if ((rs = sncpy4(cbuf,clen,op->db,".",suf,end)) >= 0) {
	            char	*idxfname = (a + (--ai * (maxpath + 1))) ;
	            if ((rs = mkpath2(idxfname,op->idname,cbuf)) >= 0) {
	                if ((rs = u_rename(op->nidxfname,idxfname)) >= 0) {
	                    op->nidxfname[0] = '\0' ;
	                } else {
	                    u_unlink(op->nidxfname) ;
	                    op->nidxfname[0] = '\0' ;
	                } /* end if */
	            } /* end if (mkpath) */
	        } /* end if (ok) */
		rs1 = mem.free(a) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (m-a-f) */
	} /* end block */
	DEBUGPRINTF("ret rs=%d\n",rs) ;
	return rs ;
} /* end subroutine (bvsmk_renamefiles) */

local int mkdname(cchar *dname,mode_t dm) noex {
	cint		rsn = SR_NOENT ;
	int		rs ;
	if (ustat sb ; (rs = u_stat(dname,&sb)) == rsn) {
	    dm |= (BVSMK_IDMODE | 0555) ;
	    if ((rs = mkdirs(dname,dm)) >= 0) {
		rs = uc_minmod(dname,dm) ;
	    }
	} /* end if (creating directory) */
	DEBUGPRINTF("ret rs=%d\n",rs) ;
	return rs ;
} /* end subroutine (mkdname) */

local int mknifname(char *rbuf,int type,cchar *id,cchar *db,cchar *suf) noex {
	int		rs ;
	int		rs1 ;
	cchar		*end = ENDIANSTR ;
	cchar		*fin = (type) ? "xXXXX" : "n" ;
	if (char *tbuf ; (rs = mem.mp(&tbuf)) >= 0) ylikely {
	    if ((rs = mkpath2(tbuf,id,db)) >= 0) ylikely {
	        rs = mkfnamesuf3(rbuf,tbuf,suf,end,fin) ;
	    }
	    rs1 = mem.free(tbuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return rs ;
} /* end subroutine (mknifname) */

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

vars::operator int () noex {
    	int		rs ;
	if ((rs = bufsizeget(bufsize_mp)) >= 0) ylikely {
	    maxpathlen = rs ;
	} /* end if (bufsizeget) */
    	return rs ;
} /* end if (vars::operator) */


