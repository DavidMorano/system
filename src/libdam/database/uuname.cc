/* uuname SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* caller interface to a UUNAME database */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-01, David A­D­ Morano
	This module was originally written.

*/

/* Copyright � 1998 David A-D- Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	uuname

	Description:
	This module implements an interface (a trivial one) that
	provides access to the UUNAME object (which is dynamically
	loaded).

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<new>			/* |nothrow(3c++)| */
#include	<usystem.h>
#include	<modload.h>
#include	<vecstr.h>
#include	<sncpyx.h>
#include	<localmisc.h>

#include	"uuname.h"
#include	"uunames.h"


/* local defines */

#define	UN		uuname
#define	UN_MODBNAME	UUNAME_MODBNAME
#define	UN_OBJNAME	UUNAME_OBJNAME

#define	LIBCNAME	"lib"


/* imported namespaces */

using std::nullptr_t ;			/* type */
using std::nothrow ;			/* constant */


/* local typedefs */

extern "C" {
    typedef int (*soopen_f)(void *,cchar *,cchar *) noex ;
    typedef int (*socount_f)(void *) noex ;
    typedef int (*soexists_f)(void *,cchar *,int) noex ;
    typedef int (*socurbegin_f)(void *,void *) noex ;
    typedef int (*socurenum_f)(void *,void *,char *,int) noex ;
    typedef int (*socurend_f)(void *,void *) noex ;
    typedef int (*soaudit_f)(void *) noex ;
    typedef int (*soclose_f)(void *) noex ;
}


/* external subroutines */


/* external variables */


/* local structures */

struct uuname_calls {
    soopen_f		open ;
    socount_f		count ;
    soexists_f		exists ;
    socurbegin_f	curbegin ;
    socurenum_f		curenum ;
    socurend_f		curend ;
    soaudit_f		audit ;
    soclose_f		close ;
} ; /* end struct (uuname_calls) */

typedef uuname_calls	calls ;
typedef uuname_calls *	callsp ;


/* forward references */

template<typename ... Args>
static int uuname_ctor(uuname *op,Args ... args) noex {
    	UUNAME		*hop = op ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    cnullptr	np{} ;
	    rs = SR_NOMEM ;
	    memclear(hop) ;
	    if ((op->mlp = new(nothrow) modload) != np) {
		if (calls *p ; (p = new(nothrow) calls) != np) {
		    op->callp = p ;
		    rs = memclear(p) ;
		}
		if (rs < 0) {
		    delete op->mlp ;
		    op->mlp = nullptr ;
		}
	    } /* end if (new-modload) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (uuname_ctor) */

static int uuname_dtor(uuname *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	    if (op->callp) {
		calls	*callp = (calls *) op->callp ;
		delete callp ;
		op->callp = nullptr ;
	    }
	    if (op->mlp) {
		delete op->mlp ;
		op->mlp = nullptr ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (uuname_dtor) */

template<typename ... Args>
static inline int uuname_magic(uuname *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magic == UUNAME_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}
/* end subroutine (uuname_magic) */

static int	uuname_objloadbegin(UN *,cchar *,cchar *) noex ;
static int	uuname_objloadend(UN *) noex ;
static int	uuname_loadcalls(UN *,vecstr *) noex ;

static bool	isrequired(int) noex ;


/* external variables */


/* local variables */

enum subs {
	sub_open,
	sub_count,
	sub_exists,
	sub_curbegin,
	sub_curenum,
	sub_curend,
	sub_audit,
	sub_close,
	sub_overlast
} ;

constexpr cpcchar	subs[] = {
	"open",
	"count",
	"exists",
	"curbegin",
	"curenum",
	"curend",
	"audit",
	"close",
	nullptr
} ;


/* exported variables */


/* exported subroutines */

int uuname_open(uuname *op,cchar *pr,cchar *dbname) noex {
	cchar		*objname = UUNAME_OBJNAME ;
	int		rs ;
	if ((rs = uuname_ctor(op,pr,dbname)) >= 0) {
	    rs = SR_INVALID ;
	    if (dbname[0]) {
	        if ((rs = uuname_objloadbegin(op,pr,objname)) >= 0) {
		    calls *callp = (calls *) op->callp ;
	            if ((rs = callp->open(op->obj,pr,dbname)) >= 0) {
	                op->magic = UUNAME_MAGIC ;
	            }
	            if (rs < 0) {
		        uuname_objloadend(op) ;
	            }
	        } /* end if )uuname_objloadbegin) */
	    } /* end if (valid) */
	    if (rs < 0) {
		uuname_dtor(op) ;
	    }
	} /* end if (uuname_ctor) */
	return rs ;
}
/* end subroutine (uuname_open) */

int uuname_close(uuname *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = uuname_magic(op)) >= 0) {
	    if (op->obj) {
		calls *callp = (calls *) op->callp ;
	        rs1 = callp->close(op->obj) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = uuname_objloadend(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
		rs1 = uuname_dtor(op) ;
		if (rs >= 0) rs = rs1 ;
	    }
	    op->magic = 0 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (uuname_close) */

int uuname_audit(uuname *op) noex {
	int		rs ;
	if ((rs = uuname_magic(op)) >= 0) {
	    calls *callp = (calls *) op->callp ;
	    if (callp->audit) {
	        rs = callp->audit(op->obj) ;
	    }
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (uuname_audit) */

int uuname_count(uuname *op) noex {
	int		rs ;
	if ((rs = uuname_magic(op)) >= 0) {
	    calls *callp = (calls *) op->callp ;
	    if (callp->count) {
	        rs = callp->count(op->obj) ;
	    }
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (uuname_count) */

int uuname_curbegin(uuname *op,UUNAME_CUR *curp) noex {
	int		rs ;
	if ((rs = uuname_magic(op,curp)) >= 0) {
	    calls *callp = (calls *) op->callp ;
	    memclear(curp) ;
	    if (callp->curbegin) {
	        if (void *p ; (rs = uc_malloc(op->cursz,&p)) >= 0) {
	            curp->scp = p ;
	            if ((rs = callp->curbegin(op->obj,curp->scp)) >= 0) {
	                curp->magic = UUNAME_MAGIC ;
	            }
	            if (rs < 0) {
		        uc_free(curp->scp) ;
		        curp->scp = nullptr ;
	            }
	        } /* end if (m-a) */
	    } /* end if (have SO method) */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (uuname_curbegin) */

int uuname_curend(uuname *op,UUNAME_CUR *curp) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = uuname_magic(op,curp)) >= 0) {
	    rs = SR_NOTOPEN ;
	    if (curp->magic == UUNAME_MAGIC) {
		rs = SR_BUGCHECK ;
	        if (curp->scp) {
		    calls *callp = (calls *) op->callp ;
		    rs = SR_OK ;
	            if (callp->curend) {
	                rs1 = callp->curend(op->obj,curp->scp) ;
	                if (rs >= 0) rs = rs1 ;
	            }
	            {
	                rs1 = uc_free(curp->scp) ;
	                if (rs >= 0) rs = rs1 ;
	                curp->scp = nullptr ;
	            }
		} /* end if (cursor) */
	        curp->magic = 0 ;
	    } /* end if (cursor-magic) */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (uuname_curend) */

int uuname_exists(uuname *op,cchar *sp,int sl) noex {
	int		rs ;
	if ((rs = uuname_magic(op)) >= 0) {
	    calls *callp = (calls *) op->callp ;
	    if (callp->exists) {
	        rs = callp->exists(op->obj,sp,sl) ;
	    }
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (uuname_exists) */

int uuname_enum(uuname *op,UUNAME_CUR *curp,char *rbuf,int rlen) noex {
	int		rs ;
	if ((rs = uuname_magic(op,curp,rbuf)) >= 0) {
	    rs = SR_NOTOPEN ;
	    if (curp->magic) {
		calls *callp = (calls *) op->callp ;
		rs = SR_NOSYS ;
	        if (callp->curenum) {
	            rs = callp->curenum(op->obj,curp->scp,rbuf,rlen) ;
	        }
	    } /* end if (cursor-magic) */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (uuname_enum) */


/* private subroutines */

static int uuname_objloadbegin(UN *op,cchar *pr,cchar *objn) noex {
	modload		*mlp = op->mlp ;
	cint		vn = sub_overlast ;
	cint		vo = VECSTR_OCOMPACT ;
	int		rs ;
	int		rs1 ;
	if (vecstr syms ; (rs = syms.start(vn,vo)) >= 0) {
	    if ((rs = syms.addsyms(objn,subs)) >= 0) {
	        if (mainv sv ; (rs = syms.getvec(&sv)) >= 0) {
	            cchar	*mn = UN_MODBNAME ;
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
			    if (void *vp ; (rs = uc_malloc(osz,&vp)) >= 0) {
	                        op->obj = vp ;
	                        rs = uuname_loadcalls(op,&syms) ;
	                        if (rs < 0) {
	                            uc_free(op->obj) ;
	                            op->obj = nullptr ;
	                        }
	                    } /* end if (memory-allocation) */
	                } /* end if (modload_getmva) */
	                if (rs < 0) {
		            op->fl.modload = false ;
	                    modload_close(mlp) ;
	                }
	            } /* end if (modload_open) */
		} /* end if (vecstr_getvec) */
	    } /* end if (vecstr_addsyms) */
	    rs1 = syms.finish ;
	    if (rs >= 0) rs = rs1 ;
	    if ((rs < 0) && op->fl.modload) {
		op->fl.modload = false ;
		modload_close(mlp) ;
	    }
	} /* end if (vecstr-syms) */
	return rs ;
}
/* end subroutine (uuname_objloadbegin) */

static int uuname_objloadend(UN *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (op->obj) {
	    rs1 = uc_free(op->obj) ;
	    if (rs >= 0) rs = rs1 ;
	    op->obj = nullptr ;
	}
	if (op->mlp && op->fl.modload) {
	    op->fl.modload = false ;
	    rs1 = modload_close(op->mlp) ;
	    if (rs >= 0) rs = rs1 ;
	}
	return rs ;
}
/* end subroutine (uuname_objloadend) */

static int uuname_loadcalls(UN *op,vecstr *slp) noex {
	modload		*mlp = op->mlp ;
	cint		rsn = SR_NOTFOUND ;
	int		rs = SR_OK ;
	int		rs1 ;
	int		c = 0 ;
	cchar		*sname{} ;
	for (int i = 0 ; (rs1 = slp->get(i,&sname)) >= 0 ; i += 1) {
	    if (cvoid *snp{} ; (rs = modload_getsym(mlp,sname,&snp)) >= 0) {
                uuname_calls   *callp = callsp(op->callp) ;
                c += 1 ;
                switch (i) {
                case sub_open:
                    callp->open = soopen_f(snp) ;
                    break ;
		case sub_count:
		    callp->count = socount_f(snp) ;
		    break ;
		case sub_exists:
		    callp->exists = soexists_f(snp) ;
		    break ;
		case sub_curbegin:
		    callp->curbegin = socurbegin_f(snp) ;
		    break ;
		case sub_curenum:
		    callp->curenum = socurenum_f(snp) ;
		    break ;
		case sub_curend:
		    callp->curend = socurend_f(snp) ;
		    break ;
		case sub_audit:
		    callp->audit = soaudit_f(snp) ;
		    break ;
		case sub_close:
		    callp->close = soclose_f(snp) ;
		    break ;
		} /* end switch */
            } else if (rs == rsn) {
                if (! isrequired(i)) rs = SR_OK ;
            } /* end if (it had the call) */
	    if (rs < 0) break ;
	} /* end for (vecstr_get) */
	if ((rs >= 0) && (rs1 != rsn)) rs = rs1 ;
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (uuname_loadcalls) */

static bool isrequired(int i) noex {
	bool		f = false ;
	switch (i) {
	case sub_open:
	case sub_exists:
	case sub_curbegin:
	case sub_curenum:
	case sub_curend:
	case sub_close:
	    f = true ;
	    break ;
	} /* end switch */
	return f ;
}
/* end subroutine (isrequired) */


