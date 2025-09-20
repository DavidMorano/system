/* uunames SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* uunames-query database manager */
/* version %I% last-modified %G% */

#define	CF_GETPROGROOT	1		/* use 'getprogroot(3dam)' */

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
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<sys/mman.h>
#include	<climits>		/* |INT_MAX| */
#include	<ctime>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<getbufsize.h>
#include	<mallocxx.h>
#include	<endian.h>
#include	<vecstr.h>
#include	<vecobj.h>
#include	<spawnproc.h>
#include	<getprogroot.h>
#include	<storebuf.h>
#include	<sbuf.h>
#include	<ids.h>
#include	<mkdirs.h>
#include	<permx.h>
#include	<ascii.h>
#include	<baops.h>
#include	<sbuf.h>
#include	<strn.h>
#include	<sfx.h>
#include	<snx.h>
#include	<snwcpy.h>
#include	<mkx.h>
#include	<mkpathx.h>
#include	<pathadd.h>
#include	<pathclean.h>
#include	<strwcpy.h>
#include	<char.h>
#include	<isnot.h>
#include	<localmisc.h>

#include	"uunames.h"

import sif ;

/* local defines */

#ifndef	VARSYSNAME
#define	VARSYSNAME	"SYSNAME"
#endif

#ifndef	VARRELEASE
#define	VARRELEASE	"RELEASE"
#endif

#ifndef	VARVERSION
#define	VARVERSION	"VERSION"
#endif

#ifndef	VARMACHINE
#define	VARMACHINE	"MACHINE"
#endif

#ifndef	VARARCHITECTURE
#define	VARARCHITECTURE	"ARCHITECTURE"
#endif

#ifndef	VARHZ
#define	VARHZ		"HZ"
#endif

#ifndef	VARDOMAIN
#define	VARDOMAIN	"DOMAIN"
#endif

#ifndef	VARNODE
#define	VARNODE		"NODE"
#endif

#ifndef	VARHOMEDNAME
#define	VARHOMEDNAME	"HOME"
#endif

#ifndef	VARUSERNAME
#define	VARUSERNAME	"USERNAME"
#endif

#ifndef	VARLOGNAME
#define	VARLOGNAME	"LOGNAME"
#endif

#ifndef	VARTZ
#define	VARTZ		"TZ"
#endif

#ifndef	VARPWD
#define	VARPWD		"PWD"
#endif

#ifndef	VARTMPDNAME
#define	VARTMPDNAME	"TMPDIR"
#endif

#ifndef	VARMAIL
#define	VARMAIL		"MAIL"
#endif

#ifndef	VARPATH
#define	VARPATH		"PATH"
#endif

#ifndef	VARLIBPATH
#define	VARLIBPATH	"LD_LIBRARY_PATH"
#endif

#ifndef	VARMANPATH
#define	VARMANPATH	"MANPATH"
#endif

#ifndef	VARVPATH
#define	VARVPATH	"VPATH"
#endif

#ifndef	VARPRLOCAL
#define	VARPRLOCAL	"LOCAL"
#endif

#ifndef	VARPRNCMP
#define	VARPRNCMP	"NCMP"
#endif

#ifndef	VARPRMKUU
#define	VARPRMKUU	"MKUUNAMES_PROGRAMROOT"
#endif

#undef	VARDBNAME
#define	VARDBNAME	"MKUUNAMES_DBNAME"

#ifndef	TMPDNAME
#define	TMPDNAME	"/tmp"
#endif

#ifndef	TMPVARDNAME
#define	TMPVARDNAME	"/var/tmp"
#endif

#define	UUNAMES_DBMAGICSTR	"UUNAMES"
#define	UUNAMES_PRNAME	VARPRNCMP

#define	UU		uunames
#define	CUR		uunames_cur
#define	ENVPOP		struct envpop

#define	VARDNAME	"var"
#define	INDDNAME	"uunames"

#define	DBNAME		"uunames"
#define	INDSUF		"sl"

#ifndef	KEYBUFLEN
#define	KEYBUFLEN	NATURALWORDLEN
#endif

#define	TO_FILEMOD	(60 * 24 * 3600)
#define	TO_MKWAIT	(5 * 50)

#define	TMPDMODE	0775

#define	DEFNAMES	10

#define	PROG_MKUU	"mkuunames"


/* imported namespaces */

using std::nullptr_t ;			/* type */
using std::nothrow ;			/* constant */


/* local typedefs */

typedef int (*uunames_f)(uunames *,time_t) noex ;

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
    } ;
    struct envpop {
	cchar		*name ;
	cchar		*sub1dname ;
	cchar		*sub2dname ;
    } ;
}

typedef liner *		linerp ;


/* forward references */

template<typename ... Args>
static int uunames_ctor(uunames *op,Args ... args) noex {
    	UUNAMES		*hop = op ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    cnullptr	np{} ;
	    rs = SR_NOMEM ;
	    memclear(hop) ;
	    if ((op->nlp = new(nothrow) vecobj) != np) {
		rs = SR_OK ;
	    } /* end if (new-vecobj) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (uunames_ctor) */

static int uunames_dtor(uunames *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	    if (op->nlp) {
		delete op->nlp ;
		op->nlp = nullptr ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (uunames_dtor) */

template<typename ... Args>
static inline int uunames_magic(uunames *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magic == UUNAMES_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}
/* end subroutine (uunames_magic) */

static int	uunames_infoloadbegin(UU *,cchar *,cchar *) noex ;
static int	uunames_infoloadend(UU *) noex ;
static int	uunames_indopen(UU *,time_t) noex ;
static int	uunames_indopenpr(UU *,time_t) noex ;
static int	uunames_indopentmp(UU *,time_t) noex ;
static int	uunames_indopendname(UU *,cchar *,time_t) noex ;
static int	uunames_indclose(UU *) noex ;
static int	uunames_mkuunamesi(UU *,cchar *) noex ;
static int	uunames_envpaths(UU *,vecstr *) noex ;
static int	uunames_indtest(UU *,cchar *,time_t) noex ;
static int	uunames_indmapcreate(UU *,cchar *,time_t) noex ;
static int	uunames_indmapdestroy(UU *) noex ;
static int	uunames_filemapcreate(UU *,time_t) noex ;
static int	uunames_filemapdestroy(UU *) noex ;
static int	uunames_indfn(UU *,cchar *,time_t) noex ;
static int	uunames_indlist(UU *) noex ;
static int	uunames_indcheck(UU *,time_t) noex ;

static int	checkdname(cchar *) noex ;

static int	vecstr_defenvs(vecstr *,mainv) noex ;
static int	vecstr_loadpath(vecstr *,cchar *) noex ;
static int	mkpathval(vecstr *,char *,int) noex ;

#ifdef	COMMENT
static int	mkindfname(char *,cchar *,cchar *,cchar *,cchar *) noex ;
#endif

extern "C" {
    static int	vesrch(cvoid **,cvoid **) noex ;
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
} ;

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
} ;

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
	nullptr
} ;

constexpr envpop	envpops[] = {
	{ VARPATH, "bin", "sbin" },
	{ VARLIBPATH, "lib", nullptr },
	{ VARMANPATH, "man", nullptr },
	{ nullptr, nullptr, nullptr }
} ;

constexpr uunames_f	indopens[] = {
	uunames_indopenpr,
	uunames_indopentmp,
	nullptr
} ;


/* exported variables */

extern const uunames_obj	uunames_modinfo = {
    	"uunames",
	szof(uunames),
	szof(uunames_cur)
} ;


/* exported subroutines */

int uunames_open(UU *op,cchar *pr,cchar *dbname) noex {
	time_t		dt = getustime ;
	int		rs ;
	if ((rs = uunames_ctor(op,pr)) >= 0) {
	    rs = SR_INVALID ;
	    if (pr[0]) {
	        if ((dbname == nullptr) || (dbname[0] == '\0')) {
	            dbname = DBNAME ;
	        }
		if ((rs = uunames_infoloadbegin(op,pr,dbname)) >= 0) {
	            cint	sz = szof(liner) ;
	            cint	ne = DEFNAMES ;
	            if ((rs = vecobj_start(op->nlp,sz,ne,0)) >= 0) {
			if ((rs = uunames_indopen(op,dt)) >= 0) {
			    op->magic = UUNAMES_MAGIC ;
			}
			if (rs < 0) {
			    vecobj_finish(op->nlp) ;
			}
		    } /* end if (vecobj_start) */
		    if (rs < 0) {
			uunames_infoloadend(op) ;
		    }
		} /* end if (infoload) */
	    } /* end if (valid) */
	    if (rs < 0) {
		uunames_dtor(op) ;
	    }
	} /* end if (uunames_ctor) */
	return rs ;
}
/* end subroutine (uunames_open) */

int uunames_close(UU *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = uunames_magic(op)) >= 0) {
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
	    op->magic = 0 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (uunames_close) */

int uunames_audit(UU *op) noex {
	int		rs ;
	if ((rs = uunames_magic(op)) >= 0) {
	    rs = uunames_indcheck(op,0) ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (uunames_audit) */

int uunames_curbegin(UU *op,cur *curp) noex {
	int		rs ;
	if ((rs = uunames_magic(op,curp)) >= 0) {
	    memclear(curp) ;
	    curp->i = -1 ;
	    op->ncursors += 1 ;
	    rs = uunames_indcheck(op,0) ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (uunames_curbegin) */

int uunames_curend(UU *op,cur *curp) noex {
	int		rs ;
	if ((rs = uunames_magic(op,curp)) >= 0) {
	    curp->i = -1 ;
	    if (op->ncursors > 0) {
	        op->ncursors -= 1 ;
	    }
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (uunames_curend) */

/* returns 0=not-found, (>0)=found-with-this-length */
int uunames_exists(UU *op,cchar *sp,int sl) noex {
	cint		rsn = SR_NOTFOUND ;
	int		rs ;
	int		rs1 ;
	int		kl = 0 ;
	if ((rs = uunames_magic(op,sp)) >= 0) {
	    rs = SR_INVALID ;
	    if (sp[0]) {
	        if ((rs = uunames_indcheck(op,0)) >= 0) {
		    if (cchar *kp ; (rs = uc_mallocstrw(sp,sl,&kp)) >= 0) {
			cnullptr	np{} ;
	                kl = rs ;
			{
			    vecobj	*nlp = op->nlp ;
			    liner	le(kp,kl) ;
			    auto	vcf = vesrch ;
	                    if ((rs = vecobj_search(nlp,&le,vcf,np)) == rsn) {
				kl = 0 ;
			    }
			} /* end block */
	                rs1 = uc_free(kp) ;
			if (rs >= 0) rs = rs1 ;
	            } /* end if (allocation) */
	        } /* end if (uunames_indcheck) */
	    } /* end if (valid) */
	} /* end if (magic) */
	return (rs >= 0) ? kl : rs ;
}
/* end subroutine (uunames_exists) */

int uunames_enum(UU *op,cur *curp,char *ubuf,int ulen) noex {
	int		rs ;
	int		len = 0 ;
	if ((rs = uunames_magic(op,curp,ubuf)) >= 0) {
	    cint	i = (curp->i >= 0) ? (curp->i + 1) : 0 ;
	    void	*vp{} ;
	    if ((rs = vecobj_get(op->nlp,i,&vp)) >= 0) {
	        liner	*lep = linerp(vp) ;
	        if ((rs = snwcpy(ubuf,ulen,lep->lp,lep->ll)) >= 0) {
	            len = rs ;
	            curp->i = i ;
	        }
	    } /* end if (vecobj_get) */
	} /* end if (magic) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (uunames_enum) */

int uunames_count(UU *op) noex {
	int		rs ;
	int		c = 0 ;
	if ((rs = uunames_magic(op)) >= 0) {
	    if ((rs = uunames_indcheck(op,0)) >= 0) {
	        rs = vecobj_count(op->nlp) ;
		c = rs ;
	    }
	} /* end if (magic) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (uunames_count) */


/* private subroutines */

static int uunames_infoloadbegin(UU *op,cc *pr,cc *dbname) noex {
	int		rs = SR_OK ;
	op->pr = pr ;
	op->dbname = dbname ;
	return rs ;
}
/* end subroutine (uunames_infoloadbegin) */

static int uunames_infoloadend(UU *op) noex {
	op->pr = nullptr ;
	op->dbname = nullptr ;
	return SR_OK ;
}
/* end subroutine (uunames_infoloadend) */

static int uunames_indmapcreate(UU *op,cchar *indname,time_t dt) noex {
	int		rs ;
	int		rs1 ;
	int		rv = 0 ;
	op->indfname = nullptr ;
	if (char *indfname ; (rs = malloc_mp(&indfname)) >= 0) {
	    cchar	*suf = INDSUF ;
	    cchar	*ends = ENDIANSTR ;
	    if ((rs = mkfnamesuf2(indfname,indname,suf,ends)) >= 0) {
		cint	fl = rs ;
	        if (cchar *cp ; (rs = uc_mallocstrw(indfname,fl,&cp)) >= 0) {
		    op->indfname = cp ;
		    if ((rs = uunames_filemapcreate(op,dt)) >= 0) {
			rs = uunames_indlist(op) ;
			rv = rs ;
		    } /* end if */
	            if (rs < 0) {
	                rs1 = uc_free(op->indfname) ;
	                if (rs >= 0) rs = rs1 ;
	                op->dbname = nullptr ;
	            }
		} /* end if (memory-allocation) */
	    } /* end if (mkfnamesuf) */
	    rs = rsfree(rs,indfname) ;
	} /* end if (m-a-f) */
	return (rs >= 0) ? rv : rs ;
}
/* end subroutine (uunames_indmapcreate) */

static int uunames_indmapdestroy(UU *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	int		c = 0 ;
	for (int i = 0 ; vecobj_del(op->nlp,i) >= 0 ; i += 1) {
	    c += 1 ;
	}
	{
	    rs1 = uunames_filemapdestroy(op) ;
	    if (rs >= 0) rs = rs1 ;
	}
	if (op->indfname) {
	    rs1 = uc_free(op->indfname) ;
	    if (rs >= 0) rs = rs1 ;
	    op->dbname = nullptr ;
	}
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (uunames_indmapdestroy) */

static int uunames_filemapcreate(UU *op,time_t dt) noex {
	cint		of = O_RDONLY ;
	int		rs ;
	int		rs1 ;
	cmode		om = 0666 ;
	if (dt == 0) dt = getustime ;
	if ((rs = uc_open(op->indfname,of,om)) >= 0) {
	    cint	fd = rs ;
	    if (USTAT sb ; (rs = uc_fstat(fd,&sb)) >= 0) {
		rs = SR_NOTSUP ;
	        if (S_ISREG(sb.st_mode)) {
		    cnullptr	np{} ;
		    rs = SR_TOOBIG ;
	            if (sb.st_size <= INT_MAX) {
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
}
/* end subroutine (uunames_filemapcreate) */

static int uunames_filemapdestroy(UU *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (op->mapdata != nullptr) {
	    rs1 = u_mmapend(op->mapdata,op->mapsize) ;
	    if (rs >= 0) rs = rs1 ;
	    op->mapdata = nullptr ;
	    op->mapsize = 0 ;
	}
	return rs ;
}
/* end subroutine (uunames_filemapdestroy) */

static int uunames_indopen(UU *op,time_t dt) noex {
	int		rs = SR_NOENT ;
	int		f = false ;
	for (int i = 0 ; indopens[i] != nullptr ; i += 1) {
	    if ((rs = (*indopens[i])(op,dt)) >= 0) {
		f = true ;
	    } else if (isNotPresent(rs)) {
		rs = SR_OK ;
	    }
	    if (rs < 0) break ;
	} /* end for */
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (uunames_indopen) */

static int uunames_indopenpr(UU *op,time_t dt) noex {
	int		rs ;
	int		rs1 ;
	if (char *idname ; (rs = malloc_mp(&idname)) >= 0) {
	    if ((rs = mkpath(idname,op->pr,VARDNAME,INDDNAME)) >= 0) {
	        rs = uunames_indopendname(op,idname,dt) ;
	    }
	    rs1 = uc_free(idname) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return rs ;
}
/* end subroutine (uunames_indopenpr) */

static int uunames_indopentmp(UU *op,time_t dt) noex {
	int		rs ;
	int		rs1 ;
	cchar		*tmpdname = TMPVARDNAME ;
	cchar		*inddname = INDDNAME ;
	if (char *idname ; (rs = malloc_mp(&idname)) >= 0) {
	    cchar	*prname{} ;
	    if ((rs = sfbasename(op->pr,-1,&prname)) > 0) {
	        if ((rs = mkpath(idname,tmpdname,prname,inddname)) >= 0) {
	            rs = uunames_indopendname(op,idname,dt) ;
	        }
	    } else {
	        rs = SR_INVALID ;
	    }
	    rs1 = uc_free(idname) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return rs ;
}
/* end subroutine (uunames_indopentmp) */

static int uunames_indopendname(UU *op,cchar *dname,time_t dt) noex {
	int		rs ;
	int		rs1 ;
	if (char *indname ; (rs = malloc_mp(&indname)) >= 0) {
	bool	f_mk = false ;
	if ((rs = mkpath2(indname,dname,op->dbname)) >= 0) {
	    if ((rs = uunames_indtest(op,indname,dt)) >= 0) {
	        bool	f_ok = (rs > 0) ;

	if (((rs < 0) && (rs != SR_NOMEM)) || (! f_ok)) {
	    if ((rs = uunames_mkuunamesi(op,dname)) >= 0) {
		f_mk = true ;
	        rs = uunames_indtest(op,indname,dt) ;
	        f_ok = (rs > 0) ;
	    }
	}

	if (((rs < 0) && (rs != SR_NOMEM)) || (! f_ok)) {
	    f_mk = true ;
	    rs = uunames_indfn(op,dname,dt) ;
	}

	if (rs >= 0) {
	    rs = uunames_indmapcreate(op,indname,dt) ;
	    op->fl.varind = (rs >= 0) ;
	}

	if ((rs < 0) && (rs != SR_BADFMT) && (! f_mk)) {
	    if ((rs = uunames_indfn(op,dname,dt)) >= 0) {
		rs = uunames_indmapcreate(op,indname,dt) ;
	        op->fl.varind = (rs >= 0) ;
	    }
	}

		} /* end if (indtest) */
	    } /* end if (mkpath) */
	    rs1 = uc_free(indname) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return rs ;
}
/* end subroutine (uunames_indopendname) */

static int uunames_indtest(UU *op,cchar *indname,time_t dt) noex {
	int		rs ;
	int		rs1 ;
	int		f = false ;
	if (char *indfname ; (rs = malloc_mp(&indfname)) >= 0) {
	    cchar	*suf = INDSUF ;
	    cchar	*ends = ENDIANSTR ;
	    if ((rs = mkfnamesuf2(indfname,indname,suf,ends)) >= 0) {
	         USTAT		sb ;
	         time_t		ti_ind ;
	         rs1 = u_stat(indfname,&sb) ;
	         ti_ind = sb.st_mtime ;
	         if ((rs1 >= 0) && ((sb.st_size == 0) || (ti_ind == 0))) {
	             rs1 = SR_NOTFOUND ;
	         }
	         if ((rs1 >= 0) && (op->ti_mod > ti_ind)) {
	             rs1 = SR_NOTFOUND ;
	         }
	         if ((rs1 >= 0) && ((dt - ti_ind) >= TO_FILEMOD)) {
	             rs1 = SR_TIMEDOUT ;
	         }
	         f = (rs1 >= 0) ;
	    } /* end if (mkfnamesuf) */
	    rs1 = uc_free(indfname) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (uunames_indtest) */

static int uunames_indfn(UU *op,cchar *dname,time_t) noex {
	int		rs ;
	int		rs1 ;
	int		len = 0 ;
	if (char *indname ; (rs = malloc_mp(&indname)) >= 0) {
	    if ((rs = checkdname(dname)) == SR_NOENT) {
	        rs = mkdirs(dname,TMPDMODE) ;
	    }
	    if (rs >= 0) {
	        rs = mkpath2(indname,dname,op->dbname) ;
		len = rs ;
	    }
	    rs1 = uc_free(indname) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (uunames_indfn) */

static int uunames_indclose(UU *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (op->fl.varind) {
	    op->fl.varind = false ;
	    rs1 = uunames_indmapdestroy(op) ;
	    if (rs >= 0) rs = rs1 ;
	}
	return rs ;
}
/* end subroutine (uunames_indclose) */

/* make the index */
static int uunames_mkuunamesi(UU *op,cchar *dname) noex {
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

	rs = mkpath2(dbname,dname,op->dbname) ;
	if (rs < 0)
	    goto ret0 ;

	rs = vecstr_start(&envs,10,VECSTR_OCOMPACT) ;
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
	    rs = vecstr_defenvs(&envs,envsys) ;

	if (rs >= 0)
	    rs = vecstr_defenvs(&envs,prnames) ;

	if (rs >= 0)
	    rs = vecstr_defenvs(&envs,envdefs) ;

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
	}
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
}
/* end subroutine (uunames_mkuunamesi) */

static int uunames_envpaths(UU *op,vecstr *elp) noex {
	cnullptr	np{} ;
	cint		vo = VECSTR_OORDERED ;
	cint		ne = 40 ;
	int		rs ;
	int		rs1 ;
	if (char *pbuf ; (rs = malloc_mp(&pbuf)) >= 0) {
	    if (vecstr pc ; (rs = pc.start(ne,vo)) >= 0) {
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
	                if ((rs = mkpath2(pbuf,op->pr,subdname)) >= 0) {
	                    pl = rs ;
	                    rs = pc.add(pbuf,pl) ;
		        }
	            } /* end if */
	            if (cchar *vp ; (rs >= 0) && ((vp = getenv(enp)) != np)) {
	                rs = pc.loadpath(vp) ;
	            }
	            if (rs >= 0) {
	                rs = pc.strsize() ;
		        sz = rs ;
	            }
	            if ((rs >= 0) && ((rs = uc_malloc((sz+1),&bp)) >= 0)) {
	                if ((rs = mkpathval(&pathcomps,bp,sz)) >= 0) {
	                    bl = rs ;
	                    rs = elp->envadd(enp,bp,bl) ;
		        }
	                rs1 = uc_free(bp) ;
			if (rs >= 0) rs = rs1 ;
	            } /* end if (memory allocation) */
	            pc.delall ;
	            if (rs < 0) break ;
	        } /* end for */
	        rs1 = pc.finish ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (vecstr) */
	    rs1 = uc_free(pbuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return rs ;
}
/* end subroutine (uunames_envpaths) */

static int uunames_indlist(UU *op) noex {
	liner		le ;
	uint		lineoff = 0 ;
	int		rs = SR_OK ;
	int		ml = op->mapsize ;
	int		n = 0 ;
	cchar		*mp = charp(op->mapdata) ;
	cchar		*tp ;
	cchar		*filemagic = UUNAMES_DBMAGICSTR ;
	lineoff = 0 ;
	while ((tp = strnchr(mp,ml,'\n')) != nullptr) {
	    cint	len = ((tp + 1) - mp) ;
	    le.lp = mp ;
	    le.ll = (len - 1) ;
	    if (lineoff > 0) {
		if ((le.ll > 0) && (le.lp[0] != '#')) {
		    n += 1 ;
		    rs = vecobj_add(op->nlp,&le) ;
		}
	    } else {
		if ((le.ll == 0) || (strncmp(le.lp,filemagic,le.ll) != 0)) {
		    rs = SR_LIBBAD ;
		}
	    }
	    if (rs < 0) break ;
	    lineoff += len ;
	    mp += len ;
	    ml -= len ;
	} /* end while (processing lines) */
	return (rs >= 0) ? n : rs ;
}
/* end subroutine (uunames_indlist) */

static int uunames_indcheck(UU *op,time_t) noex {
	int		rs = SR_NOTFOUND ;
	int		f = false ;
	if (op->mapdata) {
	    rs = SR_OK ;
	} /* end if (non-null) */
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (uunames_indcheck) */

static int checkdname(cchar *dname) noex {
	int		rs = SR_OK ;
	if (dname[0] == '/') {
	    if (USTAT sb ; (rs = u_stat(dname,&sb)) >= 0) {
		if (S_ISDIR(sb.st_mode)) {
	    	    rs = perm(dname,-1,-1,nullptr,W_OK) ;
		} else {
	            rs = SR_NOTDIR ;
		}
	    } /* end if (stat) */
	} else {
	    rs = SR_INVALID ;
	}
	return rs ;
}
/* end subroutine (checkdname) */

#ifdef	COMMENT

static int mkindfname(char *rbuf,cc *dname,cc *name,cc *suf,cc *end) noex {
	int		rs ;
	int		rs1 ;
	int		len = 0 ;
	if ((rs = getbufsize(getbufsize_mp)) >= 0) {
	    int		dnl = 0 ;
	    if (sbuf sb ; (rs = sb.start(rbuf,rs)) >= 0) {
		{
	            if ((rs = sb.strw(dname,-1)) >= 0) {
	                dnl = rs ;
		    }
	            if ((rs >= 0) && (dname[dnl - 1] != '/')) {
	                rs = sb.chr('/') ;
	            }
	            if (rs >= 0) rs = sb.strw(name) ;
	            if (rs >= 0) rs = sb.chr('.') ;
	            if (rs >= 0) rs = sb.strw(suf) ;
	            if (rs >= 0) rs = sb.strw(end) ;
		} /* end block */
		rs1 = sb.finish ;
		if (rs >= 0) rs = rs1 ;
		len = rs1 ;
	    } /* end if (sbuf) */
	} /* end if (getbufsize) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (mkindfname) */

#endif /* COMMENT */

static int vecstr_defenvs(vecstr *elp,mainv ea) noex {
	int		rs = SR_OK ;
	int		c = 0 ;
	for (int i = 0 ; ea[i] != nullptr ; i += 1) {
	    if (cchar *cp ; (cp = getenv(ea[i])) != nullptr) {
		rs = vecstr_envadd(elp,ea[i],cp,-1) ;
		c += !!(rs < INT_MAX) ;
	    }
	    if (rs < 0) break ;
	} /* end for */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (vecstr_defenvs) */

static int vecstr_loadpath(vecstr *clp,cchar *pp) noex {
	cint		rsn = SR_NOTFOUND ;
	int		rs ;
	int		rs1 ;
	int		c = 0 ;
	if (char *tbuf ; (rs = malloc_mp(&tbuf)) >= 0) {
	    sif		of(pp,-1,":;") ;
	    int		cl ;
	    cchar	*cp ;
	    while ((cl = of(&cp)) > 0) {
	        if ((rs = pathclean(tbuf,cp,cl)) >= 0) {
	            if ((rs = clp->findn(tbuf,cl)) == rsn) {
	                c += 1 ;
		        rs = clp->add(tbuf,cl) ;
	            }
		} /* end if (pathclean) */
	        if (rs < 0) break ;
	    } /* end while */
	    rs1 = uc_free(tbuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (trailing one) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (vecstr_loadpath) */

static int mkpathval(vecstr *clp,char *vbuf,int vbuflen) noex {
	int		rs = SR_OK ;
	int		rlen = 0 ;
	if (vbuflen >= 0) {
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
	                    rs = storebuf_chr(vbuf,vbuflen,rlen,sch) ;
	                    rlen += rs ;
	                } /* end if */
	                if (rs >= 0) {
	                    rs = storebuf_strw(vbuf,vbuflen,rlen,cp,-1) ;
	                    rlen += rs ;
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
	return (rs >= 0) ? rlen : rs ;
}
/* end subroutine (mkpathval) */

static int vesrch(cvoid **v1pp,cvoid **v2pp) noex {
	liner		**e1pp = (liner **) v1pp ;
	liner		**e2pp = (liner **) v2pp ;
	liner		*l1p ;
	liner		*l2p ;
	int		rc = 0 ;
	l1p = *e1pp ;
	l2p = *e2pp ;
	if (l1p || l2p) {
	    if (l1p) {
	        if (l2p) {
		    if ((rc = (l1p->lp[0] - l2p->lp[0])) == 0) {
	    	        if ((rc = strncmp(l1p->lp,l2p->lp,l1p->ll)) == 0) {
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
}
/* end subroutine (vesrch) */


