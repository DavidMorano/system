/* pcsns SUPPORT */
/* lang=C++20 */

/* PCSNS object-load management */
/* version %I% last-modified %G% */

#define	CF_LOOKOTHER	0		/* look elsewhere */

/* revision history:

	- 2008-10-01, David A­D­ Morano
	This module was originally written.

*/

/* Copyright © 2008 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	pcsns

	Description:
	This module implements an interface (a trivial one) that
	provides access to the PCSNS object (which is dynamically
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
#include	<usystem.h>
#include	<estrings.h>
#include	<vecstr.h>
#include	<localmisc.h>

#include	"pcsns.h"


/* local defines */

#define	PCSNS_MODBNAME	"pcsnso"
#define	PCSNS_OBJNAME	"pcsnso"

#ifndef	SYMNAMELEN
#define	SYMNAMELEN	60
#endif


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

static int	pcsns_objloadbegin(pcsns *,cchar *,cchar *) noex ;
static int	pcsns_objloadend(pcsns *) noex ;
static int	pcsns_loadcalls(pcsns *,cchar *) noex ;

static bool	isrequired(int) noex ;


/* local variables */

enum subs {
	sub_open,
	sub_setopts,
	sub_get,
	sub_curbegin,
	sub_enum,
	sub_curend,
	sub_audit,
	sub_close,
	sub_overlast
} ;

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
} ;


/* exported variables */


/* exported subroutines */

int pcsns_open(pcsns *op,cchar *pr) noex {
	int		rs ;
	cchar	*objname = PCSNS_OBJNAME ;

	if (op == nullptr) return SR_FAULT ;
	if (pr == nullptr) return SR_FAULT ;

	if (pr[0] == '\0') return SR_INVALID ;

	memset(op,0,sizeof(PCSNS)) ;

	if ((rs = pcsns_objloadbegin(op,pr,objname)) >= 0) {
	    if ((rs = (*op->call.open)(op->obj,pr)) >= 0) {
		op->magic = PCSNS_MAGIC ;
	    }
	    if (rs < 0)
		pcsns_objloadend(op) ;
	} /* end if (objload-begin) */

	return rs ;
}
/* end subroutine (pcsns_open) */

int pcsns_close(pcsns *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;

	if (op == nullptr) return SR_FAULT ;

	if (op->magic != PCSNS_MAGIC) return SR_NOTOPEN ;

	rs1 = (*op->call.close)(op->obj) ;
	if (rs >= 0) rs = rs1 ;

	rs1 = pcsns_objloadend(op) ;
	if (rs >= 0) rs = rs1 ;

	op->magic = 0 ;
	return rs ;
}
/* end subroutine (pcsns_close) */

int pcsns_setopts(pcsns *op,int opts) noex {
	int		rs = SR_NOSYS ;

	if (op == nullptr) return SR_FAULT ;

	if (op->magic != PCSNS_MAGIC) return SR_NOTOPEN ;

	if (op->call.setopts != nullptr) {
	    rs = (*op->call.setopts)(op->obj,opts) ;
	}

	return rs ;
}
/* end subroutine (pcsns_setopts) */

int pcsns_audit(pcsns *op) noex {
	int		rs = SR_NOSYS ;

	if (op == nullptr) return SR_FAULT ;

	if (op->magic != PCSNS_MAGIC) return SR_NOTOPEN ;

	if (op->call.audit != nullptr) {
	    rs = (*op->call.audit)(op->obj) ;
	}

	return rs ;
}
/* end subroutine (pcsns_audit) */

int pcsns_get(pcsns *op,char *rbuf,int rlen,cchar *un,int w) noex {
	int		rs = SR_NOSYS ;

	if (op == nullptr) return SR_FAULT ;

	if (op->magic != PCSNS_MAGIC) return SR_NOTOPEN ;

	if (op->call.get != nullptr) {
	    rs = (*op->call.get)(op->obj,rbuf,rlen,un,w) ;
	}

	return rs ;
}
/* end subroutine (pcsns_count) */

int pcsns_curbegin(pcsns *op,PCSNS_CUR *curp) noex {
	int		rs = SR_NOTSUP ;

	if (op == nullptr) return SR_FAULT ;
	if (curp == nullptr) return SR_FAULT ;

	if (op->magic != PCSNS_MAGIC) return SR_NOTOPEN ;

	memset(curp,0,sizeof(PCSNS_CUR)) ;

	if (op->call.curbegin != nullptr) {
	    void	*p ;
	    if ((rs = uc_malloc(op->cursize,&p)) >= 0) {
		curp->scp = p ;
		if ((rs = (*op->call.curbegin)(op->obj,curp->scp)) >= 0) {
	    	    curp->magic = PCSNS_MAGIC ;
		}
		if (rs < 0) {
	    	    uc_free(curp->scp) ;
	    	    curp->scp = nullptr ;
		}
	    } /* end if (memory-allocation) */
	}

	return rs ;
}
/* end subroutine (pcsns_curbegin) */

int pcsns_curend(pcsns *op,PCSNS_CUR *curp) noex {
	int		rs = SR_NOTSUP ;
	int		rs1 ;

	if (op == nullptr) return SR_FAULT ;
	if (curp == nullptr) return SR_FAULT ;

	if (op->magic != PCSNS_MAGIC) return SR_NOTOPEN ;
	if (curp->magic != PCSNS_MAGIC) return SR_NOTOPEN ;

	if (curp->scp != nullptr) {
	    if (op->call.curend != nullptr) {
	        rs1 = (*op->call.curend)(op->obj,curp->scp) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    rs1 = uc_free(curp->scp) ;
	    if (rs >= 0) rs = rs1 ;
	    curp->scp = nullptr ;
	}

	curp->magic = 0 ;
	return rs ;
}
/* end subroutine (pcsns_curend) */

int pcsns_enum(pcsns *op,PCSNS_CUR *curp,char *vbuf,int vlen,int w) noex {
	int		rs = SR_NOSYS ;

	if (op == nullptr) return SR_FAULT ;
	if (curp == nullptr) return SR_FAULT ;
	if (vbuf == nullptr) return SR_FAULT ;

	if (op->magic != PCSNS_MAGIC) return SR_NOTOPEN ;
	if (curp->magic != PCSNS_MAGIC) return SR_NOTOPEN ;

	if (op->call.enumerate != nullptr) {
	    rs = (*op->call.enumerate)(op->obj,curp->scp,vbuf,vlen,w) ;
	}

	return rs ;
}
/* end subroutine (pcsns_enumerate) */


/* private subroutines */

static int pcsns_objloadbegin(pcsns *op,cchar *pr,cchar *objname) noex {
	modload		*lp = &op->loader ;
	VECSTR		syms ;
	cint	n = nelem(subs) ;
	cint	vo = VECSTR_OCOMPACT ;
	int		rs ;
	int		rs1 ;

	if ((rs = vecstr_start(&syms,n,vo)) >= 0) {
	    cint	nlen = SYMNAMELEN ;
	    int		i ;
	    int		f_modload = FALSE ;
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
	            cchar	*modbname = PCSNS_MODBNAME ;
#if	CF_LOOKOTHER
	            cint	mo = (MODLOAD_OLIBVAR | MODLOAD_OSDIRS) ;
#else
	            cint	mo = 0 ;
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

	if (rs >= 0) {
	    int		mv[2] ;
	    if ((rs = modload_getmva(lp,mv,2)) >= 0) {
		void	*p ;
		op->objsize = mv[0] ;
		op->cursize = mv[1] ;
		if ((rs = uc_malloc(op->objsize,&p)) >= 0) {
		    op->obj = p ;
		    rs = pcsns_loadcalls(op,objname) ;
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
/* end subroutine (pcsns_objloadbegin) */

static int pcsns_objloadend(pcsns *op) noex {
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
/* end subroutine (pcsns_objloadend) */

static int pcsns_loadcalls(pcsns *op,cchar *objname) noex {
	modload		*lp = &op->loader ;
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

		case sub_enum:
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
/* end subroutine (pcsns_loadcalls) */

static bool isrequired(int i) noex {
	bool		f = FALSE ;
	switch (i) {
	case sub_open:
	case sub_setopts:
	case sub_get:
	case sub_close:
	    f = TRUE ;
	    break ;
	} /* end switch */
	return f ;
}
/* end subroutine (isrequired) */


