/* mfs-ns SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* MFSNS object-load management */
/* version %I% last-modified %G% */


#define	CF_DEBUGS	0		/* non-switchable debug print-outs */
#define	CF_LOOKOTHER	0		/* look elsewhere */

/* revision history:

	- 2008-10-01, David A­D­ Morano
	This module was originally written.

	= 2017-08-10, David A­D­ Morano
	This subroutine was borrowed to code MFSERVE.

*/

/* Copyright © 2008,2017 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Description:
	This module implements an interface (a trivial one) that
	provides access to the MFSNS object (which is dynamically
	loaded).

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* POSIX® */
#include	<sys/param.h>		/* POSIX® */
#include	<unistd.h>		/* POSIX® */
#include	<fcntl.h>		/* POSIX® */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstring>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<estrings.h>		/* LIBUC */
#include	<vecstr.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */

#include	"mfsns.h"


/* local defines */

#define	MFSNS_MODBNAME	"mfsnso"
#define	MFSNS_OBJNAME	"mfsnso"

#ifndef	SYMNAMELEN
#define	SYMNAMELEN	60
#endif


/* external subroutines */


/* local structures */


/* forward references */

local int	mfsns_objloadbegin(MFSNS *,cchar *,cchar *) noex ;
local int	mfsns_objloadend(MFSNS *) noex ;
local int	mfsns_loadcalls(MFSNS *,cchar *) noex ;

local bool	isrequired(int) noex ;


/* external variables */


/* local variables */

enum subs {
	sub_open,
	sub_setopts,
	sub_get,
	sub_curbegin,
	sub_curenum,
	sub_curend,
	sub_audit,
	sub_close,
	sub_overlast
} ; /* end enum */

constexpr cpcchar	subs[] = {
	"open",
	"setopts",
	"get",
	"curbegin",
	"enum",
	"curend",
	"audit",
	"close",
	nullptr
} ; /* end array */


/* exported variables */


/* exported subroutines */

int mfsns_open(MFSNS *op,cchar *pr) noex {
	int		rs ;
	cchar	*objname = MFSNS_OBJNAME ;

	if (op == nullptr) return SR_FAULT ;
	if (pr == nullptr) return SR_FAULT ;

	if (pr[0] == '\0') return SR_INVALID ;

	memset(op,0,sizeof(MFSNS)) ;

	if ((rs = mfsns_objloadbegin(op,pr,objname)) >= 0) {
	    if ((rs = (*op->call.open)(op->obj,pr)) >= 0) {
		op->magic = MFSNS_MAGIC ;
	    }
	    if (rs < 0)
		mfsns_objloadend(op) ;
	} /* end if (objload-begin) */

#if	CF_DEBUGS
	debugprintf("mfsns_open: ret rs=%d\n",rs) ;
#endif

	return rs ;
}
/* end subroutine (mfsns_open) */


int mfsns_close( MFSNS *op)
{
	int		rs = SR_OK ;
	int		rs1 ;

	if (op == nullptr) return SR_FAULT ;

	if (op->magic != MFSNS_MAGIC) return SR_NOTOPEN ;

	rs1 = (*op->call.close)(op->obj) ;
	if (rs >= 0) rs = rs1 ;

	rs1 = mfsns_objloadend(op) ;
	if (rs >= 0) rs = rs1 ;

	op->magic = 0 ;
	return rs ;
}
/* end subroutine (mfsns_close) */


int mfsns_setopts(MFSNS *op,int opts)
{
	int		rs = SR_NOSYS ;

	if (op == nullptr) return SR_FAULT ;

	if (op->magic != MFSNS_MAGIC) return SR_NOTOPEN ;

	if (op->call.setopts != nullptr) {
	    rs = (*op->call.setopts)(op->obj,opts) ;
	}

	return rs ;
}
/* end subroutine (mfsns_setopts) */


int mfsns_audit(MFSNS *op)
{
	int		rs = SR_NOSYS ;

	if (op == nullptr) return SR_FAULT ;

	if (op->magic != MFSNS_MAGIC) return SR_NOTOPEN ;

	if (op->call.audit != nullptr) {
	    rs = (*op->call.audit)(op->obj) ;
	}

	return rs ;
}
/* end subroutine (mfsns_audit) */


int mfsns_get(MFSNS *op,char *rbuf,int rlen,cchar *un,int w)
{
	int		rs = SR_NOSYS ;

	if (op == nullptr) return SR_FAULT ;

	if (op->magic != MFSNS_MAGIC) return SR_NOTOPEN ;

#if	CF_DEBUGS
	debugprintf("mfsns_get ent un=%s w=%u\n",un,w) ;
#endif

	if (op->call.get != nullptr) {
	    rs = (*op->call.get)(op->obj,rbuf,rlen,un,w) ;
	}

	return rs ;
}
/* end subroutine (mfsns_count) */


int mfsns_curbegin(MFSNS *op,MFSNS_CUR *curp)
{
	int		rs = SR_OK ;

	if (op == nullptr) return SR_FAULT ;
	if (curp == nullptr) return SR_FAULT ;

	if (op->magic != MFSNS_MAGIC) return SR_NOTOPEN ;

	memset(curp,0,sizeof(MFSNS_CUR)) ;

	if (op->call.curbegin != nullptr) {
	    void	*p ;
	    if ((rs = uc_malloc(op->cursize,&p)) >= 0) {
		curp->scp = p ;
		if ((rs = (*op->call.curbegin)(op->obj,curp->scp)) >= 0) {
	    	    curp->magic = MFSNS_MAGIC ;
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
/* end subroutine (mfsns_curbegin) */


int mfsns_curend(MFSNS *op,MFSNS_CUR *curp)
{
	int		rs = SR_OK ;
	int		rs1 ;

	if (op == nullptr) return SR_FAULT ;
	if (curp == nullptr) return SR_FAULT ;

	if (op->magic != MFSNS_MAGIC) return SR_NOTOPEN ;
	if (curp->magic != MFSNS_MAGIC) return SR_NOTOPEN ;

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

	curp->magic = 0 ;
	return rs ;
} /* end subroutine (mfsns_curend) */

int mfsns_curenum(MFSNS *op,MFSNS_CUR *curp,char *vbuf,int vlen,int w) noex {
	int		rs = SR_NOSYS ;

	if (op == nullptr) return SR_FAULT ;
	if (curp == nullptr) return SR_FAULT ;
	if (vbuf == nullptr) return SR_FAULT ;

	if (op->magic != MFSNS_MAGIC) return SR_NOTOPEN ;
	if (curp->magic != MFSNS_MAGIC) return SR_NOTOPEN ;

	if (op->call.enumerate != nullptr) {
	    rs = (*op->call.enumerate)(op->obj,curp->scp,vbuf,vlen,w) ;
	}

	return rs ;
} /* end subroutine (mfsns_curenum) */


/* private subroutines */


/* find and load the DB-access object */
local int mfsns_objloadbegin(MFSNS *op,cchar *pr,cchar *objname)
{
	MODLOAD		*lp = &op->loader ;
	VECSTR		syms ;
	const int	n = nelem(subs) ;
	const int	vo = VECSTR_OCOMPACT ;
	int		rs ;
	int		rs1 ;

#if	CF_DEBUGS
	debugprintf("mfsns_objloadbegin: pr=%s\n",pr) ;
	debugprintf("mfsns_objloadbegin: objname=%s\n",objname) ;
#endif

	if ((rs = vecstr_start(&syms,n,vo)) >= 0) {
	    const int	nlen = SYMNAMELEN ;
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
	            cchar	*modbname = MFSNS_MODBNAME ;
#if	CF_LOOKOTHER
	            const int	mo = (MODLOAD_OLIBVAR | MODLOAD_OSDIRS) ;
#else
	            const int	mo = 0 ;
#endif
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
	debugprintf("mfsns_objloadbegin: modload_open() rs=%d\n",rs) ;
#endif

	if (rs >= 0) {
	    int		mv[2] ;
	    if ((rs = modload_getmva(lp,mv,2)) >= 0) {
		void	*p ;
		op->objsize = mv[0] ;
		op->cursize = mv[1] ;
		if ((rs = uc_malloc(op->objsize,&p)) >= 0) {
		    op->obj = p ;
		    rs = mfsns_loadcalls(op,objname) ;
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
}
/* end subroutine (mfsns_objloadbegin) */


local int mfsns_objloadend(MFSNS *op)
{
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
}
/* end subroutine (mfsns_objloadend) */


local int mfsns_loadcalls(MFSNS *op,cchar *objname)
{
	MODLOAD		*lp = &op->loader ;
	const int	nlen = SYMNAMELEN ;
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

#if	CF_DEBUGS
	    debugprintf("mfsns_loadcalls: call=%s %c\n",
		subs[i],
		((snp != nullptr) ? 'Y' : 'N')) ;
#endif

	    if (snp != nullptr) {

	        c += 1 ;
		switch (i) {

		case sub_open:
		    op->call.open = 
			(int (*)(void *,cchar *)) snp ;
		    break ;

		case sub_setopts:
		    op->call.setopts =
			(int (*)(void *,int)) snp ;
		    break ;

		case sub_get:
		    op->call.get =
			(int (*)(void *,char *,int,cchar *,int)) snp ;
		    break ;

		case sub_curbegin:
		    op->call.curbegin = 
			(int (*)(void *,void *)) snp ;
		    break ;

		case sub_curenum:
		    op->call.enumerate = 
			(int (*)(void *,void *,char *,int,int)) snp ;
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
/* end subroutine (mfsns_loadcalls) */

local bool isrequired(int i) noex {
	bool		f = false ;
	switch (i) {
	case sub_open:
	case sub_setopts:
	case sub_get:
	case sub_close:
	    f = true ;
	    break ;
	} /* end switch */
	return f ;
} /* end subroutine (isrequired) */


