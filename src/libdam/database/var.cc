/* var SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* VAR management */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-12-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services
	(RNS).

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	var

	Description:
	This module implements an interface (a trivial one) that
	provides access to the VAR object (which is dynamically
	loaded).

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/param.h>		/* POSIX® */
#include	<sys/stat.h>		/* POSIX® */
#include	<dlfcn.h>		/* POSIX® */
#include	<unistd.h>		/* POSIX® */
#include	<fcntl.h>		/* POSIX® */
#include	<ctime>			/* CSTD */
#include	<climits>		/* CSTD */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstring>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<endian.h>		/* LIBU */
#include	<nulstr.h>		/* LIBU */
#include	<ucmem.h>		/* LIBUC */
#include	<getnodename.h>		/* LIBUC */
#include	<vecstr.h>		/* LIBUC */
#include	<sncpyx.h>		/* LIBUC */
#include	<mkpr.h>		/* LIBUC */
#include	<mkpathx.h>		/* LIBUC */
#include	<mkfname.h>		/* LIBUC */
#include	<isnot.h>		/* LIBUC */
#include	<vardefs.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

#include	"var.h"
#include	"vars.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |memclear(3u)| */

/* local defines */

#define	VAR_MODBNAME	"vars"
#define	VAR_OBJNAME	"vars"

#define	LIBCNAME	"lib"
#define	SONAME		"vars"
#define	INDSUF		"vi"

#ifndef	VARLIBPATH
#define	VARLIBPATH	"LD_LIBRARY_PATH"
#endif

#ifndef	SYMNAMELEN
#define	SYMNAMELEN	60
#endif


/* imported namespaces */

using libuc::mem ;			/* variable */


/* local typedefs */

extern "C" {
    typedef int	(*soopen_f)	(void *,cchar *) noex ;
    typedef int	(*socount_f)	(void *) noex ;
    typedef int	(*socurbegin_f)	(void *,void *) noex ;
    typedef int	(*socurenum_f)	(void *,void *,char *,int,char *,int) noex ;
    typedef int	(*socurend_f)	(void *,void *) noex ;
    typedef int	(*sofetch_f)	(void *,cchar *,int,void *,char *,int) noex ;
    typedef int	(*sogetinfo_f)	(void *,VARS_INFO *) noex ;
    typedef int	(*soaudit_f)	(void *) noex ;
    typedef int	(*soclose_f)	(void *) noex ;
} /* end extern (C) */


/* external subroutines */


/* external variables */


/* local structures */

struct calls {
    soopen_f		open ;
    socount_f		count ;
    socurbegin_f	curbegin ;
    socurend_f		curend ;
    socurenum_f		curenum ;
    sofetch_f		fetch ;
    sogetinfo_f		getinfo ;
    soaudit_f		audit ;
    soclose_f		close ;
} ; /* end struct (var) */

typedef calls *	callsp ;


/* forward references */

template<typename ... Args>
local int var_ctor(var *op,Args ... args) noex {
	VAR		*hop = op ;
	cnullptr	np{} ;
	cnothrow	nt{} ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    memclear(hop) ;
	    rs = SR_NOMEM ;
	    if ((op->mlp = new(nt) modload) != np) ylikely {
		calls    *callp ;
                if ((callp = new(nt) calls) != np) ylikely {
                    op->callp = callp ;
                    rs = SR_OK ;
                } /* end if (new-calls) */
                if (rs < 0) {
                    delete op->mlp ;
                    op->mlp = nullptr ;
                } /* end if (error) */
	    } /* end if (new-modload) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (var_ctor) */

local int var_dtor(var *op) noex {
	int		rs = SR_FAULT ;
	if (op) ylikely {
	    rs = SR_OK ;
            if (op->callp) ylikely {
                calls    *callp = callsp(op->callp) ;
                delete callp ;
                op->callp = nullptr ;
            }
	    if (op->mlp) ylikely {
		delete op->mlp ;
		op->mlp = nullptr ;
	    }
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (var_dtor) */

template<typename ... Args>
local inline int var_magic(var *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    rs = (op->magval == VAR_MAGIC) ? SR_OK : SR_NOTOPEN ;
	} /* end if */
	return rs ;
} /* end subroutine (var_magic) */

local int	var_objloadbegin	(var *,cchar *,cchar *) noex ;
local int	var_objloadend		(var *) noex ;
local int	var_loadcalls		(var *,vecstr *) noex ;

local bool	isrequired(int) noex ;


/* external variables */


/* local variables */

enum subs {
	sub_open,
	sub_count,
	sub_curbegin,
	sub_curenum,
	sub_curend,
	sub_fetch,
	sub_getinfo,
	sub_audit,
	sub_close,
	sub_overlast
} ; /* end enum (subs) */

constexpr cpcchar	subnames[] = {
	"open",
	"count",
	"curbegin",
	"curenum",
	"curend",
	"fetch",
	"info",
	"audit",
	"close",
	nullptr
} ; /* end array (subs) */


/* exported variables */


/* exported subroutines */

int var_open(var *op,cchar *dbname) noex {
    	cnullptr	np{} ;
	int		rs ;
	cchar		*objname = VAR_OBJNAME ;
	if ((rs = var_ctor(op,dbname)) >= 0) ylikely {
	    rs = SR_INVALID ;
	    if (dbname[0]) {
	        if ((rs = var_objloadbegin(op,np,objname)) >= 0) {
                    calls *callp = resumelife<calls>(op->callp) ;
                    if ((rs = callp->open(op->obj,dbname)) >= 0) {
                        op->magval = VAR_MAGIC ;
                    }
	            if (rs < 0) {
		        var_objloadend(op) ;
	            }
	        } /* end if (var_objloadbegin) */
	    } /* end if (valid) */
	    if (rs < 0) {
		var_dtor(op) ;
	    } /* end if (error) */
	} /* end if (var_ctor) */
	return rs ;
} /* end subroutine (var_open) */

int var_close(var *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = var_magic(op)) >= 0) ylikely {
	    {
                calls *callp = resumelife<calls>(op->callp) ;
	        rs1 = callp->close(op->obj) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = var_objloadend(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
		rs1 = var_dtor(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    op->magval = 0 ;
	} /* end if (var_magic) */
	return rs ;
} /* end subroutine (var_close) */

int var_getinfo(var *op,VAR_INFO *vip) noex {
	int		rs ;
	if ((rs = var_magic(op,vip)) >= 0) ylikely {
	    memclear(vip) ;
            if (calls *callp = resumelife<calls>(op->callp) ; callp) {
	        VARS_INFO	vsi{} ;
	        if ((rs = callp->getinfo(op->obj,&vsi)) >= 0) {
		    vip->wtime = vsi.wtime ;
		    vip->mtime = vsi.mtime ;
		    vip->nvars = vsi.nvars ;
		    vip->nskip = vsi.nskip ;
	        }
	    } /* end if */
	} /* end if (var_magic) */
	return rs ;
} /* end subroutine (var_getinfo) */

int var_count(var *op) noex {
	int		rs ;
	if ((rs = var_magic(op)) >= 0) ylikely {
            if (calls *callp = resumelife<calls>(op->callp) ; callp) {
	        rs = callp->count(op->obj) ;
	    }
	} /* end if (var_magic) */
	return rs ;
} /* end subroutine (var_count) */

int var_curbegin(var *op,var_cur *curp) noex {
	int		rs ;
	if ((rs = var_magic(op,curp)) >= 0) ylikely {
	    memclear(curp) ;
	    rs = SR_BUGCHECK ;
            if (calls *callp = resumelife<calls>(op->callp) ; callp) {
		rs = SR_NOSYS ;
	        if (callp->curbegin) {
	            if (void *vp ; (rs = mem.mall(op->cursz,&vp)) >= 0) {
		        curp->scp = vp ;
	                if ((rs = callp->curbegin(op->obj,curp->scp)) >= 0) {
	                    curp->magval = VAR_MAGIC ;
			} /* end if (ok) */
	        	if (rs < 0) {
	            	    mem.free(curp->scp) ;
	            	    curp->scp = nullptr ;
	        	} /* end if (error) */
	    	    } /* end if (memory-acquire) */
		} /* end if (non-null) */
	    } /* end if (bug-check) */
	} /* end if (var_magic) */
	return rs ;
} /* end subroutine (var_curbegin) */

int var_curend(var *op,var_cur *curp) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = var_magic(op,curp)) >= 0) ylikely {
	    rs = SR_NOTOPEN ;
	    if ((curp->magval == VAR_MAGIC) && curp->scp) ylikely {
	        rs = SR_BUGCHECK ;
                if (calls *co = resumelife<calls>(op->callp) ; co) {
		    rs = SR_OK ;
		    if (co->curend) {
	        	rs1 = co->curend(op->obj,curp->scp) ;
	        	if (rs >= 0) rs = rs1 ;
		    } else {
			rs = SR_NOSYS ;
		    }
	            {
	                rs1 = mem.free(curp->scp) ;
	                if (rs >= 0) rs = rs1 ;
	                curp->scp = nullptr ;
		    } /* end if (memory-release) */
	        } /* end if (non-null) */
		curp->magval = 0 ;
	    } /* end if (cursor-magic) */
	} /* end if (var_magic) */
	return rs ;
} /* end subroutine (var_curend) */

int var_curenum(var *op,var_cur *curp,char *kbuf,int klen,
		char *vbuf,int vlen) noex {
	int		rs ;
	if ((rs = var_magic(op,curp,kbuf)) >= 0) ylikely {
	    rs = SR_NOTOPEN ;
	    if ((curp->magval == VAR_MAGIC) && curp->scp) ylikely {
	        rs = SR_BUGCHECK ;
                if (calls *co = resumelife<calls>(op->callp) ; co) {
			rs = SR_NOSYS ;
		        if (co->curenum) {
	    		    rs = co->curenum(op->obj,curp->scp,
				kbuf,klen,vbuf,vlen) ;
			} /* end if (ok) */
	        } /* end if (bug-check) */
	    } /* end if (cursor-magic) */
	} /* end if (var_magic) */
	return rs ;
} /* end subroutine (var_curenum) */

int var_fetch(var *op,cc *kp,int kl,var_cur *curp,char *vbuf,int vlen) noex {
	int		rs ;
	if ((rs = var_magic(op,curp,kp)) >= 0) ylikely {
	    rs = SR_NOTOPEN ;
	    if ((curp->magval == VAR_MAGIC) && curp->scp) ylikely {
	        rs = SR_BUGCHECK ;
                if (calls *co = resumelife<calls>(op->callp) ; co) {
		    rs = SR_NOSYS ;
		    if (co->fetch) {
	    		rs = co->fetch(op->obj,kp,kl,curp->scp,vbuf,vlen) ;
		    } /* end if (ok) */
	        } /* end if (bug-check) */
	    } /* end if (cursor-magic) */
	} /* end if (var_magic) */
	return rs ;
} /* end subroutine (var_fetch) */

int var_audit(var *op) noex {
	int		rs ;
	if ((rs = var_magic(op)) >= 0) ylikely {
	    rs = SR_NOSYS ;
            if (calls *co = resumelife<calls>(op->callp) ; co) {
	        rs = co->audit(op->obj) ;
	    }
	} /* end if (var_magic) */
	return rs ;
} /* end subroutine (var_audit) */


/* private subroutines */

local int var_objloadbegin(var *op,cchar *pr,cchar *objn) noex {
	modload		*mlp = op->mlp ;
	cint		vn = sub_overlast ;
	cint		vo = vecstrm.compact ;
	int		rs ;
	int		rs1 ;
	if (vecstr syms ; (rs = syms.start(vn,vo)) >= 0) {
	    if ((rs = syms.addsyms(objn,subnames)) >= 0) {
	        if (mainv sv ; (rs = syms.getvec(&sv)) >= 0) {
	            cchar	*mn = VAR_MODBNAME ;
	            cchar	*on = objn ;
	            int		mo = 0 ;
	            mo |= modloadm.libvar ;
	            mo |= modloadm.libprs ;
	            mo |= modloadm.libsdirs ;
	            mo |= modloadm.avail ;
	            if ((rs = modload_open(mlp,pr,mn,on,mo,sv)) >= 0) {
		        op->fl.modload = true ;
	                if (int mv[2] ; (rs = modload_getmva(mlp,mv,2)) >= 0) {
			    cint	osz = mv[0] ;
	                    op->objsz = mv[0] ;
	                    op->cursz = mv[1] ;
			    if (void *vp ; (rs = mem.mall(osz,&vp)) >= 0) {
	                        op->obj = vp ;
	                        rs = var_loadcalls(op,&syms) ;
	                        if (rs < 0) {
	                            mem.free(op->obj) ;
	                            op->obj = nullptr ;
	                        } /* end if (error) */
	                    } /* end if (memory-allocation) */
	                } /* end if (modload_getmva) */
	                if (rs < 0) {
		            op->fl.modload = false ;
	                    modload_close(mlp) ;
	                } /* end if (error) */
	            } /* end if (modload_open) */
		} /* end if (vecstr_getvec) */
	    } /* end if (vecstr_addsyms) */
	    rs1 = syms.finish ;
	    if (rs >= 0) rs = rs1 ;
	    if ((rs < 0) && op->fl.modload) {
		op->fl.modload = false ;
		modload_close(mlp) ;
	    } /* end if (error) */
	} /* end if (vecstr-syms) */
	return rs ;
} /* end subroutine (var_objloadbegin) */

local int var_objloadend(var *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (op->obj) {
	    rs1 = mem.free(op->obj) ;
	    if (rs >= 0) rs = rs1 ;
	    op->obj = nullptr ;
	} /* end if (memory-release) */
	if (op->mlp && op->fl.modload) {
	    op->fl.modload = false ;
	    rs1 = modload_close(op->mlp) ;
	    if (rs >= 0) rs = rs1 ;
	}
	return rs ;
} /* end subroutine (var_objloadend) */

local int var_loadcalls(var *op,vecstr *slp) noex {
	modload		*mlp = op->mlp ;
	cint		rsn = SR_NOTFOUND ;
	int		rs = SR_OK ;
	int		rs1 ;
	int		c = 0 ;
	cchar		*sname{} ;
	for (int i = 0 ; (rs1 = slp->get(i,&sname)) >= 0 ; i += 1) {
	    if (cvoid *snp{} ; (rs = modload_getsym(mlp,sname,&snp)) >= 0) {
                calls   *callp = callsp(op->callp) ;
                c += 1 ;
                switch (i) {
                case sub_open:
                    callp->open		= soopen_f(snp) ;
                    break ;
		case sub_count:
		    callp->count	= socount_f(snp) ;
		    break ;
		case sub_curbegin:
		    callp->curbegin	= socurbegin_f(snp) ;
		    break ;
		case sub_curenum:
		    callp->curenum	= socurenum_f(snp) ;
		    break ;
		case sub_curend:
		    callp->curend	= socurend_f(snp) ;
		    break ;
		case sub_fetch:
		    callp->fetch	= sofetch_f(snp) ;
		    break ;
		case sub_getinfo:
		    callp->getinfo	= sogetinfo_f(snp) ;
		    break ;
		case sub_audit:
		    callp->audit	= soaudit_f(snp) ;
		    break ;
		case sub_close:
		    callp->close	= soclose_f(snp) ;
		    break ;
		} /* end switch */
            } else if (rs == rsn) {
                if (! isrequired(i)) rs = SR_OK ;
            } /* end if (it had the call) */
	    if (rs < 0) break ;
	} /* end for (vecstr_get) */
	if ((rs >= 0) && (rs1 != rsn)) rs = rs1 ;
	return (rs >= 0) ? c : rs ;
} /* end subroutine (var_loadcalls) */

local bool isrequired(int i) noex {
	bool		f = false ;
	switch (i) {
	case sub_open:
	case sub_curbegin:
	case sub_curenum:
	case sub_curend:
	case sub_close:
	    f = true ;
	    break ;
	case sub_fetch:
	    break ;
	} /* end switch */
	return f ;
} /* end subroutine (isrequired) */


