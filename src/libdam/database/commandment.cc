/* commandment SUPPORT */
/* lang=C++20 */

/* COMMANDMENT object-load management */
/* version %I% last-modified %G% */


/* revision history:

	= 2008-03-01, David A­D­ Morano
	This module was originally written.

*/

/* Copyright © 2008 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	commandment

	Description:
	This module implements an interface (a trivial one) that
	allows access to the COMMANDMENT datbase.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/param.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<usystem.h>
#include	<modload.h>
#include	<vecstr.h>
#include	<sncpy.h>
#include	<localmisc.h>

#include	"commandment.h"
#include	"commandments.h"


/* local defines */

#define	COMMANDMENT_DEFENTS	67

#define	COMMANDMENT_MODBNAME	"commandments"
#define	COMMANDMENT_OBJNAME	"commandments"

#ifndef	SYMNAMELEN
#define	SYMNAMELEN	60
#endif


/* local namespaces */

using std::nullptr_t ;			/* type */
using std::min ;			/* subroutine-template */
using std::max ;			/* subroutine-template */
using std::nothrow ;			/* constant */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

template<typename ... Args>
static int commandment_ctor(commandment *op,Args ... args) noex {
	COMMANDMENT	*hop = op ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    cnullptr	np{} ;
	    memclear(hop) ;
	    rs = SR_NOMEM ;
	    if ((op->lop = new(nothrow) modload) != np) {
		rs = SR_OK ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (commandment_ctor) */

static int commandment_dtor(commandment *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	    if (op->lop) {
		delete op->lop ;
		op->lop = nullptr ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (commandment_dtor) */

template<typename ... Args>
static inline int commandment_magic(commandment *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magic == COMMANDMENT_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}
/* end subroutine (commandment_magic) */

static int	commandment_objloadbegin(commandment *,cchar *,cchar *) noex ;
static int	commandment_objloadbeginer(commandment *,cchar *,cchar *) noex ;
static int	commandment_objloadend(commandment *) noex ;
static int	commandment_loadcalls(commandment *,cchar *) noex ;

static bool	isrequired(int) noex ;


/* external variables */


/* local variables */

enum subs {
	sub_open,
	sub_audit,
	sub_count,
	sub_max,
	sub_read,
	sub_get,
	sub_curbegin,
	sub_curend,
	sub_curenum,
	sub_close,
	sub_overlast
} ;

constexpr cpcchar	subs[] = {
	"open",
	"audit",
	"count",
	"max",
	"read",
	"get",
	"curbegin",
	"curend",
	"curenum",
	"close",
	nullptr
} ;


/* exported variables */


/* exported subroutines */

int commandment_open(commandment *op,cchar *pr,cchar *dbname) noex {
	int		rs ;
	if ((rs = commandment_ctor(op,pr)) >= 0) {
	    rs = SR_INVALID ;
	    if (pr[0]) {
	        cchar	*objname = COMMANDMENT_OBJNAME ;
	        if ((rs = commandment_objloadbegin(op,pr,objname)) >= 0) {
	            if ((rs = (*op->call.open)(op->obj,pr,dbname)) >= 0) {
		        op->magic = COMMANDMENT_MAGIC ;
	            }
	            if (rs < 0) {
		        commandment_objloadend(op) ;
	            }
	        } /* end if */
	    } /* end if (valid) */
	    if (rs < 0) {
		commandment_dtor(op) ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (commandment_open) */

int commandment_close(commandment *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = commandment_magic(op)) >= 0) {
	    {
	        rs1 = (*op->call.close)(op->obj) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = commandment_objloadend(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    op->magic = 0 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (commandment_close) */

int commandment_audit(commandment *op) noex {
	int		rs ;
	if ((rs = commandment_magic(op)) >= 0) {
	    rs = SR_NOSYS ;
	    if (op->call.audit) {
	        rs = (*op->call.audit)(op->obj) ;
	    }
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (commandment_audit) */

int commandment_count(commandment *op) noex {
	int		rs ;
	if ((rs = commandment_magic(op)) >= 0) {
	    rs = SR_NOSYS ;
	    if (op->call.count) {
	        rs = (*op->call.count)(op->obj) ;
	    }
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (commandment_count) */

int commandment_max(commandment *op) noex {
	int		rs ;
	if ((rs = commandment_magic(op)) >= 0) {
	    rs = SR_NOSYS ;
	    if (op->call.nmax) {
	        rs = (*op->call.nmax)(op->obj) ;
	    }
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (commandment_max) */

int commandment_read(commandment *op,char *rbuf,int rlen,uint cn) noex {
	int		rs ;
	if ((rs = commandment_magic(op,rbuf)) >= 0) {
	    rs = (*op->call.read)(op->obj,rbuf,rlen,cn) ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (commandment_read) */

int commandment_get(commandment *op,int i,char *rbuf,int rlen) noex {
	int		rs ;
	if ((rs = commandment_magic(op)) >= 0) {
	    rs = SR_INVALID ;
	    if (i >= 0) {
	        uint	cn = (uint) i ;
	        rs = commandment_read(op,rbuf,rlen,cn) ;
	    }
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (commandment_get) */

int commandment_curbegin(commandment *op,commandment_cur *curp) noex {
	int		rs ;
	if ((rs = commandment_magic(op,curp)) >= 0) {
	    if (op->call.curbegin) {
	        void	*p ;
	        if ((rs = uc_malloc(op->cursize,&p)) >= 0) {
		    curp->scp = p ;
		    if ((rs = (*op->call.curbegin)(op->obj,curp->scp)) >= 0) {
		         curp->magic = COMMANDMENT_MAGIC ;
		    }
	            if (rs < 0) {
		        uc_free(curp->scp) ;
		        curp->scp = nullptr ;
	            }
	        }
	    } else {
	        rs = SR_NOTSOCK ;
	    }
	    if (rs < 0) {
	        memclear(curp) ;
	    }
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (commandment_curbegin) */

int commandment_curend(commandment *op,commandment_cur *curp) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = commandment_magic(op,curp)) >= 0) {
	    rs = SR_NOTOPEN ;
	    if (curp->magic == COMMANDMENT_MAGIC) {
	        if (curp->scp) {
	            if (op->call.curend) {
	                rs1 = (*op->call.curend)(op->obj,curp->scp) ;
		        if (rs >= 0) rs = rs1 ;
	            }
	            rs1 = uc_free(curp->scp) ;
	            if (rs >= 0) rs = rs1 ;
	            curp->scp = nullptr ;
	        } else {
	            rs = SR_NOTSUP ;
	        }
	        curp->magic = 0 ;
	    } /* end if (valid) */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (commandment_curend) */

int commandment_curenum(commandment *op,commandment_cur *curp,uint *cnp,
		char *rbuf,int rlen) noex {
	int		rs ;
	if ((rs = commandment_magic(op,curp,rbuf)) >= 0) {
	    rs = SR_NOSYS ;
	    if (op->call.enumerate) {
	        COMMANDMENTS_ENT	cse ;
	        rs = (*op->call.enumerate)(op->obj,curp->scp,&cse,rbuf,rlen) ;
	        if (cnp != nullptr) *cnp = cse.cn ;
	    }
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (commandment_enum) */

#ifdef	COMMENT

int commandment_search(commandment *op,cc *s,cmpfunc,cchar **rpp) noex {
	int		rs ;
	int		i ;
	char		**rpp2 ;

	if (op == nullptr) return SR_FAULT ;

	if (op->va == nullptr) return SR_NOTOPEN ;

	if (cmpfunc == nullptr)
	    cmpfunc = defaultcmp ;

	if (op->f.osorted && (! op->f.issorted)) {

	    op->f.issorted = true ;
	    if (op->c > 1)
	        qsort(op->va,(size_t) op->i,
	            sizeof(char *),cmpfunc) ;

	} /* end if (sorting) */

	if (op->f.issorted) {

	    rpp2 = (char **) bsearch(&s,op->va,op->i,
	        sizeof(char *),cmpfunc) ;

	    rs = SR_NOTFOUND ;
	    if (rpp2 != nullptr) {

	        i = rpp2 - op->va ;
	        rs = SR_OK ;

	    }

	} else {

	    for (i = 0 ; i < op->i ; i += 1) {

	        rpp2 = op->va + i ;
	        if (*rpp2 == nullptr) continue ;

	        if ((*cmpfunc)(&s,rpp2) == 0)
	            break ;

	    } /* end for */

	    rs = (i < op->i) ? SR_OK : SR_NOTFOUND ;

	} /* end if (sorted or not) */

	if (rpp != nullptr)
	    *rpp = (rs >= 0) ? op->va[i] : nullptr ;

	return (rs >= 0) ? i : rs ;
}
/* end subroutine (commandment_search) */

#endif /* COMMENT */


/* private subroutines */

static int commandment_objloadbegin(commandment *op,cc *pr,cc *objname) noex {
	int		rs ;
	if ((rs = commandment_objloadbeginer(op,pr,objname)) >= 0) {
	    modload	*lp = op->lop ;
	    int		mv[2] ;
	    if ((rs = modload_getmva(lp,mv,2)) >= 0) {
		cint	objsize = rs ;
		void	*p ;
		op->objsize = mv[0] ;
		op->cursize = mv[1] ;
		if ((rs = uc_malloc(objsize,&p)) >= 0) {
		    op->obj = p ;
		    rs = commandment_loadcalls(op,objname) ;
		    if (rs < 0) {
			uc_free(op->obj) ;
			op->obj = nullptr ;
		    }
		} /* end if (memory-allocation) */
	    } /* end if (getmva) */
	    if (rs < 0) {
		modload_close(lp) ;
	    }
	} /* end if (ok) */
	return rs ;
}
/* end subroutine (commandment_objloadbegin) */

static int commandment_objloadbeginer(commandment *op,cc *pr,cc *objname) noex {
	modload		*lp = op->lop ;
	vecstr		syms ;
	cint		n = nelem(subs) ;
	cint		vo = VECSTR_OCOMPACT ;
	int		rs ;
	int		rs1 ;

	if ((rs = vecstr_start(&syms,n,vo)) >= 0) {
	    cint	nlen = SYMNAMELEN ;
	    int		i ;
	    int		f_modload = false ;
	    char	nbuf[SYMNAMELEN + 1] ;

	    for (i = 0 ; (i < n) && (subs[i] != nullptr) ; i += 1) {
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
	            cint	mo = (MODLOAD_OLIBVAR | MODLOAD_OSDIRS) ;
	            cchar	*modbname = COMMANDMENT_MODBNAME ;
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
/* end subroutine (commandment_objloadbeginer) */

static int commandment_objloadend(commandment *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (op->obj) {
	    rs1 = uc_free(op->obj) ;
	    if (rs >= 0) rs = rs1 ;
	    op->obj = nullptr ;
	}
	{
	    rs1 = modload_close(op->lop) ;
	    if (rs >= 0) rs = rs1 ;
	}
	return rs ;
}
/* end subroutine (commandment_objloadend) */

static int commandment_loadcalls(commandment *op,cchar *objname) noex {
	modload		*lp = op->lop ;
	cint		nlen = SYMNAMELEN ;
	int		rs = SR_OK ;
	int		i ;
	int		c = 0 ;
	char		nbuf[SYMNAMELEN + 1] ;
	cvoid		*snp = nullptr ;

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
		case sub_audit:
		    op->call.audit = (int (*)(void *)) snp ;
		    break ;
		case sub_count:
		    op->call.count = (int (*)(void *)) snp ;
		    break ;
		case sub_max:
		    op->call.nmax = (int (*)(void *)) snp ;
		    break ;
		case sub_read:
		    op->call.read = (int (*)(void *,char *,int,uint)) snp ;
		    break ;
		case sub_get:
		    op->call.get = (int (*)(void *,int,char *,int)) snp ;
		    break ;
		case sub_curbegin:
		    op->call.curbegin = (int (*)(void *,void *)) snp ;
		    break ;
		case sub_curend:
		    op->call.curend = (int (*)(void *,void *)) snp ;
		    break ;
		case sub_curenum:
		    op->call.enumerate = 
			(int (*)(void *,void *,void *,char *,int)) snp ;
		    break ;
		case sub_close:
		    op->call.close = (int (*)(void *)) snp ;
		    break ;
		} /* end switch */
	    } /* end if (it had the call) */

	} /* end for (subs) */

	return (rs >= 0) ? c : rs ;
}
/* end subroutine (commandment_loadcalls) */

static bool isrequired(int i) noex {
	bool		f = false ;
	switch (i) {
	case sub_open:
	case sub_audit:
	case sub_count:
	case sub_read:
	case sub_curbegin:
	case sub_curend:
	case sub_curenum:
	case sub_close:
	    f = true ;
	    break ;
	} /* end switch */
	return f ;
}
/* end subroutine (isrequired) */


