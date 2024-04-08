/* strlistmk SUPPORT */
/* lang=C++20 */

/* STRLISTMK management */
/* version %I% last-modified %G% */


/* revision history:

	- 1998-12-01, David A­D­ Morano
	This module was originally written for hardware CAD support.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This module implements an interface (a trivial one) that
	provides access to the STRLISTMK object (which is dynamically
	loaded).

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<dlfcn.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<vecstr.h>
#include	<modload.h>
#include	<sncpyx.h>
#include	<localmisc.h>

#include	"strlistmk.h"
#include	"strlistmks.h"


/* local defines */

#define	STRLISTMK_MODBNAME	"strlistmks"
#define	STRLISTMK_OBJNAME	"strlistmks"
#define	STRLISTMK_PRLOCAL	"LOCAL"

#define	SLM		strlistmk

#ifndef	SYMNAMELEN
#define	SYMNAMELEN	60
#endif


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

static int	strlistmk_objloadbegin(SLM *,cchar *,cchar *) noex ;
static int	strlistmk_objloadend(SLM *) noex ;
static int	strlistmk_loadcalls(SLM *,cchar *) noex ;

static bool	isrequired(int) noex ;


/* local variables */

enum subs {
	sub_open,
	sub_add,
	sub_abort,
	sub_chgrp,
	sub_close,
	sub_overlast
} ;

static constexpr cpcchar	subs[] = {
	"open",
	"add",
	"abort",
	"chgrp",
	"close",
	nullptr
} ;


/* exported variables */


/* exported subroutines */

int strlistmk_open(SLM *op,cc *pr,cc *dbname,cc *lfname,
		int of,mode_t om,int n) noex {
	int		rs ;
	cchar		*objname = STRLISTMK_OBJNAME ;

	if (op == nullptr) return SR_FAULT ;
	if (pr == nullptr) return SR_FAULT ;
	if (dbname == nullptr) return SR_FAULT ;
	if (lfname == nullptr) return SR_FAULT ;

	if (dbname[0] == '\0') return SR_INVALID ;
	if (lfname[0] == '\0') return SR_INVALID ;

	memclear(op) ;

	if ((rs = strlistmk_objloadbegin(op,pr,objname)) >= 0) {
	    if ((rs = (*op->call.open)(op->obj,dbname,lfname,of,om,n)) >= 0) {
		op->magic = STRLISTMK_MAGIC ;
	    }
	    if (rs < 0)
		strlistmk_objloadend(op) ;
	} /* end if (objloadbegin) */

	return rs ;
}
/* end subroutine (strlistmk_open) */

int strlistmk_close(SLM *op) noex {
	int	rs = SR_OK ;
	int	rs1 ;

	if (op == nullptr) return SR_FAULT ;

	if (op->magic != STRLISTMK_MAGIC) return SR_NOTOPEN ;

	rs1 = (*op->call.close)(op->obj) ;
	if (rs >= 0) rs = rs1 ;

	rs1 = strlistmk_objloadend(op) ;
	if (rs >= 0) rs = rs1 ;

	op->magic = 0 ;
	return rs ;
}
/* end subroutine (strlistmk_close) */

int strlistmk_add(SLM *op,cc *sp,int sl) noex {
	int		rs ;
	if (op == nullptr) return SR_FAULT ;
	if (sp == nullptr) return SR_FAULT ;

	if (op->magic != STRLISTMK_MAGIC) return SR_NOTOPEN ;

	rs = (*op->call.add)(op->obj,sp,sl) ;

	return rs ;
}
/* end subroutine (strlistmk_add) */

int strlistmk_abort(SLM *op) noex {
	int		rs = SR_NOSYS ;
	if (op == nullptr) return SR_FAULT ;
	if (op->magic != STRLISTMK_MAGIC) return SR_NOTOPEN ;

	if (op->call.abort != nullptr)
	    rs = (*op->call.abort)(op->obj) ;

	return rs ;
}
/* end subroutine (strlistmk_abort) */

int strlistmk_chgrp(SLM *op,gid_t gid) noex {
	int		rs = SR_NOSYS ;
	if (op == nullptr) return SR_FAULT ;
	if (op->magic != STRLISTMK_MAGIC) return SR_NOTOPEN ;

	if (op->call.chgrp != nullptr)
	    rs = (*op->call.chgrp)(op->obj,gid) ;

	return rs ;
}
/* end subroutine (strlistmk_chgrp) */


/* private subroutines */

static int strlistmk_objloadbegin(SLM *op,cc *pr,cc *objname) noex {
	MODLOAD	*lp = &op->loader ;
	int	rs ;
	{
		vecstr		syms ;
	        cint	n = nelem(subs) ;
		int		opts = VECSTR_OCOMPACT ;
	        if ((rs = vecstr_start(&syms,n,opts)) >= 0) {
		    cint	symlen = SYMNAMELEN ;
		    int		snl ;
		    mainv	sv ;
		    char	symname[SYMNAMELEN + 1] ;

	            for (int i = 0 ; (i < n) && subs[i] ; i += 1) {
	                if (isrequired(i)) {
	                    rs = sncpy3(symname,symlen,objname,"_",subs[i]) ;
		            snl = rs ;
		            if (rs >= 0) 
			        rs = vecstr_add(&syms,symname,snl) ;
		        }
		        if (rs < 0) break ;
	            } /* end for */
        
	            if (rs >= 0) {
	                rs = vecstr_getvec(&syms,&sv) ;
		    }
        
	            if (rs >= 0) {
	                cchar	*modbname = STRLISTMK_MODBNAME ;
			opts = 0 ;
	                opts |= MODLOAD_OLIBVAR ;
			opts |= MODLOAD_OPRS ;
			opts |= MODLOAD_OSDIRS ;
	                rs = modload_open(lp,pr,modbname,objname,opts,sv) ;
	            }

	            vecstr_finish(&syms) ;
	        } /* end if (allocation) */
		if (rs >= 0) {
		    int		mv[2] ;
		    if ((rs = modload_getmva(lp,mv,1)) >= 0) {
			void	*p ;
			op->objsize = mv[0] ;
			    if ((rs = uc_malloc(op->objsize,&p)) >= 0) {
			        op->obj = p ;
			        rs = strlistmk_loadcalls(op,objname) ;
			        if (rs < 0) {
	    			    uc_free(op->obj) ;
	    			    op->obj = nullptr ;
			        }
			    } /* end if (memory-allocation) */
		    } /* end if (modload_getmva) */
		    if (rs < 0) {
			modload_close(lp) ;
		    }
		} /* end if (modload_open) */
	} /* end block */

	return rs ;
}
/* end subroutine (strlistmk_objloadbegin) */

static int strlistmk_objloadend(SLM *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (op->obj) {
	    rs1 = uc_free(op->obj) ;
	    if (rs >= 0) rs = rs1 ;
	    op->obj = nullptr ;
	}
	{
	    rs1 = modload_close(&op->loader) ;
	    if (rs >= 0) rs = rs1 ;
	}
	return rs ;
}
/* end subroutine (strlistmk_objloadend) */

static int strlistmk_loadcalls(SLM *op,cc *soname) noex {
	int	rs = SR_NOTFOUND ;
	int	c = 0 ;
	char	symname[SYMNAMELEN + 1] ;
	void	*snp ;

	for (int i = 0 ; subs[i] ; i += 1) {

	    rs = sncpy3(symname,SYMNAMELEN,soname,"_",subs[i]) ;
	    if (rs < 0)
		break ;

	    snp = dlsym(op->sop,symname) ;

	    if ((snp == nullptr) && isrequired(i)) {
	        rs = SR_NOTFOUND ;
		break ;
	    }

	    if (snp != nullptr) {

	        c += 1 ;
		switch (i) {

		case sub_open:
		    op->call.open = (int (*)(void *,
			cchar *,cchar *,int,mode_t,int)) snp ;
		    break ;

		case sub_add:
		    op->call.add = (int (*)(void *,cchar *,int)) snp ;
		    break ;

		case sub_abort:
		    op->call.abort = (int (*)(void *)) snp ;
		    break ;

		case sub_chgrp:
		    op->call.chgrp = (int (*)(void *,gid_t)) snp ;
		    break ;

		case sub_close:
		    op->call.close = (int (*)(void *)) snp ;
		    break ;

		} /* end switch */

	    } /* end if (it had the call) */

	} /* end for (subs) */

	return (rs >= 0) ? c : rs ;
}
/* end subroutine (strlistmk_loadcalls) */

static bool isrequired(int i) noex {
	bool		f = FALSE ;
	switch (i) {
	case sub_open:
	case sub_add:
	case sub_close:
	    f = TRUE ;
	    break ;
	} /* end switch */
	return f ;
}
/* end subroutine (isrequired) */


