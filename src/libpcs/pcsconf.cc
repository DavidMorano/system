/* pcsconf SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* load management and interface for the PCSCONFS object */
/* version %I% last-modified %G% */

#define	CF_DEBUGS	0		/* non-switchable debug print-outs */
#define	CF_DEBUGDUMP	0		/* only if underlying SW supports */
#define	CF_UGETPW	1		/* use |ugetpw(3uc)| */

/* revision history:

	= 1992-03-10, David A­D­ Morano
	This module was originally written.

	= 1998-04-03, David A­D­ Morano
	This was modified for more general use.

	= 2008-10-07, David A­D­ Morano
	This was modified to allow for the main part to be a loadable
	module.

*/

/* Copyright © 1992,1998,2008 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	pcsconf

	Description:
	This module implements an interface (a trivial one) that
	provides access to the PCSCONFS object (which is dynamically
	loaded).

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
#include	<usystem.h>
#include	<getbufsize.h>
#include	<getnodename.h>
#include	<getusername.h>
#include	<getax.h>
#include	<ugetpw.h>
#include	<vecstr.h>
#include	<modload.h>
#include	<nulstr.h>
#include	<uinfo.h>
#include	<expcook.h>
#include	<localmisc.h>

#include	"pcsconf.h"
#include	"pcsconfs.h"


/* local defines */

#define	PCSCONF_CURMAGIC	0x97677247
#define	PCSCONF_MODBNAME	"pcsconfs"
#define	PCSCONF_OBJNAME		"pcsconfs"

#if	CF_UGETPW
#define	GETPW_NAME	ugetpw_name
#else
#define	GETPW_NAME	getpw_name
#endif /* CF_UGETPW */

#define	COOKMGR		struct cookmgr

#define	LIBCNAME	"lib"

#ifndef	VARLIBPATH
#define	VARLIBPATH	"LD_LIBRARY_PATH"
#endif

#ifndef	SYMNAMELEN
#define	SYMNAMELEN	60
#endif

#ifndef	EBUFLEN
#define	EBUFLEN		(6 * MAXPATHLEN)
#endif

#ifndef	MKCHAR
#define	MKCHAR(ch)	((ch) & UCHAR_MAX)
#endif


/* external subroutines */

#if	CF_DEBUGS
extern int	debugprintf(cchar *,...) ;
extern int	strlinelen(cchar *,int,int) ;
#endif


/* local structures */

struct cookmgr_flags {
	uint		cooks:1 ;
	uint		uname:1 ;
	uint		uaux:1 ;
} ;

struct cookmgr_nodedomain {
	cchar	*nodename ;
	cchar	*domainname ;
	cchar	*a ;		/* allocation */
} ;

struct cookmgr {
	struct cookmgr_flags	fl ;
	struct cookmgr_nodedomain	nd ;
	EXPCOOK	cooks ;
	UINFO_NAME	uname ;
	UINFO_AUX	uaux ;
	cchar	*pr ;
} ;


/* forward references */

static int	pcsconf_objloadbegin(PCSCONF *,cchar *,cchar *) ;
static int	pcsconf_objloadend(PCSCONF *) ;
static int	pcsconf_modloadopen(PCSCONF *,cchar *,cchar *) ;
static int	pcsconf_loadcalls(PCSCONF *,cchar *) ;
static int	pcsconf_getpcsids(PCSCONF *) ;
static int	pcsconf_getpcspw(PCSCONF *) ;
static int	pcsconf_expand(PCSCONF *,char *,int,int) ;
static int	pcsonf_cookmgr(PCSCONF *) ;
static int	pcsonf_cookload(PCSCONF *,cchar *,int) ;

static int	cookmgr_start(COOKMGR *,cchar *) ;
static int	cookmgr_load(COOKMGR *,cchar *,int) ;
static int	cookmgr_expand(COOKMGR *,char *,int,cchar *,int) ;
static int	cookmgr_uname(COOKMGR *) ;
static int	cookmgr_uaux(COOKMGR *) ;
static int	cookmgr_nodedomain(COOKMGR *) ;
static int	cookmgr_finish(COOKMGR *) ;

static bool	isrequired(int) noex ;


/* global variables */


/* local variables */

enum subs {
	sub_start,
	sub_curbegin,
	sub_fetch,
	sub_enum,
	sub_curend,
	sub_audit,
	sub_finish,
	sub_overlast
} ;

constexpr cpcchar	subs[] = {
	"start",
	"curbegin",
	"fetch",
	"enum",
	"curend",
	"audit",
	"finish",
	nullptr
} ;

enum cooks {
	cook_sysname,
	cook_nodename,
	cook_release,
	cook_version,
	cook_machine,
	cook_architecture,
	cook_platform,
	cook_provider,
	cook_hwserial,
	cook_nisdomain,
	cook_n,
	cook_d,
	cook_h,
	cook_u,
	cook_r,
	cook_rn,
	cook_overlast
} ;

constexpr cpcchar	cooks[] = {
	"sysname",
	"nodename",
	"release",
	"version",
	"machine",
	"architecture",
	"platform",
	"provider",
	"hwserial",
	"nisdomain",
	"N",
	"D",
	"H",
	"U",
	"R",
	"PN",
	nullptr
} ;


/* exported variables */


/* exported subroutines */

int pcsconf_start(PCSCONF *op,cchar *pr,cchar **envv,cchar *cfname) noex {
	int		rs ;
	cchar		*objname = PCSCONF_OBJNAME ;

	if (op == nullptr) return SR_FAULT ;
	if (pr == nullptr) return SR_FAULT ;

	if (pr[0] == '\0') return SR_INVALID ;

#if	CF_DEBUGS
	debugprintf("pcsconf_start: pr=%s\n",pr) ;
	debugprintf("pcsconf_start: cfname=%s\n",cfname) ;
#endif

	if ((cfname != nullptr) && (cfname[0] == '\0')) return SR_INVALID ;

	memset(op,0,sizeof(PCSCONF)) ;
	op->pr = pr ;
	op->envv = envv ;
	op->uid_pcs = -1 ;

	if ((rs = pcsconf_objloadbegin(op,pr,objname)) >= 0) {
	    if ((rs = ptm_create(&op->m,nullptr)) >= 0) {
	        rs = (*op->call.start)(op->obj,pr,envv,cfname) ;
	        if (rs >= 0) {
		    op->magic = PCSCONF_MAGIC ;
	        }
	        if (rs < 0)
		    ptm_destroy(&op->m) ;
	    } /* end if (ptm) */
	    if (rs < 0)
		pcsconf_objloadend(op) ;
	} /* end if (obj-mod loading) */

#if	CF_DEBUGS
	debugprintf("pcsconf_start: ret rs=%d\n",rs) ;
#endif

	return rs ;
}
/* end subroutine (pcsconf_start) */


/* free up the entire vector string data structure object */
int pcsconf_finish(PCSCONF *op)
{
	int		rs = SR_OK ;
	int		rs1 ;

	if (op == nullptr) return SR_FAULT ;

	if (op->magic != PCSCONF_MAGIC) return SR_NOTOPEN ;

	if (op->pcsusername != nullptr) {
	    rs1 = uc_free(op->pcsusername) ;
	    if (rs >= 0) rs = rs1 ;
	    op->pcsusername = nullptr ;
	}

	if (op->cookmgr != nullptr) {
	    rs1 = cookmgr_finish(op->cookmgr) ;
	    if (rs >= 0) rs = rs1 ;
	    rs1 = uc_free(op->cookmgr) ;
	    if (rs >= 0) rs = rs1 ;
	    op->cookmgr = nullptr ;
	}

	rs1 = (*op->call.finish)(op->obj) ;
	if (rs >= 0) rs = rs1 ;

	rs1 = ptm_destroy(&op->m) ;
	if (rs >= 0) rs = rs1 ;

	rs1 = pcsconf_objloadend(op) ;
	if (rs >= 0) rs = rs1 ;

	op->magic = 0 ;
	return rs ;
}
/* end subroutine (pcsconf_finish) */


int pcsconf_audit(PCSCONF *op)
{
	int		rs = SR_NOSYS ;
	int		rs1 ;
	int		vl = 0 ;

	if (op == nullptr) return SR_FAULT ;

	if (op->magic != PCSCONF_MAGIC) return SR_NOTOPEN ;

	if (op->call.audit != nullptr) {
	    if ((rs = ptm_lock(&op->m)) >= 0) {
		{
	            rs = (*op->call.audit)(op->obj) ;
		    vl = rs ;
	        }
		rs1 = ptm_unlock(&op->m) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (mutex) */
	} /* end if (have method) */

	return rs ;
}
/* end subroutine (pcsconf_audit) */


int pcsconf_getpcsuid(PCSCONF *op)
{
	int		rs ;
	int		rs1 ;
	int		v = 0 ;

	if (op == nullptr) return SR_FAULT ;

	if (op->magic != PCSCONF_MAGIC) return SR_NOTOPEN ;

	if ((rs = ptm_lock(&op->m)) >= 0) {
	    if ((rs = pcsconf_getpcsids(op)) >= 0) {
	        v = op->uid_pcs ;
	    }
	    rs1 = ptm_unlock(&op->m) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (mutex) */

	return (rs >= 0) ? v : rs ;
}
/* end subroutine (pcsconf_getpcsuid) */


int pcsconf_getpcsgid(PCSCONF *op)
{
	int		rs ;
	int		rs1 ;
	int		v = 0 ;

	if (op == nullptr) return SR_FAULT ;

	if (op->magic != PCSCONF_MAGIC) return SR_NOTOPEN ;

	if ((rs = ptm_lock(&op->m)) >= 0) {
	    if ((rs = pcsconf_getpcsids(op)) >= 0) {
	        v = op->gid_pcs ;
	    }
	    rs1 = ptm_unlock(&op->m) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (mutex) */

	return (rs >= 0) ? v : rs ;
}
/* end subroutine (pcsconf_getpcsgid) */


int pcsconf_getpcsusername(PCSCONF *op,char *ubuf,int ulen)
{
	int		rs ;
	int		rs1 ;
	int		vl = 0 ;

	if (op == nullptr) return SR_FAULT ;
	if (ubuf == nullptr) return SR_FAULT ;

	if (op->magic != PCSCONF_MAGIC) return SR_NOTOPEN ;

	if (ulen < 0) ulen = USERNAMELEN ;

	if ((rs = ptm_lock(&op->m)) >= 0) {
	    if (op->pcsusername == nullptr) rs = pcsconf_getpcspw(op) ;
	    if (rs >= 0) {
		if (op->pcsusername != nullptr) {
		    rs = sncpy1(ubuf,ulen,op->pcsusername) ;
		    vl = rs ;
		} else {
		    rs = SR_NOTFOUND ;
	        }
	    }
	    rs1 = ptm_unlock(&op->m) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (mutex) */

	return (rs >= 0) ? vl : rs ;
}
/* end subroutine (pcsconf_getpcsusername) */


int pcsconf_getpr(PCSCONF *op,cchar **prp)
{
	int		rs = SR_OK ;

	if (op == nullptr) return SR_FAULT ;

	if (op->magic != PCSCONF_MAGIC) return SR_NOTOPEN ;

	rs = strlen(op->pr) ;
	if (prp != nullptr) *prp = op->pr ;

	return rs ;
}
/* end subroutine (pcsconf_getpr) */


int pcsconf_getenvv(PCSCONF *op,cchar ***envvp)
{
	int		rs = SR_OK ;

	if (op == nullptr) return SR_FAULT ;

	if (op->magic != PCSCONF_MAGIC) return SR_NOTOPEN ;

	if (envvp != nullptr) *envvp = op->envv ;

	return rs ;
}
/* end subroutine (pcsconf_getenvv) */


int pcsconf_curbegin(PCSCONF *op,PCSCONF_CUR *curp)
{
	int		rs = SR_NOSYS ;
	int		rs1 ;

	if (op == nullptr) return SR_FAULT ;
	if (curp == nullptr) return SR_FAULT ;

	if (op->magic != PCSCONF_MAGIC) return SR_NOTOPEN ;

	if (op->cursize != sizeof(PCSCONFS_CUR)) return SR_BUGCHECK ;

	memset(curp,0,sizeof(PCSCONF_CUR)) ;

	if (op->call.curbegin != nullptr) {
	    if ((rs = ptm_lock(&op->m)) >= 0) {
	        void	*p ;
	        if ((rs = uc_malloc(op->cursize,&p)) >= 0) {
		    curp->scp = p ;
	            if ((rs = (*op->call.curbegin)(op->obj,curp->scp)) >= 0) {
		        curp->magic = PCSCONF_CURMAGIC ;
		    }
	            if (rs < 0) {
	                uc_free(curp->scp) ;
	                curp->scp = nullptr ;
		    }
	        } /* end if (memory-allocation) */
		rs1 = ptm_unlock(&op->m) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (mutex) */
	} /* end if (have method) */

	return rs ;
}
/* end subroutine (pcsconf_curbegin) */


int pcsconf_curend(PCSCONF *op,PCSCONF_CUR *curp)
{
	int		rs = SR_OK ;
	int		rs1 ;

	if (op == nullptr) return SR_FAULT ;
	if (curp == nullptr) return SR_FAULT ;

	if (op->magic != PCSCONF_MAGIC) return SR_NOTOPEN ;
	if (curp->magic != PCSCONF_CURMAGIC) return SR_NOTOPEN ;

#if	CF_DEBUGS
	debugprintf("pcsconf_curend: ent scp{%p}\n",curp->scp) ;
#endif

	if (curp->scp != nullptr) {
	    if ((rs = ptm_lock(&op->m)) >= 0) {
	        if (op->call.curend != nullptr) {
	            rs1 = (*op->call.curend)(op->obj,curp->scp) ;
		    if (rs >= 0) rs = rs1 ;
#if	CF_DEBUGS
	debugprintf("pcsconf_curend: pcsconfs_curend() rs=%d\n",rs) ;
#endif
		} else {
		    rs = SR_NOSYS ;
		}
	        uc_free(curp->scp) ;
	        curp->scp = nullptr ;
		rs1 = ptm_unlock(&op->m) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (mutex) */
	} else
	    rs = SR_BUGCHECK ;

	curp->magic = 0 ;

#if	CF_DEBUGS
	debugprintf("pcsconf_curend: ret rs=%d\n",rs) ;
#endif

	return rs ;
}
/* end subroutine (pcsconf_curend) */


/* lookup tags by strings */
int pcsconf_fetch(PCSCONF *op,cchar *kp,int kl,PCSCONF_CUR *curp,
		char *vbuf,int vlen)
{
	int		rs = SR_NOSYS ;
	int		rs1 ;
	int		vl = 0 ;

	if (op == nullptr) return SR_FAULT ;
	if (curp == nullptr) return SR_FAULT ;
	if (kp == nullptr) return SR_FAULT ;

	if (op->magic != PCSCONF_MAGIC) return SR_NOTOPEN ;
	if (curp->magic != PCSCONF_CURMAGIC) return SR_NOTOPEN ;

	if (op->call.fetch != nullptr) {
	    if ((rs = ptm_lock(&op->m)) >= 0) {
	        rs = (*op->call.fetch)(op->obj,kp,kl,curp->scp,vbuf,vlen) ;
	        if (rs >= 0) {
	            rs = pcsconf_expand(op,vbuf,vlen,rs) ;
		    vl = rs ;
		}
		rs1 = ptm_unlock(&op->m) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (mutex) */
	} /* end if (have method) */

	return (rs >= 0) ? vl : rs ;
}
/* end subroutine (pcsconf_fetch) */


int pcsconf_enum(op,curp,kbuf,klen,vbuf,vlen)
PCSCONF		*op ;
PCSCONF_CUR	*curp ;
char		kbuf[] ;
int		klen ;
char		vbuf[] ;
int		vlen ;
{
	int		rs = SR_NOSYS ;
	int		rs1 ;
	int		vl = 0 ;

	if (op == nullptr) return SR_FAULT ;
	if (curp == nullptr) return SR_FAULT ;
	if (kbuf == nullptr) return SR_FAULT ;

	if (op->magic != PCSCONF_MAGIC) return SR_NOTOPEN ;
	if (curp->magic != PCSCONF_CURMAGIC) return SR_NOTOPEN ;

	if (op->call.enumerate != nullptr) {
	    if ((rs = ptm_lock(&op->m)) >= 0) {
		void	*cp = curp->scp ;
	        rs = (*op->call.enumerate)(op->obj,cp,kbuf,klen,vbuf,vlen) ;
	        if (rs >= 0) {
	            rs = pcsconf_expand(op,vbuf,vlen,rs) ;
		    vl = rs ;
		}
		rs1 = ptm_unlock(&op->m) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (mutex) */
	} /* end if (have method) */

	return (rs >= 0) ? vl : rs ;
}
/* end subroutine (pcsconf_enum) */


int pcsconf_fetchone(PCSCONF *op,cchar *kp,int kl,char vbuf[],int vlen)
{
	PCSCONF_CUR	cur ;
	int		rs ;
	int		rs1 ;
	int		vl = 0 ;

	if (op == nullptr) return SR_FAULT ;
	if (kp == nullptr) return SR_FAULT ;

	if (op->magic != PCSCONF_MAGIC) return SR_NOTOPEN ;

	if ((rs = pcsconf_curbegin(op,&cur)) >= 0) {
	    {
	        rs = pcsconf_fetch(op,kp,kl,&cur,vbuf,vlen) ;
		vl = rs ;
	    }
	    rs1 = pcsconf_curend(op,&cur) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (cursor) */

	return (rs >= 0) ? vl : rs ;
}
/* end subroutine (pcsconf_fetchone) */


/* private subroutines */


/* find and load the DB-access object */
static int pcsconf_objloadbegin(PCSCONF *op,cchar *pr,cchar *objname)
{
	int		rs ;

	if ((rs = pcsconf_modloadopen(op,pr,objname)) >= 0) {
	    MODLOAD	*lp = &op->loader ;
	    if ((rs = modload_getmv(lp,0)) >= 0) {
	        op->objsize = rs ;
	        if ((rs = modload_getmv(lp,1)) >= 0) {
		    void	*p ;
	    	    op->cursize = rs ;
		    if ((rs = uc_malloc(op->objsize,&p)) >= 0) {
		        op->obj = p ;
			rs = pcsconf_loadcalls(op,objname) ;
			if (rs < 0) {
	    		    uc_free(op->obj) ;
	    		    op->obj = nullptr ;
			}
		    } /* end if (memory-allocations) */
		} /* end if (getmv) */
	    } /* end if (getmv) */
	    if (rs < 0)
		modload_close(lp) ;
	} /* end if (modloadopen) */

	return rs ;
}
/* end subroutine (pcsconf_objloadbegin) */


static int pcsconf_objloadend(PCSCONF *op)
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
/* end subroutine (pcsconf_objloadend) */


static int pcsconf_modloadopen(PCSCONF *op,cchar *pr,cchar *objname)
{
	VECSTR		syms ;
	const int	n = nelem(subs) ;
	int		opts ;
	int		rs ;
	int		rs1 ;

	opts = VECSTR_OCOMPACT ;
	if ((rs = vecstr_start(&syms,n,opts)) >= 0) {
	    MODLOAD	*lp = &op->loader ;
	    int		i ;
	    int		f_modload = false ;
	    cchar	*modbname ;
	    char	symname[SYMNAMELEN + 1] ;

	    for (i = 0 ; (i < n) && (subs[i] != nullptr) ; i += 1) {
	        if (isrequired(i)) {
	            rs = sncpy3(symname,SYMNAMELEN,objname,"_",subs[i]) ;
	            if (rs >= 0)
	                rs = vecstr_add(&syms,symname,rs) ;
	        }
	        if (rs < 0) break ;
	    } /* end for */

	    if (rs >= 0) {
	        cchar	**sv ;
	        if ((rs = vecstr_getvec(&syms,&sv)) >= 0) {
	            modbname = PCSCONF_MODBNAME ;
	            opts = (MODLOAD_OLIBVAR | MODLOAD_OPRS | MODLOAD_OSDIRS) ;
	            rs = modload_open(lp,pr,modbname,objname,opts,sv) ;
		    f_modload = (rs >= 0) ;
		}
	    }

	    rs1 = vecstr_finish(&syms) ;
	    if (rs >= 0) rs = rs1 ;
	    if ((rs < 0) && f_modload)
		modload_close(lp) ;
	} /* end if (allocation) */

	return rs ;
}
/* end subroutine (pcsconf_modloadopen) */


static int pcsconf_loadcalls(PCSCONF *op,cchar objname[])
{
	MODLOAD		*lp = &op->loader ;
	int		rs = SR_OK ;
	int		rs1 ;
	int		i ;
	int		c = 0 ;
	char		symname[SYMNAMELEN + 1] ;
	const void	*snp ;

	for (i = 0 ; subs[i] != nullptr ; i += 1) {

	    rs = sncpy3(symname,SYMNAMELEN,objname,"_",subs[i]) ;
	    if (rs < 0) break ;

	    rs1 = modload_getsym(lp,symname,&snp) ;

	    if (rs1 == SR_NOTFOUND) {
	        snp = nullptr ;
	        if (isrequired(i))
	            break ;
	    } else
	        rs = rs1 ;

	    if (rs < 0) break ;

	    if (snp != nullptr) {

	        c += 1 ;
	        switch (i) {

	        case sub_start:
	            op->call.start = (int (*)(void *,cchar *,cchar **,
	                cchar *)) snp ;
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

	        case sub_finish:
	            op->call.finish = (int (*)(void *)) snp ;
	            break ;

	        } /* end switch */

	    } /* end if (it had the call) */

	} /* end for (subs) */

	return (rs >= 0) ? c : rs ;
}
/* end subroutine (pcsconf_loadcalls) */


static int pcsconf_getpcsids(PCSCONF *op)
{
	int		rs = SR_OK ;

	if (op->uid_pcs < 0) {
	    ustat	sb ;
	    if ((rs = u_stat(op->pr,&sb)) >= 0) {
		op->uid_pcs = sb.st_uid ;
		op->gid_pcs = sb.st_gid ;
	    } /* end if (stat) */
	} /* end if (needed) */

	return rs ;
}
/* end subroutine (pcsconf_getpcsids) */


static int pcsconf_getpcspw(PCSCONF *op)
{
	int		rs = SR_OK ;
	int		rs1 ;

	if (op->pcsusername == nullptr) {
	    const uid_t	uid_pcs = op->uid_pcs ;
	    cchar	*un = PCSCONF_USER ;
	    if ((rs = pcsconf_getpcsids(op)) >= 0) {
	        struct passwd	pw ;
		const int	pwlen = getbufsize(getbufsize_pw) ;
	        char		*pwbuf ;
		if ((rs = uc_malloc((pwlen+1),&pwbuf)) >= 0) {
	            if ((rs1 = GETPW_NAME(&pw,pwbuf,pwlen,un)) >= 0) {
		        if (pw.pw_uid != uid_pcs) rs1 = SR_NOTFOUND ;
	            } else if (rs1 != SR_NOTFOUND) {
		        rs = rs1 ;
		    }
		    if ((rs >= 0) && (rs1 == SR_NOTFOUND)) {
			const uid_t	u = uid_pcs ;
		        un = nullptr ;
		        if ((rs = getpwusername(&pw,pwbuf,pwlen,u)) >= 0) {
			    un = pw.pw_name ;
		        } /* end if (getusername) */
		    } /* end if */
		    if ((rs >= 0) && (un != nullptr)) {
		        cchar	*cp ;
		        if ((rs = uc_mallocstrw(un,-1,&cp)) >= 0) {
			    op->pcsusername = cp ;
		        } /* end if (memory-allocation) */
		    } /* end if (store pcs-username) */
		    uc_free(pwbuf) ;
		} /* end if (m-a) */
	    } /* end if (pcsconf-getpcsids) */
	} else {
	    rs = strlen(op->pcsusername) ;
	}

	return rs ;
}
/* end subroutine (pcsconf_getpcspw) */


static int pcsconf_expand(PCSCONF *op,char *vbuf,int vlen,int vl)
{
	int		rs = SR_OK ;
	int		kl ;
	int		sl = vl ;
	int		f_havekeys = false ;
	cchar		*sp = (cchar *) vbuf ;
	cchar		*kp = nullptr ;

	if (vlen < 0) vlen = EBUFLEN ;

	while ((kl = sfcookkey(sp,sl,&kp)) > 0) {
	    f_havekeys = true ;

	    rs = pcsonf_cookload(op,kp,kl) ;

	    sl -= ((kp+kl)-sp) ;
	    sp = (kp+kl) ;
	    if (rs < 0) break ;
	} /* end while */

	if ((rs >= 0) && f_havekeys) {
	    const int	elen = vlen ;
	    int		size ;
	    char	*ebuf ;
	    size = (elen+1) ;
	    if ((rs = uc_malloc(size,&ebuf)) >= 0) {
	        if ((rs = pcsonf_cookmgr(op)) >= 0) {
	            COOKMGR	*cmp = (COOKMGR *) op->cookmgr ;
	            if ((rs = cookmgr_expand(cmp,ebuf,elen,vbuf,vl)) >= 0) {
	                rs = snwcpy(vbuf,vlen,ebuf,rs) ;
	                vl = rs ;
	            }
	        } /* end if (cookmgr) */
	        uc_free(ebuf) ;
	    } /* end if (memory_allocation) */
	} /* end if (had keys) */

	return (rs >= 0) ? vl : rs ;
}
/* end subroutine (pcsconf_expand) */


static int pcsonf_cookmgr(PCSCONF *op)
{
	int		rs = SR_OK ;

	if (op->cookmgr == nullptr) {
	    const int	osize = sizeof(COOKMGR) ;
	    void	*p ;
	    if ((rs = uc_malloc(osize,&p)) >= 0) {
	        COOKMGR	*cmp = (COOKMGR *) p ;
	        if ((rs = cookmgr_start(cmp,op->pr)) >= 0) {
	            op->cookmgr = cmp ;
	        }
	        if (rs < 0)
	            uc_free(p) ;
	    } /* end if (memory-allocation) */
	} /* end if (needed to initialize COOKMGR) */

	return rs ;
}
/* end subroutine (pcsconf_cookmgr) */


static int pcsonf_cookload(PCSCONF *op,cchar *kp,int kl)
{
	int		rs ;

	if ((rs = pcsonf_cookmgr(op)) >= 0) {
	    COOKMGR	*cmp = (COOKMGR *) op->cookmgr ;
	    rs = cookmgr_load(cmp,kp,kl) ;
	} /* end if (cook-manager started) */

	return rs ;
}
/* end subroutine (pcsconf_cookload) */


static int cookmgr_start(COOKMGR *cmp,cchar *pr)
{
	int		rs ;

	memset(cmp,0,sizeof(COOKMGR)) ;
	cmp->pr = pr ;

	rs = expcook_start(&cmp->cooks) ;
	cmp->fl.cooks = (rs >= 0) ;

	return rs ;
}
/* end subroutine (cookmgr_start) */


static int cookmgr_load(COOKMGR *cmp,cchar *kp,int kl)
{
	int		rs = SR_OK ;

	if (expcook_findkey(&cmp->cooks,kp,kl,nullptr) == SR_NOTFOUND) {
	    NULSTR	ns ;
	    cchar	*kname = nullptr ;
	    if ((rs = nulstr_start(&ns,kp,kl,&kname)) >= 0) {
	        const int	ci = matstr(cooks,kp,kl) ;
	        int		vl = -1 ;
	        cchar	*vp = nullptr ;
	        char	ubuf[USERNAMELEN+1] ;
	        char	*tbuf = nullptr ;
	        switch (ci) {
	        case cook_sysname:
	        case cook_nodename:
	        case cook_release:
	        case cook_version:
	        case cook_machine:
	            if ((rs = cookmgr_uname(cmp)) >= 0) {
	                switch (ci) {
	                case cook_sysname:
	                    vp = cmp->uname.sysname ;
	                    break ;
	                case cook_nodename:
	                    vp = cmp->uname.nodename ;
	                    break ;
	                case cook_release:
	                    vp = cmp->uname.release ;
	                    break ;
	                case cook_version:
	                    vp = cmp->uname.version ;
	                    break ;
	                case cook_machine:
	                    vp = cmp->uname.machine ;
	                    break ;
	                } /* end switch */
	            } /* end if (uname) */
	            break ;
	        case cook_architecture:
	        case cook_platform:
	        case cook_provider:
	        case cook_hwserial:
	        case cook_nisdomain:
	            if ((rs = cookmgr_uaux(cmp)) >= 0) {
	                switch (ci) {
	                case cook_architecture:
	                    vp = cmp->uaux.architecture ;
	                    break ;
	                case cook_platform:
	                    vp = cmp->uaux.platform ;
	                    break ;
	                case cook_provider:
	                    vp = cmp->uaux.provider ;
	                    break ;
	                case cook_hwserial:
	                    vp = cmp->uaux.hwserial ;
	                    break ;
	                case cook_nisdomain:
	                    vp = cmp->uaux.nisdomain ;
	                    break ;
	                } /* end switch */
	            } /* end if (uname) */
	            break ;
	        case cook_n:
	        case cook_d:
	        case cook_h:
	            if ((rs = cookmgr_nodedomain(cmp)) >= 0) {
	                switch (ci) {
	                case cook_n:
	                    vp = cmp->nd.nodename ;
	                    break ;
	                case cook_d:
	                    vp = cmp->nd.domainname ;
	                    break ;
	                case cook_h:
	                    {
	                        cchar	*nn = cmp->nd.nodename ;
	                        cchar	*dn = cmp->nd.domainname ;
	                        int size = 1 ;
	                        int	tlen = 0 ;
	                        tlen += (strlen(nn)+1) ;
	                        tlen += (strlen(dn)+1) ;
	                        size += tlen ;
	                        if ((rs = uc_malloc(size,&tbuf)) >= 0) {
	                            vp = tbuf ;
	                            rs = snsds(tbuf,tlen,nn,dn) ;
	                            vl = rs ;
	                        }
	                    }
	                    break ;
	                } /* end switch */
	            }
	            break ;
	        case cook_u:
	            if ((rs = getusername(ubuf,USERNAMELEN,-1)) > 0) {
			vl = rs ;
	                vp = ubuf ;
		    }
	            break ;
	        case cook_r:
	            vp = cmp->pr ;
	            break ;
	        case cook_rn:
	            vl = sfbasename(cmp->pr,-1,&vp) ;
	            break ;
	        } /* end switch */
	        if ((rs >= 0) && (vp != nullptr))
	            rs = expcook_add(&cmp->cooks,kname,vp,vl) ;
	        if (tbuf != nullptr) uc_free(tbuf) ;
	        nulstr_finish(&ns) ;
	    } /* end if (nulstr) */
	} /* end if (key not already found) */

	return rs ;
}
/* end subroutine (cookmgr_load) */


static int cookmgr_expand(COOKMGR *cmp,char ebuf[],int elen,cchar vbuf[],int vl)
{
	EXPCOOK		*ecp = (EXPCOOK *) &cmp->cooks ;
	const int	wch = MKCHAR('¿') ;
	int		rs ;

	rs = expcook_exp(ecp,wch,ebuf,elen,vbuf,vl) ;

	return rs ;
}
/* end subroutine (cookmgr_expand) */


static int cookmgr_uname(COOKMGR *cmp)
{
	int		rs = SR_OK ;

	if (! cmp->fl.uname) {
	    cmp->fl.uname = true ;
	    rs = uinfo_name(&cmp->uname) ;
	}

	return rs ;
}
/* end subroutine (cookmgr_uname) */


static int cookmgr_uaux(COOKMGR *cmp)
{
	int		rs = SR_OK ;

	if (! cmp->fl.uaux) {
	    cmp->fl.uaux = true ;
	    rs = uinfo_aux(&cmp->uaux) ;
	}

	return rs ;
}
/* end subroutine (cookmgr_uaux) */


static int cookmgr_nodedomain(COOKMGR *cmp)
{
	int		rs = SR_OK ;

	if (cmp->nd.a == nullptr) {
	    char	nn[NODENAMELEN+1] ;
	    char	dn[MAXHOSTNAMELEN+1] ;
	    if ((rs = getnodedomain(nn,dn)) >= 0) {
	        int	size = 0 ;
	        char	*bp ;
	        size += (strlen(nn)+1) ;
	        size += (strlen(dn)+1) ;
	        if ((rs = uc_malloc(size,&bp)) >= 0) {
	            cmp->nd.a = bp ;
	            cmp->nd.nodename = bp ;
	            bp = (strwcpy(bp,nn,-1)+1) ;
	            bp = (strwcpy(bp,dn,-1)+1) ;
	        } /* end if (memory-allocation) */
	    } /* end if (get node-domain) */
	} /* end if */

	return rs ;
}
/* end subroutine (cookmgr_nodedomain) */


static int cookmgr_finish(COOKMGR *cmp)
{
	int		rs = SR_OK ;
	int		rs1 ;

	if (cmp != nullptr) {
	    if (cmp->nd.a != nullptr) {
	        rs1 = uc_free(cmp->nd.a) ;
	        if (rs >= 0) rs = rs1 ;
	        cmp->nd.a = nullptr ;
	    }
	    if (cmp->fl.cooks) {
	        cmp->fl.cooks = false ;
	        rs1 = expcook_finish(&cmp->cooks) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	}

	return rs ;
}
/* end subroutine (cookmgr_finish) */

static bool isrequired(int i) noex {
	int		f = false ;
	switch (i) {
	case sub_start:
	case sub_curbegin:
	case sub_fetch:
	case sub_enum:
	case sub_curend:
	case sub_audit:
	case sub_finish:
	    f = true ;
	    break ;
	} /* end switch */
	return f ;
}
/* end subroutine (isrequired) */


