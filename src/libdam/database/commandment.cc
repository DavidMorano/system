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
#include	<ctime>			/* CSTD */
#include	<climits>		/* CSTD */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<new>			/* C++STD placement-new */
#include	<memory>		/* C++STD |destroy_at(3c++)| */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<ucmem.h>		/* LIBUC */
#include	<modload.h>		/* LIBUC */
#include	<vecstr.h>		/* LIBUC */
#include	<sncpy.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */

#include	"commandment.h"
#include	"commandments.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |memclear(3u)| */

/* local defines */

#define	CMD		commandment
#define	CMD_CA		commandment_calls
#define	CMD_CUR		commandment_cur
#define	CMD_MODBNAME	"commandments"
#define	CMD_OBJNAME	"commandments"


/* local namespaces */

using libuc::mem ;			/* variable */


/* local typedefs */

extern "C" {
    typedef int	(*soopen_f)	(void *,cchar *,cchar *) noex ;
    typedef int	(*soaudit_f)	(void *) noex ;
    typedef int	(*socount_f)	(void *) noex ;
    typedef int	(*sonummax_f)	(void *) noex ;
    typedef int	(*soread_f)	(void *,char *,int,uint) noex ;
    typedef int	(*soget_f)	(void *,int,char *,int) noex ;
    typedef int	(*socurbegin_f)	(void *,void *) noex ;
    typedef int	(*socurend_f)	(void *,void *) noex ;
    typedef int	(*socurenum_f)	(void *,void *,void *,char *,int) noex ;
    typedef int	(*soclose_f)	(void *) noex ;
} /* end extern (C) */


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
local int commandment_ctor(CMD *op,Args ... args) noex {
	COMMANDMENT	*hop = op ;
	cnullptr	np{} ;
	cnothrow	nt{} ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    memclear(hop) ;
	    rs = SR_NOMEM ;
	    if ((op->mlp = new(nt) modload) != np) ylikely {
		CMD_CA    *callp ;
                if ((callp = new(nt) CMD_CA) != np) ylikely {
                    op->callp = callp ;
                    rs = SR_OK ;
                } /* end if (new-CMD_CA) */
                if (rs < 0) {
                    delete op->mlp ;
                    op->mlp = nullptr ;
                } /* end if (error) */
	    } /* end if (new-modload) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (commandment_ctor) */

local int commandment_dtor(CMD *op) noex {
	int		rs = SR_FAULT ;
	if (op) ylikely {
	    rs = SR_OK ;
            if (op->callp) ylikely {
                CMD_CA    *callp = callsp(op->callp) ;
                delete callp ;
                op->callp = nullptr ;
            }
	    if (op->mlp) ylikely {
		delete op->mlp ;
		op->mlp = nullptr ;
	    }
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (commandment_dtor) */

template<typename ... Args>
local inline int commandment_magic(CMD *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    rs = (op->magval == COMMANDMENT_MAGIC) ? SR_OK : SR_NOTOPEN ;
	} /* end if */
	return rs ;
} /* end subroutine (commandment_magic) */

local int	commandment_objloadbegin	(CMD *,cchar *,cchar *) noex ;
local int	commandment_objloadend		(CMD *) noex ;
local int	commandment_loadcalls		(CMD *,vecstr *) noex ;

local bool	isrequired(int) noex ;


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
} ; /* end enum (subs) */

namespace {
    struct subnamer {
	cchar		*n[sub_overlast + 1] ;
	consteval subnamer() noex {
	    n[sub_open]		= "open" ;
	    n[sub_audit]	= "audit" ;
	    n[sub_count]	= "count" ;
	    n[sub_nummax]	= "max" ;
	    n[sub_read]		= "read" ;
	    n[sub_get]		= "get" ;
	    n[sub_curbegin]	= "curbegin" ;
	    n[sub_curend]	= "curend" ;
	    n[sub_curenum]	= "curenum" ;
	    n[sub_close]	= "close" ;
	    n[sub_overlast]	= nullptr ;
	} ; /* end ctor */
    } ; /* end struct (subnamer) */
} /* end namespace */

constexpr subnamer	subname ;


/* exported variables */


/* exported subroutines */

int commandment_open(CMD *op,cchar *pr,cchar *dbn) noex {
	int		rs ;
	if ((rs = commandment_ctor(op,pr)) >= 0) ylikely {
	    rs = SR_INVALID ;
	    if (pr[0]) {
	        cchar	*objn = CMD_OBJNAME ;
	        if ((rs = commandment_objloadbegin(op,pr,objn)) >= 0) ylikely {
		    CMD_CA	*callp = callsp(op->callp) ;
		    rs = SR_NOSYS ;
		    if (cauto co = callp->open ; co) {
	                if ((rs = co(op->obj,pr,dbn)) >= 0) {
		            op->magval = COMMANDMENT_MAGIC ;
	                }
		    } /* end if (open) */
	            if (rs < 0) {
		        commandment_objloadend(op) ;
	            } /* end if (error) */
	        } /* end if (objload-begin) */
	    } /* end if (valid) */
	    if (rs < 0) {
		commandment_dtor(op) ;
	    } /* end if (error) */
	} /* end if (commandment_ctor) */
	return rs ;
} /* end subroutine (commandment_open) */

int commandment_close(CMD *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = commandment_magic(op)) >= 0) ylikely {
	    rs = SR_BUGCHECK ;
	    if (CMD_CA *callp = callsp(op->callp) ; callp) ylikely {
		rs = SR_OK ;
	        if (cauto co = callp->close ; co) {
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
	        op->magval = 0 ;
	    } /* end if (valid) */
	} /* end if (commandment_magic) */
	return rs ;
} /* end subroutine (commandment_close) */

int commandment_audit(CMD *op) noex {
	int		rs ;
	if ((rs = commandment_magic(op)) >= 0) ylikely {
	    CMD_CA	*callp = callsp(op->callp) ;
	    rs = SR_NOSYS ;
	    if (cauto co = callp->audit ; co) {
	        rs = co(op->obj) ;
	    }
	} /* end if (magic) */
	return rs ;
} /* end subroutine (commandment_audit) */

int commandment_count(CMD *op) noex {
	int		rs ;
	if ((rs = commandment_magic(op)) >= 0) ylikely {
	    CMD_CA	*callp = callsp(op->callp) ;
	    rs = SR_NOSYS ;
	    if (cauto co = callp->count ; co) {
	        rs = co(op->obj) ;
	    }
	} /* end if (magic) */
	return rs ;
} /* end subroutine (commandment_count) */

int commandment_nummax(CMD *op) noex {
	int		rs ;
	if ((rs = commandment_magic(op)) >= 0) ylikely {
	    CMD_CA	*callp = callsp(op->callp) ;
	    rs = SR_NOSYS ;
	    if (cauto co = callp->nummax ; co) {
	        rs = co(op->obj) ;
	    }
	} /* end if (magic) */
	return rs ;
} /* end subroutine (commandment_nummax) */

int commandment_read(CMD *op,char *rbuf,int rlen,uint cn) noex {
	int		rs ;
	if ((rs = commandment_magic(op,rbuf)) >= 0) ylikely {
	    CMD_CA	*callp = callsp(op->callp) ;
	    rs = SR_NOSYS ;
	    if (cauto co = callp->read ; co) {
    		rs = co(op->obj,rbuf,rlen,cn) ;
	    }
	} /* end if (magic) */
	return rs ;
} /* end subroutine (commandment_read) */

int commandment_get(CMD *op,int i,char *rbuf,int rlen) noex {
	int		rs ;
	if ((rs = commandment_magic(op,rbuf)) >= 0) ylikely {
	    rs = SR_INVALID ;
	    if (i >= 0) ylikely {
	        uint	cn = uint(i) ;
	        rs = commandment_read(op,rbuf,rlen,cn) ;
	    }
	} /* end if (magic) */
	return rs ;
} /* end subroutine (commandment_get) */

int commandment_curbegin(CMD *op,CMD_CUR *curp) noex {
	int		rs ;
	if ((rs = commandment_magic(op,curp)) >= 0) ylikely {
	    CMD_CA	*callp = callsp(op->callp) ;
	    memclear(curp) ;
	    if (cauto co = callp->curbegin ; co) ylikely {
		cint	csz = op->cursz ;
	        if (void *vp ; (rs = mem.mall(csz,&vp)) >= 0) ylikely {
		    curp->scp = vp ;
		    if ((rs = co(op->obj,curp->scp)) >= 0) {
		         curp->magval = COMMANDMENT_MAGIC ;
		    }
	            if (rs < 0) {
		        mem.free(curp->scp) ;
		        curp->scp = nullptr ;
	            } /* end if (error) */
	        } /* end if (memory-allocation) */
	    } else {
	        rs = SR_NOTSOCK ;
	    }
	    if (rs < 0) {
	        memclear(curp) ;
	    } /* end if (error) */
	} /* end if (magic) */
	return rs ;
} /* end subroutine (commandment_curbegin) */

int commandment_curend(CMD *op,CMD_CUR *curp) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = commandment_magic(op,curp)) >= 0) ylikely {
	    CMD_CA	*callp = callsp(op->callp) ;
	    rs = SR_NOTOPEN ;
	    if ((curp->magval == COMMANDMENT_MAGIC) && curp->scp) ylikely {
	        if (cauto co = callp->curend ; co) ylikely {
	            rs1 = co(op->obj,curp->scp) ;
		    if (rs >= 0) rs = rs1 ;
	        }
		{
	            rs1 = mem.free(curp->scp) ;
	            if (rs >= 0) rs = rs1 ;
	            curp->scp = nullptr ;
		} /* end if (memory-release) */
	        curp->magval = 0 ;
	    } /* end if (valid) */
	} /* end if (magic) */
	return rs ;
} /* end subroutine (commandment_curend) */

int commandment_curenum(CMD *op,CMD_CUR *curp,uint *cnp,
		char *rbuf,int rlen) noex {
	int		rs ;
	if ((rs = commandment_magic(op,curp,rbuf)) >= 0) ylikely {
	    CMD_CA	*callp = callsp(op->callp) ;
	    rs = SR_NOSYS ;
	    if (cauto co = callp->curenum ; co) ylikely {
	        COMMANDMENTS_ENT	cse{} ;
	        rs = co(op->obj,curp->scp,&cse,rbuf,rlen) ;
	        if (cnp) *cnp = cse.cn ;
	    }
	} /* end if (magic) */
	return rs ;
} /* end subroutine (commandment_curenum) */

#ifdef	COMMENT

int commandment_search(CMD *op,cc *s,cmpfunc,cchar **rpp) noex {
	int		rs ;
	int		i = 0 ;
	if (cmpfunc == nullptr) cmpfunc = defaultcmp ;
	if ((rs = commandment_magic(op,s,rpp)) >= 0) ylikely {
	    char	**rpp2 ;
	    if (op->fl.osorted && (! op->fl.issorted)) {
	        op->fl.issorted = true ;
	        if (op->c > 1) {
		    csize	qn = size_t(op->i) ;
		    csize	qsz = szof(char *) ;
	            qsort(op->va,qn,qsz,cmpfunc) ;
		} /* end if (non-zero positive) */
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
} /* end subroutine (commandment_search) */

#endif /* COMMENT */


/* private subroutines */

local int commandment_objloadbegin(CMD *op,cchar *pr,cchar *objn) noex {
	modload		*mlp = op->mlp ;
	cint		vn = sub_overlast ;
	cint		vo = vecstrm.compact ;
	int		rs ;
	int		rs1 ;
	if (vecstr syms ; (rs = syms.start(vn,vo)) >= 0) ylikely {
	    if ((rs = syms.addsyms(objn,subname.n)) >= 0) ylikely {
	        if (mainv sv ; (rs = syms.getvec(&sv)) >= 0) ylikely {
	            cchar	*mn = CMD_MODBNAME ;
	            cchar	*on = objn ;
	            int		mo = 0 ;
	            mo |= modloadm.libvar ;
	            mo |= modloadm.libprs ;
	            mo |= modloadm.libsdirs ;
	            mo |= modloadm.avail ;
	            if ((rs = modload_open(mlp,pr,mn,on,mo,sv)) >= 0) ylikely {
		        op->fl.modload = true ;
	                if (int mv[2] ; (rs = modload_getmva(mlp,mv,2)) >= 0) {
			    cint	osz = mv[0] ;
	                    op->objsz = mv[0] ;
	                    op->cursz = mv[1] ;
			    if (void *vp ; (rs = mem.mall(osz,&vp)) >= 0) {
	                        op->obj = vp ;
	                        rs = commandment_loadcalls(op,&syms) ;
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
} /* end subroutine (commandment_objloadbegin) */

local int commandment_objloadend(CMD *op) noex {
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
	} /* end if */
	return rs ;
} /* end subroutine (commandment_objloadend) */

local int commandment_loadcalls(CMD *op,vecstr *slp) noex {
	modload		*mlp = op->mlp ;
	cint		rsn = SR_NOTFOUND ;
	int		rs = SR_OK ;
	int		rs1 ;
	int		c = 0 ;
	cchar		*sname{} ;
	for (int i = 0 ; (rs1 = slp->get(i,&sname)) >= 0 ; i += 1) {
	    if (cvoid *snp{} ; (rs = modload_getsym(mlp,sname,&snp)) >= 0) {
                CMD_CA   *callp = callsp(op->callp) ;
                c += 1 ;
                switch (i) {
                case sub_open:
                    callp->open		= soopen_f(snp) ;
                    break ;
                case sub_count:
                    callp->count	= socount_f(snp) ;
                    break ;
                case sub_get:
                    callp->get		= soget_f(snp) ;
                    break ;
                case sub_read:
                    callp->read		= soread_f(snp) ;
                    break ;
                case sub_nummax:
                    callp->nummax	= sonummax_f(snp) ;
                    break ;
                case sub_curbegin:
                    callp->curbegin	= socurbegin_f(snp) ;
                    break ;
                case sub_curenum:
                    callp->curenum	= socurenum_f(snp) ;
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
                if (! isrequired(i)) rs = SR_OK ;
            } /* end if (it had the call) */
	    if (rs < 0) break ;
	} /* end for (vecstr_get) */
	if ((rs >= 0) && (rs1 != rsn)) rs = rs1 ;
	return (rs >= 0) ? c : rs ;
} /* end subroutine (commandment_loadcalls) */

local bool isrequired(int i) noex {
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
} /* end subroutine (isrequired) */


