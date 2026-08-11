/* varmks SUPPORT (Variable-Makes) */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* make a VAR database */
/* version %I% last-modified %G% */

#define	CF_DEBUG	0		/* compile-time debugging */
#define	CF_FIRSTHASH	0		/* arrange for first-attempt hashing */

/* revision history:

	= 1998-03-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	varmks

	Description:
	This subroutine creates a VAR database file.

	Synopsis:
	int varmks_open(varmks *op,cc *dbname,int of,mode_t om,int n) noex

	Arguments:
	op		object pointer
	dbname		name of (path-to) DB
	of		open-flags
	om		open-mode
	n		starting estimate of numbers of variables

	Returns:
	>=0		OK
	<0		error code

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
#include	<sys/param.h>		/* POSIX® */
#include	<sys/stat.h>		/* POSIX® */
#include	<unistd.h>		/* POSIX® */
#include	<fcntl.h>		/* POSIX® */
#include	<ctime>			/* CSTD */
#include	<climits>		/* CSTD */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<endian.h>		/* LIBU */
#include	<ucmem.h>		/* LIBUC */
#include	<ucopen.h>		/* LIBUC */
#include	<ucdesc.h>		/* LIBUC */
#include	<ucfileop.h>		/* LIBUC */
#include	<estrings.h>		/* LIBUC */
#include	<sysval.hh>		/* LIBUC */
#include	<bufsizeget.h>		/* LIBUC */
#include	<mktmp.h>		/* LIBUC */
#include	<vecobj.h>		/* LIBUC */
#include	<strtab.h>		/* LIBUC */
#include	<filer.h>		/* LIBUC */
#include	<opentmp.h>		/* LIBUC */
#include	<hash.h>		/* LIBUC */
#include	<hashindex.h>		/* LIBUC */
#include	<nextpowtwo.h>		/* LIBUC */
#include	<isnot.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */
#include	<libdebug.h>		/* LIBDEBUG |DEBUGPRINTF(3debug)| */

#include	"varhdr.h"
#include	"varmks.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |memclear(3u)| */

/* local defines */

#define	VMKS		varmks
#define	VMKS_REC	varmks_rectab
#define	VMKS_SIZEMULT	4
#define	VMKS_NSKIP	5
#define	VMKS_INDPERMS	0664
#define	VMKS_MAG	VARMKS_MAGIC
#define	VMKS_NENTS	VARMKS_NENTRIES
#define	VMKS_INTOPEN	VARMKS_INTOPEN
#define	VMKS_INTSTALE	VARMKS_INTSTALE

#define	VE		varentry

#undef	RECTAB
#define	RECTAB		varmks_rectab

#define	HDRBUFLEN	(szof(varhdr) + 128)
#define	BUFLEN		(szof(varhdr) + 128)

#define	FSUF_IDX	VARHDR_FSUF

#ifndef	CF_DEBUG
#define	CF_DEBUG	0		/* compile-time debugging */
#endif
#ifndef	CF_FIRSTHASH
#define	CF_FIRSTHASH	0		/* arrange for first-attempt hashing */
#endif


/* imported namespaces */

using libuc::mem ;			/* variable */


/* local typedefs */

typedef uint (*rectab_t)[2] ;


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct varentry {
	uint	khash ;
	uint	ri ;
	uint	ki ;
	uint	hi ;
    } ; /* end struct (varentry) */
    struct vars {
	int	maxpathlen ;
	operator int () noex ;
    } ; /* end struct (vars) */
} /* end namespace */


/* forward references */

template<typename ... Args>
local inline int varmks_ctor(varmks *op,Args ... args) noex {
    	VARMKS		*hop = op ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    rs = memclear(hop) ;
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (varmks_ctor) */

local int varmks_dtor(varmks *op) noex {
	int		rs = SR_BUGCHECK ;
	if (op) ylikely {
	    rs = SR_OK ;
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (varmks_dtor) */

template<typename ... Args>
local inline int varmks_magic(varmks *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    rs = (op->magval == VMKS_MAG) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
} /* end subroutine (varmks_magic) */

local int	varmks_filebeg(VMKS *) noex ;
local int	varmks_filebegcr(VMKS *) noex ;
local int	varmks_filebegwait(VMKS *) noex ;
local int	varmks_filebegcr(VMKS *,cchar *,int,mode_t) noex ;
local int	varmks_fileend(VMKS *) noex ;

local int	varmks_listbegin(VMKS *,int) noex ;
local int	varmks_listend(VMKS *) noex ;

local int	varmks_mkvarfile(VMKS *) noex ;
local int	varmks_mkvarfiler(VMKS *) noex ;
local int	varmks_mkidxwrmain(VMKS *,varhdr *) noex ;
local int	varmks_mkidxwrhdr(VMKS *,varhdr *,filer *) noex ;
local int	varmks_mkrectab(VMKS *,varhdr *,filer *,int) noex ;
local int	varmks_mkind(VMKS *,cchar *,uint (*)[3],int) noex ;
local int	varmks_mkstrtab(VMKS *,varhdr *,filer *,int) noex ;
local int	varmks_nidxopen(VMKS *) noex ;
local int	varmks_nidxclose(VMKS *) noex ;
local int	varmks_renamefiles(VMKS *) noex ;

local int	rectab_start(RECTAB *,int) noex ;
local int	rectab_add(RECTAB *,uint,uint) noex ;
local int	rectab_done(RECTAB *) noex ;
local int	rectab_getvec(RECTAB *,uint (**)[2]) noex ;
local int	rectab_extend(RECTAB *) noex ;
local int	rectab_finish(RECTAB *) noex ;

#ifdef	COMMENT
local int	rectab_count(RECTAB *) noex ;
#endif

local int	mknewfname(char *,int,cchar *,cchar *) noex ;
local int	unlinkstale(cchar *,int) noex ;

local int	indinsert(uint (*rt)[2],uint (*it)[3],int,VE *) noex ;


/* local variables */

static sysval	pagesz(sysval_ps) ;
static vars	var ;
cbool		f_debug		= CF_DEBUG ;
cbool		f_firsthash	= CF_FIRSTHASH ;


/* exported variables */

const varmks_obj	varmks_modinfo = {
	"varmks",
	szof(varmks),
	0
} ; /* end initialization */


/* exported subroutines */

int varmks_open(VMKS *op,cchar *dbn,int of,mode_t om,int n) noex {
	int		rs ;
	int		c = 0 ; /* return-value */
	DEBUGPRINTF("ent dbn=%s\n",dbn) ;
	if (n < VMKS_NENTS) {
	    n = VMKS_NENTS ;
	}
	if ((rs = varmks_ctor(op,dbn)) >= 0) ylikely {
	    rs = SR_INVALID ;
	    if (dbn[0]) ylikely {
		if (static cint rsv = var ; (rs = rsv) >= 0) ylikely {
	            op->om		= om ;
	            op->nfd		= -1 ;
	            op->gid		= -1 ;
	            op->fl.ofcreat	= !!(of & O_CREAT) ;
	            op->fl.ofexcl	= !!(of & O_EXCL) ;
	            if (cchar *cp ; (rs = mem.strw(dbn,-1,&cp)) >= 0) ylikely {
	                op->dbname = cp ;
	                if ((rs = varmks_filebeg(op)) >= 0) ylikely {
		            c = rs ;
	                    if ((rs = varmks_listbegin(op,n)) >= 0) {
	                        op->magval = VMKS_MAG ;
	                    } /* end if */
	                    if (rs < 0) {
	                        varmks_fileend(op) ;
		            } /* end if (error) */
	                } /* end if */
	                if (rs < 0) {
	                    voidp vp = voidp(op->dbname) ;
	                    mem.free(vp) ;
	                    op->dbname = nullptr ;
	                } /* end if (error) */
	            } /* end if (memory-allocation) */
		} /* end if (vars) */
	    } /* end if (valid) */
	    if (rs < 0) {
		varmks_dtor(op) ;
	    } /* end if (error) */
	} /* end if (varmks_ctor) */
	DEBUGPRINTF("ret rs=%d c=%u\n",rs,c) ;
	return (rs >= 0) ? c : rs ;
} /* end subroutine (varmks_open) */

int varmks_close(VMKS *op) noex {
	int		rs ;
	int		rs1 ;
	int		nvars = 0 ;
	DEBUGPRINTF("ent nvars=%u\n",op->nvars) ;
	if ((rs = varmks_magic(op)) >= 0) ylikely {
	    bool	f_go = (! op->fl.abort) ;
	    nvars = op->nvars ;
	    if (! op->fl.abort) {
	        rs1 = varmks_mkvarfile(op) ;
	        if (rs >= 0) rs = rs1 ;
	        f_go = f_go && (rs1 >= 0) ;
	    }
	    DEBUGPRINTF("varmks_mkvarfile() rs=%d\n",rs) ;
	    if (op->nfd >= 0) {
	        rs1 = u_close(op->nfd) ;
	        if (rs >= 0) rs = rs1 ;
	        op->nfd = -1 ;
	    }
	    {
	        rs1 = varmks_listend(op) ;
	        if (rs >= 0) rs = rs1 ;
	        f_go = f_go && (rs1 >= 0) ;
	    }
	    if ((rs >= 0) && (nvars > 0) && f_go) {
	        rs1 = varmks_renamefiles(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    DEBUGPRINTF("varmks_renamefiles() rs=%d\n",rs) ;
	    {
	        rs1 = varmks_fileend(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    if (op->dbname) {
	        voidp vp = voidp(op->dbname) ;
	        rs1 = mem.free(vp) ;
	        if (rs >= 0) rs = rs1 ;
	        op->dbname = nullptr ;
	    } /* end if (memory-release) */
	    op->magval = 0 ;
	} /* end if (varmks_magic) */
	DEBUGPRINTF("varmks_close: ret=%d\n",rs) ;
	return (rs >= 0) ? nvars : rs ;
} /* end subroutine (varmks_close) */

int varmks_add(VMKS *op,cchar *k,cchar *vap,int val) noex {
	int		rs ;
	DEBUGPRINTF("ent k=%s v=>%r<\n",k,vap,val) ;
	if ((rs = varmks_magic(op,k,vap)) >= 0) ylikely {
	    if ((rs = strtab_add(&op->keys,k,-1)) >= 0) ylikely {
	        con uint	ki = rs ;
	        if ((rs = strtab_add(&op->vals,vap,val)) >= 0) ylikely {
	            con uint	vi = rs ;
	            if ((rs = rectab_add(&op->rectab,ki,vi)) >= 0) ylikely {
	                op->nvars += 1 ;
	            }
	        }
	    } /* end if (strtab_add) */
	} /* end if (varmks_magic) */
	DEBUGPRINTF("ret=%d\n",rs) ;
	return rs ;
} /* end subroutine (varmks_add) */

int varmks_abort(VMKS *op) noex {
    	int		rs ;
	if ((rs = varmks_magic(op)) >= 0) ylikely {
	    op->fl.abort = true ;
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (varmks_abort) */

int varmks_chgrp(VMKS *op,gid_t gid) noex {
    	int		rs ;
	if ((rs = varmks_magic(op)) >= 0) ylikely {
	    op->gid = gid ;
	} /* end if (varmks_magic) */
	return rs ;
} /* end subroutine (varmks_chgrp) */


/* private subroutines */

local int varmks_filebeg(VMKS *op) noex {
	int		rs = SR_OK ;
	int		c = 0 ; /* return-value */
	if (op->fl.ofcreat) {
	    rs = varmks_filebegcr(op) ;
	} else {
	    rs = varmks_filebegwait(op) ;
	    c = rs ;
	}
	DEBUGPRINTF("ret rs=%d c=%u\n",rs,c) ;
	return (rs >= 0) ? c : rs ;
} /* end subroutine (varmks_filebeg) */

local int varmks_filebegcr(VMKS *op) noex {
	cint		type = (op->fl.ofcreat && (! op->fl.ofexcl)) ;
	cint		maxpath = var.maxpathlen ;
	cint		sz = ((var.maxpathlen + 1) * 2) ;
	int		rs ;
	int		rs1 ;
	int		ai = 2 ; /* double allocation */
	cchar		*dbn = op->dbname ;
	cchar		*suf = FSUF_IDX	 ;
	if (char *a ; (rs = mem.mall(sz,&a)) >= 0) ylikely {
	    con mode_t	om = op->om ;
	    char	*tbuf = (a + (--ai * (maxpath + 1))) ;
	    if ((rs = mknewfname(tbuf,type,dbn,suf)) >= 0) ylikely {
	        cchar	*tfn = tbuf ;
	        char	*rbuf = (a + (--ai * (maxpath + 1))) ;
	        if (type) {
	            if ((rs = mktmpfile(rbuf,tbuf,om)) >= 0) ylikely {
	                op->fl.created = true ;
	                tfn = rbuf ;
	            }
	        } /* end if (type) */
	        if (rs >= 0) ylikely {
	            int	of = O_CREAT ;
	            if (op->fl.ofexcl) of |= O_EXCL ;
	            rs = varmks_filebegcr(op,tfn,of,om) ;
		    if ((rs < 0) && type) {
		        uc_unlink(rbuf) ;
		    } /* end if (error) */
	        } /* end if (ok) */
	    } /* end if (mknewfname) */
	    rs1 = mem.free(a) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return rs ;
} /* end subroutine (varmks_filebegcr) */

local int varmks_filebegwait(VMKS *op) noex {
	cint		nrs = SR_EXISTS ;
	int		rs ;
	int		rs1 ;
	int		c = 0 ; /* return-value */
	cchar		*dbn = op->dbname ;
	cchar		*suf = FSUF_IDX	 ;
	if (char *tbuf ; (rs = mem.mp(&tbuf)) >= 0) ylikely {
	    if ((rs = mknewfname(tbuf,false,dbn,suf)) >= 0) ylikely {
	        cmode	om = op->om ;
	        cint	to_stale = VMKS_INTSTALE ;
	        cint	of = (O_CREAT|O_WRONLY|O_EXCL) ;
	        int		to = VMKS_INTOPEN ;
	        while ((rs = varmks_filebegcr(op,tbuf,of,om)) == nrs) {
	            DEBUGPRINTF("loop ret rs=%d\n",rs) ;
	            c = 1 ;
	            sleep(1) ;
	            unlinkstale(tbuf,to_stale) ;
	            if (to-- == 0) break ;
	        } /* end while (db exists) */
	        if (rs == nrs) {
	            op->fl.ofcreat = false ;
	            c = 0 ;
	            rs = varmks_filebegcr(op) ;
	        }
	    } /* end if (mknewfname) */
	    rs1 = mem.free(tbuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	DEBUGPRINTF("ret rs=%d\n",rs) ;
	return (rs >= 0) ? c : rs ;
} /* end subroutine (varmks_filebegwait) */

local int varmks_filebegcr(VMKS *op,cc *tfn,int of,mode_t om) noex {
	int		rs ;
#if	CF_DEBUG
	{
	    char	obuf[100+1] ;
	    snflagsopen(obuf,100,of) ;
	    DEBUGPRINTF("varmks_filebegcr: ent of=%s\n",obuf) ;
	    DEBUGPRINTF("varmks_filebegcr: om=%05o\n",om) ;
	}
#endif
	if ((rs = uc_open(tfn,of,om)) >= 0) ylikely {
	    cint	fd = rs ;
	    cchar	*cp ;
	    op->fl.created = true ;
	    if ((rs = mem.strw(tfn,-1,&cp)) >= 0) {
	        op->nidxfname = (char *) cp ;
	    } /* end if (memory-acquire) */
	    u_close(fd) ;
	} /* end if (create) */
	DEBUGPRINTF("ret rs=%d\n",rs) ;
	return rs ;
} /* end subroutine (varmks_filebegcr) */

local int varmks_fileend(VMKS *op) noex {
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
	}
	if (op->idname) {
	    voidp vp = voidp(op->idname) ;
	    rs1 = mem.free(vp) ;
	    if (rs >= 0) rs = rs1 ;
	    op->idname = nullptr ;
	} /* end if (memory-release) */
	DEBUGPRINTF("varmks_fileend: ret rs=%d\n",rs) ;
	return rs ;
} /* end subroutine (varmks_fileend) */

local int varmks_listbegin(VMKS *op,int n) noex {
	cint		sz = (n * VMKS_SIZEMULT) ;
	int		rs ;
	if ((rs = strtab_start(&op->keys,sz)) >= 0) ylikely {
	    if ((rs = strtab_start(&op->vals,sz)) >= 0) ylikely {
	        rs = rectab_start(&op->rectab,n) ;
	        if (rs < 0) {
	            strtab_finish(&op->vals) ;
		} /* end if (error) */
	    } /* end if (strtab-vals) */
	    if (rs < 0) {
	        strtab_finish(&op->keys) ;
	    } /* end if (error) */
	} /* end if (strtab-keys) */
	return rs ;
} /* end subroutine (varmks_listbegin) */

local int varmks_listend(VMKS *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	{
	    rs1 = rectab_finish(&op->rectab) ;
	    if (rs >= 0) rs = rs1 ;
	}
	{
	    rs1 = strtab_finish(&op->vals) ;
	    if (rs >= 0) rs = rs1 ;
	}
	{
	    rs1 = strtab_finish(&op->keys) ;
	    if (rs >= 0) rs = rs1 ;
	}
	return rs ;
} /* end subroutine (varmks_listend) */

local int varmks_mkvarfile(VMKS *op) noex {
	int		rs ;
	int		nv = 0 ; /* return-value */
	if ((rs = rectab_done(&op->rectab)) >= 0) ylikely {
	    cint rtl = rs ;
	    if (rtl == (op->nvars + 1)) {
	        rs = varmks_mkvarfiler(op) ;
		nv = op->nvars ;
	    } else {
	        rs = SR_BUGCHECK ;
	    }
	} /* end if (rectab_done) */
	return (rs >= 0) ? nv : rs ;
} /* end subroutine (varmks_mkvarfile) */

local int varmks_mkvarfiler(VMKS *op) noex {
	int		rs ;
	int		rs1 ;
	int		wlen = 0 ;
	DEBUGPRINTF("ent\n") ;
	if ((rs = varmks_nidxopen(op)) >= 0) ylikely {
	    varhdr	hdr{} ;
	    hdr.vetu[0] = VARHDR_VERSION ;
	    hdr.vetu[1] = uchar(ENDIAN) ;
	    hdr.vetu[2] = 0 ;
	    hdr.vetu[3] = 0 ;
	    hdr.wtime = (uint) getustime ;
	    hdr.nvars = op->nvars ;
	    hdr.nskip = VMKS_NSKIP ;
	    if ((rs = varmks_mkidxwrmain(op,&hdr)) >= 0) ylikely {
	        cint	hlen = HDRBUFLEN ;
	        char	hbuf[HDRBUFLEN+1] ;
	        hdr.fsz = rs ;
	        wlen = rs ;
	        if ((rs = hdr.rd(hbuf,hlen)) >= 0) ylikely {
	            cint	bl = rs ;
	            if ((rs = u_writep(op->nfd,hbuf,bl,0z)) >= 0) ylikely {
	                cmode	om = op->om ;
	                rs = uc_fminmod(op->nfd,om) ;
	            } /* end if (u_writep) */
	        } /* end if (header-read) */
	    } /* end if (varmks_mkidxwrmain) */
	    rs1 = varmks_nidxclose(op) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (varmks_nidx) */
	DEBUGPRINTF("ret rs=%d wlen=%u\n",rs,wlen) ;
	return (rs >= 0) ? wlen : rs ;
} /* end subroutine (varmks_mkvarfiler) */

local int varmks_mkidxwrmain(VMKS *op,varhdr *hdrp) noex {
	int		rs ;
	int		rs1 ;
	int		off = 0 ; /* return-value */
	if ((rs = pagesz) >= 0) ylikely {
	    cint	nfd = op->nfd ;
	    int		bsz = (rs * 4) ;
	    if (filer hf ; (rs = hf.start(nfd,0,bsz,0)) >= 0) ylikely {
	        if ((rs = varmks_mkidxwrhdr(op,hdrp,&hf)) >= 0) ylikely {
	            off += rs ;
	            if (rs >= 0) ylikely {
	                rs = varmks_mkrectab(op,hdrp,&hf,off) ;
	                off += rs ;
	            }
	            if (rs >= 0) ylikely {
	                rs = varmks_mkstrtab(op,hdrp,&hf,off) ;
	                off += rs ;
	            }
	        } /* end if (varmks_mkidxwrhdr) */
	        rs1 = hf.finish ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (filer) */
	} /* end if (pagesz) */
	return (rs >= 0) ? off : rs ;
} /* end subroutine (varmks_mkidxwrmain) */

local int varmks_mkidxwrhdr(VMKS *op,varhdr *hdrp,filer *hfp) noex {
	int		rs = SR_BUGCHECK ;
	int		wlen = 0 ; /* return-value */
	if (op && hdrp) ylikely {
	    cint	hlen = HDRBUFLEN ;
	    char	hbuf[HDRBUFLEN+1] ;
	    if ((rs = hdrp->rd(hbuf,hlen)) >= 0) ylikely {
	        rs = filer_writefill(hfp,hbuf,rs) ;
	        wlen += rs ;
	    } /* end if (header-read) */
	} /* end if (non-null) */
	return (rs >= 0) ? wlen : rs ;
} /* end subroutine (varmks_mkidxwrhdr) */

local int varmks_mkrectab(VMKS *op,varhdr *hdrp,filer *hfp,int off) noex {
	int		sz ;
	int		rs ;
	int		rs1 ;
	int		wlen = 0 ; /* return-value */
	uint		(*rt)[2] ;
	if ((rs = rectab_getvec(&op->rectab,&rt)) >= 0) ylikely {
	    cint rtl = rs ;
	    hdrp->rtoff = off ;
	    hdrp->rtlen = rtl ;
	    sz = ((rtl + 1) * 2 * szof(uint)) ;
	    if ((rs = filer_write(hfp,rt,sz)) >= 0) {
	        strtab	*ksp = &op->keys ;
	        char	*kstab = nullptr ;
	        off += rs ;
	        wlen += rs ;
	        sz = strtab_strsize(ksp) ;
	        hdrp->ksoff = off ;
	        hdrp->kslen = sz ;
	        if ((rs = mem.mall(sz,&kstab)) >= 0) ylikely {
	            if ((rs = strtab_strmk(ksp,kstab,sz)) >= 0) ylikely {
	                rs = filer_write(hfp,kstab,sz) ;
	                off += rs ;
	    	        wlen += rs ;
	            }
	            if (rs >= 0) ylikely {
	                uint	(*indtab)[3] = nullptr ;
		        int	itl = nextpowtwo(rtl) ;
	                hdrp->itoff = off ;
	                hdrp->itlen = itl ;
	                sz = (itl + 1) * 3 * szof(int) ;
	                if ((rs = mem.mall(sz,&indtab)) >= 0) {
	                    memclear(indtab,sz) ;
	                    if ((rs = varmks_mkind(op,kstab,indtab,itl)) >= 0) {
	                        rs = filer_write(hfp,indtab,sz) ;
	                        off += rs ;
	    		        wlen += rs ;
	                    }
	                    mem.free(indtab) ;
	                } /* end if (memory allocation) */
	            } /* end if (record-index table) */
	            rs1 = mem.free(kstab) ;
		    if (rs >= 0) rs = rs1 ;
	        } /* end if (memory allocation) */
	    } /* end if (key-string table) */
	} /* end if (rectab_getvec) */
	return (rs >= 0) ? wlen : rs ;
} /* end subroutine (varmks_mkrectab) */

/* make an index table of the record table */
local int varmks_mkind(varmks *op,cc *kst,uint (*it)[3],int il) noex {
	uint		(*rt)[2] ;
	int		rs ;
	int		rs1 ;
	int		sc = 0 ; /* return-value */
	if ((rs = rectab_getvec(&op->rectab,&rt)) >= 0) ylikely {
	    varentry	ve{} ;
	    uint	ri, ki, hi ;
	    uint	khash ;
	    cint	rtl = rs ;
	    cchar	*kp ;
	    DEBUGPRINTF("varmks_mkind: rtl=%u\n",rtl) ;
	    if_constexpr (f_firsthash) {
	        cint	sz = szof(varentry) ;
	        cint	vo = vecobjm.compact ;
	        if (vecobj ves ; (rs = ves.start(sz,rtl,vo)) >= 0) {
	            for (ri = 1 ; ri < rtl ; ri += 1) {
	                ki = rt[ri][0] ;
	                kp = kst + ki ;
	                khash = hash_elf(kp,-1) ;
	                hi = hashindex(khash,il) ;
	                if (it[hi][0] == 0) {
	                    it[hi][0] = ri ;
	                    it[hi][1] = (khash & INT_MAX) ;
	                    it[hi][2] = 0 ;
	                    sc += 1 ;
	                } else {
	                    ve.ri = ri ;
	                    ve.ki = ki ;
	                    ve.khash = khash ;
	                    ve.hi = hi ;
	                    rs = vecobj_add(&ves,&ve) ;
	                } /* end if */
	                if (rs < 0) break ;
	            } /* end for */
	            if (rs >= 0) {
			void *vp ;
	                for (int i = 0 ; ves.get(i,&vp) >= 0 ; i += 1) {
	        	    if (VE *vep = resumelife<VE>(vp) ; vp) {
	                        sc += indinsert(rt,it,il,vep) ;
			    }
	                } /* end for */
	            } /* end if (ok) */
	            rs1 = ves.finish ;
		    if (rs >= 0) rs = rs1 ;
	        } /* end if (ves) */
	    } else { /* (f_firsthash) */
	        uint rilim = uint(rtl) ;
	        for (ri = 1 ; ri < rilim ; ri += 1) {
	            ki = rt[ri][0] ;
	            kp = kst + ki ;
	            DEBUGPRINTF("ri=%u k=%s\n",ri, kp,lenstr(kp,20)) ;
	            khash = hash_elf(kp,-1) ;
	            hi = hashindex(khash,il) ;
	            ve.ri = ri ;
	            ve.ki = ki ;
	            ve.khash = khash ;
	            ve.hi = hi ;
	            sc += indinsert(rt,it,il,&ve) ;
	        } /* end for */
	    } /* end if_constexpr (f_firsthash) */
	    it[il][0] = UINT_MAX ;
	    it[il][1] = 0 ;
	    it[il][2] = 0 ;
	    if (sc < 0) {
	        sc = 0 ;
	    }
	} /* end if (rectab_getvec) */
	DEBUGPRINTF("varmks_mkind: ret rs=%d\n",rs) ;
	return (rs >= 0) ? sc : rs ;
} /* end subroutine (varmks_mkind) */

local int varmks_mkstrtab(VMKS *op,varhdr *hdrp,filer *hfp,int off) noex {
	strtab		*vsp = &op->vals ;
	int		rs ;
	int		rs1 ;
	int		wlen = 0 ; /* return-value */
	if ((rs = vsp->strsize) >= 0) ylikely {
	    cint sz = rs ;
	    hdrp->vsoff = off ;
	    hdrp->vslen = sz ;
	    if (char *vstab ; (rs = mem.mall(sz,&vstab)) >= 0) ylikely {
	        if ((rs = strtab_strmk(vsp,vstab,sz)) >= 0) ylikely {
	            rs = filer_write(hfp,vstab,sz) ;
	            off += rs ;
		    wlen += rs ;
	        }
	        rs1 = mem.free(vstab) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (memory allocation) */
	} /* end if (strtab_strssize) */
	return (rs >= 0) ? wlen : rs ;
} /* end subroutine (varmks_mkstrtab) */

local int varmks_nidxopen(VMKS *op) noex {
	int		of = (O_CREAT|O_WRONLY) ;
	int		rs ;
	int		rs1 ;
	int		fd = -1 ;
	cmode		om = op->om ;
	DEBUGPRINTF("ent nidxfname=%s\n",op->nidxfname) ;
	if (op->nidxfname == nullptr) {
	    cint	maxpath = var.maxpathlen ;
	    cint	sz = ((var.maxpathlen + 1) * 2) ;
	    cint	type = (op->fl.ofcreat && (! op->fl.ofexcl)) ;
	    int		ai = 2 ; /* double allocation */
	    cchar	*dbn = op->dbname ;
	    cchar	*strsuf = FSUF_IDX ;
	    if (char *a ; (rs = mem.mall(sz,&a)) >= 0) ylikely {
	        char	*tbuf = (a + (--ai * (maxpath + 1))) ;
	        if ((rs = mknewfname(tbuf,type,dbn,strsuf)) >= 0) ylikely {
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
	            if (rs >= 0) ylikely {
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
	DEBUGPRINTF("ret rs=%d\n",rs) ;
	return (rs >= 0) ? fd : rs ;
} /* end subroutine (varmks_nidxopen) */

local int varmks_nidxclose(VMKS *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (op->nfd >= 0) {
	    rs1 = u_close(op->nfd) ;
	    if (rs >= 0) rs = rs1 ;
	    op->nfd = -1 ;
	}
	return rs ;
} /* end subroutine (varmks_nidxclose) */

local int varmks_renamefiles(VMKS *op) noex {
	int		rs ;
	int		rs1 ;
	cchar		*strsuf = FSUF_IDX ;
	cchar		*strend = ENDIANSTR ;
	if (char *ibuf ; (rs = mem.mp(&ibuf)) >= 0) ylikely {
	    if ((rs = mkfnamesuf(ibuf,op->dbname,strsuf,strend)) >= 0) {
	        if ((rs = u_rename(op->nidxfname,ibuf)) >= 0) {
	            op->nidxfname[0] = '\0' ;
	        } else {
	            u_unlink(op->nidxfname) ;
	            op->nidxfname[0] = '\0' ;
	        }
	    } /* end if (mkfnamesuf) */
	    rs1 = mem.free(ibuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return rs ;
} /* end subroutine (varmks_renamefiles) */

local int rectab_start(RECTAB *rtp,int n) noex {
	int		rs = SR_OK ;
	int		sz ;
	if (n < 10) n = 10 ;
	rtp->i = 0 ;
	rtp->n = n ;
	sz = ((n + 1) * 2 * szof(int)) ;
	if (void *p ; (rs = mem.mall(sz,&p)) >= 0) ylikely {
	    rtp->rectab = rectab_t(p) ;
	    rtp->rectab[0][0] = 0 ;
	    rtp->rectab[0][1] = 0 ;
	    rtp->i = 1 ;
	} /* end if (memory-acquire) */
	return rs ;
} /* end subroutine (rectab_start) */

local int rectab_finish(RECTAB *rtp) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (rtp->rectab) {
	    rs1 = mem.free(rtp->rectab) ;
	    if (rs >= 0) rs = rs1 ;
	    rtp->rectab = nullptr ;
	} /* end if (memory-release) */
	return rs ;
} /* end subroutine (rectab_finish) */

local int rectab_add(RECTAB *rtp,uint ki,uint vi) noex {
	int		rs = SR_OK ;
	int		i = rtp->i ; /* return-value */
	if ((i + 1) > rtp->n) {
	    rs = rectab_extend(rtp) ;
	}
	if (rs >= 0) ylikely {
	    rtp->rectab[i][0] = ki ;
	    rtp->rectab[i][1] = vi ;
	    rtp->i += 1 ;
	} /* end if (ok) */
	return (rs >= 0) ? i : rs ;
} /* end subroutine (rectab_add) */

local int rectab_extend(RECTAB *rtp) noex {
	int		rs = SR_OK ;
	if ((rtp->i + 1) > rtp->n) {
	    uint	(*va)[2] ;
	    int		nn, sz ;
	    nn = (rtp->n + 1) * 2 ;
	    sz = (nn + 1) * 2 * szof(int) ;
	    if ((rs = mem.rall(rtp->rectab,sz,&va)) >= 0) ylikely {
	        rtp->rectab = va ;
	        rtp->n = nn ;
	    } /* end if (memory-realloc) */
	} /* end if */
	return rs ;
} /* end subroutine (rectab_extend) */

local int rectab_done(RECTAB *rtp) noex {
	int		i = rtp->i ; /* return-value */
	rtp->rectab[i][0] = UINT_MAX ;
	rtp->rectab[i][1] = 0 ;
	return i ;
} /* end subroutine (rectab_done) */

local int rectab_getvec(RECTAB *rtp,uint (**rpp)[2]) noex {
    	int		rs = SR_BUGCHECK ;
	int		idx = 0 ; /* return-value */
	if (rtp && rpp) ylikely {
	    *rpp = rtp->rectab ;
	    idx = rtp->i ;
	} /* end if (non-null) */
	return (rs >= 0) ? idx : rs ;
} /* end subroutine (rectab_getvec) */

local int mknewfname(char *tbuf,int type,cchar *dbn,cchar *suf) noex {
	cchar		*strend = ENDIANSTR ;
	cchar		*strfin = (type) ? "xXXXX" : "n" ;
	return mkfnamesuf(tbuf,dbn,suf,strend,strfin) ;
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

local int indinsert(uint (*rt)[2],uint (*it)[3],int il,varentry *vep) noex {
	uint		chash ;
	uint		ri, ki ;
	uint		lhi, nhi ;
	uint		nhash = vep->khash ;
	uint		hi = vep->hi ;
	int		c = 0 ; /* return-value */
	chash = (nhash & INT_MAX) ;
	DEBUGPRINTF("indinsert: ve ri=%u ki=%u khash=%08X hi=%u\n",
	vep->ri,vep->ki,vep->khash,vep->hi) ;
	DEBUGPRINTF("indinsert: il=%u loop 1\n",il) ;
	forever {
	    DEBUGPRINTF("it%u ri=%u nhi=%u\n",hi,it[hi][0],it[hi][2]) ;
	    if (it[hi][0] == 0) break ;
	    ri = it[hi][0] ;
	    ki = rt[ri][0] ;
	    if (ki == vep->ki) break ;
	    it[hi][1] |= (compl INT_MAX) ;
	    nhash = hash_again(nhash,c++,VMKS_NSKIP) ;
	    hi = hashindex(nhash,il) ;
	    DEBUGPRINTF("nhash=%08X nhi=%u\n",nhash,hi) ;
	} /* end while */
	if (it[hi][0] > 0) {
	    DEBUGPRINTF("loop 2\n") ;
	    lhi = hi ;
	    while ((nhi = it[lhi][2]) > 0) {
	        lhi = nhi ;
	    } /* end while */
	    hi = hashindex((lhi + 1),il) ;
	    DEBUGPRINTF("loop 3 lhi=%u\n",lhi) ;
	    while (it[hi][0] > 0) {
	        hi = hashindex((hi + 1),il) ;
	    } /* end while */
	    it[lhi][2] = hi ;
	    DEBUGPRINTF("loop 3 it%u ki=%u nhi=%u\n",lhi, it[lhi][0],hi) ;
	} /* end if (same-key continuation) */
	it[hi][0] = vep->ri ;
	it[hi][1] = chash ;
	it[hi][2] = 0 ;
	DEBUGPRINTF("ret hi=%u c=%u\n",hi,c) ;
	return c ;
} /* end subroutine (indinsert) */

vars::operator int () noex {
    	int		rs ;
	if ((rs = bufsizeget(bufsize_mp)) >= 0) ylikely {
	    maxpathlen = rs ;
	} /* end if (bufsizeget) */
	return rs ;
} /* end if (vars::operator) */


