/* sysvar SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* SYSVAR management */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-12-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services
	(RNS).

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	sysvar

	Description:
	This module implements an interface (a trivial one) that
	provides access to the SYSVAR object (which is dynamically
	loaded).

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>		/* |strchr(3c)| */
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<usystem.h>
#include	<vecstr.h>
#include	<nulstr.h>
#include	<sncpyx.h>
#include	<snwcpy.h>
#include	<strxcmp.h>
#include	<matstr.h>
#include	<vstrkeycmpx.h>
#include	<isnot.h>
#include	<localmisc.h>

#include	"sysvar.h"

import libutil ;

/* local defines */

#define	SYSVAR_OBJNAME	"varmks"
#define	SYSVAR_MODBNAME	"varmks"

#define	SV		sysvar
#define	SV_DC		sysvar_defcur
#define	SV_CUR		sysvar_cur

#ifndef	VARPRLOCAL
#define	VARPRLOCAL	"LOCAL"
#endif

#ifndef	DEFINITFNAME
#define	DEFINITFNAME	"/etc/default/init"
#endif

#ifndef	DEFLOGFNAME
#define	DEFLOGFNAME	"/etc/default/login"
#endif

#define	LIBCNAME	"lib"

#define	VARLIBPATH	"LD_LIBRARY_PATH"

#define	NDEFAULTS	20


/* imported namespaces */

using std::nullptr_t ;			/* type */
using std::min ;			/* subroutine-template */
using std::max ;			/* subroutine-template */
using std::nothrow ;			/* constant */


/* local typedefs */

extern "C" {
    typedef int	(*soopen_f)(void *,cchar *,cchar *) noex ;
    typedef int	(*socount_f)(void *) noex ;
    typedef int	(*socurbegin_f)(void *,void *) noex ;
    typedef int	(*socurend_f)(void *,void *) noex ;
    typedef int	(*socurenum_f)(void *,void *,char *,int,char *,int) noex ;
    typedef int	(*sofetch_f)(void *,cchar *,int,void *,char *,int) noex ;
    typedef int	(*soaudit_f)(void *) noex ;
    typedef int	(*soclose_f)(void *) noex ;
}


/* external subroutines */


/* local structures */

struct sysvar_defcur {
	int		i ;
} ;

struct sysvar_calls {
    soopen_f		open ;
    socount_f		count ;
    socurbegin_f	curbegin ;
    socurend_f		curend ;
    socurenum_f		curenum ;
    sofetch_f		fetch ;
    soaudit_f		audit ;
    soclose_f		close ;
} ; /* end struct (sysvar_calls) */

typedef sysvar_calls *		callsp ;


/* forward references */

template<typename ... Args>
static int sysvar_ctor(SV *op,Args ... args) noex {
    	SYSVAR		*hop = op ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    cnullptr	np{} ;
	    rs = SR_NOMEM ;
	    memclear(hop) ; /* dangerous! */
	    if ((op->mlp = new(nothrow) modload) != np) {
		sysvar_calls	*callp ;
	        if ((callp = new(nothrow) sysvar_calls) != np) {
		    op->callp = callp ;
		    rs = SR_OK ;
		}
		if (rs < 0) {
		    delete op->mlp ;
		    op->mlp = nullptr ;
		}
	    } /* end new (new-modload) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (sysvar_ctor) */

static int sysvar_dtor(SV *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	    if (op->callp) {
		sysvar_calls	*callp = callsp(op->callp) ;
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
/* end subroutine (sysvar_dtor) */

template<typename ... Args>
static inline int sysvar_magic(SV *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magic == SYSVAR_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}
/* end subroutine (sysvar_magic) */

static int	sysvar_objloadbegin(SV *,cchar *,cchar *) noex ;
static int	sysvar_objloadend(SV *) noex ;
static int	sysvar_loadcalls(SV *,vecstr *) noex ;
static int	sysvar_socurbegin(SV *,SV_CUR *) noex ;
static int	sysvar_socurend(SV *,SV_CUR *) noex ;
static int	sysvar_defaults(SV *) noex ;
static int	sysvar_procsysdef(SV *,cchar *) noex ;
static int	sysvar_defcurbegin(SV *,SV_CUR *) noex ;
static int	sysvar_defcurend(SV *,SV_CUR *) noex ;
static int	sysvar_deffetch(SV *,cchar *,int,SV_DC *,char *,int) noex ;
static int	sysvar_defenum(SV *,SV_DC *,char *,int,char *,int) noex ;

static bool	isrequired(int) noex ;


/* external variables */


/* local variables */

enum subs {
	sub_open,
	sub_count,
	sub_curbegin,
	sub_curenum,
	sub_curend,
	sub_fetch,
	sub_audit,
	sub_close,
	sub_overlast
} ;

constexpr cpcchar	subs[] = {
	"open",
	"count",
	"curbegin",
	"curenum",
	"curend",
	"fetch",
	"audit",
	"close",
	nullptr
} ;

constexpr cpcchar	sysfnames[] = {
	DEFINITFNAME,	
	DEFLOGFNAME,
	nullptr
} ;

constexpr cpcchar	wstrs[] = {
	"TZ",
	"LANG",
	"UMASK",
	"PATH",
	"WSTATION",
	nullptr
} ;

constexpr cpcchar	pstrs[] = {
	"LC_",
	nullptr
} ;


/* exported variables */


/* exported subroutines */

int sysvar_open(SV *op,cchar *pr,cchar *dbname) noex {
	int		rs ;
	if ((rs = sysvar_ctor(op,pr)) >= 0) {
	    rs = SR_INVALID ;
	    if (pr[0]) {
	        cchar	*objn = SYSVAR_OBJNAME ;
	        if ((rs = sysvar_objloadbegin(op,pr,objn)) >= 0) {
	            sysvar_calls	*callp = callsp(op->callp) ;
		    if (op->callp) {
			if (callp->open) {
			    auto co = callp->open ;
	                    if ((rs = co(op->obj,pr,dbname)) >= 0) {
	    	                op->magic = SYSVAR_MAGIC ;
	            	    }
		        } else {
			    rs = SR_NOTOPEN ;
			}
		    } else {
			rs = SR_NOTSOCK ;
		    }
	            if (rs < 0) {
		        sysvar_objloadend(op) ;
	            }
	        } /* end if */
		if (isNotPresent(rs)) {
	    	    if ((rs = sysvar_defaults(op)) >= 0) {
	    		op->magic = SYSVAR_MAGIC ;
		    }
		}
	    } /* end if (valid) */
	    if (rs < 0) {
		sysvar_dtor(op) ;
	    }
	} /* end if (sysvar_ctor) */
	return rs ;
}
/* end subroutine (sysvar_open) */

int sysvar_close(SV *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = sysvar_magic(op)) >= 0) {
	    if (op->fl.defaults) {
	        rs1 = vecstr_finish(op->dlp) ;
	        if (rs >= 0) rs = rs1 ;
	    } else if (op->callp) {
	        sysvar_calls	*callp = callsp(op->callp) ;
		if (callp->close) {
		    auto co = callp->close ;
	            rs = co(op->obj) ;
		}
		{
	            rs1 = sysvar_objloadend(op) ;
	            if (rs >= 0) rs = rs1 ;
		}
	    } /* end if */
	    {
		rs1 = sysvar_dtor(op) ;
		if (rs >= 0) rs = rs1 ;
	    }
	    op->magic = 0 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (sysvar_close) */

int sysvar_audit(SV *op) noex {
	int		rs ;
	if ((rs = sysvar_magic(op)) >= 0) {
	    sysvar_calls	*callp = callsp(op->callp) ;
	    rs = SR_NOSYS ;
	    if (op->fl.defaults) {
	        rs = vecstr_audit(op->dlp) ;
	    } else if (callp->audit) {
		auto co = callp->audit ;
	        rs = co(op->obj) ;
	    }
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (sysvar_audit) */

int sysvar_count(SV *op) noex {
	int		rs ;
	if ((rs = sysvar_magic(op)) >= 0) {
	    sysvar_calls	*callp = callsp(op->callp) ;
	    rs = SR_NOSYS ;
	    if (op->fl.defaults) {
	        rs = vecstr_count(op->dlp) ;
	    } else if (callp->count) {
		auto co = callp->audit ;
	        rs = co(op->obj) ;
	    }
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (sysvar_count) */

int sysvar_curbegin(SV *op,sysvar_cur *curp) noex {
	int		rs ;
	if ((rs = sysvar_magic(op,curp)) >= 0) {
	    memclear(curp) ;
	    if (op->fl.defaults) {
	        rs = sysvar_defcurbegin(op,curp) ;
	    } else {
	        rs = sysvar_socurbegin(op,curp) ;
	    }
	    if (rs >= 0) {
	        curp->magic = SYSVAR_MAGIC ;
	    }
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (sysvar_curbegin) */

int sysvar_curend(SV *op,sysvar_cur *curp) noex {
	int		rs ;
	if ((rs = sysvar_magic(op,curp)) >= 0) {
	    rs = SR_NOTOPEN ;
	    if (curp->magic == SYSVAR_MAGIC) {
	        if (op->fl.defaults) {
	            rs = sysvar_defcurend(op,curp) ;
	        } else {
	            rs = sysvar_socurend(op,curp) ;
	        }
	        curp->magic = 0 ;
	    } /* end if (valid) */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (sysvar_curend) */

int sysvar_fetch(SV *op,cchar *kp,int kl,SV_CUR *curp,
		char *vbuf,int vlen) noex {
	int		rs ;
	if ((rs = sysvar_magic(op,kp,curp)) >= 0) {
	    rs = SR_NOTOPEN ;
	    if (curp->magic == SYSVAR_MAGIC) {
		sysvar_calls	*callp = callsp(op->callp) ;
		rs = SR_NOSYS ;
	        if (op->fl.defaults) {
		    SV_DC	*dcurp = (SV_DC *) curp->scp ;
	            rs = sysvar_deffetch(op,kp,kl,dcurp,vbuf,vlen) ;
	        } else if (callp->fetch) {
		    auto co = callp->fetch ;
	    	    rs = co(op->obj,kp,kl,curp->scp,vbuf,vlen) ;
		}
	    } /* end if (valid) */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (sysvar_fetch) */

int sysvar_curenum(SV *op,SV_CUR *curp,char *kbuf,int klen,
		char *vbuf,int vlen) noex {
	int		rs ;
	if ((rs = sysvar_magic(op,curp,kbuf)) >= 0) {
	    rs = SR_NOTOPEN ;
	    if (curp->magic == SYSVAR_MAGIC) {
		sysvar_calls	*callp = callsp(op->callp) ;
		rs = SR_NOSYS ;
	        if (op->fl.defaults) {
		    SV_DC	*dcurp = (SV_DC *) curp->scp ;
	            rs = sysvar_defenum(op,dcurp,kbuf,klen,vbuf,vlen) ;
	        } else if (callp->curenum) {
		    auto co = callp->curenum ;
	            rs = co(op->obj,curp->scp,kbuf,klen,vbuf,vlen) ;
	        }

	    } /* end if (valid) */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (sysvar_curenum) */


/* private subroutines */

static int sysvar_objloadbegin(SV *op,cchar *pr,cchar *objn) noex {
	modload		*lp = op->mlp ;
	cint		vn = sub_overlast ;
	cint		vo = VECSTR_OCOMPACT ;
	int		rs ;
	int		rs1 ;
	if (vecstr syms ; (rs = syms.start(vn,vo)) >= 0) {
	    if ((rs = syms.addsyms(objn,subs)) >= 0) {
	        if (mainv sv{} ; (rs = syms.getvec(&sv)) >= 0) {
	            cchar	*mn = SYSVAR_MODBNAME ;
	            cchar	*on = objn ;
	            int		mo = 0 ;
	            mo |= modloadm.libvar ;
	            mo |= modloadm.libprs ;
	            mo |= modloadm.libsdirs ;
	            mo |= modloadm.avail ;
	            if ((rs = modload_open(lp,pr,mn,on,mo,sv)) >= 0) {
		        op->fl.modload = true ;
	                if (int mv[2] ; (rs = modload_getmva(lp,mv,1)) >= 0) {
			    cint	osz = op->objsize ;
	                    op->objsize = mv[0] ;
			    if (void *vp{} ; (rs = uc_malloc(osz,&vp)) >= 0) {
	                        op->obj = vp ;
	                        rs = sysvar_loadcalls(op,&syms) ;
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
/* end subroutine (sysvar_objloadbegin) */

static int sysvar_objloadend(SV *op) noex {
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
/* end subroutine (sysvar_objloadend) */

static int sysvar_loadcalls(SV *op,vecstr *slp) noex {
	modload		*lp = op->mlp ;
	cint		rsn = SR_NOTFOUND ;
	int		rs = SR_OK ;
	int		rs1 ;
	int		c = 0 ;
	cchar		*sname{} ;
	for (int i = 0 ; (rs1 = slp->get(i,&sname)) >= 0 ; i += 1) {
	    if (cvoid *snp{} ; (rs = modload_getsym(lp,sname,&snp)) >= 0) {
                sysvar_calls        *callp = callsp(op->callp) ;
                c += 1 ;
                switch (i) {
                case sub_open:
                    callp->open = soopen_f(snp) ;
                    break ;
                case sub_count:
                    callp->count = socount_f(snp) ;
                    break ;
                case sub_curbegin:
                    callp->curbegin = socurbegin_f(snp) ;
                    break ;
                case sub_curenum:
                    callp->curenum = socurenum_f(snp) ;
                    break ;
                case sub_fetch:
                    callp->fetch = sofetch_f(snp) ;
                    break ;
                case sub_curend:
                    callp->curend = socurend_f(snp) ;
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
/* end subroutine (sysvar_loadcalls) */

static int sysvar_socurbegin(SV *op,SV_CUR *curp) noex {
	int		rs = SR_BUGCHECK ;
	if (op->callp) {
	    cint	sz = op->cursize ;
	    if (void *vp{} ; (rs = uc_malloc(sz,&vp)) >= 0) {
		sysvar_calls	*callp = callsp(op->callp) ;
		curp->scp = vp ;
		rs = SR_NOSYS ;
		if (callp->curbegin) {
		    auto co = callp->curbegin ;
		    rs = co(op->obj,curp->scp) ;
		}
		if (rs < 0) {
	    	    uc_free(curp->scp) ;
	    	    curp->scp = nullptr ;
		}
	    } /* end if (m-a) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (sysvar_socurbegin) */

static int sysvar_socurend(SV *op,SV_CUR *curp) noex {
	int		rs = SR_BUGCHECK ;
	int		rs1 ;
	if (curp->scp) {
	    if (op->callp) {
		sysvar_calls	*callp = callsp(op->callp) ;
		if (callp->curend) {
		    auto	co = callp->curend ;
	            rs1 = co(op->obj,curp->scp) ;
	            if (rs >= 0) rs = rs1 ;
		}
		{
	            rs1 = uc_free(curp->scp) ;
	            if (rs >= 0) rs = rs1 ;
	            curp->scp = nullptr ;
		}
	    } else {
	        rs = SR_NOSYS ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (sysvar_socurend) */

static int sysvar_defaults(SV *op) noex {
	int		rs ;
	if ((rs = vecstr_start(op->dlp,NDEFAULTS,0)) >= 0) {
	    op->fl.defaults = true ;
	    for (int i = 0 ; sysfnames[i] != nullptr ; i += 1) {
	        bool	f = false ;
	        rs = sysvar_procsysdef(op,sysfnames[i]) ;
	        f = f || (rs == SR_NOENT) ;
	        f = f || (rs == SR_ACCESS) ;
	        if ((rs < 0) && (! f)) break ;
	    } /* end for */
	    if (rs >= 0) {
	        vecstr_sort(op->dlp,vstrkeycmp) ;
	    }
	    if (rs < 0) {
		op->fl.defaults = false ;
		vecstr_finish(op->dlp) ;
	    }
	} /* end if (vecstr_start) */
	return rs ;
}
/* end subroutine (sysvar_defaults) */

static int sysvar_procsysdef(SV *op,cchar *fname) noex {
    	cnullptr	np{} ;
    	cint		vn = 10 ;
	cint		vo = 0 ;
	cint		rsn = SR_NOTFOUND ;
	int		rs ;
	int		rs1 ;
	int		rs2 ;
	if (vecstr lvars ; (rs = lvars.start(vn,vo)) >= 0) {
	    if ((rs = lvars.envfile(fname)) >= 0) {
		cchar	*cp{} ;
	        for (int i = 0 ; (rs2 = lvars.get(i,&cp)) >= 0 ; i += 1) {
	            if (cp) {
	                if (cchar *tp ; (tp = strchr(cp,'=')) != np) {
			    cint tl = intconv(tp - cp) ;
			    {
	                        bool	f = (matstr(wstrs,cp,tl) >= 0) ;
	                        f = f || (matpstr(pstrs,10,cp,tl) >= 0) ;
	                        if (f) {
	                            rs = vecstr_adduniq(op->dlp,cp,-1) ;
	                        } /* end if */
			    } /* end block */
		         } /* end if (strchr) */
		    } /* end if (non-null) */
		    if (rs < 0) break ;
	        } /* end for */
		if ((rs >= 0) && (rs2 != rsn)) rs = rs2 ;
	    } /* end if (vecstr_envfile) */
	    rs1 = lvars.finish ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (lvars) */
	return rs ;
}
/* end subroutine (sysvar_procsysdef) */

static int sysvar_defcurbegin(SV *op,SV_CUR *curp) noex {
	int		rs = SR_BUGCHECK ;
	if (op && curp) {
	    cint	sz = szof(SV_DC) ;
	    if (void *vp{} ; (rs = uc_malloc(sz,&vp)) >= 0) {
	        SV_DC	*dcp = (SV_DC *) vp ;
	        curp->scp = vp ;
	        dcp->i = -1 ;
	    } /* end if (memory-allocation) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (sysvar_defcurbegin) */

static int sysvar_defcurend(SV *op,SV_CUR *curp) noex {
	int		rs = SR_BUGCHECK ;
	int		rs1 ;
	if (op && curp) {
	    if (curp->scp) {
	    	rs = SR_OK ;
		{
	            rs1 = uc_free(curp->scp) ;
	            if (rs >= 0) rs = rs1 ;
	            curp->scp = nullptr ;
		}
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (sysvar_defcurend) */

static int sysvar_deffetch(SV *op,cc *kp,int kl,SV_DC *dcp,
		char *vbuf,int vlen) noex {
	int		rs ;
	int		rs1 ;
	int		vl = 0 ;
	cchar		*key{} ;
	if (vbuf) vbuf[0] = '\0' ;
	if (nulstr ns ; (rs = ns.start(kp,kl,&key)) >= 0) {
	    int		i = (dcp->i >= 0) ? (dcp->i + 1) : 0 ;
	    cchar	*cp ; /* multiple-use */
	    while ((rs = vecstr_get(op->dlp,i,&cp)) >= 0) {
	        if (strkeycmp(key,cp) == 0) break ;
	        i += 1 ;
	    } /* end while */
	    if (rs >= 0) {
	        cchar	*valp = nullptr ;
	        if (cchar *tp{} ; (tp = strchr(cp,'=')) != nullptr) {
		    valp = (tp + 1) ;
		}
	        if (valp) {
	            if (vbuf) {
		        rs = sncpy1(vbuf,vlen,valp) ;
		        vl = rs ;
	            } else {
		        vl = lenstr(valp) ;
		    }
	        }
	        if (rs >= 0) {
	            dcp->i = i ;
		}
	    } /* end if */
	    rs1 = ns.finish ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (nul-string) */
	return (rs >= 0) ? vl : rs ;
}
/* end subroutine (sysvar_deffetch) */

static int sysvar_defenum(SV *op,SV_DC *dcp,char *kbuf,int klen,
		char *vbuf,int vlen) noex {
	int		rs = SR_OK ;
	int		i = (dcp->i >= 0) ? (dcp->i + 1) : 0 ;
	int		vl = 0 ;
	cchar		*cp{} ;
	if (vbuf) vbuf[0] = '\0' ;
	while ((rs = vecstr_get(op->dlp,i,&cp)) >= 0) {
	    if (cp != nullptr) break ;
	    i += 1 ;
	} /* end while */
	if (rs >= 0) {
	    int		kl = -1 ;
	    cchar	*valp = nullptr ;
	    if (cchar *tp ; (tp = strchr(cp,'=')) != nullptr) {
		valp = (tp + 1) ;
		kl = intconv(tp - cp) ;
	    }
	    if (kbuf) {
		rs = snwcpy(kbuf,klen,cp,kl) ;
	    }
	    if ((rs >= 0) && valp) {
	        if (vbuf) {
		    rs = sncpy1(vbuf,vlen,valp) ;
		    vl = rs ;
	        } else {
		    vl = lenstr(valp) ;
		}
	    }
	    if (rs >= 0) {
	        dcp->i = i ;
	    }
	} else if (vbuf) {
	    vbuf[0] = '\0' ;
	}
	return (rs >= 0) ? vl : rs ;
}
/* end subroutine (sysvar_defenum) */

static bool isrequired(int i) noex {
	bool		f = false ;
	switch (i) {
	case sub_open:
	case sub_curbegin:
	case sub_fetch:
	case sub_curenum:
	case sub_curend:
	case sub_close:
	    f = true ;
	    break ;
	} /* end switch */
	return f ;
}
/* end subroutine (isrequired) */


