/* cyimk SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* Calendar-Year-Index DB-make (make a CYI database) */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	cyimk

	Description:
	This module creates a CYI database file.

	Synopsis:
	int cyimk_open(cyimk *op,int year,cc *dn,cc *cn,int of,mode_t om) noex

	Arguments:
	op		object pointer
	dn		directory path
	cn		name of calendar
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

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>		/* POSIX® */
#include	<sys/param.h>		/* POSIX® */
#include	<sys/stat.h>		/* POSIX® */
#include	<unistd.h>		/* POSIX® */
#include	<fcntl.h>		/* POSIX® */
#include	<ctime>			/* CSTD */
#include	<climits>		/* CSTD |INT_MAX| */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstring>		/* CSTD */
#include	<algorithm>		/* C++STD |min(3c++)| + |max(3c++)| */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<endian.h>		/* LIBU */
#include	<intcmp.h>		/* LIBU */
#include	<ucmem.h>		/* LIBUC */
#include	<ucsysmisc.h>		/* LIBUC |ucpagesize(3u)| */
#include	<ucopen.h>		/* LIBUC */
#include	<ucdesc.h>		/* LIBUC */
#include	<ucfileop.h>		/* LIBUC */
#include	<bufsizeget.h>		/* LIBUC */
#include	<estrings.h>		/* LIBUC */
#include	<path.h>		/* LIBUC */
#include	<ids.h>			/* LIBUC */
#include	<permx.h>		/* LIBUC */
#include	<mkdirs.h>		/* LIBUC */
#include	<mktmp.h>		/* LIBUC */
#include	<storebuf.h>		/* LIBUC */
#include	<filer.h>		/* LIBUC */
#include	<tmtime.hh>		/* LIBUC */
#include	<opentmp.h>		/* LIBUC */
#include	<char.h>		/* LIBUC */
#include	<isnot.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */

#include	"cyihdr.h"
#include	"cyimk.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |memclear(3u)| */

/* local defines */

#define	CYIMK_DEFENTS	1024
#define	CYIMK_NSKIP	5

#define	HDRBUFLEN	(szof(cyihdr) + 128)
#define	BUFLEN		(szof(cyihdr) + 128)

#define	FSUF_IDX	"cyi"

#define	TO_OLDFILE	(5 * 60)


/* imported namespaces */

using libuc::mem ;			/* variable */


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
} ; /* end struct */

struct blentry {
	uint		loff ;
	uint		llen ;
} ; /* end struct */

namespace {
    struct vars {
	int		pagesz ;
	int		maxpathlen ;
	operator int () noex ;
    } ; /* end struct (vars) */
} /* end namespace */


/* forward references */

template<typename ... Args>
local inline int cyimk_ctor(cyimk *op,Args ... args) noex {
    	CYIMK		*hop = op ;
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
	        } /* emd of (error) */
	    } /* end if (new-vecobj) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (cyimk_ctor) */

local int cyimk_dtor(cyimk *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	    if (op->llp) {
		delete op->llp ;
		op->llp = nullptr ;
	    } /* end if (delete-vecobj) */
	    if (op->vlp) {
		delete op->vlp ;
		op->vlp = nullptr ;
	    } /* end if (delete-vecobj) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (cyimk_dtor) */

template<typename ... Args>
local inline int cyimk_magic(cyimk *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    rs = (op->magval == CYIMK_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
} /* end subroutine (cyimk_magic) */

local int	cyimk_idbegin(cyimk *,cchar *,int) noex ;
local int	cyimk_idend(cyimk *) noex ;
local int	cyimk_idxdir(cyimk *,ids *,cchar *) noex ;
local int	cyimk_minown(cyimk *,cchar *,mode_t) noex ;

local int	cyimk_filesbegin(cyimk *) noex ;
local int	cyimk_filesbeginc(cyimk *) noex ;
local int	cyimk_filesbeginwait(cyimk *) noex ;
local int	cyimk_filesbegcr(cyimk *,cchar *,int,mode_t) noex ;
local int	cyimk_filesend(cyimk *) noex ;

local int	cyimk_listbegin(cyimk *,int) noex ;
local int	cyimk_listend(cyimk *) noex ;

local int	cyimk_mkidx(cyimk *) noex ;
local int	cyimk_mkidxmain(cyimk *,cyihdr *) noex ;
local int	cyimk_mkidxhdr(cyimk *,cyihdr *,filer *) noex ;
local int	cyimk_mkidxstrs(cyimk *,cyihdr *,filer *,int) noex ;
local int	cyimk_mkidxents(cyimk *,cyihdr *,filer *,int) noex ;
local int	cyimk_mkidxlines(cyimk *,cyihdr *,filer *,int) noex ;
local int	cyimk_nidxopen(cyimk *) noex ;
local int	cyimk_nidxclose(cyimk *) noex ;
local int	cyimk_renamefiles(cyimk *) noex ;

local int	mkydname(char *,cchar *,int) noex ;
local int	mkcitation(uint *,cyimk_ent *) noex ;
local int	mknewfname(char *,int,cchar *,cchar *) noex ;
local int	unlinkstale(cchar *,int) noex ;

local int	vcmpve(cvoid **,cvoid **) noex ;


/* local variables */


/* exported variables */

const cyimk_obj		cyimk_modinfo = {
	"cyimk",
	szof(cyimk),
	0
} ; /* end initialization */

static vars		var ;


/* exported subroutines */

local int cyimk_opens(cyimk *,int,cc *,cc *,int) noex ;

int cyimk_open(cyimk *op,int year,cc *dname,cc *cname,int of,mode_t om) noex {
	cint		n = CYIMK_DEFENTS ;
	int		rs ;
	int		c = 0 ;
	if ((rs = cyimk_ctor(op,dname,cname)) >= 0) ylikely {
	    rs = SR_INVALID ;
	    if (dname[0] && cname[0]) ylikely {
		if (static cint rsv = var ; (rs = rsv) >= 0) ylikely {
	            if (year <= 0) {
	                tmtime	tmval ;
	                time_t	dt = getustime ;
	                rs = tmtime_timelocal(&tmval,dt) ;
	                year = (tmval.year + TMTIME_YEARBASE) ;
	            } /* end if */
	            op->om = (om | 0600) ;
	            op->nfd = -1 ;
	            op->year = year ;
	            op->fl.ofcreat	= !!(of & O_CREAT) ;
	            op->fl.ofexcl	= !!(of & O_EXCL) ;
	            op->fl.none = (! op->fl.ofcreat) && (! op->fl.ofexcl) ;
	            if (rs >= 0) ylikely {
		        rs = cyimk_opens(op,year,dname,cname,n) ;
		        c = rs ;
	            } /* end if (ok) */
		} /* end if (vars) */
	    } /* end if (valid) */
	    if (rs < 0) {
		cyimk_dtor(op) ;
	    } /* end if (error) */
	} /* end if (ok) */
	return (rs >= 0) ? c : rs ;
} /* end subroutine (cyimk_open) */

local int cyimk_opens(cyimk *op,int year,cc *dname,cc *cname,int n) noex {
    	int		rs ;
	int		c = 0 ; /* return-value */
	if ((rs = cyimk_idbegin(op,dname,year)) >= 0) ylikely {
	    if (cchar *cp ; (rs = mem.strw(cname,-1,&cp)) >= 0) ylikely {
	        op->cname = cp ;
	        if ((rs = cyimk_filesbegin(op)) >= 0) ylikely {
	            c = rs ;
	            if ((rs = cyimk_listbegin(op,n)) >= 0) ylikely {
	                op->magval = CYIMK_MAGIC ;
	            } /* end if */
	            if (rs < 0) {
	                cyimk_filesend(op) ;
		    } /* end if (error) */
	        } /* end if */
	        if (rs < 0) {
	            if (op->cname) {
	                voidp vp = voidp(op->cname) ;
	                mem.free(vp) ;
	                op->cname = nullptr ;
	            } /* end if (memory-release) */
	        } /* end if (error) */
	    } /* end if (memory-allocation) */
	    if (rs < 0) {
	        cyimk_idend(op) ;
	    } /* end if (error) */
	} /* end if (cyim_idbegin) */
	return (rs >= 0) ? c : rs ;
} /* end subroutine (cyimk_opens) */

int cyimk_close(cyimk *op) noex {
	int		rs ;
	int		rs1 ;
	int		n = 0 ; /* return-value */
	if ((rs = cyimk_magic(op)) >= 0) ylikely {
	    bool	f_go = false ;
	    f_go = (! op->fl.abort) ;
	    if ((n = op->nentries) > 0) {
	        if (op->fl.notsorted) {
	            vecobj_sort(op->vlp,vcmpve) ;
	        }
	        {
	            rs1 = cyimk_mkidx(op) ;
	            if (rs >= 0) rs = rs1 ;
	            f_go = f_go && (rs1 >= 0) ;
	        }
	    } /* end if (non-zero positive) */
	    {
	        rs1 = cyimk_listend(op) ;
	        if (rs >= 0) rs = rs1 ;
	        f_go = f_go && (rs1 >= 0) ;
	    }
	    if ((rs >= 0) && f_go) {
	        rs1 = cyimk_renamefiles(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = cyimk_filesend(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    if (op->cname) {
	        voidp vp = voidp(op->cname) ;
	        rs1 = mem.free(vp) ;
	        if (rs >= 0) rs = rs1 ;
	        op->cname = nullptr ;
	    } /* end if (momory-release) */
	    {
	        rs1 = cyimk_idend(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = cyimk_dtor(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    op->magval = 0 ;
	} /* end if (cyimk_magic) */
	return (rs >= 0) ? n : rs ;
} /* end subroutine (cyimk_close) */

int cyimk_add(cyimk *op,cyimk_ent *bvp) noex {
	int		rs ;
	if ((rs = cyimk_magic(op,bvp)) >= 0) ylikely {
	    uint	li = UINT_MAX ;
	    if ((bvp->lines != nullptr) && (bvp->nlines > 0)) {
	        blentry	ble ;
	        cint	imax = UCHAR_MAX ;
	        for (int i = 0 ; (i < bvp->nlines) && (i < imax) ; i += 1) {
	            ble.loff = bvp->lines[i].loff ;
	            ble.llen = bvp->lines[i].llen ;
	            rs = vecobj_add(op->llp,&ble) ;
	            if (i == 0) li = rs ;
	            if (rs < 0) break ;
	        } /* end for */
	    } /* end if */
	    if (rs >= 0) ylikely {
	        bventry	bve ;
	        uint	citcmpval ;
	        bve.voff = bvp->voff ;
	        bve.vlen = bvp->vlen ;
	        bve.li = li ;
	        bve.hash = bvp->hash ;
	        mkcitation(&bve.citation,bvp) ;
	        citcmpval = (bve.citation & 0x0000FFFF) ;
	        if (citcmpval < op->pcitation) op->fl.notsorted = true ;
	        op->pcitation = citcmpval ;
	        rs = vecobj_add(op->vlp,&bve) ;
	        op->nentries += 1 ;
	    } /* end if (ok) */
	} /* end if (cyimk_magic) */
	return rs ;
} /* end subroutine (cyimk_add) */

int cyimk_abort(cyimk *op,int f) noex {
    	int		rs ;
	if ((rs = cyimk_magic(op)) >= 0) ylikely {
	    rs = SR_OK ;
	    op->fl.abort = !!f ;
	} /* end if (cyimk_magic) */
	return rs ;
} /* end subroutine (cyimk_abort) */


/* private subroutines */

local int cyimk_idbegin(cyimk *op,cchar *dname,int year) noex {
	int		rs ;
	int		rs1 ;
	if (ustat sb ; (rs = uc_stat(dname,&sb)) >= 0) ylikely {
	    cint	am = (W_OK|X_OK) ;
	    op->gid = sb.st_gid ;
	    op->uid = sb.st_uid ;
	    if (ids id ; (rs = id.load) >= 0) ylikely {
	        if ((rs = permids(&id,&sb,am)) >= 0) ylikely {
	            if (char *ydname ; (rs = mem.mp(&ydname)) >= 0) {
	                if ((rs = mkydname(ydname,dname,year)) >= 0) {
		            if ((rs = cyimk_idxdir(op,&id,ydname)) >= 0) {
	                        cchar	*cp ;
	                        if ((rs = mem.strw(ydname,-1,&cp)) >= 0) {
	                            op->idname = cp ;
	                        } /* end if (memory-acquire) */
		            } /* end if (cyimk_idxdir) */
	                } /* end if (mkydname) */
		        rs1 = mem.free(ydname) ;
		        if (rs >= 0) rs = rs1 ;
		    } /* end if (m-a-f) */
		} /* end if (permids) */
		rs1 = id.release ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (ids) */
	} /* end if (uc_stat) */
	return rs ;
} /* end subroutine (cyimk_idbegin) */

local int cyimk_idxdir(cyimk *op,ids *idp,cchar *ydname) noex {
	cint		rsn = SR_NOENT ;
	int		rs = SR_OK ;
	cmode		dm = 0777 ;
	if (ustat sb ; (rs = uc_stat(ydname,&sb)) >= 0) ylikely {
	    cint	am = (W_OK|X_OK) ;
	    uid_t	uid_yd = sb.st_uid ;
	    gid_t	gid_yd = sb.st_gid ;
	    if ((rs = permids(idp,&sb,am)) >= 0) {
		uid_t	uid = geteuid() ;
		if (uid == uid_yd) {
		    if ((uid_yd != op->uid) || (gid_yd != op->gid)) {
			rs = cyimk_minown(op,ydname,dm) ;
		    }
		}
	    } /* end if (permids) */
	} else if (rs == rsn) {
	    if ((rs = mkdirs(ydname,dm)) >= 0) {
		rs = cyimk_minown(op,ydname,dm) ;
	    } /* end if (mkdirs) */
	} /* end if */
	return rs ;
} /* end subroutine (cyimk_idxdir) */

local int cyimk_minown(cyimk *op,cchar *dname,mode_t dm) noex {
	int		rs ;
	if ((rs = uc_minmod(dname,dm)) >= 0) ylikely {
	    con gid_t	gid = op->gid ;
	    con uid_t	uid = op->uid ;
	    rs = uc_chown(dname,uid,gid) ;
	} /* end if */
	return rs ;
} /* end subroutine (cyimk_minown) */

local int cyimk_idend(cyimk *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (op->idname) {
	    voidp vp = voidp(op->idname) ;
	    rs1 = mem.free(vp) ;
	    if (rs >= 0) rs = rs1 ;
	    op->idname = nullptr ;
	} /* end if (memory-release) */
	return rs ;
} /* end subroutine (cyimk_idend) */

local int cyimk_filesbegin(cyimk *op) noex {
	int		rs = SR_OK ;
	int		c = 0 ;
	if (op->fl.ofcreat) {
	    rs = cyimk_filesbeginc(op) ;
	} else {
	    rs = cyimk_filesbeginwait(op) ;
	    c = rs ;
	} /* end if */
	return (rs >= 0) ? c : rs ;
} /* end subroutine (cyimk_filesbegin) */

local int cyimk_filesbeginc(cyimk *op) noex {
	cint		type = (op->fl.ofcreat && (! op->fl.ofexcl)) ;
	cint		maxpath = var.maxpathlen ;
	cint		sz = (3 * (var.maxpathlen + 1)) ;
	int		rs ;
	int		rs1 ;
	int		ai = 3 ; /* tripple allocation */
	if (char *a ; (rs = mem.mall(sz,&a)) >= 0) ylikely {
	    char	*dbn = (a + (--ai * (maxpath + 1))) ;
	    if ((rs = mkpath(dbn,op->idname,op->cname)) >= 0) ylikely {
	        cchar	*s_suf = FSUF_IDX	 ;
	        char	*tbuf = (a + (--ai * (maxpath + 1))) ;
	        if ((rs = mknewfname(tbuf,type,dbn,s_suf)) >= 0) ylikely {
	            mode_t	om = op->om ;
	            cchar	*tfn = tbuf ;
	            char	*rbuf = (a + (--ai * (maxpath + 1))) ;
	            if (type) {
	                if ((rs = mktmpfile(rbuf,tbuf,om)) >= 0) {
	                    op->fl.created = true ;
	                    tfn = rbuf ;
	                }
	            } /* end if (type) */
	            if (rs >= 0) ylikely {
	                int	of = O_CREAT ;
	                om = op->om ;
	                if (op->fl.ofexcl) of |= O_EXCL ;
	                rs = cyimk_filesbegcr(op,tfn,of,om) ;
	                if ((rs < 0) && type) {
	                    uc_unlink(rbuf) ;
	                } /* end if (error) */
	            } /* end if (ok) */
	        } /* end if (mknewfname) */
	    } /* end if (mkpath) */
	    rs1 = mem.free(a) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return rs ;
} /* end subroutine (cyimk_filesbeginc) */

local int cyimk_filesbeginwait(cyimk *op) noex {
    	cint		maxpath = var.maxpathlen ;
	cint		sz = (2 * (var.maxpathlen + 1)) ;
	cint		rsn = SR_EXISTS ;
	int		rs ;
	int		rs1 ;
	int		c = 0 ; /* return-value */
	int		ai = 2 ; /* double allocation */
	if (char *a ; (rs = mem.mall(sz,&a)) >= 0) ylikely {
	    char	*dbn = (a + (--ai * (maxpath + 1))) ;
	    if ((rs = mkpath(dbn,op->idname,op->cname)) >= 0) ylikely {
	        cchar	*suf = FSUF_IDX	 ;
	        char	*tbuf = (a + (--ai * (maxpath + 1))) ;
	        if ((rs = mknewfname(tbuf,false,dbn,suf)) >= 0) {
	            cmode	om = op->om ;
	            cint	to_stale = CYIMK_INTSTALE ;
	            cint	of = (O_CREAT|O_WRONLY|O_EXCL) ;
	            int	to = CYIMK_INTOPEN ;
	            while ((rs = cyimk_filesbegcr(op,tbuf,of,om)) == rsn) {
	                c = 1 ;
	                sleep(1) ;
	                unlinkstale(tbuf,to_stale) ;
	                if (to-- == 0) break ;
	            } /* end while (db exists) */
	            if (rs == rsn) {
	                op->fl.ofcreat = false ;
	                c = 0 ;
	                rs = cyimk_filesbeginc(op) ;
	            }
	        } /* end if (mknewfname) */
	    } /* end if (mkpath) */
	    rs1 = mem.free(a) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return (rs >= 0) ? c : rs ;
} /* end subroutine (cyimk_filesbeginwait) */

local int cyimk_filesbegcr(cyimk *op,cchar *tfn,int of,mode_t om) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = uc_open(tfn,of,om)) >= 0) ylikely {
	    cint	fd = rs ;
	    op->fl.created = true ;
	    if (cchar *cp ; (rs = mem.strw(tfn,-1,&cp)) >= 0) ylikely {
	        op->nidxfname = charp(cp) ;
	    } /* end if (memory-acquire) */
	    rs1 = u_close(fd) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (create) */
	return rs ;
} /* end subroutine (cyimk_filesbegcr) */

local int cyimk_filesend(cyimk *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (op->nfd >= 0) {
	    rs1 = u_close(op->nfd) ;
	    if (rs >= 0) rs = rs1 ;
	    op->nfd = -1 ;
	} /* end if */
	if (op->nidxfname) {
	    if (op->fl.created && (op->nidxfname[0] != '\0')) {
	        rs1 = u_unlink(op->nidxfname) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = mem.free(op->nidxfname) ;
	        if (rs >= 0) rs = rs1 ;
	        op->nidxfname = nullptr ;
	    } /* end if (memory-release) */
	} /* end if */
	return rs ;
} /* end subroutine (cyimk_filesend) */

local int cyimk_listbegin(cyimk *op,int vn) noex {
	cint		sz = szof(bventry) ;
	int		rs ;
	int		vo = 0 ;
	vo |= vecobjm.stationary ;
	vo |= vecobjm.ordered ;
	vo |= vecobjm.compact ;
	if ((rs = vecobj_start(op->vlp,sz,vn,vo)) >= 0) {
	    rs = vecobj_start(op->llp,sz,(vn * 2),vo) ;
	    if (rs < 0) {
	        vecobj_finish(op->vlp) ;
	    } /* end if (error) */
	}
	return rs ;
} /* end subroutine (cyimk_listbegin) */

local int cyimk_listend(cyimk *op) noex {
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
} /* end subroutine (cyimk_listend) */

local int cyimk_mkidx(cyimk *op) noex {
	int		rs ;
	int		rs1 ;
	int		wlen = 0 ;
	if ((rs = cyimk_nidxopen(op)) >= 0) ylikely {
	    custime dt = getustime ;
	    cyihdr	hdr{} ;
	    hdr.vetu[0] = uchar(CYIHDR_VERSION) ;
	    hdr.vetu[1] = uchar(ENDIAN) ;
	    hdr.vetu[2] = 0 ;
	    hdr.vetu[3] = 0 ;
	    hdr.wtime = uint(dt) ;
	    hdr.nentries = op->nentries ;
	    hdr.nskip = CYIMK_NSKIP ;
	    hdr.year = op->year ;
	    if ((rs = cyimk_mkidxmain(op,&hdr)) >= 0) ylikely {
	        cint	hlen = HDRBUFLEN ;
	        char	hbuf[HDRBUFLEN+1] ;
	        hdr.fsz = rs ;
	        wlen = rs ;
	        if ((rs = hdr.rd(hbuf,hlen)) >= 0) ylikely {
	            cint	bl = rs ;
	            if ((rs = u_writep(op->nfd,hbuf,bl,0z)) >= 0) ylikely {
	                cmode	om = op->om ;
	                rs = uc_fminmod(op->nfd,om) ;
	            }
	        } /* end if (cyihdr_rd) */
	    } /* end if (cyimk_mkidxmain) */
	    rs1 = cyimk_nidxclose(op) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (cyimk_nidx) */
	return (rs >= 0) ? wlen : rs ;
} /* end subroutine (cyimk_mkidx) */

local int cyimk_mkidxmain(cyimk *op,cyihdr *hdrp) noex {
	cint		nfd = op->nfd ;
	cint		ps = var.pagesz ;
	int		bsz ;
	int		rs ;
	int		rs1 ;
	int		off = 0 ;
	bsz = (ps * 4) ;
	filer		hf, *hfp = &hf ;
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
} /* end subroutine (cyimk_mkidxmain) */

/* ARGSUSED */
local int cyimk_mkidxhdr(cyimk *op,cyihdr *hdrp,filer *hfp) noex {
	int		rs ;
	int		wlen = 0 ; /* return-value */
	if (op) ylikely {
	    cint	hlen = HDRBUFLEN ;
	    char	hbuf[HDRBUFLEN+1] ;
	    if ((rs = hdrp->rd(hbuf,hlen)) >= 0) {
	        rs = filer_writefill(hfp,hbuf,rs) ;
	        wlen += rs ;
	    } /* end if (cyihdr_rd) */
	} /* end if (non-null) */
	return (rs >= 0) ? wlen : rs ;
} /* end subroutine (cyimk_mkidxhdr) */

local int cyimk_mkidxstrs(cyimk *op,cyihdr *hdrp,filer *hfp,int off) noex {
	int		rs ;
	int		rs1 ;
	int		wlen = 0 ; /* return-value */
	if (char *tbuf ; (rs = mem.mp(&tbuf)) >= 0) ylikely {
	    hdrp->diroff = off ;
	    if ((rs = pathclean(tbuf,op->idname,-1)) >= 0) {
	        mut int	tl = rs ;
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
	    rs1 = mem.free(tbuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return (rs >= 0) ? wlen : rs ;
} /* end subroutine (cyimk_mkidxstrs) */

local int cyimk_mkidxents(cyimk *op,cyihdr *hdrp,filer *hfp,int off) noex {
	vecobj		*elp = op->vlp ;
	uint		a[5] = {} ;
	cint		sz = (5 * szof(uint)) ;
	int		rs = SR_OK ;
	int		n = 0 ;
	int		wlen = 0 ; /* return-value */
	hdrp->vioff = off ;
	void *vp ;
	for (int i = 0 ; elp->get(i,&vp) >= 0 ; i += 1) {
	    if (bventry *bvep = resumelife<bventry>(vp) ; bvep) {
	        a[0] = bvep->voff ;
	        a[1] = bvep->vlen ;
	        a[2] = bvep->li ;
	        a[3] = bvep->citation ;
	        a[4] = bvep->hash ;
	        n += 1 ;
	        rs = filer_write(hfp,a,sz) ;
	        off += rs ;
	        wlen += rs ;
	    } /* end if (non-null) */
	    if (rs < 0) break ;
	} /* end for */
	hdrp->vilen = n ;
	return (rs >= 0) ? wlen : rs ;
} /* end subroutine (cyimk_mkidxents) */

local int cyimk_mkidxlines(cyimk *op,cyihdr *hdrp,filer *hfp,int off) noex {
	vecobj		*llp = op->llp ;
	uint		a[2] = {} ;
	cint		sz = (2 * szof(uint)) ;
	int		rs = SR_OK ;
	int		n = 0 ;
	int		wlen = 0 ; /* return-value */
	hdrp->vloff = off ;
	void *vp ;
	for (int i = 0 ; llp->get(i,&vp) >= 0 ; i += 1) {
	    if (blentry *blep = resumelife<blentry>(vp) ; blep) {
	        a[0] = blep->loff ;
	        a[1] = blep->llen ;
	        n += 1 ;
	        rs = filer_write(hfp,a,sz) ;
	        off += rs ;
	        wlen += rs ;
	    } /* end if (non-null) */
	    if (rs < 0) break ;
	} /* end for */
	hdrp->vllen = n ;
	return (rs >= 0) ? wlen : rs ;
} /* end subroutine (cyimk_mkidxlines) */

local int cyimk_nidxopen(cyimk *op) noex {
    	cint		maxpath = var.maxpathlen ;
	cint		sz = (2 * (var.maxpathlen + 1)) ;
	int		rs ;
	int		rs1 ;
	int		fd = -1 ;
	int		of = (O_CREAT|O_WRONLY) ;
	int		ai = 2 ; /* double allocation */
	cmode		om = op->om ;
	if (op->nidxfname == nullptr) {
	    cint	type = (op->fl.ofcreat && (! op->fl.ofexcl)) ;
	    cchar	*dbn = op->cname ;
	    cchar	*s_suf = FSUF_IDX ;
	    if (char *a ; (rs = mem.mall(sz,&a)) >= 0) ylikely {
	        char	*tbuf = (a + (--ai * (maxpath + 1))) ;
	        if ((rs = mknewfname(tbuf,type,dbn,s_suf)) >= 0) {
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
	return (rs >= 0) ? fd : rs ;
} /* end subroutine (cyimk_nidxopen) */

local int cyimk_nidxclose(cyimk *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (op->nfd >= 0) {
	    rs1 = u_close(op->nfd) ;
	    if (rs >= 0) rs = rs1 ;
	    op->nfd = -1 ;
	} /* end if */
	return rs ;
} /* end subroutine (cyimk_nidxclose) */

local int cyimk_renamefiles(cyimk *op) noex {
    	cint		maxpath = var.maxpathlen ;
    	cint		sz = (2 * (var.maxpathlen + 1)) ;
	int		rs ;
	int		rs1 ;
	int		ai = 0 ;
	cchar		*s_suf = FSUF_IDX ;
	cchar		*s_end = ENDIANSTR ;
	if (char *a ; (rs = mem.mall(sz,&a)) >= 0) ylikely {
	    char	*dbn = (a + (ai++ * (maxpath + 1))) ;
	    if ((rs = mkpath(dbn,op->idname,op->cname)) >= 0) {
	        char	*idxfname = (a + (ai++ * (maxpath + 1))) ;
	        if ((rs = mkfnamesuf(idxfname,dbn,s_suf,s_end)) >= 0) {
	            if ((rs = u_rename(op->nidxfname,idxfname)) >= 0) {
	                op->nidxfname[0] = '\0' ;
	            } else {
	                u_unlink(op->nidxfname) ;
	                op->nidxfname[0] = '\0' ;
	            }
	        } /* end if (mkfnamesuf) */
	    } /* end if (mkpath) */
	    rs1 = mem.free(a) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return rs ;
} /* end subroutine (cyimk_renamefiles) */

local int mkcitation(uint *cip,cyimk_ent *bvp) noex {
	int		rs = SR_BUGCHECK ;
	if (cip && bvp) ylikely {
	    uint nlines = (bvp->lines != nullptr) ? bvp->nlines : 0 ;
	    uint ci = 0 ;
	    rs = SR_OK ;
	    ci |= ((nlines & UCHAR_MAX) << 24) ;
	    ci |= ((bvp->m & UCHAR_MAX) << 8) ;
	    ci |= ((bvp->d & UCHAR_MAX) << 0) ;
	    *cip = ci ;
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (mkcitation) */

local int mkydname(char *rbuf,cchar *dname,int year) noex {
	cint		rlen = var.maxpathlen ;
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
} /* end subroutine (mkydname) */

local int mknewfname(char *tbuf,int type,cchar *dbn,cchar *suf) noex {
	cchar		*s_end = ENDIANSTR ;
	cchar		*s_fin = (type) ? "xXXXX" : "n" ;
	return mkfnamesuf3(tbuf,dbn,suf,s_end,s_fin) ;
} /* end subroutine (mknewfname) */

local int unlinkstale(cchar *fn,int to) noex {
	int		rs ;
	if (ustat sb ; (rs = uc_stat(fn,&sb)) >= 0) {
	    custime	dt = getustime ;
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

vars::operator int () noex {
    	int		rs ;
	if ((rs = ucpagesize) >= 0) ylikely {
	    pagesz = rs ;
	    if ((rs = bufsizeget(bufsize_mp)) >= 0) ylikely {
	        maxpathlen = rs ;
	    } /* end if */
	} /* end if (ucpagesize) */
	return rs ;
} /* end method (vars::operator) */

local int cmpve(con bventry *e1p,con bventry *e2p) noex {
	int		rc = 0 ;
	if (e1p || e2p) ylikely {
	    if (e1p) {
	        if (e2p) {
	            uint vc1 = e1p->citation & 0x0000FFFF ;
	            uint vc2 = e2p->citation & 0x0000FFFF ;
	            rc = intcmp(vc1,vc2) ;
	        } else {
	            rc = -1 ;
	        }
	    } else {
	        rc = +1 ;
	    }
	} /* end if */
	return rc ;
} /* end subroutine (cmpve) */

local int vcmpve(cvoid **v1pp,cvoid **v2pp) noex {
	con bventry	**e1pp = (con bventry **) v1pp ;
	con bventry	**e2pp = (con bventry **) v2pp ;
	int		rc = 0 ;
	if (e1pp && e2pp) ylikely {
	    rc = cmpve(*e1pp,*e2pp) ;
	}
	return rc ;
} /* end subroutine (vcmpve) */


