/* biblepara SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* BIBLEPARA object-load management */
/* version %I% last-modified %G% */

#define	CF_DEBUG	0		/* non-switchable debug print-outs */
#define	CF_LOOKSELF	0		/* try searching "SELF" for SO */

/* revision history:

	- 2008-10-01, David A­D­ Morano
	This module was originally written.

*/

/* Copyright © 2008 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	biblepara

	Description:
	This module implements an interface (a trivial one) that
	allows access to the BIBLEPARA datbase.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* POSIX */
#include	<sys/param.h>		/* POSIX */
#include	<unistd.h>		/* POSIX */
#include	<fcntl.h>		/* POSIX */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstring>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<vecstr.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */

#include	"biblepara.h"
#include	"bibleparas.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |memclear(3u)| */

/* local defines */

#define	BIBLEPARA_DEFENTS	(44 * 1000)

#define	BIBLEPARA_MODBNAME	"bibleparas"
#define	BIBLEPARA_OBJNAME	"bibleparas"

#ifndef	SYMNAMELEN
#define	SYMNAMELEN	60
#endif

#define	BVS_Q			BIBLEPARAS_CITE
#define	BVS_C			BIBLEPARAS_CUR
#define	BVS_I			BIBLEPARAS_INFO


/* external subroutines */


/* local structures */


/* forward references */

local int	biblepara_objloadbegin(BIBLEPARA *,cchar *,cchar *) ;
local int	biblepara_objloadend(BIBLEPARA *) ;
local int	biblepara_loadcalls(BIBLEPARA *,cchar *) ;

local int	isrequired(int) ;


/* external variables */


/* local variables */

enum subs {
	sub_open,
	sub_count,
	sub_ispara,
	sub_curbegin,
	sub_curenum,
	sub_curend,
	sub_audit,
	sub_info,
	sub_close,
	sub_overlast
} ; /* end enum */

constexpr cpcchar	subnames[] = {
	"open",
	"count",
	"ispara",
	"curbegin",
	"curenum",
	"curend",
	"audit",
	"info",
	"close",
	nullptr
} ; /* end array (subnames) */


/* exported variables */


/* exported subroutines */

int biblepara_open(BIBLEPARA *op,cchar *pr,cchar *dbname) noex {
	int		rs ;
	cchar		*objname = BIBLEPARA_OBJNAME ;

	if (op == nullptr) return SR_FAULT ;
	if (pr == nullptr) return SR_FAULT ;

	if (pr[0] == '\0') return SR_INVALID ;

	memclear(op) ;
	if ((rs = biblepara_objloadbegin(op,pr,objname)) >= 0) {
	    if ((rs = (*op->call.open)(op->obj,pr,dbname)) >= 0) {
		op->magval = BIBLEPARA_MAGIC ;
	    }
	    if (rs < 0)
		biblepara_objloadend(op) ;
	} /* end if (biblepara_objloadbegin) */

#if	CF_DEBUG
	debugprintf("biblepara_open: ret rs=%d\n",rs) ;
#endif

	return rs ;
} /* end subroutine (biblepara_open) */

int biblepara_close(BIBLEPARA *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;

	if (op == nullptr) return SR_FAULT ;

	if (op->magval != BIBLEPARA_MAGIC) return SR_NOTOPEN ;

	rs1 = (*op->call.close)(op->obj) ;
	if (rs >= 0) rs = rs1 ;

#if	CF_DEBUG
	debugprintf("biblepara_close: OBJ_close() rs=%d\n",rs) ;
#endif

	rs1 = biblepara_objloadend(op) ;
	if (rs >= 0) rs = rs1 ;

#if	CF_DEBUG
	debugprintf("biblepara_close: _objloadend() rs=%d\n",rs) ;
#endif

	op->magval = 0 ;
	return rs ;
} /* end subroutine (biblepara_close) */

int biblepara_count(BIBLEPARA *op) {
	int		rs = SR_NOSYS ;

	if (op == nullptr) return SR_FAULT ;

	if (op->magval != BIBLEPARA_MAGIC) return SR_NOTOPEN ;

	if (op->call.count != nullptr)
	    rs = (*op->call.count)(op->obj) ;

	return rs ;
} /* end subroutine (biblepara_count) */

int biblepara_audit(BIBLEPARA *op) {
	int		rs = SR_NOSYS ;

	if (op == nullptr) return SR_FAULT ;

	if (op->magval != BIBLEPARA_MAGIC) return SR_NOTOPEN ;

	if (op->call.audit != nullptr)
	    rs = (*op->call.audit)(op->obj) ;

	return rs ;
} /* end subroutine (biblepara_audit) */

/* get a string by its index */
int biblepara_ispara(BIBLEPARA *op,BIBLEPARA_CITE *qp) {
	BIBLEPARAS_CITE	sq ;
	int		rs ;

	if (op == nullptr) return SR_FAULT ;
	if (qp == nullptr) return SR_FAULT ;

	if (op->magval != BIBLEPARA_MAGIC) return SR_NOTOPEN ;

#if	CF_DEBUG
	debugprintf("biblepara_ispara: q=%u:%u:%u\n",
		qp->b,qp->c,qp->v) ;
#endif

	sq.b = qp->b ;
	sq.c = qp->c ;
	sq.v = qp->v ;
	rs = (*op->call.ispara)(op->obj,&sq) ;

#if	CF_DEBUG
	debugprintf("biblepara_ispara: ret rs=%d\n",rs) ;
#endif

	return rs ;
} /* end subroutine (biblepara_ispara) */

int biblepara_curbegin(BIBLEPARA *op,BIBLEPARA_CUR *curp) {
	int		rs = SR_OK ;

	if (op == nullptr) return SR_FAULT ;
	if (curp == nullptr) return SR_FAULT ;

	if (op->magval != BIBLEPARA_MAGIC) return SR_NOTOPEN ;

	if (op->call.curbegin != nullptr) {
	    void	*p ;
	    if ((rs = uc_malloc(op->cursize,&p)) >= 0) {
		curp->scp = p ;
	        if ((rs = (*op->call.curbegin)(op->obj,curp->scp)) >= 0) {
		    curp->magval = BIBLEPARA_MAGIC ;
		}
		if (rs < 0) {
		    uc_free(curp->scp) ;
		    curp->scp = nullptr ;
		}
	    } /* end if (m-a) */
	    if (rs < 0)
		memset(curp,0,sizeof(BIBLEPARA_CUR)) ;
	} /* end if (curbegin) */

	return rs ;
} /* end subroutine (biblepara_curbegin) */

int biblepara_curend(BIBLEPARA *op,BIBLEPARA_CUR *curp) {
	int		rs = SR_OK ;
	int		rs1 ;

	if (op == nullptr) return SR_FAULT ;
	if (curp == nullptr) return SR_FAULT ;

	if (op->magval != BIBLEPARA_MAGIC) return SR_NOTOPEN ;
	if (curp->magval != BIBLEPARA_MAGIC) return SR_NOTOPEN ;

	if (curp->scp == nullptr) return SR_NOTSOCK ;

	if (op->call.curend != nullptr) {
	    rs1 = (*op->call.curend)(op->obj,curp->scp) ;
	    if (rs >= 0) rs = rs1 ;
	}

	rs1 = uc_free(curp->scp) ;
	if (rs >= 0) rs = rs1 ;
	curp->scp = nullptr ;

	curp->magval = 0 ;
	return rs ;
} /* end subroutine (biblepara_curend) */

/* enumerate entries */
int biblepara_curenum(BIBLEPARA *op,BIBLEPARA_CUR *curp,BIBLEPARA_CITE *qp) {
	BIBLEPARAS_CITE	sq ;
	int		rs = SR_NOSYS ;

	if (op == nullptr) return SR_FAULT ;
	if (curp == nullptr) return SR_FAULT ;

	if (op->magval != BIBLEPARA_MAGIC) return SR_NOTOPEN ;
	if (curp->magval != BIBLEPARA_MAGIC) return SR_NOTOPEN ;

	if (op->call.enumerate != nullptr) {
	    if ((rs = (*op->call.enumerate)(op->obj,curp->scp,&sq)) >= 0) {
	        qp->b = sq.b ;
	        qp->c = sq.c ;
	        qp->v = sq.v ;
	    }
	}

#if	CF_DEBUG
	debugprintf("biblepara_enum: ret rs=%d\n",rs) ;
#endif

	return rs ;
} /* end subroutine (biblepara_curenum) */

int biblepara_info(BIBLEPARA *op,BIBLEPARA_INFO *ip) {
	BIBLEPARAS_INFO	bi ;
	int		rs = SR_NOSYS ;
	int		nverses = 0 ;

	if (op == nullptr) return SR_FAULT ;

	if (op->magval != BIBLEPARA_MAGIC) return SR_NOTOPEN ;

	if (ip != nullptr)
	    memset(ip,0,sizeof(BIBLEPARAS_INFO)) ;

	if (op->call.info != nullptr)
	    rs = (*op->call.info)(op->obj,&bi) ;

	nverses = bi.nverses ;
	if (ip != nullptr) {
	    memset(ip,0,sizeof(BIBLEPARA_INFO)) ;
	    ip->dbtime = bi.dbtime ;
	    ip->vitime = bi.vitime ;
	    ip->maxbook = bi.maxbook ;
	    ip->maxchapter = bi.maxchapter ;
	    ip->nverses = bi.nverses ;
	    ip->nzverses = bi.nzverses ;
	}

	return (rs >= 0) ? nverses : rs ;
} /* end subroutine (biblepara_info) */


/* private subroutines */

/* find and load the DB-access object */
local int biblepara_objloadbegin(BIBLEPARA *op,cchar *pr,cchar *objname) {
	MODLOAD		*lp = &op->loader ;
	VECSTR		syms ;
	cint	n = nelem(subs) ;
	int		rs ;
	int		rs1 ;
	int		opts ;

#if	CF_DEBUG
	debugprintf("biblepara_objloadbegin: pr=%s\n",pr) ;
	debugprintf("biblepara_objloadbegin: objname=%s\n",objname) ;
#endif

	opts = vecstrm.compact ;
	if ((rs = vecstr_start(&syms,n,opts)) >= 0) {
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
		cchar	**sv ;
	        if ((rs = vecstr_getvec(&syms,&sv)) >= 0) {
	            cchar	*modbname = BIBLEPARA_MODBNAME ;
	            opts = (MODLOAD_OLIBVAR | MODLOAD_OSDIRS) ;
	            rs = modload_open(lp,pr,modbname,objname,opts,sv) ;
		    f_modload = (rs >= 0) ;
		}
	    }

	    rs1 = vecstr_finish(&syms) ;
	    if (rs >= 0) rs = rs1 ;
	    if ((rs < 0) && f_modload)
		modload_close(lp) ;
	} /* end if (allocation) */

#if	CF_DEBUG
	debugprintf("biblepara_objloadbegin: modload_open() rs=%d\n",rs) ;
#endif

	if (rs >= 0) {
	    int		mv[2] ;
	    if ((rs = modload_getmva(lp,mv,2)) >= 0) {
		void	*p ;
		op->objsize = mv[0] ;
		op->cursize = mv[1] ;
		if ((rs = uc_malloc(op->objsize,&p)) >= 0) {
		    op->obj = p ;
		    rs = biblepara_loadcalls(op,objname) ;
		    if (rs < 0) {
			uc_free(op->obj) ;
			op->obj = nullptr ;
		    }
		} /* end if (memory-allocation) */
	    } /* end if (getmva) */
	    if (rs < 0)
		modload_close(lp) ;
	} /* end if (ok) */

	return rs ;
} /* end subroutine (biblepara_objloadbegin) */

local int biblepara_objloadend(BIBLEPARA *op) {
	int		rs = SR_OK ;
	int		rs1 ;

	if (op->obj != nullptr) {
	    rs1 = uc_free(op->obj) ;
	    if (rs >= 0) rs = rs1 ;
	    op->obj = nullptr ;
	}

	rs1 = modload_close(&op->loader) ;
	if (rs >= 0) rs = rs1 ;

	return rs ;
} /* end subroutine (biblepara_objloadend) */

local int biblepara_loadcalls(BIBLEPARA *op,cchar *objname) {
	MODLOAD		*lp = &op->loader ;
	cint	nlen = SYMNAMELEN ;
	int		rs = SR_OK ;
	int		i ;
	int		c = 0 ;
	char		nbuf[SYMNAMELEN + 1] ;
	cvoid	*snp ;

	for (i = 0 ; subs[i] != nullptr ; i += 1) {

	    if ((rs = sncpy3(nbuf,nlen,objname,"_",subs[i])) >= 0) {
	         if ((rs = modload_getsym(lp,nbuf,&snp)) == SR_NOTFOUND) {
		     snp = nullptr ;
		     if (! isrequired(i)) rs = SR_OK ;
		}
	    }

	    if (rs < 0) break ;

#if	CF_DEBUG
	    debugprintf("biblepara_loadcalls: call=%s %c\n",
		subs[i],
		((snp != nullptr) ? 'Y' : 'N')) ;
#endif

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

		case sub_ispara:
		    op->call.ispara = (int (*)(void *,BVS_Q *)) snp ;
		    break ;

		case sub_curbegin:
		    op->call.curbegin = 
			(int (*)(void *,BVS_C *)) snp ;
		    break ;

		case sub_curenum:
		    op->call.enumerate = 
			(int (*)(void *,BVS_C *,BVS_Q *)) snp ;
		    break ;

		case sub_curend:
		    op->call.curend= 
			(int (*)(void *,BVS_C *)) snp ;
		    break ;

		case sub_audit:
		    op->call.audit = (int (*)(void *)) snp ;
		    break ;

		case sub_info:
		    op->call.info = (int (*)(void *,BVS_I *)) snp ;
		    break ;

		case sub_close:
		    op->call.close = (int (*)(void *)) snp ;
		    break ;

		} /* end switch */

	    } /* end if (it had the call) */

	} /* end for (subs) */

	return (rs >= 0) ? c : rs ;
} /* end subroutine (biblepara_loadcalls) */

local bool isrequired(int i) noex {
	bool	f = false ;
	switch (i) {
	case sub_open:
	case sub_ispara:
	case sub_close:
	    f = true ;
	    break ;
	} /* end switch */
	return f ;
} /* end subroutine (isrequired) */


