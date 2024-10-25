/* varmk SUPPORT */
/* encoding=ISO8859-1 */
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

	This module implements an interface (a trivial one) that
	provides access to the VARMK object (which is dynamically
	loaded).

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<dlfcn.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<vecstr.h>
#include	<localmisc.h>

#include	"varmk.h"
#include	"varmks.h"


/* local defines */

#define	VARMK_OBJNAME	"varmks"
#define	VARMK_MODBNAME	"varmks"

#ifndef	VARPRLOCAL
#define	VARPRLOCAL	"LOCAL"
#endif

#ifndef	SYMNAMELEN
#define	SYMNAMELEN	60
#endif


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

static int	varmk_objloadbegin(VARMK *,cchar *,cchar *) noex ;
static int	varmk_objloadend(VARMK *) noex ;
static int	varmk_loadcalls(VARMK *,cchar *) noex ;

static bool	isrequired(int) noex ;


/* external variables */


/* local variables */

static cchar	*subs[] = {
	"open",
	"addvar",
	"abort",
	"chgrp",
	"close",
	nullptr
} ;

enum subs {
	sub_open,
	sub_addvar,
	sub_abort,
	sub_chgrp,
	sub_close,
	sub_overlast
} ;


/* exported variables */


/* exported subroutines */

int varmk_open(VARMK *op,cchar *dbname,int of,mode_t om,int n) noex {
	int		rs ;
	cchar	*objname = VARMK_OBJNAME ;
	char		dn[MAXHOSTNAMELEN+1] ;

	if (op == nullptr) return SR_FAULT ;
	if (dbname == nullptr) return SR_FAULT ;

	if (dbname[0] == '\0') return SR_INVALID ;

	memclear(op) ;

	if ((rs = getnodedomain(nullptr,dn)) >= 0) {
	    cint	prlen = MAXPATHLEN ;
	    cchar	*pn = VARPRLOCAL ;
	    char	prbuf[MAXPATHLEN+1] ;
	    if ((rs = mkpr(prbuf,prlen,pn,dn)) >= 0) {
	        if ((rs = varmk_objloadbegin(op,prbuf,objname)) >= 0) {
	            if ((rs = (*op->call.open)(op->obj,dbname,of,om,n)) >= 0) {
	                op->magic = VARMK_MAGIC ;
	            }
	            if (rs < 0)
	                varmk_objloadend(op) ;
	        } /* end if (objloadbegin) */
	    } /* end if (mkpr) */
	} /* end if (getnodedomain) */

	return rs ;
}
/* end subroutine (varmk_open) */


/* free up the entire vector string data structure object */
int varmk_close(VARMK *op)
{
	int		rs = SR_OK ;
	int		rs1 ;

	if (op == nullptr) return SR_FAULT ;

	if (op->magic != VARMK_MAGIC) return SR_NOTOPEN ;

	rs1 = (*op->call.close)(op->obj) ;
	if (rs >= 0) rs = rs1 ;

	rs1 = varmk_objloadend(op) ;
	if (rs >= 0) rs = rs1 ;

	op->magic = 0 ;
	return rs ;
}
/* end subroutine (varmk_close) */


int varmk_addvar(VARMK *op,cchar k[],cchar vp[],int vl)
{
	int		rs ;

	if (op == nullptr) return SR_FAULT ;

	if (op->magic != VARMK_MAGIC) return SR_NOTOPEN ;

	rs = (*op->call.addvar)(op->obj,k,vp,vl) ;

	return rs ;
}
/* end subroutine (varmk_addvar) */


int varmk_abort(VARMK *op)
{
	int		rs = SR_NOSYS ;

	if (op == nullptr) return SR_FAULT ;

	if (op->magic != VARMK_MAGIC) return SR_NOTOPEN ;

	if (op->call.abort != nullptr) {
	    rs = (*op->call.abort)(op->obj) ;
	}

	return rs ;
}
/* end subroutine (varmk_abort) */


int varmk_chgrp(VARMK *op,gid_t gid)
{
	int		rs = SR_NOSYS ;

	if (op == nullptr) return SR_FAULT ;

	if (op->magic != VARMK_MAGIC) return SR_NOTOPEN ;

	if (op->call.chgrp != nullptr) {
	    rs = (*op->call.chgrp)(op->obj,gid) ;
	}

	return rs ;
}
/* end subroutine (varmk_chgrp) */


/* private subroutines */

static int varmk_objloadbegin(VARMK *op,cchar *pr,cchar *objname) noex {
	modload		*lp = &op->loader ;
	vecstr		syms ;
	cint		ne = sub_overlast ;
	cint		vo = VECSTR_OCOMPACT ;
	int		rs ;
	int		rs1 ;

	if ((rs = vecstr_start(&syms,ne,vo)) >= 0) {
	    cint	snl = SYMNAMELEN ;
	    int		i ;
	    int		f_modload = false ;
	    cchar	**sv ;
	    cchar	*on = objname ;
	    char	snb[SYMNAMELEN + 1] ;

	    for (i = 0 ; (i < ne) && subs[i] ; i += 1) {
	            if ((rs = sncpy3(snb,snl,on,"_",subs[i])) >= 0) {
	                rs = vecstr_add(&syms,snb,rs) ;
		    }
	        if (rs < 0) break ;
	    } /* end for */

	    if (rs >= 0) {
	        if ((rs = vecstr_getvec(&syms,&sv)) >= 0) {
	            cchar	*mn = VARMK_MODBNAME ;
	            cchar	*on = objname ;
	            int		mo = 0 ;
	            mo |= MODLOAD_OLIBVAR ;
	            mo |= MODLOAD_OPRS ;
	            mo |= MODLOAD_OSDIRS ;
	            mo |= MODLOAD_OAVAIL ;
	            rs = modload_open(lp,pr,mn,on,mo,sv) ;
		    f_modload = (rs >= 0) ;
	        } /* end if (getvec) */
	    } /* end if (ok) */

	    rs1 = vecstr_finish(&syms) ;
	    if (rs >= 0) rs = rs1 ;
	    if ((rs < 0) && f_modload) {
		modload_close(lp) ;
	    }
	} /* end if (vecstr_start) */

	if (rs >= 0) {
	    int		mv[2] ;
	    if ((rs = modload_getmva(lp,mv,1)) >= 0) {
	        void	*p ;
	        op->objsize = mv[0] ;
	        if ((rs = uc_malloc(op->objsize,&p)) >= 0) {
	            op->obj = p ;
	            rs = varmk_loadcalls(op,objname) ;
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

	return rs ;
}
/* end subroutine (varmk_objloadbegin) */

static int varmk_objloadend(VARMK *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;

	if (op->obj) {
	    rs1 = uc_free(op->obj) ;
	    if (rs >= 0) rs = rs1 ;
	    op->obj = nullptr ;
	}

	rs1 = modload_close(&op->loader) ;
	if (rs >= 0) rs = rs1 ;

	return rs ;
}
/* end subroutine (varmk_objloadend) */

static int varmk_loadcalls(VARMK *op,cchar *soname) noex {
	modload		*lp = &op->loader ;
	int		rs = SR_OK ;
	int		c = 0 ;
	char		symname[SYMNAMELEN + 1] ;
	cvoid	*snp ;

	for (int i = 0 ; subs[i] != nullptr ; i += 1) {

	    if ((rs = sncpy3(symname,SYMNAMELEN,soname,"_",subs[i])) >= 0) {
	        if ((rs = modload_getsym(lp,symname,&snp)) == SR_NOTFOUND) {
	            snp = nullptr ;
	            if (! isrequired(i)) rs = SR_OK ;
	        }
	    }

	    if (rs < 0) break ;

	    if (snp != nullptr) {

	        c += 1 ;
	        switch (i) {

	        case sub_open:
	            op->call.open = (int (*)(void *,
	                cchar *,int,mode_t,int)) snp ;
	            break ;

	        case sub_addvar:
	            op->call.addvar = (int (*)(void *,cchar *,
	                cchar *,int)) snp ;
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
/* end subroutine (varmk_loadcalls) */

static bool isrequired(int i) noex {
	bool		f = false ;
	switch (i) {
	case sub_open:
	case sub_addvar:
	case sub_close:
	    f = true ;
	    break ;
	} /* end switch */
	return f ;
}
/* end subroutine (isrequired) */


