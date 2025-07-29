/* txtindex_main SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* interface to the TXTINDEXES loadable object */
/* version %I% last-modified %G% */


/* revision history:

	- 2008-10-01, David A­D­ Morano
	This module was originally written.

*/

/* Copyright © 2008 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	txtindex

	Description:
	This module implements an interface (a trivial one) that
	provides access to the TXTINDEX object (which is dynamically
	loaded).

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<usystem.h>
#include	<vecstr.h>
#include	<sncpyx.h>
#include	<localmisc.h>

#include	"txtindex.h"
#include	"txtindexes.h"

import libutil ;

/* local defines */

#define	TXTINDEX_DEFENTS	(44 * 1000)
#define	TXTINDEX_MODBNAME	"txtindexes"
#define	TXTINDEX_OBJNAME	"txtindexes"

#define	TI_CUR			txtindex_cur
#define	TI_TAG			txtindex_tag

#undef	TIS_CUR
#define	TIS_CUR			TXTINDEXES_CUR
#undef	TIS_TAG
#define	TIS_TAG			TXTINDEXES_TAG


/* imported namespaces */

using std::nullptr_t ;			/* type */
using std::min ;			/* subroutine-template */
using std::max ;			/* subroutine-template */
using std::nothrow ;			/* constant */


/* local typedefs */

extern "C" {
    typedef int	(*soopen_f)(void *,cchar *) noex ;
    typedef int	(*socount_f)(void *) noex ;
    typedef int	(*soneigen_f)(void *) noex ;
    typedef int	(*sogetinfo_f)(void *,txtindexes_info *) noex ;
    typedef int	(*sogetsdn_f)(void *,char *,int) noex ;
    typedef int	(*sogetsfn_f)(void *,char *,int) noex ;
    typedef int	(*soiseigen_f)(void *,cchar *,int) noex ;
    typedef int	(*socurbegin_f)(void *,void *) noex ;
    typedef int	(*socurlook_f)(void *,void *,mainv) noex ;
    typedef int	(*socurenum_f)(void *,void *,txtindexes_tag *,char *,int) noex ;
    typedef int	(*socurend_f)(void *,void *) noex ;
    typedef int	(*soaudit_f)(void *) noex ;
    typedef int	(*soclose_f)(void *) noex ;
} /* end extern (C) */


/* external subroutines */


/* external variables */


/* local structures */

struct txtindex_calls {
	soopen_f	open ;
	socount_f	count ;
	soneigen_f	neigen ;
	sogetinfo_f	getinfo ;
	sogetsdn_f	getsdn ;
	sogetsfn_f	getsfn ;
	soiseigen_f	iseigen ;
	socurbegin_f	curbegin ;
	socurlook_f	curlook ;
	socurenum_f	curenum ;
	socurend_f	curend ;
	soaudit_f	audit ;
	soclose_f	close ;
} ; /* end struct (txtindex_calls) */

typedef txtindex_calls *	callsp ;


/* forward references */

template<typename ... Args>
static int txtindex_ctor(txtindex *op,Args ... args) noex {
	TXTINDEX	*hop = op ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    cnullptr	np{} ;
	    rs = SR_NOMEM ;
	    memclear(hop) ;
	    if ((op->mlp = new(nothrow) modload) != np) {
		txtindex_calls	*callp ;
	        if ((callp = new(nothrow) txtindex_calls) != np) {
		    op->callp = callp ;
		    rs = SR_OK ;
		}
		if (rs < 0) {
		    delete op->mlp ;
		    op->mlp = nullptr ;
		}
	    } /* end new (new-modload) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (txtindex_ctor) */

static int txtindex_dtor(txtindex *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	    if (op->callp) {
		txtindex_calls	*callp = callsp(op->callp) ;
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
/* end subroutine (txtindex_dtor) */

template<typename ... Args>
static inline int txtindex_magic(txtindex *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magic == TXTINDEX_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}
/* end subroutine (txtindex_magic) */

static int	txtindex_objloadbegin(txtindex *,cchar *,cchar *) noex ;
static int	txtindex_objloadend(txtindex *) noex ;
static int	txtindex_loadcalls(txtindex *,vecstr *) noex ;

static bool	isrequired(int) noex ;


/* local variables */

enum subs {
	sub_open,
	sub_count,
	sub_neigen,
	sub_getinfo,
	sub_getsdn,
	sub_getsfn,
	sub_iseigen,
	sub_curbegin,
	sub_curlook,
	sub_curenum,
	sub_curend,
	sub_audit,
	sub_close,
	sub_overlast
} ;

constexpr cpcchar	subs[] = {
	"open",
	"count",
	"neigen",
	"getinfo",
	"getsdn",
	"getsfn",
	"iseigen",
	"curbegin",
	"curlook",
	"curenum",
	"curend",
	"audit",
	"close",
	nullptr
} ;


/* exported variables */


/* exported subroutines */

int txtindex_open(txtindex *op,cchar *pr,cchar *dbname) noex {
	int		rs ;
	if ((rs = txtindex_ctor(op,pr,dbname)) >= 0) {
	    rs = SR_INVALID ;
	    if (dbname[0]) {
		txtindex_calls	*callp = callsp(op->callp) ;
	        cchar		*objn = TXTINDEX_OBJNAME ;
	        if ((rs = txtindex_objloadbegin(op,pr,objn)) >= 0) {
		    auto	co = callp->open ;
	            if ((rs = co(op->obj,dbname)) >= 0) {
		        op->magic = TXTINDEX_MAGIC ;
	            }
	            if (rs < 0) {
		        txtindex_objloadend(op) ;
	            }
	        } /* end if (txtindex_objloadbegin) */
	    } /* end if (valid) */
	    if (rs < 0) {
		txtindex_dtor(op) ;
	    }
	} /* end if (txtindex_ctor) */
	return rs ;
}
/* end subroutine (txtindex_open) */

int txtindex_close(txtindex *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = txtindex_magic(op)) >= 0) {
	    txtindex_calls	*callp = callsp(op->callp) ;
	    {
		auto	co = callp->close ;
	        rs1 = co(op->obj) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = txtindex_objloadend(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    op->magic = 0 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (txtindex_close) */

int txtindex_audit(txtindex *op) noex {
	int		rs ;
	if ((rs = txtindex_magic(op)) >= 0) {
	    txtindex_calls	*callp = callsp(op->callp) ;
	    rs = SR_NOSYS ;
	    if (callp->audit) {
		auto	co = callp->audit ;
	        rs = co(op->obj) ;
	    }
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (txtindex_audit) */

int txtindex_count(txtindex *op) noex {
	int		rs ;
	if ((rs = txtindex_magic(op)) >= 0) {
	    txtindex_calls	*callp = callsp(op->callp) ;
	    rs = SR_NOSYS ;
	    if (callp->count) {
		auto	co = callp->count ;
	        rs = co(op->obj) ;
	    }
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (txtindex_count) */

int txtindex_neigen(txtindex *op) noex {
	int		rs ;
	if ((rs = txtindex_magic(op)) >= 0) {
	    txtindex_calls	*callp = callsp(op->callp) ;
	    rs = SR_NOSYS ;
	    if (callp->neigen) {
		auto	co = callp->neigen ;
	        rs = co(op->obj) ;
	    }
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (txtindex_neigen) */

int txtindex_getinfo(txtindex *op,TXTINDEX_INFO *ip) noex {
	int		rs ;
	if ((rs = txtindex_magic(op,ip)) >= 0) {
	    txtindex_calls	*callp = callsp(op->callp) ;
	    rs = SR_NOSYS ;
	    if (auto co = callp->getinfo ; co != nullptr) {
	        rs = co(op->obj,ip) ;
	    }
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (txtindex_getinfo) */

int txtindex_getsdn(txtindex *op,char *rb,int rl) noex {
	int		rs ;
	if ((rs = txtindex_magic(op,rb)) >= 0) {
	    txtindex_calls	*callp = callsp(op->callp) ;
	    rs = SR_NOSYS ;
	    if (auto co = callp->getsdn ; co != nullptr) {
	        rs = co(op->obj,rb,rl) ;
	    }
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (txtindex_getsdn) */

int txtindex_getsfn(txtindex *op,char *rb,int rl) noex {
	int		rs ;
	if ((rs = txtindex_magic(op,rb)) >= 0) {
	    txtindex_calls	*callp = callsp(op->callp) ;
	    rs = SR_NOSYS ;
	    if (auto co = callp->getsfn ; co != nullptr) {
	        rs = co(op->obj,rb,rl) ;
	    }
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (txtindex_getsfn) */

int txtindex_curbegin(txtindex *op,TI_CUR *curp) noex {
	int		rs ;
	if ((rs = txtindex_magic(op,curp)) >= 0) {
	    txtindex_calls	*callp = callsp(op->callp) ;
	    memclear(curp) ;
	    if (callp->curbegin) {
	        if (void *vp{} ; (rs = uc_malloc(op->cursize,&vp)) >= 0) {
		    curp->scp = vp ;
		    rs = SR_BUGCHECK ;
		    if (callp->curbegin) {
		        auto	co = callp->curbegin ;
		        if ((rs = co(op->obj,curp->scp)) >= 0) {
	    	            curp->magic = TXTINDEX_MAGIC ;
		        }
		    }
	            if (rs < 0) {
	    	        uc_free(curp->scp) ;
	    	        curp->scp = nullptr ;
		    }
	        } /* end if (m-a) */
	    } else {
	        rs = SR_NOSYS ;
	    }
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (txtindex_curbegin) */

int txtindex_curend(txtindex *op,TI_CUR *curp) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = txtindex_magic(op,curp)) >= 0) {
	    txtindex_calls	*callp = callsp(op->callp) ;
	    rs = SR_NOTOPEN ;
	    if (curp->magic == TXTINDEX_MAGIC) {
	        rs = SR_BUGCHECK ;
	        if (curp->scp) {
	            if (callp->curend) {
		        auto	co = callp->curend ;
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
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (txtindex_curend) */

int txtindex_curlook(txtindex *op,TI_CUR *curp,mainv klp) noex {
	int		rs ;
	if ((rs = txtindex_magic(op,curp,klp)) >= 0) {
	    txtindex_calls	*callp = callsp(op->callp) ;
	    rs = SR_NOTOPEN ;
	    if (curp->magic == TXTINDEX_MAGIC) {
		rs = SR_BUGCHECK ;
		if (auto co = callp->curlook ; co != nullptr) {
		    rs = co(op->obj,curp->scp,klp) ;
		}
	    } /* end if (magic) */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (txtindex_curlook) */

int txtindex_curenum(txtindex *op,TI_CUR *curp,TI_TAG *tagp,
		char *rb,int rl) noex {
	int		rs ;
	int		fl = 0 ;
	if ((rs = txtindex_magic(op,curp,tagp)) >= 0) {
	    txtindex_calls	*callp = callsp(op->callp) ;
	    rs = SR_NOTOPEN ;
	    if (curp->magic == TXTINDEX_MAGIC) {
		rs = SR_NOSYS ;
	        if (auto co = callp->curenum ; co != nullptr) {
	            rs = co(op->obj,curp->scp,tagp,rb,rl) ;
		    fl = rs ;
	        }
	    } /* end if (magic) */
	} /* end if (magic) */
	return (rs >= 0) ? fl : rs ;
}
/* end subroutine (txtindex_curenum) */


/* private subroutines */

static int txtindex_objloadbegin(txtindex *op,cchar *pr,cchar *objn) noex {
	modload		*lp = op->mlp ;
	cint		vn = sub_overlast ;
	cint		vo = VECSTR_OCOMPACT ;
	int		rs ;
	int		rs1 ;
	if (vecstr syms ; (rs = syms.start(vn,vo)) >= 0) {
	    if ((rs = syms.addsyms(objn,subs)) >= 0) {
		if (mainv sv{} ; (rs = syms.getvec(&sv)) >= 0) {
	            cchar	*modbname = TXTINDEX_MODBNAME ;
	            int		mo = 0 ;
	            mo |= modloadm.libvar ;
	            mo |= modloadm.libprs ;
	            mo |= modloadm.libsdirs ;
	            if ((rs = modload_open(lp,pr,modbname,objn,mo,sv)) >= 0) {
		        op->fl.modload = true ;
	                if (int mv[2] ; (rs = modload_getmva(lp,mv,2)) >= 0) {
		            cint	sz = op->objsize ;
		            op->objsize = mv[0] ;
		            op->cursize = mv[1] ;
		            if (void *vp ; (rs = uc_malloc(sz,&vp)) >= 0) {
		                op->obj = vp ;
		                rs = txtindex_loadcalls(op,&syms) ;
		                if (rs < 0) {
			            op->obj = nullptr ;
			            uc_free(op->obj) ;
		                }
		            } /* end if (memory-allocation) */
	                } /* end if (modload_getmva) */
	                if (rs < 0) {
		            op->fl.modload = false ;
		            modload_close(lp) ;
	                }
	            } /* end if (modload_open) */
		} /* end it (vecstr_getvec) */
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
/* end subroutine (txtindex_objloadbegin) */

static int txtindex_objloadend(txtindex *op) noex {
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
/* end subroutine (txtindex_objloadend) */

static int txtindex_loadcalls(txtindex *op,vecstr *slp) noex {
	modload		*lp = op->mlp ;
	txtindex_calls	*callp = callsp(op->callp) ;
	cint		rsn = SR_NOTFOUND ;
	int		rs = SR_OK ;
	int		rs1 ;
	int		c = 0 ;
	cchar		*sname{} ;
	for (int i = 0 ; (rs1 = slp->get(i,&sname)) >= 0 ; i += 1) {
	    if (cvoid *snp{} ; (rs = modload_getsym(lp,sname,&snp)) >= 0) {
                c += 1 ;
                switch (i) {
                case sub_open:
                    callp->open = soopen_f(snp) ;
                    break ;
                case sub_count:
                    callp->count = socount_f(snp) ;
                    break ;
                case sub_neigen:
                    callp->neigen = soneigen_f(snp) ;
                    break ;
                case sub_getinfo:
                    callp->getinfo = sogetinfo_f(snp) ;
                    break ;
                case sub_getsdn:
                    callp->getsdn = sogetsdn_f(snp) ;
                    break ;
                case sub_getsfn:
                    callp->getsfn = sogetsfn_f(snp) ;
                    break ;
                case sub_iseigen:
                    callp->iseigen = soiseigen_f(snp) ;
                    break ;
                case sub_curbegin:
                    callp->curbegin = socurbegin_f(snp) ;
                    break ;
                case sub_curlook:
                    callp->curlook = socurlook_f(snp) ;
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
/* end subroutine (txtindex_loadcalls) */

static bool isrequired(int i) noex {
	bool		f = false ;
	switch (i) {
	case sub_open:
	case sub_neigen:
	case sub_iseigen:
	case sub_curbegin:
	case sub_curlook:
	case sub_curenum:
	case sub_curend:
	case sub_close:
	    f = true ;
	    break ;
	} /* end switch */
	return f ;
}
/* end subroutine (isrequired) */


