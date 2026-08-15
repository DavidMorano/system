/* calyear SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* CALYEAR object loader */
/* version %I% last-modified %G% */

#define	CF_DEBUG	0		/* non-switchable debug print-outs */

/* revision history:

	- 2008-10-01, David A­D­ Morano
	This module was originally written.

*/

/* Copyright © 2008 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	calyear

	Description:
	This module implements an interface (a trivial one) that
	allows access to the CALYEAR datbase.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* POSIX® */
#include	<sys/param.h>		/* POSIX® */
#include	<sys/stat.h>		/* POSIX® */
#include	<unistd.h>		/* POSIX® */
#include	<fcntl.h>		/* POSIX® */
#include	<dlfcn.h>		/* POSIX® */
#include	<ctime>			/* CSTD */
#include	<climits>		/* CSTD */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstring>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<ucmem.h>		/* LIBUC */
#include	<vecstr.h>		/* LIBUC */
#include	<modload.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */
#include	<libdebug.h>		/* LIBDEBUG */
#include	<calyears.h>		/* LIBDAM */

#include	"calyear.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |memclear(3u)| */

/* local defines */

#define	CY		calyear
#define	CY_Q		calyear_q
#define	CY_FL		calyear_fl
#define	CY_CUR		calyear_cur
#define	CY_MAG		CALYEAR_MAGIC
#define	CY_DEFENTS	(44 * 1000)
#define	CY_MODBNAME	"calyears"
#define	CY_OBJNAME 	"calyears"

#define	CYS		calyears
#define	CYS_Q		calyears_q
#define	CYS_C		calyears_cur

#ifndef	CF_DEBUG
#define	CF_DEBUG	0		/* non-switchable debug print-outs */
#endif


/* imported namespaces */

using libuc::mem ;			/* variable */


/* local typedefs */

extern "C" {
    typedef int	(*soopen_f)	(void *,cchar *,con mainv,con mainv) noex ;
    typedef int	(*socount_f)	(void *) noex ;
    typedef int	(*socurbegin_f)	(void *,CYS_C *) noex ;
    typedef int	(*socurlook_f)	(void *,CYS_C *,con CYS_Q *) noex ;
    typedef int	(*socurread_f)	(void *,CYS_C *,mut CYS_Q *,char *,int) noex ;
    typedef int	(*socurend_f)	(void *,CYS_C *) noex ;
    typedef int	(*socheck_f)	(void *,time_t) noex ;
    typedef int	(*soaudit_f)	(void *) noex ;
    typedef int	(*soclose_f)	(void *) noex ;
} /* end extern (C) */


/* external subroutines */


/* external variables */


/* local structures */

struct calls {
	soopen_f	open ;
	socount_f	count ;
	socurbegin_f	curbegin ;
	socurlook_f	curlook ;
	socurread_f	curread ;
	socurend_f	curend ;
	socheck_f	check ;
	soaudit_f	audit ;
	soclose_f	close ;
} ; /* end struct (calls) */

typedef calls *		callsp ;


/* forward references */

template<typename ... Args>
local int calyear_ctor(calyear *op,Args ... args) noex {
    	CALYEAR		*hop = op ;
	cnullptr	np{} ;
	cnothrow	nt{} ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    rs = SR_NOMEM ;
	    memclear(hop) ;
	    if ((op->mlp = new(nt) modload) != np) ylikely {
	        if (callsp p ; (p = new(nt) calls) != np) ylikely {
		    op->callp = callsp(p) ;
		    rs = SR_OK ;
		} /* end if (new-calls) */
		if (rs < 0) {
		    delete op->mlp ;
		    op->mlp = np ;
		} /* end if (error) */
	    } /* end new (modload) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (calyear_ctor) */

local int calyear_dtor(calyear *op) noex {
	int		rs = SR_FAULT ;
	if (op) ylikely {
	    rs = SR_OK ;
	    if (op->callp) ylikely {
		callsp p = callsp(op->callp) ;
		delete p ;
		op->callp = nullptr ;
	    } /* end if (delete-calls) */
	    if (op->mlp) ylikely {
		delete op->mlp ;
		op->mlp = nullptr ;
	    } /* end if (delete-modload) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (calyear_dtor) */

template<typename ... Args>
local inline int calyear_magic(calyear *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magval == CALYEAR_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
} /* end subroutine (calyear_magic) */

local int	calyear_objloadbegin	(CY *,cchar *,cchar *) noex ;
local int	calyear_objloadend	(CY *) noex ;
local int	calyear_loadcalls	(CY *,vecstr *) noex ;

local CYS_Q	cyq_conv	(const CY_Q &) noex ;
local CY_Q	cysq_conv	(const CYS_Q &) noex ;

local bool	isnotrequired(int) noex ;


/* local variables */

enum subs {
	sub_open,
	sub_count,
	sub_curbegin,
	sub_curlook,
	sub_curread,
	sub_curend,
	sub_check,
	sub_audit,
	sub_close,
	sub_overlast
} ; /* end enum */

constexpr cpcchar	subnames[] = {
	"open",
	"count",
	"curbegin",
	"curlook",
	"curread",
	"curend",
	"check",
	"audit",
	"close",
	nullptr
} ; /* end array */

cbool			f_debug		= CF_DEBUG ;


/* exported variables */


/* exported subroutines */

local int calyear_opens(calyear *,cc *,con mainv,con mainv) noex ;

int calyear_open(calyear *op,cc *pr,con mainv dns,con mainv cns) noex {
	int		rs ;
	if ((rs = calyear_ctor(op,pr)) >= 0) ylikely {
	    rs = SR_INVALID ;
	    if (pr[0]) ylikely {
	        rs = calyear_opens(op,pr,dns,cns) ;
	    } /* end if (valid) */
	    if (rs < 0) {
		calyear_dtor(op) ;
	    } /* end if (error) */
	} /* end if (calyear_ctor) */
	DEBUGPRINTF("ret rs=%d\n",rs) ;
	return rs ;
} /* end subroutine (calyear_open) */

local int calyear_opens(calyear *op,cc *pr,con mainv dns,con mainv cns) noex {
    	int		rs ;
	cchar		*objname = CY_OBJNAME ;
	if ((rs = calyear_objloadbegin(op,pr,objname)) >= 0) ylikely {
	    if (calls *p = resumelife<calls>(op->callp) ; p) ylikely {
	        if ((rs = p->open(op->obj,pr,dns,cns)) >= 0) {
		    op->magval = CY_MAG ;
	        }
	    } /* end if (non-null) */
	    if (rs < 0) {
		calyear_objloadend(op) ;
	    } /* end if (error) */
	} /* end if (objload-begin) */
	return rs ;
} /* end subroutine (calyear_opens) */

int calyear_close(CY *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = calyear_magic(op)) >= 0) ylikely {
	    if (op->callp && op->obj) ylikely {
		if (callsp p = resumelife<calls>(op->callp) ; p) {
		    if (p->close) {
	                rs1 = p->close(op->obj) ;
	                if (rs >= 0) rs = rs1 ;
		    } else {
		        rs = SR_BUGCHECK ;
		    }
		} else {
		    rs = SR_BUGCHECK ;
		}
	    } else {
		rs = SR_BUGCHECK ;
	    }
	    {
	        rs1 = calyear_objloadend(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = calyear_dtor(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    op->magval = 0 ;
	} /* end if (calyear_magic) */
	return rs ;
} /* end subroutine (calyear_close) */

int calyear_count(CY *op) noex {
	int		rs ;
	if ((rs = calyear_magic(op)) >= 0) ylikely {
	    rs = SR_BUGCHECK ;
	    if (op->callp && op->obj) ylikely {
	        if (callsp p = resumelife<calls>(op->callp) ; p) ylikely {
		    rs = SR_NOSYS ;
		    if (p->count) {
		        rs = p->count(op->obj) ;
		    }
	        }
	    }
	} /* end if (calyear_magic) */
	return rs ;
} /* end subroutine (calyear_count) */

local int calyear_curbegs(CY *op,CY_CUR *curp) noex {
    	int		rs = SR_BUGCHECK ;
	if (callsp p = resumelife<calls>(op->callp) ; p) ylikely {
	    if (CYS_C *scp = resumelife<CYS_C>(curp->scp) ; scp) ylikely {
	        rs = p->curbegin(op->obj,scp) ;
	    }
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (calyear_curbegs) */

int calyear_curbegin(CY *op,CY_CUR *curp) noex {
	int		rs ;
	if ((rs = calyear_magic(op,curp)) >= 0) ylikely {
	    rs = SR_BUGCHECK ;
	    curp->scp = nullptr ;
	    curp->magval = 0 ;
	    if (op->callp && op->obj) ylikely {
	        if (callsp p = resumelife<calls>(op->callp) ; p) ylikely {
		    if (p->curbegin) {
			cint sz = op->cursz ;
			if (voidp vp ; (rs = mem.mall(sz,&vp)) >= 0) {
			    curp->scp = vp ;
			    if ((rs = calyear_curbegs(op,curp)) >= 0) {
				curp->magval = CY_MAG ;
			    } /* end if (calyear_curbegs) */
			    if (rs < 0) {
				mem.free(vp) ;
				curp->scp = nullptr ;
			    } /* end if (error) */
			} /* end if (memory-acquire) */
		    } /* end if (non-null) */
		} /* end if (non-null) */
	    } /* end if (non-null) */
	} /* end if (calyear_magic) */
	return rs ;
} /* end subroutine (calyear_curbegin) */

local int calyear_curends(CY *,CY_CUR *,calls *) noex ;

int calyear_curend(CY *op,CY_CUR *curp) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = calyear_magic(op,curp)) >= 0) ylikely {
	    rs = SR_NOTOPEN ;
	    if (curp->magval == CY_MAG) ylikely {
		rs = SR_BUGCHECK ;
	        if (op->callp && op->obj) ylikely {
	            if (callsp p = resumelife<calls>(op->callp) ; p) ylikely {
			rs = SR_BUGCHECK ;
			if (curp->scp) {
			    rs = SR_OK ;
		            {
			        rs1 = calyear_curends(op,curp,p) ;
	    		        if (rs >= 0) rs = rs1 ;
		            }
		            {
	    		        rs1 = mem.free(curp->scp) ;
	    		        if (rs >= 0) rs = rs1 ;
	    		        curp->scp = nullptr ;
		            }
			} /* end if (non-null) */
		        curp->magval = 0 ;
		    } /* end if (cursor-magic) */
		} /* end if (non-null) */
	    } /* end if (cursor-magic) */
	} /* end if (calyear_magic) */
	return rs ;
} /* end subroutine (calyear_curend) */

local int calyear_curends(CY *op,CY_CUR *curp,calls *p) noex {
    	int		rs = SR_BUGCHECK ;
	int		rs1 ;
	if (p->curend) {
	    if (CYS_C *scp = resumelife<CYS_C>(curp->scp) ; scp) {
		rs = SR_OK ;
		{
		    rs1 = p->curend(op->obj,scp) ;
		    if (rs >= 0) rs = rs1 ;
		}
	    } /* end if (non-null) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (calyear_curends) */

/* args: 'qp' is in an input */
int calyear_curlook(CY *op,CY_CUR *curp,con CY_Q *qp) noex {
	int		rs ;
	DEBUGPRINTF("ent\n") ;
	if ((rs = calyear_magic(op,curp,qp)) >= 0) ylikely {
	    rs = SR_NOTOPEN ;
	    if (curp->magval == CY_MAG) ylikely {
	        rs = SR_BUGCHECK ;
	        if (op->callp && op->obj) ylikely {
	            if (callsp p = resumelife<calls>(op->callp) ; p) ylikely {
			CYS_C *scp = resumelife<CYS_C>(curp->scp) ;
			rs = SR_BUGCHECK ;
			if (curp->scp) {
			    rs = SR_NOSYS ;
		            if (cauto fp = p->curlook ; fp) {
				con CYS_Q cyscq = cyq_conv(*qp) ;
				voidp objp = op->obj ;
				rs = fp(objp,scp,&cyscq) ;
			    } /* end if (non-null) */
			} /* end if (non-null) */
		    } /* end if (non-null) */
	        } /* end if (buf-check) */
	    } /* end if (cursor-magic) */
	} /* end if (calyear_magic) */
	DEBUGPRINTF("ret rs=%d\n",rs) ;
	return rs ;
} /* end subroutine (calyear_curlook) */

/* args: 'qp' is an output */
int calyear_curread(CY *op,CY_CUR *curp,mut CY_Q *qp,
		char *rbuf,int rlen) noex {
	int		rs ;
	if ((rs = calyear_magic(op,curp,qp,rbuf)) >= 0) ylikely {
	    rs = SR_NOTOPEN ;
	    if (curp->magval == CY_MAG) ylikely {
	        rs = SR_BUGCHECK ;
	        if (op->callp && op->obj) ylikely {
	            if (callsp p = resumelife<calls>(op->callp) ; p) ylikely {
			CYS_C *scp = resumelife<CYS_C>(curp->scp) ;
			rs = SR_BUGCHECK ;
			if (curp->scp) {
			    rs = SR_NOSYS ;
		            if (cauto fp = p->curread ; fp) {
				mut CYS_Q cyscq{} ;
				voidp objp = op->obj ;
				if ((rs = fp(objp,scp,&cyscq,rbuf,rlen)) >= 0) {
				    *qp = cysq_conv(cyscq) ;
				} /* end if */
			    } /* end if (non-null) */
			} /* end if (non-null) */
		    }
		} /* end if (bug-check) */
	    } /* end if (cursor-magic) */
	} /* end if (calyear_magic) */
	return rs ;
} /* end subroutine (calyear_curread) */

int calyear_check(CY *op,time_t dt) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = calyear_magic(op)) >= 0) ylikely {
	    rs = SR_BUGCHECK ;
	    if (op->callp && op->obj) ylikely {
		if (callsp p = resumelife<calls>(op->callp) ; p) {
		    rs = SR_NOSYS ;
		    if (p->check) {
	                rs1 = p->check(op->obj,dt) ;
	                if (rs >= 0) rs = rs1 ;
		    }
		}
	    } /* end if (bug-check) */
	} /* end if (calyear_magic) */
	return rs ;
} /* end subroutine (calyear_check) */

int calyear_audit(CY *op) noex {
	int		rs ;
	if ((rs = calyear_magic(op)) >= 0) ylikely {
	    rs = SR_BUGCHECK ;
	    if (op->callp && op->obj) ylikely {
		if (callsp p = resumelife<calls>(op->callp) ; p) {
		    rs = SR_NOSYS ;
		    if (p->audit) {
		        rs = p->audit(op->obj) ;
		    }
		}
	    }
	} /* end if (calyear_magic) */
	return rs ;
} /* end subroutine (calyear_audit) */


/* private subroutines */

local int calyear_objloadbegin(CY *op,cchar *pr,cchar *objn) noex {
	modload		*mlp = op->mlp ;
	cint		vn = sub_overlast ;
	cint		vo = vecstrm.compact ;
	int		rs ;
	int		rs1 ;
	if (vecstr syms ; (rs = syms.start(vn,vo)) >= 0) ylikely {
	    if ((rs = syms.addsyms(objn,subnames)) >= 0) ylikely {
	        if (mainv sv ; (rs = syms.getvec(&sv)) >= 0) ylikely {
	            cchar	*mn = CY_MODBNAME ;
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
	                        rs = calyear_loadcalls(op,&syms) ;
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
} /* end subroutine (calyear_objloadbegin) */

local int calyear_objloadend(CY *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (op->obj) {
	    rs1 = mem.free(op->obj) ;
	    if (rs >= 0) rs = rs1 ;
	    op->obj = nullptr ;
	}
	if (op->mlp && op->fl.modload) {
	    op->fl.modload = false ;
	    rs1 = modload_close(op->mlp) ;
	    if (rs >= 0) rs = rs1 ;
	}
	return rs ;
} /* end subroutine (calyear_objloadend) */

local int calyear_loadcalls(CY *op,vecstr *slp) noex {
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
                case sub_curlook:
                    callp->curlook	= socurlook_f(snp) ;
                    break ;
                case sub_curread:
                    callp->curread	= socurread_f(snp) ;
                    break ;
                case sub_curend:
                    callp->curend	= socurend_f(snp) ;
                    break ;
                case sub_audit:
                    callp->audit	= soaudit_f(snp) ;
                    break ;
                case sub_close:
                    callp->close	= soclose_f(snp) ;
                    break ;
                } /* end switch */
            } else if (rs == rsn) {
                if (isnotrequired(i)) rs = SR_OK ;
            } /* end if (it had the call) */
	    if (rs < 0) break ;
	} /* end for (vecstr_get) */
	if ((rs >= 0) && (rs1 != rsn)) rs = rs1 ;
	return (rs >= 0) ? c : rs ;
} /* end subroutine (calyear_loadcalls) */

local bool isnotrequired(int i) noex {
	bool	f = true ;
	switch (i) {
	case sub_open:
	case sub_curbegin:
	case sub_curlook:
	case sub_curread:
	case sub_curend:
	case sub_close:
	    f = false ;
	    break ;
	} /* end switch */
	return f ;
} /* end subroutine (isnotrequired) */

local CYS_Q cyq_conv(const CY_Q &q) noex {
    	CYS_Q	cysq{} ;
	cysq.y = q.y ;
	cysq.m = q.m ;
	cysq.d = q.d ;
	return cysq ;
} /* end subroutine (cyq_conv) */

local CY_Q cysq_conv	(const CYS_Q &q) noex {
    	CY_Q	cyq{} ;
	cyq.y = q.y ;
	cyq.m = q.m ;
	cyq.d = q.d ;
	return cyq ;
} /* end subroutine (cysq_conv) */


