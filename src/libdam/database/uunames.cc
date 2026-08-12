/* uunames SUPPORT (UU-Names) */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* uunames-query database manager */
/* version %I% last-modified %G% */

#define	CF_DEBUG	0		/* debugging */
#define	CF_GETPROGROOT	1		/* use |getprogroot(3dam)| */

/* revision history:

	= 1998-02-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/******************************************************************************

  	Object:
	uunames

	Description:
	This little object provides access to the UUNAMES database
	and index (if any).

******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/param.h>		/* POSIX® */
#include	<sys/stat.h>		/* POSIX® */
#include	<sys/mman.h>		/* POSIX® */
#include	<ctime>			/* CSTD */
#include	<climits>		/* CSTD |INT_MAX| */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstring>		/* CSTD |strncmp(3c)| */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<endian.h>		/* LIBU */
#include	<uclibmem.h>		/* LIBUC */
#include	<ucopen.h>		/* LIBUC */
#include	<ucdesc.h>		/* LIBUC */
#include	<getprogroot.h>		/* LIBUC */
#include	<bufsizeget.h>		/* LIBUC */
#include	<vecstr.h>		/* LIBUC */
#include	<vecobj.h>		/* LIBUC */
#include	<spawnproc.h>		/* LIBUC */
#include	<storebuf.h>		/* LIBUC */
#include	<sbuf.h>		/* LIBUC */
#include	<ids.h>			/* LIBUC */
#include	<mkdirs.h>		/* LIBUC */
#include	<permx.h>		/* LIBUC */
#include	<ascii.h>		/* LIBUC */
#include	<baops.h>		/* LIBUC */
#include	<sbuf.h>		/* LIBUC */
#include	<strn.h>		/* LIBUC */
#include	<sfx.h>			/* LIBUC |sfbasename(3uc)| */
#include	<mkx.h>			/* LIBUC */
#include	<snx.h>			/* LIBUC */
#include	<snwcpy.h>		/* LIBUC */
#include	<mkpathx.h>		/* LIBUC */
#include	<pathadd.h>		/* LIBUC */
#include	<pathclean.h>		/* LIBUC */
#include	<strwcpy.h>		/* LIBUC */
#include	<char.h>		/* LIBUC */
#include	<isnot.h>		/* LIBUC */
#include	<vardefs.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

#include	"uunames.h"

#pragma		GCC dependency		"mod/libutil.ccm"
#pragma		GCC dependency		"mod/uconstants.ccm"
#pragma		GCC dependency		"mod/sif.ccm"

import libutil ;			/* |memclear(3u)| */
import uconstants ;			/* |sysword(3c)| */
import sif ;

/* local defines */

#ifndef	VARPRMKUU
#define	VARPRMKUU	"MKUUNAMES_PROGRAMROOT"
#endif

#undef	VARDBNAME
#define	VARDBNAME	"MKUUNAMES_DBNAME"

#define	UU		uunames
#define	UU_CUR		uunames_cur
#define	UU_OBJ		uunames_obj
#define	UU_MAGSTR	"UUNAMES"

#define	VARDNAME	"var"
#define	INDDNAME	"uunames"
#define	DBNAME		"uunames"
#define	INDSUF		"sl"

#define	TO_FILEMOD	(60 * 24 * 3600)
#define	TO_MKWAIT	(5 * 50)

#define	TMPDMODE	0775

#define	DEFNAMES	10

#define	PROG_MKUU	"mkuunames"

#ifndef	CF_DEBUG
#define	CF_DEBUG	0		/* debugging */
#endif
#ifndef	CF_GETPROGROOT
#define	CF_GETPROGROOT	1		/* use |getprogroot(3dam)| */
#endif


/* imported namespaces */


/* local typedefs */

extern "C" {
    typedef int (*indopen_f)(uunames *,time_t) noex ;
}

typedef uunames_cur	cur ;


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct liner {
	cchar		*lp ;
	int		ll ;
	liner() noex : lp(nullptr), ll(0) { } ;
	liner(cchar *p,int l) noex : lp(p), ll(l) { } ;
    } ; /* end struct */
    struct envpop {
	cchar		*name ;
	cchar		*sub1dname ;
	cchar		*sub2dname ;
    } ; /* end struct */
    struct vars {
	int		maxpathlen ;
	operator int () noex ;
    } ; /* end struct (vars) */
} /* end namespace */

typedef liner *		linerp ;


/* forward references */

template<typename ... Args>
local int uunames_ctor(uunames *op,Args ... args) noex {
    	UUNAMES		*hop = op ;
	cnullptr	np{} ;
	cnothrow	nt{} ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    rs = SR_NOMEM ;
	    memclear(hop) ;
	    if ((op->nlp = new(nt) vecobj) != np) ylikely {
		rs = SR_OK ;
	    } /* end if (new-vecobj) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (uunames_ctor) */

local int uunames_dtor(uunames *op) noex {
	int		rs = SR_FAULT ;
	if (op) ylikely {
	    rs = SR_OK ;
	    if (op->nlp) ylikely {
		delete op->nlp ;
		op->nlp = nullptr ;
	    } /* end if (delete-vecobj) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (uunames_dtor) */

template<typename ... Args>
local inline int uunames_magic(uunames *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    rs = (op->magval == UUNAMES_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
} /* end subroutine (uunames_magic) */

local int	uunames_infoloadbegin(UU *,cchar *,cchar *) noex ;
local int	uunames_infoloadend(UU *) noex ;
local int	uunames_indopen(UU *,time_t) noex ;
local int	uunames_indopenpr(UU *,time_t) noex ;
local int	uunames_indopentmp(UU *,time_t) noex ;
local int	uunames_indopendname(UU *,cchar *,time_t) noex ;
local int	uunames_indclose(UU *) noex ;
local int	uunames_mkuunamesi(UU *,cchar *) noex ;
local int	uunames_envpaths(UU *,vecstr *) noex ;
local int	uunames_indreq(UU *,cchar *,time_t) noex ;
local int	uunames_indmapcreate(UU *,cchar *,time_t) noex ;
local int	uunames_indmapdestroy(UU *) noex ;
local int	uunames_filemapcreate(UU *,time_t) noex ;
local int	uunames_filemapdestroy(UU *) noex ;
local int	uunames_indlist(UU *) noex ;
local int	uunames_indcheck(UU *,time_t) noex ;

local int	vecstr_envdefs	(vecstr *,con mainv) noex ;
local int	vecstr_loadpath	(vecstr *,cchar *) noex ;
local int	vecstr_pathval	(vecstr *,char *,int) noex ;

local int	ckdirs(cchar *dn,mode_t dm) noex ;
local int	mkpathbn(char *,cc *,cc *,int,cc *) noex ;

#ifdef	COMMENT
local int	mkindfn(char *,cchar *,cchar *,cchar *,cchar *) noex ;
#endif

extern "C" {
    local int	vesrch(cvoid **,cvoid **) noex ;
}


/* local variables */

constexpr cpcchar	envsys[] = {
	VARSYSNAME,
	VARRELEASE,
	VARVERSION,
	VARMACHINE,
	VARARCHITECTURE,
	VARHZ,
	VARNODE,
	VARDOMAIN,
	VARHOMEDNAME,
	VARUSERNAME,
	VARLOGNAME,
	VARTZ,
	VARPWD,
	nullptr
} ; /* end array */

constexpr cpcchar	prnames[] = {
	"LOCAL",
	"NCMP",
	"EXTRA",
	"PCS",
	"GNU",
	"TOOLS",
	"DWB",
	"XDIR",
	"AST",
	nullptr
} ; /* end array */

constexpr cpcchar	envdefs[] = {
	"LOCALDOMAIN",
	"USER",
	"MAIL",
	"MAILDIR",
	"MAILDIRS",
	"LANG",
	"LC_COLLATE",
	"LC_CTYPE",
	"LC_MESSAGES",
	"LC_MONETARY",
	"LC_NUMERIC",
	"LC_TIME",
	"PATH",
	"LD_LIBRARY_PATH",
	"DYLD_LIBRARY_PATH",
	nullptr
} ; /* end array */

constexpr envpop	envpops[] = {
	{ VARPATH,	"bin",		"sbin" },
	{ VARLIBPATH,	"lib",		nullptr },
	{ VARMANPATH,	"man",		nullptr },
	{ nullptr,	nullptr,	nullptr }
} ; /* end array */

constexpr indopen_f	indopens[] = {
	uunames_indopenpr,
	uunames_indopentmp
} ; /* end array */

static vars		var ;
constexpr char		fmagstr[]	= UU_MAGSTR ;
cbool			f_debug		= CF_DEBUG ;
cbool			f_getprogroot	= CF_GETPROGROOT ;


/* exported variables */

const uunames_obj	uunames_modinfo = {
    	"uunames",
	szof(uunames),
	szof(uunames_cur)
} ; /* end initialization */


/* exported variables */


/* exported subroutines */

local int uunames_opens(UU *op,cchar *pr,cchar *dbname) noex ;

int uunames_open(UU *op,cchar *pr,cchar *dbname) noex {
	int		rs ;
	if ((dbname == nullptr) || (dbname[0] == '\0')) {
	    dbname = DBNAME ;
	}
	if ((rs = uunames_ctor(op,pr)) >= 0) ylikely {
	    rs = SR_INVALID ;
	    if (pr[0]) ylikely {
		if (static cint rsv = var ; (rs = rsv) >= 0) ylikely {
		    rs = uunames_opens(op,pr,dbname) ;
		} /* end if (vars) */
	    } /* end if (valid) */
	    if (rs < 0) {
		uunames_dtor(op) ;
	    } /* end if (error) */
	} /* end if (uunames_ctor) */
	return rs ;
} /* end subroutine (uunames_open) */

local int uunames_opens(UU *op,cchar *pr,cchar *dbname) noex {
	time_t		dt = getustime ;
    	int		rs ;
        if ((rs = uunames_infoloadbegin(op,pr,dbname)) >= 0) ylikely {
            cint        sz = szof(liner) ;
            cint        ne = DEFNAMES ;
            if ((rs = vecobj_start(op->nlp,sz,ne,0)) >= 0) ylikely {
                if ((rs = uunames_indopen(op,dt)) >= 0) ylikely {
                    op->magval = UUNAMES_MAGIC ;
                }
                if (rs < 0) {
                    vecobj_finish(op->nlp) ;
                } /* end if (error) */
            } /* end if (vecobj_start) */
            if (rs < 0) {
                uunames_infoloadend(op) ;
            } /* end if (error) */
        } /* end if (infoload) */
	return rs ;
} /* end subroutine (uunames_opens) */

int uunames_close(UU *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = uunames_magic(op)) >= 0) ylikely {
	    {
	        rs1 = uunames_indclose(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    if (op->nlp) {
	        rs1 = vecobj_finish(op->nlp) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = uunames_infoloadend(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
		rs1 = uunames_dtor(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    op->magval = 0 ;
	} /* end if (magic) */
	return rs ;
} /* end subroutine (uunames_close) */

int uunames_audit(UU *op) noex {
	int		rs ;
	if ((rs = uunames_magic(op)) >= 0) ylikely {
	    rs = uunames_indcheck(op,0) ;
	} /* end if (magic) */
	return rs ;
} /* end subroutine (uunames_audit) */

int uunames_curbegin(UU *op,cur *curp) noex {
	int		rs ;
	if ((rs = uunames_magic(op,curp)) >= 0) ylikely {
	    memclear(curp) ;
	    curp->i = -1 ;
	    op->ncursors += 1 ;
	    rs = uunames_indcheck(op,0) ;
	} /* end if (magic) */
	return rs ;
} /* end subroutine (uunames_curbegin) */

int uunames_curend(UU *op,cur *curp) noex {
	int		rs ;
	if ((rs = uunames_magic(op,curp)) >= 0) ylikely {
	    curp->i = -1 ;
	    if (op->ncursors > 0) {
	        op->ncursors -= 1 ;
	    }
	} /* end if (magic) */
	return rs ;
} /* end subroutine (uunames_curend) */

/* returns 0=not-found, (>0)=found-with-this-length */
int uunames_exists(UU *op,cchar *sp,int sl) noex {
	cnullptr	np{} ;
	cint		rsn = SR_NOTFOUND ;
	int		rs ;
	int		rs1 ;
	int		kl = 0 ;
	if ((rs = uunames_magic(op,sp)) >= 0) ylikely {
	    rs = SR_INVALID ;
	    if (sp[0]) ylikely {
	        if ((rs = uunames_indcheck(op,0)) >= 0) ylikely {
		    if (cchar *kp ; (rs = lm_strw(sp,sl,&kp)) >= 0) ylikely {
	                kl = rs ;
			{
			    vecobj	*nlp = op->nlp ;
			    liner	le(kp,kl) ;
			    cauto	vcf = vesrch ;
	                    if ((rs = nlp->search(&le,vcf,np)) == rsn) {
				kl = 0 ;
			    }
			} /* end block */
			void *vp = voidp(kp) ;
	                rs1 = lm_free(vp) ;
			if (rs >= 0) rs = rs1 ;
	            } /* end if (m-a-f) */
	        } /* end if (uunames_indcheck) */
	    } /* end if (valid) */
	} /* end if (magic) */
	return (rs >= 0) ? kl : rs ;
} /* end subroutine (uunames_exists) */

int uunames_curenum(UU *op,cur *curp,char *ubuf,int ulen) noex {
	int		rs ;
	int		len = 0 ;
	if ((rs = uunames_magic(op,curp,ubuf)) >= 0) ylikely {
	    cint	i = (curp->i >= 0) ? (curp->i + 1) : 0 ;
	    void	*vp{} ;
	    if ((rs = vecobj_get(op->nlp,i,&vp)) >= 0) ylikely {
	        liner	*lep = linerp(vp) ;
	        if ((rs = snwcpy(ubuf,ulen,lep->lp,lep->ll)) >= 0) {
	            len = rs ;
	            curp->i = i ;
	        }
	    } /* end if (vecobj_get) */
	} /* end if (magic) */
	return (rs >= 0) ? len : rs ;
} /* end subroutine (uunames_curenum) */

int uunames_count(UU *op) noex {
	int		rs ;
	int		c = 0 ;
	if ((rs = uunames_magic(op)) >= 0) ylikely {
	    if ((rs = uunames_indcheck(op,0)) >= 0) ylikely {
	        rs = vecobj_count(op->nlp) ;
		c = rs ;
	    }
	} /* end if (magic) */
	return (rs >= 0) ? c : rs ;
} /* end subroutine (uunames_count) */


/* private subroutines */

local int uunames_infoloadbegin(UU *op,cc *pr,cc *dbname) noex {
	int		rs = SR_BUGCHECK ;
	if (op) ylikely {
	    rs = SR_OK ;
	    op->pr = pr ;
	    op->dbname = dbname ;
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (uunames_infoloadbegin) */

local int uunames_infoloadend(UU *op) noex {
	int		rs = SR_BUGCHECK ;
	if (op) ylikely {
	    rs = SR_OK ;
	    op->pr = nullptr ;
	    op->dbname = nullptr ;
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (uunames_infoloadend) */

local int uunames_indmapcreate(UU *op,cchar *indname,time_t dt) noex {
	int		rs ;
	int		rs1 ;
	int		rv = 0 ;
	if (char *ibuf ; (rs = lm_mp(&ibuf)) >= 0) ylikely {
	    cchar	*s_suf = INDSUF ;
	    cchar	*s_end = ENDIANSTR ;
	    if ((rs = mkfnamesuf(ibuf,indname,s_suf,s_end)) >= 0) {
		cint	fl = rs ;
	        if (cchar *cp ; (rs = lm_strw(ibuf,fl,&cp)) >= 0) ylikely {
		    op->indfname = cp ;
		    if ((rs = uunames_filemapcreate(op,dt)) >= 0) ylikely {
			rs = uunames_indlist(op) ;
			rv = rs ;
		    } /* end if */
	            if (rs < 0) {
	                void *vp = voidp(op->indfname) ;
	                rs1 = lm_free(vp) ;
	                if (rs >= 0) rs = rs1 ;
	                op->indfname = nullptr ;
	            } /* end if (error) */
		} /* end if (memory-acquire) */
	    } /* end if (mkfnamesuf) */
	    rs1 = lm_free(ibuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return (rs >= 0) ? rv : rs ;
} /* end subroutine (uunames_indmapcreate) */

local int uunames_indmapdestroy(UU *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	int		c = 0 ; /* return-value */
	for (int i = 0 ; vecobj_del(op->nlp,i) >= 0 ; i += 1) {
	    c += 1 ;
	} /* end for */
	{
	    rs1 = uunames_filemapdestroy(op) ;
	    if (rs >= 0) rs = rs1 ;
	}
	if (op->indfname) {
	    void *vp = voidp(op->indfname) ;
	    rs1 = lm_free(vp) ;
	    if (rs >= 0) rs = rs1 ;
	    op->indfname = nullptr ;
	} /* end if (memory-release) */
	return (rs >= 0) ? c : rs ;
} /* end subroutine (uunames_indmapdestroy) */

local int uunames_filemapcreate(UU *op,time_t dt) noex {
	cnullptr	np{} ;
	cint		of = O_RDONLY ;
	int		rs ;
	int		rs1 ;
	cmode		om = 0666 ;
	if (dt == 0) dt = getustime ;
	if ((rs = uc_open(op->indfname,of,om)) >= 0) {
	    cint	fd = rs ;
	    if (ustat sb ; (rs = uc_fstat(fd,&sb)) >= 0) {
		rs = SR_NOTSUP ;
	        if (S_ISREG(sb.st_mode)) {
		    rs = SR_TOOBIG ;
	            if (sb.st_size <= INT_MAX) ylikely {
			csize	ms = size_t(sb.st_size) ;
			cint	mp = PROT_READ ;
			cint	mf = MAP_SHARED ;
			void	*md{} ;
	                op->ti_mod = sb.st_mtime ;
			if ((rs = u_mmapbegin(np,ms,mp,mf,fd,0z,&md)) >= 0) {
	    		    op->ti_map = dt ;
	    	            op->ti_lastcheck = dt ;
			    op->mapdata = caddr_t(md) ;
	                    op->mapsize = ms ;
	                    op->ti_mod = sb.st_mtime ;
			} /* end if (memory-mapped) */
		    } /* end if (not-toobig) */
		} /* end if (regular-file) */
	    } /* end if (stat) */
	    rs1 = uc_close(fd) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (file) */
	return rs ;
} /* end subroutine (uunames_filemapcreate) */

local int uunames_filemapdestroy(UU *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (op->mapdata) ylikely {
	    caddr_t	md = op->mapdata ;
	    csize	ms = op->mapsize ;
	    rs1 = u_mmapend(md,ms) ;
	    if (rs >= 0) rs = rs1 ;
	    op->mapdata = nullptr ;
	    op->mapsize = 0 ;
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (uunames_filemapdestroy) */

local int uunames_indopen(UU *op,time_t dt) noex {
	int		rs = SR_NOENT ;
	int		f = false ;
	for (cauto &fun : indopens) {
	    if ((rs = fun(op,dt)) >= 0) {
		f = true ;
	    } else if (isNotPresent(rs)) {
		rs = SR_OK ;
	    }
	    if (rs < 0) break ;
	} /* end for */
	return (rs >= 0) ? f : rs ;
} /* end subroutine (uunames_indopen) */

local int uunames_indopenpr(UU *op,time_t dt) noex {
	int		rs ;
	int		rs1 ;
	if (char *ibuf ; (rs = lm_mp(&ibuf)) >= 0) ylikely {
	    if ((rs = mkpath(ibuf,op->pr,VARDNAME,INDDNAME)) >= 0) ylikely {
	        rs = uunames_indopendname(op,ibuf,dt) ;
	    }
	    rs1 = lm_free(ibuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return rs ;
} /* end subroutine (uunames_indopenpr) */

local int uunames_indopentmp(UU *op,time_t dt) noex {
	int		rs ;
	int		rs1 ;
	cchar		*tmpdname = sysword.w_vartmpdir ;
	cchar		*inddname = INDDNAME ;
	if (char *ibuf ; (rs = lm_mp(&ibuf)) >= 0) ylikely {
	    if (cchar *bn ; (rs = sfbasename(op->pr,-1,&bn)) > 0) {
	        if ((rs = mkpathbn(ibuf,tmpdname,bn,rs,inddname)) >= 0) {
	            rs = uunames_indopendname(op,ibuf,dt) ;
	        }
	    } else if (rs == 0) {
	        rs = SR_INVALID ;
	    }
	    rs1 = lm_free(ibuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return rs ;
} /* end subroutine (uunames_indopentmp) */

local int uunames_indopendname(UU *op,cchar *dname,time_t dt) noex {
	int		rs ;
	int		rs1 ;
	int		rv = 0 ; /* return-value */
	cmode		dm = 0777 ;
	if ((rs = ckdirs(dname,dm)) > 0) {
	    if (char *ibuf ; (rs = lm_mp(&ibuf)) >= 0) ylikely {
	        if ((rs = mkpath(ibuf,dname,op->dbname)) >= 0) ylikely {
	            if ((rs = uunames_indreq(op,ibuf,dt)) > 0) {
	                if ((rs = uunames_mkuunamesi(op,dname)) >= 0) {
		            rs = uunames_indmapcreate(op,ibuf,dt) ;
			    rv = rs ;
		        }
		    } else if (rs == SR_OK) {
		        rs = uunames_indmapcreate(op,ibuf,dt) ;
		        rv = rs ;
		    } /* end if (ok) */
	        } /* end if (mkpath) */
	        rs1 = lm_free(ibuf) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (m-a-f) */
	} /* end if (ckdirs) */
	return (rs >= 0) ? rv : rs ;
} /* end subroutine (uunames_indopendname) */

/* new-index-required - ret: 0=NO, 1=YES */
local int uunames_indreq(UU *op,cchar *indname,time_t dt) noex {
	int		rs ;
	int		rs1 ;
	int		freq = false ; /* return-value */
	if (char *ibuf ; (rs = lm_mp(&ibuf)) >= 0) ylikely {
	    cchar	*s_suf = INDSUF ;
	    cchar	*s_end = ENDIANSTR ;
	    if ((rs = mkfnamesuf(ibuf,indname,s_suf,s_end)) >= 0) ylikely {
	         if (ustat sb ; (rs = u_stat(ibuf,&sb)) >= 0) {
	             custime ti_ind = sb.st_mtime ;
		     if (dt == 0) dt = getustime ;
		     if (sb.st_size == 0) {
			 freq = true ;
		     } else if (ti_ind > op->ti_mod) {
			 freq = true ;
		     } else if ((dt - ti_ind) >= TO_FILEMOD) {
			 freq = true ;
		     }
		 } else if (isNotPresent(rs)) {
		     freq = true ;
	         } /* end if (u_stat) */
	    } /* end if (mkfnamesuf) */
	    rs1 = lm_free(ibuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return (rs >= 0) ? freq : rs ;
} /* end subroutine (uunames_indreq) */

local int uunames_indclose(UU *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (op->fl.varind) {
	    op->fl.varind = false ;
	    rs1 = uunames_indmapdestroy(op) ;
	    if (rs >= 0) rs = rs1 ;
	}
	return rs ;
} /* end subroutine (uunames_indclose) */

#ifdef	COMMENT
namespace {
    struct spawer {
	UU		*op ;
	cchar		*dname ;
	spawner(UU *o,cchar *d) noex : op(o), dname(d) { } ;
	operator int () noex ;
    } ; /* end struct (spanwer) */
} /* end namespace */
#endif /* COMMENT */

/* make the index */
local int uunames_mkuunamesi(UU *op,cchar *dname) noex {
    	int		rs = SR_BUGCHECK ;
	if (op && dname) ylikely {
	    spawner so(op,dname) ;
	    rs = so ;
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (uunames_mkuunamesi) */

spawner::operator int () noex {
    	int		rs = SR_OK ;
    	return rs ;
} /* end method (spawner::operator) */

#ifdef	COMMENT
local int uunames_mkuunamesi(UU *op,cchar *dname) noex {
	SPAWNPROC_CON	ps{} ;
	vecstr		envs ;
	pid_t	cpid ;
	int	rs ;
	int	i, cstat, cex ;
	int	prlen = 0 ;

	cchar		*varprmkuu = VARPRMKUU ;
	cchar		*pn = PROG_MKUU ;
	cchar		*av[10] ;
	mainv		ev ;

	char	progfname[MAXPATHLEN + 1] ;
	char	dbname[MAXPATHLEN + 1] ;
	if (dname == nullptr)
	    return SR_FAULT ;

	if (dname[0] == '\0')
	    return SR_INVALID ;

	rs = mkpath(dbname,dname,op->dbname) ;
	if (rs < 0)
	    goto ret0 ;

	rs = vecstr_start(&envs,10,vecstrm.compact) ;
	if (rs < 0)
	    goto ret0 ;

#if	CF_GETPROGROOT

	rs = getprogroot(op->pr,prnames,&prlen,progfname,pn) ;

	if (rs == 0)
	    rs = mkpath1(progfname,pn) ;

#ifdef	COMMENT
	if ((rs >= 0) && (prlen > 0)) {
	    rs = vecstr_envadd(&envs,varprmkuu,progfname,prlen) ;
	} else if (rs >= 0)
	    rs = vecstr_envadd(&envs,varprmkuu,op->pr,-1) ;
#endif /* COMMENT */

#else /* CF_GETPROGROOT */

	rs = prgetprogpath(op->pr,progfname,pn,-1) ;
	if (rs == 0)
	    rs = mkpath1(progfname,progmkuunamesi) ;

#endif /* CF_GETPROGROOT */

	if (rs < 0)
	    goto ret2 ;

/* setup environment for child process */

	if (rs >= 0)
	    rs = uunames_envpaths(op,&envs) ;

	if (rs >= 0)
	    rs = vecstr_envadd(&envs,varprmkuu,op->pr,-1) ;

	if (rs >= 0)
	    rs = vecstr_envadd(&envs,VARDBNAME,dbname,-1) ;

	if (rs >= 0)
	    rs = vecstr_envdefs(&envs,envsys) ;

	if (rs >= 0)
	    rs = vecstr_envdefs(&envs,prnames) ;

	if (rs >= 0)
	    rs = vecstr_envdefs(&envs,envdefs) ;

	if (rs < 0)
	    goto ret2 ;

/* setup arguments */

	i = 0 ;
	av[i++] = pn ;
	av[i++] = nullptr ;

/* go */

	vecstr_getvec(&envs,&ev) ;
	ps.opts |= SPAWNPROC_OIGNINTR ;
	ps.opts |= SPAWNPROC_OSETPGRP ;
	for (i = 0 ; i < 3 ; i += 1) {
	    ps.disp[i] = (i != 2) ? SPAWNPROC_DCLOSE : SPAWNPROC_DINHERIT ;
	} /* end for */
	rs = spawnproc(&ps,progfname,av,ev) ;
	cpid = rs ;

ret2:
	vecstr_finish(&envs) ;
	if (rs < 0)
	    goto ret0 ;

	cstat = 0 ;
	if ((rs = u_waitpid(cpid,&cstat,0)) >= 0) {
	    cex = 0 ;
	    if (WIFSIGNALED(cstat)) {
	        rs = SR_UNATCH ;	/* protocol not attached */
	    }
	    if ((rs >= 0) && WIFEXITED(cstat)) {
	        cex = WEXITSTATUS(cstat) ;
	        if (cex != 0) rs = SR_LIBBAD ;
	    }
	} /* end if (process finished) */

ret0:
	return rs ;
} /* end subroutine (uunames_mkuunamesi) */
#endif /* COMMENT */

local int uunames_envpaths(UU *op,vecstr *elp) noex {
	cnullptr	np{} ;
	cint		vo = vecstrm.ordered ;
	cint		ne = 40 ;
	int		rs ;
	int		rs1 ;
	if (char *pbuf ; (rs = lm_mp(&pbuf)) >= 0) ylikely {
	    if (vecstr pc ; (rs = pc.start(ne,vo)) >= 0) ylikely {
	        int	bl ;
	        int	pl ;
	        char	*bp = nullptr ;
	        for (int i = 0 ; envpops[i].name ; i += 1) {
		    int		sz = 0 ;
	            cchar	*enp = envpops[i].name ;
	            cchar	*subdname = envpops[i].sub1dname ;
	            if ((rs >= 0) && (subdname != nullptr)) {
	                if ((rs = mkpath(pbuf,op->pr,subdname)) >= 0) {
	                    pl = rs ;
	                    rs = pc.add(pbuf,pl) ;
		        }
	            } /* end if */
	            subdname = envpops[i].sub2dname ;
	            if ((rs >= 0) && (subdname != nullptr)) {
	                if ((rs = mkpath(pbuf,op->pr,subdname)) >= 0) {
	                    pl = rs ;
	                    rs = pc.add(pbuf,pl) ;
		        }
	            } /* end if */
	            if (cchar *vp ; (rs >= 0) && ((vp = getenver(enp)) != np)) {
	                rs = vecstr_loadpath(&pc,vp) ;
	            }
	            if (rs >= 0) {
	                rs = pc.strsize() ;
		        sz = rs ;
	            }
	            if ((rs >= 0) && ((rs = lm_mall((sz+1),&bp)) >= 0)) {
	                if ((rs = vecstr_pathval(&pc,bp,sz)) >= 0) {
	                    bl = rs ;
	                    rs = elp->envadd(enp,bp,bl) ;
		        }
	                rs1 = lm_free(bp) ;
			if (rs >= 0) rs = rs1 ;
	            } /* end if (memory allocation) */
	            pc.delall() ;
	            if (rs < 0) break ;
	        } /* end for */
	        rs1 = pc.finish ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (vecstr) */
	    rs1 = lm_free(pbuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return rs ;
} /* end subroutine (uunames_envpaths) */

local int uunames_indlist(UU *op) noex {
    	vecobj		*nlp = op->nlp ;
	cnullptr	np{} ;
	uint		lineoff = 0 ;
	int		rs = SR_OK ;
	int		ml = intsat(op->mapsize) ;
	int		n = 0 ; /* return-value */
	cchar		*mp = charp(op->mapdata) ;
	for (cchar *tp ; (tp = strnchr(mp,ml,'\n')) != np ; ) {
	    liner	le ;
	    cint	len = intconv((tp + 1) - mp) ;
	    le.lp = mp ;
	    le.ll = (len - 1) ; /* subtract off NL */
	    if (lineoff > 0) {
		if (cchar *cp ; sfcontent(le.lp,le.ll,&cp) > 0) {
		    n += 1 ;
		    rs = nlp->add(&le) ;
		}
	    } else {
		if ((le.ll == 0) || (strncmp(le.lp,fmagstr,le.ll) != 0)) {
		    rs = SR_LIBBAD ;
		}
	    } /* end if */
	    if (rs < 0) break ;
	    lineoff += len ;
	    mp += len ;
	    ml -= len ;
	} /* end while (processing lines) */
	return (rs >= 0) ? n : rs ;
} /* end subroutine (uunames_indlist) */

local int uunames_indcheck(UU *op,time_t) noex {
	int		rs = SR_NOTFOUND ;
	int		f = false ;
	if (op->mapdata) {
	    rs = SR_OK ;
	} /* end if (non-null) */
	return (rs >= 0) ? f : rs ;
} /* end subroutine (uunames_indcheck) */

local int vecstr_envdefs(vecstr *elp,con mainv ea) noex {
	int		rs = SR_OK ;
	int		c = 0 ; /* return-value */
	for (int i = 0 ; ea[i] ; i += 1) {
	    if (cchar *cp = getenver(ea[i]) ; cp) {
		rs = vecstr_envadd(elp,ea[i],cp,-1) ;
		c += !!(rs < INT_MAX) ;
	    } /* end if (getenver) */
	    if (rs < 0) break ;
	} /* end for */
	return (rs >= 0) ? c : rs ;
} /* end subroutine (vecstr_envdefs) */

local int vecstr_loadpath(vecstr *clp,cchar *pp) noex {
	cint		rsn = SR_NOTFOUND ;
	int		rs ;
	int		rs1 ;
	int		c = 0 ; /* return-value */
	if (char *tbuf ; (rs = lm_mp(&tbuf)) >= 0) ylikely {
	    sif		of(pp,-1,":;") ;
	    int		cl ;
	    cchar	*cp ;
	    while ((cl = of(&cp)) > 0) {
	        if ((rs = pathclean(tbuf,cp,cl)) >= 0) ylikely {
	            if ((rs = clp->findn(tbuf,cl)) == rsn) {
	                c += 1 ;
		        rs = clp->add(tbuf,cl) ;
	            }
		} /* end if (pathclean) */
	        if (rs < 0) break ;
	    } /* end while */
	    rs1 = lm_free(tbuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (trailing one) */
	return (rs >= 0) ? c : rs ;
} /* end subroutine (vecstr_loadpath) */

local int vecstr_pathval(vecstr *clp,char *vbuf,int vlen) noex {
	int		rs = SR_OK ;
	int		rl = 0 ; /* return-value */
	if (vlen >= 0) {
	    int		sch{} ;
	    int		c = 0 ;
	    cchar	*cp ;
	    bool	f_semi = false ;
	    vbuf[0] = '\0' ;
	    for (int i = 0 ; clp->get(i,&cp) >= 0 ; i += 1) {
	        if (cp) {
	            if (cp[0] != ';') {
	                if (c++ > 0) {
	                    if (f_semi) {
	                        f_semi = false ;
	                        sch = ';' ;
	                    } else {
	                        sch = ':' ;
		            }
	                    rs = storebuf_chr(vbuf,vlen,rl,sch) ;
	                    rl += rs ;
	                } /* end if */
	                if (rs >= 0) {
	                    rs = storebuf_strw(vbuf,vlen,rl,cp,-1) ;
	                    rl += rs ;
	                }
	            } else {
	                f_semi = true ;
	            }
	        }
	        if (rs < 0) break ;
	    } /* end for */
	} else {
	    rs = SR_NOANODE ;
	}
	return (rs >= 0) ? rl : rs ;
} /* end subroutine (vecstr_pathval) */

vars::operator int () noex {
    	int		rs ;
	if ((rs = bufsizeget(bufsize_mp)) >= 0) {
	    maxpathlen = rs ;
	} /* end if */
    	return rs ;
} /* end method (vars::operator) */

local int ckdirs(cchar *dn,mode_t dm) noex {
    	cnullptr	np{} ;
    	int		rs = SR_FAULT ;
	int		fok = false ;
	if (dn) ylikely {
	    if (ustat sb ; (rs = u_stat(dn,&sb)) >= 0) {
		rs = SR_NOTDIR ;
		if (S_ISDIR(sb.st_mode)) {
	    	    if ((rs = perm(dn,-1,-1,np,W_OK)) >= 0) {
			fok = true ;
		    }
		} /* end if (is-dir) */
	    } else if (isNotPresent(rs)) {
		if ((rs = mkdirs(dn,dm)) >= 0) {
		    fok = true ;
		} else if (isNotPresent(rs)) {
		    rs = SR_OK ;
		} /* end if (mkdirs) */
	    } /* end if */
	} /* end if (non-null) */
    	return (rs >= 0) ? fok : rs ;
} /* end subroutine (ckdirs) */

#ifdef	COMMENT
local int mkindfn(char *rbuf,cc *dn,cc *nn,cc *s_suf,cc *s_end) noex {
    	cint		rlen = var.maxpathlen ;
	int		rs ;
	int		rs1 ;
        if (sbuf sb ; (rs = sb.start(rbuf,rlen)) >= 0) ylikely {
            {
                if ((rs = sb.strw(dn)) >= 0) ylikely {
                    cint dnl = rs ;
                    if (dn[dnl - 1] != '/') {
                        rs = sb.chr('/') ;
                    }
                } /* end if */
                if (rs >= 0) rs = sb.strw(nn) ;
                if (rs >= 0) rs = sb.chr('.') ;
                if (rs >= 0) rs = sb.strw(s_suf) ;
                if (rs >= 0) rs = sb.strw(s_end) ;
            } /* end block */
            rs1 = sb.finish ;
            if (rs >= 0) rs = rs1 ;
        } /* end if (sbuf) */
	return rs ;
} /* end subroutine (mkindfn) */
#endif /* COMMENT */

local int mkpathbn(char *rbuf,cc *dn,cc *bp,int bl,cc *nn) noex {
    	cint		rlen = var.maxpathlen ;
	int		rs ;
	int		rs1 ;
        if (sbuf sb ; (rs = sb.start(rbuf,rlen)) >= 0) ylikely {
            {
                if ((rs = sb.strw(dn)) >= 0) ylikely {
                    cint dnl = rs ;
                    if (dn[dnl - 1] != '/') {
                        rs = sb.chr('/') ;
                    }
                } /* end if */
                if (rs >= 0) rs = sb.strw(bp,bl) ;
                if (rs >= 0) rs = sb.chr('/') ;
                if (rs >= 0) rs = sb.strw(nn) ;
            } /* end block */
            rs1 = sb.finish ;
            if (rs >= 0) rs = rs1 ;
        } /* end if (sbuf) */
	return rs ;
} /* end subroutine (mkpathbn) */

local int esrch(con liner *l1p,con liner *l2p) noex {
	int		rc = 0 ;
        if (l1p || l2p) ylikely {
            if (l1p) {
                if (l2p) {
                    cint cl = l1p->ll ;
                    cc *c1p = l1p->lp ;
                    cc *c2p = l2p->lp ;
                    if ((rc = (c1p[0] - c2p[0])) == 0) {
                        if ((rc = strncmp(c1p,c2p,cl)) == 0) {
                            rc = (l1p->ll - l2p->ll) ;
                        }
                    }
                } else {
                    rc = -1 ;
                }
            } else {
                rc = +1 ;
            }
        } /* end if (at least one is non-null) */
	return rc ;
} /* end subroutine (esrch) */

local int vesrch(cvoid **v1pp,cvoid **v2pp) noex {
	con liner	**e1pp = cast_reinterpret<con liner **>(v1pp) ;
	con liner	**e2pp = cast_reinterpret<con liner **>(v2pp) ;
	int		rc = 0 ;
	if (e1pp && e2pp) ylikely {
	    con liner	*l1p = *e1pp ;
	    con liner	*l2p = *e2pp ;
	    rc = esrch(l1p,l2p) ;
	} /* end if (non-null) */
	return rc ;
} /* end subroutine (vesrch) */


