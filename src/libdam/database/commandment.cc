/* commandment SUPPORT */
/* charset=ISO8859-1 */
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
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<new>			/* |nothrow(3c++)| */
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<usystem.h>
#include	<modload.h>
#include	<vecstr.h>
#include	<sncpy.h>
#include	<localmisc.h>

#include	"commandment.h"
#include	"commandments.h"


/* local defines */

#define	CMD_MODBNAME	"commandments"
#define	CMD_OBJNAME	"commandments"
#define	CMD		commandment
#define	CMD_CUR		commandment_cur


/* local namespaces */

using std::nullptr_t ;			/* type */
using std::min ;			/* subroutine-template */
using std::max ;			/* subroutine-template */
using std::nothrow ;			/* constant */


/* local typedefs */

extern "C" {
    typedef int	(*soopen_f)(void *,cchar *,cchar *) noex ;
    typedef int	(*soaudit_f)(void *) noex ;
    typedef int	(*socount_f)(void *) noex ;
    typedef int	(*sonummax_f)(void *) noex ;
    typedef int	(*soread_f)(void *,char *,int,uint) noex ;
    typedef int	(*soget_f)(void *,int,char *,int) noex ;
    typedef int	(*socurbegin_f)(void *,void *) noex ;
    typedef int	(*socurend_f)(void *,void *) noex ;
    typedef int	(*socurenum_f)(void *,void *,void *,char *,int) noex ;
    typedef int	(*soclose_f)(void *) noex ;
}


/* external subroutines */


/* external variables */


/* local structures */

struct commandment_calls {
    soopen_f		open ;
    soaudit_f		audit ;
    socount_f		count ;
    sonummax_f		nummax ;
    soread_f		read ;
    soget_f		get ;
    socurbegin_f	curbegin ;
    socurend_f		curend ;
    socurenum_f		curenum ;
    soclose_f		close ;
} ; /* end struct (commandment) */

typedef commandment_calls *	callsp ;


/* forward references */

template<typename ... Args>
static int commandment_ctor(CMD *op,Args ... args) noex {
	COMMANDMENT	*hop = op ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    cnullptr	np{} ;
	    memclear(hop) ;
	    rs = SR_NOMEM ;
	    if ((op->mlp = new(nothrow) modload) != np) {
		commandment_calls    *callp ;
                if ((callp = new(nothrow) commandment_calls) != np) {
                    op->callp = callp ;
                    rs = SR_OK ;
                } /* end if (new-commandment_calls) */
                if (rs < 0) {
                    delete op->mlp ;
                    op->mlp = nullptr ;
                }
	    } /* end if (new-modload) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (commandment_ctor) */

static int commandment_dtor(CMD *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
            if (op->callp) {
                commandment_calls    *callp = callsp(op->callp) ;
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
/* end subroutine (commandment_dtor) */

template<typename ... Args>
static inline int commandment_magic(CMD *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magic == COMMANDMENT_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}
/* end subroutine (commandment_magic) */

static int	commandment_objloadbegin(CMD *,cchar *,cchar *) noex ;
static int	commandment_objloadend(CMD *) noex ;
static int	commandment_loadcalls(CMD *,vecstr *) noex ;

static bool	isrequired(int) noex ;


/* external variables */


/* local variables */

enum subs {
	sub_open,
	sub_audit,
	sub_count,
	sub_nummax,
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

int commandment_open(CMD *op,cchar *pr,cchar *dbname) noex {
	int		rs ;
	if ((rs = commandment_ctor(op,pr)) >= 0) {
	    rs = SR_INVALID ;
	    if (pr[0]) {
	        cchar	*objn = CMD_OBJNAME ;
	        if ((rs = commandment_objloadbegin(op,pr,objn)) >= 0) {
		    commandment_calls	*callp = callsp(op->callp) ;
		    rs = SR_NOSYS ;
		    if (callp->open) {
			auto 	co = callp->open ;
	                if ((rs = co(op->obj,pr,dbname)) >= 0) {
		            op->magic = COMMANDMENT_MAGIC ;
	                }
		    } /* end if (open) */
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

int commandment_close(CMD *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = commandment_magic(op)) >= 0) {
	    commandment_calls	*callp = callsp(op->callp) ;
	    if (callp->close) {
		auto 	co = callp->close ;
	        rs1 = co(op->obj) ;
	        if (rs >= 0) rs = rs1 ;
	    } else {
		rs = SR_NOSYS ;
	    }
	    {
	        rs1 = commandment_objloadend(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
		rs1 = commandment_dtor(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    op->magic = 0 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (commandment_close) */

int commandment_audit(CMD *op) noex {
	int		rs ;
	if ((rs = commandment_magic(op)) >= 0) {
	    commandment_calls	*callp = callsp(op->callp) ;
	    rs = SR_NOSYS ;
	    if (callp->audit) {
		auto 	co = callp->audit ;
	        rs = co(op->obj) ;
	    }
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (commandment_audit) */

int commandment_count(CMD *op) noex {
	int		rs ;
	if ((rs = commandment_magic(op)) >= 0) {
	    commandment_calls	*callp = callsp(op->callp) ;
	    rs = SR_NOSYS ;
	    if (callp->count) {
		auto 	co = callp->count ;
	        rs = co(op->obj) ;
	    }
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (commandment_count) */

int commandment_nummax(CMD *op) noex {
	int		rs ;
	if ((rs = commandment_magic(op)) >= 0) {
	    commandment_calls	*callp = callsp(op->callp) ;
	    rs = SR_NOSYS ;
	    if (callp->nummax) {
		auto 	co = callp->nummax ;
	        rs = co(op->obj) ;
	    }
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (commandment_nummax) */

int commandment_read(CMD *op,char *rbuf,int rlen,uint cn) noex {
	int		rs ;
	if ((rs = commandment_magic(op,rbuf)) >= 0) {
	    commandment_calls	*callp = callsp(op->callp) ;
	    rs = SR_NOSYS ;
	    if (callp->read) {
		auto 	co = callp->read ;
    		rs = co(op->obj,rbuf,rlen,cn) ;
	    }
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (commandment_read) */

int commandment_get(CMD *op,int i,char *rbuf,int rlen) noex {
	int		rs ;
	if ((rs = commandment_magic(op,rbuf)) >= 0) {
	    rs = SR_INVALID ;
	    if (i >= 0) {
	        uint	cn = uint(i) ;
	        rs = commandment_read(op,rbuf,rlen,cn) ;
	    }
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (commandment_get) */

int commandment_curbegin(CMD *op,CMD_CUR *curp) noex {
	int		rs ;
	if ((rs = commandment_magic(op,curp)) >= 0) {
	    commandment_calls	*callp = callsp(op->callp) ;
	    memclear(curp) ;
	    if (callp->curbegin) {
		cint	csz = op->cursz ;
	        if (void *vp ; (rs = uc_malloc(csz,&vp)) >= 0) {
		    curp->scp = vp ;
		    auto 	co = callp->curbegin ;
		    if ((rs = co(op->obj,curp->scp)) >= 0) {
		         curp->magic = COMMANDMENT_MAGIC ;
		    }
	            if (rs < 0) {
		        uc_free(curp->scp) ;
		        curp->scp = nullptr ;
	            }
	        } /* end if (memory-allocation) */
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

int commandment_curend(CMD *op,CMD_CUR *curp) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = commandment_magic(op,curp)) >= 0) {
	    commandment_calls	*callp = callsp(op->callp) ;
	    rs = SR_NOTOPEN ;
	    if ((curp->magic == COMMANDMENT_MAGIC) && curp->scp) {
	        if (callp->curend) {
		    auto 	co = callp->curend ;
	            rs1 = co(op->obj,curp->scp) ;
		    if (rs >= 0) rs = rs1 ;
	        }
		{
	            rs1 = uc_free(curp->scp) ;
	            if (rs >= 0) rs = rs1 ;
	            curp->scp = nullptr ;
		}
	        curp->magic = 0 ;
	    } /* end if (valid) */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (commandment_curend) */

int commandment_curenum(CMD *op,CMD_CUR *curp,uint *cnp,
		char *rbuf,int rlen) noex {
	int		rs ;
	if ((rs = commandment_magic(op,curp,rbuf)) >= 0) {
	    commandment_calls	*callp = callsp(op->callp) ;
	    rs = SR_NOSYS ;
	    if (callp->curenum) {
	        COMMANDMENTS_ENT	cse{} ;
		auto 	co = callp->curenum ;
	        rs = co(op->obj,curp->scp,&cse,rbuf,rlen) ;
	        if (cnp) *cnp = cse.cn ;
	    }
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (commandment_enum) */

#ifdef	COMMENT

int commandment_search(CMD *op,cc *s,cmpfunc,cchar **rpp) noex {
	int		rs ;
	int		i = 0 ;
	if (cmpfunc == nullptr) cmpfunc = defaultcmp ;
	if ((rs = commandment_magic(op,s,rpp)) >= 0) {
	    char	**rpp2 ;
	    if (op->fl.osorted && (! op->fl.issorted)) {
	        op->fl.issorted = true ;
	        if (op->c > 1) {
		    csize	qn = size_t(op->i) ;
		    csize	qsz = szof(char *) ;
	            qsort(op->va,qn,qsz,cmpfunc) ;
	    } /* end if (sorting) */
	    if (op->fl.issorted) {
	        csize	bn = size_t(op->i) ;
	        csize	bsz = szof(char *) ;
	        rpp2 = (char **) bsearch(&s,op->va,bn,bsz,cmpfunc) ;
	        rs = SR_NOTFOUND ;
	        if (rpp2) {
	            i = rpp2 - op->va ;
	            rs = SR_OK ;
	        }
	    } else {
	        for (i = 0 ; i < op->i ; i += 1) {
	            rpp2 = op->va + i ;
	            if (*rpp2 == nullptr) continue ;
	            if ((*cmpfunc)(&s,rpp2) == 0) break ;
	        } /* end for */
	        rs = (i < op->i) ? SR_OK : SR_NOTFOUND ;
	    } /* end if (sorted or not) */
	    if (rpp) {
	        *rpp = (rs >= 0) ? op->va[i] : nullptr ;
	    }
	} /* end if (magic) */
	return (rs >= 0) ? i : rs ;
}
/* end subroutine (commandment_search) */

#endif /* COMMENT */


/* private subroutines */

static int commandment_objloadbegin(CMD *op,cchar *pr,cchar *objn) noex {
	modload		*lp = op->mlp ;
	cint		vn = sub_overlast ;
	cint		vo = VECSTR_OCOMPACT ;
	int		rs ;
	int		rs1 ;
	if (vecstr syms ; (rs = syms.start(vn,vo)) >= 0) {
	    if ((rs = syms.addsyms(objn,subs)) >= 0) {
	        if (mainv sv ; (rs = syms.getvec(&sv)) >= 0) {
	            cchar	*mn = CMD_MODBNAME ;
	            cchar	*on = objn ;
	            int		mo = 0 ;
	            mo |= modloadm.libvar ;
	            mo |= modloadm.libprs ;
	            mo |= modloadm.libsdirs ;
	            mo |= modloadm.avail ;
	            if ((rs = modload_open(lp,pr,mn,on,mo,sv)) >= 0) {
		        op->fl.modload = true ;
	                if (int mv[2] ; (rs = modload_getmva(lp,mv,2)) >= 0) {
			    cint	osz = mv[0] ;
	                    op->objsz = mv[0] ;
	                    op->cursz = mv[1] ;
			    if (void *vp ; (rs = uc_malloc(osz,&vp)) >= 0) {
	                        op->obj = vp ;
	                        rs = commandment_loadcalls(op,&syms) ;
	                        if (rs < 0) {
	                            uc_free(op->obj) ;
	                            op->obj = nullptr ;
	                        }
	                    } /* end if (memory-allocation) */
	                } /* end if (modload_getmva) */
	                if (rs < 0) {
		            op->fl.modload = false ;
	                    modload_close(lp) ;
	                }
	            } /* end if (modload_open) */
		} /* end if (vecstr_getvec) */
	    } /* end if (vecstr_addsyms) */
	    rs1 = syms.finish ;
	    if (rs >= 0) rs = rs1 ;
	    if ((rs < 0) && op->fl.modload) {
		op->fl.modload = false ;
		modload_close(lp) ;
	    }
	} /* end if (vecstr-syms) */
	return rs ;
}
/* end subroutine (commandment_objloadbegin) */

static int commandment_objloadend(CMD *op) noex {
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
/* end subroutine (commandment_objloadend) */

static int commandment_loadcalls(CMD *op,vecstr *slp) noex {
	modload		*mlp = op->mlp ;
	cint		rsn = SR_NOTFOUND ;
	int		rs = SR_OK ;
	int		rs1 ;
	int		c = 0 ;
	cchar		*sname{} ;
	for (int i = 0 ; (rs1 = slp->get(i,&sname)) >= 0 ; i += 1) {
	    if (cvoid *snp{} ; (rs = modload_getsym(mlp,sname,&snp)) >= 0) {
                commandment_calls   *callp = callsp(op->callp) ;
                c += 1 ;
                switch (i) {
                case sub_open:
                    callp->open = soopen_f(snp) ;
                    break ;
                case sub_count:
                    callp->count = socount_f(snp) ;
                    break ;
                case sub_get:
                    callp->get = soget_f(snp) ;
                    break ;
                case sub_read:
                    callp->read = soread_f(snp) ;
                    break ;
                case sub_nummax:
                    callp->nummax = sonummax_f(snp) ;
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


