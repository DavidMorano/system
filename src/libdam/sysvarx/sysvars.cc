/* sysvars SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* interface to query the system-variable database */
/* version %I% last-modified %G% */

#define	CF_MKSYSVARS	1		/* call 'sysvar' program */

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	sysvars

	Description:
	This little object provides access to the SYS-variible
	database and index (if any).  The "in-sequence" procedure
	for finding indices is:

	%R/var/%D
	/var/tmp/%{PRN}/%D
	/var/tmp/%D
	%T/$D
	/tmp/%{PRN}/%D
	/tmp/%D

	Where:
	%R		program-root
	%D		DB-name
	%T		the user TMPDIR
	%{PRN}		program-root name

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/param.h>		/* POSIX */
#include	<sys/stat.h>		/* POSIX */
#include	<ctime>			/* CSTD */
#include	<climits>		/* CSTD */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstring>		/* CSTD */
#include	<new>			/* C++STD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<uclibmem.h>		/* LIBUC */
#include	<strlibval.hh>		/* LIBUC */
#include	<bufsizeget.h>		/* LIBUC */
#include	<vecobj.h>		/* LIBUC */
#include	<vecstr.h>		/* LIBUC */
#include	<spawnproc.h>		/* LIBUC */
#include	<storebuf.h>		/* LIBUC */
#include	<ids.h>			/* LIBUC */
#include	<hdbstr.h>		/* LIBUC */
#include	<expcook.h>		/* LIBUC */
#include	<dirseen.h>		/* LIBUC */
#include	<pathclean.h>		/* LIBUC */
#include	<sfx.h>			/* LIBUC */
#include	<mkpathx.h>		/* LIBUC */
#include	<mkdirs.h>		/* LIBUC */
#include	<permx.h>		/* LIBUC */
#include	<strwcpy.h>		/* LIBUC */
#include	<char.h>		/* LIBUC */
#include	<isnot.h>		/* LIBUC */
#include	<mkchar.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */
#include	<var.h>			/* LIBDAM */

#include	"sysvars.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| */

/* local defines */

#define	SVS		sysvars
#define	SVS_C		sysvars_cur

#define	SI		subinfo
#define	SI_FL		subinfo_fl

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

#ifndef	VARWSTATION
#define	VARWSTATION	"ESTATION"
#endif

#ifndef	VARPWD
#define	VARPWD		"PWD"
#endif

#ifndef	VARTMPDNAME
#define	VARTMPDNAME	"TMPDIR"
#endif

#ifndef	VARPRLOCAL
#define	VARPRLOCAL	"LOCAL"
#endif

#undef	VARSVDBNAME
#define	VARSVDBNAME	"SYSVARS_DBNAME"

#undef	VARSVPR
#define	VARSVPR		"SYSVARS_PROGRAMROOT"

#ifndef	TMPDNAME
#define	TMPDNAME	"/tmp"
#endif

#ifndef	VCNAME
#define	VCNAME		"var"
#endif

#define	INDDNAME	"sysvars"

#define	INDNAME		"sysvars"

#ifndef	KBUFLEN
#define	KBUFLEN		120
#endif

#define	TO_FILEMOD	(60 * 24 * 3600)
#define	TO_MKWAIT	(5 * 50)

#define	PROG_MKSYSVARSI	"sysvar"

#ifndef	DEFINITFNAME
#define	DEFINITFNAME	"/etc/default/init"
#endif

#ifndef	DEFLOGFNAME
#define	DEFLOGFNAME	"/etc/default/login"
#endif

#ifndef	DEFNVARS
#define	DEFNVARS	20
#endif

#ifndef	CF_MKSYSVARS
#define	CF_MKSYSVARS	1		/* call 'sysvar' program */
#endif


/* imported namespaces */

using libuc::libmem ;			/* variable */


/* local typedefs */


/* external subroutines */

extern "C" {
    extern int	sysvars_procget(hdbstr *,cchar *) noex ;
    extern int	sysvars_procset(hdbstr *,cchar *,int) noex ;
}


/* external variables */


/* local structures */

struct subinfo_fl {
	uint		id:1 ;
} ; /* end struct */

struct subinfo {
	ids		id ;
	SI_FL		fl ;
	time_t		daytime ;
} ; /* end struct */

namespace {
    struct varer {
	int		maxpathlen ;
	operator int () noex ;
    } ; /* end struct (varer) */
} /* end namespace */


/* local structures */


/* forward references */

template<typename ... Args>
local int sysvars_ctor(sysvars *op,Args ... args) noex {
    	SYSVARS		*hop = op ;
	cnullptr	np{} ;
	cnothrow	nt{} ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    memclear(hop) ;
	    rs = SR_NOMEM ;
	    if ((op->vindp = new(nt) var) != np) ylikely {
		rs = SR_OK ;
	    } /* end if (new-var) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (sysvars_ctor) */

local int sysvars_dtor(sysvars *op) noex {
	int		rs = SR_FAULT ;
	if (op) ylikely {
	    rs = SR_OK ;
	    if (op->vindp) ylikely {
		delete op->vindp ;
		op->vindp = nullptr ;
	    }
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (sysvars_dtor) */

template<typename ... Args>
local inline int sysvars_magic(sysvars *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    rs = (op->magval == SYSVARS_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
} /* end subroutine (sysvars_magic) */

local int	sysvars_infoloadbegin(SVS *,cchar *,cchar *) noex ;
local int	sysvars_infoloadend(SVS *) noex ;
local int	sysvars_indopen(SVS *,SI *) noex ;

local int	sysvars_indclose(SVS *) noex ;
local int	sysvars_indmk(SVS *,cchar *) noex ;
local int	sysvars_indmkdata(SVS *,cchar *,mode_t) noex ;
local int	sysvars_indopenseq(SVS *,SI *) noex ;
local int	sysvars_indopenseqer(SVS *,SI *,dirseen *,
			vecstr *,expcook *) noex ;
local int	sysvars_loadcooks(SVS *,expcook *) noex ;
local int	sysvars_indopenalt(SVS *,SI *,dirseen *) noex ;

#if	CF_MKSYSVARS
local int	sysvars_mksysvarsi(SVS *,SI *,cchar *) noex ;
#endif

local int	subinfo_start(SI *) noex ;
local int	subinfo_ids(SI *) noex ;
local int	subinfo_finish(SI *) noex ;

local int	checkdname(cchar *) noex ;

#ifdef	COMMENT
local int	mkindfname(char *,cchar *,cchar *,cchar *,cchar *) noex ;
#endif

local int havefile(cchar *) noex ;


/* local variables */

#if	CF_MKSYSVARS
constexpr cpcchar	envdefs[] = {
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
	VARPWD,
	VARTZ,
	VARWSTATION,
	nullptr
} ; /* end array (envdefs) */
#endif /* CF_MKSYSVARS */

/* use fixed locations for security reasons (like we care!) */
#if	CF_MKSYSVARS
constexpr cpcchar	prbins[] = {
	"bin",
	"sbin",
	nullptr
} ; /* end array (prbins) */
#endif /* CF_MKSYSVARS */

constexpr cpcchar	sysfnames[] = {
	DEFINITFNAME,	
	DEFLOGFNAME,
	nullptr
} ; /* end array (sysfnames) */

constexpr cpcchar	dbdirs[] = {
	"%R/var",
	"/var/tmp/%{PRN}",
	"/var/tmp",
	"/tmp/%{PRN}",
	"/tmp",
	"%T/%{PRN}",
	"%T",
	nullptr
} ; /* end array (dbdirs) */

static varer		vdata ;
static strlibval	tmpdname(strlibval_tmpdir) ;
cbool			f_mksysvars = CF_MKSYSVARS ;


/* exported variables */

const sysvars_obj		sysvars_modinfo = {
	"sysvars",
	szof(sysvars),
	szof(sysvars_cur)
} ; /* end innitialization */


/* exported subroutines */

int sysvars_open(SVS *op,cchar *pr,cchar *dbname) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if ((dbname == nullptr) || (dbname[0] == '\0')) {
	    dbname = INDNAME ;
	}
	if ((rs = sysvars_ctor(op,pr)) >= 0) ylikely {
	    rs = SR_INVALID ;
	    if (pr[0]) ylikely {
		if (static cint rsv = vdata ; (rs = rsv) >= 0) {
	            if (SI si ; (rs = subinfo_start(&si)) >= 0) ylikely {
	                if ((rs = sysvars_infoloadbegin(op,pr,dbname)) >= 0) {
	                    if ((rs = sysvars_indopen(op,&si)) >= 0) {
	            	        op->magval = SYSVARS_MAGIC ;
			    }
	                    if (rs < 0) {
	                        sysvars_infoloadend(op) ;
			    } /* end if (error) */
	                } /* end if (infoload) */
	                rs1 = subinfo_finish(&si) ;
	                if (rs >= 0) rs = rs1 ;
	            } /* end if (subinfo) */
		} /* end if (varer) */
	    } /* end if (valid) */
	    if (rs < 0) {
		sysvars_dtor(op) ;
	    } /* end if (error) */
	} /* end if (sysvars_ctor) */
	return rs ;
} /* end subroutine (sysvars_open) */

int sysvars_close(SVS *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = sysvars_magic(op)) >= 0) ylikely {
	    {
	        rs1 = sysvars_indclose(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = sysvars_infoloadend(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
		rs1 = sysvars_dtor(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    op->magval = 0 ;
	} /* end if (magic) */
	return rs ;
} /* end subroutine (sysvars_close) */

int sysvars_audit(SVS *op) noex {
	int		rs ;
	if ((rs = sysvars_magic(op)) >= 0) ylikely {
	    rs = var_audit(op->vindp) ;
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (sysvars_audit) */

int sysvars_curbegin(SVS *op,SVS_C *curp) noex {
	int		rs ;
	if ((rs = sysvars_magic(op,curp)) >= 0) ylikely {
	    cint	osz = szof(var) ;
	    memclear(curp) ;
	    if (void *vp ; (rs = lm_mall(osz,&vp)) >= 0) ylikely {
		var_cur		*vcp = (var_cur *) vp ;
	        if ((rs = var_curbegin(op->vindp,vcp)) >= 0) {
		    curp->vcurp = vcp ;
	            op->ncursors += 1 ;
	        } /* end if (var_curbegin) */
	    } /* end if (memory-allocation) */
	} /* end if (magic) */
	return rs ;
} /* end subroutine (sysvars_curbegin) */

int sysvars_curend(SVS *op,SVS_C *curp) noex {
	int		rs ;
	if ((rs = sysvars_magic(op,curp)) >= 0) ylikely {
	    rs = SR_BUGCHECK ;
	    if (curp->vcurp) ylikely {
	        var_cur		*vcp = (var_cur *) curp->vcurp ;
	        if ((rs = var_curend(op->vindp,vcp)) >= 0) {
	            op->ncursors -= 1 ;
	        }
	    } /* end if (valid) */
	} /* end if (magic) */
	return rs ;
} /* end subroutine (sysvars_curend) */

int sysvars_fetch(SVS *op,cc *kp,int kl,SVS_C *curp,char *vbuf,int vlen) noex {
	int		rs ;
	if ((rs = sysvars_magic(op,kp,curp)) >= 0) ylikely {
	    rs = SR_BUGCHECK ;
	    if (curp->vcurp) ylikely {
	        var_cur		*vcp = (var_cur *) curp->vcurp ;
		rs = var_fetch(op->vindp,kp,kl,vcp,vbuf,vlen) ;
	    }
	} /* end if (magic) */
	if ((rs < 0) && vbuf) {
	    vbuf[0] = '\0' ;
	} /* end if (error) */
	return rs ;
} /* end subroutine (sysvars_fetch) */

int sysvars_curenum(SVS *op,SVS_C *curp,char *kp,int kl,char *vp,int vl) noex {
	int		rs ;
	if ((rs = sysvars_magic(op,curp,kp)) >= 0) ylikely {
	    rs = SR_BUGCHECK ;
	    if (curp->vcurp) ylikely {
	        var_cur		*vcp = (var_cur *) curp->vcurp ;
		rs = var_curenum(op->vindp,vcp,kp,kl,vp,vl) ;
	    } /* end if (open) */
	} /* end if (non-null) */
	if ((rs < 0) && vp) {
	    vp[0] = '\0' ;
	} /* end if (error) */
	return rs ;
} /* end subroutine (sysvars_curenum) */

int sysvars_count(SVS *op) noex {
	int		rs = SR_FAULT ;
	if ((rs = sysvars_magic(op)) >= 0) ylikely {
	    rs = var_count(op->vindp) ;
	} /* end if (magic) */
	return rs ;
} /* end subroutine (sysvars_count) */


/* private subroutines */

local int sysvars_infoloadbegin(SVS *op,cchar *pr,cchar *dbname) noex {
	int		rs ;
	int		sz = 0 ;
	sz += (lenstr(pr) + 1) ;
	sz += (lenstr(dbname) + 1) ;
	if (char *bp ; (rs = lm_mall(sz,&bp)) >= 0) ylikely {
	    op->a = bp ;
	    op->pr = bp ;
	    bp = (strwcpy(bp,pr,-1)+1) ;
	    op->dbname = bp ;
	    bp = (strwcpy(bp,dbname,-1)+1) ;
	} /* emd if (memory-acquire) */
	return rs ;
} /* end subroutine (sysvars_infoloadbegin) */

local int sysvars_infoloadend(SVS *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (op->a) ylikely {
	    rs1 = lm_free(op->a) ;
	    if (rs >= 0) rs = rs1 ;
	    op->a = nullptr ;
	} /* end if (memory-release) */
	op->pr = nullptr ;
	op->dbname = nullptr ;
	return rs ;
} /* end subroutine (sysvars_infoloadend) */

local int sysvars_indopen(SVS *op,SI *sip) noex {
	return sysvars_indopenseq(op,sip) ;
} /* end subroutine (sysvars_indopen) */

local int sysvars_indopenseq(SVS *op,SI *sip) noex {
	int		rs ;
	int		rs1 ;
	if (dirseen ds ; (rs = dirseen_start(&ds)) >= 0) ylikely {
	    expcook	cooks ;
	    cint	vn = 6 ;
	    cint	vo = vecstrm.compact ;
	    if (vecstr sdirs ; (rs = sdirs.start(vn,vo)) >= 0) {
	        if ((rs = expcook_start(&cooks)) >= 0) {
		    if ((rs = sysvars_loadcooks(op,&cooks)) >= 0) ylikely {
			rs = sysvars_indopenseqer(op,sip,&ds,&sdirs,&cooks) ;
		    }
		    rs1 = expcook_finish(&cooks) ;
		    if (rs >= 0) rs = rs1 ;
		} /* end if (expcooks_loadcooks) */
		rs1 = sdirs.finish ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (vecstr) */
	    rs1 = dirseen_finish(&ds) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (dirseen) */
	return rs ;
} /* end subroutine (sysvars_indopenseq) */

local int sysvars_indopenseqer(SVS *op,SI *sip,dirseen *dsp,
		vecstr *sdp,expcook *ecp) noex {
	cint		maxpath = vdata.maxpathlen ;
	cint		sz = (2 * (vdata.maxpathlen + 1)) ;
	int		rs ;
	int		rs1 ;
	int		ai = 2 ; /* double allocation */
	if (char *a ; (rs = libmem.mall(sz,&a)) >= 0) {
	    cint	elen = maxpath ;
	    char	*ebuf = (a + (--ai * (maxpath + 1))) ;
	    char	*pbuf = (a + (--ai * (maxpath + 1))) ;
	    /* first phase: expand possible directory paths */
	    for (int i = 0 ; (rs >= 0) && dbdirs[i] ; i += 1) {
	        if ((rs = expcook_exp(ecp,0,ebuf,elen,dbdirs[i],-1)) > 0) {
	            cint	el = rs ;
	            if ((rs = pathclean(pbuf,ebuf,el)) > 0) {
	                cint pl = rs ;
	                rs1 = dirseen_havename(dsp,pbuf,pl) ;
	                if (rs1 == SR_NOTFOUND) {
	                    rs = dirseen_add(dsp,pbuf,pl,nullptr) ;
		        }
	            } /* end if (pathclean) */
	        } /* end if (expcook_exp) */
	    } /* end for */
	    /* next phase: create DB file-paths from directories */
	    if (rs >= 0) {
	        if (op->dbname[0] != '/') {
		    dirseen_cur	cur ;
	            if ((rs = dirseen_curbegin(dsp,&cur)) >= 0) {
	        	while (rs >= 0) {
	                    cint el = dirseen_curenum(dsp,&cur,ebuf,elen) ;
	                    if (el == SR_NOTFOUND) break ;
	                    rs = el ;
	                    if (rs >= 0) {
	                        if ((rs = mkpath(pbuf,ebuf,op->dbname)) >= 0) {
	                            cint pl = rs ;
	                            rs = vecstr_add(sdp,pbuf,pl) ;
			        }
	                    } /* end if (ok) */
	                } /* end while */
	                rs1 = dirseen_curend(dsp,&cur) ;
		        if (rs >= 0) rs = rs1 ;
	            } /* end if (cursor) */
	        } else {
		    rs = vecstr_add(sdp,op->dbname,-1) ;
	        } /* end if */
	    } /* end if (ok) */
	    /* final phase: try to open all of them in-sequence */
	    if (rs >= 0) {
	        if (mainv dv ; (rs = vecstr_getvec(sdp,&dv)) >= 0) {
		    for (int i = 0 ; i < rs ; i += 1) {
			if (cchar *fn = dv[i] ; (rs = havefile(fn)) > 0) {
	                    if ((rs = var_open(op->vindp,fn)) >= 0) {
	                        op->fl.var = true ;
		            }
			} /* end if (havefile) */
	                if (op->fl.var) break ;
			if (rs < 0) break ;
		    } /* end for */
	            if (((rs < 0) && isNotPresent(rs)) || (! op->fl.var)) {
	                rs = sysvars_indopenalt(op,sip,dsp) ;
		    } /* end if (error) */
	        }
	    } /* end if (ok) */
	    rs1 = libmem.free(a) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return rs ;
} /* end subroutine (sysvars_indopenseqer) */

local int sysvars_loadcooks(SVS *op,expcook *ecp) noex {
	int		rs = SR_OK ;
	cchar		*ks = "RST" ;
	char		kbuf[2] = {} ;
	for (int i = 0 ; (rs >= 0) && (ks[i] != '\0') ; i += 1) {
	    cint	kch = mkchar(ks[i]) ;
	    int		val = -1 ;
	    cchar	*vap = nullptr ;
	    switch (kch) {
	    case 'R':
		vap = op->pr ;
		break ;
	    case 'S':
		vap = INDDNAME ;
		break ;
	    case 'T':
		vap = tmpdname ;
		break ;
	    } /* end switch */
	    if ((rs >= 0) && vap) {
		kbuf[0] = charconv(kch) ;
		rs = expcook_add(ecp,kbuf,vap,val) ;
	    }
	} /* end for */
	if (rs >= 0) {
	    if (cchar *prname{} ; (rs = sfbasename(op->pr,-1,&prname)) >= 0) {
	        rs = SR_NOENT ;
	        if (prname) {
	            rs = expcook_add(ecp,"PRN",prname,-1) ;
		}
	    } /* end if (sfbasename) */
	} /* end if (ok) */
	return rs ;
} /* end subroutine (sysvars_loadcooks) */

local int sysvars_indopenalt(SVS *op,SI *sip,dirseen *dsp) noex {
    	cint		maxpath = vdata.maxpathlen ;
	cint		sz = (2 * (vdata.maxpathlen + 1)) ;
	int		rs ;
	int		rs1 ;
	int		ai = 2 ; /* double memory allocation */
	if (char *a ; (rs = libmem.mall(sz,&a)) >= 0) {
	    cint	elen = maxpath ;
	    if (dirseen_cur cur ; (rs = dirseen_curbegin(dsp,&cur)) >= 0) {
	        char	*ebuf		= (a + (--ai * (maxpath + 1))) ;
	        char	*indname	= (a + (--ai * (maxpath + 1))) ;
	        while (rs >= 0) {
	            int el = dirseen_curenum(dsp,&cur,ebuf,elen) ;
	            if (el == SR_NOTFOUND) break ;
	            rs = el ;
	            if (rs >= 0) {
		        if_constexpr (f_mksysvars) {
	                    rs = sysvars_mksysvarsi(op,sip,ebuf) ;
		        } else {
		            rs = SR_NOENT ;
		        } /* end if_constexpr (f_mksysvars) */
	                if ((rs < 0) && isNotPresent(rs)) {
	                    rs = sysvars_indmk(op,ebuf) ;
		        } /* end if (error) */
	                if (rs >= 0) {
	                    if ((rs = mkpath2(indname,ebuf,op->dbname)) >= 0) {
	                        rs = var_open(op->vindp,indname) ;
	                        op->fl.var = (rs >= 0) ;
	                    }
	                } /* end if (ok) */
	            } /* end if */
	            if ((rs >= 0) || (! isNotPresent(rs))) break ;
	        } /* end while */
	        rs1 = dirseen_curend(dsp,&cur) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (cursor) */
	    rs1 = libmem.free(a) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return rs ;
} /* end subroutine (sysvars_indopenalt) */

local int sysvars_indmk(SVS *op,cchar *dname) noex {
	cint		rsn = SR_NOTFOUND ;
	int		rs ;
	int		rs1 ;
	int		c = 0 ;
	/* check the given directory for writability */
	if ((rs = checkdname(dname)) == rsn) {
	    rs = mkdirs(dname,0775) ;
	}
	/* create the index-name */
	if (rs >= 0) ylikely {
	    if (char *ibuf ; (rs = lm_mp(&ibuf)) >= 0) ylikely {
	        cmode	om = 0664 ;
	        if ((rs = mkpath(ibuf,dname,op->dbname)) >= 0) ylikely {
	            rs = sysvars_indmkdata(op,ibuf,om) ;
	      	    c += rs ;
	    	}
		rs1 = lm_free(ibuf) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (m-a-f) */
	} /* end if (ok) */
	return (rs >= 0) ? c : rs ;
} /* end subroutine (sysvars_indmk) */

local int sysvars_indmkdata(SVS *op,cchar *indname,mode_t om) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		c = 0 ;
	if (op) ylikely {
	    cint	hn = DEFNVARS ;
	    if (hdbstr vt ; (rs = hdbstr_start(&vt,hn)) >= 0) ylikely {
	        bool	f  = false ;
	        for (int i = 0 ; sysfnames[i] != nullptr ; i += 1) {
	            rs = sysvars_procget(&vt,sysfnames[i]) ;
	            f = false ;
	            f = f || (rs == SR_NOENT) ;
	            f = f || (rs == SR_ACCESS) ;
	            if (f) rs = SR_OK ;
	            c += rs ;
	            if (rs < 0) break ;
	        } /* end for */
	        if (rs >= 0) {
	            rs = sysvars_procset(&vt,indname,om) ;
	        }
	        rs1 = hdbstr_finish(&vt) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (hdbstr) */
	} /* end if (non-null) */
	return (rs >= 0) ? c : rs ;
} /* end subroutine (sysvars_indmkdata) */

local int sysvars_indclose(SVS *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (op->fl.var) {
	    op->fl.var = false ;
	    rs1 = var_close(op->vindp) ;
	    if (rs >= 0) rs = rs1 ;
	}
	return rs ;
} /* end subroutine (sysvars_indclose) */

/* make the index */
local int sysvars_mksysvarsi(SVS *op,SI *sip,cchar *dname) noex {
    	cnullptr	np{} ;
	cint		maxpath = vdata.maxpathlen ;
	cint		sz = (2 * (vdata.maxpathlen + 1)) ;
	int		rs ;
	int		rs1 ;
	int		ai = 2 ; /* double memory allocation */
	cchar		*prog = PROG_MKSYSVARSI ;
	if (char *a ; (rs = libmem.mall(sz,&a)) >= 0) {
	    char	*dbname	= (a + (--ai * (maxpath + 1))) ;
	    if ((rs = mkpath2(dbname,dname,op->dbname)) >= 0) {
	        if ((rs = subinfo_ids(sip)) >= 0) {
	            int		rs_last = SR_NOTFOUND ;
	            int		pl = 0 ;
		    cchar	*pr = op->pr ;
	            char	*pbuf = (a + (--ai * (maxpath + 1))) ;
	            for (int i = 0 ; prbins[i] ; i += 1) {
	                if ((rs = mkpath3(pbuf,pr,prbins[i],prog)) >= 0) {
	                    pl = rs ;
	        	    if (ustat sb ; (rs = u_stat(pbuf,&sb)) >= 0) {
	                        if ((rs = permid(&sip->id,&sb,X_OK)) >= 0) {
				    rs = 0 ;
			        } else if (isNotPresent(rs)) {
	                            rs_last = rs ;
	                            pl = 0 ;
	                            rs = SR_OK ;
	                        }
	                    } else if (isNotPresent(rs)) {
	                        rs_last = rs ;
	                        pl = 0 ;
	                        rs = SR_OK ;
	                    }
	                } /* end if (mkpath) */
	                if (pl > 0) break ;
	                if (rs >= 0) break ;
	            } /* end for */
	            if ((rs >= 0) && (pl == 0)) {
		        rs = rs_last ;
		    }
	            if (rs >= 0) ylikely {
	               vecstr	envs ;
		       cint	vn = 20 ;
	               cint	vo = vecstrm.compact ;
	               if ((rs = vecstr_start(&envs,vn,vo)) >= 0) {
	                   cchar	*cp ;
			   /* setup environment */
	                   if (rs >= 0) ylikely {
	                       rs = envs.envadd(VARSVPR,op->pr,-1) ;
	                   }
	                   if (rs >= 0) ylikely {
	                       rs = envs.envadd(VARSVDBNAME,dbname,-1) ;
	                   }
			   if (rs >= 0) ylikely {
	                       for (int i = 0 ; envdefs[i] ; i += 1) {
				   cchar	*evn = envdefs[i] ;
	                           if ((cp = getenv(evn)) != np) {
	                               rs = envs.envadd(evn,cp,-1) ;
	                           }
	                           if (rs < 0) break ;
	                       } /* end for */
			   } /* end if (ok) */
			   /* go */
	                   if (rs >= 0) ylikely {
	                       if (mainv ev ; (rs = envs.getvec(&ev)) >= 0) {
	                            SPAWNPROC_CON	ps{} ;
				    int		i = 0 ;
	                            cchar	*av[10] ;
	                            av[i++] = prog ;
	                            av[i++] = "-s" ;
	                            av[i++] = nullptr ;
	                            ps.opts |= SPAWNPROC_OIGNINTR ;
	                            ps.opts |= SPAWNPROC_OSETPGRP ;
	                            for (int j = 0 ; j < 3 ; j += 1) {
	                                if (j != 2) {
	                                    ps.disp[j] = SPAWNPROC_DCLOSE ;
	                                } else {
	                                    ps.disp[j] = SPAWNPROC_DINHERIT ;
	                                }
	                            } /* end for */
	                            if ((rs = spawnproc(&ps,pbuf,av,ev)) >= 0) {
	                                pid_t	cpid = rs ;
	                                int	cs = 0 ;
	                                if ((rs = u_waitpid(cpid,&cs,0)) >= 0) {
	                                    int	cex = 0 ;
	                                    if (WIFSIGNALED(cs)) {
	                                        rs = SR_UNATCH ;
	                                    }
	                                    if ((rs >= 0) && WIFEXITED(cs)) {
	                                        cex = WEXITSTATUS(cs) ;
	                                        if (cex != 0) rs = SR_LIBBAD ;
	                                    } /* end if */
	                                } /* end if (process finished) */
	                            } /* end if (spawnproc) */
	                        } /* end if (vecstr_getvec) */
	                    } /* end if (ok) */
	                    rs1 = vecstr_finish(&envs) ;
			    if (rs >= 0) rs = rs1 ;
	                } /* end if (vecstr) */
	            } /* end if (ok) */
	        } /* end if (subinfo_ids) */
	    } /* end if (mkpath) */
	    rs1 = libmem.free(a) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return rs ;
} /* end subroutine (sysvars_mksysvarsi) */

local int subinfo_start(SI *sip) noex {
	int		rs = SR_FAULT ;
	if (sip) ylikely {
	    rs = memclear(sip) ;
	    sip->daytime = time(nullptr) ;
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (subinfo_start) */

local int subinfo_ids(SI *sip) noex {
	int		rs = SR_FAULT ;
	if (sip) ylikely {
	    rs = SR_OK ;
	    if (! sip->fl.id) {
	        sip->fl.id = true ;
	        rs = ids_load(&sip->id) ;
	    }
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (subinfo_ids) */

local int subinfo_finish(SI *sip) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (sip) ylikely {
	    rs = SR_OK ;
	    if (sip->fl.id) {
	        sip->fl.id = false ;
	        rs1 = ids_release(&sip->id) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (subinfo_finish) */

local int checkdname(cchar *dname) noex {
	int		rs = SR_INVALID ;
	if (dname[0] != '/') ylikely {
	    if (ustat sb ; (rs = u_stat(dname,&sb)) >= 0) ylikely {
		rs = SR_NOTDIR ;
	        if (S_ISDIR(sb.st_mode)) ylikely {
	            rs = perm(dname,-1,-1,nullptr,W_OK) ;
	        } /* end if (directory) */
	    } /* end if (u_stat) */
	} /* end if */
	return rs ;
} /* end subroutine (checkdname) */

varer::operator int () noex {
    	int		rs ;
	if ((rs = bufsizeget(bufsize_mp)) >= 0) ylikely {
	    maxpathlen = rs ;
	} /* end if (bufsizeget) */
	return rs ;
} /* end method (varer::operator) */

local int havefile(cchar *fn) noex {
    	int		rs ;
	int		f = false ; /* return-value */
	if (fn[0]) {
	    if (ustat sb ; (rs = u_stat(fn,&sb)) >= 0) {
		f = true ;
	    } else if (isNotPresent(rs)) {
		rs = SR_OK ;
	    } /* end if */
	} /* end if (valid) */
	return (rs >= 0) ? f : rs ;
} /* end subroutine (havefile) */


