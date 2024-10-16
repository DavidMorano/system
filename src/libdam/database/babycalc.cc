/* babycalc SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* object load management for the BABYCALCS object */
/* version %I% last-modified %G% */


/* revision history:

	- 2008-10-01, David A­D­ Morano
	This module was originally written.

*/

/* Copyright © 2008 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	object:
	babycalc

	Description:
	This module implements an interface (a trivial one) that
	provides access to the BABYCALC object (which is dynamically
	loaded).

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<dlfcn.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<new>
#include	<usystem.h>
#include	<vecstr.h>
#include	<localmisc.h>

#include	"babycalc.h"
#include	"babycalcs.h"


/* local defines */

#define	BC		babycalc
#define	BC_INFO		babycalc_info

#define	BC_MODBNAME	"babycalcs"
#define	BC_OBJNAME	"babycalcs"

#ifndef	VARLIBPATH
#define	VARLIBPATH	"LD_LIBRARY_PATH"
#endif


/* imported namespaces */

using std::nullptr_t ;			/* type */
using std::min ;			/* subroutine-template */
using std::max ;			/* subroutine-template */
using std::nothrow ;			/* constant */


/* local typedefs */

struct babycalc_calls {
	typedef int (*soopen_f)(void *,cchar *,cchar *) noex ;
	typedef int (*socheck_f)(void *,time_t) noex ;
	typedef int (*solookup_f)(void *,time_t,uint *) noex ;
	typedef int (*soinfo_f)(void *,babycalcs_info *) noex ;
	typedef int (*soclose_f)(void *) noex ;
} ;


/* external subroutines */


/* local structures */

struct babycalc_calls {
	soopen_f	open ;
	socheck_f	check ;
	solookup_f	lookup ;
	soinfo_f	info ;
	soclose_f	close ;
} ;


/* forward references */

static int	babycalc_objloadbegin(BC *,cchar *,cchar *) noex ;
static int	babycalc_objloadend(BC *) noex ;
static int	babycalc_loadcalls(BC *,cchar *) noex ;

static bool	isrequired(int) noex ;


/* external variables */


/* local variables */

enum subs {
	sub_open,
	sub_check,
	sub_lookup,
	sub_info,
	sub_close,
	sub_overlast
} ;

constexpr cpcchar	subs[] = {
	"open",
	"check",
	"lookup",
	"info",
	"close",
	NULL
} ;


/* exported variables */


/* exported subroutines */

int babycalc_open(BC *op,cchar *pr,cchar *dbname) noex {
	int		rs ;
	cchar		*objname = BC_OBJNAME ;

	if (op == NULL) return SR_FAULT ;
	if (pr == NULL) return SR_FAULT ;

	if (pr[0] == '\0') return SR_INVALID ;

	memset(op,0,sizeof(BABYCALC)) ;

	if ((rs = babycalc_objloadbegin(op,pr,objname)) >= 0) {
	    if ((rs = (*op->call.open)(op->obj,pr,dbname)) >= 0) {
	        op->magic = BABYCALC_MAGIC ;
	    }
	    if (rs < 0)
		babycalc_objloadend(op) ;
	} /* end if (obj-load-begin) */

	return rs ;
}
/* end subroutine (babycalc_open) */

int babycalc_close(BC *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = babycalc_magic(op)) >= 0) {
	    {
	        rs1 = (*op->call.close)(op->obj) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = babycalc_objloadend(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    op->magic = 0 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (babycalc_close) */

int babycalc_check(BC *op,time_t daytime) noex {
	int		rs ;
	if ((rs = babycalc_magic(op)) >= 0) {
	    rs = SR_NOSYS ;
	    if (op->call.check) {
	        rs = (*op->call.check)(op->obj,daytime) ;
	    }
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (babycalc_check) */

int babycalc_lookup(BC *op,time_t datereq,uint *rp) noex {
	int		rs ;
	if ((rs = babycalc_magic(op)) >= 0) {
	    rs = SR_NOSYS ;
	    if (op->call.lookup) {
	        rs = (*op->call.lookup)(op->obj,datereq,rp) ;
	    }
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (babycalc_lookup) */

int babycalc_info(BC *op,BC_INFO *ip) noex {
	int		rs ;
	int		n = 0 ;
	if ((rs = babycalc_magic(op)) >= 0) {
	    BABYCALCS_INFO	bi{} ;
	    if (op->call.info) {
	        rs = (*op->call.info)(op->obj,&bi) ;
	        n = rs ;
	    }
	    if (ip) {
	        memclear(ip) ;
	        if (rs >= 0) {
		    ip->wtime = bi.wtime ;
		    ip->atime = bi.atime ;
		    ip->acount = bi.acount ;
	        }
	    }
	} /* end if (magic) */
	return (rs >= 0) ? n : rs ;
}
/* end subroutine (babycalc_info) */


/* private subroutines */

static int commandment_objloadbegin(CMD *op,cchar *pr,cchar *objn) noex {
	modload		*lp = op->mlp ;
	cint		vn = sub_overlast ;
	cint		vo = VECSTR_OCOMPACT ;
	int		rs ;
	int		rs1 ;
	if (vecstr syms ; (rs = syms.start(vn,vo)) >= 0) {
	    if ((rs = syms.addsyms(objn,subs)) >= 0) {
	        if (mainv sv{} ; (rs = syms.getvec(&sv)) >= 0) {
	            cchar	*mn = CMD_MODBNAME ;
	            cchar	*on = objn ;
	            int		mo = 0 ;
	            mo |= MODLOAD_OLIBVAR ;
	            mo |= MODLOAD_OPRS ;
	            mo |= MODLOAD_OSDIRS ;
	            mo |= MODLOAD_OAVAIL ;
	            if ((rs = modload_open(lp,pr,mn,on,mo,sv)) >= 0) {
		        op->fl.modload = true ;
	                if (int mv[2] ; (rs = modload_getmva(lp,mv,2)) >= 0) {
			    cint	osz = op->objsize ;
	                    op->objsize = mv[0] ;
	                    op->cursize = mv[1] ;
			    if (void *vp{} ; (rs = uc_malloc(osz,&vp)) >= 0) {
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
	modload		*lp = op->mlp ;
	cint		rsn = SR_NOTFOUND ;
	int		rs = SR_OK ;
	int		rs1 ;
	int		c = 0 ;
	cchar		*sname{} ;
	for (int i = 0 ; (rs1 = slp->get(i,&sname)) >= 0 ; i += 1) {
	    if (cvoid *snp{} ; (rs = modload_getsym(lp,sname,&snp)) >= 0) {
                commandment_calls   *callp = callsp(op->callp) ;
                c += 1 ;
                switch (i) {
                case sub_open:
                    callp->open = soopen_f(snp) ;
                    break ;
                case sub_check:
                    callp->check = socheck_f(snp) ;
                    break ;
                case sub_lookup:
                    callp->lookup = solookup_f(snp) ;
                    break ;
                case sub_info:
                    callp->info = soinfo_f(snp) ;
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
	case sub_check:
	case sub_lookup:
	case sub_info:
	case sub_close:
	    f = true ;
	    break ;
	} /* end switch */
	return f ;
}
/* end subroutine (isrequired) */


