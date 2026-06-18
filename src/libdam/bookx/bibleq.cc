/* bibleq SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* BIBLEQ object-load management */
/* version %I% last-modified %G% */

#define	CF_DEBUG	0		/* non-switchable debug print-outs */

/* revision history:

	- 2008-10-01, David A­D­ Morano
	This module was originally written.

*/

/* Copyright © 2008 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	bibleq

	Description:
	This module implements an interface (a trivial one) that
	provides access to the BIBLEQ object (which is dynamically
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
#include	<localmisc.h>		/* LIBU */
#include	<libdebug.h>		/* LIBDEBUG |DEBUGPRINTF(3debug)| */

#include	"bibleqs.h"
#include	"bibleq.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |memclear(3u)| */

/* local defines */

#define	BIBLEQ_MODBNAME	"bibleqs"
#define	BIBLEQ_OBJNAME	"bibleqs"

#define	BQ		bibleq
#define	BQ_Q		bibleq_q
#define	BQ_C		bibleq_cur
#define	BQ_MAG		BIBLEQ_MAGIC
#define	BQ_MODBNAME	BIBLEQ_MODBNAME
#define	BQ_OBJNAME	BIBLEQ_OBJNAME

#define	BQS		bibleqs
#define	BQS_C		bibleqs_cur
#define	BQS_Q		bibleqs_q

#ifndef	CF_DEBUG
#define	CF_DEBUG	0		/* non-switchable debug print-outs */
#endif


/* imported namespaces */

using libuc::mem ;			/* variable */


/* local typedefs */

extern "C" {
    typedef int	(*soopen_f)	(void *,cchar *,cchar *) noex ;
    typedef int	(*socount_f)	(void *) noex ;
    typedef int	(*socurbegin_f)	(void *,void *) noex ;
    typedef int	(*socurend_f)	(void *,void *) noex ;
    typedef int	(*socurenum_f)	(void *,void *,BQS_Q *,char *,int) noex ;
    typedef int	(*socurlook_f)	(void *,void *,int,cchar **) noex ;
    typedef int	(*soaudit_f)	(void *) noex ;
    typedef int	(*soclose_f)	(void *) noex ;
} /* end extern (C) */


/* external subroutines */


/* local structures */

struct bibleq_calls {
    soopen_f		open ;
    socount_f		count ;
    socurbegin_f	curbegin ;
    socurend_f		curend ;
    socurenum_f		curenum ;
    socurlook_f		curlook ;
    soaudit_f		audit ;
    soclose_f		close ;
} ; /* end struct (bibleq_calls) */

typedef bibleq_calls *	callsp ;


/* forward references */

template<typename ... Args>
local int bibleq_ctor(BQ *op,Args ... args) noex {
	BIBLEQ		*hop = op ;
	cnullptr	np{} ;
	cnothrow	nt{} ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    memclear(hop) ;
	    rs = SR_NOMEM ;
	    if ((op->mlp = new(nt) modload) != np) ylikely {
		bibleq_calls    *callp ;
                if ((callp = new(nt) bibleq_calls) != np) ylikely {
                    op->callp = callp ;
                    rs = SR_OK ;
                } /* end if (new-bibleq_calls) */
                if (rs < 0) {
                    delete op->mlp ;
                    op->mlp = nullptr ;
                } /* end if (error) */
	    } /* end if (new-modload) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (bibleq_ctor) */

local int bibleq_dtor(BQ *op) noex {
	int		rs = SR_FAULT ;
	if (op) ylikely {
	    rs = SR_OK ;
            if (op->callp) ylikely {
                bibleq_calls    *callp = callsp(op->callp) ;
                delete callp ;
                op->callp = nullptr ;
            }
	    if (op->mlp) ylikely {
		delete op->mlp ;
		op->mlp = nullptr ;
	    }
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (bibleq_dtor) */

template<typename ... Args>
local inline int bibleq_magic(BQ *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    rs = (op->magval == BQ_MAG) ? SR_OK : SR_NOTOPEN ;
	} /* end if */
	return rs ;
} /* end subroutine (bibleq_magic) */

local int	bibleq_objloadbegin	(BQ *,cchar *,cchar *) noex ;
local int	bibleq_objloadend	(BQ *) noex ;
local int	bibleq_loadcalls	(BQ *,vecstr *) noex ;

local bool	isrequired(int) noex ;


/* external variables */


/* local variables */

enum subs {
	sub_open,
	sub_count,
	sub_curbegin,
	sub_curend,
	sub_curlook,
	sub_curenum,
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
	    n[sub_curbegin]	= "curbegin" ;
	    n[sub_curend]	= "curend" ;
	    n[sub_curlook]	= "curlook" ;
	    n[sub_curenum]	= "curenum" ;
	    n[sub_audit]	= "audit" ;
	    n[sub_close]	= "close" ;
	    n[sub_overlast]	= nullptr ;
	} ; /* end ctor */
    } ; /* end struct (subnamee) */
} /* end namespace */

constexpr subnamer	subname ;
cbool			f_debug		= CF_DEBUG ;


/* exported variables */


/* exported subroutines */

int bibleq_open(BQ *op,cchar *pr,cchar *dbn) noex {
	int		rs ;
	if ((rs = bibleq_ctor(op,pr,dbn)) >= 0) ylikely {
	    cchar	*objname = BQ_OBJNAME ;
	    if (pr[0] && dbn[0]) ylikely {
	        if ((rs = bibleq_objloadbegin(op,pr,objname)) >= 0) ylikely {
                    bibleq_calls *callp = callsp(op->callp) ;
                    rs = SR_NOSYS ;
                    if (cauto co = callp->open ; co) ylikely {
                        if ((rs = co(op->obj,pr,dbn)) >= 0) ylikely {
                            op->magval = BQ_MAG ;
                        }
                    } /* end if (non-null) */
	            if (rs < 0) {
		        bibleq_objloadend(op) ;
	            } /* end if (error) */
	        } /* end if (objload-begin) */
	    } /* end if (valid) */
	    if (rs < 0) {
		bibleq_dtor(op) ;
	    } /* end if (error) */
	} /* end if (bibleq_ctor) */
	return rs ;
} /* end subroutine (bibleq_open) */

int bibleq_close(BQ *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = bibleq_magic(op)) >= 0) ylikely {
	    rs = SR_BUGCHECK ;
            if (bibleq_calls *callp = callsp(op->callp) ; callp) ylikely {
		rs = SR_OK ;
                if (cauto co = callp->close ; co) ylikely {
                    rs1 = co(op->obj) ;
                    if (rs >= 0) rs = rs1 ; 
                } else {
                    rs = SR_NOSYS ; 
                }
	        {
	            rs1 = bibleq_objloadend(op) ;
	            if (rs >= 0) rs = rs1 ;
	        }
	        {
	            rs1 = bibleq_dtor(op) ;
	            if (rs >= 0) rs = rs1 ;
	        }
	        op->magval = 0 ;
	    } /* end if (valid) */
	} /* end if (bibleq_magic) */
	return rs ;
} /* end subroutine (bibleq_close) */

int bibleq_audit(BQ *op) noex {
	int		rs ;
	if ((rs = bibleq_magic(op)) >= 0) ylikely {
            bibleq_calls   *callp = callsp(op->callp) ;
	    rs = SR_NOSYS ;
            if (cauto co = callp->audit ; co) ylikely {
	        rs = co(op->obj) ;
	    }
	} /* end if (bibleq_magic) */
	return rs ;
} /* end subroutine (bibleq_audit) */

int bibleq_count(BQ *op) noex {
	int		rs ;
	if ((rs = bibleq_magic(op)) >= 0) ylikely {
            bibleq_calls   *callp = callsp(op->callp) ;
            if (cauto co = callp->count ; co) ylikely {
	        rs = co(op->obj) ;
	    }
	} /* end if (bibleq_magic) */
	return rs ;
} /* end subroutine (bibleq_count) */

int bibleq_curbegin(BQ *op,BQ_C *curp) noex {
    	cnothrow	nt{} ;
	int		rs ;
	if ((rs = bibleq_magic(op,curp)) >= 0) ylikely {
            rs = SR_BUGCHECK ;
            if (bibleq_calls *callp = callsp(op->callp) ; callp) ylikely {
                rs = SR_NOSYS ;
                if (cauto co = callp->curbegin ; co) ylikely {
                    rs = SR_NOMEM ;
                    if (BQS_C *bcurp = new(nt) BQS_C ; bcurp) ylikely {
                        curp->scp = bcurp ;
                        if ((rs = co(op->obj,bcurp)) >= 0) ylikely {
                            curp->magval = BQ_MAG ;
                        } /* end if */  
                        if (rs < 0) {
                            delete bcurp ;
                            curp->scp = nullptr ;
                        } /* end if (error) */
                    } /* end if (non-null) */
                } /* end if (new-BQS_C) */ 
            } /* end if (non-null) */
            if (rs < 0) {       
                memclear(curp) ;
            } /* end if (error) */
	} /* end if (bibleq_magic) */
	return rs ;
} /* end subroutine (bibleq_curbegin) */

int bibleq_curend(BQ *op,BQ_C *curp) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = bibleq_magic(op,curp)) >= 0) ylikely {
            bibleq_calls   *callp = callsp(op->callp) ;
            rs = SR_NOTOPEN ;
            if (BQS_C *bcurp = resumelife<BQS_C>(curp->scp) ; bcurp) ylikely {
                rs = SR_BADFMT ;
                if (curp->magval == BQ_MAG) ylikely {
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
	} /* end if (bibleq_magic) */
	return rs ;
} /* end subroutine (bibleq_curend) */

int bibleq_curenum(BQ *op,BQ_C *curp,BQ_Q *bcp,char *vbuf,int vlen) noex {
	int		rs ;
	if ((rs = bibleq_magic(op,curp,bcp,vbuf)) >= 0) ylikely {
            bibleq_calls   *callp = callsp(op->callp) ;
	    rs = SR_NOTOPEN ;
	    if (BQS_C *bcurp = resumelife<BQS_C>(curp->scp) ; bcurp) ylikely {
		rs = SR_BADFMT ;
		if (curp->magval == BQ_MAG) ylikely {
            	    rs = SR_NOSYS ;
                    if (cauto co = callp->curenum ; co) ylikely {
	    	        BQS_Q	sq{} ;
	    	        if ((rs = co(op->obj,bcurp,&sq,vbuf,vlen)) >= 0) {
	        	    bcp->b = sq.b ;
	        	    bcp->c = sq.c ;
	        	    bcp->v = sq.v ;
	    	        } /* end if (object-call) */
		    } /* end if (non-null) */
		} /* end if (good cursor magic) */
	    } /* end if (non-null) */
	} /* end if (bibleq_magic) */
	return rs ;
} /* end subroutine (bibleq_curenum) */

int bibleq_curlook(BQ *op,BQ_C *curp,int qo,cchar **klp) noex {
	int		rs ;
	if ((rs = bibleq_magic(op,curp,klp)) >= 0) ylikely {
            bibleq_calls   *callp = callsp(op->callp) ;
	    rs = SR_NOTOPEN ;
	    if (BQS_C *bcurp = resumelife<BQS_C>(curp->scp) ; bcurp) ylikely {
		rs = SR_BADFMT ;
		if (curp->magval == BQ_MAG) ylikely {
            	    rs = SR_NOSYS ;
                    if (cauto co = callp->curlook ; co) ylikely {
	    	        rs = co(op->obj,curp->scp,qo,klp) ;
		    } /* end if (non-null) */
		} /* end if (good cursor magic) */
	    } /* end if (non-null) */
	} /* end if (bibleq_magic) */
	return rs ;
} /* end subroutine (bibleq_curlook) */


/* private subroutines */

local int bibleq_objloadbegin(BQ *op,cchar *pr,cchar *objn) noex {
	modload		*mlp = op->mlp ;
	cint		vn = sub_overlast ;
	cint		vo = vecstrm.compact ;
	int		rs ;
	int		rs1 ;
	if (vecstr syms ; (rs = syms.start(vn,vo)) >= 0) ylikely {
	    if ((rs = syms.addsyms(objn,subname.n)) >= 0) ylikely {
	        if (mainv sv ; (rs = syms.getvec(&sv)) >= 0) ylikely {
	            cchar	*mn = BQ_MODBNAME ;
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
	                        rs = bibleq_loadcalls(op,&syms) ;
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
} /* end subroutine (bibleq_objloadbegin) */

local int bibleq_objloadend(BQ *op) noex {
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
} /* end subroutine (bibleq_objloadend) */

local int bibleq_loadcalls(BQ *op,vecstr *slp) noex {
	modload		*mlp = op->mlp ;
	cint		rsn = SR_NOTFOUND ;
	int		rs = SR_OK ;
	int		rs1 ;
	int		c = 0 ;
	cchar		*sname{} ;
	for (int i = 0 ; (rs1 = slp->get(i,&sname)) >= 0 ; i += 1) ylikely {
	    if (cvoid *snp{} ; (rs = modload_getsym(mlp,sname,&snp)) >= 0) {
                bibleq_calls   *callp = callsp(op->callp) ;
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
                case sub_curend:
                    callp->curend	= socurend_f(snp) ;
                    break ;
                case sub_curenum:
                    callp->curenum	= socurenum_f(snp) ;
                    break ;
                case sub_curlook:
                    callp->curlook	= socurlook_f(snp) ;
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
} /* end subroutine (bibleq_loadcalls) */

local bool isrequired(int i) noex {
	bool	f = false ;
	switch (i) {
	case sub_open:
	case sub_curbegin:
	case sub_curlook:
	case sub_curenum:
	case sub_curend:
	case sub_close:
	    f = true ;
	    break ;
	} /* end switch */
	return f ;
} /* end subroutine (isrequired) */


