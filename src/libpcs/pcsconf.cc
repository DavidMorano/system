/* pcsconf SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* load management and interface for the PCSCONFS object */
/* version %I% last-modified %G% */


/* revision history:

	= 1992-03-10, David A­D­ Morano
	This module was originally written.

	= 1998-04-03, David A­D­ Morano
	This was modified for more general use.

	= 2008-10-07, David A­D­ Morano
	This was modified to allow for the main part to be a loadable
	module.

*/

/* Copyright © 1992,1998,2008 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Onject:
	pcsconf

	Description:
	This module implements an interface (a trivial one) that
	provides access to the PCSCONFS object (which is dynamically
	loaded).

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<dlfcn.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<clanguage.h>
#include	<usysbase.h>
#include	<usyscalls.h>
#include	<uclibmem.h>
#include	<getbufsize.h>
#include	<getnodedomain.h>
#include	<getusername.h>
#include	<getpwx.h>
#include	<getax.h>
#include	<getpwx.h>
#include	<vecstr.h>
#include	<modload.h>
#include	<nulstr.h>
#include	<uinfo.h>
#include	<expcook.h>
#include	<snx.h>
#include	<sfx.h>
#include	<strwcpy.h>
#include	<snwcpy.h>
#include	<matstr.h>
#include	<mkchar.h>
#include	<localmisc.h>

#include	"pcsconf.h"
#include	"pcsconfs.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| */

/* local defines */

#define	PC		pcsconf
#define	PC_CUR		pcsconf_cur
#define	PC_CURMAGIC	0x97677247
#define	PC_MODBNAME	"pcsconfs"
#define	PC_OBJNAME	"pcsconfs"

#define	CM		cookmgr

#define	LIBCNAME	"lib"

#ifndef	VARLIBPATH
#define	VARLIBPATH	"LD_LIBRARY_PATH"
#endif

#ifndef	SYMNAMELEN
#define	SYMNAMELEN	60
#endif

#ifndef	EBUFLEN
#define	EBUFLEN		(6 * MAXPATHLEN)
#endif


/* namespaces */

using libuc::libmem ;			/* variable */


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct variables {
	int	nodenamelen ;
	int	hostnamelen ;
	int	usernamelen ;
	int	symnamelen ;
	operator int () noex ;
    } ; /* end struct (variables) */
} /* end namespace */

struct cookmgr_flags {
	uint		cooks:1 ;
	uint		uname:1 ;
	uint		uaux:1 ;
} ; /* end struct */

struct cookmgr_nd {
	cchar		*nodename ;
	cchar		*domname ;
	char		*a ;		/* allocation */
} ; /* end struct */

struct cookmgr {
	expcook		cooks ;
	uinfo_names	uname ;
	uinfo_auxs	uaux ;
	cchar		*pr ;
	cookmgr_nd	nd ;
	cookmgr_flags	fl ;
} ; /* end struct (cookmgr) */


/* forward references */

template<typename ... Args>
local int pcsconf_ctor(pcsconf *op,Args ... args) noex {
    	PCSCONF		*hop = op ;
	cnullptr	np{} ;
	cnothrow	nt{} ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = SR_NOMEM ;
	    memclear(hop) ; /* dangerous */
	    if ((op->mlp = new(nt) modload) != np) {
	        if ((op->mxp = new(nt) ptm) != np) {
		    rs = SR_OK ;
		} /* end if (new-ptm) */
		if (rs < 0) {
		    delete op->mlp ;
		    op->mlp = nullptr ;
		}
	    } /* end if (new-modload) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (pcsconf_ctor) */

local int pcsconf_dtor(pcsconf *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	    if (op->mxp) {
		delete op->mxp ;
		op->mxp = nullptr ;
	    }
	    if (op->mlp) {
		delete op->mlp ;
		op->mlp = nullptr ;
	    }
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (pcsconf_dtor) */

template<typename ... Args>
local inline int pcsconf_magic(pcsconf *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magic == PCSCONF_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
} /* end subroutine (pcsconf_magic) */

local int	pcsconf_objloadbegin(PC *,cchar *,cchar *) noex ;
local int	pcsconf_objloadend(PC *) noex ;
local int	pcsconf_modloadopen(PC *,cchar *,cchar *) noex ;
local int	pcsconf_loadcalls(PC *,cchar *) noex ;
local int	pcsconf_getpcsids(PC *) noex ;
local int	pcsconf_getpcspw(PC *) noex ;
local int	pcsconf_expand(PC *,char *,int,int) noex ;
local int	pcsconf_cookmgr(PC *) noex ;
local int	pcsconf_cookload(PC *,cchar *,int) noex ;

local int	cookmgr_start(CM *,cchar *) noex ;
local int	cookmgr_load(CM *,cchar *,int) noex ;
local int	cookmgr_expand(CM *,char *,int,cchar *,int) noex ;
local int	cookmgr_uname(CM *) noex ;
local int	cookmgr_uaux(CM *) noex ;
local int	cookmgr_nodedomain(CM *) noex ;
local int	cookmgr_finish(CM *) noex ;

local bool	isrequired(int) noex ;


/* global variables */


/* local variables */

enum subs {
	sub_start,
	sub_curbegin,
	sub_fetch,
	sub_enum,
	sub_curend,
	sub_audit,
	sub_finish,
	sub_overlast
} ; /* end enum */

constexpr cpcchar	subs[] = {
	"start",
	"curbegin",
	"fetch",
	"enum",
	"curend",
	"audit",
	"finish",
	nullptr
} ; /* end array */

enum cooks {
	cook_sysname,
	cook_nodename,
	cook_release,
	cook_version,
	cook_machine,
	cook_architecture,
	cook_platform,
	cook_provider,
	cook_hwserial,
	cook_nisdomain,
	cook_n,
	cook_d,
	cook_h,
	cook_u,
	cook_r,
	cook_rn,
	cook_overlast
} ; /* end enum */

constexpr cpcchar	cooks[] = {
	"sysname",
	"nodename",
	"release",
	"version",
	"machine",
	"architecture",
	"platform",
	"provider",
	"hwserial",
	"nisdomain",
	"N",
	"D",
	"H",
	"U",
	"R",
	"PN",
	nullptr
} ; /* end array */

constexpr uid_t		uidend = uid_t(-1) ;

static variables	sysvar ;


/* exported variables */


/* exported subroutines */

int pcsconf_start(PC *op,cchar *pr,mainv envv,cchar *cfname) noex {
	int		rs ;
	if ((rs = pcsconf_ctor(op,pr,envv,cfname)) >= 0) {
	    rs = SR_INVALID ;
	    if (pr[0] && cfname[0]) {
	        static cint	rsv = sysvar ;
		if ((rs = rsv) >= 0) {
	            cchar	*objname = PC_OBJNAME ;
	            op->pr = pr ;
	            op->envv = envv ;
	            op->uid_pcs = -1 ;
	            if ((rs = pcsconf_objloadbegin(op,pr,objname)) >= 0) {
	                ptm *mxp = op->mxp ;
	                if ((rs = mxp->create) >= 0) {
	                    rs = (*op->call.start)(op->obj,pr,envv,cfname) ;
	                    if (rs >= 0) {
		                op->magic = PCSCONF_MAGIC ;
	                    }
	                    if (rs < 0) {
		                mxp->destroy() ;
		            }
	                } /* end if (ptm) */
	                if (rs < 0) {
		            pcsconf_objloadend(op) ;
	                }
	            } /* end if (obj-mod loading) */
		} /* end if (vars) */
	    } /* end if (valid) */
	    if (rs < 0) {
		pcsconf_dtor(op) ;
	    } /* end if (error) */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (pcsconf_start) */

int pcsconf_finish(PC *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = pcsconf_magic(op)) >= 0) {
	    if (op->pcsusername != nullptr) {
		void *vp = voidp(op->pcsusername) ;
	        rs1 = lm_free(vp) ;
	        if (rs >= 0) rs = rs1 ;
	        op->pcsusername = nullptr ;
	    }
	    if (op->cookmgr != nullptr) {
		cookmgr *cmp = (cookmgr *) op->cookmgr ;
	        {
	            rs1 = cookmgr_finish(cmp) ;
	            if (rs >= 0) rs = rs1 ;
	        }
	        {
	            rs1 = lm_free(cmp) ;
	            if (rs >= 0) rs = rs1 ;
	            op->cookmgr = nullptr ;
	        }
	    }
	    {
	        rs1 = (*op->call.finish)(op->obj) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
	        ptm *mxp = op->mxp ;
	        rs1 = mxp->destroy ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = pcsconf_objloadend(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = pcsconf_dtor(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    op->magic = 0 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (pcsconf_finish) */

int pcsconf_audit(PC *op) noex {
	int		rs ;
	int		rs1 ;
	int		vl = 0 ;
	if ((rs = pcsconf_magic(op)) >= 0) {
	    if (op->call.audit != nullptr) {
	        ptm *mxp = op->mxp ;
	        if ((rs = mxp->lockbegin) >= 0) {
		    {
	                rs = (*op->call.audit)(op->obj) ;
		        vl = rs ;
	            }
		    rs1 = mxp->lockend ;
		    if (rs >= 0) rs = rs1 ;
	        } /* end if (mutex) */
	    } /* end if (have method) */
	} /* end if (magic) */
	return (rs >= 0) ? vl : rs ;
}
/* end subroutine (pcsconf_audit) */

int pcsconf_getpcsuid(PC *op) noex {
	int		rs ;
	int		rs1 ;
	int		v = 0 ;
	if ((rs = pcsconf_magic(op)) >= 0) {
	    ptm *mxp = op->mxp ;
	    if ((rs = mxp->lockbegin) >= 0) {
	        if ((rs = pcsconf_getpcsids(op)) >= 0) {
	            v = op->uid_pcs ;
	        }
	        rs1 = mxp->lockend ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (mutex) */
	} /* end if (magic) */
	return (rs >= 0) ? v : rs ;
}
/* end subroutine (pcsconf_getpcsuid) */

int pcsconf_getpcsgid(PC *op) noex {
	int		rs ;
	int		rs1 ;
	int		v = 0 ;
	if ((rs = pcsconf_magic(op)) >= 0) {
	    ptm *mxp = op->mxp ;
	    if ((rs = mxp->lockbegin) >= 0) {
	        if ((rs = pcsconf_getpcsids(op)) >= 0) {
	            v = op->gid_pcs ;
	        }
	        rs1 = mxp->lockend ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (mutex) */
	} /* end if (magic) */
	return (rs >= 0) ? v : rs ;
}
/* end subroutine (pcsconf_getpcsgid) */

int pcsconf_getpcsusername(PC *op,char *ubuf,int ulen) noex {
	int		rs ;
	int		rs1 ;
	int		vl = 0 ;
	if ((rs = pcsconf_magic(op,ubuf)) >= 0) {
	    ptm *mxp = op->mxp ;
	    if ((rs = mxp->lockbegin) >= 0) {
	        if (op->pcsusername == nullptr) {
		    rs = pcsconf_getpcspw(op) ;
		}
	        if (rs >= 0) {
		    if (op->pcsusername != nullptr) {
		        rs = sncpy1(ubuf,ulen,op->pcsusername) ;
		        vl = rs ;
		    } else {
		        rs = SR_NOTFOUND ;
	            }
	        }
	        rs1 = mxp->lockend ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (mutex) */
	} /* end if (magic) */
	return (rs >= 0) ? vl : rs ;
}
/* end subroutine (pcsconf_getpcsusername) */

int pcsconf_getpr(PC *op,cchar **prp) noex {
	int		rs ;
	if ((rs = pcsconf_magic(op)) >= 0) {
	    rs = lenstr(op->pr) ;
	    if (prp) {
	        *prp = op->pr ;
	    }
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (pcsconf_getpr) */

int pcsconf_getenvv(PC *op,mainv *envvp) noex {
	int		rs ;
	if ((rs = pcsconf_magic(op)) >= 0) {
	    if (envvp) {
	        *envvp = op->envv ;
	    }
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (pcsconf_getenvv) */

int pcsconf_curbegin(PC *op,pcsconf_cur *curp) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = pcsconf_magic(op,curp)) >= 0) {
	    cint	csz = szof(PCSCONFS_CUR) ;
	    rs = SR_BUGCHECK ;
	    if (op->cursize == csz) {
	        memclear(curp) ;
	        if (op->call.curbegin != nullptr) {
	            ptm *mxp = op->mxp ;
	            if ((rs = mxp->lockbegin) >= 0) {
	                if (void *p ; (rs = lm_mall(csz,&p)) >= 0) {
		            curp->scp = p ;
	                    if ((rs = (*op->call.curbegin)
					(op->obj,curp->scp)) >= 0) {
		                curp->magic = PC_CURMAGIC ;
		            }
	                    if (rs < 0) {
	                        lm_free(curp->scp) ;
	                        curp->scp = nullptr ;
		            }
	                } /* end if (memory-allocation) */
		        rs1 = mxp->lockend ;
	                if (rs >= 0) rs = rs1 ;
	            } /* end if (mutex) */
	        } /* end if (have method) */
	    } /* end if (valid) */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (pcsconf_curbegin) */

int pcsconf_curend(PC *op,pcsconf_cur *curp) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = pcsconf_magic(op,curp)) >= 0) {
	    rs = SR_NOTOPEN ;
	    if (curp->magic == PC_CURMAGIC) {
	        if (curp->scp != nullptr) {
	            ptm *mxp = op->mxp ;
	            if ((rs = mxp->lockbegin) >= 0) {
	                if (op->call.curend != nullptr) {
	                    rs1 = (*op->call.curend)(op->obj,curp->scp) ;
		            if (rs >= 0) rs = rs1 ;
		        } else {
		            rs = SR_NOSYS ;
		        }
	                lm_free(curp->scp) ;
	                curp->scp = nullptr ;
		        rs1 = mxp->lockend ;
	                if (rs >= 0) rs = rs1 ;
	            } /* end if (mutex) */
	        } else {
	            rs = SR_BUGCHECK ;
	        }
	        curp->magic = 0 ;
	    } /* end if (valid) */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (pcsconf_curend) */

/* lookup tags by strings */
int pcsconf_curfetch(PC *op,cchar *kp,int kl,PC_CUR *curp,
		char *vbuf,int vlen) noex {
	int		rs = SR_NOSYS ;
	int		rs1 ;
	int		vl = 0 ;
	if ((rs = pcsconf_magic(op,curp,kp)) >= 0) {
	    rs = SR_NOTOPEN ;
	    if (curp->magic == PC_CURMAGIC) {
	        if (op->call.fetch != nullptr) {
	            ptm *mxp = op->mxp ;
	            if ((rs = mxp->lockbegin) >= 0) {
	                rs = (*op->call.fetch)
			    (op->obj,kp,kl,curp->scp,vbuf,vlen) ;
	                if (rs >= 0) {
	                    rs = pcsconf_expand(op,vbuf,vlen,rs) ;
		            vl = rs ;
		        }
		        rs1 = mxp->lockend ;
	                if (rs >= 0) rs = rs1 ;
	            } /* end if (mutex) */
	        } /* end if (have method) */
	    } /* end if (valid) */
	} /* end if (pcsconf_magic) */
	return (rs >= 0) ? vl : rs ;
}
/* end subroutine (pcsconf_curfetch) */

int pcsconf_curenum(PC *op,PC_CUR *curp,char *kbuf,int klen,
		char *vbuf,int vlen) noex {
	int		rs = SR_NOSYS ;
	int		rs1 ;
	int		vl = 0 ;
	if ((rs = pcsconf_magic(op,curp,kbuf)) >= 0) {
	    rs = SR_NOTOPEN ;
	    if (curp->magic == PC_CURMAGIC) {
	        if (op->call.enumerate != nullptr) {
	            ptm *mxp = op->mxp ;
	            if ((rs = mxp->lockbegin) >= 0) {
		        void	*cp = curp->scp ;
	                rs = (*op->call.enumerate)
			    (op->obj,cp,kbuf,klen,vbuf,vlen) ;
	                if (rs >= 0) {
	                    rs = pcsconf_expand(op,vbuf,vlen,rs) ;
		            vl = rs ;
		        }
		        rs1 = mxp->lockend ;
	                if (rs >= 0) rs = rs1 ;
	            } /* end if (mutex) */
	        } /* end if (have method) */
	    } /* end if (valid) */
	} /* end if (pcsconf_magic) */
	return (rs >= 0) ? vl : rs ;
}
/* end subroutine (pcsconf_curenum) */

int pcsconf_fetchone(PC *op,cchar *kp,int kl,char *vbuf,int vlen) noex {
	int		rs ;
	int		rs1 ;
	int		vl = 0 ;
	if ((rs = pcsconf_magic(op,kp,vbuf)) >= 0) {
	    if (PC_CUR cur ; (rs = pcsconf_curbegin(op,&cur)) >= 0) {
	        {
	            rs = pcsconf_curfetch(op,kp,kl,&cur,vbuf,vlen) ;
		    vl = rs ;
	        }
	        rs1 = pcsconf_curend(op,&cur) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (cursor) */
	} /* end if (pcsconf_magic) */
	return (rs >= 0) ? vl : rs ;
}
/* end subroutine (pcsconf_fetchone) */


/* private subroutines */

/* find and load the DB-access object */
local int pcsconf_objloadbegin(PC *op,cchar *pr,cchar *objname) noex {
	int		rs ;
	if ((rs = pcsconf_modloadopen(op,pr,objname)) >= 0) {
	    modload	*lp = op->mlp ;
	    if ((rs = modload_getmv(lp,0)) >= 0) {
	        op->objsize = rs ;
	        if ((rs = modload_getmv(lp,1)) >= 0) {
		    cint osz = op->objsize ;
	    	    op->cursize = rs ;
		    if (void *p ; (rs = lm_mall(osz,&p)) >= 0) {
		        op->obj = p ;
			rs = pcsconf_loadcalls(op,objname) ;
			if (rs < 0) {
	    		    lm_free(op->obj) ;
	    		    op->obj = nullptr ;
			}/* end if (error) */
		    } /* end if (memory-allocations) */
		} /* end if (getmv) */
	    } /* end if (getmv) */
	    if (rs < 0) {
		modload_close(lp) ;
	    }
	} /* end if (modloadopen) */
	return rs ;
}
/* end subroutine (pcsconf_objloadbegin) */

local int pcsconf_objloadend(PC *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (op->obj != nullptr) {
	    rs1 = lm_free(op->obj) ;
	    if (rs >= 0) rs = rs1 ;
	    op->obj = nullptr ;
	}
	{
	    rs1 = modload_close(op->mlp) ;
	    if (rs >= 0) rs = rs1 ;
	}
	return rs ;
}
/* end subroutine (pcsconf_objloadend) */

local int pcsconf_modloadopen(PC *op,cchar *pr,cchar *objname) noex {
	int		rs ;
	int		rs1 ;
	if (char *sbuf ; (rs = lm_sn(&sbuf)) >= 0) {
	    cint	slen = rs ;
	    cint	vn = nelem(subs) ;
	    cint	vo = vecstrm.compact ;
	    if (vecstr syms ; (rs = vecstr_start(&syms,vn,vo)) >= 0) {
	        modload	*lp = op->mlp ;
	        int		f_modload = false ;
	        cchar		*modbname ;
	        for (int i = 0 ; (i < vn) && subs[i] ; i += 1) {
	            if (isrequired(i)) {
	                rs = sncpy3(sbuf,slen,objname,"_",subs[i]) ;
	                if (rs >= 0)
	                    rs = vecstr_add(&syms,sbuf,rs) ;
	            }
	            if (rs < 0) break ;
	        } /* end for */
	        if (rs >= 0) {
	            if (mainv sv ; (rs = vecstr_getvec(&syms,&sv)) >= 0) {
		        const modloadms &mlm = modloadm ;
		        int		mo = 0 ;
	                modbname = PC_MODBNAME ;
		        mo |= mlm.libvar ;
		        mo |= mlm.libprs ;
		        mo |= mlm.libsdirs ;
	                rs = modload_open(lp,pr,modbname,objname,mo,sv) ;
		        f_modload = (rs >= 0) ;
		    } /* end if (vecstr_getvec) */
	        } /* end if (ok) */
	        rs1 = vecstr_finish(&syms) ;
	        if (rs >= 0) rs = rs1 ;
	        if ((rs < 0) && f_modload) {
		    modload_close(lp) ;
	        }
	    } /* end if (modload-open) */
	    rs1 = lm_free(sbuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return rs ;
}
/* end subroutine (pcsconf_modloadopen) */

local int pcsconf_loadcalls(PC *op,cchar *objname) noex {
	modload		*lp = op->mlp ;
	int		rs = SR_OK ;
	int		rs1 ;
	int		c = 0 ;
	char		symname[SYMNAMELEN + 1] ;
	cvoid		*snp ;

	for (int i = 0 ; subs[i] != nullptr ; i += 1) {

	    rs = sncpy3(symname,SYMNAMELEN,objname,"_",subs[i]) ;
	    if (rs < 0) break ;

	    rs1 = modload_getsym(lp,symname,&snp) ;

	    if (rs1 == SR_NOTFOUND) {
	        snp = nullptr ;
	        if (isrequired(i))
	            break ;
	    } else
	        rs = rs1 ;

	    if (rs < 0) break ;

	    if (snp != nullptr) {

	        c += 1 ;
	        switch (i) {

	        case sub_start:
	            op->call.start = (int (*)(void *,cchar *,mainv,
	                cchar *)) snp ;
	            break ;

	        case sub_curbegin:
	            op->call.curbegin = 
	                (int (*)(void *,void *)) snp ;
	            break ;

	        case sub_fetch:
	            op->call.fetch = 
	                (int (*)(void *,cchar *,int,void *,char *,int))
	                snp ;
	            break ;

	        case sub_enum:
	            op->call.enumerate = 
	                (int (*)(void *,void *,char *,int,char *,int)) snp ;
	            break ;

	        case sub_curend:
	            op->call.curend = 
	                (int (*)(void *,void *)) snp ;
	            break ;

	        case sub_audit:
	            op->call.audit = (int (*)(void *)) snp ;
	            break ;

	        case sub_finish:
	            op->call.finish = (int (*)(void *)) snp ;
	            break ;

	        } /* end switch */

	    } /* end if (it had the call) */

	} /* end for (subs) */

	return (rs >= 0) ? c : rs ;
}
/* end subroutine (pcsconf_loadcalls) */

local int pcsconf_getpcsids(PC *op) noex {
	int		rs = SR_OK ;
	if (op->uid_pcs != uidend) {
	    if (ustat sb ; (rs = u_stat(op->pr,&sb)) >= 0) {
		op->uid_pcs = sb.st_uid ;
		op->gid_pcs = sb.st_gid ;
	    } /* end if (stat) */
	} /* end if (needed) */
	return rs ;
}
/* end subroutine (pcsconf_getpcsids) */

local int pcsconf_getpcspw(PC *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (op->pcsusername == nullptr) {
	    const uid_t	uid_pcs = op->uid_pcs ;
	    cchar	*un = PCSCONF_USER ;
	    if ((rs = pcsconf_getpcsids(op)) >= 0) {
	        ucentpw	pw ;
	        if (char *pwbuf ; (rs = lm_pw(&pwbuf)) >= 0) {
		    cint pwlen = rs ;
	            if ((rs1 = getpwx_name(&pw,pwbuf,pwlen,un)) >= 0) {
		        if (pw.pw_uid != uid_pcs) rs1 = SR_NOTFOUND ;
	            } else if (rs1 != SR_NOTFOUND) {
		        rs = rs1 ;
		    }
		    if ((rs >= 0) && (rs1 == SR_NOTFOUND)) {
			const uid_t	u = uid_pcs ;
		        un = nullptr ;
		        if ((rs = getpwusername(&pw,pwbuf,pwlen,u)) >= 0) {
			    un = pw.pw_name ;
		        } /* end if (getusername) */
		    } /* end if */
		    if ((rs >= 0) && (un != nullptr)) {
		        if (cchar *cp ; (rs = lm_strw(un,-1,&cp)) >= 0) {
			    op->pcsusername = cp ;
		        } /* end if (memory-allocation) */
		    } /* end if (store pcs-username) */
		    lm_free(pwbuf) ;
		} /* end if (m-a) */
	    } /* end if (pcsconf-getpcsids) */
	} else {
	    rs = lenstr(op->pcsusername) ;
	}
	return rs ;
}
/* end subroutine (pcsconf_getpcspw) */

local int pcsconf_expand(PC *op,char *vbuf,int vlen,int vl) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	int		sl = vl ;
	int		f_havekeys = false ;
	cchar		*sp = (cchar *) vbuf ;
	cchar		*kp{} ;
	if (vlen < 0) {
	    vlen = EBUFLEN ;
	}
	for (int kl ; (kl = sfcookkey(sp,sl,&kp)) > 0 ; ) {
	    f_havekeys = true ;
	    {
	        rs = pcsconf_cookload(op,kp,kl) ;
	    }
	    sl -= intconv((kp + kl) - sp) ;
	    sp = (kp + kl) ;
	    if (rs < 0) break ;
	} /* end for */
	if ((rs >= 0) && f_havekeys) {
	    cint	elen = vlen ;
	    if (char *ebuf ; (rs = lm_mall((elen + 1),&ebuf)) >= 0) {
	        if ((rs = pcsconf_cookmgr(op)) >= 0) {
	            CM	*cmp = (CM *) op->cookmgr ;
	            if ((rs = cookmgr_expand(cmp,ebuf,elen,vbuf,vl)) >= 0) {
	                rs = snwcpy(vbuf,vlen,ebuf,rs) ;
	                vl = rs ;
	            }
	        } /* end if (cookmgr) */
	        rs1 = lm_free(ebuf) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (memory_allocation) */
	} /* end if (had keys) */
	return (rs >= 0) ? vl : rs ;
}
/* end subroutine (pcsconf_expand) */

local int pcsconf_cookmgr(PC *op) noex {
	int		rs = SR_OK ;
	if (op->cookmgr == nullptr) {
	    cint	osz = szof(CM) ;
	    if (void *p ; (rs = lm_mall(osz,&p)) >= 0) {
	        CM	*cmp = (CM *) p ;
	        if ((rs = cookmgr_start(cmp,op->pr)) >= 0) {
	            op->cookmgr = cmp ;
	        }
	        if (rs < 0) {
	            lm_free(p) ;
		}
	    } /* end if (memory-allocation) */
	} /* end if (needed to initialize CM) */
	return rs ;
}
/* end subroutine (pcsconf_cookmgr) */

local int pcsconf_cookload(PC *op,cchar *kp,int kl) noex {
	int		rs ;
	if ((rs = pcsconf_cookmgr(op)) >= 0) {
	    CM	*cmp = (CM *) op->cookmgr ;
	    rs = cookmgr_load(cmp,kp,kl) ;
	} /* end if (cook-manager started) */
	return rs ;
}
/* end subroutine (pcsconf_cookload) */

local int cookmgr_start(CM *cmp,cchar *pr) noex {
	int		rs ;
	{
	    memclear(cmp) ;
	    cmp->pr = pr ;
	}
	{
	    rs = expcook_start(&cmp->cooks) ;
	    cmp->fl.cooks = (rs >= 0) ;
	}
	return rs ;
}
/* end subroutine (cookmgr_start) */

local int cookmgr_load(CM *cmp,cchar *kp,int kl) noex {
    	cnullptr	np{} ;
	cint		rsn = SR_NOTFOUND ;
	int		rs ;
	int		rs1 ;
	if ((rs = expcook_findkey(&cmp->cooks,kp,kl,np)) == rsn) {
	    cchar	*kname = nullptr ;
	    if (nulstr ns ; (rs = ns.start(kp,kl,&kname)) >= 0) {
		if (char *ubuf ; (rs = lm_un(&ubuf)) >= 0) {
		    cint	ulen = rs ;
	            cint	ci = matstr(cooks,kp,kl) ;
	            int	vl = -1 ;
	            cchar	*vp = nullptr ;
	            char	*tbuf = nullptr ;
	            switch (ci) {
	            case cook_sysname:
	            case cook_nodename:
	            case cook_release:
	            case cook_version:
	            case cook_machine:
	                if ((rs = cookmgr_uname(cmp)) >= 0) {
	                    switch (ci) {
	                    case cook_sysname:
	                        vp = cmp->uname.sysname ;
	                        break ;
	                    case cook_nodename:
	                        vp = cmp->uname.nodename ;
	                        break ;
	                    case cook_release:
	                        vp = cmp->uname.release ;
	                        break ;
	                    case cook_version:
	                        vp = cmp->uname.version ;
	                        break ;
	                    case cook_machine:
	                        vp = cmp->uname.machine ;
	                        break ;
	                    } /* end switch */
	                } /* end if (uname) */
	                break ;
	            case cook_architecture:
	            case cook_platform:
	            case cook_provider:
	            case cook_hwserial:
	            case cook_nisdomain:
	                if ((rs = cookmgr_uaux(cmp)) >= 0) {
	                    switch (ci) {
	                    case cook_architecture:
	                        vp = cmp->uaux.architecture ;
	                        break ;
	                    case cook_platform:
	                        vp = cmp->uaux.platform ;
	                        break ;
	                    case cook_provider:
	                        vp = cmp->uaux.hwprovider ;
	                        break ;
	                    case cook_hwserial:
	                        vp = cmp->uaux.hwserial ;
	                        break ;
	                    case cook_nisdomain:
	                        vp = cmp->uaux.nisdomain ;
	                        break ;
	                    } /* end switch */
	                } /* end if (uname) */
	                break ;
	            case cook_n:
	            case cook_d:
	            case cook_h:
	                if ((rs = cookmgr_nodedomain(cmp)) >= 0) {
	                    switch (ci) {
	                    case cook_n:
	                        vp = cmp->nd.nodename ;
	                        break ;
	                    case cook_d:
	                        vp = cmp->nd.domname ;
	                        break ;
	                    case cook_h:
	                        {
	                            cchar	*nn = cmp->nd.nodename ;
	                            cchar	*dn = cmp->nd.domname ;
	                            int 	sz = 1 ;
	                            int	tlen = 0 ;
	                            tlen += (lenstr(nn)+1) ;
	                            tlen += (lenstr(dn)+1) ;
	                            sz += tlen ;
	                            if ((rs = lm_mall(sz,&tbuf)) >= 0) {
	                                vp = tbuf ;
	                                rs = snsds(tbuf,tlen,nn,dn) ;
	                                vl = rs ;
	                            }
	                        }
	                        break ;
	                    } /* end switch */
	                }
	                break ;
	            case cook_u:
	                if ((rs = getusername(ubuf,ulen,-1)) > 0) {
			    vl = rs ;
	                    vp = ubuf ;
		        }
	                break ;
	            case cook_r:
	                vp = cmp->pr ;
	                break ;
	            case cook_rn:
	                vl = sfbasename(cmp->pr,-1,&vp) ;
	                break ;
	            } /* end switch */
	            if ((rs >= 0) && (vp != nullptr))
	                rs = expcook_add(&cmp->cooks,kname,vp,vl) ;
	            if (tbuf != nullptr) {
		        lm_free(tbuf) ;
		    }
		    rs1 = lm_free(ubuf) ;
	            if (rs >= 0) rs = rs1 ;
		} /* end if (m-a-f) */
		rs1 = ns.finish ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (nulstr) */
	} /* end if (key not already found) */
	return rs ;
}
/* end subroutine (cookmgr_load) */

local int cookmgr_expand(CM *cmp,char *ebuf,int elen,cc *vbuf,int vl) noex {
	expcook		*ecp = (expcook *) &cmp->cooks ;
	cint		wch = MKCHAR('¿') ;
	int		rs ;
	{
	    rs = expcook_exp(ecp,wch,ebuf,elen,vbuf,vl) ;
	}
	return rs ;
}
/* end subroutine (cookmgr_expand) */

local int cookmgr_uname(CM *cmp) noex {
	int		rs = SR_OK ;
	if (! cmp->fl.uname) {
	    cmp->fl.uname = true ;
	    rs = uinfo_name(&cmp->uname) ;
	}
	return rs ;
}
/* end subroutine (cookmgr_uname) */

local int cookmgr_uaux(CM *cmp) noex {
	int		rs = SR_OK ;
	if (! cmp->fl.uaux) {
	    cmp->fl.uaux = true ;
	    rs = uinfo_aux(&cmp->uaux) ;
	}
	return rs ;
}
/* end subroutine (cookmgr_uaux) */

local int cookmgr_nodedomain(CM *cmp) noex {
    	cint		nlen = sysvar.nodenamelen ;
    	cint		hlen = sysvar.hostnamelen ;
	int		rs = SR_OK ;
	int		rs1 ;
	if (cmp->nd.a == nullptr) {
	    int sz = ((nlen + 1) + (hlen + 1)) ;
	    if (char *a ; (rs = lm_mall(sz,&a)) >= 0) {
	        char	*nn = (a + 0) ;
	        char	*dn = (a + (nlen + 1)) ;
	        if ((rs = getnodedomain(nn,dn)) >= 0) {
	            sz = 0 ;
	            sz += (lenstr(nn) + 1) ;
	            sz += (lenstr(dn) + 1) ;
	            if (char *bp ; (rs = lm_mall(sz,&bp)) >= 0) {
	                cmp->nd.a = bp ;
	                cmp->nd.nodename = bp ;
	                bp = (strwcpy(bp,nn,-1)+1) ;
	                bp = (strwcpy(bp,dn,-1)+1) ;
	            } /* end if (memory-allocation) */
	        } /* end if (get node-domain) */
	        rs1 = lm_free(a) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (m-a-f) */
	} /* end if (i-null) */
	return rs ;
}
/* end subroutine (cookmgr_nodedomain) */

local int cookmgr_finish(CM *cmp) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (cmp != nullptr) {
	    if (cmp->nd.a != nullptr) {
	        rs1 = lm_free(cmp->nd.a) ;
	        if (rs >= 0) rs = rs1 ;
	        cmp->nd.a = nullptr ;
	    }
	    if (cmp->fl.cooks) {
	        cmp->fl.cooks = false ;
	        rs1 = expcook_finish(&cmp->cooks) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (cookmgr_finish) */

variables::operator int () noex {
    	int		rs ;
	if ((rs = getbufsize(bufsize_nn)) >= 0) {
	    nodenamelen = rs ;
	    if ((rs = getbufsize(bufsize_hn)) >= 0) {
	        hostnamelen = rs ;
	        if ((rs = getbufsize(bufsize_un)) >= 0) {
	            usernamelen = rs ;
	            if ((rs = getbufsize(bufsize_sn)) >= 0) {
		        symnamelen = rs ;
	            }
		}
	    }
	} /* end if */
	return rs ;
} /* end method (variables::operator) */

local bool isrequired(int i) noex {
	bool		f = false ;
	switch (i) {
	case sub_start:
	case sub_curbegin:
	case sub_fetch:
	case sub_enum:
	case sub_curend:
	case sub_audit:
	case sub_finish:
	    f = true ;
	    break ;
	} /* end switch */
	return f ;
}
/* end subroutine (isrequired) */


