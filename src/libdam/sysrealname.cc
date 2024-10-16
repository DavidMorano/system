/* sysrealname SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* load management and interface for the System REALNAME data-base */
/* version %I% last-modified %G% */


/* revision history:

	- 2008-10-01, David A­D­ Morano
	This module was originally written.

*/

/* Copyright © 2008 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	sysrealname

	Description:
	This module interfaces to the system REALNAME database.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<dlfcn.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<new>
#include	<usystem.h>
#include	<vecstr.h>
#include	<realname.h>
#include	<modload.h>
#include	<localmisc.h>

#include	"sysrealname.h"


/* local defines */

#define	SRN		sysrealname
#define	SRN_FL		sysrealname_fl
#define	SRN_CUR		sysrealname_cur
#define	SRN_INFO	sysrealname_info
#define	SRN_MODBNAME	"ipasswd"
#define	SRN_OBJNAME	"ipasswd"

#undef	SMM_INFO
#define	SMM_INFO	IPASSWD_INFO

#define	LIBCNAME	"lib"

#ifndef	VARLIBPATH
#define	VARLIBPATH	"LD_LIBRARY_PATH"
#endif

#ifndef	SYMNAMELEN
#define	SYMNAMELEN	60
#endif


/* imported namespaces */


/* local typedefs */

struct sysrealname_calls {
	int (*open)(void *,const char *) noex ;
	int (*info)(void *,IPASSWD_INFO *) noex ;
	int (*curbegin)(void *,IPASSWD_CUR *) noex ;
	int (*curend)(void *,IPASSWD_CUR *) noex ;
	int (*enumerate)(void *,IPASSWD_CUR *,char *,cchar **,char *,int) noex ;
	int (*fetch)(void *,IPASSWD_CUR *,int,char *,cchar **,int) noex ;
	int (*audit)(void *) noex ;
	int (*close)(void *) noex ;
} ; /* end if (sysrealaname_calls) */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

template<typename ... Args>
static int sysrealname_ctor(sysrealname *op,Args ... args) noex {
    	SYSREALNAME	*hop = op ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    cnullptr	np{} ;
	    memclear(hop) ;
	    rs = SR_NOMEM ;
	    if ((op->mlp = new(nothrow) modload) != np) {
		sysrealname_calls    *callp ;
                if ((callp = new(nothrow) sysrealname_calls) != np) {
                    op->callp = callp ;
                    rs = SR_OK ;
                } /* end if (new-sysrealname_calls) */
                if (rs < 0) {
                    delete op->mlp ;
                    op->mlp = nullptr ;
                }
	    } /* end if (new-modload) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (sysrealname_ctor) */

static int sysrealname_dtor(sysrealname *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
            if (op->callp) {
                sysrealname_calls    *callp = callsp(op->callp) ;
                delete callp ;
                op->callp = nullptr ;
            }
	    if (op->mlp) {
		delete op->mlp ;
		op->mlp = nullptr ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (sysrealname_dtor) */

template<typename ... Args>
static inline int sysrealname_magic(sysrealname *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magic == SYSREALNAME_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}
/* end subroutine (sysrealname_magic) */

static int	sysrealname_objloadbegin(SRM *,cchar *) noex ;
static int	sysrealname_objloadend(SRM *) noex ;
static int	sysrealname_loadcalls(SRM *,cchar *) noex ;

static int	sysrealname_curload(SRM *,SRN_CUR *,
			int,cchar **,int) noex ;

static bool	isrequired(int) noex ;


/* external variables */


/* local variables */

enum subs {
	sub_open,
	sub_info,
	sub_curbegin,
	sub_curend,
	sub_fetch,
	sub_enum,
	sub_audit,
	sub_close,
	sub_overlast
} ;

constexpt cpcchar		subs[] = {
	"open",
	"info",
	"curbegin",
	"curend",
	"fetch",
	"enum",
	"audit",
	"close",
	nullptr
} ;


/* exported variables */


/* exported subroutines */

int sysrealname_open(SRM *op,cchar *dbname) noex {
	int		rs ;
	cchar		*objname = SRN_OBJNAME ;

	if (op == nullptr) return SR_FAULT ;

	if (dbname == nullptr) dbname = SYSREALNAME_DBNAME ;

	memclear(op) ;

	if ((rs = sysrealname_objloadbegin(op,objname)) >= 0) {
	    if ((rs = (*op->call.open)(op->obj,dbname)) >= 0) {
	        op->magic = SYSREALNAME_MAGIC ;
	    }
	}

	return rs ;
}
/* end subroutine (sysrealname_open) */

int sysrealname_close(SRM *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;

	if (op == nullptr) return SR_FAULT ;

	if (op->magic != SYSREALNAME_MAGIC) return SR_NOTOPEN ;

	rs1 = (*op->call.close)(op->obj) ;
	if (rs >= 0) rs = rs1 ;

	rs1 = sysrealname_objloadend(op) ;
	if (rs >= 0) rs = rs1 ;

	op->magic = 0 ;
	return rs ;
}
/* end subroutine (sysrealname_close) */

int sysrealname_info(SRM *op,SYSREALNAME_INFO *ip) noex {
	IPASSWD_INFO	iinfo ;
	int		rs = SR_NOSYS ;
	int		n = 0 ;

	if (op == nullptr) return SR_FAULT ;

	if (op->magic != SYSREALNAME_MAGIC) return SR_NOTOPEN ;

	if (op->call.info != nullptr) {
	    rs = (*op->call.info)(op->obj,&iinfo) ;
	    n = rs ;
	}

	if (ip != nullptr) {
	    memset(ip,0,sizeof(SYSREALNAME_INFO)) ;
	    if (rs >= 0) {
		ip->writetime = iinfo.writetime ;
		ip->writecount = iinfo.writecount ;
		ip->entries = iinfo.entries ;
		ip->version = iinfo.version ;
		ip->encoding = iinfo.encoding ;
		ip->type = iinfo.type ;
		ip->collisions = iinfo.collisions ;
	    }
	}

	return (rs >= 0) ? n : rs ;
}
/* end subroutine (sysrealname_info) */

int sysrealname_curbegin(SRM *op,SRN_CUR *curp) noex {
	int		rs = SR_OK ;

	if (op == nullptr) return SR_FAULT ;
	if (curp == nullptr) return SR_FAULT ;

	memset(curp,0,sizeof(SRN_CUR)) ;

	if (op->call.curbegin != nullptr) {
	    char	*p ;
	    if ((rs = uc_malloc(op->cursize,&p)) >= 0) {
		curp->scp = p ;
	        if ((rs = (*op->call.curbegin)(op->obj,curp->scp)) >= 0) {
		    curp->magic = SRN_CURMAGIC ;
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
/* end subroutine (sysrealname_curbegin) */

int sysrealname_curend(SRM *op,SRN_CUR *curp) noex {
	int		rs = SR_OK ;
	int		rs1 ;

	if (op == nullptr) return SR_FAULT ;
	if (curp == nullptr) return SR_FAULT ;

	if (op->magic != SYSREALNAME_MAGIC) return SR_NOTOPEN ;
	if (curp->magic != SRN_CURMAGIC) return SR_NOTOPEN ;

	if (op->call.curend != nullptr) {
	    rs1 = (*op->call.curend)(op->obj,curp->scp) ;
	    if (rs >= 0) rs = rs1 ;
	}

	rs1 = uc_free(curp->scp) ;
	if (rs >= 0) rs = rs1 ;
	curp->scp = nullptr ;

	if (curp->sa != nullptr) {
	    rs1 = uc_free(curp->sa) ;
	    if (rs >= 0) rs = rs1 ;
	    curp->sa = nullptr ;
	}

	curp->magic = 0 ;
	return rs ;
}
/* end subroutine (sysrealname_curend) */

int sysrealname_look(SRM *op,SRN_CUR *curp,int fo,
		cchar *sbuf,int slen) noex {
	realname	rn ;
	int		rs ;

	if (sbuf == nullptr) return SR_FAULT ;

	if ((rs = realname_start(&rn,sbuf,slen)) >= 0) {
	    cchar	*sa[6] ;
	    if ((rs = realname_getpieces(&rn,sa)) > 0) {
		rs = sysrealname_lookparts(op,curp,fo,sa,rs) ;
	    }
	    realname_finish(&rn) ;
	} /* end if (realname) */

	return rs ;
}
/* end subroutine (sysrealname_look) */

int sysrealname_lookparts(SRM *op,SRN_CUR *curp,int fo,
		cchar **sa,int sn) noex {
	int		rs = SR_OK ;

	if (op == nullptr) return SR_FAULT ;
	if (curp == nullptr) return SR_FAULT ;
	if (sa == nullptr) return SR_FAULT ;

	if (op->magic != SYSREALNAME_MAGIC) return SR_NOTOPEN ;
	if (curp->magic != SRN_CURMAGIC) return SR_NOTOPEN ;

	if (op->call.fetch != nullptr) {
	    rs = sysrealname_curload(op,curp,fo,sa,sn) ;
	} else {
	    rs = SR_NOSYS ;
	}

	return rs ;
}
/* end subroutine (sysrealname_lookparts) */

int sysrealname_lookread(SRM *op,SRN_CUR *curp,char *rbuf) noex {
	int		rs = SR_OK ;

	if (op == nullptr) return SR_FAULT ;
	if (curp == nullptr) return SR_FAULT ;

	if (op->magic != SYSREALNAME_MAGIC) return SR_NOTOPEN ;
	if (curp->magic != SRN_CURMAGIC) return SR_NOTOPEN ;

	if (op->call.fetch != nullptr) {
	    IPASSWD_CUR	*icp = curp->scp ;
	    cint	rsn = SR_NOTFOUND ;
	    int		sn = curp->sn ;
	    int		fo = curp->fo;
	    cchar	**sa = curp->sa ;
	    if ((rs = (*op->call.fetch)(op->obj,icp,fo,rbuf,sa,sn)) == rsn) {
		rs = SR_OK ;
	    }
	} else {
	    rs = SR_NOSYS ;
	}

	return rs ;
}
/* end subroutine (sysrealname_lookread) */

int sysrealname_enum(SRM *op,SRN_CUR *curp,char *ubuf,
		cchar **sa,char *rbuf,int rlen) noex {
	int		rs = SR_OK ;

	if (op == nullptr) return SR_FAULT ;
	if (curp == nullptr) return SR_FAULT ;
	if (sa == nullptr) return SR_FAULT ;
	if (rbuf == nullptr) return SR_FAULT ;

	if (op->magic != SYSREALNAME_MAGIC) return SR_NOTOPEN ;
	if (curp->magic != SRN_CURMAGIC) return SR_NOTOPEN ;

	if (op->call.enumerate != nullptr) {
	    IPASSWD_CUR	*icp = curp->scp ;
	    rs = (*op->call.enumerate)(op->obj,icp,ubuf,sa,rbuf,rlen) ;
	} else {
	    rs = SR_NOTAVAIL ;
	}

	return rs ;
}
/* end subroutine (sysrealname_enum) */

int sysrealname_audit(SRM *op) noex {
	int		rs = SR_NOTAVAIL ;

	if (op == nullptr) return SR_FAULT ;

	if (op->magic != SYSREALNAME_MAGIC) return SR_NOTOPEN ;

	if (op->call.audit != nullptr) {
	    rs = (*op->call.audit)(op->obj) ;
	}

	return rs ;
}
/* end subroutine (sysrealname_audit) */


/* private subroutines */

static int sysrealname_objloadbegin(CMD *op,cchar *pr,cchar *objn) noex {
	modload		*lp = op->mlp ;
	cint		vn = sub_overlast ;
	cint		vo = VECSTR_OCOMPACT ;
	int		rs ;
	int		rs1 ;
	if (vecstr syms ; (rs = syms.start(vn,vo)) >= 0) {
	    if ((rs = syms.addsyms(objn,subs)) >= 0) {
	        if (mainv sv{} ; (rs = syms.getvec(&sv)) >= 0) {
	            cchar	*mn = CMD_MODBNAME ;
	            cchar	*on = objn ;
	            int		mo = 0 ;
	            mo |= MODLOAD_OLIBVAR ;
	            mo |= MODLOAD_OPRS ;
	            mo |= MODLOAD_OSDIRS ;
	            mo |= MODLOAD_OAVAIL ;
	            if ((rs = modload_open(lp,pr,mn,on,mo,sv)) >= 0) {
		        op->fl.modload = true ;
	                if (int mv[2] ; (rs = modload_getmva(lp,mv,2)) >= 0) {
			    cint	osz = op->objsize ;
	                    op->objsize = mv[0] ;
	                    op->cursize = mv[1] ;
			    if (void *vp{} ; (rs = uc_malloc(osz,&vp)) >= 0) {
	                        op->obj = vp ;
	                        rs = sysrealname_loadcalls(op,&syms) ;
	                        if (rs < 0) {
	                            uc_free(op->obj) ;
	                            op->obj = nullptr ;
	                        }
	                    } /* end if (memory-allocation) */
	                } /* end if (modload_getmva) */
	                if (rs < 0) {
		            op->fl.modload = false ;
	                    modload_close(lp) ;
	                }
	            } /* end if (modload_open) */
		} /* end if (vecstr_getvec) */
	    } /* end if (vecstr_addsyms) */
	    rs1 = syms.finish ;
	    if (rs >= 0) rs = rs1 ;
	    if ((rs < 0) && op->fl.modload) {
		op->fl.modload = false ;
		modload_close(lp) ;
	    }
	} /* end if (vecstr-syms) */
	return rs ;
}
/* end subroutine (sysrealname_objloadbegin) */

static int sysrealname_objloadend(CMD *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (op->obj) {
	    rs1 = uc_free(op->obj) ;
	    if (rs >= 0) rs = rs1 ;
	    op->obj = nullptr ;
	}
	if (op->mlp && op->fl.modload) {
	    op->fl.modload = false ;
	    rs1 = modload_close(op->mlp) ;
	    if (rs >= 0) rs = rs1 ;
	}
	return rs ;
}
/* end subroutine (sysrealname_objloadend) */

static int sysrealname_loadcalls(CMD *op,vecstr *slp) noex {
	modload		*lp = op->mlp ;
	cint		rsn = SR_NOTFOUND ;
	int		rs = SR_OK ;
	int		rs1 ;
	int		c = 0 ;
	cchar		*sname{} ;
	for (int i = 0 ; (rs1 = slp->get(i,&sname)) >= 0 ; i += 1) {
	    if (cvoid *snp{} ; (rs = modload_getsym(lp,sname,&snp)) >= 0) {
                sysrealname_calls   *callp = callsp(op->callp) ;
                c += 1 ;
                switch (i) {
                case sub_open:
                    callp->open = soopen_f(snp) ;
                    break ;
                case sub_info:
                    callp->info = soinfo_f(snp) ;
                    break ;
                case sub_curbegin:
                    callp->curbegin = socurbegin_f(snp) ;
                    break ;
                case sub_curenum:
                    callp->curenum = socurenum_f(snp) ;
                    break ;
                case sub_curend:
                    callp->curend = socurend_f(snp) ;
                    break ;
                case sub_fetch:
                    callp->fetch = sofetch_f(snp) ;
                    break ;
                case sub_audit:
                    callp->audit = soaudit_f(snp) ;
                    break ;
                case sub_close:
                    callp->close = soclose_f(snp) ;
                    break ;
                } /* end switch */
            } else if (rs == rsn) {
                if (! isrequired(i)) rs = SR_OK ;
            } /* end if (it had the call) */
	    if (rs < 0) break ;
	} /* end for (vecstr_get) */
	if ((rs >= 0) && (rs1 != rsn)) rs = rs1 ;
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (sysrealname_loadcalls) */

static int sysrealname_curload(SRM *op,SRN_CUR *curp,
		int fo,cc **sa,int sn) noex {
	int		rs ;
	int		i ;
	int		sasize ;
	int		ssize = 0 ;
	char		*bp ;

	if (op == nullptr) return SR_FAULT ;
	if (sa == nullptr) return SR_FAULT ;

	if (sn < 0) {
	    for (sn = 0 ; sa[sn] != nullptr ; sn += 1) ;
	}

	sasize = ((sn+1) * sizeof(cchar *)) ;
	ssize += sasize ;
	for (i = 0 ; i < sn ; i += 1) {
	    ssize += (strlen(sa[i]) + 1) ;
	}

	if ((rs = uc_malloc(ssize,&bp)) >= 0) {
	    cchar	**sap = (cchar **) bp ;
	    char	*sbp = (bp + sasize) ;
	    curp->fo = fo ;
	    curp->sa = (cchar **) bp ;
	    curp->sn = sn ;
	    for (i = 0 ; i < sn ; i += 1) {
		sap[i] = sbp ;
		sbp = (strwcpy(sbp,sa[i],-1) + 1) ;
	    }
	    sap[i] = nullptr ;
	} /* end if (memory-allocation) */

	return rs ;
}
/* end subroutine (sysrealname_curload) */

static bool isrequired(int i) noex {
	bool		f = FALSE ;
	switch (i) {
	case sub_open:
	case sub_curbegin:
	case sub_curend:
	case sub_fetch:
	case sub_close:
	    f = TRUE ;
	    break ;
	} /* end switch */
	return f ;
}
/* end subroutine (isrequired) */


