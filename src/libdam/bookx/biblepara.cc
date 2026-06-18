/* biblepara SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* BIBLEPARA object-load management */
/* version %I% last-modified %G% */

#define	CF_DEBUG	0		/* non-switchable debug print-outs */

/* revision history:

	- 2008-10-01, David A­D­ Morano
	This module was originally written.

*/

/* Copyright © 2008 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	biblepara

	Description:
	This module implements an interface (a trivial one) that
	allows access to the BIBLEPARA datbase.

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
#include	<biblecite.h>		/* LIBDAM */
#include	<localmisc.h>		/* LIBU */

#include	"bibleparas.h"
#include	"biblepara.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |memclear(3u)| */

/* local defines */

#define	BIBLEPARA_DEFENTS	(44 * 1000)

#define	BIBLEPARA_CA		biblepara_calls
#define	BIBLEPARA_MODBNAME	"bibleparas"
#define	BIBLEPARA_OBJNAME	"bibleparas"

#define	BPA		biblepara
#define	BPA_Q		biblepara_q
#define	BPA_C		biblepara_cur
#define	BPA_I		biblepara_info
#define	BPA_CA		biblepara_calls
#define	BPA_MAG		BIBLEPARA_MAGIC
#define	BPA_MODBNAME	BIBLEPARA_MODBNAME
#define	BPA_OBJNAME	BIBLEPARA_OBJNAME

#define	BPAS_Q		bibleparas_q
#define	BPAS_C		bibleparas_cur
#define	BPAS_I		bibleparas_info

#ifndef	CF_DEBUG
#define	CF_DEBUG	0		/* non-switchable debug print-outs */
#endif


/* imported namespaces */

using libuc::mem ;			/* variable */


/* local typedefs */

extern "C" {
    typedef int	(*soopen_f)	(void *,cchar *,cchar *) noex ;
    typedef int	(*socount_f)	(void *) noex ;
    typedef int	(*soispara_f)	(void *,con BPAS_Q *) noex ;
    typedef int	(*socurbegin_f)	(void *,BPAS_C *) noex ;
    typedef int	(*socurenum_f)	(void *,BPAS_C *,BPAS_Q *) noex ;
    typedef int	(*socurend_f)	(void *,BPAS_C *) noex ;
    typedef int	(*sogetinfo_f)	(void *,BPAS_I *) noex ;
    typedef int	(*soaudit_f)	(void *) noex ;
    typedef int	(*soclose_f)	(void *) noex ;
} /* end extern (C) */


/* external subroutines */


/* local structures */

struct biblepara_calls {
    soopen_f		open ;
    socount_f		count ;
    soispara_f		ispara ;
    socurbegin_f	curbegin ;
    socurend_f		curend ;
    socurenum_f		curenum ;
    sogetinfo_f		getinfo ;
    soaudit_f		audit ;
    soclose_f		close ;
} ; /* end struct (biblepara_calls) */

typedef biblepara_calls *	callsp ;


/* forward references */

template<typename ... Args>
local int biblepara_ctor(BPA *op,Args ... args) noex {
	BIBLEPARA	*hop = op ;
	cnullptr	np{} ;
	cnothrow	nt{} ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    memclear(hop) ;
	    rs = SR_NOMEM ;
	    if ((op->mlp = new(nt) modload) != np) ylikely {
		biblepara_calls    *callp ;
                if ((callp = new(nt) biblepara_calls) != np) ylikely {
                    op->callp = callp ;
                    rs = SR_OK ;
                } /* end if (new-biblepara_calls) */
                if (rs < 0) {
                    delete op->mlp ;
                    op->mlp = nullptr ;
                } /* end if (error) */
	    } /* end if (new-modload) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (biblepara_ctor) */

local int biblepara_dtor(BPA *op) noex {
	int		rs = SR_FAULT ;
	if (op) ylikely {
	    rs = SR_OK ;
            if (op->callp) ylikely {
                biblepara_calls    *callp = callsp(op->callp) ;
                delete callp ;
                op->callp = nullptr ;
            }
	    if (op->mlp) ylikely {
		delete op->mlp ;
		op->mlp = nullptr ;
	    }
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (biblepara_dtor) */

template<typename ... Args>
local inline int biblepara_magic(BPA *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    rs = (op->magval == BPA_MAG) ? SR_OK : SR_NOTOPEN ;
	} /* end if */
	return rs ;
} /* end subroutine (biblepara_magic) */

local int	biblepara_objloadbegin	(BPA *,cchar *,cchar *) noex ;
local int	biblepara_objloadend	(BPA *) noex ;
local int	biblepara_loadcalls	(BPA *,vecstr *) noex ;

local bool	isrequired(int) noex ;


/* external variables */


/* local variables */

enum subs {
	sub_open,
	sub_count,
	sub_ispara,
	sub_curbegin,
	sub_curend,
	sub_curenum,
	sub_getinfo,
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
	    n[sub_ispara]	= "ispara" ;
	    n[sub_curbegin]	= "curbegin" ;
	    n[sub_curend]	= "curend" ;
	    n[sub_curenum]	= "curenum" ;
	    n[sub_getinfo]	= "getinfo" ;
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

int biblepara_open(BPA *op,cchar *pr,cchar *dbn) noex {
	int		rs ;
	cchar		*objname = BIBLEPARA_OBJNAME ;
	if ((rs = biblepara_ctor(op,pr,dbn)) >= 0) ylikely {
	    rs = SR_INVALID ;
	    if (pr[0]) ylikely {
	        if ((rs = biblepara_objloadbegin(op,pr,objname)) >= 0) {
                    biblepara_calls *callp = callsp(op->callp) ;
                    rs = SR_NOSYS ;
                    if (cauto co = callp->open ; co) ylikely {
                        if ((rs = co(op->obj,pr,dbn)) >= 0) {
                            op->magval = BPA_MAG ;
                        }
                    } /* end if (non-null) */
                    if (rs < 0) {
                        biblepara_objloadend(op) ;
                    } /* end if (error) */
	        } /* end if (biblepara_objloadbegin) */
	    } /* end if (valid) */
	    if (rs < 0) {
		biblepara_dtor(op) ;
	    }
	} /* end if (biblepara_ctor) */
	return rs ;
} /* end subroutine (biblepara_open) */

int biblepara_close(BPA *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = biblepara_magic(op)) >= 0) ylikely {
            rs = SR_BUGCHECK ;
            if (biblepara_calls *callp = callsp(op->callp) ; callp) ylikely {
                rs = SR_OK ;
                if (cauto co = callp->close ; co) ylikely {
                    rs1 = co(op->obj) ;
                    if (rs >= 0) rs = rs1 ;
                } else {
                    rs = SR_NOSYS ; 
                }
	        {
	            rs1 = biblepara_objloadend(op) ;
	            if (rs >= 0) rs = rs1 ;
	        }
	        {
		    rs1 = biblepara_dtor(op) ;
	            if (rs >= 0) rs = rs1 ;
	        }
	        op->magval = 0 ;
	    } /* end if (valid) */
	} /* end if (biblepara_magic) */
	return rs ;
} /* end subroutine (biblepara_close) */

int biblepara_count(BPA *op) noex {
	int		rs ;
	if ((rs = biblepara_magic(op)) >= 0) ylikely {
            biblepara_calls   *callp = callsp(op->callp) ;
            if (cauto co = callp->count ; co) ylikely {
	        rs = co(op->obj) ;
	    }
	} /* end if (biblepara_magic) */
	return rs ;
} /* end subroutine (biblepara_count) */

int biblepara_audit(BPA *op) noex {
	int		rs ;
	if ((rs = biblepara_magic(op)) >= 0) ylikely {
            biblepara_calls   *callp = callsp(op->callp) ;
            if (cauto co = callp->audit ; co) ylikely {
	        rs = co(op->obj) ;
	    }
	} /* end if (biblepara_magic) */
	return rs ;
} /* end subroutine (biblepara_audit) */

/* get a string by its index */
int biblepara_ispara(BPA *op,con BPA_Q *qp) noex {
    	int		rs ;
	if ((rs = biblepara_magic(op,qp)) >= 0) ylikely {
            biblepara_calls   *callp = callsp(op->callp) ;
            if (cauto co = callp->ispara ; co) ylikely {
		BPAS_Q sq{} ;
		sq.b = qp->b ;
		sq.c = qp->c ;
		sq.v = qp->v ;
	        rs = co(op->obj,&sq) ;
	    } /* end of (object-call) */
	} /* end if (biblepara_magic) */
	return rs ;
} /* end subroutine (biblepara_ispara) */

int biblepara_curbegin(BPA *op,BPA_C *curp) noex {
        cnothrow        nt{} ;
        int             rs ;
        if ((rs = biblepara_magic(op,curp)) >= 0) ylikely {
            rs = SR_BUGCHECK ;
            if (biblepara_calls *callp = callsp(op->callp) ; callp) ylikely {
                rs = SR_NOSYS ;
                if (cauto co = callp->curbegin ; co) ylikely {
                    rs = SR_NOMEM ;
                    if (BPAS_C *bcurp = new(nt) BPAS_C ; bcurp) ylikely {
                        curp->scp = bcurp ;
                        if ((rs = co(op->obj,bcurp)) >= 0) ylikely {
                            curp->magval = BPA_MAG ;
                        } /* end if */  
                        if (rs < 0) {
                            delete bcurp ;
                            curp->scp = nullptr ;
                        } /* end if (error) */
                    } /* end if (non-null) */
                } /* end if (new-BPAS_C) */
            } /* end if (non-null) */
            if (rs < 0) {
                memclear(curp) ;
            } /* end if (error) */
        } /* end if (biblepara_magic) */
	return rs ;
} /* end subroutine (biblepara_curbegin) */

int biblepara_curend(BPA *op,BPA_C *curp) noex {
        int             rs ;
        int             rs1 ;
        if ((rs = biblepara_magic(op,curp)) >= 0) ylikely {
            biblepara_calls   *callp = callsp(op->callp) ;
            rs = SR_NOTOPEN ;
            if (BPAS_C *bcurp = resumelife<BPAS_C>(curp->scp) ; bcurp) ylikely {
                rs = SR_BADFMT ;
                if (curp->magval == BPA_MAG) ylikely {
                    rs = SR_OK ;
                    if (cauto co = callp->curend ; co) ylikely {
                        rs1 = co(op->obj,bcurp) ;
                        if (rs >= 0) rs = rs1 ;
                    } else {
                        rs = SR_NOSYS ;
                    }
                    {
                        delete bcurp ;
                        curp->scp = nullptr ;
                    }
                    curp->magval = 0 ;
                } /* end if (good cursor magic) */
            } /* end if (non-null) */
        } /* end if (biblepara_magic) */
        return rs ;
} /* end subroutine (biblepara_curend) */

int biblepara_curenum(BPA *op,BPA_C *curp,BPA_Q *qp) noex {
        int             rs ;
        if ((rs = biblepara_magic(op,curp,qp)) >= 0) ylikely {
            biblepara_calls   *callp = callsp(op->callp) ;
            rs = SR_NOTOPEN ;
            if (BPAS_C *bcurp = resumelife<BPAS_C>(curp->scp) ; bcurp) ylikely {
                rs = SR_BADFMT ;
                if (curp->magval == BPA_MAG) ylikely {
                    rs = SR_NOSYS ;
                    if (cauto co = callp->curenum ; co) ylikely {
                        BPAS_Q   sq{} ;
                        if ((rs = co(op->obj,bcurp,&sq)) >= 0) {
                            qp->b = sq.b ;
                            qp->c = sq.c ;
                            qp->v = sq.v ;
                        } /* end if (object-call) */
                    } /* end if (non-null) */
                } /* end if (good cursor magic) */
            } /* end if (non-null) */
        } /* end if (biblepara_magic) */
        return rs ;
} /* end subroutine (biblepara_curenum) */

int biblepara_getinfo(BPA *op,BPA_I *ip) noex {
	int		rs ;
	int		nv = 0 ; /* return-value */
	if ((rs = biblepara_magic(op)) >= 0) ylikely {
            biblepara_calls   *callp = callsp(op->callp) ;
            rs = SR_NOSYS ;
            if (cauto co = callp->getinfo ; co) ylikely {
                if (BPAS_I bi{} ; (rs = co(op->obj,&bi)) >= 0) ylikely {
                    nv = bi.nverses ;  
		    if (ip) {
	    	        memclear(ip) ;
	    	        ip->dbtime	= bi.dbtime ;
	    	        ip->vitime	= bi.vitime ;
	    	        ip->maxbook	= bi.maxbook ;
	    	        ip->maxchap	= bi.maxchap ;
	    	        ip->nverses	= bi.nverses ;
	    	        ip->nzverses	= bi.nzverses ;
		    } /* end if */
		} /* end if (object-call) */
	    } /* end if (non-null) */
	    if (ip && (rs < 0)) {
		memclear(ip) ;
	    } /* end if (error) */
	} /* end if (biblepara_magic) */
	return (rs >= 0) ? nv : rs ;
} /* end subroutine (biblepara_getinfo) */


/* private subroutines */

local int biblepara_objloadbegin(BPA *op,cchar *pr,cchar *objn) noex {
	modload		*mlp = op->mlp ;
	cint		vn = sub_overlast ;
	cint		vo = vecstrm.compact ;
	int		rs ;
	int		rs1 ;
	if (vecstr syms ; (rs = syms.start(vn,vo)) >= 0) ylikely {
	    if ((rs = syms.addsyms(objn,subname.n)) >= 0) ylikely {
	        if (mainv sv ; (rs = syms.getvec(&sv)) >= 0) ylikely {
	            cchar	*mn = BPA_MODBNAME ;
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
	                        rs = biblepara_loadcalls(op,&syms) ;
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
} /* end subroutine (biblepara_objloadbegin) */

local int biblepara_objloadend(BPA *op) noex {
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
} /* end subroutine (biblepara_objloadend) */

local int biblepara_loadcalls(BPA *op,vecstr *slp) noex {
	modload		*mlp = op->mlp ;
	cint		rsn = SR_NOTFOUND ;
	int		rs = SR_OK ;
	int		rs1 ;
	int		c = 0 ;
	cchar		*sname{} ;
	for (int i = 0 ; (rs1 = slp->get(i,&sname)) >= 0 ; i += 1) ylikely {
	    if (cvoid *snp{} ; (rs = modload_getsym(mlp,sname,&snp)) >= 0) {
                biblepara_calls   *callp = callsp(op->callp) ;
                c += 1 ;
                switch (i) {
                case sub_open:
                    callp->open		= soopen_f(snp) ;
                    break ;
                case sub_count:
                    callp->count	= socount_f(snp) ;
                    break ;
		case sub_ispara:
                    callp->ispara	= soispara_f(snp) ;
		    break ;
                case sub_curbegin:
                    callp->curbegin	= socurbegin_f(snp) ;
                    break ;
                case sub_curend:
                    callp->curend	= socurend_f(snp) ;
                    break ;
                case sub_curenum:
                    callp->curenum	= socurenum_f(snp) ;
                    break ;
                case sub_audit:
                    callp->audit	= soaudit_f(snp) ;
                    break ;
		case sub_getinfo:
		    callp->getinfo	= sogetinfo_f(snp) ;
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
} /* end subroutine (biblepara_loadcalls) */

local bool isrequired(int i) noex {
	bool	f = false ;
	switch (i) {
	case sub_open:
	case sub_ispara:
	case sub_close:
	    f = true ;
	    break ;
	} /* end switch */
	return f ;
} /* end subroutine (isrequired) */


