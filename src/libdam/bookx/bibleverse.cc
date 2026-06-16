/* bibleverse SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* BV object-load management */
/* version %I% last-modified %G% */

#define	CF_DEBUGS	0		/* non-switchable debug print-outs */

/* revision history:

	- 2008-10-01, David A­D­ Morano
	This module was originally written.

*/

/* Copyright © 2008 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Description:
	This module implements an interface (a trivial one) that
	allows access to the BV datbase.

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
#include	<ucmem.h>		/* LIBUC */
#include	<vecstr.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */

#include	"bibleverse.h"
#include	"bibleverses.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |memclear(3u)| */

/* local defines */

#define	BIBLEVERSE_DEFENTS	(44 * 1000)
#define	BIBLEVERSE_MODBNAME	"bibleverses"
#define	BIBLEVERSE_OBJNAME	"bibleverses"

#ifndef	SYMNAMELEN
#define	SYMNAMELEN	60
#endif

#define	BV		bibleverse
#define	BV_CUR		bibleverse_cur
#define	BV_CITE		bibleverse_cite
#define	BV_Q		bibleverse_q
#define	BV_INFO		bibleverse_info
#define	BV_CA		bibleverse_ca

#define	BVS_Q		bibleverses_cite
#define	BVS_C		bibleverses_cur
#define	BVS_I		bibleverses_info


/* imported namespaces */

using libuc::mem ;			/* variable */


/* local typedefs */



/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

local int	bibleverse_objloadbegin(BV *,cchar *,cchar *) noex ;
local int	bibleverse_objloadbeginer(BV *op,cchar *,cchar *) noex ;
local int	bibleverse_objloadend(BV *) noex ;
local int	bibleverse_loadcalls(BV *,cchar *) noex ;

local bool	isrequired(int) noex ;


/* external variables */


/* local variables */

enum subs {
	sub_open,
	sub_count,
	sub_read,
	sub_get,
	sub_curbegin,
	sub_curenum,
	sub_curend,
	sub_audit,
	sub_info,
	sub_chapters,
	sub_close,
	sub_overlast
} ; /* end enum (subs) */

constexpr cpcchar	subnames[] = {
	"open",
	"count",
	"read",
	"get",
	"curbegin",
	"curenum",
	"curend",
	"audit",
	"info",
	"chapters",
	"close",
	nullptr
} ; /* end array (subs) */


/* exported variables */


/* exported subroutines */

int bibleverse_open(BV *op,cchar *pr,cchar *dbname) noex {
	int		rs ;
	cchar		*objname = BIBLEVERSE_OBJNAME ;

	if (op == nullptr) return SR_FAULT ;
	if (pr == nullptr) return SR_FAULT ;

	if (pr[0] == '\0') return SR_INVALID ;

	memclear(op) ;

	if ((rs = bibleverse_objloadbegin(op,pr,objname)) >= 0) {
	    if ((rs = (*op->call.open)(op->obj,pr,dbname)) >= 0) {
		op->magval = BIBLEVERSE_MAGIC ;
	    }
	    if (rs < 0) {
		bibleverse_objloadend(op) ;
	    } /* end if (error) */
	} /* end if (objload-begin) */

#if	CF_DEBUGS
	debugprintf("bibleverse_open: ret rs=%d\n",rs) ;
#endif

	return rs ;
}
/* end subroutine (bibleverse_open) */

/* finish the entire vector string data structure object */
int bibleverse_close(BV *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;

#if	CF_DEBUGS
	debugprintf("bibleverse_close: ent\n") ;
#endif

	if (op == nullptr) return SR_FAULT ;

	if (op->magval != BIBLEVERSE_MAGIC) return SR_NOTOPEN ;
	{
	rs1 = (*op->call.close)(op->obj) ;
	if (rs >= 0) rs = rs1 ;
	}
	{
	rs1 = bibleverse_objloadend(op) ;
	if (rs >= 0) rs = rs1 ;
	}

#if	CF_DEBUGS
	debugprintf("bibleverse_close: ret rs=%d\n",rs) ;
#endif

	op->magval = 0 ;
	return rs ;
}
/* end subroutine (bibleverse_close) */

int bibleverse_count(BV *op) noex {
	int		rs = SR_NOSYS ;

	if (op == nullptr) return SR_FAULT ;

	if (op->magval != BIBLEVERSE_MAGIC) return SR_NOTOPEN ;

	if (op->call.count != nullptr) {
	    rs = (*op->call.count)(op->obj) ;
	}

	return rs ;
}
/* end subroutine (bibleverse_count) */

int bibleverse_audit(BV *op) noex {
	int		rs = SR_NOSYS ;

	if (op == nullptr) return SR_FAULT ;
	if (op->magval != BIBLEVERSE_MAGIC) return SR_NOTOPEN ;

	if (op->call.audit != nullptr) {
	    rs = (*op->call.audit)(op->obj) ;
	}

	return rs ;
}
/* end subroutine (bibleverse_audit) */

int bibleverse_read(BV *op,char *vbuf,int vlen,BIBLEVERSE_Q *qp) noex {
	BIBLEVERSES_Q	sq ;
	int		rs ;

	if (op == nullptr) return SR_FAULT ;
	if (qp == nullptr) return SR_FAULT ;

	if (op->magval != BIBLEVERSE_MAGIC) return SR_NOTOPEN ;

#if	CF_DEBUGS
	debugprintf("bibleverse_read: ent q=%u:%u:%u\n",
		qp->b,qp->c,qp->v) ;
#endif

	sq.b = qp->b ;
	sq.c = qp->c ;
	sq.v = qp->v ;
	rs = (*op->call.read)(op->obj,vbuf,vlen,&sq) ;

#if	CF_DEBUGS
	debugprintf("bibleverse_read: ret rs=%d\n",rs) ;
#endif

	return rs ;
}
/* end subroutine (bibleverse_read) */

int bibleverse_get(BV *op,BIBLEVERSE_Q *qp,char *vbuf,int vlen) noex {
	return bibleverse_read(op,vbuf,vlen,qp) ;
}
/* end subroutine (bibleverse_get) */

int bibleverse_curbegin(BV *op,BV_CUR *curp) noex {
	int		rs = SR_OK ;

	if (op == nullptr) return SR_FAULT ;
	if (curp == nullptr) return SR_FAULT ;

	if (op->magval != BIBLEVERSE_MAGIC) return SR_NOTOPEN ;

	if (op->call.curbegin != nullptr) {
	    if (void *p ; (rs = mem.mall(op->cursize,&p)) >= 0) {
		curp->scp = p ;
		if ((rs = (*op->call.curbegin)(op->obj,curp->scp)) >= 0) {
		     curp->magval = BIBLEVERSE_MAGIC ;
		}
	        if (rs < 0) {
		    mem.free(curp->scp) ;
		    curp->scp = nullptr ;
	        } /* end if (error) */
	    } /* end if (memory-acquire) */
	} else {
	    rs = SR_NOTSOCK ;
	}
	if (rs < 0) {
	    memclear(curp) ;
	} /* end if (error) */

	return rs ;
}
/* end subroutine (bibleverse_curbegin) */

int bibleverse_curend(BV *op,BV_CUR *curp) noex {
	int		rs = SR_OK ;
	int		rs1 ;

	if (op == nullptr) return SR_FAULT ;
	if (curp == nullptr) return SR_FAULT ;

	if (op->magval != BIBLEVERSE_MAGIC) return SR_NOTOPEN ;
	if (curp->magval != BIBLEVERSE_MAGIC) return SR_NOTOPEN ;

	if (curp->scp != nullptr) {
	    if (op->call.curend != nullptr) {
	        rs1 = (*op->call.curend)(op->obj,curp->scp) ;
		if (rs >= 0) rs = rs1 ;
	    }
	    rs1 = mem.free(curp->scp) ;
	    if (rs >= 0) rs = rs1 ;
	    curp->scp = nullptr ;
	} else
	    return SR_NOTSUP ;

	curp->magval = 0 ;
	return rs ;
}
/* end subroutine (bibleverse_curend) */

/* enumerate entries */
int bibleverse_curenum(BV *op,BV_CUR *curp,BV_CUTE *qp,
		char *vbuf,int vlen) noex {
	int		rs = SR_NOSYS ;

	if (op == nullptr) return SR_FAULT ;
	if (curp == nullptr) return SR_FAULT ;
	if (vbuf == nullptr) return SR_FAULT ;

	if (op->magval != BIBLEVERSE_MAGIC) return SR_NOTOPEN ;
	if (curp->magval != BIBLEVERSE_MAGIC) return SR_NOTOPEN ;

	if (op->call.enumerate != nullptr) {
	    BIBLEVERSES_Q	sq ;
	    rs = (*op->call.enumerate)(op->obj,curp->scp,&sq,vbuf,vlen) ;
	    if (rs >= 0) {
	        qp->b = sq.b ;
	        qp->c = sq.c ;
	        qp->v = sq.v ;
	    }
	}

#if	CF_DEBUGS
	debugprintf("bibleverse_enum: ret rs=%d\n",rs) ;
#endif

	return rs ;
}
/* end subroutine (bibleverse_curenum) */

int bibleverse_info(BV *op,BIBLEVERSE_INFO *ip) noex {
	BIBLEVERSES_I	bi ;
	int		rs = SR_NOSYS ;
	int		nverses = 0 ;

	if (op == nullptr) return SR_FAULT ;

	if (op->magval != BIBLEVERSE_MAGIC) return SR_NOTOPEN ;

#if	CF_DEBUGS
	debugprintf("bibleverse_info: ent\n") ;
#endif

	if (ip) {
	    memclear(ip) ;
	}
	if (op->call.info) {
	    if ((rs = (*op->call.info)(op->obj,&bi)) >= 0) {
	        nverses = bi.nverses ;
	        if (ip) {
	            memclear(ip) ;
	            ip->dbtime = bi.dbtime ;
	            ip->vitime = bi.vitime ;
	            ip->maxbook = bi.maxbook ;
	            ip->maxchapter = bi.maxchapter ;
	            ip->nverses = bi.nverses ;
	            ip->nzverses = bi.nzverses ;
		} /* end if */
	    } /* end if */
	} /* end if */

#if	CF_DEBUGS
	debugprintf("bibleverse_info: ret rs=%d nv=%u\n",rs,nverses) ;
#endif

	return (rs >= 0) ? nverses : rs ;
}
/* end subroutine (bibleverse_info) */

int bibleverse_chapters(BV *op,int book,uchar *ap,int al) noex {
	int		rs = SR_NOSYS ;

	if (op == nullptr) return SR_FAULT ;

	if (op->magval != BIBLEVERSE_MAGIC) return SR_NOTOPEN ;

	if (op->call.chapters != nullptr) {
	    rs = (*op->call.chapters)(op->obj,book,ap,al) ;
	}

	return rs ;
}
/* end subroutine (bibleverse_chapters) */


/* private subroutines */

/* find and load the DB-access object */
local int bibleverse_objloadbegin(BV *op,cchar *pr,cchar *objname) noex {
	int		rs ;

	if ((rs = bibleverse_objloadbeginer(op,pr,objname)) >= 0) {
	    modload	*lp = &op->loader ;
	    int		mv[2] ;
	    if ((rs = modload_getmva(lp,mv,2)) >= 0) {
		void	*p ;
		op->objsize = mv[0] ;
		op->cursize = mv[1] ;
		if ((rs = mem.mall(op->objsize,&p)) >= 0) {
		    op->obj = p ;
		    rs = bibleverse_loadcalls(op,objname) ;
		    if (rs < 0) {
			mem.free(op->obj) ;
			op->obj = nullptr ;
		    } /* end if (error) */
		} /* end if (memory-acquire) */
	    } /* end if (getmva) */
	    if (rs < 0) {
		modload_close(lp) ;
	    } /* end if (error) */
	} /* end if (ok) */

	return rs ;
} /* end subroutine (bibleverse_objloadbegin) */

local int bibleverse_objloadbeginer(BV *op,cchar *pr,cchar *objname) noex {
	modload		*lp = &op->loader ;
	VECSTR		syms ;
	cint	n = nelem(subnames) ;
	int		rs ;
	int		rs1 ;
	int		opts ;

#if	CF_DEBUGS
	debugprintf("bibleverse_objloadbegin: pr=%s\n",pr) ;
	debugprintf("bibleverse_objloadbegin: objname=%s\n",objname) ;
#endif

	opts = vecstrm.compact ;
	if ((rs = vecstr_start(&syms,n,opts)) >= 0) {
	    cint	nlen = SYMNAMELEN ;
	    int		i ;
	    int		f_modload = false ;
	    char	nbuf[SYMNAMELEN + 1] ;

	    for (i = 0 ; (i < n) && subsnames[i] ; i += 1) {
	        if (isrequired(i)) {
		    cc *sn = subnames[i] ;
	            if ((rs = sncpy3(nbuf,nlen,objname,"_",sn)) >= 0) {
			rs = vecstr_add(&syms,nbuf,rs) ;
		    }
		}
		if (rs < 0) break ;
	    } /* end for */

	    if (rs >= 0) {
		cchar	**sv ;
	        if ((rs = vecstr_getvec(&syms,&sv)) >= 0) {
	            cchar	*modbname = BIBLEVERSE_MODBNAME ;
	            opts = (modload_OLIBVAR | modload_OSDIRS) ;
	            rs = modload_open(lp,pr,modbname,objname,opts,sv) ;
		    f_modload = (rs >= 0)  ;
		}
	    }

	    rs1 = vecstr_finish(&syms) ;
	    if (rs >= 0) rs = rs1 ;
	    if ((rs < 0) && f_modload) {
		modload_close(lp) ;
	    } /* end if (error) */
	} /* end if (allocation) */

	return rs ;
} /* end subroutine (bibleverse_objloadbeginer) */

local int bibleverse_objloadend(BV *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (op->obj) {
	    rs1 = mem.free(op->obj) ;
	    if (rs >= 0) rs = rs1 ;
	    op->obj = nullptr ;
	} /* end if (memory-release) */
	{
	    rs1 = modload_close(&op->loader) ;
	    if (rs >= 0) rs = rs1 ;
	}
	return rs ;
} /* end subroutine (bibleverse_objloadend) */

local int bibleverse_loadcalls(BV *op,cchar *objname) noex {
	modload		*lp = &op->loader ;
	cint		nlen = SYMNAMELEN ;
	int		rs = SR_OK ;
	int		i ;
	int		c = 0 ;
	char		nbuf[SYMNAMELEN + 1] ;
	cvoid		*snp = nullptr ;

	for (i = 0 ; subs[i] != nullptr ; i += 1) {

	    if ((rs = sncpy3(nbuf,nlen,objname,"_",subs[i])) >= 0) {
	         if ((rs = modload_getsym(lp,nbuf,&snp)) == SR_NOTFOUND) {
		     snp = nullptr ;
		     if (! isrequired(i)) rs = SR_OK ;
		}
	    }

	    if (rs < 0) break ;

#if	CF_DEBUGS
	    debugprintf("bibleverse_loadcalls: call=%s %c\n",
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

		case sub_read:
		    op->call.read = (int (*)(void *,char *,int,BVS_Q *)) snp ;
		    break ;

		case sub_get:
		    op->call.get = (int (*)(void *,BVS_Q *,char *,int)) snp ;
		    break ;

		case sub_curbegin:
		    op->call.curbegin = 
			(int (*)(void *,BVS_C *)) snp ;
		    break ;

		case sub_curenum:
		    op->call.enumerate = 
			(int (*)(void *,BVS_C *,BVS_Q *,char *,int)) snp ;
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

		case sub_chapters:
		    op->call.chapters = (int (*)(void *,int,uchar *,int)) snp ;
		    break ;

		case sub_close:
		    op->call.close = (int (*)(void *)) snp ;
		    break ;

		} /* end switch */

	    } /* end if (it had the call) */

	} /* end for (subs) */

	return (rs >= 0) ? c : rs ;
} /* end subroutine (bibleverse_loadcalls) */

local bool isrequired(int i) noex {
	bool		f = false ;
	switch (i) {
	case sub_open:
	case sub_read:
	case sub_get:
	case sub_close:
	    f = true ;
	    break ;
	} /* end switch */
	return f ;
} /* end subroutine (isrequired) */


