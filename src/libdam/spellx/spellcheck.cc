/* spellcheck SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* load management and interface for the SPELLCHECKS object */
/* version %I% last-modified %G% */

#define	CF_DEBUG	0		/* non-switchable debug print-outs */

/* revision history:

	- 2008-10-01, David A­D­ Morano
	This module was originally written.

*/

/* Copyright © 2008 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	spellcheck

	Description:
	This module implements an interface (a trivial one) that
	provides access to the SPELLCHECKS object (which is dynamically
	loaded).

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* POSIX® */
#include	<sys/param.h>		/* POSIX® */
#include	<sys/stat.h>		/* POSIX® */
#include	<dlfcn.h>		/* POSIX® */
#include	<unistd.h>		/* POSIX® */
#include	<fcntl.h>		/* POSIX® */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstring>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<vecstr.h>		/* LIBUC */
#include	<modload.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */

#include	"spellcheck.h"
#include	"spellchecks.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| */

/* local defines */

#define	SPELLCHECK_MODBNAME	"spellchecks"
#define	SPELLCHECK_OBJNAME	"spellchecks"

#define	SCS_C		SPELLCHECKS_CUR

#define	LIBCNAME	"lib"

#ifndef	VARLIBPATH
#define	VARLIBPATH	"LD_LIBRARY_PATH"
#endif

#ifndef	SYMNAMELEN
#define	SYMNAMELEN	60
#endif

#ifndef	CF_DEBUG
#define	CF_DEBUG	0		/* non-switchable debug print-outs */
#endif


/* external subroutines */


/* local structures */


/* forward references */

local int	spellcheck_objloadbegin(SPELLCHECK *,cchar *,
			cchar *) noex ;
local int	spellcheck_objloadend(SPELLCHECK *) noex ;
local int	spellcheck_loadcalls(SPELLCHECK *,cchar *) noex ;

local bool	isrequired(int) noex ;


/* external variables */


/* local variables */

enum subs {
	sub_start,
	sub_count,
	sub_look,
	sub_looks,
	sub_curbegin,
	sub_curenum,
	sub_curend,
	sub_audit,
	sub_finish,
	sub_overlast
} ; /* end enum */

constexpr cpcchar	subnames[] = {
	"start",
	"count",
	"look",
	"looks",
	"curbegin",
	"curenum",
	"curend",
	"audit",
	"finish",
	nullptr
} ; /* end array */


/* exported variables */


/* exported subroutines */

int spellcheck_start(op,pr,dbname)
SPELLCHECK	*op ;
cchar	pr[] ;
cchar	dbname[] ;
{
	int	rs ;

	cchar	*objname = SPELLCHECK_OBJNAME ;


	if (op == nullptr)
	    return SR_FAULT ;

#if	CF_DEBUG
	debugprintf("spellcheck_open: pr=%s\n",pr) ;
	debugprintf("spellcheck_open: dbname=%s\n",dbname) ;
#endif

	if (pr == nullptr) return SR_FAULT ;
	if (dbname == nullptr) return SR_FAULT ;

	if (pr[0] == '\0') return SR_INVALID ;
	if (dbname[0] == '\0') return SR_INVALID ;

	memclear(op) ;

	if ((rs = spellcheck_objloadbegin(op,pr,objname)) >= 0) {
	    if ((rs = (*op->call.start)(op->obj,pr,dbname)) >= 0) {
		op->magvalg = SPELLCHECK_MAGIC ;
	    }
	    if (rs < 0)
		spellcheck_objloadend(op) ;
	} /* end if (objload-begin) */

#if	CF_DEBUG
	debugprintf("spellcheck_start: ret rs=%d\n",rs) ;
#endif

	return rs ;
}
/* end subroutine (spellcheck_start) */


/* free up the entire vector string data structure object */
int spellcheck_finish(op)
SPELLCHECK	*op ;
{
	int	rs = SR_OK ;
	int	rs1 ;


	if (op == nullptr)
	    return SR_FAULT ;

	if (op->magvalg != SPELLCHECK_MAGIC)
	    return SR_NOTOPEN ;

	rs1 = (*op->call.finish)(op->obj) ;
	if (rs >= 0) rs = rs1 ;

	rs1 = spellcheck_objloadend(op) ;
	if (rs >= 0) rs = rs1 ;

	op->magvalg = 0 ;
	return rs ;
}
/* end subroutine (spellcheck_finish) */


int spellcheck_count(op)
SPELLCHECK	*op ;
{
	int	rs = SR_NOSYS ;


	if (op == nullptr)
	    return SR_FAULT ;

	if (op->magvalg != SPELLCHECK_MAGIC)
	    return SR_NOTOPEN ;

	if (op->call.count != nullptr)
	    rs = (*op->call.count)(op->obj) ;

	return rs ;
}
/* end subroutine (spellcheck_count) */


int spellcheck_look(op,wp,wl)
SPELLCHECK	*op ;
cchar	*wp ;
int		wl ;
{
	int	rs = SR_NOSYS ;

	if (op == nullptr) return SR_FAULT ;
	if (wp == nullptr) return SR_FAULT ;

	if (op->magvalg != SPELLCHECK_MAGIC) return SR_NOTOPEN ;

	if (op->call.look != nullptr) {
	    rs = (*op->call.look)(op->obj,wp,wl) ;
	}

	return rs ;
}
/* end subroutine (spellcheck_look) */


int spellcheck_looks(op,bop,wa,wn)
SPELLCHECK	*op ;
bits		*bop ;
cchar	**wa ;
int		wn ;
{
	int	rs = SR_NOSYS ;

	if (op == nullptr) return SR_FAULT ;
	if (bop == nullptr) return SR_FAULT ;
	if (wa == nullptr) return SR_FAULT ;

	if (op->magvalg != SPELLCHECK_MAGIC) return SR_NOTOPEN ;

	if (op->call.looks != nullptr) {
	    rs = (*op->call.looks)(op->obj,bop,wa,wn) ;
	}

	return rs ;
}
/* end subroutine (spellcheck_looks) */


int spellcheck_curbegin(op,curp)
SPELLCHECK	*op ;
SPELLCHECK_CUR	*curp ;
{
	int	rs = SR_OK ;


	if (op == nullptr) return SR_FAULT ;
	if (curp == nullptr) return SR_FAULT ;

	if (op->magvalg != SPELLCHECK_MAGIC) return SR_NOTOPEN ;

	if (op->call.curbegin != nullptr) {
	    void	*p ;
	    if ((rs = uc_malloc(op->cursize,&p)) >= 0) {
	        curp->scp = p ;
		if ((rs = (*op->call.curbegin)(op->obj,curp->scp)) >= 0) {
		    curp->magvalg = SPELLCHECK_MAGIC ;
		}
	        if (rs < 0) {
		   uc_free(curp->scp) ;
		   curp->scp = nullptr ;
		}
	    } /* end if (memory-allocation) */
	} else
	    rs = SR_NOTSUP ;

	return rs ;
}
/* end subroutine (spellcheck_curbegin) */


int spellcheck_curend(op,curp)
SPELLCHECK	*op ;
SPELLCHECK_CUR	*curp ;
{
	int	rs = SR_OK ;
	int	rs1 ;


	if (op == nullptr) return SR_FAULT ;
	if (curp == nullptr) return SR_FAULT ;

	if (op->magvalg != SPELLCHECK_MAGIC) return SR_NOTOPEN ;
	if (curp->magvalg != SPELLCHECK_MAGIC) return SR_NOTOPEN ;

	if (curp->scp != nullptr) {
	    if (op->call.curend != nullptr) {
	        rs1 = (*op->call.curend)(op->obj,curp->scp) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    rs1 = uc_free(curp->scp) ;
	    if (rs >= 0) rs = rs1 ;
	    curp->scp = nullptr ;
	} else
	    rs = SR_NOTSUP ;

	curp->magvalg = 0 ;
	return rs ;
}
/* end subroutine (spellcheck_curend) */

int spellcheck_curenum(op,curp,vbuf,vlen)
SPELLCHECK	*op ;
SPELLCHECK_CUR	*curp ;
char		vbuf[] ;
int		vlen ;
{
	int	rs = SR_OK ;


	if (op == nullptr) return SR_FAULT ;
	if (curp == nullptr) return SR_FAULT ;
	if (vbuf == nullptr) return SR_FAULT ;

	if (op->magvalg != SPELLCHECK_MAGIC) return SR_NOTOPEN ;
	if (curp->magvalg != SPELLCHECK_MAGIC) return SR_NOTOPEN ;

	if (op->call.enumerate != nullptr) {
	    rs = (*op->call.enumerate)(op->obj,curp->scp,vbuf,vlen) ;
	} else
	    rs = SR_NOTSUP ;

#if	CF_DEBUG
	debugprintf("spellcheck_curenum: ret rs=%d\n",rs) ;
#endif

	return rs ;
}
/* end subroutine (spellcheck_curenum) */


int spellcheck_audit(op)
SPELLCHECK	*op ;
{
	int	rs = SR_NOSYS ;


	if (op == nullptr)
	    return SR_FAULT ;

	if (op->magvalg != SPELLCHECK_MAGIC)
	    return SR_NOTOPEN ;

	if (op->call.audit != nullptr)
	    rs = (*op->call.audit)(op->obj) ;

	return rs ;
} /* end subroutine (spellcheck_audit) */


/* private subroutines */

/* find and load the DB-access object */
local int spellcheck_objloadbegin(op,pr,objname)
SPELLCHECK	*op ;
cchar	*pr ;
cchar	*objname ;
{
	MODLOAD	*lp = &op->loader ;

	VECSTR	syms ;

	int	rs ;
	int	opts ;
	int	snl ;
	int	n ;


#if	CF_DEBUG
	debugprintf("spellcheck_objloadbegin: pr=%s\n",pr) ;
	debugprintf("spellcheck_objloadbegin: objname=%s\n",objname) ;
#endif

	n = nelem(subs) ;
	opts = VECSTR_OCOMPACT ;
	if ((rs = vecstr_start(&syms,n,opts)) >= 0) {
	    int		i ;
	    char	symname[SYMNAMELEN + 1] ;

	    for (i = 0 ; (i < n) && (subnames[i] != nullptr) ; i += 1) {
	        if (isrequired(i)) {
	            rs = sncpy3(symname,SYMNAMELEN,objname,"_",subnames[i]) ;
		    snl = rs ;
		    if (rs >= 0) 
			rs = vecstr_add(&syms,symname,snl) ;
		}
		if (rs < 0) break ;
	    } /* end for */

	    if (rs >= 0) {
		cchar	**sv ;
	        if ((rs = vecstr_getvec(&syms,&sv)) >= 0) {
	            cchar	*modbname = SPELLCHECK_MODBNAME ;
	            opts = (MODLOAD_OLIBVAR | MODLOAD_OPRS | MODLOAD_OSDIRS) ;
	            rs = modload_open(lp,pr,modbname,objname,opts,sv) ;
		}
	    }

	    vecstr_finish(&syms) ;
	} /* end if (allocation) */

#if	CF_DEBUG
	debugprintf("spellcheck_objloadbegin: modload_open() rs=%d\n",rs) ;
#endif

	if (rs >= 0) {
	    int	mv[2] ;
	    if ((rs = modload_getmva(lp,mv,2)) >= 0) {
		int	objsize = rs ;
		char	*p ;
		if ((rs = uc_malloc(objsize,&p)) >= 0) {
		    op->obj = p ;
		    if ((rs = spellcheck_loadcalls(op,objname)) >= 0) {
			op->objsize = mv[0] ;
			op->cursize = mv[1] ;
		    }
		    if (rs < 0) {
	    		uc_free(op->obj) ;
	    		op->obj = nullptr ;
		    }
		} /* end if (memory-allocation) */
	    } /* end if */
	    if (rs < 0) {
		modload_close(lp) ;
	    }
	} /* end if */
	return rs ;
} /* end subroutine (spellcheck_objloadbegin) */

local int spellcheck_objloadend(op)
SPELLCHECK	*op ;
{
	int	rs = SR_OK ;
	int	rs1 ;


	if (op->obj != nullptr) {
	    rs1 = uc_free(op->obj) ;
	    if (rs >= 0) rs = rs1 ;
	    op->obj = nullptr ;
	}

	rs1 = modload_close(&op->loader) ;
	if (rs >= 0) rs = rs1 ;

	return rs ;
}
/* end subroutine (spellcheck_objloadend) */


local int spellcheck_loadcalls(op,objname)
SPELLCHECK	*op ;
cchar	objname[] ;
{
	MODLOAD	*lp = &op->loader ;

	int	rs = SR_OK ;
	int	rs1 ;
	int	i ;
	int	c = 0 ;

	char	symname[SYMNAMELEN + 1] ;

	const void	*snp ;


	for (i = 0 ; subnames[i] != nullptr ; i += 1) {
	    rs = sncpy3(symname,SYMNAMELEN,objname,"_",subnames[i]) ;

#if	CF_DEBUG
	    debugprintf("spellcheck_loadcalls: sncpy3() rs=%d\n",rs) ;
#endif

	    if (rs < 0)
		break ;

#if	CF_DEBUG
	    debugprintf("spellcheck_loadcalls: symname=%s\n",symname) ;
#endif

	    rs1 = modload_getsym(lp,symname,&snp) ;

#if	CF_DEBUG
	    debugprintf("spellcheck_loadcalls: modload_getsym() rs=%d\n",rs1) ;
#endif

	    if (rs1 == SR_NOTFOUND) {
		snp = nullptr ;
		if (isrequired(i))
		    break ;
	    } else
		rs = rs1 ;

	    if (rs < 0)
		break ;

#if	CF_DEBUG
	    debugprintf("spellcheck_loadcalls: call=%s %c\n",
		subnames[i],
		((snp != nullptr) ? 'Y' : 'N')) ;
#endif

	    if (snp != nullptr) {
	        c += 1 ;
		switch (i) {
		case sub_start:
		    op->call.start = 
			(int (*)(void *,cchar *,cchar *)) snp ;
		    break ;

		case sub_count:
		    op->call.count = (int (*)(void *)) snp ;
		    break ;

		case sub_look:
		    op->call.look = 
			(int (*)(void *,cchar *,int)) snp ;
		    break ;

		case sub_looks:
		    op->call.looks = 
			(int (*)(void *,bits *,cchar **,int)) snp ;
		    break ;

		case sub_curbegin:
		    op->call.curbegin = 
			(int (*)(void *,SCS_C *)) snp ;
		    break ;

		case sub_curenum:
		    op->call.enumerate = 
			(int (*)(void *,SCS_C *,char *,int)) snp ;
		    break ;

		case sub_curend:
		    op->call.curend= 
			(int (*)(void *,SCS_C *)) snp ;
		    break ;


		case sub_audit:
		    op->call.audit = (int (*)(void *)) snp ;
		    break ;

		case sub_finish:
		    op->call.finish = (int (*)(void *)) snp ;
		    break ;

		} /* end switch */

	    } /* end if (it had the call) */

	} /* end for (subs) */

#if	CF_DEBUG
	debugprintf("spellcheck_loadcalls: ret rs=%d c=%u\n",rs,c) ;
#endif

	return (rs >= 0) ? c : rs ;
} /* end subroutine (spellcheck_loadcalls) */

local bool isrequired(int i) noex {
	bool	f = false ;
	switch (i) {
	case sub_start:
	case sub_count:
	case sub_look:
	case sub_curbegin:
	case sub_curenum:
	case sub_curend:
	case sub_audit:
	case sub_finish:
	    f = true ;
	    break ;
	} /* end switch */
	return f ;
} /* end subroutine (isrequired) */


