/* uuname SUPPORT */
/* lang=C++20 */

/* caller interface to a UUNAME database */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-01, David A­D­ Morano
	This module was originally written.

*/

/* Copyright � 1998 David A-D- Morano.  All rights reserved. */

/*******************************************************************************

	This module implements an interface (a trivial one) that
	provides access to the UUNAME object (which is dynamically
	loaded).

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstring>
#include	<usystem.h>
#include	<modload.h>
#include	<vecstr.h>
#include	<sncpyx.h>
#include	<localmisc.h>

#include	"uuname.h"
#include	"uunames.h"


/* local defines */

#define	UUNAME_MODBNAME	"uunames"
#define	UUNAME_OBJNAME	"uunames"

#define	LIBCNAME	"lib"

#ifndef	SYMNAMELEN
#define	SYMNAMELEN	60
#endif


/* imported namespaces */

using std::nullptr_t ;			/* type */
using std::nothrow ;			/* constant */


/* local typedefs */


/* external subroutines */


/* local structures */


/* forward references */

template<typename ... Args>
static int uuname_ctor(uuname *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    cnullptr	np{} ;
	    rs = SR_NOMEM ;
	    memclear(op) ;
	    if ((op->loader = new(nothrow) modload) != np) {
		rs = SR_OK ;
	    } /* end if (new-modload) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (uuname_ctor) */

static int uuname_dtor(uuname *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	    if (op->loader) {
		delete op->loader ;
		op->loader = nullptr ;
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

static int	uuname_objloadbegin(uuname *,cchar *,cchar *) noex ;
static int	uuname_objloadbeginer(uuname *,cchar *,cchar *) noex ;
static int	uuname_objloadend(uuname *) noex ;
static int	uuname_loadcalls(uuname *,cchar *) noex ;

static bool	isrequired(int) noex ;


/* global variables */


/* local variables */

enum subs {
	sub_open,
	sub_count,
	sub_exists,
	sub_curbegin,
	sub_enum,
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
	"enum",
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
	            if ((rs = (*op->call.open)(op->obj,pr,dbname)) >= 0) {
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
	        rs1 = (*op->call.close)(op->obj) ;
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
	    if (op->call.audit != nullptr) {
	        rs = (*op->call.audit)(op->obj) ;
	    }
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (uuname_audit) */

int uuname_count(uuname *op) noex {
	int		rs ;
	if ((rs = uuname_magic(op)) >= 0) {
	    if (op->call.count != nullptr) {
	        rs = (*op->call.count)(op->obj) ;
	    }
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (uuname_count) */

int uuname_curbegin(uuname *op,UUNAME_CUR *curp) noex {
	int		rs = SR_NOSYS ;
	void		*p ;

	if (op == nullptr) return SR_FAULT ;
	if (curp == nullptr) return SR_FAULT ;

	if (op->magic != UUNAME_MAGIC) return SR_NOTOPEN ;

	memclear(curp) ;

	if (op->call.curbegin != nullptr) {
	    if ((rs = uc_malloc(op->cursize,&p)) >= 0) {
	        curp->scp = p ;
	        if ((rs = (*op->call.curbegin)(op->obj,curp->scp)) >= 0) {
	            curp->magic = UUNAME_MAGIC ;
	        }
	        if (rs < 0) {
		    uc_free(curp->scp) ;
		    curp->scp = nullptr ;
	        }
	    } /* end if (m-a) */
	} /* end if (have SO method) */

	return rs ;
}
/* end subroutine (uuname_curbegin) */

int uuname_curend(uuname *op,UUNAME_CUR *curp) noex {
	int		rs = SR_NOSYS ;
	int		rs1 ;

	if (op == nullptr) return SR_FAULT ;
	if (curp == nullptr) return SR_FAULT ;

	if (op->magic != UUNAME_MAGIC) return SR_NOTOPEN ;
	if (curp->magic != UUNAME_MAGIC) return SR_NOTOPEN ;

	if (curp->scp == nullptr) return SR_BADFMT ;

	if (op->call.curend != nullptr) {
	    rs1 = (*op->call.curend)(op->obj,curp->scp) ;
	    if (rs >= 0) rs = rs1 ;
	}

	rs1 = uc_free(curp->scp) ;
	if (rs >= 0) rs = rs1 ;
	curp->scp = nullptr ;

	curp->magic = 0 ;
	return rs ;
}
/* end subroutine (uuname_curend) */

int uuname_exists(uuname *op,cchar *sp,int sl) noex {
	int		rs ;

	if (op == nullptr) return SR_FAULT ;

	if (op->magic != UUNAME_MAGIC) return SR_NOTOPEN ;

	rs = (*op->call.exists)(op->obj,sp,sl) ;

	return rs ;
}
/* end subroutine (uuname_exists) */

int uuname_enum(uuname *op,UUNAME_CUR *curp,char *rbuf,int rlen) noex {
	int		rs = SR_NOSYS ;

	if (op == nullptr) return SR_FAULT ;
	if (curp == nullptr) return SR_FAULT ;
	if (rbuf == nullptr) return SR_FAULT ;

	if (op->magic != UUNAME_MAGIC) return SR_NOTOPEN ;
	if (curp->magic != UUNAME_MAGIC) return SR_NOTOPEN ;

	if (op->call.enumerate != nullptr) {
	    rs = (*op->call.enumerate)(op->obj,curp->scp,rbuf,rlen) ;
	}

	return rs ;
}
/* end subroutine (uuname_enum) */


/* private subroutines */

static int uuname_objloadbegin(uuname *op,cchar *pr,cchar *objname) noex {
	int		rs ;

	if ((rs = uuname_objloadbeginer(op,pr,objname)) >= 0) {
	    modload	*lp = op->loader ;
	    if ((rs = modload_getmv(lp,0)) >= 0) {
		int	objsize = rs ;
		void	*p ;
		if ((rs = uc_malloc(objsize,&p)) >= 0) {
		    op->obj = p ;
		    rs = uuname_loadcalls(op,objname) ;
		    if (rs < 0) {
			uc_free(op->obj) ;
			op->obj = nullptr ;
		    }
		} /* end if (memory-allocation) */
	    } /* end if (getmv) */
	    if (rs < 0)
		modload_close(lp) ;
	} /* end if (ok) */

	return rs ;
}
/* end subroutine (uuname_objloadbegin) */

static int uuname_objloadbeginer(uuname *op,cchar *pr,cchar *objname) noex {
	modload		*lp = op->loader ;
	vecstr		syms ;
	cint		ne = nelem(subs) ;
	cint		vo = VECSTR_OCOMPACT ;
	int		rs ;
	int		rs1 ;

	if ((rs = vecstr_start(&syms,ne,vo)) >= 0) {
	    cint	nlen = SYMNAMELEN ;
	    int		f_modload = false ;
	    char	nbuf[SYMNAMELEN + 1] ;

	    for (int i = 0 ; (i < ne) && subs[i] ; i += 1) {
	        if (isrequired(i)) {
	            if ((rs = sncpy3(nbuf,nlen,objname,"_",subs[i])) >= 0) {
			rs = vecstr_add(&syms,nbuf,rs) ;
		    }
		}
		if (rs < 0) break ;
	    } /* end for */

	    if (rs >= 0) {
		mainv	sv ;
	        if ((rs = vecstr_getvec(&syms,&sv)) >= 0) {
	            cchar	*modbname = UUNAME_MODBNAME ;
	            cint	mo = (MODLOAD_OLIBVAR | MODLOAD_OSDIRS) ;
	            rs = modload_open(lp,pr,modbname,objname,mo,sv) ;
		    f_modload = (rs >= 0)  ;
		}
	    }

	    rs1 = vecstr_finish(&syms) ;
	    if (rs >= 0) rs = rs1 ;
	    if ((rs < 0) && f_modload) {
		modload_close(lp) ;
	    }
	} /* end if (allocation) */

	return rs ;
}
/* end subroutine (uuname_objloadbeginer) */

static int uuname_objloadend(uuname *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;

	if (op->obj) {
	    rs1 = uc_free(op->obj) ;
	    if (rs >= 0) rs = rs1 ;
	    op->obj = nullptr ;
	}

	rs1 = modload_close(op->loader) ;
	if (rs >= 0) rs = rs1 ;

	return rs ;
}
/* end subroutine (uuname_objloadend) */

static int uuname_loadcalls(uuname *op,cchar *objname) noex {
	modload		*lp = op->loader ;
	cint		nlen = SYMNAMELEN ;
	int		rs = SR_OK ;
	int		i ;
	int		c = 0 ;
	char		nbuf[SYMNAMELEN + 1] ;
	cvoid	*snp = nullptr ;

	for (i = 0 ; subs[i] != nullptr ; i += 1) {

	    if ((rs = sncpy3(nbuf,nlen,objname,"_",subs[i])) >= 0) {
	         if ((rs = modload_getsym(lp,nbuf,&snp)) == SR_NOTFOUND) {
		     snp = nullptr ;
		     if (! isrequired(i)) rs = SR_OK ;
		}
	    }

	    if (rs < 0) break ;

	    if (snp != nullptr) {
	        c += 1 ;
		switch (i) {
		case sub_open:
		    op->call.open = 
			(int (*)(void *,cchar *,cchar *)) snp ;
		    break ;
		case sub_count:
		    op->call.count = (int (*)(void *)) snp ;
		    break ;
		case sub_exists:
		    op->call.exists = 
			(int (*)(void *,cchar *,int)) snp ;
		    break ;
		case sub_curbegin:
		    op->call.curbegin = 
			(int (*)(void *,void *)) snp ;
		    break ;
		case sub_enum:
		    op->call.enumerate = 
			(int (*)(void *,void *,char *,int)) snp ;
		    break ;
		case sub_curend:
		    op->call.curend = 
			(int (*)(void *,void *)) snp ;
		    break ;
		case sub_audit:
		    op->call.audit = (int (*)(void *)) snp ;
		    break ;
		case sub_close:
		    op->call.close = (int (*)(void *)) snp ;
		    break ;
		} /* end switch */
	    } /* end if (it had the call) */

	} /* end for (subs) */

	return (rs >= 0) ? c : rs ;
}
/* end subroutine (uuname_loadcalls) */

static bool isrequired(int i) noex {
	bool		f = false ;
	switch (i) {
	case sub_open:
	case sub_exists:
	case sub_curbegin:
	case sub_enum:
	case sub_curend:
	case sub_close:
	    f = true ;
	    break ;
	} /* end switch */
	return f ;
}
/* end subroutine (isrequired) */


