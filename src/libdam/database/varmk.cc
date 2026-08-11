/* varmk SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* VARMK management */
/* version %I% last-modified %G% */

#define	CF_LOOKSELF	0		/* try searching "SELF" for SO */

/* revision history:

	= 1998-12-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services
	(RNS).

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	varmk

	Description:
	This module implements an interface (a trivial one) that
	provides access to the VARMK object (which is dynamically
	loaded).

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/param.h>		/* POSIX® */
#include	<sys/stat.h>		/* POSIX® */
#include	<dlfcn.h>		/* POSIX® */
#include	<unistd.h>		/* POSIX® */
#include	<fcntl.h>		/* POSIX® */
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
#include	<localmisc.h>		/* LIBU */

#include	"varmk.h"
#include	"varmks.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |memclear(3u)| */

/* local defines */

#define	VARMK_OBJNAME	"varmks"
#define	VARMK_MODBNAME	"varmks"

#define	VM		varmk

#ifndef	VARPRLOCAL
#define	VARPRLOCAL	"LOCAL"
#endif

#ifndef	SYMNAMELEN
#define	SYMNAMELEN	60
#endif


/* imported namespaces */

using libuc::mem ;			/* variable */


/* local typedefs */

extern "C" {
    typedef int	(*soopen_f)	(void *,cchar *,int,mode_t,int) noex ;
    typedef int	(*sochgrp_f)	(void *,gid_t) noex ;
    typedef int	(*soadd_f)	(void *,cchar *,cchar *,int) noex ;
    typedef int	(*soabort_f)	(void *) noex ;
    typedef int	(*soclose_f)	(void *) noex ;
} /* end extern (C) */


/* external subroutines */


/* external variables */


/* local structures */

struct calls {
    soopen_f		open ;
    sochgrp_f		chgrp ;
    soadd_f		add ;
    soabort_f		abort ;
    soclose_f		close ;
} ; /* end struct (varmk) */

typedef calls *	callsp ;


/* forward references */

template<typename ... Args>
local int varmk_ctor(VM *op,Args ... args) noex {
	VARMK		*hop = op ;
	cnullptr	np{} ;
	cnothrow	nt{} ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    memclear(hop) ;
	    rs = SR_NOMEM ;
	    if ((op->mlp = new(nt) modload) != np) ylikely {
		calls    *callp ;
                if ((callp = new(nt) calls) != np) ylikely {
                    op->callp = callp ;
                    rs = SR_OK ;
                } /* end if (new-calls) */
                if (rs < 0) {
                    delete op->mlp ;
                    op->mlp = nullptr ;
                } /* end if (error) */
	    } /* end if (new-modload) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (varmk_ctor) */

local int varmk_dtor(VM *op) noex {
	int		rs = SR_FAULT ;
	if (op) ylikely {
	    rs = SR_OK ;
            if (op->callp) ylikely {
                calls    *callp = callsp(op->callp) ;
                delete callp ;
                op->callp = nullptr ;
            }
	    if (op->mlp) ylikely {
		delete op->mlp ;
		op->mlp = nullptr ;
	    }
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (varmk_dtor) */

template<typename ... Args>
local inline int varmk_magic(VM *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    rs = (op->magval == VARMK_MAGIC) ? SR_OK : SR_NOTOPEN ;
	} /* end if */
	return rs ;
} /* end subroutine (varmk_magic) */

local int	varmk_objloadbegin(varmk *,cchar *,cchar *) noex ;
local int	varmk_objloadend(varmk *) noex ;
local int	varmk_loadcalls(varmk *,vecstr *) noex ;

local bool	isrequired(int) noex ;


/* external variables */


/* local variables */

enum subs {
	sub_open,
	sub_add,
	sub_abort,
	sub_chgrp,
	sub_close,
	sub_overlast
} ; /* end enum (subs) */

constexpr cpcchar	subnames[] = {
	"open",
	"add",
	"abort",
	"chgrp",
	"close",
	nullptr
} ; /* end array (subnames) */


/* exported variables */


/* exported subroutines */

namespace {
    struct opener {
	VM		*op ;
	int		of ;
	int		n ;
	mode_t		om ;
	opener(VM *p,int o,mode_t m,int ªn) noex : op(p), of(o) {
	    n = ªn ;
	    om = m ;
	} ; /* end ctor */
	int operator () (cchar *) noex ;
    } ; /* end struct (opener) */
} /* end namespace */

int varmk_open(VM *op,cchar *dbname,int of,mode_t om,int n) noex {
	int		rs ;
	if ((rs = varmk_ctor(op,dbname)) >= 0) {
	    rs = SR_INVALID ;
	    if (dbname[0]) {
		opener oo(op,of,om,n) ;
		rs = oo(dbname) ;
	    } /* end if (valid) */
	    if (rs < 0) {
		varmk_dtor(op) ;
	    } /* end if (error) */
	} /* end if (varmk_ctor) */
	return rs ;
} /* end subroutine (varmk_open) */

int opener::operator () (cchar *dbn) noex {
	cchar		*objname = VARMK_OBJNAME ;
	int		rs ;
	if ((rs = varmk_objloadbegin(op,dbn,objname)) >= 0) {
	    calls *callp = resumelife<calls>(op->callp) ;
	    if ((rs = callp->open(op->obj,dbn,of,om,n)) >= 0) {
		op->magval = VARMK_MAGIC ;
	    } /* end if (ok) */
	    if (rs < 0) {
		varmk_objloadend(op) ;
	    } /* end if (error) */
	} /* end if (objloadbegin) */
	return rs ;
} /* end method (opener::operator) */

/* free up the entire vector string data structure object */
int varmk_close(VM *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = varmk_magic(op)) >= 0) ylikely {
	    if (calls *callp = resumelife<calls>(op->callp) ; callp) ylikely {
	        rs1 = callp->close(op->obj) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = varmk_objloadend(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = varmk_dtor(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    op->magval = 0 ;
	} /* end if (varmk_magic) */
	return rs ;
} /* end subroutine (varmk_close) */

int varmk_add(VM *op,cchar *k,cchar *vap,int val) noex {
	int		rs ;
	if ((rs = varmk_magic(op)) >= 0) ylikely {
	    rs = SR_BUGCHECK ;
	    if (calls *callp = resumelife<calls>(op->callp) ; callp) ylikely {
		rs = SR_NOSYS ;
		if (callp->add) {
		    rs = callp->add(op->obj,k,vap,val) ;
	         } /* end if (non-null) */
	    } /* end if (bug-check) */
	} /* end if (varmk_magic) */
	return rs ;
} /* end subroutine (varmk_add) */

int varmk_abort(VM *op) noex {
	int		rs ;
	if ((rs = varmk_magic(op)) >= 0) ylikely {
	    rs = SR_BUGCHECK ;
	    if (calls *callp = resumelife<calls>(op->callp) ; callp) ylikely {
		rs = SR_NOSYS ;
		if (callp->abort) {
		    rs = callp->abort(op->obj) ;
	         } /* end if (non-null) */
	    } /* end if (bug-check) */
	} /* end if (varmk_magic) */
	return rs ;
} /* end subroutine (varmk_abort) */

int varmk_chgrp(VM *op,gid_t gid) noex {
	int		rs ;
	if ((rs = varmk_magic(op)) >= 0) ylikely {
	    rs = SR_BUGCHECK ;
	    if (calls *callp = resumelife<calls>(op->callp) ; callp) ylikely {
		rs = SR_NOSYS ;
		if (callp->chgrp) {
		    rs = callp->chgrp(op->obj,gid) ;
	         } /* end if (non-null) */
	    } /* end if (bug-check) */
	} /* end if (varmk_magic) */
	return rs ;
} /* end subroutine (varmk_chgrp) */


/* private subroutines */

local int varmk_objloadbegin(VM *op,cchar *pr,cchar *objn) noex {
	modload		*mlp = op->mlp ;
	cint		vn = sub_overlast ;
	cint		vo = vecstrm.compact ;
	int		rs ;
	int		rs1 ;
	if (vecstr syms ; (rs = syms.start(vn,vo)) >= 0) ylikely {
	    if ((rs = syms.addsyms(objn,subnames)) >= 0) ylikely {
	        if (mainv sv ; (rs = syms.getvec(&sv)) >= 0) ylikely {
	            cchar	*mn = VARMK_MODBNAME ;
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
	                        rs = varmk_loadcalls(op,&syms) ;
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
} /* end subroutine (varmk_objloadbegin) */

local int varmk_objloadend(VM *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (op->obj) {
	    rs1 = mem.free(op->obj) ;
	    if (rs >= 0) rs = rs1 ;
	    op->obj = nullptr ;
	} /* end if (memory-release) */
	if (op->mlp && op->fl.modload) {
	    op->fl.modload = false ;
	    rs1 = modload_close(op->mlp) ;
	    if (rs >= 0) rs = rs1 ;
	}
	return rs ;
} /* end subroutine (varmk_objloadend) */

local int varmk_loadcalls(VM *op,vecstr *slp) noex {
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
		case sub_add:
		    callp->add		= soadd_f(snp) ;
		    break ;
		case sub_abort:
		    callp->abort	= soabort_f(snp) ;
		    break ;
		case sub_chgrp:
		    callp->chgrp	= sochgrp_f(snp) ;
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
} /* end subroutine (varmk_loadcalls) */

local bool isrequired(int i) noex {
	bool		f = false ;
	switch (i) {
	case sub_open:
	case sub_add:
	case sub_close:
	    f = true ;
	    break ;
	} /* end switch */
	return f ;
} /* end subroutine (isrequired) */


