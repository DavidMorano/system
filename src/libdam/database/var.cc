/* var SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* VAR management */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-12-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services
	(RNS).

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	var

	Description:
	This module implements an interface (a trivial one) that
	provides access to the VAR object (which is dynamically
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
#include	<getnodename.h>
#include	<vecstr.h>
#include	<nulstr.h>
#include	<endian.h>
#include	<sncpyx.h>
#include	<mkpr.h>
#include	<mkpathx.h>
#include	<mkfname.h>
#include	<isnot.h>
#include	<localmisc.h>

#include	"var.h"
#include	"vars.h"


/* local defines */

#ifndef	VARPRLOCAL
#define	VARPRLOCAL	"LOCAL"
#endif

#define	VAR_MODBNAME	"vars"
#define	VAR_OBJNAME	"vars"

#define	LIBCNAME	"lib"
#define	SONAME		"vars"
#define	INDSUF		"vi"

#ifndef	VARLIBPATH
#define	VARLIBPATH	"LD_LIBRARY_PATH"
#endif

#ifndef	SYMNAMELEN
#define	SYMNAMELEN	60
#endif


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

static int	var_objloadbegin(VAR *,cchar *) noex ;
static int	var_objloadend(VAR *) noex ;
static int	var_loadcalls(VAR *,cchar *) noex ;

static bool	isrequired(int) noex ;


/* external variables */


/* local variables */

constexpr cpcchar	subs[] = {
	"open",
	"count",
	"curbegin",
	"curenum",
	"curend",
	"fetch",
	"info",
	"audit",
	"close",
	nullptr
} ;

enum subs {
	sub_open,
	sub_count,
	sub_curbegin,
	sub_curenum,
	sub_curend,
	sub_fetch,
	sub_info,
	sub_audit,
	sub_close,
	sub_overlast
} ;


/* exported variables */


/* exported subroutines */

int var_open(VAR *op,cchar *dbname) noex {
	int		rs ;
	cchar		*objname = VAR_OBJNAME ;

	if (op == nullptr) return SR_FAULT ;
	if (dbname == nullptr) return SR_FAULT ;

	if (dbname[0] == '\0') return SR_INVALID ;

	memclear(op) ;

	if ((rs = var_objloadbegin(op,objname)) >= 0) {
	    if ((rs = (*op->call.open)(op->obj,dbname)) >= 0) {
		op->magic = VAR_MAGIC ;
	    }
	    if (rs < 0)
		var_objloadend(op) ;
	} /* end if (objloadbegin) */

	return rs ;
}
/* end subroutine (var_open) */

int var_opena(VAR *op,cchar **narr) noex {
	int		rs ;
	cchar	*objname = VAR_OBJNAME ;

	if (op == nullptr) return SR_FAULT ;
	if (narr == nullptr) return SR_FAULT ;

	if (narr[0] == nullptr) return SR_INVALID ;
	if (narr[0][0] == '\0') return SR_INVALID ;

	memclear(op) ;

	if ((rs = var_objloadbegin(op,objname)) >= 0) {
	    int	i ;
	    for (i = 0 ; narr[i] != nullptr ; i += 1) {
	        rs = (*op->call.open)(op->obj,narr[i]) ;
	        if ((rs >= 0) || (! isNotPresent(rs))) break ;
	    } /* end for */
	    if (rs >= 0) {
		op->magic = VAR_MAGIC ;
	    }
	    if (rs < 0)
		var_objloadend(op) ;
	} /* end if (objloadbegin) */

	return rs ;
}
/* end subroutine (var_opena) */

int var_close(VAR *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;

	if (op == nullptr) return SR_FAULT ;

	if (op->magic != VAR_MAGIC) return SR_NOTOPEN ;

	rs1 = (*op->call.close)(op->obj) ;
	if (rs >= 0) rs = rs1 ;

	rs1 = var_objloadend(op) ;
	if (rs >= 0) rs = rs1 ;

	op->magic = 0 ;
	return rs ;
}
/* end subroutine (var_close) */

int var_getinfo(VAR *op,VAR_INFO *vip) noex {
	VARS_INFO	vsi ;
	int		rs = SR_NOSYS ;

	if (op == nullptr) return SR_FAULT ;
	if (vip == nullptr) return SR_FAULT ;

	if (op->magic != VAR_MAGIC) return SR_NOTOPEN ;

	memclear(vip) ;

	if (op->call.info != nullptr) {
	    if ((rs = (*op->call.info)(op->obj,&vsi)) >= 0) {
		vip->wtime = vsi.wtime ;
		vip->mtime = vsi.mtime ;
		vip->nvars = vsi.nvars ;
		vip->nskip = vsi.nskip ;
	    }
	} /* end if */

	return rs ;
}
/* end subroutine (var_info) */

int var_audit(VAR *op) noex {
	int		rs = SR_NOSYS ;

	if (op == nullptr) return SR_FAULT ;

	if (op->magic != VAR_MAGIC) return SR_NOTOPEN ;

	if (op->call.audit != nullptr) {
	    rs = (*op->call.audit)(op->obj) ;
	}

	return rs ;
}
/* end subroutine (var_audit) */

int var_count(VAR *op) noex {
	int		rs = SR_NOSYS ;

	if (op == nullptr) return SR_FAULT ;

	if (op->magic != VAR_MAGIC) return SR_NOTOPEN ;

	if (op->call.count != nullptr) {
	    rs = (*op->call.count)(op->obj) ;
	}

	return rs ;
}
/* end subroutine (var_count) */

int var_curbegin(VAR *op,VAR_CUR *curp) noex {
	int		rs = SR_OK ;

	if (op == nullptr) return SR_FAULT ;
	if (curp == nullptr) return SR_FAULT ;

	if (op->magic != VAR_MAGIC) return SR_NOTOPEN ;

	memclear(curp) ;

	if (op->call.curbegin != nullptr) {
	    if (void *vp{} ; (rs = uc_malloc(op->cursz,&vp)) >= 0) {
		curp->scp = vp ;
	        if ((rs = (*op->call.curbegin)(op->obj,curp->scp)) >= 0) {
	            curp->magic = VAR_MAGIC ;
		}
	        if (rs < 0) {
	            uc_free(curp->scp) ;
	            curp->scp = nullptr ;
	        }
	    } /* end if (memory-allocation) */
	} else {
	    rs = SR_NOSYS ;
	}
	return rs ;
}
/* end subroutine (var_curbegin) */

int var_curend(VAR *op,VAR_CUR *curp) noex {
	int		rs = SR_OK ;
	int		rs1 ;

	if (op == nullptr) return SR_FAULT ;
	if (curp == nullptr) return SR_FAULT ;

	if (op->magic != VAR_MAGIC) return SR_NOTOPEN ;
	if (curp->magic != VAR_MAGIC) return SR_NOTOPEN ;

	if (curp->scp != nullptr) {
	    if (op->call.curend != nullptr) {
	        rs1 = (*op->call.curend)(op->obj,curp->scp) ;
	        if (rs >= 0) rs = rs1 ;
	    } else {
	        rs = SR_NOSYS ;
	    }
	    {
	        rs1 = uc_free(curp->scp) ;
	        if (rs >= 0) rs = rs1 ;
	        curp->scp = nullptr ;
	    }
	} else {
	    rs = SR_NOANODE ;
	}

	curp->magic = 0 ;
	return rs ;
}
/* end subroutine (var_curend) */

int var_fetch(VAR *op,cc *kp,int kl,VAR_CUR *curp,char *vbuf,int vlen) noex {
	int		rs = SR_NOSYS ;

	if (op == nullptr) return SR_FAULT ;
	if (curp == nullptr) return SR_FAULT ;
	if (kp == nullptr) return SR_FAULT ;

	if (op->magic != VAR_MAGIC) return SR_NOTOPEN ;
	if (curp->magic != VAR_MAGIC) return SR_NOTOPEN ;

	if (op->call.fetch != nullptr) {
	    rs = (*op->call.fetch)(op->obj,kp,kl,curp->scp,vbuf,vlen) ;
	}

	return rs ;
}
/* end subroutine (var_fetch) */

int var_curenum(VAR *op,VAR_CUR *curp,char *kbuf,int klen,
		char *vbuf,int vlen) noex {
	int		rs = SR_NOSYS ;

	if (op == nullptr) return SR_FAULT ;
	if (curp == nullptr) return SR_FAULT ;
	if (kbuf == nullptr) return SR_FAULT ;

	if (op->magic != VAR_MAGIC) return SR_NOTOPEN ;
	if (curp->magic != VAR_MAGIC) return SR_NOTOPEN ;

	if (op->call.enumerate != nullptr) {
	    rs = (*op->call.enumerate)(op->obj,curp->scp,
		kbuf,klen,vbuf,vlen) ;
	}

	return rs ;
}
/* end subroutine (var_curenum) */

int varinfo(VARINFO *vip,cchar dbnp[],int dbnl) noex {
	int		rs ;
	int		rs1 ;
	cchar	*np ;

	if (vip == nullptr) return SR_FAULT ;
	if (dbnp == nullptr) return SR_FAULT ;

	if (dbnp[0] == '\0') return SR_INVALID ;

	if (nulstr ns ; (rs = ns.start(dbnp,dbnl,&np)) >= 0) {
	    cchar	*end = ENDIANSTR ;
	    char	tmpfname[MAXPATHLEN + 1] ;

	    memclear(vip) ;

	    if ((rs = mkfnamesuf2(tmpfname,np,INDSUF,end)) >= 0) {
	        USTAT	sb ;
		if ((rs = u_stat(tmpfname,&sb)) >= 0) {
		    vip->size = sb.st_size ;
		    vip->mtime = sb.st_mtime ;
		}
	    }

	    rs1 = ns.finish ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (nulstr) */

	return rs ;
}
/* end subroutine (varinfo) */

int varunlink(cchar *dbnp,int dbnl) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	cchar	*np ;

	if (dbnp == nullptr) return SR_FAULT ;
	if (dbnp[0] == '\0') return SR_INVALID ;

	if (nulstr ns ; (rs = ns.start(dbnp,dbnl,&np)) >= 0) {
	    cchar	*end = ENDIANSTR ;
	    char	tmpfname[MAXPATHLEN + 1] ;

	    if ((rs = mkfnamesuf2(tmpfname,np,INDSUF,end)) >= 0) {
		rs = uc_unlink(tmpfname) ;
	    }

	    rs1 = ns.finish ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (nulstr) */

	return rs ;
}
/* end subroutine (varunlink) */


/* private subroutines */

static int var_objloadbegin(VAR *op,cchar *objname) noex {
	modload		*lp = &op->loader ;
	int		rs ;
	char		dn[MAXHOSTNAMELEN+1] ;
	if ((rs = getnodedomain(nullptr,dn)) >= 0) {
	    cchar	*prname = VARPRLOCAL ;
	    char	pr[MAXPATHLEN+1] ;
	    if ((rs = mkpr(pr,MAXPATHLEN,prname,dn)) >= 0) {
		vecstr	syms ;
	        cint	vn = nelem(subs) ;
		cint	vo = VECSTR_OCOMPACT ;

	        if ((rs = vecstr_start(&syms,vn,vo)) >= 0) {
		    cint	snl = SYMNAMELEN ;
		    cchar	*on = objname ;
		    char	snb[SYMNAMELEN + 1] ;

	            for (int i = 0 ; (i < vn) && subs[i] ; i += 1) {
	                if (isrequired(i)) {
	                    if ((rs = sncpy3(snb,snl,on,"_",subs[i])) >= 0) {
			        rs = vecstr_add(&syms,snb,rs) ;
			    }
		        }
		        if (rs < 0) break ;
	            } /* end for */
        
	            if (rs >= 0) {
		        if (mainv sv{} ; (rs = vecstr_getvec(&syms,&sv)) >= 0) {
	                    cchar	*modbname = VAR_MODBNAME ;
			    int		mo = 0 ;
	                    mo |= modloadm.libvar ;
			    mo |= modloadm.libprs ;
			    mo |= modloadm.libsdirs ;
	                    rs = modload_open(lp,pr,modbname,objname,mo,sv) ;
			} /* end if (getvec) */
	            } /* end if (ok) */

	            vecstr_finish(&syms) ;
	        } /* end if (allocation) */

		if (rs >= 0) {
		    if (int mv[2] ; (rs = modload_getmva(lp,mv,2)) >= 0) {
			op->objsz = mv[0] ;
			op->cursz = mv[1] ;
			if (void *p ; (rs = uc_malloc(op->objsz,&p)) >= 0) {
			    op->obj = p ;
			    rs = var_loadcalls(op,objname) ;
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

	    } /* end if (mkpr) */
	} /* end if (getnodedomain) */

	return rs ;
}
/* end subroutine (var_objloadbegin) */

static int var_objloadend(VAR *op) noex {
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
/* end subroutine (var_objloadend) */

static int var_loadcalls(VAR *op,cchar *objname) noex {
	modload		*lp = &op->loader ;
	int		rs = SR_OK ;
	int		c = 0 ;
	char		symname[SYMNAMELEN + 1] ;
	cvoid	*snp ;

	for (int i = 0 ; subs[i] != nullptr ; i += 1) {

	    if ((rs = sncpy3(symname,SYMNAMELEN,objname,"_",subs[i])) >= 0) {
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
		    op->call.open = 
			(int (*)(void *,cchar *)) snp ;
		    break ;

		case sub_count:
		    op->call.count = (int (*)(void *)) snp ;
		    break ;

		case sub_curbegin:
		    op->call.curbegin = 
			(int (*)(void *,void *)) snp ;
		    break ;

		case sub_fetch:
		    op->call.fetch = 
			(int (*)(void *,cchar *,int,void *,char *,int)) 
				snp ;
		    break ;

		case sub_curenum:
		    op->call.enumerate = 
			(int (*)(void *,void *,char *,int,char *,int)) snp ;
		    break ;

		case sub_curend:
		    op->call.curend = (int (*)(void *,void *)) snp ;
		    break ;

		case sub_info:
		    op->call.info = (int (*)(void *,VARS_INFO *)) snp ;
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
/* end subroutine (var_loadcalls) */

static bool isrequired(int i) noex {
	bool		f = false ;
	switch (i) {
	case sub_open:
	case sub_curbegin:
	case sub_curenum:
	case sub_curend:
	case sub_close:
	    f = true ;
	    break ;
	case sub_fetch:
	    break ;
	} /* end switch */
	return f ;
}
/* end subroutine (isrequired) */


