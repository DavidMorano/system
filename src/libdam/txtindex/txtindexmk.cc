/* txtindexmk SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* interface to the TIMS loadable object */
/* version %I% last-modified %G% */


/* revision history:

	- 2008-10-01, David A­D­ Morano
	This module was originally written.

*/

/* Copyright © 2008 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	txtindexmk

	Description:
	This module implements an interface (a trivial one) that
	provides access to the TIM object (which is dynamically
	loaded).

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<new>
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<netdb.h>
#include	<usystem.h>
#include	<getbufsize.h>
#include	<getnodedomain.h>
#include	<mkpr.h>
#include	<estrings.h>
#include	<vecstr.h>
#include	<localmisc.h>

#include	"txtindexmk.h"
#include	"txtindexmks.h"

import libutil ;

/* local defines */

#define	TIM_DEFENTS	(44 * 1000)
#define	TIM_MODBNAME	"txtindexmks"
#define	TIM_OBJNAME	"txtindexmks"
#define	TIM		txtindexmk
#define	TIM_PA		txtindexmk_pa
#define	TIM_TAG		txtindexmk_tag
#define	TIM_KEY		txtindexmk_key
#define	TIM_ENTS	txtindexmk_ents

#define	VARPRNAME	"LOCAL"


/* imported namespaces */

using std::nullptr_t ;			/* type */
using std::min ;			/* subroutine-template */
using std::max ;			/* subroutine-template */
using std::nothrow ;			/* constant */


/* local typedefs */

extern "C" {            
    typedef int	(*soopen_f)(void *,TIM_PA *,cchar *,int,int) noex ;
    typedef int	(*soaddeigens_f)(void *,TIM_KEY *,int) noex ;
    typedef int	(*soaddtags_f)(void *,TIM_TAG *,int) noex ;
    typedef int	(*sonoop_f)(void *) noex ;
    typedef int	(*soabort_f)(void *) noex ;
    typedef int	(*soclose_f)(void *) noex ;
} /* end extern (C) */


/* external subroutines */


/* external variables */


/* local structures */

struct vars {
	int		maxhostlen ;
	int		maxpathlen ;
} ;

struct txtindexmk_calls {
    soopen_f		open ;
    soaddeigens_f	addeigens ;
    soaddtags_f		addtags ;
    sonoop_f		noop ;
    soabort_f		abort ;
    soclose_f		close ;
} ;

typedef txtindexmk_calls *	callsp ;

namespace {
    struct opener {
	TIM		*op ;
	TIM_PA		*pp ;
	cchar		*db ;
	int		of ;
	mode_t		om ;
	opener(TIM *aop,TIM_PA *app,cchar *adb,int aof,mode_t aom) noex { 
		op = aop ;
		pp = app ;
		db = adb ;
		of = aof ;
		om = aom ;
	} ;
	int operator () (char *ap) noex ;
    } ; /* end struct (opener) */
}


/* forward references */

template<typename ... Args>
static int txtindexmk_ctor(txtindexmk *op,Args ... args) noex {
	TXTINDEXMK	*hop = op ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    cnullptr	np{} ;
	    rs = SR_NOMEM ;
	    memclear(hop) ;
	    if ((op->mlp = new(nothrow) modload) != np) {
	        if ((op->callp = new(nothrow) txtindexmk_calls) != np) {
		    rs = SR_OK ;
	        } /* end if (new-txtindexmks_calls) */
		if (rs < 0) {
		    delete op->mlp ;
		    op->mlp = nullptr ;
		}
	    } /* end if (new-modload) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (txtindexmk_ctor) */

static int txtindexmk_dtor(txtindexmk *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	    if (op->callp) {
	        txtindexmk_calls	*callp = callsp(op->callp) ;
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
/* end subroutine (txtindexmk_dtor) */

template<typename ... Args>
static inline int txtindexmk_magic(txtindexmk *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magic == TXTINDEXMK_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}
/* end subroutine (txtindexmk_magic) */

static int	txtindexmk_objloadbegin(TIM *,cchar *,cchar *) noex ;
static int	txtindexmk_objloadend(TIM *) noex ;
static int	txtindexmk_loadcalls(TIM *,vecstr *) noex ;

static int	mkvars() noex ;

static bool	isrequired(int) noex ;


/* local variables */

enum subs {
	sub_open,
	sub_addeigens,
	sub_addtags,
	sub_close,
	sub_noop,
	sub_abort,
	sub_overlast
} ;

constexpr cpcchar	subs[] = {
	"open",
	"addeigens",
	"addtags",
	"close",
	"noop",
	"abort",
	nullptr
} ;

static vars		var ;


/* exported variables */


/* exported subroutines */

int txtindexmk_open(TIM *op,TIM_PA *pp,cchar *db,int of,mode_t om) noex {
	int		rs ;
	int		rs1 ;
	int		rv = 0 ;
	if ((rs = txtindexmk_ctor(op,pp,db)) >= 0) {
	    static cint		rsv = mkvars() ;
	    if ((rs = rsv) >= 0) {
		cint	sz = (var.maxhostlen + 1 + var.maxpathlen + 1) ;
		if (char *ap{} ; (rs = uc_malloc(sz,&ap)) >= 0) {
		    opener	oo(op,pp,db,of,om) ;
		    {
		        rs = oo(ap) ;
			rv = rs ;
		    }
		    rs1 = uc_free(ap) ;
		    if (rs >= 0) rs = rs1 ;
		} /* end if (m-a-f) */
	    } /* end if (mkvars) */
	    if (rs < 0) {
		txtindexmk_dtor(op) ;
	    }
	} /* end if (txtindexmk_ctor) */
	return (rs >= 0) ? rv : rs ;
}
/* end subroutine (txtindexmk_open) */

int opener::operator () (char *ap) noex {
	int		rs ;
	int		rv = 0 ;
	char		*dbuf = ap ;
	if ((rs = getnodedomain(nullptr,dbuf)) >= 0) {
	    cint	plen = var.maxpathlen ;
	    cchar	*pn = VARPRNAME ;
	    char	*pbuf = (ap + (var.maxhostlen + 1)) ;
	    if ((rs = mkpr(pbuf,plen,pn,dbuf)) >= 0) {
	        cchar	*objn = TIM_OBJNAME ;
		cchar	*pr = pbuf ;
		if ((rs = txtindexmk_objloadbegin(op,pr,objn)) >= 0) {
	    	    txtindexmk_calls	*callp = callsp(op->callp) ;
	    	    if ((rs = (*callp->open)(op->obj,pp,db,of,om)) >= 0) {
			rv = rs ;
			op->magic = TXTINDEXMK_MAGIC ;
	    	    }
	    	    if (rs < 0) {
			txtindexmk_objloadend(op) ;
		    }
		} /* end if (txtindexmk_objloadbegin) */
	    } /* end if (mkpr) */
	} /* end if (getnodedomain) */
	return (rs >= 0) ? rv : rs ;
}
/* end method (opener::operator) */

int txtindexmk_close(TIM *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = txtindexmk_magic(op)) >= 0) {
	    if (op->obj) {
	        txtindexmk_calls	*callp = callsp(op->callp) ;
	        rs1 = (*callp->close)(op->obj) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = txtindexmk_objloadend(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
		rs1 = txtindexmk_dtor(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    op->magic = 0 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (txtindexmk_close) */

int txtindexmk_addeigens(TIM *op,TIM_KEY *keys,int nkeys) noex {
	int		rs ;
	if ((rs = txtindexmk_magic(op,keys)) >= 0) {
	    txtindexmk_calls	*callp = callsp(op->callp) ;
	    rs = (*callp->addeigens)(op->obj,keys,nkeys) ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (txtindexmk_addeigens) */

int txtindexmk_addtags(TIM *op,TIM_TAG *tags,int ntags) noex {
	int		rs ;
	if ((rs = txtindexmk_magic(op,tags)) >= 0) {
	    txtindexmk_calls	*callp = callsp(op->callp) ;
	    rs = (*callp->addtags)(op->obj,tags,ntags) ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (txtindexmk_addtags) */

int txtindexmk_noop(TIM *op) noex {
	int		rs ;
	if ((rs = txtindexmk_magic(op)) >= 0) {
	    txtindexmk_calls	*callp = callsp(op->callp) ;
	    if (callp->noop) {
	        rs = (*callp->noop)(op->obj) ;
	    }
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (txtindexmk_noop) */


/* private subroutines */

static int txtindexmk_objloadbegin(TIM *op,cchar *pr,cchar *objn) noex {
	modload		*lp = op->mlp ;
	cint		vn = sub_overlast ;
	cint		vo = VECSTR_OCOMPACT ;
	int		rs ;
	int		rs1 ;
	if (vecstr syms ; (rs = syms.start(vn,vo)) >= 0) {
	    if ((rs = syms.addsyms(objn,subs)) >= 0) {
		if (mainv sv{} ; (rs = syms.getvec(&sv)) >= 0) {
	            cchar	*mn = TIM_MODBNAME ;
		    int		mo = 0 ;
	            mo |= modloadm.libvar ;
	            mo |= modloadm.libsdirs ;
	            if ((rs = modload_open(lp,pr,mn,objn,mo,sv)) >= 0) {
			op->fl.modload = true ;
	    		if (int mv[2] ; (rs = modload_getmva(lp,mv,2)) >= 0) {
			    cint	sz = op->objsize ;
			    op->objsize = mv[0] ;
		            op->cursize = mv[1] ;
			    if (void *vp{} ; (rs = uc_malloc(sz,&vp)) >= 0) {
		                op->obj = vp ;
		                rs = txtindexmk_loadcalls(op,&syms) ;
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
/* end subroutine (txtindexmk_objloadbegin) */

static int txtindexmk_objloadend(TIM *op) noex {
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
/* end subroutine (txtindexmk_objloadend) */

static int txtindexmk_loadcalls(TIM *op,vecstr *slp) noex {
	modload		*lp = op->mlp ;
	callsp		callp = callsp(op->callp) ;
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
                case sub_addeigens:
                    callp->addeigens = soaddeigens_f(snp) ;
                    break ;
                case sub_addtags:
                    callp->addtags = soaddtags_f(snp) ;
                    break ;
                case sub_noop:
                    callp->noop = sonoop_f(snp) ;
                    break ;
                case sub_abort:
                    callp->abort = soabort_f(snp) ;
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
/* end subroutine (txtindexmk_loadcalls) */

static int mkvars() noex {
	int		rs ;
	if ((rs = getbufsize(getbufsize_hn)) >= 0) {
	    var.maxhostlen = rs ;
	    if ((rs = getbufsize(getbufsize_hn)) >= 0) {
		var.maxpathlen = rs ;
	    }
	}
	return rs ;
}
/* end subroutine (mkvars) */

static bool isrequired(int i) noex {
	bool		f = false ;
	switch (i) {
	case sub_open:
	case sub_addeigens:
	case sub_addtags:
	case sub_close:
	    f = true ;
	    break ;
	} /* end switch */
	return f ;
}
/* end subroutine (isrequired) */


