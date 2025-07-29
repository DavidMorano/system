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
	This little object provides access to the SVS database and
	index (if any).  The "in-sequence" procedure for finding
	indices is:

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
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<climits>
#include	<ctime>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<new>
#include	<usystem.h>
#include	<getbufsize.h>
#include	<mallocxx.h>
#include	<strlibval.hh>
#include	<vecobj.h>
#include	<vecstr.h>
#include	<spawnproc.h>
#include	<storebuf.h>
#include	<ids.h>
#include	<hdbstr.h>
#include	<expcook.h>
#include	<dirseen.h>
#include	<pathclean.h>
#include	<sfx.h>
#include	<mkpathx.h>
#include	<mkdirs.h>
#include	<permx.h>
#include	<strwcpy.h>
#include	<char.h>
#include	<mkchar.h>
#include	<isnot.h>
#include	<localmisc.h>

#include	"sysvars.h"
#include	"var.h"

import libutil ;

/* local defines */

#define	SVS		sysvars
#define	SVS_C		sysvars_cur

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

#ifndef	TMPVARDNAME
#define	TMPVARDNAME	"/var/tmp"
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

#define	SUBINFO		struct subinfo
#define	SUBINFO_FL	struct subinfo_flags


/* imported namespaces */

using std::nullptr_t ;			/* type */
using std::min ;			/* subroutine-template */
using std::max ;			/* subroutine-template */
using std::nothrow ;			/* constant */


/* local typedefs */


/* external subroutines */

extern "C" {
    extern int	sysvars_procget(hdbstr *,cchar *) noex ;
    extern int	sysvars_procset(hdbstr *,cchar *,int) noex ;
}


/* external variables */


/* local structures */

struct subinfo_flags {
	uint		id:1 ;
} ;

struct subinfo {
	ids		id ;
	SUBINFO_FL	f ;
	time_t		daytime ;
} ;


/* local structures */


/* forward references */

template<typename ... Args>
static int sysvars_ctor(sysvars *op,Args ... args) noex {
    	SYSVARS		*hop = op ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    cnullptr	np{} ;
	    memclear(hop) ;
	    rs = SR_NOMEM ;
	    if ((op->vindp = new(nothrow) var) != np) {
		rs = SR_OK ;
	    } /* end if (new-var) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (sysvars_ctor) */

static int sysvars_dtor(sysvars *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	    if (op->vindp) {
		delete op->vindp ;
		op->vindp = nullptr ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (sysvars_dtor) */

template<typename ... Args>
static inline int sysvars_magic(sysvars *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magic == SYSVARS_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}
/* end subroutine (sysvars_magic) */

static int	sysvars_infoloadbegin(SVS *,cchar *,cchar *) noex ;
static int	sysvars_infoloadend(SVS *) noex ;
static int	sysvars_indopen(SVS *,SUBINFO *) noex ;

static int	sysvars_indclose(SVS *) noex ;
static int	sysvars_indmk(SVS *,cchar *) noex ;
static int	sysvars_indmkdata(SVS *,cchar *,mode_t) noex ;
static int	sysvars_indopenseq(SVS *,SUBINFO *) noex ;
static int	sysvars_indopenseqer(SVS *,SUBINFO *,dirseen *,
			vecstr *,expcook *) noex ;
static int	sysvars_loadcooks(SVS *,expcook *) noex ;
static int	sysvars_indopenalt(SVS *,SUBINFO *,dirseen *) noex ;

#if	CF_MKSYSVARS
static int	sysvars_mksysvarsi(SVS *,SUBINFO *,cchar *) noex ;
#endif

static int	subinfo_start(SUBINFO *) noex ;
static int	subinfo_ids(SUBINFO *) noex ;
static int	subinfo_finish(SUBINFO *) noex ;

static int	checkdname(cchar *) noex ;

#ifdef	COMMENT
static int	mkindfname(char *,cchar *,cchar *,cchar *,cchar *) noex ;
#endif


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
} ;
#endif /* CF_MKSYSVARS */

/* use fixed locations for security reasons (like we care!) */
#if	CF_MKSYSVARS
constexpr cpcchar	prbins[] = {
	"bin",
	"sbin",
	nullptr
} ;
#endif /* CF_MKSYSVARS */

constexpr cpcchar	sysfnames[] = {
	DEFINITFNAME,	
	DEFLOGFNAME,
	nullptr
} ;

constexpr cpcchar	dbdirs[] = {
	"%R/var",
	"/var/tmp/%{PRN}",
	"/var/tmp",
	"/tmp/%{PRN}",
	"/tmp",
	"%T/%{PRN}",
	"%T",
	nullptr
} ;

static strlibval	tmpdname(strlibval_tmpdir) ;

cbool			f_mksysvars = CF_MKSYSVARS ;


/* exported variables */

const sysvars_obj		sysvars_modinfo = {
	"sysvars",
	szof(sysvars),
	szof(sysvars_cur)
} ;


/* exported subroutines */

int sysvars_open(SVS *op,cchar *pr,cchar *dbname) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if ((dbname == nullptr) || (dbname[0] == '\0')) dbname = INDNAME ;
	if ((rs = sysvars_ctor(op,pr)) >= 0) {
	    rs = SR_INVALID ;
	    if (pr[0]) {
	        if (SUBINFO si ; (rs = subinfo_start(&si)) >= 0) {
	            if ((rs = sysvars_infoloadbegin(op,pr,dbname)) >= 0) {
	                if ((rs = sysvars_indopen(op,&si)) >= 0) {
	            	    op->magic = SYSVARS_MAGIC ;
			}
	                if (rs < 0) {
	                    sysvars_infoloadend(op) ;
			}
	            } /* end if (infoload) */
	            rs1 = subinfo_finish(&si) ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (subinfo) */
	    } /* end if (valid) */
	    if (rs < 0) {
		sysvars_dtor(op) ;
	    }
	} /* end if (sysvars_ctor) */
	return rs ;
}
/* end subroutine (sysvars_open) */

int sysvars_close(SVS *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = sysvars_magic(op)) >= 0) {
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
	    op->magic = 0 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (sysvars_close) */

int sysvars_audit(SVS *op) noex {
	int		rs ;
	if ((rs = sysvars_magic(op)) >= 0) {
	    rs = var_audit(op->vindp) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (sysvars_audit) */

int sysvars_curbegin(SVS *op,SVS_C *curp) noex {
	int		rs ;
	if ((rs = sysvars_magic(op,curp)) >= 0) {
	    cint	osz = szof(var) ;
	    memclear(curp) ;
	    if (void *vp{} ; (rs = uc_malloc(osz,&vp)) >= 0) {
		var_cur		*vcp = (var_cur *) vp ;
	        if ((rs = var_curbegin(op->vindp,vcp)) >= 0) {
		    curp->vcurp = vcp ;
	            op->ncursors += 1 ;
	        } /* end if (var_curbegin) */
	    } /* end if (memory-allocation) */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (sysvars_curbegin) */

int sysvars_curend(SVS *op,SVS_C *curp) noex {
	int		rs ;
	if ((rs = sysvars_magic(op,curp)) >= 0) {
	    rs = SR_BUGCHECK ;
	    if (curp->vcurp) {
	        var_cur		*vcp = (var_cur *) curp->vcurp ;
	        if ((rs = var_curend(op->vindp,vcp)) >= 0) {
	            op->ncursors -= 1 ;
	        }
	    } /* end if (valid) */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (sysvars_curend) */

int sysvars_fetch(SVS *op,cc *kp,int kl,SVS_C *curp,char *vbuf,int vlen) noex {
	int		rs ;
	if ((rs = sysvars_magic(op,kp,curp)) >= 0) {
	    rs = SR_BUGCHECK ;
	    if (curp->vcurp) {
	        var_cur		*vcp = (var_cur *) curp->vcurp ;
		rs = var_fetch(op->vindp,kp,kl,vcp,vbuf,vlen) ;
	    }
	} /* end if (magic) */
	if ((rs < 0) && vbuf) {
	    vbuf[0] = '\0' ;
	}
	return rs ;
}
/* end subroutine (sysvars_fetch) */

int sysvars_curenum(SVS *op,SVS_C *curp,char *kp,int kl,char *vp,int vl) noex {
	int		rs = SR_FAULT ;
	if ((rs = sysvars_magic(op,curp,kp)) >= 0) {
	    rs = SR_BUGCHECK ;
	    if (curp->vcurp) {
	        var_cur		*vcp = (var_cur *) curp->vcurp ;
		rs = var_enum(op->vindp,vcp,kp,kl,vp,vl) ;
	    } /* end if (open) */
	} /* end if (non-null) */
	if ((rs < 0) && vp) {
	    vp[0] = '\0' ;
	}
	return rs ;
}
/* end subroutine (sysvars_curenum) */

int sysvars_count(SVS *op) noex {
	int		rs = SR_FAULT ;
	if ((rs = sysvars_magic(op)) >= 0) {
	    rs = var_count(op->vindp) ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (sysvars_count) */


/* private subroutines */

static int sysvars_infoloadbegin(SVS *op,cchar *pr,cchar *dbname) noex {
	int		rs ;
	int		sz = 0 ;
	sz += (lenstr(pr) + 1) ;
	sz += (lenstr(dbname) + 1) ;
	if (char *bp{} ; (rs = uc_malloc(sz,&bp)) >= 0) {
	    op->a = bp ;
	    op->pr = bp ;
	    bp = (strwcpy(bp,pr,-1)+1) ;
	    op->dbname = bp ;
	    bp = (strwcpy(bp,dbname,-1)+1) ;
	}
	return rs ;
}
/* end subroutine (sysvars_infoloadbegin) */

static int sysvars_infoloadend(SVS *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (op->a) {
	    rs1 = uc_free(op->a) ;
	    if (rs >= 0) rs = rs1 ;
	    op->a = nullptr ;
	}
	op->pr = nullptr ;
	op->dbname = nullptr ;
	return rs ;
}
/* end subroutine (sysvars_infoloadend) */

static int sysvars_indopen(SVS *op,SUBINFO *sip) noex {
	return sysvars_indopenseq(op,sip) ;
}
/* end subroutine (sysvars_indopen) */

static int sysvars_indopenseq(SVS *op,SUBINFO *sip) noex {
	int		rs ;
	int		rs1 ;
	if (dirseen ds ; (rs = dirseen_start(&ds)) >= 0) {
	    expcook	cooks ;
	    cint	vn = 6 ;
	    cint	vo = VECSTR_OCOMPACT ;
	    if (vecstr sdirs ; (rs = vecstr_start(&sdirs,vn,vo)) >= 0) {
	        if ((rs = expcook_start(&cooks)) >= 0) {
		    if ((rs = sysvars_loadcooks(op,&cooks)) >= 0) {
			rs = sysvars_indopenseqer(op,sip,&ds,&sdirs,&cooks) ;
		    }
		    rs1 = expcook_finish(&cooks) ;
		    if (rs >= 0) rs = rs1 ;
		} /* end if (expcooks_loadcooks) */
		rs1 = vecstr_finish(&sdirs) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (vecstr) */
	    rs1 = dirseen_finish(&ds) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (dirseen) */
	return rs ;
}
/* end subroutines (sysvars_indopenseq) */

static int sysvars_indopenseqer(SVS *op,SUBINFO *sip,dirseen *dsp,
		vecstr *sdp,expcook *ecp) noex {
	cint	elen = MAXPATHLEN ;
	int		rs = SR_OK ;
	int		rs1 ;
	int		i ;
	int		el, pl ;
	char		ebuf[MAXPATHLEN + 1] ;
	char		pbuf[MAXPATHLEN + 1] ;

/* first phase: expand possible directory paths */

	for (i = 0 ; (rs >= 0) && (dbdirs[i] != nullptr) ; i += 1) {
	    if ((rs = expcook_exp(ecp,0,ebuf,elen,dbdirs[i],-1)) > 0) {
	        el = rs ;
	        if ((rs = pathclean(pbuf,ebuf,el)) > 0) {
	            pl = rs ;
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
	            el = dirseen_curenum(dsp,&cur,ebuf,elen) ;
	            if (el == SR_NOTFOUND) break ;
	            rs = el ;
	            if (rs >= 0) {
	                if ((rs = mkpath2(pbuf,ebuf,op->dbname)) >= 0) {
	                    pl = rs ;
	                    rs = vecstr_add(sdp,pbuf,pl) ;
			}
	            }

	        } /* end while */

	        dirseen_curend(dsp,&cur) ;
	    } /* end if (cursor) */

	    } else {
		rs = vecstr_add(sdp,op->dbname,-1) ;
	    } /* end if */

	} /* end if (ok) */

/* final phase: try to open all of them in-sequence */

	if (rs >= 0) {
	    if (mainv dv ; (rs = vecstr_getvec(sdp,&dv)) >= 0) {
	        if ((rs = var_opena(op->vindp,dv)) >= 0) {
	            op->f.var = true ;
		}
	        if ((rs < 0) && isNotPresent(rs)) {
	            rs = sysvars_indopenalt(op,sip,dsp) ;
		}
	    }
	} /* end if (ok) */

	return rs ;
}
/* end subroutine (sysvars_indopenseqer) */

static int sysvars_loadcooks(SVS *op,expcook *ecp) noex {
	int		rs = SR_OK ;
	cchar		*ks = "RST" ;
	char		kbuf[2] = {} ;
	for (int i = 0 ; (rs >= 0) && (ks[i] != '\0') ; i += 1) {
	    cint	kch = mkchar(ks[i]) ;
	    int		vl = -1 ;
	    cchar	*vp = nullptr ;
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
		kbuf[0] = charconv(kch) ;
		rs = expcook_add(ecp,kbuf,vp,vl) ;
	    }
	} /* end for */
	if (rs >= 0) {
	    if (cchar *prname{} ; (rs = sfbasename(op->pr,-1,&prname)) >= 0) {
	        rs = SR_NOENT ;
	        if (prname) {
	            rs = expcook_add(ecp,"PRN",prname,-1) ;
		}
	    }
	} /* end if (ok) */
	return rs ;
}
/* end subroutines (sysvars_loadcooks) */

static int sysvars_indopenalt(SVS *op,SUBINFO *sip,dirseen *dsp) noex {
	int		rs ;
	int		rs1 ;
	if (dirseen_cur cur ; (rs = dirseen_curbegin(dsp,&cur)) >= 0) {
	    cint	elen = MAXPATHLEN ;
	    int		el ;
	    char	ebuf[MAXPATHLEN + 1] ;
	    char	indname[MAXPATHLEN + 1] ;

	    while (rs >= 0) {

	        el = dirseen_curenum(dsp,&cur,ebuf,elen) ;
	        if (el == SR_NOTFOUND) break ;
	        rs = el ;

	        if (rs >= 0) {

#if	CF_MKSYSVARS
	            rs = sysvars_mksysvarsi(op,sip,ebuf) ;
#else /* CF_MKSYSVARS */
		    rs = SR_NOENT ;
#endif /* CF_MKSYSVARS */

	            if ((rs < 0) && isNotPresent(rs)) {
	                rs = sysvars_indmk(op,ebuf) ;
		    }

	            if (rs >= 0) {
	                if ((rs = mkpath2(indname,ebuf,op->dbname)) >= 0) {
	                    rs = var_open(op->vindp,indname) ;
	                    op->f.var = (rs >= 0) ;
	                }
	            }

	        } /* end if */

	        if ((rs >= 0) || (! isNotPresent(rs))) break ;
	    } /* end while */

	    rs1 = dirseen_curend(dsp,&cur) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (cursor) */

	return rs ;
}
/* end subroutines (sysvars_indopenalt) */

static int sysvars_indmk(SVS *op,cchar *dname) noex {
	cint		rsn = SR_NOTFOUND ;
	int		rs ;
	int		c = 0 ;
/* check the given directory for writability */
	if ((rs = checkdname(dname)) == rsn) {
	    rs = mkdirs(dname,0775) ;
	}
/* create the index-name */
	if (rs >= 0) {
	    if (char *ibuf{} ; (rs = malloc_mp(&ibuf)) >= 0) {
	        cmode	om = 0664 ;
	        if ((rs = mkpath(ibuf,dname,op->dbname)) >= 0) {
	            rs = sysvars_indmkdata(op,ibuf,om) ;
	      	    c += rs ;
	    	}
		rs = rsfree(rs,ibuf) ;
	    } /* end if (m-a-f) */
	} /* end if (ok) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (sysvars_indmk) */

static int sysvars_indmkdata(SVS *op,cchar *indname,mode_t om) noex {
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
}
/* end subroutine (sysvars_indmkdata) */

static int sysvars_indclose(SVS *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (op->f.var) {
	    op->f.var = false ;
	    rs1 = var_close(op->vindp) ;
	    if (rs >= 0) rs = rs1 ;
	}
	return rs ;
}
/* end subroutine (sysvars_indclose) */

/* make the index */
#if	CF_MKSYSVARS
static int sysvars_mksysvarsi(SVS *op,SUBINFO *sip,cchar *dname) noex {
    	cnullptr	np{} ;
	int		rs ;
	int		rs1 ;
	cchar		*prog = PROG_MKSYSVARSI ;
	char		dbname[MAXPATHLEN + 1] ;

	if (dname == nullptr) return SR_FAULT ;

	if (dname[0] == '\0') return SR_INVALID ;

	if ((rs = mkpath2(dbname,dname,op->dbname)) >= 0) {
	    if ((rs = subinfo_ids(sip)) >= 0) {
	        USTAT	sb ;
	        int		rs_last = SR_NOTFOUND ;
	        int		pl = 0 ;
		cchar		*pr = op->pr ;
	        char		pbuf[MAXPATHLEN + 1] ;

	        for (int i = 0 ; prbins[i] != nullptr ; i += 1) {
	            if ((rs = mkpath3(pbuf,pr,prbins[i],prog)) >= 0) {
	                pl = rs ;
	                if ((rs = u_stat(pbuf,&sb)) >= 0) {
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

	        if (rs >= 0) {
	            vecstr	envs ;
		    cint	vn = 20 ;
	            cint	vo = VECSTR_OCOMPACT ;
	            if ((rs = vecstr_start(&envs,vn,vo)) >= 0) {
	                cchar	*cp ;

/* setup environment */

	                if (rs >= 0) {
	                    rs = vecstr_envadd(&envs,VARSVPR,op->pr,-1) ;
	                }
	                if (rs >= 0) {
	                    rs = vecstr_envadd(&envs,VARSVDBNAME,dbname,-1) ;
	                }
			if (rs >= 0) {
	                    for (int i = 0 ; envdefs[i] ; i += 1) {
				cchar	*evn = envdefs[i] ;
	                        if ((cp = getenv(evn)) != np) {
	                            rs = vecstr_envadd(&envs,evn,cp,-1) ;
	                        }
	                        if (rs < 0) break ;
	                    } /* end for */
			} /* end if (ok) */

/* go */

	                if (rs >= 0) {
	                    if (mainv ev ; (rs = envs.getvec(&ev)) >= 0) {
	                        SPAWNPROC_CON	ps{} ;
	                        int		i = 0 ;
	                        cchar		*av[10] ;
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
	                            int		cs = 0 ;
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

	return rs ;
}
/* end subroutine (sysvars_mksysvarsi) */
#endif /* CF_MKSYSVARS */

static int subinfo_start(SUBINFO *sip) noex {
	int		rs = SR_FAULT ;
	if (sip) {
	    rs = memclear(sip) ;
	    sip->daytime = time(nullptr) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (subinfo_start) */

static int subinfo_ids(SUBINFO *sip) noex {
	int		rs = SR_FAULT ;
	if (sip) {
	    rs = SR_OK ;
	    if (! sip->f.id) {
	        sip->f.id = true ;
	        rs = ids_load(&sip->id) ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (subinfo_ids) */

static int subinfo_finish(SUBINFO *sip) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (sip) {
	    rs = SR_OK ;
	    if (sip->f.id) {
	        sip->f.id = false ;
	        rs1 = ids_release(&sip->id) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (subinfo_finish) */

static int checkdname(cchar *dname) noex {
	int		rs = SR_OK ;
	if (dname[0] != '/') {
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


