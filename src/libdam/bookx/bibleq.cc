/* bibleq SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* BIBLEQ object-load management */
/* version %I% last-modified %G% */

#define	CF_DEBUGS	0		/* non-switchable debug print-outs */
#define	CF_LOOKOTHER	0		/* look elsewhere */

/* revision history:

	- 2008-10-01, David A­D­ Morano
	This module was originally written.

*/

/* Copyright © 2008 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This module implements an interface (a trivial one) that
	provides access to the BIBLEQ object (which is dynamically
	loaded).

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
#include	<vecstr.h>		/* LIBUC  */
#include	<localmisc.h>		/* LIBU */

#include	"bibleq.h"
#include	"bibleqs.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |memclear(3u)| */

/* local defines */

#define	BIBLEQ_MODBNAME	"bibleqs"
#define	BIBLEQ_OBJNAME	"bibleqs"

#ifndef	SYMNAMELEN
#define	SYMNAMELEN	60
#endif

#undef	TIS_CUR
#define	TIS_CUR		BIBLEQS_CUR
#undef	TIS_CITE
#define	TIS_CITE	BIBLEQS_CITE


/* external subroutines */


/* local structures */


/* forward references */

local int	bibleq_objloadbegin(BIBLEQ *,cchar *,cchar *) ;
local int	bibleq_objloadend(BIBLEQ *) ;
local int	bibleq_loadcalls(BIBLEQ *,cchar *) ;

local bool	isrequired(int) noex ;


/* external variables */


/* local variables */

enum subs {
	sub_open,
	sub_count,
	sub_curbegin,
	sub_lookup,
	sub_read,
	sub_curend,
	sub_audit,
	sub_close,
	sub_overlast
} ; /* end enum */

constexpr cpcchar	subnames[] = {
	"open",
	"count",
	"curbegin",
	"lookup",
	"read",
	"curend",
	"audit",
	"close",
	nullptr
} ; /* end array (subnames) */


/* exported variables */


/* exported subroutines */

int bibleq_open(BIBLEQ *op,cchar *pr,cchar *dbname) noex {
	int		rs ;
	cchar	*objname = BIBLEQ_OBJNAME ;

	if (op == nullptr) return SR_FAULT ;
	if (pr == nullptr) return SR_FAULT ;
	if (dbname == nullptr) return SR_FAULT ;

	if (pr[0] == '\0') return SR_INVALID ;

#ifdef	COMMENT /* we let both nullptr and NIL go down */
	if (dbname[0] == '\0')
	    return SR_INVALID ;
#endif

	memclear(op) ;
	if ((rs = bibleq_objloadbegin(op,pr,objname)) >= 0) {
	    if ((rs = (*op->call.open)(op->obj,pr,dbname)) >= 0) {
		op->magval = BIBLEQ_MAGIC ;
	    }
	    if (rs < 0)
		bibleq_objloadend(op) ;
	} /* end if (objload-begin) */

#if	CF_DEBUGS
	debugprintf("bibleq_open: ret rs=%d\n",rs) ;
#endif

	return rs ;
} /* end subroutine (bibleq_open) */

int bibleq_close(BIBLEQ *op) {
	int		rs = SR_OK ;
	int		rs1 ;

	if (op == nullptr) return SR_FAULT ;

	if (op->magval != BIBLEQ_MAGIC) return SR_NOTOPEN ;

	rs1 = (*op->call.close)(op->obj) ;
	if (rs >= 0) rs = rs1 ;

	rs1 = bibleq_objloadend(op) ;
	if (rs >= 0) rs = rs1 ;

	op->magval = 0 ;
	return rs ;
} /* end subroutine (bibleq_close) */

int bibleq_audit(BIBLEQ *op) {
	int		rs = SR_NOSYS ;

	if (op == nullptr) return SR_FAULT ;

	if (op->magval != BIBLEQ_MAGIC) return SR_NOTOPEN ;

	if (op->call.audit != nullptr) {
	    rs = (*op->call.audit)(op->obj) ;
	}

	return rs ;
} /* end subroutine (bibleq_audit) */

int bibleq_count(BIBLEQ *op) {
	int		rs = SR_NOSYS ;

	if (op == nullptr) return SR_FAULT ;

	if (op->magval != BIBLEQ_MAGIC) return SR_NOTOPEN ;

	if (op->call.count != nullptr) {
	    rs = (*op->call.count)(op->obj) ;
	}

	return rs ;
} /* end subroutine (bibleq_count) */

int bibleq_curbegin(BIBLEQ *op,BIBLEQ_CUR *curp) noex {
	int		rs = SR_OK ;

	if (op == nullptr) return SR_FAULT ;
	if (curp == nullptr) return SR_FAULT ;

	if (op->magval != BIBLEQ_MAGIC) return SR_NOTOPEN ;

	memclear(curp) ;
	if (op->call.curbegin != nullptr) {
	    void	*p ;
	    if ((rs = uc_malloc(op->cursize,&p)) >= 0) {
		curp->scp = p ;
		if ((rs = (*op->call.curbegin)(op->obj,curp->scp)) >= 0) {
	    	    curp->magval = BIBLEQ_MAGIC ;
		}
		if (rs < 0) {
	    	    uc_free(curp->scp) ;
	    	    curp->scp = nullptr ;
		}
	    } /* end if (memory-allocation) */
	} else {
	    rs = SR_NOTSUP ;
	}

	return rs ;
} /* end subroutine (bibleq_curbegin) */

int bibleq_curend(BIBLEQ *op,BIBLEQ_CUR *curp) {
	int		rs = SR_OK ;
	int		rs1 ;

	if (op == nullptr) return SR_FAULT ;
	if (curp == nullptr) return SR_FAULT ;

	if (op->magval != BIBLEQ_MAGIC) return SR_NOTOPEN ;
	if (curp->magval != BIBLEQ_MAGIC) return SR_NOTOPEN ;

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

	curp->magval = 0 ;
	return rs ;
} /* end subroutine (bibleq_curend) */

/* lookup tags by strings */
int bibleq_lookup(BIBLEQ *op,BIBLEQ_CUR *curp,int qo,cchar **klp) {
	int		rs ;

	if (op == nullptr) return SR_FAULT ;
	if (curp == nullptr) return SR_FAULT ;
	if (klp == nullptr) return SR_FAULT ;

	if (op->magval != BIBLEQ_MAGIC) return SR_NOTOPEN ;
	if (curp->magval != BIBLEQ_MAGIC) return SR_NOTOPEN ;

	rs = (*op->call.lookup)(op->obj,curp->scp,qo,klp) ;

	return rs ;
} /* end subroutine (bibleq_lookup) */

/* enumerate entries */
int bibleq_read(BIBLEQ *op,BIBLEQ_CUR *curp,BIBLEQ_CITE *bcp,
		char *vbuf,int vlen) {
	int		rs = SR_NOSYS ;

	if (op == nullptr) return SR_FAULT ;
	if (curp == nullptr) return SR_FAULT ;
	if (bcp == nullptr) return SR_FAULT ;
	if (vbuf == nullptr) return SR_FAULT ;

	if (op->magval != BIBLEQ_MAGIC) return SR_NOTOPEN ;
	if (curp->magval != BIBLEQ_MAGIC) return SR_NOTOPEN ;

	if (op->call.enumerate != nullptr) {
	    rs = (*op->call.enumerate)(op->obj,curp->scp,bcp,vbuf,vlen) ;
	}

	return rs ;
} /* end subroutine (bibleq_read) */


/* private subroutines */

/* find and load the DB-access object */
local int bibleq_objloadbegin(BIBLEQ *op,cchar *pr,cchar *objname) {
	MODLOAD		*lp = &op->loader ;
	VECSTR		syms ;
	cint	n = nelem(subs) ;
	cint	vo = vecstrm.compact ;
	int		rs ;
	int		rs1 ;

#if	CF_DEBUGS
	debugprintf("bibleq_objloadbegin: pr=%s\n",pr) ;
	debugprintf("bibleq_objloadbegin: objname=%s\n",objname) ;
#endif

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
		cchar	**sv ;
	        if ((rs = vecstr_getvec(&syms,&sv)) >= 0) {
#if	CF_LOOKOTHER
	            cint	mo = (MODLOAD_OLIBVAR | MODLOAD_OSDIRS) ;
#else
	            cint	mo = 0 ;
#endif
	            cchar	*modbname = BIBLEQ_MODBNAME ;
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

#if	CF_DEBUGS
	debugprintf("bibleq_objloadbegin: modload_open() rs=%d\n",rs) ;
#endif

	if (rs >= 0) {
	    int		mv[2] ;
	    if ((rs = modload_getmva(lp,mv,2)) >= 0) {
		void	*p ;
		op->objsize = mv[0] ;
		op->cursize = mv[1] ;
		if ((rs = uc_malloc(op->objsize,&p)) >= 0) {
		    op->obj = p ;
		    rs = bibleq_loadcalls(op,objname) ;
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
} /* end subroutine (bibleq_objloadbegin) */

local int bibleq_objloadend(BIBLEQ *op) {
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
} /* end subroutine (bibleq_objloadend) */

local int bibleq_loadcalls(BIBLEQ *op,cchar *objname) {
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

#if	CF_DEBUGS
	    debugprintf("bibleq_loadcalls: call=%s %c\n",
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
		case sub_curbegin:
		    op->call.curbegin = 
			(int (*)(void *,void *)) snp ;
		    break ;
		case sub_lookup:
		    op->call.lookup = 
			(int (*)(void *,void *,int,cchar **)) snp ;
		    break ;
		case sub_read:
		    op->call.enumerate = 
			(int (*)(void *,void *,TIS_CITE *,char *,int)) snp ;
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
} /* end subroutine (bibleq_loadcalls) */

local bool isrequired(int i) noex {
	bool	f = false ;
	switch (i) {
	case sub_open:
	case sub_curbegin:
	case sub_lookup:
	case sub_read:
	case sub_curend:
	case sub_close:
	    f = true ;
	    break ;
	} /* end switch */
	return f ;
} /* end subroutine (isrequired) */


