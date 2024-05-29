/* sysvar SUPPORT */
/* lang=C++20 */

/* SYSVAR management */
/* version %I% last-modified %G% */

#define	CF_DEBUGS	0		/* non-switchable debug print-outs */
#define	CF_LOOKSELF	0		/* try searching "SELF" for SO */

/* revision history:

	= 1998-12-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services
	(RNS).

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This module implements an interface (a trivial one) that
	provides access to the SYSVAR object (which is dynamically
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
#include	<ids.h>
#include	<vecstr.h>
#include	<nulstr.h>
#include	<storebuf.h>
#include	<dirseen.h>
#include	<isnot.h>
#include	<localmisc.h>

#include	"sysvar.h"


/* local defines */

#define	SYSVAR_DEFCUR	struct sysvar_defcur
#define	SYSVAR_OBJNAME	"varmks"
#define	SYSVAR_MODBNAME	"varmks"

#define	SV_DC		SYSVAR_DEFCUR

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

#ifndef	SYMNAMELEN
#define	SYMNAMELEN	60
#endif

#define	NDEFAULTS	20


/* external subroutines */

extern int	sncpy1(char *,int,cchar *) ;
extern int	sncpy3(char *,int,cchar *,cchar *,cchar *) ;
extern int	snwcpy(char *,int,cchar *,int) ;
extern int	mkpath1(char *,cchar *) ;
extern int	mkpath1w(char *,cchar *,int) ;
extern int	mkpath2(char *,cchar *,cchar *) ;
extern int	mkpath3(char *,cchar *,cchar *,cchar *) ;
extern int	mkpath4(char *,cchar *,cchar *,cchar *,
			cchar *) ;
extern int	mkfnamesuf1(char *,cchar *,cchar *) ;
extern int	matstr(cchar **,cchar *,int) ;
extern int	matostr(cchar **,int,cchar *,int) ;
extern int	matpstr(cchar **,int,cchar *,int) ;
extern int	nleadstr(cchar *,cchar *,int) ;
extern int	sperm(IDS *,USTAT *,int) ;
extern int	pathclean(char *,cchar *,int) ;
extern int	strkeycmp(cchar *,cchar *) ;
extern int	vecstr_adduniq(vecstr *,cchar *,int) ;
extern int	vecstr_envfile(vecstr *,cchar *) ;
extern int	vstrkeycmp(cchar **,cchar **) ;
extern int	getnodedomain(char *,char *) ;
extern int	mkpr(cchar *,int,cchar *,cchar *) ;

#if	CF_DEBUGS
extern int	debugprintf(cchar *,...) ;
extern int	strlinelen(cchar *,int,int) ;
#endif

extern cchar	*getourenv(cchar **,cchar *) ;

extern char	*strwcpy(char *,cchar *,int) ;


/* local structures */

struct sysvar_defcur {
	int		i ;
} ;


/* forward references */

static int	sysvar_objloadbegin(sysvar *,cchar *,cchar *) noex ;
static int	sysvar_objloadend(sysvar *) noex ;
static int	sysvar_loadcalls(sysvar *,cchar *) noex ;
static int	sysvar_socurbegin(sysvar *,SYSVAR_CUR *) noex ;
static int	sysvar_socurend(sysvar *,SYSVAR_CUR *) noex ;
static int	sysvar_defaults(sysvar *) noex ;
static int	sysvar_procsysdef(sysvar *,cchar *) noex ;
static int	sysvar_defcurbegin(sysvar *,SYSVAR_CUR *) noex ;
static int	sysvar_defcurend(sysvar *,SYSVAR_CUR *) noex ;
static int	sysvar_deffetch(sysvar *,cchar *,int,
			SV_DEF *,char *,int) noex ;
static int	sysvar_defenum(sysvar *,SYSVAR_DEFCUR *,
			char *,int,char *,int) noex ;

static bool	isrequired(int) noex ;


/* external variables */


/* local variables */

enum subs {
	sub_open,
	sub_count,
	sub_curbegin,
	sub_fetch,
	sub_enum,
	sub_curend,
	sub_audit,
	sub_close,
	sub_overlast
} ;

static constexpr cpcchar	subs[] = {
	"open",
	"count",
	"curbegin",
	"fetch",
	"enum",
	"curend",
	"audit",
	"close",
	nullptr
} ;

static constexpr cpcchar	sysfnames[] = {
	DEFINITFNAME,	
	DEFLOGFNAME,
	nullptr
} ;

static constexpr cpcchar	wstrs[] = {
	"TZ",
	"LANG",
	"UMASK",
	"PATH",
	"WSTATION",
	nullptr
} ;

static constexpr cpcchar	pstrs[] = {
	"LC_",
	nullptr
} ;


/* exported variables */


/* exported subroutines */

int sysvar_open(sysvar *op,cchar *pr,cchar *dbname) noex {
	int		rs ;
	cchar	*objname = SYSVAR_OBJNAME ;

	if (op == nullptr) return SR_FAULT ;
	if (pr == nullptr) return SR_FAULT ;

	if (pr[0] == '\0') return SR_INVALID ;

#if	CF_DEBUGS
	debugprintf("sysvar_open: pr=%s dbname=%s\n",pr,dbname) ;
#endif

	memset(op,0,sizeof(SYSVAR)) ;

	if ((rs = sysvar_objloadbegin(op,pr,objname)) >= 0) {
	    if ((rs = (*op->call.open)(op->obj,pr,dbname)) >= 0) {
	    	op->magic = SYSVAR_MAGIC ;
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

#if	CF_DEBUGS
	debugprintf("sysvar_open: ret rs=%d\n",rs) ;
#endif

	return rs ;
}
/* end subroutine (sysvar_open) */

int sysvar_close(sysvar *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;

	if (op == nullptr) return SR_FAULT ;

	if (op->magic != SYSVAR_MAGIC) return SR_NOTOPEN ;

	if (op->f.defaults) {
	    rs1 = vecstr_finish(&op->defaults) ;
	    if (rs >= 0) rs = rs1 ;
	} else {
	    rs = (*op->call.close)(op->obj) ;
	    rs1 = sysvar_objloadend(op) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if */

	op->magic = 0 ;
	return rs ;
}
/* end subroutine (sysvar_close) */

int sysvar_audit(sysvar *op) noex {
	int		rs = SR_NOSYS ;

	if (op == nullptr) return SR_FAULT ;

	if (op->magic != SYSVAR_MAGIC) return SR_NOTOPEN ;

	if (op->f.defaults) {
	    rs = vecstr_audit(&op->defaults) ;
	} else if (op->call.audit != nullptr)
	    rs = (*op->call.audit)(op->obj) ;

	return rs ;
}
/* end subroutine (sysvar_audit) */

int sysvar_count(sysvar *op) noex {
	int		rs = SR_NOSYS ;

	if (op == nullptr) return SR_FAULT ;

	if (op->magic != SYSVAR_MAGIC) return SR_NOTOPEN ;

	if (op->f.defaults) {
	    rs = vecstr_count(&op->defaults) ;
	} else if (op->call.count != nullptr)
	    rs = (*op->call.count)(op->obj) ;

	return rs ;
}
/* end subroutine (sysvar_count) */

int sysvar_curbegin(sysvar *op,SYSVAR_CUR *curp) noex {
	int		rs = SR_NOSYS ;

	if (op == nullptr) return SR_FAULT ;
	if (curp == nullptr) return SR_FAULT ;

	if (op->magic != SYSVAR_MAGIC) return SR_NOTOPEN ;

	memset(curp,0,sizeof(SYSVAR_CUR)) ;

	if (op->f.defaults) {
	    rs = sysvar_defcurbegin(op,curp) ;
	} else {
	    rs = sysvar_socurbegin(op,curp) ;
	}

	if (rs >= 0) {
	    curp->magic = SYSVAR_MAGIC ;
	}

	return rs ;
}
/* end subroutine (sysvar_curbegin) */

int sysvar_curend(sysvar *op,SYSVAR_CUR *curp) noex {
	int		rs = SR_NOSYS ;

	if (op == nullptr) return SR_FAULT ;
	if (curp == nullptr) return SR_FAULT ;

	if (op->magic != SYSVAR_MAGIC) return SR_NOTOPEN ;

	if (curp->magic != SYSVAR_MAGIC) return SR_NOTOPEN ;

	if (op->f.defaults) {
	    rs = sysvar_defcurend(op,curp) ;
	} else {
	    rs = sysvar_socurend(op,curp) ;
	}

	curp->magic = 0 ;
	return rs ;
}
/* end subroutine (sysvar_curend) */

int sysvar_fetch(sysvar *op,cchar *kp,int kl,SYSVAR_CUR *curp,
		char *vbuf,int vlen) noex {
	int		rs ;

	if (op == nullptr) return SR_FAULT ;
	if (curp == nullptr) return SR_FAULT ;
	if (kp == nullptr) return SR_FAULT ;

	if (op->magic != SYSVAR_MAGIC) return SR_NOTOPEN ;
	if (curp->magic != SYSVAR_MAGIC) return SR_NOTOPEN ;

	if (op->f.defaults) {
	    rs = sysvar_deffetch(op,kp,kl,curp->scp,vbuf,vlen) ;
	} else {
	    rs = (*op->call.fetch)(op->obj,kp,kl,curp->scp,vbuf,vlen) ;
	}

	return rs ;
}
/* end subroutine (sysvar_fetch) */

int sysvar_enum(SYAVR *op,SYAVR_CUR *curp,char *kbuf,int klen,
		char *vbuf,int vlen) noex {
	int		rs = SR_NOSYS ;

	if (op == nullptr) return SR_FAULT ;
	if (curp == nullptr) return SR_FAULT ;
	if (kbuf == nullptr) return SR_FAULT ;

	if (op->magic != SYSVAR_MAGIC) return SR_NOTOPEN ;
	if (curp->magic != SYSVAR_MAGIC) return SR_NOTOPEN ;

	if (op->f.defaults) {
	    rs = sysvar_defenum(op,curp->scp,kbuf,klen,vbuf,vlen) ;
	} else if (op->call.enumerate != nullptr) {
	    rs = (*op->call.enumerate)(op->obj,curp->scp,kbuf,klen,vbuf,vlen) ;
	}

	return rs ;
}
/* end subroutine (sysvar_enum) */


/* private subroutines */

static int sysvar_objloadbegin(sysvar *op,cchar *pr,cchar *objname) noex {
	modeload	*lp = &op->loader ;
	verstr		syms ;
	cint		n = nelem(subs) ;
	cint		vo = VECSTR_OCOMPACT ;
	int		rs ;
	int		rs1 ;

#if	CF_DEBUGS
	debugprintf("sysvar_objloadbegin: ent pr=%s on=%s\n",pr,objname) ;
#endif

	if ((rs = vecstr_start(&syms,n,vo)) >= 0) {
	    cint	snl = SYMNAMELEN ;
	    int		f_modload = false ;
	    cchar	**sv ;
	    cchar	*on = objname ;
	    char	snb[SYMNAMELEN + 1] ;

	    for (int i = 0 ; (i < n) && (subs[i] != nullptr) ; i += 1) {
	        if (isrequired(i)) {
	            if ((rs = sncpy3(snb,snl,on,"_",subs[i])) >= 0) {
	                rs = vecstr_add(&syms,snb,rs) ;
		    }
	        }
	        if (rs < 0) break ;
	    } /* end for */

	    if (rs >= 0) {
	        if ((rs = vecstr_getvec(&syms,&sv)) >= 0) {
	            cchar	*mn = SYSVAR_MODBNAME ;
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
	            rs = sysvar_loadcalls(op,objname) ;
	            if (rs < 0) {
	                uc_free(op->obj) ;
	                op->obj = nullptr ;
	            }
	        } /* end if (memory-allocation) */
	    } /* end if (modload_getmva) */
	    if (rs < 0)
	        modload_close(lp) ;
	} /* end if (modload_open) */

#if	CF_DEBUGS
	debugprintf("sysvar_objloadbegin: ret rs=%d\n",rs) ;
#endif

	return rs ;
}
/* end subroutine (sysvar_objloadbegin) */

static int sysvar_objloadend(sysvar *op) noex {
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
/* end subroutine (sysvar_objloadend) */

static int sysvar_loadcalls(sysvar *op,cchar *soname) noex {
	modeload	*lp = &op->loader ;
	cint		symlen = SYMNAMELEN ;
	int		rs = SR_OK ;
	int		c = 0 ;
	char		symbuf[SYMNAMELEN + 1] ;
	cvoid		*snp ;

	for (int i = 0 ; subs[i] != nullptr ; i += 1) {

	    if ((rs = sncpy3(symbuf,symlen,soname,"_",subs[i])) >= 0) {
	        if ((rs = modload_getsym(lp,symbuf,&snp)) == SR_NOTFOUND) {
	            snp = nullptr ;
	            if (! isrequired(i)) rs = SR_OK ;
	        }
	    }

	    if (rs < 0) break ;

#if	CF_DEBUGS
	    debugprintf("sysvar_loadcalls: call=%s %c\n",
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
		case sub_fetch:
		    op->call.fetch = 
			(int (*)(void *,cchar *,int,void *,char *,int)) 
				snp ;
		    break ;
		case sub_enum:
		    op->call.enumerate = 
			(int (*)(void *,void *,char *,int,char *,int)) snp ;
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
/* end subroutine (sysvar_loadcalls) */

static int sysvar_socurbegin(sysvar *op,SYSVAR_CUR *curp) noex {
	int		rs = SR_OK ;

	if (op->call.curbegin != nullptr) {
	    void	*p ;
	    if ((rs = uc_malloc(op->cursize,&p)) >= 0) {
		curp->scp = p ;
		rs = (*op->call.curbegin)(op->obj,curp->scp) ;
		if (rs < 0) {
	    	    uc_free(curp->scp) ;
	    	    curp->scp = nullptr ;
		}
	    } /* end if (m-a) */
	} else {
	    rs = SR_NOSYS ;
	}

	return rs ;
}
/* end subroutine (sysvar_socurbegin) */

static int sysvar_socurend(sysvar *op,SYSVAR_CUR *curp) noex {
	int		rs = SR_OK ;
	int		rs1 ;

	if (curp->scp == nullptr) return SR_NOTSOCK ;

	if (op->call.curend != nullptr) {
	    rs1 = (*op->call.curend)(op->obj,curp->scp) ;
	    if (rs >= 0) rs = rs1 ;
	    rs1 = uc_free(curp->scp) ;
	    if (rs >= 0) rs = rs1 ;
	    curp->scp = nullptr ;
	} else {
	    rs = SR_NOSYS ;
	}

	return rs ;
}
/* end subroutine (sysvar_socurend) */

static int sysvar_defaults(sysvar *op) noex {
	int		rs ;
	int		i ;
	int		f ;

#if	CF_DEBUGS
	debugprintf("sysvar_defaults: ent\n") ;
#endif
	if ((rs = vecstr_start(&op->defaults,NDEFAULTS,0)) >= 0) {
	op->f.defaults = (rs >= 0) ;
	for (i = 0 ; sysfnames[i] != nullptr ; i += 1) {
	    rs = sysvar_procsysdef(op,sysfnames[i]) ;
	    f = false ;
	    f = f || (rs == SR_NOENT) ;
	    f = f || (rs == SR_ACCESS) ;
	    if ((rs < 0) && (! f)) break ;
	} /* end for */
	if (rs >= 0) {
	vecstr_sort(&op->defaults,vstrkeycmp) ;
	}
	    if (rs < 0) {
		op->f.defaults = false ;
		vecstr_finish(&op->defaults) ;
	    }
	} /* end if (vecstr_start) */

#if	CF_DEBUGS
	debugprintf("sysvar_defaults: ret rs=%d\n",rs) ;
#endif
	return rs ;
}
/* end subroutine (sysvar_defaults) */

static int sysvar_procsysdef(sysvar *op,cchar *fname) noex {
	vecstr		lvars ;
	int		rs ;
	int		rs1 ;

	if ((rs = vecstr_start(&lvars,10,0)) >= 0) {
	    bool	f ;
	    cchar	*tp, *cp ;
	    if ((rs = vecstr_envfile(&lvars,fname)) >= 0) {
	        for (int i = 0 ; vecstr_get(&lvars,i,&cp) >= 0 ; i += 1) {
	            if (cp != nullptr) {
	                if ((tp = strchr(cp,'=')) != nullptr) {
	                    f = (matstr(wstrs,cp,(tp - cp)) >= 0) ;
	                    f = f || (matpstr(pstrs,10,cp,(tp - cp)) >= 0) ;
	                    if (f) {
	                        rs = vecstr_adduniq(&op->defaults,cp,-1) ;
	                     } /* end if */
		         }
		    } /* end if (non-null) */
		    if (rs < 0) break ;
	        } /* end for */
	    } /* end if (vecstr_envfile) */
	    rs1 = vecstr_finish(&lvars) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (lvars) */

	return rs ;
}
/* end subroutine (sysvar_procsysdef) */

static int sysvar_defcurbegin(sysvar *op,SYSVAR_CUR *curp) noex {
	SV_DEF	*dcp ;
	cint	size = sizeof(SV_DEF) ;
	int		rs ;

	if (op == nullptr) return SR_FAULT ;
	if (curp == nullptr) return SR_FAULT ;

	if ((rs = uc_malloc(size,&curp->scp)) >= 0) {
	    dcp = (SV_DEF *) curp->scp ;
	    dcp->i = -1 ;
	}

	return rs ;
}
/* end subroutine (sysvar_defcurbegin) */

static int sysvar_defcurend(sysvar *op,SYSVAR_CUR *curp) noex {
	int		rs = SR_OK ;
	int		rs1 ;

	if (op == nullptr) return SR_FAULT ;
	if (curp == nullptr) return SR_FAULT ;

	if (curp->scp == nullptr) return SR_NOTSOCK ;

	rs1 = uc_free(curp->scp) ;
	if (rs >= 0) rs = rs1 ;
	curp->scp = nullptr ;

	return rs ;
}
/* end subroutine (sysvar_defcurend) */

static int sysvar_deffetch(sysvar *op,cc *kp,int kl,SV_DC *dcp,
		char *vbuf,int vlen) noex {
	nulstr		ns ;
	int		rs ;
	int		rs1 ;
	int		vl = 0 ;
	cchar		*key ;

	if (vbuf != nullptr)
	    vbuf[0] = '\0' ;

	if ((rs = nulstr_start(&ns,kp,kl,&key)) >= 0) {
	    int		i = (dcp->i >= 0) ? (dcp->i + 1) : 0 ;
	    cchar	*cp ;
	    while ((rs = vecstr_get(&op->defaults,i,&cp)) >= 0) {
	        if (strkeycmp(key,cp) == 0) break ;
	        i += 1 ;
	    } /* end while */

	    if (rs >= 0) {
	        cchar	*tp, *vp ;

	        vp = nullptr ;
	        if ((tp = strchr(cp,'=')) != nullptr) {
		    vp = (tp + 1) ;
		}

	        if (vp != nullptr) {
	            if (vbuf != nullptr) {
		        rs = sncpy1(vbuf,vlen,vp) ;
		        vl = rs ;
	            } else
		        vl = strlen(vp) ;
	        }

	        if (rs >= 0)
	            dcp->i = i ;

	    } /* end if */

	    rs1 = nulstr_finish(&ns) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (nul-string) */

	return (rs >= 0) ? vl : rs ;
}
/* end subroutine (sysvar_deffetch) */

static int sysvar_defenum(sysvar *op,SV_DC *dcp,char *kbuf,int klen,
		char *vbuf,int vlen) noex {
	int		rs = SR_OK ;
	int		i ;
	int		kl ;
	int		vl = 0 ;
	cchar	*tp, *vp ;
	cchar	*cp ;

#ifdef	OPTIONAL
	if (vbuf != nullptr)
	    vbuf[0] = '\0' ;
#endif

	i = (dcp->i >= 0) ? (dcp->i + 1) : 0 ;

	while ((rs = vecstr_get(&op->defaults,i,&cp)) >= 0) {
	    if (cp != nullptr) break ;
	    i += 1 ;
	} /* end while */

#if	CF_DEBUGS
	debugprintf("sysvar_defenum: mid rs=%d\n",rs) ;
	if (rs >= 0)
	debugprintf("sysvar_defenum: cp=>%t<\n",cp,strnlen(cp,40)) ;
#endif

	if (rs >= 0) {

	    kl = -1 ;
	    vp = nullptr ;
	    if ((tp = strchr(cp,'=')) != nullptr) {
		vp = (tp + 1) ;
		kl = (tp - cp) ;
	    }

	    if (kbuf != nullptr) {
		rs = snwcpy(kbuf,klen,cp,kl) ;
	    }

	    if ((rs >= 0) && (vp != nullptr)) {
	        if (vbuf != nullptr) {
		    rs = sncpy1(vbuf,vlen,vp) ;
		    vl = rs ;
	        } else {
		    vl = strlen(vp) ;
		}
	    }

	    if (rs >= 0)
	        dcp->i = i ;

	} else if (vbuf != nullptr) {
	    vbuf[0] = '\0' ;
	}

#if	CF_DEBUGS
	debugprintf("sysvar_defenum: ret rs=%d vl=%u\n",rs,vl) ;
#endif

	return (rs >= 0) ? vl : rs ;
}
/* end subroutine (sysvar_defenum) */

static bool isrequired(int i) noex {
	bool		f = false ;
	switch (i) {
	case sub_open:
	case sub_curbegin:
	case sub_fetch:
	case sub_enum:
	case sub_curend:
	case sub_close:
	    f = true ;
	    break ;
	} /* end switch */
	return f ;
}
/* end subroutine (isrequired) */


