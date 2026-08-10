/* bvs SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* Bible Verse Structure */
/* load management and interface for the BVSES object */
/* version %I% last-modified %G% */

#define	CF_DEBUG	0		/* non-switchable debug print-outs */

/* revision history:

	- 2008-10-01, David A­D­ Morano
	This module was originally written.

*/

/* Copyright © 2008 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	bvs

	Description:
	This module implements an interface (a trivial one) that
	provides access to the BVSES object (which is dynamically
	loaded).

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* POSIX */
#include	<sys/param.h>		/* POSIX */
#include	<unistd.h>		/* POSIX */
#include	<fcntl.h>		/* POSIX */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstring>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<ucmem.h>		/* LIBUC */
#include	<vecstr.h>		/* LIBUC */
#include	<sncpyx.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */

#include	"bvses.h"
#include	"bvs.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |memclear(3u)| */

/* local defines */

#define	BVS_MODBNAME	"bvses"
#define	BVS_OBJNAME	"bvses"

#ifndef	SYMNAMELEN
#define	SYMNAMELEN	60
#endif

#ifndef	CF_DEBUG
#define	CF_DEBUG	0		/* non-switchable debug print-outs */
#endif


/* imported namespaces */

using libuc::mem ;			/* variable */


/* local typedefs */

extern "C" {
    typedef int	(*soopen_f)	(void *,cchar *,cchar *) noex ;
    typedef int	(*socount_f)	(void *) noex ;
    typedef int	(*sogetinfo_f)	(void *,bvses_info *) noex ;
    typedef int	(*somkmodq_f)	(void *,bvses_v *,int) noex ;
    typedef int	(*soaudit_f)	(void *) noex ;
    typedef int	(*soclose_f)	(void *) noex ;
} /* end extern (C) */


/* external subroutines */


/* external variables */


/* local structures */

struct bvs_calls {
    soopen_f		open ;
    socount_f		count ;
    sogetinfo_f		getinfo ;
    somkmodq_f		mkmodq ;
    soaudit_f		audit ;
    soclose_f		close ;
} ; /* end struct (bvs) */

typedef bvs_calls *	callsp ;


/* forward references */

template<typename ... Args>
local int bvs_ctor(bvs *op,Args ... args) noex {
	BVS		*hop = op ;
	cnullptr	np{} ;
	cnothrow	nt{} ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    memclear(hop) ;
	    rs = SR_NOMEM ;
	    if ((op->mlp = new(nt) modload) != np) ylikely {
		bvs_calls    *callp ;
                if ((callp = new(nt) bvs_calls) != np) ylikely {
                    op->callp = callp ;
                    rs = SR_OK ;
                } /* end if (new-bvs_calls) */
                if (rs < 0) {
                    delete op->mlp ;
                    op->mlp = nullptr ;
                } /* end if (error) */
	    } /* end if (new-modload) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (bvs_ctor) */

local int bvs_dtor(bvs *op) noex {
	int		rs = SR_FAULT ;
	if (op) ylikely {
	    rs = SR_OK ;
            if (op->callp) ylikely {
                bvs_calls    *callp = callsp(op->callp) ;
                delete callp ;
                op->callp = nullptr ;
            }
	    if (op->mlp) ylikely {
		delete op->mlp ;
		op->mlp = nullptr ;
	    }
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (bvs_dtor) */

template<typename ... Args>
local inline int bvs_magic(bvs *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    rs = (op->magval == BVS_MAGIC) ? SR_OK : SR_NOTOPEN ;
	} /* end if */
	return rs ;
} /* end subroutine (bvs_magic) */

local int	bvs_objloadbegin	(bvs *,cchar *,cchar *) noex ;
local int	bvs_objloadend		(bvs *) noex ;
local int	bvs_loadcalls		(bvs *,vecstr *) noex ;

local bool	isrequired(int) noex ;


/* local variables */

enum subs {
	sub_open,
	sub_count,
	sub_getinfo,
	sub_mkmodq,
	sub_audit,
	sub_close,
	sub_overlast
} ; /* end enum */

namespace {
    struct subnamer {
	cchar		*n[sub_overlast + 1] ;
        consteval subnamer() noex {
	    n[sub_open]		= "open" ;
	    n[sub_count]	= "count" ;
	    n[sub_getinfo]	= "getinfo" ;
	    n[sub_mkmodq]	= "mkmodq" ;
	    n[sub_audit]	= "audit" ;
	    n[sub_close]	= "close" ;
	    n[sub_overlast]	= nullptr ;
	} ; /* end ctor */
    } ; /* end struct (subnamer) */
} /* end namespace */

constexpr subnamer	subname ;
cbool			f_debug		= CF_DEBUG ;


/* exported variables */


/* exported subroutines */

int bvs_open(bvs *op,cchar *pr,cchar *dbn) noex {
	int		rs ;
	if ((rs = bvs_ctor(op,pr,dbn)) >= 0) {
	    rs = SR_INVALID ;
	    if (pr[0] && dbn[0]) {
	        cchar	*objn = BVS_OBJNAME ;
                if ((rs = bvs_objloadbegin(op,pr,objn)) >= 0) {
                    bvs_calls   *callp = callsp(op->callp) ;
                    rs = SR_NOSYS ;
                    if (callp->open) {
                        cauto   co = callp->open ;
                        if ((rs = co(op->obj,pr,dbn)) >= 0) {
                            op->magval = BVS_MAGIC ;
                        }
                    } /* end if (open) */
	            if (rs < 0) {
		        bvs_objloadend(op) ;
	            } /* end if (error) */
	        } /* end if (bvs-objloadbegin) */
	    } /* end if (valid) */
	    if (rs < 0) {
		bvs_dtor(op) ;
	    } /* end if (error) */
	} /* end if (bvs_ctor) */
	return rs ;
} /* end subroutine (bvs_open) */

/* free up the entire vector string data structure object */
int bvs_close(bvs *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = bvs_magic(op)) >= 0) ylikely {
	    rs = SR_BUGCHECK ;
	    if (bvs_calls *callp = callsp(op->callp) ; callp) {
		rs = SR_OK ;
	        if (callp->close) {
		    cauto 	co = callp->close ;
	            rs1 = co(op->obj) ;
	            if (rs >= 0) rs = rs1 ;
	        } else {
		    rs = SR_NOSYS ;
	        }
	        {
	            rs1 = bvs_objloadend(op) ;
	            if (rs >= 0) rs = rs1 ;
	        }
	        {
	            rs1 = bvs_dtor(op) ;
	            if (rs >= 0) rs = rs1 ;
	        }
	        op->magval = 0 ;
	    } /* end if (valid) */
	} /* end if (bvs_magic) */
	return rs ;
} /* end subroutine (bvs_close) */

int bvs_count(bvs *op) noex {
	int		rs ;
	if ((rs = bvs_magic(op)) >= 0) ylikely {
	    bvs_calls	*callp = callsp(op->callp) ;
	    rs = SR_NOSYS ;
	    if (callp->count) {
		cauto 	co = callp->count ;
    		rs = co(op->obj) ;
	    }
	} /* end if (bvs_magic) */
	return rs ;
} /* end subroutine (bvs_count) */

int bvs_getinfo(bvs *op,bvs_info *ip) noex {
	int		rs ;
	int		n = 0 ; /* return-value */
	if ((rs = bvs_magic(op)) >= 0) ylikely {
	    bvses_info	bvsi{} ;
	    bvs_calls	*callp = callsp(op->callp) ;
	    rs = SR_NOSYS ;
	    if (callp->getinfo) {
		cauto 	co = callp->getinfo ;
    		rs = co(op->obj,&bvsi) ;
	    }
	    if (ip) {
	        memclear(ip) ;
	        if (rs >= 0) {
		    ip->ctime		= bvsi.ctime ;
		    ip->mtime		= bvsi.mtime ;
		    ip->nzbooks		= bvsi.nzbooks ;
		    ip->nbooks		= bvsi.nbooks ;
		    ip->nchapters	= bvsi.nchapters ;
		    ip->nverses		= bvsi.nverses ;
		    ip->nzverses	= bvsi.nzverses ;
	        } /* end if */
	    } /* end if (non-null) */
	} /* end if (bvs_magic) */
	return (rs >= 0) ? n : rs ;
} /* end subroutine (bvs_getinfo) */

int bvs_mkmodq(bvs *op,bvs_v *bvep,int mjd) noex {
	int		rs ;
	int		n = 0 ; /* return-value */
	if ((rs = bvs_magic(op)) >= 0) ylikely {
	    bvses_v	bv{} ;
	    bvs_calls	*callp = callsp(op->callp) ;
	    rs = SR_NOSYS ;
	    if (callp->mkmodq) {
		cauto 	co = callp->mkmodq ;
    		rs = co(op->obj,&bv,mjd) ;
	    }
	    if (bvep) {
	        memclear(bvep) ;
	        if (rs >= 0) {
		    bvep->b = bv.b ;
		    bvep->c = bv.c ;
		    bvep->v = bv.v ;
	        } /* end if (ok) */
	    } /* end if (non-null) */
	} /* end if (bvs_magic) */
	return (rs >= 0) ? n : rs ;
} /* end subroutine (bvs_mkmodq) */

int bvs_audit(bvs *op) noex {
	int		rs ;
	if ((rs = bvs_magic(op)) >= 0) ylikely {
	    bvs_calls	*callp = callsp(op->callp) ;
	    rs = SR_NOSYS ;
	    if (callp->audit) {
		cauto 	co = callp->audit ;
	        rs = co(op->obj) ;
	    }
	} /* end if (bvs_magic) */
	return rs ;
} /* end subroutine (bvs_audit) */


/* private subroutines */

local int bvs_objloadbegin(bvs *op,cchar *pr,cchar *objn) noex {
	modload		*mlp = op->mlp ;
	cint		vn = sub_overlast ;
	cint		vo = vecstrm.compact ;
	int		rs ;
	int		rs1 ;
	if (vecstr syms ; (rs = syms.start(vn,vo)) >= 0) ylikely {
	    if ((rs = syms.addsyms(objn,subname.n)) >= 0) ylikely {
	        if (mainv sv ; (rs = syms.getvec(&sv)) >= 0) ylikely {
	            cchar	*mn = BVS_MODBNAME ;
	            cchar	*on = objn ;
	            int		mo = 0 ;
	            mo |= modloadm.libvar ;
	            mo |= modloadm.libprs ;
	            mo |= modloadm.libsdirs ;
	            mo |= modloadm.avail ;
	            if ((rs = modload_open(mlp,pr,mn,on,mo,sv)) >= 0) ylikely {
		        op->fl.modload = true ;
	                if (int mv[2] ; (rs = modload_getmva(mlp,mv,2)) >= 0) {
			    cint	osz = mv[0] ;
	                    op->objsz = mv[0] ;
	                    op->cursz = mv[1] ;
			    if (void *vp ; (rs = mem.mall(osz,&vp)) >= 0) {
	                        op->obj = vp ;
	                        rs = bvs_loadcalls(op,&syms) ;
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
} /* end subroutine (bvs_objloadbegin) */

local int bvs_objloadend(bvs *op) noex {
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
	} /* end if */
	return rs ;
} /* end subroutine (bvs_objloadend) */

local int bvs_loadcalls(bvs *op,vecstr *slp) noex {
	modload		*mlp = op->mlp ;
	cint		rsn = SR_NOTFOUND ;
	int		rs = SR_OK ;
	int		rs1 ;
	int		c = 0 ;
	cchar		*sname{} ;
	for (int i = 0 ; (rs1 = slp->get(i,&sname)) >= 0 ; i += 1) {
	    if (cvoid *snp{} ; (rs = modload_getsym(mlp,sname,&snp)) >= 0) {
                bvs_calls   *callp = callsp(op->callp) ;
                c += 1 ;
                switch (i) {
                case sub_open:
                    callp->open		= soopen_f(snp) ;
                    break ;
                case sub_count:
                    callp->count	= socount_f(snp) ;
                    break ;
                case sub_getinfo:
                    callp->getinfo	= sogetinfo_f(snp) ;
                    break ;
		case sub_mkmodq:
                    callp->mkmodq	= somkmodq_f(snp) ;
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
} /* end subroutine (bvs_loadcalls) */

local bool isrequired(int i) noex {
	bool		f = false ;
	switch (i) {
	case sub_open:
	case sub_count:
	case sub_getinfo:
	case sub_mkmodq:
	case sub_audit:
	case sub_close:
	    f = true ;
	    break ;
	} /* end switch */
	return f ;
} /* end subroutine (isrequired) */


