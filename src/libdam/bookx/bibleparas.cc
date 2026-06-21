/* bibleparas SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* BIBLEPARAS implementation */
/* version %I% last-modified %G% */

#define	CF_DEBUG	0		/* non-switchable debug print-outs */
#define	CF_EMPTYTERM	1		/* terminate entry on empty line */
#define	CF_STARTAUDIT	1		/* start w/ audit on BPI object */

/* revision history:

	- 2008-10-01, David A­D­ Morano
	This object module was originally written.

*/

/* Copyright © 2008 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	bibleparas

	Description:
	This module implements an interface (a trivial one) that
	allows access to the BIBLEPARAS datbase.

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
#include	<cstring>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<ucmem.h>		/* LIBUC */
#include	<ids.h>			/* LIBUC */
#include	<vecobj.h>		/* LIBUC */
#include	<vecstr.h>		/* LIBUC */
#include	<expcook.h>		/* LIBUC */
#include	<dirseen.h>		/* LIBUC */
#include	<mkdirs.h>		/* LIBUC */
#include	<sbuf.h>		/* LIBUC */
#include	<path.h>		/* LIBUC */
#include	<mkpathx.h>		/* LIBUC */
#include	<strn.h>		/* LIBUC */
#include	<snx.h>			/* LIBUC */
#include	<char.h>		/* LIBUC */
#include	<isnot.h>		/* LIBUC */
#include	<biblecite.h>		/* LIBDAM */
#include	<mkchar.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */
#include	<libdebug.h>		/* LIBDEBUG |DEBUGPRINTF(3debug)| */

#include	"bpi.h"
#include	"bpimk.h"
#include	"bibleparas.h"

#pragma		GCC dependency		"mod/libutil.ccm"
#pragma		GCC dependency		"mod/uconstants.ccm"

import libutil ;			/* |memclear(3u)| */
import uconstants ;

/* local defines */

#define	BIBLEPARAS_DBDNAME	"share/bibleparas"
#define	BIBLEPARAS_DBSUF	"txt"
#define	BIBLEPARAS_ENT		biblecite
#define	BIBLEPARAS_NLE		4	/* default number line entries */
#define	BIBLEPARAS_DIRMODE	0777
#define	BIBLEPARAS_IDXMODE	0664

#define	ISWHT(ch)	CHAR_ISWHITE(ch)

#ifndef	TMPVARDNAME
#define	TMPVARDNAME	"/var/tmp"
#endif

#define	INDCNAME	"bibleparas"
#define	INDSUF		"pi"

#define	TO_FILEMOD	(60 * 24 * 3600)
#define	TO_MKWAIT	(5 * 50)
#define	TO_CHECK	4

#define	VS		vecstr

#define	EC		expcook

#define	DS		dirseen
#define	DS_C		dirseen_cur

#define	BPAS		bibleparas
#define	BPAS_Q		bibleparas_q
#define	BPAS_C		bibleparas_cur
#define	BPAS_I		bibleparas_info
#define	BPAS_E		biblecite
#define	BPAS_MAG	BIBLEPARAS_MAGIC

#define	SI		subinfo
#define	SI_FL		subinfo_fl

#ifndef	CF_DEBUG
#define	CF_DEBUG	0		/* non-switchable debug print-outs */
#endif
#ifndef	CF_EMPTYTERM
#define	CF_EMPTYTERM	1		/* terminate entry on empty line */
#endif
#ifndef	CF_STARTAUDIT
#define	CF_STARTAUDIT	1		/* start w/ audit on BPI object */
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
	time_t		dt ;
	SI_FL		fl ;
} ; /* end struct */

namespace {
    struct vars {
	int	maxnamelen ;
	int	maxpathlen ;
	operator int () noex ;
    } ; /* end struct (vars) */
} /* end namespace */


/* forward references */

template<typename ... Args>
local int bibleparas_ctor(BPAS *op,Args ... args) noex {
	BIBLEPARAS	*hop = op ;
	cnullptr	np{} ;
	cnothrow	nt{} ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    memclear(hop) ;
	    rs = SR_NOMEM ;
	    if ((op->vindp = new(nt) bpi) != np) ylikely {
                rs = SR_OK ;
            } /* end if (new-bpi) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (bibleparas_ctor) */

local int bibleparas_dtor(BPAS *op) noex {
	int		rs = SR_FAULT ;
	if (op) ylikely {
	    rs = SR_OK ;
	    if (op->vindp) ylikely {
		delete op->vindp ;
		op->vindp = nullptr ;
	    } /* end if (memory-release) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (bibleparas_dtor) */

template<typename ... Args>
local inline int bibleparas_magic(BPAS *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    rs = (op->magval == BPAS_MAG) ? SR_OK : SR_NOTOPEN ;
	} /* end if */
	return rs ;
} /* end subroutine (bibleparas_magic) */

local int	bibleparas_dbldbeg	(BPAS *,SI *) noex ;
local int	bibleparas_dbldend	(BPAS *) noex ;
local int	bibleparas_dbmapcreate	(BPAS *,time_t) noex ;
local int	bibleparas_dbmapdestroy	(BPAS *) noex ;
local int	bibleparas_checkup	(BPAS *,time_t) noex ;
local int	bibleparas_indopen	(BPAS *,SI *) noex ;

local int	bibleparas_indopencheck	(BPAS *,cchar *) noex ;

local int	bibleparas_indclose	(BPAS *) noex ;
local int	bibleparas_indmk	(BPAS *,cchar *,time_t) noex ;
local int	bibleparas_indmkdata	(BPAS *,cchar *,mode_t) noex ;
local int	bibleparas_indopens	(BPAS *,SI *) noex ;
local int	bibleparas_indopenser	(BPAS *,SI *,DS *,VS *,EC *) noex ;
local int	bibleparas_loadcooks	(BPAS *,EC *) noex ;
local int	bibleparas_indopenalt	(BPAS *,SI *,DS *) noex ;

local int	subinfo_start		(SI *) noex ;
local int	subinfo_finish		(SI *) noex ;

local int	entry_start		(BPAS_E *,BPAS_Q *) noex ;
local int	entry_finish		(BPAS_E *) noex ;

local int	bvemk_start		(bpimk_v *,BPAS_E *) noex ;
local int	bvemk_finish		(bpimk_v *) noex ;

local int	mkdname			(cchar *,mode_t) noex ;
local int	checkdname		(cchar *) noex ;


/* local variables */

constexpr cpcchar	idxdirs[] = {
	"%R/var/%S",
	"/var/tmp/%{PRN}",
	"/var/tmp",
	"/tmp/%{PRN}",
	"/tmp",
	"%T",
	nullptr
} ; /* end array */

static vars		var ;
cbool			f_debug		= CF_DEBUG ;
cbool			f_startaudit	= CF_STARTAUDIT ;
cbool			f_emptyterm	= CF_EMPTYTERM ;


/* exported variables */

const bibleparas_obj	bibleparas_modinfo = {
	"bibleparas",
	szof(bibleparas),
	szof(bibleparas_cur)
} ; /* end initialization */


/* exported subroutines */

local int bibleparas_opens(BPAS *,cchar *,cchar *) noex ;

int bibleparas_open(BPAS *op,cchar *pr,cchar *dbn) noex {
	int		rs ;
	int		nv = 0 ; /* return-value */
	if ((dbn == nullptr) || (dbn[0] == '\0')) {
	    dbn = BIBLEPARAS_DBNAME ;
	}
	if ((rs = bibleparas_ctor(op,pr)) >= 0) ylikely {
	    rs = SR_INVALID ;
	    if (pr[0]) ylikely {
	        if (static cint rsv = var ; (rs = rsv) >= 0) {
		    op->pr	= pr ;
		    op->dbname	= dbn ;
		    rs = bibleparas_opens(op,pr,dbn) ;
		    nv = rs ;
		} /* end if (vars) */
	    } /* end if (valid) */
	    if (rs < 0) {
		bibleparas_dtor(op) ;
	    } /* end if (error) */
        } /* end if (bibleparas_ctor) */
	return (rs >= 0) ? nv : rs ;
} /* end subroutine (bibleparas_open) */

int bibleparas_opens(BPAS *op,cchar *pr,cchar *dbn) noex {
    	int		rs ;
	int		rs1 ;
	int		nv = 0 ; /* return-value */
	cchar		*suf = BIBLEPARAS_DBSUF ;
	if (char *cbuf ; (rs = mem.mn(&cbuf)) >= 0) ylikely {
	    cint clen = rs ;
	    if ((rs = snsds(cbuf,clen,dbn,suf)) >= 0) ylikely {
	        if (char *tbuf ; (rs = mem.mp(&tbuf)) >= 0) ylikely {
	            cchar	*dn = BIBLEPARAS_DBDNAME ;
	            if ((rs = mkpath(tbuf,pr,dn,cbuf)) >= 0) {
	                if (cchar *cp ; (rs = mem.strw(tbuf,rs,&cp)) >= 0) {
	    	            op->dbfname = cp ;
		            if (SI si ; (rs = subinfo_start(&si)) >= 0) {
	    	                if ((rs = bibleparas_dbldbeg(op,&si)) >= 0) {
			            nv = op->nverses ;
			            op->magval = BIBLEPARAS_MAGIC ;
	                        } /* end if */
	    	                rs1 = subinfo_finish(&si) ;
			        if (rs >= 0) rs = rs1 ;
		            } /* end if (subinfo) */
		            if (rs < 0) {
	        	        voidp vp = voidp(op->dbfname) ;
	        	        rs1 = mem.free(vp) ;
	                        op->dbfname = nullptr ;
		            } /* end if (error) */
	                } /* end if (m-a) */
	            } /* end if (mkpath) */
	    	    rs1 = mem.free(tbuf) ;
	    	    if (rs >= 0) rs = rs1 ;
	        } /* end if (m-a-f) */
	    } /* end if (snsds) */
	    rs1 = mem.free(cbuf) ;
	    if (rs >= 0) rs = rs1 ;
        } /* end if (m-a-f) */
	return (rs >= 0) ? nv : rs ;
} /* end subroutine (bibleparas_opens) */

int bibleparas_close(BPAS *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = bibleparas_magic(op)) >= 0) {
	    {
	        rs1 = bibleparas_dbldend(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    if (op->dbfname) {
	        voidp vp = voidp(op->dbfname) ;
	        rs1 = mem.free(vp) ;
	        if (rs >= 0) rs = rs1 ;
	        op->dbfname = nullptr ;
	    } /* end if (memory-release) */
	    {
		rs1 = bibleparas_dtor(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    op->magval = 0 ;
        } /* end if (biblebooks_magic) */
	return rs ;
} /* end subroutine (bibleparas_close) */

int bibleparas_count(BPAS *op) noex {
	int		rs ;
	if ((rs = bibleparas_magic(op)) >= 0) {
	    rs = op->nverses ;
        } /* end if (biblebooks_magic) */
	return rs ;
} /* end subroutine (bibleparas_count) */

int bibleparas_ispara(BPAS *op,con BPAS_Q *qp) noex {
	int		rs ;
	int		f = false ; /* return-value */
	if ((rs = bibleparas_magic(op,qp)) >= 0) {
	    /* check for update */
	    time_t	dt = 0 ;
	    if (op->ncursors == 0) {
	        rs = bibleparas_checkup(op,dt) ;
	    }
	    if (rs >= 0) {
	        BPI_Q	viq{} ;
	        viq.b = qp->b ;
	        viq.c = qp->c ;
	        viq.v = qp->v ;
	        if ((rs = bpi_get(op->vindp,&viq)) >= 0) {
	            f = true ;
	        } else if (rs == SR_NOTFOUND) 
	            rs = SR_OK ;
	    } /* end if (ok) */
        } /* end if (biblebooks_magic) */
	return (rs >= 0) ? f : rs ;
} /* end subroutine (bibleparas_ispara) */

int bibleparas_curbegin(BPAS *op,BPAS_C *curp) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = bibleparas_magic(op,curp)) >= 0) {
	    rs = SR_NOMEM ;
	    if (bpi_cur *bcurp = new(nt) bpi_cur ; bcurp) ylikely {
		if ((rs = bpi_curbegin(op->vindp,bcurp)) >= 0) {
		    curp->bicurp = bcurp ;
	    	    curp->magval = BPAS_MAG ;
	    	    op->ncursors += 1 ;
		} /* end if (bpi_curbegin) */
		if (rs < 0) {
		    delete bcurp ;
		} /* end if (error) */
	    } /* end if (new-bpi_cur) */
	    if (rs < 0) {
		memclear(curp) ;
	    } /* end if (error) */
        } /* end if (biblebooks_magic) */
	return rs ;
} /* end subroutine (bibleparas_curbegin) */

int bibleparas_curend(BPAS *op,BPAS_C *curp) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = bibleparas_magic(op,curp)) >= 0) {
	    rs = SR_NOTOPEN ;

	    {
	rs1 = bpi_curend(op->vindp,curp->vicurp) ;
	if (rs >= 0) rs = rs1 ;
	    }
	if (op->ncursors > 0) {
	    op->ncursors -= 1 ;
	}

        } /* end if (bibleparas_magic) */
	return rs ;
} /* end subroutine (bibleparas_curend) */

int bibleparas_curenum(BPAS *op,BPAS_C *curp,BPAS_Q *qp) noex {
	int		rs ;
	if ((rs = bibleparas_magic(op,curp,qp)) >= 0) {
	time_t		dt = 0 ;
	if (op->ncursors == 0) {
	    rs = bibleparas_checkup(op,dt) ;
	}
	if (rs >= 0) {
	    bpi_v	viv ;
	    if ((rs = bpi_curenum(op->vindp,curp->vicurp,&viv)) >= 0) {
	        if (qp) {
		    qp->b = viv.b ;
		    qp->c = viv.c ;
		    qp->v = viv.v ;
	        }
	    } /* end if (bpi_curenum) */
	} /* end if (ok) */
        } /* end if (bibleparas_magic) */
	DEBUGPRINTF("ret rs=%d\n", rs) ;
	return rs ;
} /* end subroutine (bibleparas_curenum) */

int bibleparas_getinfo(BPAS *op,BIBLEPARAS_INFO *ip) noex {
	int		rs ;
	int		nv = 0 ; /* return-value */
	if ((rs = bibleparas_magic(op)) >= 0) {
	    if (bpi_info bi ; (rs = bpi_getinfo(op->vindp,&bi)) >= 0) {
	        nv = bi.count ;
	        if (ip) {
	            memclear(ip) ;
	            ip->dbtime		= op->ti_db ;
	            ip->vitime		= op->ti_vind ;
	            ip->maxbook		= bi.maxbook ;
	            ip->maxchap		= bi.maxchap ;
	            ip->nverses		= bi.count ;
	            ip->nzverses	= bi.nzverses ;
	        } /* end if */
	    } /* end if (bpi_getinfo) */
        } /* end if (bibleparas_magic) */
	return (rs >= 0) ? nv : rs ;
} /* end subroutine (bibleparas_getinfo) */

int bibleparas_audit(BPAS *op) noex {
	int		rs ;
	if ((rs = bibleparas_magic(op)) >= 0) {
	    if (op->fl.vind) {
	        rs = bpi_audit(op->vindp) ;
	    }
        } /* end if (biblebooks_magic) */
	return rs ;
} /* end subroutine (bibleparas_audit) */


/* private subroutines */

local int bibleparas_dbldbeg(BPAS *op,SI *sip) noex {
	int		rs ;
	if ((rs = bibleparas_dbmapcreate(op,sip->dt)) >= 0) {
	    rs = bibleparas_indopen(op,sip) ;
	    if (rs < 0) {
		bibleparas_dbmapdestroy(op) ;
	    }
	}
	return rs ;
} /* end subroutine (bibleparas_dbldbeg) */

local int bibleparas_dbldend(BPAS *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	{
	    rs1 = bibleparas_indclose(op) ;
	    if (rs >= 0) rs = rs1 ;
	}
	{
	    rs1 = bibleparas_dbmapdestroy(op) ;
	    if (rs >= 0) rs = rs1 ;
	}
	return rs ;
} /* end subroutine (bibleparas_dbldend) */

local int bibleparas_dbmapcreate(BPAS *op,time_t dt) noex {
    	cnullptr	np{} ;
	int		rs ;
	int		rs1 ;
	DEBUGPRINTF("dbfname=%s\n",op->dbfname) ;
	if ((rs = u_open(op->dbfname,O_RDONLY,0666)) >= 0) {
	    cint	fd = rs ;
	    if (ustat sb ; (rs = u_fstat(fd,&sb)) >= 0) {
		csize fsize = size_t(sb.st_size) ;
	        if (S_ISREG(sb.st_mode)) {
	            if (sb.st_size <= INT_MAX) {
	                csize	ms = filesize ;
	                cint	mp = PROT_READ ;
	                cint	mf = MAP_SHARED ;
	                void	*md ;
			op->filesize = (fsize & INT_MAX) ;
			op->ti_db = sb.st_mtime ;
	    		if ((rs = u_mmap(np,ms,mp,mf,fd,0z,&md)) >= 0) {
			    cint	madv = MADV_RANDOM ;
			    const caddr_t	ma = md ;
	        	    if ((rs = u_madvise(ma,ms,madv)) >= 0) {
	            		op->mapdata = md ;
	            		op->mapsize = ms ;
	            		op->ti_map = dt ;
	            		op->ti_lastcheck = dt ;
			    } /* end if (u_madvise) */
	        	    if (rs < 0) {
		    	        u_munmap(md,ms) ;
	                        op->mapdata = nullptr ;
	                        op->mapsize = 0 ;
	                    } /* end if (error) */
	                } /* end if (u_mmap) */
	            } else {
	                rs = SR_TOOBIG ;
		    }
	        } else {
	            rs = SR_NOTSUP ;
		}
	    } /* end if (stat) */
	    rs1 = u_close(fd) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (file) */
	return rs ;
} /* end subroutine (bibleparas_dbmapcreate) */

local int bibleparas_dbmapdestroy(BPAS *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (op->mapdata) {
	    csize ms = op->mapsize ;
	    voidp md = op->mapdata ;
	    rs1 = u_munmap(md,ms) ;
	    if (rs >= 0) rs = rs1 ;
	    op->mapdata = nullptr ;
	    op->mapsize = 0 ;
	} /* end if (un-map) */
	return rs ;
} /* end subroutine (bibleparas_dbmapdestroy) */

local int bibleparas_checkup(BPAS *op,time_t dt) noex {
	int		rs = SR_OK ;
	int		f = false ;
	if (op->ncursors == 0) {
	    if (dt <= 0) dt = time(nullptr) ;
	    if ((dt - op->ti_lastcheck) >= TO_CHECK) {
	        op->ti_lastcheck = dt ;
		if (ustat sb ; (rs = u_stat(op->dbfname,&sb)) >= 0) {
	            f = f || (sb.st_mtime > op->ti_db) ;
	            f = f || (sb.st_mtime > op->ti_map) ;
	            if (f) {
	                bibleparas_dbldend(op) ;
		        if (SI si ; (rs = subinfo_start(&si)) >= 0) {
			    {
	                    rs = bibleparas_dbldbeg(op,&si) ;
			    }
		            subinfo_finish(&si) ;
	                } /* end if */
	            } /* end if (update) */
	        } else if (isNotPresent(rs)) {
	            rs = SR_OK ;
	        }
	    } /* end if (timed-out) */
	} /* end if (no cursors out) */
	return (rs >= 0) ? f : rs ;
} /* end subroutine (bibleparas_checkup) */

namespace {
    struct indopener {
        BPAS            *op ;
        SI              *sip ;
	char		*a ;
	char		*ebuf ;
	char		*tbuf ;
	int		elen ;
	int		tlen ;
        indopener(BPAS *o,SI *s) noex : op(o), sip(s) { 
	    a = nullptr ;
	} ;
        operator int () noex ;
        int opens       (DS *,EC *) noex ;
	int ph1(DS *,VS *,EC *) noex ;
	int ph2(DS *,VS *,EC *) noex ;
	int ph3(DS *,VS *,EC *) noex ;
    } ; /* end struct (indopener) */
} /* end namespace */

local int bibleparas_indopen(BPAS *op,SI *sip) noex {
	int		rs ;
	if ((rs = bibleparas_indopens(op,sip)) >= 0) {
	    if (op->fl.vind) {
	        rs = bpi_count(op->vindp) ;
	        op->nverses = rs ;
	    }
	}
	return rs ;
} /* end subroutine (bibleparas_indopen) */

intopener::operator int (sip) noex {
	int		rs ;
	int		rs1 ;
	if (DS ds ; (rs = ds.start) >= 0) {
	    cint	vn = 6 ;
	    cint	vo = vecstrm.compact ;
	    if (vecstr sdirs ; (rs = sdirs.start(vn,vo)) >= 0) {
	        if (EC ck ; (rs = ck.start) >= 0) {
	            if ((rs = bibleparas_loadcooks(op,&ck)) >= 0) {
	                rs = opens(&ds,&sdirs,&ck) ;
	            }
	            rs1 = ck.finish ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (cooks) */
		rs1 = sdirs.finish ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (sdirs) */
	    rs1 = ds.finish ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (dirseen) */
	return rs ;
} /* end method (indopener::operator) */

typedef int (indopener::*indopener_m)(DS *,VS *,EC *) noex ;

constexpr indopener_m	phases[] = {
    &indopener::ph1,
    &indopener::ph2,
    &indopener::ph3
} ; /* end array (methods) */

int indopener::opens(DS *dsp,VS *sdp,EC *ecp) noex {
    	cint		maxpath = var.maxpathlen ;
    	cint		psz = (2 * (var.maxpathlen + 1)) ;
	int		rs ;
	int		rs1 ;
	int		ai = 2 ; /* two path buffers */
	if ((rs = mem.mall(psz,&a)) >= 0) ylikely {
	    ebuf = (a + (--ai * (maxpath + 1))) ;
	    pbuf = (a + (--ai * (maxpath + 1))) ;
	    elen = maxpath ;
	    plen = maxpath ;
	    {
		for (cauto &m : phases) {
		    (this->&m)(dsp,sdp,ecp) ;
		    if (rs < 0) break ;
		} /* end for */
	    }
	    rs1 = mem.free(a) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return rs ;
} /* end method (indopener::opens) */

/* first phase: expand possible directory paths */
int indopener::ph1(DS *dsp,VS *sdp,EC *ecp) noex {
    	cint		rsn = SR_NOTFOUND ;
    	int		rs = SR_OK ;
	for (int i = 0 ; (rs >= 0) && idxdirs[i] ; i += 1) {
	    if ((rs = ecp->exp(0,ebuf,elen,idxdirs[i],-1)) > 0) {
		if ((rs = pathclean(pbuf,ebuf,rs)) > 0) {
		    cint pl = rs ;
		    if ((rs = dsp->havename(pbuf,pl)) == rsn) {
			rs = dsp->add(pbuf,pl,nullptr) ;
		    }
		}
	    }
	} /* end for */
	return rs ;
} /* end method (indopener::ph1) */

int indopener::ph2(DS *dsp,VS *sdp,EC *ecp) noex {
    	cint		rsn = SR_NOTFOUND ;
    	int		rs = SR_OK ;
	int		rs1 ;
	    if (DS_C cur ; (rs = dsp->curbegin(&cur)) >= 0) {
		int	el ;
	        while ((el = dsp->curenum(&cur,ebuf,elen)) >= 0) {
		    if ((rs = mkpath2(pbuf,ebuf,op->dbname)) >= 0) {
		        rs = sdp->add(pbuf,rs) ;
		    }
		    if (rs < 0) break ;
	        } /* end while */
		if ((rs >= 0) && (el != SR_NOTFOUND)) rs = el ;
	        rs1 = dsp->curend(&cur) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (dirseen-cur) */
	    return rs ;
} /* end method (indopener::ph2) */

int indopener::ph3(DS *dsp,VS *sdp,EC *ecp) noex {
    	int		rs ;
	int		rs1 ;
	if (mainv dv ; (rs = sdp->getvec(&dv)) >= 0) {
		for (int i = 0 ; dv[i] ; i += 1) {
		    rs = bibleparas_indopencheck(op,dv[i]) ;
		    if ((rs >= 0) || (! isNotPresent(rs))) break ;
		} /* end for */
	} /* end if (vecstr_getvex) */
	if ((rs < 0) && isNotPresent(rs)) {
	    rs = bibleparas_indopenalt(op,sip,dsp) ;
	} /* end if */
	return rs ;
} /* end method (indopener::ph3) */

local int bibleparas_loadcooks(BPAS *op,EC *ecp) noex {
	static cchar	*tmpdname = getenver(varname.tmpdir) ;
	int		rs = SR_OK ;
	cchar		*ks = "RST" ;
	char		kbuf[2] = {} ;
	if (tmpdname == nullptr) {
	    tmpdname = sysword.w_tmpdir ;
	}
	for (int i = 0 ; (rs >= 0) && (ks[i] != '\0') ; i += 1) {
	    cint	kch = MKCHAR(ks[i]) ;
	    cchar	*vap = nullptr ;
	    int		val = -1 ;
	    switch (kch) {
	    case 'R':
	        vap = op->pr ;
	        break ;
	    case 'S':
	        vap = INDCNAME ;
	        break ;
	    case 'T':
	        vap = tmpdname ;
	        break ;
	    } /* end switch */
	    if ((rs >= 0) && vap) {
	        kbuf[0] = kch ;
	        rs = ecp->add(kbuf,vap,val) ;
	    }
	} /* end for */
	if (rs >= 0) {
	    if (cchar *prname ; (rs = sfbasename(op->pr,-1,&prname)) >= 0) {
	        rs = SR_NOENT ;
	        if (prname) {
	            rs = ecp->add("PRN",prname,-1) ;
		}
	    } /* end if (sfbasename) */
	} /* end if (ok) */
	return rs ;
} /* end subroutine (bibleparas_loadcooks) */

local int bibleparas_indopenalt(BPAS *op,SI *sip,DS *dsp) noex {
	cint		maxpath = var.maxpathlen ;
    	cint		psz = (2 * (var.maxpathlen + 1)) ;
	int		rs ;
	int		rs1 ;
	int		ai = 2 ; /* two path buffers */
	DEBUGPRINTF("ent\n") ;
	if (char *a ; (rs = mem.mall(psz,&a)) >= 0) {
	    cint	elen = maxpath ;
	    char	*ebuf = (a + (--ai * (maxpath + 1))) ;
	    char	*tbuf = (a + (--ai * (maxpath + 1))) ;
	    if (DS_C cur ; (rs = dsp->curbegin(&cur)) >= 0) {
	        int	el ;
	        while ((el = dsp->curenum(&cur,ebuf,elen)) >= 0) {
	            if (el > 0) {
	                if ((rs = bibleparas_indmk(op,ebuf,sip->dt)) >= 0) {
		            if ((rs = mkpath(tbuf,ebuf,op->dbname)) >= 0) {
	                        rs = bpi_open(op->vindp,tbuf)  ;
	                        op->fl.vind = (rs >= 0) ;
		            }
	                }
		    }
		    if ((rs >= 0) || (! isNotPresent(rs))) break ;
	        } /* end while */
	        rs1 = dsp->curend(&cur) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (cursor) */
	    if_constexpr (f_startaudit) {
	        if ((rs >= 0) && op->fl.vind) {
	            rs = bpi_audit(op->vindp) ;
	        }
	    } /* end if_constexpr (f_startaudit) */
	    rs1 = mem.free(a) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	DEBUGPRINTF("ret rs=%d n",rs) ;
	return rs ;
} /* end subroutine (bibleparas_indopenalt) */

local int bibleparas_indopencheck(BPAS *op,cchar *dbname) noex {
	int		rs ;
	int		nv = 0 ; /* return-value */
	if ((rs = bpi_open(op->vindp,dbname)) >= 0) {
	    if (bpi_info binfo ; (rs = bpi_getinfo(op->vindp,&binfo)) >= 0) {
		if (binfo.ctime >= op->ti_db) {
		    op->fl.vind = true ;
		} else {
		    rs = SR_STALE ;
		}
	    } /* end if (bpi_getinfo) */
	    if (rs < 0) {
	        bpi_close(op->vindp) ;
	    } /* end if (error) */
	} /* end if (bpi_open) */
	return (rs >= 0) ? nv : rs ;
} /* end subroutine (bibleparas_indopencheck) */

local int bibleparas_indmk(BPAS *op,cchar dname[],time_t dt) noex {
	cmode		dm = BIBLEPARAS_DIRMODE ;
	int		rs ;
	int		c = 0 ; /* return-value */
	if ((rs = mkdname(dname,dm)) >= 0) {
	    cchar	*db = op->dbname ;
	    if (char *tbuf ; (rs = mem.mp(&tbuf)) >= 0) {
	        if ((rs = mkpath2(tbuf,dname,db)) >= 0) {
		    cmode	om = BIBLEPARAS_IDXMODE ;
		    if ((rs = bibleparas_indmkdata(op,tbuf,om)) >= 0) {
		        c += rs ;
	                op->ti_vind = dt ;
		    } /* end if (index-make) */
	        } /* end if (mkpath) */
	        rs1 = mem.free(tbuf) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (m-a-f) */
	} /* end if (mkdname) */
	return (rs >= 0) ? c : rs ;
} /* end subroutine (bibleparas_indmk) */

local int bibleparas_indmkdata(BPAS *op,cchar indname[],mode_t om) noex {
    	cnullptr	np{} ;
	cint		of = 0 ;
	int		rs ;
	int		rs1 ;
	int		c = 0 ; /* return-value */
	if (bpimk bpind ; (rs = bpimk_open(&bpind,indname,of,om)) >= 0) {
	    c = rs ;
	    if (rs == 0) {
	        bpimk_v	bve ;
	        BPAS_E	e ;
	        BPAS_Q	q ;
	        int	foff = 0 ;
	        int	ml = op->mapsize ;
	        bool	f_ent = false ;
	        cchar	*mp = op->mapdata ;
	        for (cc *tp ; (tp = strnchr(mp,ml,'\n')) != np ; ) {
	    	    cint len = ((tp + 1) - mp) ;
		    cchar *lp ;
		    if (int ll = sfcontent(mp,(len - 1),&lp) ; ll > 0) {
			DEBUGPRINTF("line>%r<\n", lp,strnlen(lp,MIN(ll,40))) ;
	                if ((rs = biblecite_ver(&q,lp,ll)) > 0) {
		            cint si = rs ;
	                    if (f_ent) {
	                        c += 1 ;
	                        if ((rs = bvemk_start(&bve,&e)) >= 0) {
	                            rs = bpimk_add(&bpind,&bve) ;
			            bvemk_finish(&bve) ;
		                }
			        f_ent = false ;
	                        entry_finish(&e) ;
	                    } /* end if (entry) */
	                    if (rs >= 0) {
	                        rs = entry_start(&e,&q) ;
			        if (rs >= 0) f_ent = true ;
		            } /* end if (ok) */
                        } else if (rs >= 0) {
                            if (f_ent) {
                                rs = entry_add(&e,foff,ll) ;
                            } /* end if (entry) */
	                } /* end if (biblecite_ver) */
                    } else {
                        if_constexpr (f_emptyterm) {
                            if (f_ent) {
                                c += 1 ;
                                if ((rs = bvemk_start(&bve,&e)) >= 0) {
                                    {
                                        rs = bpimk_add(&bpind,&bve) ;
                                    }
                                    rs1 = bvemk_finish(&bve) ;
                                    if (rs >= 0) rs = rs1 ;
                                } /* end if (bvemk) */
                                f_ent = false ;
                                entry_finish(&e) ;
                            } /* end if (entry) */
                        } /* end if_constexpr (f_emptyterm) */
	            } /* end if (not empty) */
	            foff += len ;
	            ml -= len ;
	            mp += len ;
	            if (rs < 0) break ;
	        } /* end while (readling lines) */
	        if ((rs >= 0) && f_ent) {
	            c += 1 ;
	            if ((rs = bvemk_start(&bve,&e)) >= 0) {
	                rs = bpimk_add(&bpind,&bve) ;
		        bvemk_finish(&bve) ;
	            }
	            f_ent = false ;
	            entry_finish(&e) ;
	        } /* end if (entry) */
	        if (f_ent) {
	            f_ent = false ;
	            entry_finish(&e) ;
	        } /* end if (entry) */
#if	CF_DEBUG
	        {
	            bpimk_info	bi ;
	            rs1 = bpimk_getinfo(&bpind,&bi) ;
	            DEBUGPRINTF("maxbook=%u\n", bi.maxbook) ;
	            DEBUGPRINTF("maxchap=%u\n", bi.maxchap) ;
	            DEBUGPRINTF("maxvers=%u\n", bi.maxvers) ;
	            DEBUGPRINTF("nverses=%u\n", bi.nverses) ;
	            DEBUGPRINTF("nzverses=%u\n", bi.nzverses) ;
	        }
#endif /* CF_DEBUG */
	    } /* end if (creation needed) */
	    rs1 = bpimk_close(&bpind) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (bpimk) */
	DEBUGPRINTF("ret rs=%d c=%u\n",rs,c) ;
	return (rs >= 0) ? c : rs ;
} /* end subroutine (bibleparas_indmkdata) */

local int bibleparas_indclose(BPAS *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (op->fl.vind) {
	    op->fl.vind = false ;
	    rs1 = bpi_close(op->vindp) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (close-index) */
	return rs ;
} /* end subroutine (bibleparas_indclose) */

local int subinfo_start(SI *sip) noex {
	int		rs = SR_BUGCHECK ;
	if (sip) {
	    rs = memclear(sip) ;
	    sip->dt = time(nullptr) ;
	} /* end if (bug-check) */
	return rs ;
} /* end subroutine (subinfo_start) */

local int subinfo_finish(SI *sip) noex {
	int		rs = SR_BUGCHECK ;
	if (sip) {
	    rs = SR_OK ;
	} /* end if (bug-check) */
	return rs ;
} /* end subroutine (subinfo_finish) */

local int entry_start(BPAS_E *ep,BPAS_Q *qp) noex {
	int		rs = SR_BUGCHECK ;
	if (ep) {
	    rs = memclear(ep) ;
	    ep->b = qp->b ;
	    ep->c = qp->c ;
	    ep->v = qp->v ;
	} /* end if (bug-check) */
	return rs ;
} /* end subroutine (entry_start) */

local int entry_finish(BPAS_E *ep) noex {
    	int		rs = SR_BUGCHECK ;
	if (ep) {
	    rs = SR_OK ;
	}
	return rs ;
} /* end subroutine (entry_finish) */

local int bvemk_start(bpimk_v *bvep,BPAS_E *ep) noex {
	int		rs = SR_BUGCHECK ;
	if (ep) {
	    rs = SR_OK ;
	    bvep->nlines = 0 ;
	    bvep->b = ep->b ;
	    bvep->c = ep->c ;
	    bvep->v = ep->v ;
	} /* end if (bug-check) */
	return rs ;
} /* end subroutine (bvemk_start) */

local int bvemk_finish(bpimk_v *bvep) noex {
    	int		rs = SR_BUGCHECK ;
	if (bvep) {
	    rs = SR_OK ;
	} /* end if (bug-check) */
	return rs ;
} /* end subroutine (bvemk_finish) */

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

local int mkdname(cchar *dname,mode_t dm) noex {
	cint		nrs = SR_NOENT ;
	int		rs ;
	if ((rs = checkdname(dname)) == nrs) {
	    rs = mkdirs(dname,dm) ;
	}
	return rs ;
} /* end subroutine (mkdname) */

local int checkdname(cchar *dname) noex {
	int		rs = SR_OK ;
	if (dname[0] == '/') {
	    if (ustat sb ; (rs = u_stat(dname,&sb)) >= 0) {
		rs = SR_NOTDIR ;
		if (S_ISDIR(sb.st_mode)) {
		    rs = perm(dname,-1,-1,nullptr,W_OK) ;
		} /* end if (is-dir) */
	    } /* end if (u_stat) */
	} else {
	    rs = SR_INVALID ;
	}
	return rs ;
} /* end subroutine (checkdname) */


