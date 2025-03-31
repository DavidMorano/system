/* sysdialer SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* system-dialer storage object */
/* version %I% last-modified %G% */

#define	CF_SAMEMODULE	1		/* try to use same module */
#define	CF_PRCACHE	1		/* PR-cache */
#define	CF_LOOKSELF	0		/* allow for compiled-in sysdialers */

/* revision history:

	- 2003-11-04, David A­D­ Morano
	This subroutine was adopted for use from the DWD program.

*/

/* Copyright © 2003 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	sysdialer

	Description:
	This object manages what sysdialers have been loaded so far.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<dlfcn.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<ctime>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>		/* |strnlen(3c)| */
#include	<usystem.h>
#include	<vecobj.h>
#include	<vecstr.h>
#include	<fsdir.h>
#include	<ids.h>
#include	<dirseen.h>
#include	<localmisc.h>

#include	"sysdialer.h"


/* local defines */

#define	SD		sysdialer_head
#define	SD_FL		sysdialer_flags
#define	SD_ENT		sysdialer_entry
#define	SD_CALLS	sysdialer_calls
#define	SD_INFO		sysdialer_information
#define	SD_ARGS		sysdialer_arguments
#define	SD_PRC		sysdialer_prcache
#define	SD_MOD		sysdialer_module

#define	TO_FILECHECK	3

#define	NEXTS		3		/* number of extensions */

#define	LIBCNAME	"lib"

#ifndef	SDDNAME
#define	SDDNAME		"sysdialers"
#endif

#ifndef	VARLIBPATH
#define	VARLIBPATH	"LD_LIBRARY_PATH"
#endif

#ifndef	CF_SAMEMODULE
#define	CF_SAMEMODULE	1		/* try to use same module */
#endif
#ifndef	CF_PRCACHE
#define	CF_PRCACHE	1		/* PR-cache */
#endif
#ifndef	CF_LOOKSELF
#define	CF_LOOKSELF	0		/* allow for compiled-in sysdialers */
#endif


/* imported namespaces */


/* local typedefs */

typedef sysdialer_ent		ent ;
typedef sysdialer_mod		mod ;


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct sysdialer_calls {
	int		(*open)() noex ;
	int		(*reade)() noex ;
	int		(*recve)() noex ;
	int		(*recvfrome)() noex ;
	int		(*recvmsge)() noex ;
	int		(*write)() noex ;
	int		(*send)() noex ;
	int		(*sendto)() noex ;
	int		(*sendmsg)() noex ;
	int		(*shutdown)() noex ;
	int		(*close)() noex ;
    } ; /* end struct (sysdialer_calls) */
}

struct fext {
	cchar		*exp ;
	int		exl ;
} ;


/* forward references */

static int	sysdialer_sofind(sysdialer *,cchar *,cchar *,
			sysdialer_ent *) noex ;
static int	sysdialer_sofindprs(sysdialer *,ids *,dirseen *,
			cchar *,sysdialer_ent *) noex ;
static int	sysdialer_sofindpr(sysdialer *,ids *,dirseen *,cchar *,
			cchar *,sysdialer_ent *) noex ;
static int	sysdialer_sofindvar(sysdialer *,ids *,dirseen *,
			cchar *,sysdialer_ent *) noex ;
static int	sysdialer_socheckvarc(sysdialer *,ids *,dirseen *,
			cchar *,int, cchar *,sysdialer_ent *) noex ;
static int	sysdialer_sochecklib(sysdialer *,ids *,dirseen *,cchar *,
			cchar *,sysdialer_ent *) noex ;

#if	CF_LOOKSELF
static int	sysdialer_sofindloc(sysdialer *,ids *,
			cchar *,sysdialer_ent *) noex ;
#endif

#ifdef	COMMENT
static int	sysdialer_sotest(sysdialer *,cchar *) noex ;
#endif

static int	prcache_start(prcache *) noex ;
static int	prcache_lookup(prcache *,int,cchar **) noex ;
static int	prcache_finish(prcache *) noex ;

static int	entry_start(sysdialer_ent *,cchar *,cchar *,
			sysdialer_mod *) noex ;
static int	entry_checkdir(sysdialer_ent *,cchar *,cchar *) noex ;
static int	entry_loadcalls(sysdialer_ent *,void *) noex ;
static int	entry_hasname(sysdialer_ent *,void *,cchar *) noex ;
static int	entry_finish(sysdialer_ent *) noex ;

static int	vecstr_loadexts(vecstr *,cchar *,cchar *,int) noex ;

static int	vcmpname(cvoid **,cvoid **) noex ;

static int	getext(fext *,cchar *,int) noex ;


/* local variables */

constexpr cpcchar	prnames[] = {
	"LOCAL",
	"PCS",
	"NCMP",
	"EXTRA",
	nullptr
} ;

constexpr cpcchar	exts[] = {
	"so",
	"o",
	"",
	nullptr
} ;

constexpr cpcchar	de64[] = {
	"sparcv9",
	"sparc",
	"",
	nullptr
} ;

constexpr cpcchar	de32[] = {
	"sparcv8",
	"sparcv7",
	"sparc",
	"",
	nullptr
} ;

#if	_LP64
static cchar	*dirs64[] = {
	"syssysdialer/sparcv9",
	"syssysdialer/sparc",
	"syssysdialer",
	"sysdialers/sparcv9",
	"sysdialers/sparc",
	"sysdialers",
	nullptr
} ;
#else /* _LP64 */
static cchar	*dirs32[] = {
	"syssysdialer/sparcv8",
	"syssysdialer/sparcv7",
	"syssysdialer",
	"sysdialers/sparcv8",
	"sysdialers/sparcv7",
	"sysdialers/sparc",
	"sysdialers",
	nullptr
} ;
#endif /* _LP64 */

enum subs {
	sub_open,
	sub_reade,
	sub_recve,
	sub_recvfrome,
	sub_recvmsge,
	sub_write,
	sub_send,
	sub_sendto,
	sub_sendmsg,
	sub_shutdown,
	sub_close,
	sub_overlast
} ;

constexpr cpcchar	subs[] = {
	"open",
	"reade",
	"recve",
	"recvfrome",
	"recvmsge",
	"write",
	"send",
	"sendto",
	"sendmsg",
	"shutdown",
	"close",
	nullptr
} ;

constexpr int		rsnotconnected[] = {
	SR_PFNOSUPPORT,
	SR_AFNOSUPPORT,
	SR_NETUNREACH,
	SR_NETDOWN,
	SR_HOSTUNREACH,
	SR_HOSTDOWN,
	SR_TIMEDOUT,
	SR_CONNREFUSED,
	SR_NETRESET,
	SR_CONNABORTED,
	SR_CONNRESET,
	SR_NOENT,
	SR_COMM,
	SR_PROTO,
	0
} ;

const bool	f_samemodule = CF_SAMEMODULE ;
const bool	f_prcache = CF_PRCACHE ;
const bool	f_lookself = CF_LOOKSELF ;


/* exported variables */


/* exported subroutines */

int sysdialer_start(sysdialer *op,cchar *pr,cchar **prs,cchar **dirs) noex {
	int		rs = SR_OK ;
	int		size ;
	int		opts ;
	cchar		*cp ;

	if (op == nullptr) return SR_FAULT ;
	if (pr == nullptr) return SR_FAULT ;

	memclear(op) ;

	rs = uc_mallocstrw(pr,-1,&cp) ;
	if (rs < 0)
	    goto bad0 ;

	op->pr = cp ;
	if (prs != nullptr) {

	    rs = vecstr_start(&op->prlist,5,0) ;
	    if (rs < 0)
	        goto bad1 ;

	    op->f.vsprs = TRUE ;
	    for (int i = 0 ; prs[i] != nullptr ; i += 1) {
	        if (strcmp(prs[i],op->pr) != 0) {
	            rs = vecstr_add(&op->prlist,prs[i],-1) ;
	        }
	        if (rs < 0) break ;
	    } /* end if */

	    if (rs < 0)
	        goto bad2 ;

	} /* end if (had program roots) */

	if (dirs != nullptr) {

	    rs = vecstr_start(&op->dirlist,10,0) ;
	    if (rs < 0)
	        goto bad2 ;

	    op->f.vsdirs = (rs >= 0) ;
	    for (int i = 0 ; (rs >= 0) && (dirs[i] != nullptr) ; i += 1) {
	        rs = vecstr_add(&op->dirlist,dirs[i],-1) ;
	        if (rs < 0) break ;
	    } /* end for */

	    if (rs >= 0)
		vecstr_getvec(&op->dirlist,&op->dirs) ;

	    if (rs < 0)
	        goto bad3 ;

	} else {

#ifdef	_LP64
	    op->dirs = dirs64 ;
#else
	    op->dirs = dirs32 ;
#endif /* _LP64 */

	} /* end if */

	size = szof(SD_ENT) ;
	opts = VECOBJ_OSORTED ;
	if ((rs = vecobj_start(&op->entries,size,10,opts)) >= 0) {
	    if ((rs = prcache_start(&op->pc)) >= 0){
	        op->magic = SD_MAGIC ;
	    }
	    if (rs < 0) {
	        vecobj_finish(&op->entries) ;
	    }
	} /* end if (vecobj_start) */

ret0:
	return rs ;

/* bad stuff */
bad3:
	vecstr_finish(&op->dirlist) ;

bad2:
	if (op->f.vsprs) {
	    vecstr_finish(&op->prlist) ;
	}

bad1:
	uc_free(op->pr) ;
	op->pr = nullptr ;

bad0:
	goto ret0 ;
}
/* end subroutine (sysdialer_start) */

int sysdialer_finish(sysdialer *op) noex {
	SD_ENT	*ep ;
	int		rs = SR_OK ;
	int		rs1 ;
	int		i ;

	if (op == nullptr) return SR_FAULT ;

	if (op->magic != SD_MAGIC) return SR_NOTOPEN ;

	rs1 = prcache_finish(&op->pc) ;
	if (rs >= 0) rs = rs1 ;

	for (i = 0 ; vecobj_get(&op->entries,i,&ep) >= 0 ; i += 1) {
	    if (ep == nullptr) continue ;
	    rs1 = entry_finish(ep) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end for */

	rs1 = vecobj_finish(&op->entries) ;
	if (rs >= 0) rs = rs1 ;

	if (op->f.vsdirs) {
	    rs1 = vecstr_finish(&op->dirlist) ;
	    if (rs >= 0) rs = rs1 ;
	}

	if (op->f.vsprs) {
	    rs1 = vecstr_finish(&op->prlist) ;
	    if (rs >= 0) rs = rs1 ;
	}

	if (op->pr != nullptr) {
	    rs1 = uc_free(op->pr) ;
	    if (rs >= 0) rs = rs1 ;
	    op->pr = nullptr ;
	}

	op->magic = 0 ;
	return rs ;
}
/* end subroutine (sysdialer_finish) */

/* load a sysdialer */
int sysdialer_loadin(sysdialer *op,cchar *name,sysdialer_ent **depp) noex {
	sysdialer_mod	*mp ;
	SD_ENT	se, e, *dep ;
	int		rs ;
	int		osize ;
	int		f_alloc = FALSE ;
	void		*dhp ;

	if ((name == nullptr) || (name[0] == '\0'))
	    return SR_INVALID ;

	se.name = name ;
	rs = vecobj_search(&op->entries,&se,vcmpname,depp) ;

	if (rs >= 0) {
	    (*depp)->count += 1 ;
	    goto ret0 ;
	}

/* search for it in any modules that we have already loaded */

#if	CF_SAMEMODULE
	for (int i = 0 ; (rs = vecobj_get(&op->entries,i,&dep)) >= 0 ; i += 1) {
	    if (dep != nullptr) {
	        mp = dep->mp ;
	        dhp = mp->dhp ;
	        rs = entry_hasname(dep,dhp,name) ;
	    }
	    if (rs >= 0) break ;
	} /* end for */

	if (rs >= 0) {

	    rs = entry_start(&e,name,nullptr,mp) ;
	    if (rs < 0)
	        goto ret0 ;

	    rs = entry_loadcalls(&e,dhp) ;
	    if (rs < 0)
	        entry_finish(&e) ;

	    goto ret1 ;
	}
#endif /* CF_SAMEMODULE */

/* create a new load module descriptor */

	osize = szof(sysdialer_mod) ;
	rs = uc_malloc(osize,&mp) ;

	if (rs < 0)
	    goto bad0 ;

	f_alloc = TRUE ;
	memset(mp,0,osize) ;

/* initialize a sysdialer entry */

	rs = entry_start(&e,name,nullptr,mp) ;
	if (rs < 0)
	    goto bad1 ;

/* search for it in the filesystem */

	rs = sysdialer_sofind(op,op->pr,name,&e) ;
	if (rs < 0)
	    goto bad2 ;

	dhp = e.mp->dhp ;

/* got it */

	rs = entry_loadcalls(&e,dhp) ;
	if (rs < 0)
	    goto bad2 ;

/* save this entry */
ret1:
	rs = vecobj_add(&op->entries,&e) ;
	i = rs ;
	if (rs < 0)
	    goto bad2 ;

/* return point-to-entry to our caller */

	if (depp != nullptr) {
	    vecobj_get(&op->entries,i,depp) ;
	}

ret0:

	return rs ;

/* bad stuff */
bad2:
	entry_finish(&e) ;

bad1:
	if (f_alloc)
	    uc_free(mp) ;

bad0:
	goto ret0 ;
}
/* end subroutine (sysdialer_loadin) */


int sysdialer_loadout(sysdialer *op,cchar *name)
{
	SD_ENT	te, *dep ;
	int		rs ;
	int		ei ;

	if (op == nullptr) return SR_FAULT ;
	if (name == nullptr) return SR_FAULT ;

	if (op->magic != SD_MAGIC) return SR_NOTOPEN ;

	if (name[0] == '\0') return SR_INVALID ;

	te.name = name ;
	if ((rs = vecobj_search(&op->entries,&te,vcmpname,&dep)) >= 0) {
	    ei = rs ;
	    if (dep->count <= 1) {
	        entry_finish(dep) ;
	        vecobj_del(&op->entries,ei) ;
	    } else {
	        dep->count -= 1 ;
	    }
	} /* end if */

	return rs ;
}
/* end subroutine (sysdialer_loadout) */

int sysdialer_check(sysdialer *op,time_t daytime) noex {
	int		rs = SR_FAULT ;
	if (op) {

	if (op->magic != SD_MAGIC) return SR_NOTOPEN ;

	if (daytime == 0) daytime = time(nullptr) ;

	op->ti_lastcheck = daytime ;

	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (sysdialer_check) */


/* private subroutines */

static int sysdialer_sofind(sysdialer *op,cc *pr,cc *soname,ent *ep) noex {
	ids		id ;
	int		rs ;

	if ((rs = ids_load(&id)) >= 0) {
	dirseen		ds ;

	rs = SR_NOENT ;

#if	CF_LOOKSELF
	if ((rs < 0) && isOneOf(rsnotconnected,rs))
	    rs = sysdialer_sofindloc(op,&id,soname,ep) ;
#endif

	if ((rs < 0) && isOneOf(rsnotconnected,rs)) {
	    if ((rs = dirseen_start(&ds)) >= 0) {

	        rs = sysdialer_sofindpr(op,&id,&ds,pr,soname,ep) ;

	        if ((rs < 0) && isOneOf(rsnotconnected,rs))
	            rs = sysdialer_sofindprs(op,&id,&ds,soname,ep) ;

	        if ((rs < 0) && isOneOf(rsnotconnected,rs))
	            rs = sysdialer_sofindvar(op,&id,&ds,soname,ep) ;

	        dirseen_finish(&ds) ;
	    } /* end if (dirseen) */
	} /* end if */

	ids_release(&id) ;
	} /* end if (ids) */

	return rs ;
}
/* end subroutine (sysdialer_sofind) */

#if	CF_LOOKSELF
static int sysdialer_sofindloc(sysdialer *op,ids *idp,cc *soname,ent *ep) noex {
	sysdialer_info	*dip ;
	void		*dhp = RTLD_SELF ;
	int		rs = SR_NOTFOUND ;
	cnullptr	np{} ;
	if ((dip = (sysdialer_info *) dlsym(dhp,soname)) != np) {
	    if (strcmp(dip->name,soname) == 0) {
	        ep->osize = dip->osize ;
	        ep->flags = dip->flags ;
	        ep->mp->dhp = dhp ;
	        rs = SR_OK ;
	    } /* end if */
	} /* end if (non-null) */

	return rs ;
}
/* end subroutine (sysdialer_sofindloc) */
#endif /* CF_LOOKSELF */

static int sysdialer_sofindprs(op,idp,dsp,soname,ep)
SD	*op ;
IDS		*idp ;
dirseen		*dsp ;
cchar	soname[] ;
SD_ENT	*ep ;
{
	int		rs = SR_NOENT ;
	int		rs1 ;
	cchar	*prp ;

	for (int i = 0 ; prnames[i] != nullptr ; i += 1) {

	    rs1 = prcache_lookup(&op->pc,i,&prp) ;

	    if (rs1 >= 0)
	        rs = sysdialer_sofindpr(op,idp,dsp,prp,soname,ep) ;

	    if (rs >= 0) break ;
	} /* end for */

	return rs ;
}
/* end subroutine (sysdialer_sofindprs) */


static int sysdialer_sofindpr(op,idp,dsp,pr,soname,ep)
SD	*op ;
IDS		*idp ;
dirseen		*dsp ;
cchar	pr[] ;
cchar	soname[] ;
SD_ENT	*ep ;
{
	USTAT		sb ;
	int		rs ;
	int		rs1 ;
	int		pathlen ;
	char		libdname[MAXPATHLEN + 1] ;
	char		pathbuf[MAXPATHLEN + 1] ;

	rs = mkpath2(libdname,pr,LIBCNAME) ;
	if (rs < 0)
	    goto ret0 ;

	rs1 = dirseen_havename(dsp,libdname,-1) ;
	if (rs1 >= 0) {
	    rs = SR_NOENT ;
	    goto ret0 ;
	}

	rs = u_stat(libdname,&sb) ;
	if ((rs >= 0) && (! S_ISDIR(sb.st_mode)))
	    rs = SR_NOTDIR ;

	if (rs >= 0) {
	    rs1 = dirseen_havedevino(dsp,&sb) ;
	    if (rs1 >= 0) {
		rs = SR_NOENT ;
		goto ret0 ;
	    }
	}

	if (rs >= 0)
	    rs = sysdialer_sochecklib(op,idp,dsp,libdname,soname,ep) ;

	if ((rs < 0) && (rs != SR_NOMEM)) {
	    pathlen = pathclean(pathbuf,libdname,-1) ;
	    if (pathlen >= 0)
	        dirseen_add(dsp,pathbuf,pathlen,&sb) ;
	}

ret0:
	return rs ;
}
/* end subroutine (sysdialer_sofindpr) */


static int sysdialer_sofindvar(op,idp,dsp,soname,ep)
SD	*op ;
IDS		*idp ;
dirseen		*dsp ;
cchar	soname[] ;
SD_ENT	*ep ;
{
	int		rs = SR_NOENT ;
	cchar	*sp = getenv(VARLIBPATH) ;
	cchar	*tp ;

	if (sp != nullptr) {

	while ((tp = strpbrk(sp,":;")) != nullptr) {

	    if ((tp - sp) > 0) {

	        rs = sysdialer_socheckvarc(op,idp,dsp,sp,(tp - sp),
	            soname,ep) ;

	        if ((rs >= 0) || (rs == SR_NOMEM))
		    break ;

	    } /* end if (non-zero length) */

	    sp = (tp + 1) ;

	} /* end for */

	if ((rs < 0) && (rs != SR_NOMEM) && (sp[0] != '\0'))
	    rs = sysdialer_socheckvarc(op,idp,dsp,sp,-1,soname,ep) ;

	} /* end if (non-null) */

	return rs ;
}
/* end subroutine (sysdialer_sofindvar) */


static int sysdialer_socheckvarc(op,idp,dsp,ldnp,ldnl,soname,ep)
SD	*op ;
IDS		*idp ;
dirseen		*dsp ;
cchar	ldnp[] ;
int		ldnl ;
cchar	soname[] ;
SD_ENT	*ep ;
{
	USTAT		sb ;
	int		rs ;
	int		rs1 ;
	int		pl ;
	cchar	*pp ;
	char		pathbuf[MAXPATHLEN + 1] ;

	pp = (cchar *) pathbuf ;
	rs = pathclean(pathbuf,ldnp,ldnl) ;
	pl = rs ;
	if (rs < 0)
	    goto ret0 ;

	rs = SR_NOENT ;
	rs1 = dirseen_havename(dsp,pp,pl) ;
	if (rs1 >= 0)
	    goto ret0 ;

	rs = u_stat(pp,&sb) ;
	if ((rs >= 0) && (! S_ISDIR(sb.st_mode)))
	    rs = SR_NOTDIR ;

	if (rs >= 0) {
	    rs1 = dirseen_havedevino(dsp,&sb) ;
	    if (rs1 >= 0) {
		rs = SR_NOENT ;
		goto ret0 ;
	    }
	}

	if (rs >= 0)
	    rs = sysdialer_sochecklib(op,idp,dsp,pp,soname,ep) ;

	if ((rs < 0) && (rs != SR_NOMEM))
	     dirseen_add(dsp,pp,pl,&sb) ;

ret0:
	return rs ;
}
/* end subroutine (sysdialer_sofindvarc) */


static int sysdialer_sochecklib(op,idp,dsp,libdname,soname,ep)
SD	*op ;
IDS		*idp ;
dirseen		*dsp ;
cchar	libdname[] ;
cchar	soname[] ;
SD_ENT	*ep ;
{
	USTAT		sb ;
	int		rs = SR_NOENT ;
	int		rs1 ;
	int		i ;
	int		dsize ;
	cchar	**dirs ;
	cchar	*ldnp ;
	char		subdname[MAXPATHLEN + 1] ;

	dsize = szof(caddr_t) ;
	dirs = (dsize == 8) ? de64 : de32 ;

	for (i = 0 ; dirs[i] != nullptr ; i += 1) {

	    rs1 = SR_OK ;
	    ldnp = libdname ;
	    if (dirs[i][0] != '\0') {
		ldnp = subdname ;
	        rs1 = mkpath2(subdname,libdname,dirs[i]) ;
	    }

	    if (rs1 >= 0) {
		rs1 = u_stat(ldnp,&sb) ;
		if ((rs1 >= 0) && (! S_ISDIR(sb.st_mode)))
		    rs1 = SR_NOTDIR ;
	    }

	    if ((rs1 < 0) && (rs1 != SR_NOMEM))
		dirseen_add(dsp,ldnp,-1,&sb) ;

	    if (rs1 >= 0) {
		rs = entry_checkdir(ep,ldnp,soname) ;
	    }

	    if ((rs >= 0) || (! isNotPresent(rs))) break ;
	} /* end for (dirs) */

	return rs ;
}
/* end subroutine (sysdialer_sochecklib) */

#ifdef	COMMENT
static int sysdialer_sotest(sysdialer *op,cc *soname) noex {
	sysdialer_info	*mip ;
	cnullptr	np{} ;
	int		rs = SR_NOTFOUND ;
	if ((mip = (sysdialer_info *) dlsym(op->sop,soname)) != np) {
	    if (strcmp(mip->name,soname) == 0) {
	        ep->osize = dip->osize ;
	        ep->flags = dip->flags ;
	        ep->mp->dhp = dhp ;
		rs = SR_OK ;
	    }
	}
	return rs ;
}
/* end subroutine (sysdialer_sotest) */
#endif /* COMMENT */

#if	CF_PRCACHE

static int prcache_start(prcache *pcp) noex {
	int		rs ;
	int		osize ;
	pcp->domainname = nullptr ;
	osize = (nelem(prnames) + 1) * szof(char *) ;
	if ((rs = uc_malloc(osize,&pcp->prs)) >= 0) {
	    memset(pcp->prs,0,osize) ;
	}
	return rs ;
}
/* end subroutine (prcache_start) */

static int prcache_finish(prcache *pcp) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (pcp->domainname != nullptr) {
	    rs1 = uc_free(pcp->domainname) ;
	    if (rs >= 0) rs = rs1 ;
	    pcp->domainname = nullptr ;
	}
	if (pcp->prs != nullptr) {
	    for (int i = 0 ; i < nelem(prnames) ; i += 1) {
		if (pcp->prs[i] != nullptr) {
		    rs1 = uc_free(pcp->prs[i]) ;
	    	    if (rs >= 0) rs = rs1 ;
		}
	    } /* end for */
	    rs1 = uc_free(pcp->prs) ;
	    if (rs >= 0) rs = rs1 ;
	    pcp->prs = nullptr ;
	}
	return rs ;
}
/* end subroutine (prcache_finish) */

static int prcache_lookup(prcache *pcp,int i,cchar **rpp) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	int		osize ;
	int		len = 0 ;
	char		pr[MAXPATHLEN + 1] ;

	*rpp = nullptr ;
	if (i >= (nelem(prnames) - 1))
	    return 0 ;

	if (pcp->domainname == nullptr) {
	    char	dn[MAXHOSTNAMELEN + 1] ;
	    if ((rs = getnodedomain(nullptr,dn)) >= 0) {
		cchar	*cp ;
		if ((rs = uc_mallocstrw(dn,-1,&cp)) >= 0) {
		    pcp->domainname = cp ;
		}
	    }
	} /* end if */

	if (rs < 0)
	    goto ret0 ;

	if (pcp->prs[i] == nullptr) {

	    rs1 = mkpr(pr,MAXPATHLEN,prnames[i],pcp->domainname) ;
	    osize = 0 ;
	    if (rs1 >= 0)
		osize = rs1 ;

	    rs = uc_mallocstrw(pr,osize,(pcp->prs + i)) ;

	} /* end if */

	if ((rs >= 0) && (pcp->prs[i][0] != '\0')) {
	    *rpp = pcp->prs[i] ;
	    if ((*rpp)[0] != '\0')
		len = strlen(*rpp) ;
	}

ret0:
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (prcache_lookup) */

#endif /* CF_PRCACHE */

static int entry_start(ent *ep,cc *name,cc *itype,mod *mp) noex {
	int		rs = SR_FAULT ;
	if (ep && name) {
	    cchar	*cp ;
	    memclear(ep) ;
	    if ((rs = uc_mallocstrw(name,-1,&cp)) >= 0) {
		ep->name = cp ;
	        if ((itype != nullptr) && (itype[0] != '\0')) {
	            if ((rs = uc_mallocstrw(itype,-1,&cp)) >= 0) {
			ep->itype = cp ;
		    }
	        }
	        if (mp != nullptr) {
	            mp->count += 1 ;
	            ep->mp = mp ;
	        }
	        ep->count = 1 ;
		if ((rs < 0) && ep->itype) {
		    uc_free(ep->itype) ;
		    ep->itype = nullptr ;
		}
	    } /* end if (memory-allocation) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (entry_start) */

static int entry_finish(ent *ep) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (ep) {
	    rs = SR_OK ;
	    if (ep->name != nullptr) {
	        rs1 = uc_free(ep->name) ;
	        if (rs >= 0) rs = rs1 ;
	        ep->name = nullptr ;
	    }
	    if (ep->itype != nullptr) {
	        rs1 = uc_free(ep->itype) ;
	        if (rs >= 0) rs = rs1 ;
	        ep->itype = nullptr ;
	    }
	    if (ep->mp != nullptr) {
	        sysdialer_mod	*mp = ep->mp ;
	        if (mp->count <= 1) {
	            if ((mp->dhp != nullptr) && (mp->dhp != RTLD_DEFAULT)) {
	                dlclose(mp->dhp) ;
		    }
	            rs1 = uc_free(mp) ;
		    if (rs >= 0) rs = rs1 ;
		    ep->mp = nullptr ;
	        } else {
	            mp->count -= 1 ;
	        }
	    } /* end if */
	    memclear(ep) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (entry_finish) */

/* try to load a file with the given name */
static int entry_checkdir(ent *ep,cc *libdname,cc *name) noex {
	USTAST		sb ;
	sysdialer_info	*dip ;
	vecstr		enames ;
	int		rs ;
	int		rs1 ;
	int		dlmode ;
	int		namelen ;
	int		c ;
	int		fl = 0 ;
	void		*dhp = nullptr ;
	cchar	*fn ;
	cchar	*sublibdname = SDDNAME ;
	char		dname[MAXPATHLEN + 1] ;
	char		dlfname[MAXPATHLEN + 1] ;
	char		fname[MAXNAMELEN + 1] ;

	rs = mkpath2(dname,libdname,sublibdname) ;
	if (rs < 0)
	    goto ret0 ;

/* test for directory existing */

	rs = u_stat(dname,&sb) ;

	if ((rs >= 0) && (! S_ISDIR(sb.st_mode)))
	    rs = SR_NOTDIR ;

	if (rs < 0)
	    goto ret0 ;

	rs = vecstr_start(&enames,3,0) ;
	if (rs < 0)
	    goto ret0 ;

/* read the directory looking for the prefix name parts */

	namelen = strlen(name) ;

	rs = vecstr_loadexts(&enames,dname,name,namelen) ;
	c = rs ;
	if (rs < 0)
	    goto ret1 ;

/* load them in turn */

	rs = SR_NOTFOUND ;
	if (c == 0)
	    goto ret1 ;

	for (int i = 0 ; exts[i] != nullptr ; i += 1) {

	    if ((rs1 = vecstr_findn(&enames,exts[i],-1)) >= 0) {

	        rs = SR_INVALID ;
		fn = name ;
		if (exts[i][0] != '\0') {
		    fn = fname ;
	            mkfnamesuf1(fname,name,exts[i]) ;
		}

	        fl = mkpath2(dlfname,dname,fn) ;

	        dlmode = (RTLD_LAZY | RTLD_LOCAL) ;
	        if ((dhp = dlopen(dlfname,dlmode)) != nullptr) {

	            dip = (sysdialer_info *) dlsym(dhp,name) ;

	            if ((dip != nullptr) && (strcmp(dip->name,name) == 0)) {
	                ep->osize = dip->osize ;
	                ep->flags = dip->flags ;
	                ep->mp->dhp = dhp ;
	                rs = SR_OK ;
	                break ;
	            } else {
	                dlclose(dhp) ;
		    }

		} else {
		    rs = SR_LIBACC ;
	        } /* end if */

	    } /* end if (tried one) */

	} /* end for (extensions) */

ret1:
	vecstr_finish(&enames) ;

ret0:

	return (rs >= 0) ? fl : rs ;
}
/* end subroutine (entry_checkdir) */

/* load up the available subroutines from this module */
static int entry_loadcalls(ent *ep,void *dhp) noex {
	int		rs ;
	int		rs1 ;
	char		*nbuf{} ;
	if ((rs = malloc_mn(&nbuf)) >= 0) {
	    cnullptr	np{} ;
	    cint	nlen = rs ;
	    int		nl, cl ;
	    char	*cp ;
	    strwcpy(nbuf,ep->name,nlen) ;
	    nl = strlen(ep->name) ;
	    cp = charp(nbuf + nl) ;
	    cl = (nlen - nl) ;
	    for (int i = 0 ; (rs >= 0) && subs[i] ; i += 1) {
	        if ((rs = sncpy2(cp,cl,"_",subs[i])) >= 0) {
		    int		(*fp)() ;
	            if ((fp = (int (*)()) dlsym(dhp,symname)) != np) {
	                switch (i) {
	                case sub_open:
	                    ep->c.open = fp ;
	                    break ;
	                case sub_reade:
	                    ep->c.reade = fp ;
	                    break ;
	                case sub_recve:
	                    ep->c.recve = fp ;
	                    break ;
	                case sub_recvfrome:
	                    ep->c.recvfrome = fp ;
	                    break ;
	                case sub_recvmsge:
	                    ep->c.recvmsge = fp ;
	                    break ;
	                case sub_write:
	                    ep->c.write= fp ;
	                    break ;
	                case sub_send:
	                    ep->c.send = fp ;
	                    break ;
	                case sub_sendto:
	                    ep->c.sendto = fp ;
	                    break ;
	                case sub_sendmsg:
	                    ep->c.sendmsg = fp ;
	                    break ;
	                case sub_shutdown:
	                    ep->c.shutdown = fp ;
	                    break ;
	                case sub_close:
	                    ep->c.close = fp ;
	                    break ;
	                } /* end switch */
	            } /* end if (got a symbol) */
	        } /* end if (sncpy) */
	    } /* end for */
	    rs1 = uc_free(nbuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	if ((rs >= 0) && (ep->c.open == nullptr)) {
	    rs = SR_LIBACC ;
	}
	return rs ;
}
/* end subroutine (entry_loadcalls) */

static int entry_hasname(ent *ep,void *dhp,cc *name) noex {
	sysdialer_info	*dip ;
	int		rs = SR_NOTFOUND ;
	dip = (sysdialer_info *) dlsym(dhp,name) ;
	if ((dip != nullptr) && (strcmp(dip->name,name) == 0)) {
	    ep->osize = dip->osize ;
	    rs = SR_OK ;
	}
	return rs ;
}
/* end subroutine (entry_hasname) */

static int vecstr_loadexts(vecstr *lp,cc *dname,cc *name,int namelen) noex {
	int		rs ;
	int		rs1 ;
	int		nl ;
	int		c = 0 ;
	if ((cchar *nbuf ; (rs = malloc_mp(&nbuf)) >= 0) {
	    cint	nlen = rs ;
	    if (fsdir dir ; (rs = fsdir_open(&dir,dname)) >= 0) {
	        fsdir_ent	slot ;
	        while ((dnl = fsdir_read(&dir,&slot,nbuf,nlen)) > 0) {
		    fext	e ;
		    cint	dnl = rs ;
	            cchar	*dnp = slot.name ;
	            if (dnl < namelen) continue ;
		    nl = getext(&e,dnp,dnl) ;
	            if (nl != namelen) continue ;
 		    if (strncmp(dnp,name,namelen) != 0) continue ;
	            if ((e.exl == 0) || (matstr(exts,e.exp,e.exl) >= 0)) {
	                c += 1 ;
	                if ((rs = vecstr_add(lp,e.exp,e.exl)) >= 0) {
	                    if (c >= NEXTS) break ;
			}
	            } /* end if (got a match) */
		    if (rs < 0) break ;
	        } /* end while (directory entries) */
	        rs1 = fsdir_close(&dir) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (fsdir) */
	    rs1 = uc_free(nbuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (vecstr_loadexts) */

/* compare the whole entries (including the netgroup) */
static int vcmpname(cvoid **v1pp,cvoid **v2pp) noex {
	ent		**e1pp = (ent **) v1pp ;
	ent		**e2pp = (ent **) v2pp ;
	int		rs = 0 ;
	ent		*e1p = *e1pp ;
	ent		*e2p = *e2pp ;
	if (e1p || e2p) {
	    rc = +1 ;
	    if (e1p) {
		rc = -1 ;
	        if (e2p) {
		    rc = strcmp(*e1p->name,e2p->name) ;
		}
	    }
	}
	return rc ;
}
/* end subroutine (vcmpname) */

static int getext(fext *ep,cchar *name,int namelen) noex {
	int		mnl = strnlen(name,namelen) ;
	ep->exp = (name + mnl) ;
	ep->exl = 0 ;
	if (cchar *tp ; (tp = strnrchr(name,mnl,'.')) != nullptr) {
	    ep->exp = (tp + 1) ;
	    ep->exl = (name + mnl - (tp + 1)) ;
	    mnl = (tp - name) ;
	}
	return mnl ;
}
/* end subroutine (getext) */


