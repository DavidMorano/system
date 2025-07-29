/* sysrealname SUPPORT */
/* charset=ISO8859-1 */
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
#include	<unistd.h>
#include	<fcntl.h>
#include	<dlfcn.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<new>
#include	<usystem.h>
#include	<vecstr.h>
#include	<realname.h>
#include	<modload.h>
#include	<ipasswd.h>
#include	<strwcpy.h>
#include	<localmisc.h>

#include	"sysrealname.h"

import libutil ;

/* local defines */

#define	SRN		sysrealname
#define	SRN_FL		sysrealname_fl
#define	SRN_CUR		sysrealname_cur
#define	SRN_INFO	sysrealname_info
#define	SRN_MODBNAME	"ipasswd"
#define	SRN_OBJNAME	"ipasswd"
#define	SRN_CURMAGIC	SYSREALNAME_CURMAGIC

#define	IPW_CUR		ipasswd_cur
#define	IPW_INFO	ipasswd_info

#undef	SMM_INFO
#define	SMM_INFO	IPASSWD_INFO

#define	LIBCNAME	"lib"

#ifndef	VARLIBPATH
#define	VARLIBPATH	"LD_LIBRARY_PATH"
#endif


/* imported namespaces */

using std::nullptr_t ;			/* type */
using std::min ;			/* subroutine-template */
using std::max ;			/* subroutine-template */
using std::nothrow ;			/* constant */


/* local typedefs */

extern "C" {
    typedef int (*soopen_f)(void *,cchar *) noex ;
    typedef int (*sogetinfo_f)(void *,IPW_INFO *) noex ;
    typedef int (*socurbegin_f)(void *,IPW_CUR *) noex ;
    typedef int (*socurend_f)(void *,IPW_CUR *) noex ;
    typedef int (*socurenum_f)(void *,IPW_CUR *,char *,cc **,char *,int) noex ;
    typedef int (*socurfetch_f)(void *,IPW_CUR *,int,char *,cc **,int) noex ;
    typedef int (*soaudit_f)(void *) noex ;
    typedef int (*soclose_f)(void *) noex ;
}


/* external subroutines */


/* external variables */


/* local structures */

struct sysrealname_calls {
	soopen_f	open ;
	sogetinfo_f	getinfo ;
	socurbegin_f	curbegin ;
	socurend_f	curend ;
	socurenum_f	curenum ;
	socurfetch_f	curfetch ;
	soaudit_f	audit ;
	soclose_f	close ;
} ; /* end struct (sysrealaname_calls) */

typedef sysrealname_calls *		callsp ;


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

static int	sysrealname_objloadbegin(SRN *,cchar *,cchar *) noex ;
static int	sysrealname_objloadend(SRN *) noex ;
static int	sysrealname_loadcalls(SRN *,vecstr *) noex ;

static int	sysrealname_curload(SRN *,SRN_CUR *,
			int,cchar **,int) noex ;

static bool	isrequired(int) noex ;


/* external variables */


/* local variables */

enum subs {
	sub_open,
	sub_getinfo,
	sub_curbegin,
	sub_curend,
	sub_curenum,
	sub_curfetch,
	sub_audit,
	sub_close,
	sub_overlast
} ;

constexpr cpcchar	subs[] = {
	"open",
	"getinfo",
	"curbegin",
	"curend",
	"curenum",
	"curfetch",
	"audit",
	"close",
	nullptr
} ;

constexpr char		extradir[] = "/usr/extra" ;


/* exported variables */


/* exported subroutines */

int sysrealname_open(SRN *op,cchar *dbname) noex {
	int		rs ;
	if (dbname == nullptr) dbname = SYSREALNAME_DBNAME ;
	if ((rs = sysrealname_ctor(op)) >= 0) ylikely {
	    cchar	*objname = SRN_OBJNAME ;
	    cchar	*pr = extradir ;
	    if ((rs = sysrealname_objloadbegin(op,pr,objname)) >= 0) ylikely {
                sysrealname_calls    *callp = callsp(op->callp) ;
		rs = SR_NOSYS ;
		if (op->callp) {
		    cauto co = callp->open ;
	            if ((rs = co(op->obj,dbname)) >= 0) {
	                op->magic = SYSREALNAME_MAGIC ;
	            }
		}
	    } /* end if (sysrealname_objloadbegin) */
	    if (rs < 0) {
		sysrealname_dtor(op) ;
	    }
	} /* end if (sysrealname_ctor) */
	return rs ;
}
/* end subroutine (sysrealname_open) */

int sysrealname_close(SRN *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = sysrealname_magic(op)) >= 0) ylikely {
            sysrealname_calls    *callp = callsp(op->callp) ;
	    rs = SR_NOSYS ;
	    if (callp->close) {
		cauto co = callp->close ;
	        rs1 = co(op->obj) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = sysrealname_objloadend(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
		rs1 = sysrealname_dtor(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    op->magic = 0 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (sysrealname_close) */

int sysrealname_getinfo(SRN *op,SRN_INFO *ip) noex {
	int		rs ;
	int		n = 0 ;
	if ((rs = sysrealname_magic(op)) >= 0) ylikely {
            sysrealname_calls    *callp = callsp(op->callp) ;
	    IPW_INFO		iinfo{} ;
	    rs = SR_NOSYS ;
	    if (callp->getinfo) {
		cauto co = callp->getinfo ;
	        rs = co(op->obj,&iinfo) ;
	        n = rs ;
	    }
	    if (ip) ylikely {
	        memclear(ip) ;
	        if (rs >= 0) ylikely {
		    ip->writetime = iinfo.writetime ;
		    ip->writecount = iinfo.writecount ;
		    ip->entries = iinfo.entries ;
		    ip->version = iinfo.version ;
		    ip->encoding = iinfo.encoding ;
		    ip->type = iinfo.type ;
		    ip->collisions = iinfo.collisions ;
	        }
	    }
	} /* end if (magic) */
	return (rs >= 0) ? n : rs ;
}
/* end subroutine (sysrealname_getinfo) */

int sysrealname_curbegin(SRN *op,SRN_CUR *curp) noex {
	int		rs ;
	if ((rs = sysrealname_magic(op,curp)) >= 0) ylikely {
            sysrealname_calls	*callp = callsp(op->callp) ;
	    cint		csz = op->cursize ;
	    rs = SR_NOSYS ;
	    memclear(curp) ;
	    if (callp->curbegin) {
	        if (void *vp ; (rs = uc_malloc(csz,&vp)) >= 0) {
		    cauto co = callp->curbegin ;
		    IPW_CUR	*icurp = (IPW_CUR *) curp->scp ;
		    curp->scp = vp ;
	            if ((rs = co(op->obj,icurp)) >= 0) {
		        curp->magic = SRN_CURMAGIC ;
		    }
		    if (rs < 0) {
		        uc_free(curp->scp) ;
		        curp->scp = nullptr ;
		    }
	        } /* end if (memory-allocation) */
	    }
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (sysrealname_curbegin) */

int sysrealname_curend(SRN *op,SRN_CUR *curp) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = sysrealname_magic(op,curp)) >= 0) ylikely {
            sysrealname_calls	*callp = callsp(op->callp) ;
	    rs = SR_NOTOPEN ;
	    if (curp->magic == SRN_CURMAGIC) {
		rs = SR_OK ;
	        if (callp->curend) {
		    cauto 	co = callp->curend ;
		    IPW_CUR	*icurp = (IPW_CUR *) curp->scp ;
		    if (icurp) {
	                rs1 = co(op->obj,icurp) ;
	                if (rs >= 0) rs = rs1 ;
		    }
	        } else {
		    rs = SR_NOSYS ;
		}
	        if (curp->scp) {
	            rs1 = uc_free(curp->scp) ;
	            if (rs >= 0) rs = rs1 ;
	            curp->scp = nullptr ;
	        }
	        if (curp->sa) {
	            rs1 = uc_free(curp->sa) ;
	            if (rs >= 0) rs = rs1 ;
	            curp->sa = nullptr ;
	        }
		curp->magic = 0 ;
	    } /* end if (valid-magic) */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (sysrealname_curend) */

int sysrealname_curlook(SRN *op,SRN_CUR *curp,int fo,
		cchar *sbuf,int slen) noex {
	int		rs ;
	int		rs1 ;
	int		rv = 0 ;
	if ((rs = sysrealname_magic(op,curp,sbuf)) >= 0) ylikely {
	    if (realname rn ; (rs = rn.start(sbuf,slen)) >= 0) {
	        cchar	*sa[6] ;
	        if ((rs = rn.getpieces(sa)) > 0) {
		    rs = sysrealname_curlookparts(op,curp,fo,sa,rs) ;
		    rv = rs ;
	        }
	        rs1 = rn.finish ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (realname) */
	} /* end if (magic) */
	return (rs >= 0) ? rv : rs ;
}
/* end subroutine (sysrealname_curlook) */

int sysrealname_curlookparts(SRN *op,SRN_CUR *curp,int fo,
		cchar **sa,int sn) noex {
	int		rs ;
	if ((rs = sysrealname_magic(op,curp,sa)) >= 0) ylikely {
	    rs = SR_NOTOPEN ;
	    if (curp->magic == SRN_CURMAGIC) {
	        rs = sysrealname_curload(op,curp,fo,sa,sn) ;
	    } /* end if (valid) */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (sysrealname_curlookparts) */

int sysrealname_curlookread(SRN *op,SRN_CUR *curp,char *rbuf) noex {
	int		rs ;
	if ((rs = sysrealname_magic(op,curp,rbuf)) >= 0) ylikely {
            sysrealname_calls	*callp = callsp(op->callp) ;
	    rs = SR_NOTOPEN ;
	    if (curp->magic == SRN_CURMAGIC) {
		rs = SR_NOSYS ;
	        if (callp->curfetch) {
		    cauto	co = callp->curfetch ;
	            IPW_CUR	*icp = (IPW_CUR *) curp->scp ;
	            cint	rsn = SR_NOTFOUND ;
	            cint	sn = curp->sn ;
	            cint	fo = curp->fo ;
	            cchar	**sa = curp->sa ;
	            if ((rs = co(op->obj,icp,fo,rbuf,sa,sn)) == rsn) {
		        rs = SR_OK ;
	            }
	        }
	    } /* end if (valid) */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (sysrealname_curlookread) */

int sysrealname_curenum(SRN *op,SRN_CUR *curp,char *ubuf,
		cchar **sa,char *rbuf,int rlen) noex {
	int		rs ;
	if ((rs = sysrealname_magic(op,curp,ubuf,sa,rbuf)) >= 0) ylikely {
            sysrealname_calls	*callp = callsp(op->callp) ;
	    rs = SR_NOTOPEN ;
	    if (curp->magic == SRN_CURMAGIC) {
		rs = SR_NOSYS ;
	        if (callp->curenum) {
		    cauto 	co = callp->curenum ;
	            IPW_CUR	*icurp = (IPW_CUR *) curp->scp ;
	            rs = co(op->obj,icurp,ubuf,sa,rbuf,rlen) ;
		}
	    } /* end if (valid) */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (sysrealname_curenum) */

int sysrealname_audit(SRN *op) noex {
	int		rs ;
	if ((rs = sysrealname_magic(op)) >= 0) ylikely {
            sysrealname_calls	*callp = callsp(op->callp) ;
	    rs = SR_NOSYS ;
	    if (callp->audit) {
		cauto co = callp->audit ;
	        rs = co(op->obj) ;
	    } /* end if (valid) */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (sysrealname_audit) */


/* private subroutines */

static int sysrealname_objloadbegin(SRN *op,cchar *pr,cchar *objn) noex {
	modload		*lp = op->mlp ;
	cint		vn = sub_overlast ;
	cint		vo = VECSTR_OCOMPACT ;
	int		rs ;
	int		rs1 ;
	if (vecstr syms ; (rs = syms.start(vn,vo)) >= 0) ylikely {
	    if ((rs = syms.addsyms(objn,subs)) >= 0) {
	        if (mainv sv{} ; (rs = syms.getvec(&sv)) >= 0) {
	            cchar	*mn = SRN_MODBNAME ;
	            cchar	*on = objn ;
	            int		mo = 0 ;
	            mo |= modloadm.libvar ;
	            mo |= modloadm.libprs ;
	            mo |= modloadm.libsdirs ;
	            mo |= modloadm.avail ;
	            if ((rs = modload_open(lp,pr,mn,on,mo,sv)) >= 0) {
		        op->fl.modload = true ;
	                if (int mv[2] ; (rs = modload_getmva(lp,mv,2)) >= 0) {
			    cint	osz = op->objsize ;
	                    op->objsize = mv[0] ;
	                    op->cursize = mv[1] ;
			    if (void *vp ; (rs = uc_malloc(osz,&vp)) >= 0) {
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

static int sysrealname_objloadend(SRN *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (op->obj) ylikely {
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

static int sysrealname_loadcalls(SRN *op,vecstr *slp) noex {
        sysrealname_calls   *callp = callsp(op->callp) ;
	modload		*lp = op->mlp ;
	cint		rsn = SR_NOTFOUND ;
	int		rs = SR_OK ;
	int		rs1 ;
	int		c = 0 ;
	cchar		*sname{} ;
	for (int i = 0 ; (rs1 = slp->get(i,&sname)) >= 0 ; i += 1) {
	    if (cvoid *snp{} ; (rs = modload_getsym(lp,sname,&snp)) >= 0) {
                c += 1 ;
                switch (i) {
                case sub_open:
                    callp->open = soopen_f(snp) ;
                    break ;
                case sub_getinfo:
                    callp->getinfo = sogetinfo_f(snp) ;
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
                case sub_curfetch:
                    callp->curfetch = socurfetch_f(snp) ;
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

static int sysrealname_curload(SRN *op,SRN_CUR *curp,
		int fo,cc **sa,int sn) noex {
	int		rs = SR_FAULT ;
	if (op && sa) ylikely {
	    int		sasize ;
	    int		ssize = 0 ;
	    if (sn < 0) {
	        for (sn = 0 ; sa[sn] != nullptr ; sn += 1) ;
	    }
	    sasize = ((sn + 1) * szof(cchar *)) ;
	    ssize += sasize ;
	    for (int i = 0 ; i < sn ; i += 1) {
	        ssize += (lenstr(sa[i]) + 1) ;
	    }
	    if (char *bp ; (rs = uc_malloc(ssize,&bp)) >= 0) {
	        int	i{} ; /* used-afterwards */
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
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (sysrealname_curload) */

static bool isrequired(int i) noex {
	bool		f = false ;
	switch (i) {
	case sub_open:
	case sub_curbegin:
	case sub_curend:
	case sub_curfetch:
	case sub_close:
	    f = true ;
	    break ;
	} /* end switch */
	return f ;
}
/* end subroutine (isrequired) */


