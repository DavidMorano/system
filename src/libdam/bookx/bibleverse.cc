/* bibleverse SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* BV object-load management */
/* version %I% last-modified %G% */

#define	CF_DEBUG	0		/* non-switchable debug print-outs */

/* revision history:

	- 2008-10-01, David A­D­ Morano
	This module was originally written.

*/

/* Copyright © 2008 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	bibleverse

  	Description:
	This module implements an interface (a trivial one) that
	allows access to the BV datbase.

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
#include	<biblecite.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */
#include	<libdebug.h>		/* LIBDEBUG |DEBUGPRINTF(3debug)| */

#include	"bibleverses.h"
#include	"bibleverse.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |memclear(3u)| */

/* local defines */

#define	BIBLEVERSE_DEFENTS	(44 * 1000)
#define	BIBLEVERSE_MODBNAME	"bibleverses"
#define	BIBLEVERSE_OBJNAME	"bibleverses"

#define	BV		bibleverse
#define	BV_CUR		bibleverse_cur
#define	BV_CITE		bibleverse_cite
#define	BV_Q		bibleverse_q
#define	BV_I		bibleverse_info
#define	BV_CA		bibleverse_ca
#define	BV_MODBNAME	BIBLEVERSE_MODBNAME
#define	BV_MAG		BIBLEVERSE_MAGIC

#define	BVSS		bibleverses
#define	BVSS_Q		bibleverses_q
#define	BVSS_C		bibleverses_cur
#define	BVSS_I		bibleverses_info

#ifndef	CF_DEBUG
#define	CF_DEBUG	0		/* non-switchable debug print-outs */
#endif


/* imported namespaces */

using libuc::mem ;			/* variable */


/* local typedefs */

extern "C" {
    typedef int	(*soopen_f)	(void *,cchar *,cchar *) noex ;
    typedef int	(*socount_f)	(void *) noex ;
    typedef int	(*soread_f)	(void *,char *,int,con BVSS_Q *) noex ;
    typedef int	(*soget_f)	(void *,con BVSS_Q *,char *,int) noex ;
    typedef int	(*socurbegin_f)	(void *,BVSS_C *) noex ;
    typedef int	(*socurend_f)	(void *,BVSS_C *) noex ;
    typedef int	(*socurenum_f)	(void *,BVSS_C *,BVSS_Q *,char *,int) noex ;
    typedef int	(*soaudit_f)	(void *) noex ;
    typedef int	(*sogetinfo_f)	(void *,BVSS_I *) noex ;
    typedef int	(*sochapters_f)	(void *,int,uchar *,int) noex ;
    typedef int	(*soclose_f)	(void *) noex ;
} /* end extern (C) */


/* external subroutines */


/* external variables */


/* local structures */

struct bibleverse_calls {
    soopen_f		open ;
    socount_f		count ;
    soread_f		read ;
    soget_f		get ;
    socurbegin_f	curbegin ;
    socurend_f		curend ;
    socurenum_f		curenum ;
    soaudit_f		audit ;
    sogetinfo_f		getinfo ;
    sochapters_f	chapters ;
    soclose_f		close ;
} ; /* end struct (bibleverse_calls) */

typedef bibleverse_calls *	callsp ;


/* forward references */

template<typename ... Args>
local int bibleverse_ctor(BV *op,Args ... args) noex {
	BIBLEVERSE	*hop = op ;
	cnullptr	np{} ;
	cnothrow	nt{} ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    memclear(hop) ;
	    rs = SR_NOMEM ;
	    if ((op->mlp = new(nt) modload) != np) ylikely {
		bibleverse_calls    *callp ;
                if ((callp = new(nt) bibleverse_calls) != np) ylikely {
                    op->callp = callp ;
                    rs = SR_OK ;
                } /* end if (new-bibleverse_calls) */
                if (rs < 0) {
                    delete op->mlp ;
                    op->mlp = nullptr ;
                } /* end if (error) */
	    } /* end if (new-modload) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (bibleverse_ctor) */

local int bibleverse_dtor(BV *op) noex {
	int		rs = SR_FAULT ;
	if (op) ylikely {
	    rs = SR_OK ;
            if (op->callp) ylikely {
                bibleverse_calls    *callp = callsp(op->callp) ;
                delete callp ;
                op->callp = nullptr ;
            }
	    if (op->mlp) ylikely {
		delete op->mlp ;
		op->mlp = nullptr ;
	    }
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (bibleverse_dtor) */

template<typename ... Args>
local inline int bibleverse_magic(BV *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    rs = (op->magval == BV_MAG) ? SR_OK : SR_NOTOPEN ;
	} /* end if */
	return rs ;
} /* end subroutine (bibleverse_magic) */

local int	bibleverse_objloadbegin	(BV *,cchar *,cchar *) noex ;
local int	bibleverse_objloadend	(BV *) noex ;
local int	bibleverse_loadcalls	(BV *,vecstr *) noex ;

local bool	isrequired(int) noex ;


/* external variables */


/* local variables */

enum subs {
	sub_open,
	sub_count,
	sub_read,
	sub_get,
	sub_curbegin,
	sub_curenum,
	sub_curend,
	sub_getinfo,
	sub_chapters,
	sub_audit,
	sub_close,
	sub_overlast
} ; /* end enum (subs) */

namespace {
    struct subnamer {
	cchar		*n[sub_overlast + 1] ;
	consteval subnamer() noex {
	    n[sub_open]		= "open" ;
	    n[sub_count]	= "count" ;
	    n[sub_read]		= "read" ;
	    n[sub_get]		= "get" ;
	    n[sub_curbegin]	= "curbegin" ;
	    n[sub_curenum]	= "curenum" ;
	    n[sub_curend]	= "curend" ;
	    n[sub_getinfo]	= "getinfo" ;
	    n[sub_chapters]	= "chapters" ;
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

int bibleverse_open(BV *op,cchar *pr,cchar *dbn) noex {
	int		rs ;
	cchar		*objn = BIBLEVERSE_OBJNAME ;
	if ((rs = bibleverse_ctor(op,pr,dbn)) >= 0) ylikely {
	    rs = SR_INVALID ;
	    if (pr[0]) ylikely {
	        if ((rs = bibleverse_objloadbegin(op,pr,objn)) >= 0) ylikely {
		    bibleverse_calls *callp = callsp(op->callp) ;
		    rs = SR_NOSYS ;
		    if (cauto co = callp->open ; co) ylikely {
                        if ((rs = co(op->obj,pr,dbn)) >= 0) {
		            op->magval = BV_MAG ;
			}
	            } /* end if (non-null) */
	            if (rs < 0) {
		        bibleverse_objloadend(op) ;
	            } /* end if (error) */
	        } /* end if (objload-begin) */
	    } /* end if (valid) */
	    if (rs < 0) {
		bibleverse_dtor(op) ;
	    } /* end if (error) */
	} /* end if (bibleverse_ctor) */
	return rs ;
} /* end subroutine (bibleverse_open) */

/* finish the entire vector string data structure object */
int bibleverse_close(BV *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = bibleverse_magic(op)) >= 0) ylikely {
	    rs = SR_BUGCHECK ;
            if (bibleverse_calls *callp = callsp(op->callp) ; callp) ylikely {
		rs = SR_OK ;
                if (cauto co = callp->close ; co) ylikely {
                    rs1 = co(op->obj) ;
                    if (rs >= 0) rs = rs1 ;
                } else {
                    rs = SR_NOSYS ;
                }
	        {
	            rs1 = bibleverse_objloadend(op) ;
	            if (rs >= 0) rs = rs1 ;
	        }
	        {
	            rs1 = bibleverse_dtor(op) ;
	            if (rs >= 0) rs = rs1 ;
	        }
	        op->magval = 0 ;
	    } /* end if (valid) */
	} /* end if (bibleverse_magic) */
	DEBUGPRINTF("ret rs=%d\n",rs) ;
	return rs ;
} /* end subroutine (bibleverse_close) */

int bibleverse_count(BV *op) noex {
	int		rs ;
	if ((rs = bibleverse_magic(op)) >= 0) ylikely {
            bibleverse_calls   *callp = callsp(op->callp) ;
            rs = SR_NOSYS ;
            if (cauto co = callp->count ; co) ylikely {
                rs = co(op->obj) ;
            }
	} /* end if (bibleverse_magic) */
	return rs ;
} /* end subroutine (bibleverse_count) */

int bibleverse_read(BV *op,char *vbuf,int vlen,con BV_Q *qp) noex {
	int		rs ;
	DEBUGPRINTF("ent q=%u:%u:%u\n", qp->b,qp->c,qp->v) ;
	if ((rs = bibleverse_magic(op,vbuf,qp)) >= 0) ylikely {
            bibleverse_calls   *callp = callsp(op->callp) ;
	    BVSS_Q	sq{} ;
	    sq.b = qp->b ;
	    sq.c = qp->c ;
	    sq.v = qp->v ;
            rs = SR_NOSYS ;
            if (cauto co = callp->read ; co) {
                rs = co(op->obj,vbuf,vlen,&sq) ;
            }
	} /* end if (bibleverse_magic) */
	DEBUGPRINTF("ret rs=%d\n",rs) ;
	return rs ;
} /* end subroutine (bibleverse_read) */

int bibleverse_get(BV *op,con BV_Q *qp,char *vbuf,int vlen) noex {
	return bibleverse_read(op,vbuf,vlen,qp) ;
} /* end subroutine (bibleverse_get) */

int bibleverse_curbegin(BV *op,BV_CUR *curp) noex {
    	cnothrow	nt{} ;
	int		rs ;
	if ((rs = bibleverse_magic(op,curp)) >= 0) ylikely {
	    rs = SR_BUGCHECK ;
            if (bibleverse_calls *callp = callsp(op->callp) ; callp) ylikely {
		rs = SR_NOSYS ;
		if (cauto co = callp->curbegin ; co) ylikely {
		    rs = SR_NOMEM ;
		    if (BVSS_C *bcurp = new(nt) BVSS_C ; bcurp) ylikely {
			curp->scp = bcurp ;
                        if ((rs = co(op->obj,bcurp)) >= 0) ylikely {
			    curp->magval = BV_MAG ;
			} /* end if */
	                if (rs < 0) {
		            delete bcurp ;
		            curp->scp = nullptr ;
	                } /* end if (error) */
		    } /* end if (non-null) */
		} /* end if (new-BVSS_C) */
	    } /* end if (non-null) */
	    if (rs < 0) {
	        memclear(curp) ;
	    } /* end if (error) */
	} /* end if (bibleverse_magic) */
	return rs ;
} /* end subroutine (bibleverse_curbegin) */

int bibleverse_curend(BV *op,BV_CUR *curp) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = bibleverse_magic(op,curp)) >= 0) ylikely {
            bibleverse_calls   *callp = callsp(op->callp) ;
	    rs = SR_NOTOPEN ;
	    if (BVSS_C *bcurp = resumelife<BVSS_C>(curp->scp) ; bcurp) ylikely {
		rs = SR_BADFMT ;
		if (curp->magval == BV_MAG) ylikely {
		    rs = SR_NOSYS ;
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
	} /* end if (bibleverse_magic) */
	return rs ;
} /* end subroutine (bibleverse_curend) */

int bibleverse_curenum(BV *op,BV_CUR *curp,BV_Q *qp,char *vbuf,int vlen) noex {
	int		rs ;
	if ((rs = bibleverse_magic(op,curp,qp,vbuf)) >= 0) ylikely {
            bibleverse_calls   *callp = callsp(op->callp) ;
	    rs = SR_NOTOPEN ;
	    if (BVSS_C *bcurp = resumelife<BVSS_C>(curp->scp) ; bcurp) ylikely {
		rs = SR_BADFMT ;
		if (curp->magval == BV_MAG) ylikely {
            	    rs = SR_NOSYS ;
                    if (cauto co = callp->curenum ; co) ylikely {
	    	        BVSS_Q	sq{} ;
	    	        if ((rs = co(op->obj,bcurp,&sq,vbuf,vlen)) >= 0) {
	        	    qp->b = sq.b ;
	        	    qp->c = sq.c ;
	        	    qp->v = sq.v ;
	    	        } /* end if (object-call) */
		    } /* end if (non-null) */
		} /* end if (good cursor magic) */
	    } /* end if (non-null) */
	} /* end if (bibleverse_magic) */
	DEBUGPRINTF("ret rs=%d\n",rs) ;
	return rs ;
} /* end subroutine (bibleverse_curenum) */

int bibleverse_getinfo(BV *op,BV_I *ip) noex {
	int		rs ;
	int		nv = 0 ; /* return-value */
	DEBUGPRINTF("ent\n") ;
	if ((rs = bibleverse_magic(op,ip)) >= 0) ylikely {
            bibleverse_calls   *callp = callsp(op->callp) ;
	    rs = SR_NOSYS ;
            if (cauto co = callp->getinfo ; co) ylikely {
	        if (BVSS_I bi{} ; (rs = co(op->obj,&bi)) >= 0) ylikely {
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
	} /* end if (bibleverse_magic) */
	DEBUGPRINTF("ret rs=%d nv=%u\n",rs,nv) ;
	return (rs >= 0) ? nv : rs ;
} /* end subroutine (bibleverse_getinfo) */

int bibleverse_chapters(BV *op,int book,uchar *ap,int al) noex {
	int		rs ;
	if ((rs = bibleverse_magic(op,ap)) >= 0) ylikely {
            bibleverse_calls   *callp = callsp(op->callp) ;
            rs = SR_NOSYS ;
            if (cauto co = callp->chapters ; co) ylikely {
                rs = co(op->obj,book,ap,al) ;
            }
	} /* end if (bibleverse_magic) */
	return rs ;
} /* end subroutine (bibleverse_chapters) */

int bibleverse_audit(BV *op) noex {
	int		rs ;
	if ((rs = bibleverse_magic(op)) >= 0) {
            bibleverse_calls   *callp = callsp(op->callp) ;
            rs = SR_NOSYS ;
            if (callp->audit) {
                cauto   co = callp->audit ;
                rs = co(op->obj) ;
            } 
	} /* end if (bibleverse_magic) */
	return rs ;
} /* end subroutine (bibleverse_audit) */


/* private subroutines */

local int bibleverse_objloadbegin(BV *op,cchar *pr,cchar *objn) noex {
	modload		*mlp = op->mlp ;
	cint		vn = sub_overlast ;
	cint		vo = vecstrm.compact ;
	int		rs ;
	int		rs1 ;
	if (vecstr syms ; (rs = syms.start(vn,vo)) >= 0) ylikely {
	    if ((rs = syms.addsyms(objn,subname.n)) >= 0) ylikely {
	        if (mainv sv ; (rs = syms.getvec(&sv)) >= 0) ylikely {
	            cchar	*mn = BV_MODBNAME ;
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
	                        rs = bibleverse_loadcalls(op,&syms) ;
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
} /* end subroutine (bibleverse_objloadbegin) */

local int bibleverse_objloadend(BV *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (op->obj) ylikely {
	    rs1 = mem.free(op->obj) ;
	    if (rs >= 0) rs = rs1 ;
	    op->obj = nullptr ;
	} /* end if (memory-release) */
	if (op->mlp && op->fl.modload) ylikely {
	    op->fl.modload = false ;
	    rs1 = modload_close(op->mlp) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if */
	return rs ;
} /* end subroutine (bibleverse_objloadend) */

local int bibleverse_loadcalls(BV *op,vecstr *slp) noex {
	modload		*mlp = op->mlp ;
	cint		rsn = SR_NOTFOUND ;
	int		rs = SR_OK ;
	int		rs1 ;
	int		c = 0 ;
	cchar		*sname{} ;
	for (int i = 0 ; (rs1 = slp->get(i,&sname)) >= 0 ; i += 1) {
	    if (cvoid *snp{} ; (rs = modload_getsym(mlp,sname,&snp)) >= 0) {
                bibleverse_calls   *callp = callsp(op->callp) ;
                c += 1 ;
                switch (i) {
                case sub_open:
                    callp->open		= soopen_f(snp) ;
                    break ;
                case sub_count:
                    callp->count	= socount_f(snp) ;
                    break ;
                case sub_read:
                    callp->read		= soread_f(snp) ;
                    break ;
                case sub_get:
                    callp->get		= soget_f(snp) ;
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
                case sub_audit:
                    callp->audit	= soaudit_f(snp) ;
                    break ;
		case sub_getinfo:
		    callp->getinfo	= sogetinfo_f(snp) ;
		    break ;
		case sub_chapters:
		    callp->chapters	= sochapters_f(snp) ;
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
} /* end subroutine (bibleverse_loadcalls) */

local bool isrequired(int i) noex {
	bool		f = false ;
	switch (i) {
	case sub_open:
	case sub_read:
	case sub_get:
	case sub_close:
	    f = true ;
	    break ;
	} /* end switch */
	return f ;
} /* end subroutine (isrequired) */


