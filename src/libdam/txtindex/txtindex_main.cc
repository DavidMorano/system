/* txtindex_main SUPPORT */
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
#include	<cstring>
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<usystem.h>
#include	<vecstr.h>
#include	<sncpyx.h>
#include	<localmisc.h>

#include	"txtindex.h"
#include	"txtindexes.h"


/* local defines */

#define	TXTINDEX_DEFENTS	(44 * 1000)

#define	TXTINDEX_MODBNAME	"txtindexes"
#define	TXTINDEX_OBJNAME	"txtindexes"

#ifndef	SYMNAMELEN
#define	SYMNAMELEN	60
#endif

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


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

template<typename ... Args>
static int txtindex_ctor(txtindex *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    cnullptr	np{} ;
	    rs = SR_NOMEM ;
	    memclear(op) ; /* dangerous! */
	    if ((op->loaderp = new(nothrow) modload) != np) {
		rs = SR_OK ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (txtindex_ctor) */

static int txtindex_dtor(txtindex *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	    if (op->loaderp) {
		delete op->loaderp ;
		op->loaderp = nullptr ;
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
static int	txtindex_loadcalls(txtindex *,cchar *) noex ;

static bool	isrequired(int) noex ;


/* global variables */


/* local variables */

enum subs {
	sub_open,
	sub_count,
	sub_neigen,
	sub_getinfo,
	sub_iseigen,
	sub_curbegin,
	sub_lookup,
	sub_enum,
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
	"iseigen",
	"curbegin",
	"lookup",
	"read",
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
	        cchar	*objname = TXTINDEX_OBJNAME ;
	        if ((rs = txtindex_objloadbegin(op,pr,objname)) >= 0) {
	            if ((rs = (*op->call.open)(op->obj,dbname)) >= 0) {
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
	    {
	        rs1 = (*op->call.close)(op->obj) ;
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
	int		rs = SR_NOSYS ;
	if ((rs = txtindex_magic(op)) >= 0) {
	    rs = SR_NOSYS ;
	    if (op->call.audit) {
	        rs = (*op->call.audit)(op->obj) ;
	    }
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (txtindex_audit) */

int txtindex_count(txtindex *op) noex {
	int		rs ;
	if ((rs = txtindex_magic(op)) >= 0) {
	    rs = SR_NOSYS ;
	    if (op->call.count) {
	        rs = (*op->call.count)(op->obj) ;
	    }
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (txtindex_count) */

int txtindex_neigen(txtindex *op) noex {
	int		rs ;
	if ((rs = txtindex_magic(op)) >= 0) {
	    rs = SR_NOSYS ;
	    if (op->call.neigen) {
	        rs = (*op->call.neigen)(op->obj) ;
	    }
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (txtindex_neigen) */

int txtindex_getinfo(txtindex *op,TXTINDEX_INFO *ip) noex {
	int		rs ;
	if ((rs = txtindex_magic(op,ip)) >= 0) {
	    rs = SR_NOSYS ;
	    if (op->call.getinfo) {
	        rs = (*op->call.getinfo)(op->obj,ip) ;
	    }
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (txtindex_getinfo) */

int txtindex_curbegin(txtindex *op,txtindex_cur *curp) noex {
	int		rs ;
	if ((rs = txtindex_magic(op,curp)) >= 0) {
	    memclear(curp) ;
	    if (op->call.curbegin) {
	        void	*vp ;
	        if ((rs = uc_malloc(op->cursize,&vp)) >= 0) {
		    curp->scp = vp ;
		    if ((rs = (*op->call.curbegin)(op->obj,curp->scp)) >= 0) {
	    	        curp->magic = TXTINDEX_MAGIC ;
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

int txtindex_curend(txtindex *op,txtindex_cur *curp) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = txtindex_magic(op,curp)) >= 0) {
	    rs = SR_NOTOPEN ;
	    if (curp->magic == TXTINDEX_MAGIC) {
	        rs = SR_BUGCHECK ;
	        if (curp->scp) {
		    rs = SR_OK ;
	            if (op->call.curend) {
	                rs1 = (*op->call.curend)(op->obj,curp->scp) ;
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

int txtindex_lookup(txtindex *op,txtindex_cur *curp,cchar **klp) noex {
	int		rs ;
	if ((rs = txtindex_magic(op,curp,klp)) >= 0) {
	    rs = SR_NOTOPEN ;
	    if (curp->magic == TXTINDEX_MAGIC) {
		rs = (*op->call.lookup)(op->obj,curp->scp,klp) ;
	    } /* end if (magic) */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (txtindex_lookup) */

int txtindex_read(txtindex *op,txtindex_cur *curp,TXTINDEX_TAG *tagp) noex {
	int		rs = SR_NOSYS ;

	if (op == nullptr) return SR_FAULT ;
	if (curp == nullptr) return SR_FAULT ;

	if (op->magic != TXTINDEX_MAGIC) return SR_NOTOPEN ;
	if (curp->magic != TXTINDEX_MAGIC) return SR_NOTOPEN ;

	if (op->call.read != nullptr) {
	    rs = (*op->call.read)(op->obj,curp->scp,tagp) ;
	}

	return rs ;
}
/* end subroutine (txtindex_read) */


/* private subroutines */

static int txtindex_objloadbegin(txtindex *op,cchar *pr,cchar *objname) noex {
	modload		*lp = op->loaderp ;
	vecstr		syms ;
	cint		n = nelem(subs) ;
	cint		vo = VECSTR_OCOMPACT ;
	int		rs ;
	int		rs1 ;

	if ((rs = vecstr_start(&syms,n,vo)) >= 0) {
	    cint	nlen = SYMNAMELEN ;
	    bool	f_modload = false ;
	    char	nbuf[SYMNAMELEN + 1] ;

	    for (int i = 0 ; (i < n) && (subs[i] != nullptr) ; i += 1) {
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
	            cchar	*modbname = TXTINDEX_MODBNAME ;
	            int	mo = (MODLOAD_OLIBVAR | MODLOAD_OPRS | MODLOAD_OSDIRS) ;
	            rs = modload_open(lp,pr,modbname,objname,mo,sv) ;
		    f_modload = (rs >= 0) ;
		}
	    } /* end if (ok) */

	    rs1 = vecstr_finish(&syms) ;
	    if (rs >= 0) rs = rs1 ;
	    if ((rs < 0) && f_modload)
		modload_close(lp) ;
	} /* end if (vecstr) */

	if (rs >= 0) {
	    int	mv[2] ;
	    if ((rs = modload_getmva(lp,mv,2)) >= 0) {
		void	*p ;

		op->objsize = mv[0] ;
		op->cursize = mv[1] ;
		if ((rs = uc_malloc(op->objsize,&p)) >= 0) {
		    op->obj = p ;
		    rs = txtindex_loadcalls(op,objname) ;
		    if (rs < 0) {
			uc_free(op->obj) ;
			op->obj = nullptr ;
		    }
		} /* end if (memory-allocation) */
	    } /* end if (modload_getmva) */
	    if (rs < 0)
		modload_close(lp) ;
	} /* end if (ok) */

	return rs ;
}
/* end subroutine (txtindex_objloadbegin) */

static int txtindex_objloadend(txtindex *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;

	if (op->obj != nullptr) {
	    rs1 = uc_free(op->obj) ;
	    if (rs >= 0) rs = rs1 ;
	    op->obj = nullptr ;
	}

	rs1 = modload_close(op->loaderp) ;
	if (rs >= 0) rs = rs1 ;

	return rs ;
}
/* end subroutine (txtindex_objloadend) */

static int txtindex_loadcalls(txtindex *op,cchar *objname) noex {
	modload		*lp = op->loaderp ;
	cint	nlen = SYMNAMELEN ;
	int		rs = SR_OK ;
	int		i ;
	int		c = 0 ;
	char		nbuf[SYMNAMELEN + 1] ;
	const void	*snp ;

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
			(int (*)(void *,cchar *)) snp ;
		    break ;

		case sub_count:
		    op->call.count = (int (*)(void *)) snp ;
		    break ;

		case sub_neigen:
		    op->call.neigen = (int (*)(void *)) snp ;
		    break ;

		case sub_getinfo:
		    op->call.getinfo = (int (*)(void *,txtindexes_info *)) snp ;
		    break ;

		case sub_iseigen:
		    op->call.iseigen = (int (*)(void *,cchar *,int)) snp ;
		    break ;

		case sub_curbegin:
		    op->call.curbegin = 
			(int (*)(void *,void *)) snp ;
		    break ;

		case sub_lookup:
		    op->call.lookup = 
			(int (*)(void *,void *,cchar **)) snp ;
		    break ;

		case sub_enum:
		    op->call.read = 
			(int (*)(void *,void *,TIS_TAG *)) snp ;
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
/* end subroutine (txtindex_loadcalls) */

static bool isrequired(int i) noex {
	bool		f = false ;
	switch (i) {
	case sub_open:
	case sub_neigen:
	case sub_iseigen:
	case sub_curbegin:
	case sub_lookup:
	case sub_enum:
	case sub_curend:
	case sub_close:
	    f = true ;
	    break ;
	} /* end switch */
	return f ;
}
/* end subroutine (isrequired) */


