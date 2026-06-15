/* bibleverses SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* BIBLEVERSES implementation */
/* version %I% last-modified %G% */

#define	CF_DEBUG	0		/* non-switchable debug print-outs */
#define	CF_DEBUGTART	0		/* debug |sistart()| */
#define	CF_DEUGMKDATA	0		/* debug make-data */
#define	CF_EMPTYTERM	1		/* terminate entry on empty line */
#define	CF_SAFE		0		/* normal safety */

/* revision history:

	- 2008-10-01, David A­D­ Morano
	This object module was originally written.

*/

/* Copyright © 2008 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	bibleverses

	Description:
	This module implements the (actual) interface to the
	BIBLEVERSES datbase.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* POSIX */
#include	<sys/param.h>		/* POSIX */
#include	<sys/stat.h>		/* POSIX */
#include	<sys/mman.h>		/* POSIX */
#include	<ctime>			/* CSTD */
#include	<climits>		/* CSTD */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<ucmem.h>		/* LIBUC */
#include	<vecobj.h>		/* LIBUC */
#include	<sbuf.h>		/* LIBUC */
#include	<expcook.h>		/* LIBUC */
#include	<dirseen.h>		/* LIBUC */
#include	<ids.h>			/* LIBUC */
#include	<char.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */
#include	<libdebug.h>		/* LIBDEBUG |DEBUGPRINTF(3debug)| */

#include	"bvi.h"
#include	"bvimk.h"
#include	"bibleverses.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |memclear(3u)| */

/* local defines */

#define	BIBLEVERSES_DBDNAME	"share/bibledbs"
#define	BIBLEVERSES_DBTITLE	"Bible"
#define	BIBLEVERSES_DBSUF	"txt"
#define	BIBLEVERSES_ENT		bibleverses_entry
#define	BIBLEVERSES_EL		bibleverses_eline
#define	BIBLEVERSES_NLE		4	/* default number line entries */
#define	BIBLEVERSES_DIRMODE	0777
#define	BIBLEVERSES_IDXMODE	0664
#define	BIBLEVERSES_NLINES	20

#ifndef	VARTMPDNAME
#define	VARTMPDNAME	"TMPDIR"
#endif

#ifndef	TMPDNAME
#define	TMPDNAME	"/tmp"
#endif

#ifndef	VTMPDNAME
#define	VTMPDNAME	"/var/tmp"
#endif

#ifndef	VCNAME
#define	VCNAME		"var"
#endif

#define	INDDNAME	"bibleverses"
#define	INDNAME		"bibleverses"
#define	INDSUF		"vi"

#define	DS		dirseen

#define	EC		expcook

#define	BVS		bibleverses
#define	BVSS_ENT	bibleverses_entry
#define	BVSS_EL		bibleverses_eline
#define	BVSS_INFO	bibleverses_info
#define	BVSS_Q		bibleverses_q

#define	SI		subinfo
#define	SI_FL		subinfo_fl

#define	TO_FILEMOD	(60 * 24 * 3600)
#define	TO_MKWAIT	(5 * 50)
#define	TO_CHECK	4

#ifndef	CF_DEBUG
#define	CF_DEBUG	0		/* non-switchable debug print-outs */
#endif


/* imported namespaces */

using libuc::mem ;			/* variable */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

struct subinfo_fl {
	uint		dummy:1 ;
} ; /* end struct */

struct subinfo {
	SI_FL		fl ;
	time_t		dt ;
} ; /* end struct */

struct bibleverses_eline {
	uint		loff ;
	uint		llen ;
} ; /* end struct */

struct bibleverses_entry {
	BVSS_EL		*lines ;
	uint		voff ;
	uint		vlen ;
	int		i ;
	int		e ;
	uchar		b, c, v ;
} ; /* end if (bibleverses_entry) */

namespace {
    struct vars {
	int	maxnamelen ;
	int	maxpathlen ;
	operator int () noex ;
    } ; /* end struct (vars) */
} /* end namespace */


/* forward references */

template<typename ... Args>
local inline int bibleverses_ctor(bibleverses *op,Args ... args) noex {
    	cnullptr	np{} ;
	cnothrow	nt{} ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    rs = SR_NOMEM ;
	    op->pr	= np ;
	    op->dbname	= np ;
	    op->fname	= np ;
	    op->mapdata	= np ;
	    op->magval	= 0 ;
	    if ((op->bvip = new(nt) bvi) != np) {
		rs = SR_OK ;
	    } /* end if (new-bvi) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (bibleverses_ctor) */

local int bibleverses_dtor(bibleverses *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	    if (op->bvip) {
		delete op->bvip ;
		op->bvip = nullptr ;
	    } /* end if (memory-release) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (bibleverses_dtor) */

template<typename ... Args>
local inline int bibleverses_magic(bibleverses *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    rs = (op->magval == BIBLEVERSES_MAGIC) ? SR_OK : SR_NOTOPEN ;
	} /* end if */
	return rs ;
} /* end subroutine (bibleverses_magic) */

local int	bibleverses_dbloadbegin(BVSS *,SI *) noex ;
local int	bibleverses_dbloadend(BVSS *) noex ;
local int	bibleverses_dbmapcreate(BVSS *,time_t) noex ;
local int	bibleverses_dbmapdestroy(BVSS *) noex ;
local int	bibleverses_checkup(BVSS *,time_t) noex ;
local int	bibleverses_loadbuf(BVSS *,bvi_v *,char *,int) noex ;
local int	bibleverses_indopen(BVSS *,SI *) noex ;

local int	bibleverses_indclose(BVSS *) noex ;
local int	bibleverses_indmk(BVSS *,cchar *,time_t) noex ;
local int	bibleverses_indmkdata(BVSS *,cchar *,mode_t) noex ;
local int	bibleverses_indopenseq(BVSS *,SI *) noex ;
local int	bibleverses_indopenseqer(BVSS *,SI *,
			DS *,EC *) noex ;
local int	bibleverses_indopencheck(BVSS *,cchar *) noex ;
local int	bibleverses_indopenmk(BVSS *,SI *,cchar *) noex ;

local int	bibleverses_loadcooks(BVSS *,EC *) noex ;
local int	bibleverses_dirok(BVSS *,DS *,ids *,
			cchar *,int) noex ;
local int	bibleverses_mkdir(BVSS *,cchar *) noex ;

local int	subinfo_start(SI *) noex ;
local int	subinfo_finish(SI *) noex ;

local int	entry_start	(BVSS_ENT *,BVSS_Q *,uint,uint) noex ;
local int	entry_add	(BVSS_ENT *,uint,uint) noex ;
local int	entry_finish	(BVSS_ENT *) noex ;

local int	bvemk_start	(bvimk_v *,BVSS_ENT *) noex ;
local int	bvemk_finish	(bvimk_v *) noex ;

local int	mkdname		(cchar *,mode_t) noex ;
local int	checkdname	(cchar *) noex ;
local int	sistart		(cchar *,int,BVSS_Q *,int *) noex ;

local bool	isempty		(cchar *,int) noex ;
local bool	isNeedIndex	(int) noex ;


/* local variables */

constexpr cpcchar	idxdirs[] = {
	"/var/tmp/%{PRN}/%S",
	"/tmp/%{PRN}/%S",
	"%R/var/%S",
	"/var/tmp",
	"/tmp",
	"%T",
	nullptr
} ; /* end array */

constexpr int		rsneeds[] = {
	SR_STALE,
	0
} ; /* end array */

static vars		var ;
cbool			f_debug		= CF_DEBUG ;


/* exported variables */

extern const bibleverses_obj	bibleverses_modinfo = {
	"bibleverses",
	szof(bibleverses),
	szof(bibleverses_cur)
} ; /* end initialization */


/* exported variables */


/* exported subroutines */

local bibleverses_opener(bibleverses *op) noex {
     	int		rs  ;
	int		rs1 ;
	int		nv = 0 ; /* return-value */
	if (SI si ; (rs = subinfo_start(&si)) >= 0) {
	    if ((rs = bibleverses_dbloadbegin(op,&si)) >= 0) {
	        nv = op->nverses ;
		op->magval = BIBLEVERSES_MAGIC ;
	    }
	    rs1 = subinfo_finish(&si) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (subinfo) */
	return (rs >= 0) ? nv : rs ;
} /* end subroutine (bibleverses_opener) */

local int bibleverses_opens(BVSS *op,cc *pr,cc *dbn) noex {
    	int		rs ;
	int		rs1 ;
	int		nv = 0 ; /* return-value */
	cchar	*suf = BIBLEVERSES_DBSUF ;
	op->pr = pr ;
	op->dbname = dbn ;
	if (char *cbuf ; (rs = mem.mn(&cbuf)) >= 0) {
	    cint	clen = rs ;
	    if ((rs = snsds(cbuf,clen,dbn,suf)) >= 0) {
	        cchar	*dn = BIBLEVERSES_DBDNAME ;
		if (char *tbuf ; (rs = mem.mp(&tbuf)) >= 0) {
	            if ((rs = mkpath(tbuf,pr,dn,cbuf)) >= 0) {
	                if (cchar *cp ; (rs = mem.strw(tbuf,-1,&cp)) >= 0) {
	                    op->dbfname = cp ;
		            rs = bibleverses_opener(op) ;
		            nv = rs ;
	                    if (rs < 0) {
			        voidp vp = voidp(op->dbfname) ;
	                        mem.free(vp) ;
	                        op->dbfname = nullptr ;
	                    } /* end if (error) */
			} /* end if (memory-acquire) */
	    	    } /* end if (mkpath) */
		    rs1 = mem.free(tbuf) ;
		    if (rs >= 0) rs = rs1 ;
		} /* end if (m-a-f) */
	    } /* end if (snsds) */
	    rs1 = mem.free(cbuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return (rs >= 0) ? nv : rs ;
} /* end subroutine (bibleverses_opens) */

int bibleverses_open(BVSS *op,cchar *pr,cchar *dbn) noex {
	int		rs ;
	int		rs1 ;
	int		nv = 0 ; /* return-value */
	if ((dbn == nullptr) || (dbn[0] == '\0')) {
	    dbn = BIBLEVERSES_DBNAME ;
	} /* end if */
	if ((rs = bibleverse_magic(op,pr)) >= 0) {
	    rs = SR_INVALID ;
	    if (pr[0]) {
		rs = bibleverses_opens(op,pr,dbn) ;
		nv = rs ;
	    } /* end if (valid) */
	} /* end if (bpi_magic) */
	return (rs >= 0) ? nv : rs ;
} /* end subroutine (bibleverses_open) */

/* free up the entire vector string data structure object */
int bibleverses_close(BVSS *op) noex {
	int		rs ;
	int		rs1 ;
	DEBUGPRINTF("ent\n") ;
	if ((rs = bibleverses_magic(op)) >= 0) {
	    {
	        rs1 = bibleverses_dbloadend(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    if (op->dbfname) {
	        voidp vp = voidp(op->dbfname) ;
	        rs1 = mem.free(vp) ;
	        if (rs >= 0) rs = rs1 ;
	        op->dbfname = nullptr ;
	    } /* end if (memory-release) */
	    {
	        rs1 = bibleverses_dtor(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    op->magval = 0 ;
	} /* end if (bibleverses_magic) */
	return rs ;
} /* end subroutine (bibleverses_close) */

int bibleverses_count(BVSS *op) noex {
	int		rs ;
	if ((rs = bibleverses_magic(op)) >= 0) {
	    rs = op->nverses ;
	} /* end if (bibleverses_magic) */
	return rs ;
} /* end subroutine (bibleverses_count) */

int bibleverses_audit(BVSS *op) noex {
	int		rs ;
	if ((rs = bibleverses_magic(op)) >= 0) {
	    if (op->fl.vind) {
	        rs = bvi_audit(op->bvip) ;
	    }
	} /* end if (bibleverses_magic) */
	return rs ;
} /* end subroutine (bibleverses_audit) */

int bibleverses_read(BVSS *op,char *vbuf,int vlen,BVSS_Q *qp) noex {
	int		rs ;
	int		len = 0 ; /* return-value */
	if ((rs = bibleverses_magic(op,vbuf,qp)) >= 0) {
	    bvi_line	linea[BIBLEVERSES_NLINES + 1] ;
	    time_t	dt = 0 ;
	    cint	linen = BIBLEVERSES_NLINES ;
	    /* check for update */
	    if (op->ncursors == 0) {
	        rs = bibleverses_checkup(op,dt) ;
	    }
	    if (rs >= 0) {
	        bvi_q	viq{} ;
	        bvi_v	viv{} ;
	        cint	lsz = ((linen + 1) * szof(bvi_line)) ;
	        char	*lb = charp(linea) ;
	        viq.b = qp->b ;
	        viq.c = qp->c ;
	        viq.v = qp->v ;
	        if ((rs = bvi_read(op->bvip,&viv,lb,lsz,&viq)) >= 0) {
	            if (vbuf) {
	                rs = bibleverses_loadbuf(op,&viv,vbuf,vlen) ;
		        len = rs ;
	            }
	        } /* end if (bvi_read) */
	    } /* end if (ok) */
	} /* end if (bibleverses_magic) */
	return (rs >= 0) ? len : rs ;
} /* end subroutine (bibleverses_read) */

int bibleverses_get(BVSS *op,BVSS_Q *qp,char *vbuf,int vlen) noex {
	return bibleverses_read(op,vbuf,vlen,qp) ;
} /* end subroutine (bibleverses_get) */

int bibleverses_curbegin(BVSS *op,BVSS_CUR *curp) noex {
    	cnothrow	nt{} ;
	int		rs ;
	if ((rs = bibleverses_magic(op,curp)) >= 0) {
	    rs = SR_NOMEM ;
	    if (bvi_cur *vicurp = new(nt) bvi_cur ; vucurp) {
		curp->vicurp = vicurp ;
	        if ((rs = bvi_curbegin(op->bvip,vicurp)) >= 0) {
	            op->ncursors += 1 ;
	        }
		if (rs < 0) {
		    delete vicurp ;
		    curp->vucurp = nullptr ;
		} /* end if (new-bvi_cur) */
	    } /* end if (new-bvi_cur) */
	} /* end if (bibleverses_magic) */
	return rs ;
} /* end subroutine (bibleverses_curbegin) */

int bibleverses_curend(BVSS *op,BVSS_CUR *curp) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = bibleverses_magic(op,curp)) >= 0) {
	    rs = SR_NOTOPEN ;
	    if (bvi_cur *vicurp = curp->vicurp ; vicurp) {
		rs = SR_OK ;
	        {
	            rs1 = bvi_curend(op->bvip,vicurp) ;
	            if (rs >= 0) rs = rs1 ;
	        }
	        if (op->ncursors > 0) {
	            op->ncursors -= 1 ;
	        }
		{
		    delete vicurp ;
		    curp->cicurp = nullptr ;
		}
	    } /* end if (cursor non-null) */
	} /* end if (bibleverses_magic) */
	return rs ;
} /* end subroutine (bibleverses_curend) */

int bibleverses_curenum(BVSS *op,BVSS_CUR *curp,BVSS_Q *qp,
		char *vbuf,int vlen) noex {
	int		rs ;
	int		len = 0 ; /* return-value */
	if ((rs = bibleverses_magic(op,curp,qp,vbuf)) >= 0) {
	    bvi_line linea[BIBLEVERSES_NLINES + 1] ;
	    time_t	dt = 0 ;
	    cint     linen = ((BIBLEVERSES_NLINES + 1) * szof(bvi_line)) ;
	    rs = SR_NOTOPEN ;
	    if (bvi_cur *vicurp = curp->vicurp ; vicurp) {
		rs = SR_OK ;
	        if (op->ncursors == 0) {
	            rs = bibleverses_checkup(op,dt) ;
	        }
	        if (rs >= 0) {
	            bvi_cur	*bcurp = &curp->vicur ;
	            bvi_v	viv{} ;
	            cint	lsz = ((linen + 1) * szof(bvi_line)) ;
	            char	*lb = charp(linea) ;
	            if ((rs = bvi_curenum(op->bvip,bcurp,&viv,lb,lsz)) >= 0) {
	                if (vbuf) {
	                    rs = bibleverses_loadbuf(op,&viv,vbuf,vlen) ;
		            len = 0 ;
	                }
	                if ((rs >= 0) && qp) {
	                    qp->b = viv.b ;
	                    qp->c = viv.c ;
	                    qp->v = viv.v ;
	                } /* end if */
	            } /* end if (bvi_getenum) */
	        } /* end if (ok) */
	    } /* end if (cursor non-null) */
	} /* end if (bibleverses_magic) */
	return (rs >= 0) ? len : rs ;
} /* end subroutine (bibleverses_curenum) */

int bibleverses_getinfo(BVSS *op,BVSS_INFO *ip) noex {
	int		rs ;
	int		nv = 0 ; /* return-value */
	if ((rs = bibleverses_magic(op)) >= 0) {
	    if (bvi_info bi ; (rs = bvi_getinfo(op->bvip,&bi)) >= 0) {
	        nv = bi.count ;
	        if (ip) {
	            memclear(ip) ;
	            ip->dbtime		= op->ti_db ;
	            ip->vitime		= op->ti_vind ;
	            ip->maxbook		= bi.maxbook ;
	            ip->maxchapter	= bi.maxchapter ;
	            ip->nverses		= bi.count ;
	            ip->nzverses	= bi.nzverses ;
	        } /* end if (non-null) */
	    } /* end if (bvi_getinfo) */
	} /* end if (bibleverses_magic) */
	return (rs >= 0) ? nv : rs ;
} /* end subroutine (bibleverses_getinfo) */

/* retrieve a table w/ number of verses for each chapter of a book */
int bibleverses_chapters(BVSS *op,int book,uchar *ap,int al) noex {
	int		rs ;
	int		n = 0 ; /* return-value */
	if ((rs = bibleverses_magic(op,ap)) >= 0) {
	    rs = SR_INVALID ;
	    if (book >= 0) {
		rs = SR_OK ;
	        if (ap) {
	            if (op->ncursors == 0) {
	                rs = bibleverses_checkup(op,0L) ;
	            }
	            if (rs >= 0) {
	                rs = bvi_chapters(op->bvip,book,ap,al) ;
	                n = rs ;
	            } /* end if (ok) */
	        } /* end if (non-null) */
	    } /* end if (valid) */
	} /* end if (bibleverses_magic) */
	return (rs >= 0) ? n : rs ;
} /* end subroutine (bibleverses_chapters) */


/* private subroutines */

local int bibleverses_dbloadbegin(BVSS *op,SI *sip) noex {
	int		rs ;
	if ((rs = bibleverses_dbmapcreate(op,sip->dt)) >= 0) {
	    rs = bibleverses_indopen(op,sip) ;
	    if (rs < 0) {
	        bibleverses_dbmapdestroy(op) ;
	    } /* end if (error) */
	}
	return rs ;
} /* end subroutine (bibleverses_dbloadbegin) */

local int bibleverses_dbloadend(BVSS *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	{
	    rs1 = bibleverses_indclose(op) ;
	    if (rs >= 0) rs = rs1 ;
	}
	{
	    rs1 = bibleverses_dbmapdestroy(op) ;
	    if (rs >= 0) rs = rs1 ;
	}
	return rs ;
} /* end subroutine (bibleverses_dbloadend) */

local int bibleverses_dbmapcreate(BVSS *op,time_t dt) noex {
    	cnullptr	np{} ;
	int		rs ;
	inr		rs1 ;
	if ((rs = u_open(op->dbfname,O_RDONLY,0666)) >= 0) {
	    cint	fd = rs ;
	    if (ustat sb ; (rs = u_fstat(fd,&sb)) >= 0) {
	        if (S_ISREG(sb.st_mode)) {
	            if ((sb.st_size <= INT_MAX) && (sb.st_size > 0)) {
	                csize	ms = size_t(sb.st_size) ;
	                cint	mp = PROT_READ ;
	                cint	mf = MAP_SHARED ;
	                void	*md ;
	                op->filesize = ms ;
	                op->ti_db = sb.st_mtime ;
	                debugprintf("bibleverses_dbmapcreate: ms=%ld\n",ms) ;
	                if ((rs = u_mmapbegin(nl,ms,mp,mf,fd,0z,&md)) >= 0) {
	                    const caddr_t	ma = md ;
	                    cint		madv = MADV_RANDOM ;
	                    if ((rs = u_madvise(ma,ms,madv)) >= 0) {
	                        op->mapdata = md ;
	                        op->mapsize = ms ;
	                        op->ti_map = dt ;
	                        op->ti_lastcheck = dt ;
	                    } /* end if (map-advise) */
	                    if (rs < 0) {
	                        u_mmapend(md,ms) ;
	                        op->mapdata = nullptr ;
	                    } /* end if (error) */
	                } /* end if (u_mmap) */
	            } else {
	                rs = SR_TOOBIG ;
		    }
	        } else {
	            rs = SR_NOTSUP ;
		}
	    } /* end if (u_fstat) */
	    rs1 = u_close(fd) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (file) */
	DEBUGPRINTF("ret rs=%d\n",rs) ;
	return rs ;
} /* end subroutine (bibleverses_dbmapcreate) */

local int bibleverses_dbmapdestroy(BVSS *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (op->mapdata) {
	    voidp md = op->mapdata ;
	    csize ms = op->mapsize ;
	    rs1 = u_mmapend(md,ms) ;
	    if (rs >= 0) rs = rs1 ;
	    op->mapdata = nullptr ;
	    op->mapszie = 0 ;
	} /* end if (un-map) */
	return rs ;
} /* end subroutine (bibleverses_dbmapdestroy) */

local int bibleverses_checkup(BVSS *op,time_t dt) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	int		f = false ; /* return-value */
	if (op->ncursors == 0) {
	    if (dt <= 0) dt = time(nullptr) ;
	    if ((dt - op->ti_lastcheck) >= TO_CHECK) {
	        op->ti_lastcheck = dt ;
	        if (ustat sb ; (rs = u_stat(op->dbfname,&sb)) >= 0) {
	            f = f || (sb.st_mtime > op->ti_db) ;
	            f = f || (sb.st_mtime > op->ti_map) ;
	            if (f) {
	                bibleverses_dbloadend(op) ;
		        if (SI si ; (rs = subinfo_start(&si)) >= 0) {
			    {
	                    rs = bibleverses_dbloadbegin(op,&si) ;
			    }
	                    rs1 = subinfo_finish(&si) ;
		            if (rs >= 0) rs = rs1 ;
	                } /* end if */
	            } /* end if (update) */
	        } else if (isNotPresent(rs)) {
	            rs = SR_OK ;
	        } /* end if (stat) */
	    } /* end if (timed out) */
	} /* end if (no cursors out) */
	return (rs >= 0) ? f : rs ;
} /* end subroutine (bibleverses_checkup) */

local int bibleverses_loadbuf(BVSS *op,bvi_v *vivp,char *rbuf,int rlen) noex {
	int		rs ;
	int		len = 0 ; /* return-value */
	if (sbuf b ; (rs = sbuf_start(&b,rbuf,rlen)) >= 0) {
	    bvi_line	*lines = vivp->lines ;
	    cint	nlines = vivp->nlines ;
	    int		ll ;
	    cchar	*lp ;
	    for (int i = 0 ; i < nlines ; i += 1) {
	        if (i > 0) {
	            sbuf_chr(&b,' ') ;
		}
	        lp = (op->mapdata + lines[i].loff) ;
	        ll = lines[i].llen ;
	        rs = sbuf_strw(&b,lp,ll) ;
	        if (rs < 0) break ;
	    } /* end for */
	    len = sbuf_finish(&b) ;
	    if (rs >= 0) rs = len ;
	} /* end if (sbuf) */
	return (rs >= 0) ? len : rs ;
} /* end subroutine (bibleverses_loadbuf) */

local int bibleverses_indopen(BVSS *op,SI *sip) noex {
	int		rs ;
	if ((rs = bibleverses_indopenseq(op,sip)) >= 0) {
	    if (op->fl.vind) {
	        rs = bvi_count(op->bvip) ;
	        op->nverses = rs ;
	    }
	} /* end if (bibleverses_indopenseq) */
	return rs ;
} /* end subroutine (bibleverses_indopen) */

local int bibleverses_indopenseq(BVSS *op,SI *sip) noex {
	int		rs ;
	int		rs1 ;
	if (DS ds ; (rs = dirseen_start(&ds)) >= 0) {
	    EC cooks, *ckp = &cooks ;
	    if ((rs = expcook_start(ckp)) >= 0) {
	        if ((rs = bibleverses_loadcooks(op,ckp)) >= 0) {
	            rs = bibleverses_indopenseqer(op,sip,&ds,ckp) ;
	        }
	        rs1 = expcook_finish(ckp) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (cooks) */
	    rs1 = dirseen_finish(&ds) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (ds) */
	return rs ;
} /* end subroutines (bibleverses_indopenseq) */

local int bibleverses_indopenseqer(BVSS *op,SI *sip,
		DS *dsp,EC *ckp) noex {
	ids		id ;
	cint	elen = MAXPATHLEN ;
	int		rs ;
	int		rs1 ;
	int		nverses = 0 ;

#if	CF_DEBUG
	debugprintf("bibleverses_indopenseqer: ent\n") ;
#endif

/* first phase: expand possible directory paths */

	if ((rs = ids_load(&id)) >= 0) {
	    int		i ;
	    char	ebuf[MAXPATHLEN + 1] ;
	    char	pbuf[MAXPATHLEN + 1] ;
	    for (i = 0 ; (rs >= 0) && (idxdirs[i] != nullptr) ; i += 1) {
	        cchar	*dir = idxdirs[i] ;
#if	CF_DEBUG
	debugprintf("bibleverses_indopenseqer: dir=%s\n",dir) ;
#endif
	        if ((rs = expcook_exp(ckp,'\0',ebuf,elen,dir,-1)) >= 0) {
	            if ((rs = pathclean(pbuf,ebuf,rs)) > 0) {
		        if ((rs = bibleverses_dirok(op,dsp,&id,pbuf,rs)) > 0) {
	            	    rs = bibleverses_indopencheck(op,pbuf) ;
			    nverses = rs ;
			    if ((rs < 0) && isNeedIndex(rs)) {
			        rs = bibleverses_indopenmk(op,sip,pbuf) ;
			        nverses = rs ;
			    }
		        } /* end if (bibleverses_dirok) */
		    } /* end if (pathclean) */
	        } /* end if (expcook_exp) */
#if	CF_DEBUG
		debugprintf("bibleverses_indopenseqer: bot rs=%d nv=%u\n",
		rs,nverses) ;
#endif
		if (nverses > 0) break ;
	        if (rs < 0) break ;
	    } /* end for */
	    rs1 = ids_release(&id) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (ids) */

#if	CF_DEBUG
	debugprintf("bibleverses_indopenseqer: ret rs=%d nv=%u\n",rs,nverses) ;
#endif

	return (rs >= 0) ? nverses : rs ;
} /* end subroutines (bibleverses_indopenseqer) */

local int bibleverses_dirok(BVSS *op,DS *dsp,ids *idp,
		cchar *dp,int dl) noex {
	cint		rsn = SR_NOTFOUND ;
	int		rs ;
	int		f_ok = false ; /* return-value */
	if ((rs = dirseen_havename(dsp,dp,dl)) == rsn) {
	    if (ustat sb ; (rs = uc_stat(dp,&sb)) >= 0) {
		if ((rs = dirseen_havedevino(dsp,&sb)) == rsn) {
		    cint	am = (W_OK|R_OK|X_OK) ;
		    if ((rs = permid(idp,&sb,am)) >= 0) {
			f_ok = true ;
		    } else if (isNotPresent(rs)) {
			rs = dirseen_add(dsp,dp,dl,&sb) ;
		    }
		}
	    } else if (isNotPresent(rs)) {
		if ((rs = bibleverses_mkdir(op,dp)) > 0) {
		    f_ok = true ;
		}
	    }
	} /* end if (dirseen_havename) */
	return (rs >= 0) ? f_ok : rs ;
} /* end subroutine (bibleverses_dirok) */

local int bibleverses_mkdir(BVSS *op,cchar *dp) noex {
	int		rs ;
	int		f_ok = false ; /* return-value */
	cmode		dm = 0777 ;
	if ((rs = mkdirs(dp,dm)) >= 0) {
	     if ((rs = uc_minmod(dp,dm)) >= 0) {
		if ((rs = chownsame(dp,op->pr)) >= 0) {
	            f_ok = true ;
	        }
	    }
	} else if (isNotPresent(rs)) {
	    rs = SR_OK ;
	}
	return (rs >= 0) ? f_ok : rs ;
} /* end subroutine (bibleverses_mkdir) */

local int bibleverses_loadcooks(BVSS *op,EC *ecp) noex {
	int		rs = SR_OK ;
	int		kch ;
	int		vl ;
	cchar		*tmpdname = getenv(VARTMPDNAME) ;
	cchar		*ks = "RST" ;
	cchar		*vp ;
	char		kbuf[2] ;

	if (tmpdname == nullptr) tmpdname = TMPDNAME ;

	kbuf[1] = '\0' ;
	for (int i = 0 ; (rs >= 0) && (ks[i] != '\0') ; i += 1) {
	    kch = MKCHAR(ks[i]) ;
	    vp = nullptr ;
	    vl = -1 ;
	    switch (kch) {
	    case 'R':
	        vp = op->pr ;
	        break ;
	    case 'S':
	        vp = INDDNAME ;
	        break ;
	    case 'T':
	        vp = tmpdname ;
	        break ;
	    } /* end switch */
	    if ((rs >= 0) && vp) {
	        kbuf[0] = kch ;
	        rs = expcook_add(ecp,kbuf,vp,vl) ;
	    }
	} /* end for */
	if (rs >= 0) {
	    if (cchar *prname ; (rs = sfbasename(op->pr,-1,&prname)) >= 0) {
	        if (prname) {
	            rs = expcook_add(ecp,"PRN",prname,rs) ;
	        } else {
	            rs = SR_NOENT ;
		}
	    }
	} /* end if (ok) */

	return rs ;
} /* end subroutines (bibleverses_loadcooks) */

local int bibleverses_indopencheck(BVSS *op,cchar *idir) noex {
	int		rs ;
	int		nverses = 0 ;
	cchar		*db = op->dbname ;
	char		dbname[MAXPATHLEN+1] ;

#if	CF_DEBUG
	debugprintf("bibleverses_indopencheck: ent idir=%s\n",idir) ;
#endif

	if ((rs = mkpath2(dbname,idir,db)) >= 0) {
	    if ((rs = bvi_open(op->bvip,dbname)) >= 0) {
	        bvi_info	binfo ;
	        nverses = rs ;

#if	CF_DEBUG
	    debugprintf("bibleverses_indopencheck: bvi_open() rs=%d\n",
		rs) ;
#endif

	        if ((rs = bvi_getinfo(op->bvip,&binfo)) >= 0) {
	            if (binfo.ctime < op->ti_db) {
	                rs = SR_STALE ;
	            } else {
	                op->fl.vind = true ;
	            }
	            if (rs < 0) {
	                bvi_close(op->bvip) ;
		    }
	        } /* end if (bvi_getinfo) */
	    } /* end if (bvi_open) */
	} /* end if (mkpath) */

#if	CF_DEBUG
	debugprintf("bibleverses_indopencheck: ret rs=%d nv=%u\n",
	    rs,nverses) ;
#endif

	return (rs >= 0) ? nverses : rs ;
} /* end subroutine (bibleverses_indopencheck) */

local int bibleverses_indopenmk(BVSS *op,SI *sip,cchar *idir) noex {
	int		rs ;
	int		nverses = 0 ;

	if ((rs = bibleverses_indmk(op,idir,sip->dt)) >= 0) {
	    char	tbuf[MAXPATHLEN+1] ;
	    if ((rs = mkpath2(tbuf,idir,op->dbname)) >= 0) {
		if ((rs = bvi_open(op->bvip,tbuf)) >= 0) {
	            nverses = rs ;
		    op->fl.vind = true ;
		}
	    }
	}

	return (rs >= 0) ? nverses : rs ;
} /* end subroutines (bibleverses_indopenmk) */

local int bibleverses_indmk(BVSS *op,cchar *dname,time_t dt) noex {
	int		rs ;
	int		c = 0 ;
	cmode	dm = BIBLEVERSES_DIRMODE ;

	if ((rs = mkdname(dname,dm)) >= 0) {
	    char	indname[MAXPATHLEN + 1] ;
	    if ((rs = mkpath2(indname,dname,op->dbname)) >= 0) {
	        cmode	om = BIBLEVERSES_IDXMODE ;
	        if ((rs = bibleverses_indmkdata(op,indname,om)) >= 0) {
	            c += rs ;
	            op->ti_vind = dt ;
	        }
	    } /* end if (mkpath) */
	} /* end if (mkdname) */

#if	CF_DEBUG
	debugprintf("bibleverses_indmk: ret rs=%d c=%u\n",rs,c) ;
#endif

	return (rs >= 0) ? c : rs ;
} /* end subroutine (bibleverses_indmk) */

local int bibleverses_indmkdata(BVSS *op,cchar *indname,mode_t om) noex {
	BVIMK		bvind ;
	bvimk_v	bve ;
	int		rs ;
	int		rs1 ;
	int		of = 0 ;
	int		c = 0 ;

#if	CF_DEBUG
	debugprintf("bibleverses_indmkdata: ent indname=%s\n",indname) ;
#endif

	if ((rs = bvimk_open(&bvind,indname,of,om)) >= 0) {
	    c = rs ;
	    if (rs == 0) {
	        BVSS_ENT	e ;
	        BVSS_Q	q ;
	        int		foff = 0 ;
	        int		len ;
	        int		ml = (op->filesize & INT_MAX) ;
	        int		ll ;
	        int		si ;
	        int		f_ent = false ;
	        cchar		*mp = op->mapdata ;
	        cchar		*tp, *lp ;

	        while ((tp = strnchr(mp,ml,'\n')) != nullptr) {

	            len = ((tp + 1) - mp) ;
	            lp = mp ;
	            ll = (len - 1) ;

	            if ((ll > 0) && (! isempty(lp,ll))) {

#if	CF_DEBUG && CF_DEBUGMKDATA
	                debugprintf("bibleverses_indmkdata: line>%r<\n",
	                    lp,strnlen(lp,MIN(ll,40))) ;
#endif

	                if ((tp = strnchr(lp,ll,'#')) != nullptr) {
	                    ll = (tp - lp) ;
	                }

	                if ((rs = sistart(lp,ll,&q,&si)) > 0) {

	                    if (f_ent) {
	                        c += 1 ;
	                        if ((rs = bvemk_start(&bve,&e)) >= 0) {
	                            rs = bvimk_add(&bvind,&bve) ;
	                            bvemk_finish(&bve) ;
	                        }
	                        f_ent = false ;
	                        entry_finish(&e) ;
	                    }

	                    if (rs >= 0) {
	                        rs = entry_start(&e,&q,(foff + si),(ll - si)) ;
	                        if (rs >= 0) {
	                            f_ent = true ;
	                        }
	                    }

	                } else if (rs >= 0) {

	                    if (f_ent) {
	                        rs = entry_add(&e,foff,ll) ;
	                    }

	                } /* end if (entry start of add) */

	            } else {

#if	CF_EMPTYTERM
	                if (f_ent) {
	                    c += 1 ;
	                    if ((rs = bvemk_start(&bve,&e)) >= 0) {
	                        rs = bvimk_add(&bvind,&bve) ;
	                        bvemk_finish(&bve) ;
	                    }
	                    f_ent = false ;
	                    entry_finish(&e) ;
	                }
#else
	                rs = SR_OK ;
#endif /* CF_EMPTYTERM */

	            } /* end if (not empty) */

	            foff += len ;
	            ml -= len ;
	            mp += len ;

	            if (rs < 0) break ;
	        } /* end while (readling lines) */

	        if ((rs >= 0) && f_ent) {
	            c += 1 ;
	            if ((rs = bvemk_start(&bve,&e)) >= 0) {
	                rs = bvimk_add(&bvind,&bve) ;
	                bvemk_finish(&bve) ;
	            }
	            f_ent = false ;
	            entry_finish(&e) ;
	        }

	        if (f_ent) {
	            f_ent = false ;
	            entry_finish(&e) ;
	        }

#if	CF_DEBUG && CF_DEBUGMKDATA
	        {
	            BVIMK_INFO	bi ;
	            rs1 = bvimk_info(&bvind,&bi) ;
	            debugprintf("bibleverses_indmkdata: maxbook=%u\n",
	                bi.maxbook) ;
	            debugprintf("bibleverses_indmkdata: maxchapter=%u\n",
	                bi.maxchapter) ;
	            debugprintf("bibleverses_indmkdata: maxverse=%u\n",
	                bi.maxverse) ;
	            debugprintf("bibleverses_indmkdata: nverses=%u\n",
	                bi.nverses) ;
	            debugprintf("bibleverses_indmkdata: nzverses=%u\n",
	                bi.nzverses) ;
	        }
#endif /* CF_DEBUG */

	    } /* end if (creation needed) */
	    rs1 = bvimk_close(&bvind) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (bvimk) */

#if	CF_DEBUG
	debugprintf("bibleverses_indmkdata: ret rs=%d c=%u\n",rs,c) ;
#endif

	return (rs >= 0) ? c : rs ;
} /* end subroutine (bibleverses_indmkdata) */

local int bibleverses_indclose(BVSS *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (op->fl.vind) {
	    op->fl.vind = false ;
	    rs1 = bvi_close(op->bvip) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if */
	return rs ;
} /* end subroutine (bibleverses_indclose) */

local int subinfo_start(SI *sip) noex {
	int		rs = SR_BUGCHECK ;
	int		rs1 ;
	if (sip) {
	    rs = memclear(sip) ;
	    sip->dt = time(nullptr) ;
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (subinfo_start) */

local int subinfo_finish(SI *sip) noex {
    	int		rs = SR_FAULT ;
	if (sip) {
	    rs = SR_OK ;
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (subinfo_finish) */

local int entry_start(BVSS_ENT *ep,BVSS_Q *qp,uint loff,uint llen) noex {
	cint		ne = BIBLEVERSES_NLE ;
	int		rs = SR_BUGCHECK ;
	if (ep && qp) {
	    cint	sz = (ne * szof(BVSS_EL)) ;
	    memclear(ep) ;
	    ep->b = qp->b ;
	    ep->c = qp->c ;
	    ep->v = qp->v ;
	    ep->voff = loff ;
	    ep->vlen = llen ;
	    if (void *p ; (rs = mem.mall(sz,&p)) >= 0) {
	        ep->lines = resumelife<BVSS_EL>(p) ;
	        ep->e = ne ;
	        {
	            BVSS_EL *elp = p ;
	            ep->i += 1 ;
	            elp->loff = loff ;
	            elp->llen = llen ;
	        } /* end block (first line) */
	    } /* end if (memory-qcquire) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (entry_start) */

local int entry_add(BVSS_ENT *ep,uint loff,uint llen) {
	int		rs = SR_OK ;
	int		ne ;
	int		sz ;

	if (ep == nullptr) return SR_FAULT ;

	if (ep->e <= 0) return SR_NOTOPEN ;

	if ((ep->i < 0) || (ep->i > ep->e)) return SR_BADFMT ;
	BVSS_EL	*elp ;

	if (ep->i == ep->e) {
	    ne = ep->e + BIBLEVERSES_NLE ;
	    sz = (ne * szof(BVSS_EL)) ;
	    if ((rs = uc_realloc(ep->lines,sz,&elp)) >= 0) {
	        ep->lines = elp ;
	        ep->e = ne ;
	    }
	} /* end if */
	if (rs >= 0) {
	    ep->vlen = ((loff + llen) - ep->voff) ;
	    elp = (ep->lines + ep->i) ;
	    elp->loff = loff ;
	    elp->llen = llen ;
	    ep->i += 1 ;
	} /* end if (ok) */

	return rs ;
} /* end subroutine (entry_add) */

local int entry_finish(BVSS_ENT *ep) {
	int		rs = SR_BUGCHECK ;
	int		rs1 ;
	if (ep) {
	    rs = SR_NOTOPEN ;
	    if (ep->e > 0) {
	        rs = SR_BADFMT ;
		if ((ep->i >= 0) && (ep->i <= ep->e)) {
		    rs = SR_OK ;
	            if (ep->lines) {
	                rs1 = mem.free(ep->lines) ;
	                if (rs >= 0) rs = rs1 ;
	                ep->lines = nullptr ;
	            } /* end if (memory-release) */
	            ep->i = 0 ;
	            ep->e = 0 ;
	        } /* end if (cursor-valid) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (entry_finish) */

local int bvemk_start(bvimk_v *bvep,BVSS_ENT *ep) {
	int		rs = SR_BUGCHECK ;
	int		nlines = 0 ; /* return-value */
	if (bvep && ep) {
	    rs = SR_OK ;
	    bvep->b	= ep->b ;
	    bvep->c	= ep->c ;
	    bvep->v	= ep->v ;
	    bvep->voff	= ep->voff ;
	    bvep->vlen	= ep->vlen ;
	    bvep->lines	= nullptr ;
	    nlines = ep->i ;
	    if (nlines <= UCHAR_MAX) {
	        cint	sz = (nlines + 1) * szof(bvimk_line) ;
	        bvep->nlines = nlines ;
	        if (void *vp ; (rs = mem.mall(sz,&vp)) >= 0) {
	            bvimk_line *lines = resumelife<vbimk_line>(vp) ;
	            int	i ; /* used-afterwards */
	            bvep->lines = lines ;
	            for (i = 0 ; i < nlines ; i += 1) {
	                lines[i].loff = ep->lines[i].loff ;
	                lines[i].llen = ep->lines[i].llen ;
	            } /* end for */
	            lines[i].loff = 0 ;
	            lines[i].llen = 0 ;
	        } /* end if (memory-acquire) */
	    } else {
	        rs = SR_TOOBIG ;
	    }
	} /* end if (non-null) */
	return (rs >= 0) ? nlines : rs ;
} /* end subroutine (bvemk_start) */

local int bvemk_finish(bvimk_v *bvep) noex {
	int		rs = SR_BUGCHECK ;
	int		rs1 ;
	if (bvep) {
	    rs = SR_OK ;
	    if (bvep->lines) {
	        rs1 = mem.free(bvep->lines) ;
	        if (rs >= 0) rs = rs1 ;
	        bvep->lines = nullptr ;
	    } /* end if (memory-release) */
	} /* end if (buf-check) */
	return rs ;
} /* end subroutine (bvemk_finish) */

local int mkdname(cchar *dname,mode_t dm) {
	cint		nrs = SR_NOENT ;
	int		rs ;
	if ((rs = checkdname(dname)) == nrs) {
	    rs = mkdirs(dname,dm) ;
	}
	return rs ;
} /* end subroutine (mkdname) */

local int checkdname(cchar *dname) {
	int		rs = SR_INVALID ;
	if (dname[0] == '/') {
	    if (ustat sb ; (rs = u_stat(dname,&sb)) >= 0) {
		rs = SR_NOTDIR ;
	        if (S_ISREG(sb.st_mode))  {
	            rs = perm(dname,-1,-1,nullptr,W_OK) ;
	        }
	    } /* end if (stat) */
	} /* end if (valid) */
	return rs ;
} /* end subroutine (checkdname) */

vars::operator int () noex {
    	int		rs ;
	if ((rs = bufsizeget(bufsize_mn)) >= 0) {
	    maxnamelen = rs ;
	    if ((rs = bufsizeget(bufsize_mp)) >= 0) {
	        maxpathlen = rs ;
	    } /* end if (bufsizeget) */
	} /* end if (bufsizeget) */
    	return rs ;
} /* end if (vars::operator) */

local int sistart(cchar *lp,int ll,BVSS_Q *qp,int sip) noex {
	int		rs1 ;
	int		sl = ll ;
	int		ch ;
	int		si = 0 ;
	bool		f = false ;
	cchar	*sp = lp ;

#if	CF_DEBUG && CF_DEBUGTART
	debugprintf("bibleqs/isstart: ent l=>%r<\n",lp,
	    strlinelen(lp,ll,40)) ;
#endif

	if (CHAR_ISWHITE(sp[0]) && ((si = siskipwhite(lp,ll)) > 0)) {
	    sp += si ;
	    sl -= si ;
	}

	ch = MKCHAR(sp[0]) ;
	if ((sl >= 5) && isdigitlatin(ch)) {
	    int		i ;
	    int		cl ;
	    int		v ;
	    cchar	*tp, *cp ;

	    for (i = 0 ; i < 3 ; i += 1) {

	        cp = sp ;
	        cl = sl ;
	        if ((tp = strnbrk(sp,sl,": \t\n")) != nullptr) {
	            cl = (tp - sp) ;
	            sl -= ((tp + 1) - sp) ;
	            sp = (tp + 1) ;
	        } else {
	            cl = sl ;
	            sp += sl ;
	            sl = 0 ;
	        }

	        if (cl == 0)
	            break ;

	        si = ((cp + cl) - lp) ;
	        rs1 = cfdeci(cp,cl,&v) ;

#if	CF_DEBUG && CF_DEBUGTART
	        debugprintf("bibleqs/isstart: cfdeci() rs=%d\n",rs1) ;
#endif

	        if (rs1 < 0)
	            break ;

	        switch (i) {
	        case 0:
	            qp->b = (uchar) v ;
	            break ;
	        case 1:
	            qp->c = (uchar) v ;
	            break ;
	        case 2:
	            qp->v = (uchar) v ;
	            break ;
	        } /* end switch */

	    } /* end for */

	    f = (i == 3) ;
	    if (f) {
	        si += siskipwhite(sp,sl) ;
	    }

	} /* end if (have a start) */

	if (sip != nullptr) {
	    *sip = (f) ? si : 0 ;
	}

#if	CF_DEBUG && CF_DEBUGTART
	debugprintf("bibleqs/isstart: f=%u si=%u\n",f,si) ;
#endif

	return (f) ? si : 0  ;
} /* end subroutine (sistart) */

local bool isempty(cchar *lp,int ll) noex {
	bool		f = false ;
	f = f || (ll == 0) ;
	f = f || (lp[0] == '#') ;
	if ((! f) && CHAR_ISWHITE(*lp)) {
	    cchar	*cp ;
	    cint cl = sfskipwhite(lp,ll,&cp) ;
	    f = f || (cl == 0) ;
	    f = f || (cp[0] == '#') ;
	} /* end if */
	return f ;
} /* end subroutine (isempty) */

local bool isNeedIndex(int rs) noex {
	bool		f = false ;
	f = f || isOneOf(rsneeds,rs) ;
	f = f || isNotPresent(rs) ;
	return f ;
} /* end subroutine (isNeedIndex) */


