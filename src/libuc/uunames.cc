/* uunames SUPPORT */
/* lang=C++20 */

/* uunames-query database manager */
/* version %I% last-modified %G% */

#define	CF_NULSTR	0		/* use 'nulstr(3)' */
#define	CF_WITHENDIAN	0		/* use ENDIANness */
#define	CF_GETPROGROOT	1		/* use 'getprogroot(3dam)' */

/* revision history:

	= 1998-02-01, David A­D­ Morano
	This subroutine was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/******************************************************************************

	This little object provides access to the UUNAMES database
	and index (if any).

******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<sys/mman.h>
#include	<climits>
#include	<ctime>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<ucvariables.hh>
#include	<getbufsize.h>
#include	<mallocxx.h>
#include	<endian.h>
#include	<vecstr.h>
#include	<vecobj.h>
#include	<spawnproc.h>
#include	<storebuf.h>
#include	<ids.h>
#include	<ascii.h>
#include	<baops.h>
#include	<sbuf.h>
#include	<snx.h>
#include	<mkx.h>
#include	<mkpathx.h>
#include	<strwcpy.h>
#include	<char.h>
#include	<isnot.h>
#include	<localmisc.h>

#include	"uunames.h"
#include	"nulstr.h"


/* local defines */

#ifndef	VARSYSNAME
#define	VARSYSNAME	"SYSNAME"
#endif

#ifndef	VARRELEASE
#define	VARRELEASE	"RELEASE"
#endif

#ifndef	VARVERSION
#define	VARVERSION	"VERSION"
#endif

#ifndef	VARMACHINE
#define	VARMACHINE	"MACHINE"
#endif

#ifndef	VARARCHITECTURE
#define	VARARCHITECTURE	"ARCHITECTURE"
#endif

#ifndef	VARHZ
#define	VARHZ		"HZ"
#endif

#ifndef	VARDOMAIN
#define	VARDOMAIN	"DOMAIN"
#endif

#ifndef	VARNODE
#define	VARNODE		"NODE"
#endif

#ifndef	VARHOMEDNAME
#define	VARHOMEDNAME	"HOME"
#endif

#ifndef	VARUSERNAME
#define	VARUSERNAME	"USERNAME"
#endif

#ifndef	VARLOGNAME
#define	VARLOGNAME	"LOGNAME"
#endif

#ifndef	VARTZ
#define	VARTZ		"TZ"
#endif

#ifndef	VARPWD
#define	VARPWD		"PWD"
#endif

#ifndef	VARTMPDNAME
#define	VARTMPDNAME	"TMPDIR"
#endif

#ifndef	VARMAIL
#define	VARMAIL		"MAIL"
#endif

#ifndef	VARPATH
#define	VARPATH		"PATH"
#endif

#ifndef	VARLIBPATH
#define	VARLIBPATH	"LD_LIBRARY_PATH"
#endif

#ifndef	VARMANPATH
#define	VARMANPATH	"MANPATH"
#endif

#ifndef	VARVPATH
#define	VARVPATH	"VPATH"
#endif

#ifndef	VARPRLOCAL
#define	VARPRLOCAL	"LOCAL"
#endif

#ifndef	VARPRNCMP
#define	VARPRNCMP	"NCMP"
#endif

#ifndef	VARPRMKUU
#define	VARPRMKUU	"MKUUNAMES_PROGRAMROOT"
#endif

#undef	VARDBNAME
#define	VARDBNAME	"MKUUNAMES_DBNAME"

#ifndef	TMPDNAME
#define	TMPDNAME	"/tmp"
#endif

#ifndef	TMPVARDNAME
#define	TMPVARDNAME	"/var/tmp"
#endif

#define	UUNAMES_DBMAGICSTR	"UUNAMES"
#define	UUNAMES_PRNAME	VARPRNCMP

#define	UU		uunames
#define	LINER		struct liner
#define	ENVPOP		struct envpop

#define	VARDNAME	"var"
#define	INDDNAME	"uunames"

#define	DBNAME		"uunames"
#define	INDSUF		"sl"

#ifndef	KEYBUFLEN
#define	KEYBUFLEN	NATURALWORDLEN
#endif

#define	TO_FILEMOD	(60 * 24 * 3600)
#define	TO_MKWAIT	(5 * 50)

#define	TMPDMODE	0775

#define	DEFNAMES	10

#define	PROG_MKUUNAMES	"mkuunames"


/* imported namespaces */

using std::nullptr_t ;			/* type */
using std::nothrow ;			/* constant */


/* local typedefs */


/* external subroutines */


/* exported variables */

struct liner {
	cchar		*lp ;
	int		ll ;
} ;

struct envpop {
	cchar		*name ;
	cchar		*sub1dname ;
	cchar		*sub2dname ;
} ;


/* forward references */

template<typename ... Args>
static int uunames_ctor(uunames *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    cnullptr	np{} ;
	    rs = SR_NOMEM ;
	    memclear(op) ;
	    if ((op->nlp = new(nothrow) vecobj) != np) {
		rs = SR_OK ;
	    } /* end if (new-vecobj) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (uunames_ctor) */

static int uunames_dtor(uunames *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	    if (op->nlp) {
		delete op->nlp ;
		op->nlp = nullptr ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (uunames_dtor) */

template<typename ... Args>
static inline int uunames_magic(uunames *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magic == UUNAMES_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}
/* end subroutine (uunames_magic) */

static int	uunames_infoloadbegin(UU *,cchar *,cchar *) noex ;
static int	uunames_infoloadend(UU *) noex ;
static int	uunames_indopen(UU *,time_t) noex ;
static int	uunames_indopenpr(UU *,time_t) noex ;
static int	uunames_indopentmp(UU *,time_t) noex ;
static int	uunames_indopendname(UU *,cchar *,time_t) noex ;
static int	uunames_indclose(UU *) noex ;
static int	uunames_mkuunamesi(UU *,cchar *) noex ;
static int	uunames_envpaths(UU *,vecstr *) noex ;
static int	uunames_indtest(UU *,cchar *,time_t) noex ;
static int	uunames_indmapcreate(UU *,cchar *,time_t) noex ;
static int	uunames_indmapdestroy(UU *) noex ;
static int	uunames_filemapcreate(UU *,time_t) noex ;
static int	uunames_filemapdestroy(UU *) noex ;
static int	uunames_indmk(UU *,cchar *,time_t) noex ;
static int	uunames_indlist(UU *) noex ;
static int	uunames_indcheck(UU *,time_t) noex ;

static int	checkdname(cchar *) noex ;

static int	vecstr_defenvs(vecstr *,cchar **) noex ;
static int	vecstr_loadpath(vecstr *,cchar *) noex ;
static int	mkpathval(vecstr *,char *,int) noex ;

#ifdef	COMMENT
static int	mkindfname(char *,cchar *,cchar *,cchar *,cchar *) noex ;
#endif

static int	vesrch(cvoid *,cvoid *) noex ;


/* local variables */

constexpr cpcchar	envsys[] = {
	VARSYSNAME,
	VARRELEASE,
	VARVERSION,
	VARMACHINE,
	VARARCHITECTURE,
	VARHZ,
	VARNODE,
	VARDOMAIN,
	VARHOMEDNAME,
	VARUSERNAME,
	VARLOGNAME,
	VARTZ,
	VARPWD,
	nullptr
} ;

constexpr cpcchar	prnames[] = {
	"LOCAL",
	"NCMP",
	"EXTRA",
	"PCS",
	"GNU",
	"TOOLS",
	"DWB",
	"XDIR",
	"AST",
	nullptr
} ;

constexpr cpcchar	*envdefs[] = {
	"LOCALDOMAIN",
	"USER",
	"MAIL",
	"MAILDIR",
	"MAILDIRS",
	"LANG",
	"LC_COLLATE",
	"LC_CTYPE",
	"LC_MESSAGES",
	"LC_MONETARY",
	"LC_NUMERIC",
	"LC_TIME",
	"PATH",
	"LD_LIBRARY_PATH",
	nullptr
} ;

constexpr envpop	envpops[] = {
	{ VARPATH, "bin", "sbin" },
	{ VARLIBPATH, "lib", nullptr },
	{ VARMANPATH, "man", nullptr },
	{ nullptr, nullptr, nullptr }
} ;

static int	(*indopens[])(UU *,time_t) = {
	uunames_indopenpr,
	uunames_indopentmp,
	nullptr
} ;


/* exported variables */


/* exported subroutines */

int uunames_open(UU *op,cchar *pr,cchar *dbname) noex {
	time_t		dt = time(nullptr) ;
	int		rs ;
	if ((rs = uunames_ctor(op,pr)) >= 0) {
	    rs = SR_INVALID ;
	    if (pr[0]) {
	        if ((dbname == nullptr) || (dbname[0] == '\0')) {
	            dbname = DBNAME ;
	        }
		if ((rs = uunames_infoloadbegin(op,pr,dbname)) >= 0) {
	            cint	sz = sizeof(LINER) ;
	            cint	ne = DEFNAMES ;
	            if ((rs = vecobj_start(op->nlp,sz,ne,0)) >= 0) {
			if ((rs = uunames_indopen(op,dt)) >= 0) {
			    op->magic = UUNAMES_MAGIC ;
			}
			if (rs < 0) {
			    vecobj_finish(op->nlp) ;
			}
		    } /* end if (vecobj_start) */
		    if (rs < 0) {
			uunames_infoloadend(op) ;
		    }
		} /* end if (infoload) */
	    } /* end if (valid) */
	    if (rs < 0) {
		uunames_dtor(op) ;
	    }
	} /* end if (uunames_ctor) */
	return rs ;
/* end subroutine (uunames_open) */

int uunames_close(UU *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = uunames_magic(op)) >= 0) {
	    {
	        rs1 = uunames_indclose(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    if (op->nlp) {
	        rs1 = vecobj_finish(op->nlp) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = uunames_infoloadend(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
		rs1 = uunames_dtor(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    op->magic = 0 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (uunames_close) */


int uunames_audit(UU *op)
{
	int		rs = SR_OK ;

	if (op == nullptr) return SR_FAULT ;

	if (op->magic != UUNAMES_MAGIC) return SR_NOTOPEN ;

	rs = uunames_indcheck(op,0) ;

	return rs ;
}
/* end subroutine (uunames_audit) */


int uunames_curbegin(UU *op,UUNAMES_CUR *curp)
{
	int		rs = SR_OK ;

	if (op == nullptr) return SR_FAULT ;
	if (curp == nullptr) return SR_FAULT ;

	if (op->magic != UUNAMES_MAGIC) return SR_NOTOPEN ;

	memset(curp,0,sizeof(UUNAMES_CUR)) ;
	curp->i = -1 ;
	op->ncursors += 1 ;

	rs = uunames_indcheck(op,0) ;

	return rs ;
}
/* end subroutine (uunames_curbegin) */


int uunames_curend(UU *op,UUNAMES_CUR *curp)
{
	int		rs = SR_OK ;

	if (op == nullptr) return SR_FAULT ;
	if (curp == nullptr) return SR_FAULT ;

	if (op->magic != UUNAMES_MAGIC) return SR_NOTOPEN ;

	curp->i = -1 ;
	if (op->ncursors > 0)
	    op->ncursors -= 1 ;

	return rs ;
}
/* end subroutine (uunames_curend) */


int uunames_exists(UU *op,cchar *s,int slen)
{
	LINER	le ;
	int		rs = SR_OK ;
	int		kl ;
	cchar	*kp = nullptr ;

	if (op == nullptr) return SR_FAULT ;
	if (s == nullptr) return SR_FAULT ;

	if (op->magic != UUNAMES_MAGIC) return SR_NOTOPEN ;

	if (s[0] == '\0') return SR_INVALID ;

	rs = uunames_indcheck(op,0) ;
	if (rs < 0)
	    goto ret0 ;

#if	CF_NULSTR
	{
	    nulstr	ss ;
	    if ((rs = nulstr_start(&ss,s,slen,&kp)) >= 0) {
	        kl = rs ;
		le.lp = kp ;
		le.ll = kl ;
	        rs = vecobj_search(op->nlp,&le,vesrch,nullptr) ;
	        nulstr_finish(&ss) ;
	    } /* end if (nulstr) */
	}
#else
	{
	    if ((rs = uc_mallocstrw(s,slen,&kp)) >= 0) {
	        kl = rs ;

	        le.lp = kp ;
	        le.ll = kl ;
	        rs = vecobj_search(op->nlp,&le,vesrch,nullptr) ;
	        uc_free(kp) ;
	    } /* end if (allocation) */

	}
#endif /* CF_NULSTR */

ret0:
	return rs ;
}
/* end subroutine (uunames_exists) */


int uunames_enum(op,curp,buf,buflen)
UUNAMES		*op ;
UUNAMES_CUR	*curp ;
char		buf[] ;
int		buflen ;
{
	LINER	*lep ;
	int		rs = SR_OK ;
	int		i ;
	int		len = 0 ;

	if (op == nullptr) return SR_FAULT ;
	if (curp == nullptr) return SR_FAULT ;
	if (buf == nullptr) return SR_FAULT ;

	if (op->magic != UUNAMES_MAGIC) return SR_NOTOPEN ;

	i = (curp->i >= 0) ? (curp->i + 1) : 0 ;

	if ((rs = vecobj_get(op->nlp,i,&lep)) >= 0) {
	    if ((rs = snwcpy(buf,buflen,lep->lp,lep->ll)) >= 0) {
	        len = rs ;
	        curp->i = i ;
	    }
	}

ret0:
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (uunames_enum) */


int uunames_count(op)
UUNAMES		*op ;
{
	int		rs ;

	if (op == nullptr) return SR_FAULT ;

	if (op->magic != UUNAMES_MAGIC) return SR_NOTOPEN ;

	if ((rs = uunames_indcheck(op,0)) >= 0) {
	    rs = vecobj_count(op->nlp) ;
	}

	return rs ;
}
/* end subroutine (uunames_count) */


/* private subroutines */


static int uunames_infoloadbegin(op,pr,dbname)
UUNAMES		*op ;
cchar	pr[] ;
cchar	dbname[] ;
{
	int		rs = SR_OK ;

	op->pr = (char *) pr ;
	op->dbname = (char *) dbname ;

	return rs ;
}
/* end subroutine (uunames_infoloadbegin) */


static int uunames_infoloadend(UU *op)
{

	op->pr = nullptr ;
	op->dbname = nullptr ;
	return SR_OK ;
}
/* end subroutine (uunames_infoloadend) */


static int uunames_indmapcreate(UU *op,cchar *indname,time_t dt)
{
	int		rs = SR_OK ;
	int		fl ;
	char		indfname[MAXPATHLEN + 1] ;

	op->indfname = nullptr ;
#if	CF_WITHENDIAN
	rs = mkfnamesuf2(indfname,indname,INDSUF,ENDIANSTR) ;
#else
	rs = mkfnamesuf1(indfname,indname,INDSUF) ;
#endif
	fl = rs ;
	if (rs < 0)
	    goto bad0 ;

	rs = uc_mallocstrw(indfname,fl,&op->indfname) ;
	if (rs < 0)
	    goto bad0 ;

	rs = uunames_filemapcreate(op,dt) ;
	if (rs < 0)
	    goto bad1 ;

	rs = uunames_indlist(op) ;

ret0:
	return rs ;

/* bad stuff */
bad1:
	uc_free(op->indfname) ;
	op->dbname = nullptr ;

bad0:
	goto ret0 ;
}
/* end subroutine (uunames_indmapcreate) */


static int uunames_indmapdestroy(UU *op)
{
	int		rs = SR_OK ;
	int		rs1 ;
	int		i ;

	for (i = 0 ; vecobj_del(op->nlp,i) >= 0 ; i += 1) ;

	rs1 = uunames_filemapdestroy(op) ;
	if (rs >= 0) rs = rs1 ;

	if (op->indfname != nullptr) {
	    rs1 = uc_free(op->indfname) ;
	    if (rs >= 0) rs = rs1 ;
	    op->dbname = nullptr ;
	}

	return rs ;
}
/* end subroutine (uunames_indmapdestroy) */


static int uunames_filemapcreate(UU *op,time_t dt)
{
	USTAT		sb ;
	int		rs = SR_OK ;
	int		fd ;
	int		mprot, mflags ;

	if (dt == 0) dt = time(nullptr) ;

/* open it */

	rs = u_open(op->indfname,O_RDONLY,0666) ;
	fd = rs ;
	if (rs < 0)
	    goto ret0 ;

	rs = u_fstat(fd,&sb) ;
	if (rs < 0)
	    goto ret1 ;

	if (! S_ISREG(sb.st_mode)) {
	    rs = SR_NOTSUP ;
	    goto ret1 ;
	}

	if ((sb.st_size > INT_MAX) || (sb.st_size < 0)) {
	    rs = SR_TOOBIG ;
	    goto ret1 ;
	}

	op->indfsize = sb.st_size ;
	op->ti_mod = sb.st_mtime ;

/* map it */

	mprot = PROT_READ ;
	mflags = MAP_SHARED ;
	rs = u_mmap(nullptr,(size_t) op->indfsize,mprot,mflags,
	    fd,0L,&op->indfmap) ;

	if (rs >= 0) {
	    op->ti_map = dt ;
	    op->ti_lastcheck = dt ;
	}

/* close it */
ret1:
	u_close(fd) ;

ret0:
	return rs ;
}
/* end subroutine (uunames_filemapcreate) */


static int uunames_filemapdestroy(UU *op)
{
	int		rs = SR_OK ;
	int		rs1 ;

	if (op->indfmap != nullptr) {
	    rs1 = u_munmap(op->indfmap,op->indfsize) ;
	    if (rs >= 0) rs = rs1 ;
	    op->indfmap = nullptr ;
	    op->indfsize = 0 ;
	}

	return rs ;
}
/* end subroutine (uunames_filemapdestroy) */


static int uunames_indopen(UU *op,time_t dt)
{
	int		rs = SR_NOENT ;
	int		f = FALSE ;
	int		i ;

	for (i = 0 ; indopens[i] != nullptr ; i += 1) {
	    if ((rs = (*indopens[i])(op,dt)) >= 0) {
		f = TRUE ;
	    } else if (isNotPresent(rs)) {
		rs = SR_OK ;
	    }
	    if (rs < 0) break ;
	} /* end for */

	return (rs >= 0) ? f : rs ;
}
/* end subroutine (uunames_indopen) */


static int uunames_indopenpr(UU *op,time_t dt)
{
	int		rs ;
	char		idname[MAXPATHLEN + 1] ;

	if ((rs = mkpath3(idname,op->pr,VARDNAME,INDDNAME)) >= 0) {
	    rs = uunames_indopendname(op,idname,dt) ;
	}

	return rs ;
}
/* end subroutine (uunames_indopenpr) */


static int uunames_indopentmp(UU *op,time_t dt)
{
	int		rs ;
	cchar	*tmpdname = TMPVARDNAME ;
	cchar	*inddname = INDDNAME ;
	cchar	*prname ;
	char		idname[MAXPATHLEN + 1] ;

	if ((rs = sfbasename(op->pr,-1,&prname)) > 0) {
	    if ((rs = mkpath3(idname,tmpdname,prname,inddname)) >= 0) {
	        rs = uunames_indopendname(op,idname,dt) ;
	    }
	} else {
	    rs = SR_INVALID ;
	}

	return rs ;
}
/* end subroutine (uunames_indopentmp) */


static int uunames_indopendname(UU *op,cchar *dname,time_t dt)
{
	int		rs ;
	int		f_ok = FALSE ;
	int		f_mk = FALSE ;
	char		indname[MAXPATHLEN + 1] ;

	rs = mkpath2(indname,dname,op->dbname) ;
	if (rs < 0)
	    goto ret0 ;

	rs = uunames_indtest(op,indname,dt) ;
	f_ok = (rs > 0) ;

	if (rs < 0)
	    goto ret0 ;

	if (((rs < 0) && (rs != SR_NOMEM)) || (! f_ok)) {
	    if ((rs = uunames_mkuunamesi(op,dname)) >= 0) {
		f_mk = TRUE ;
	        rs = uunames_indtest(op,indname,dt) ;
	        f_ok = (rs > 0) ;
	    }
	}

	if (((rs < 0) && (rs != SR_NOMEM)) || (! f_ok)) {
	    f_mk = TRUE ;
	    rs = uunames_indmk(op,dname,dt) ;
	}

	if (rs >= 0) {
	    rs = uunames_indmapcreate(op,indname,dt) ;
	    op->f.varind = (rs >= 0) ;
	}

	if ((rs < 0) && (rs != SR_BADFMT) && (! f_mk)) {
	    rs = uunames_indmk(op,dname,dt) ;
	    if (rs >= 0) {
		rs = uunames_indmapcreate(op,indname,dt) ;
	        op->f.varind = (rs >= 0) ;
	    }
	}

ret0:
	return rs ;
}
/* end subroutine (uunames_indopendname) */


static int uunames_indtest(UU *op,cchar *indname,time_t dt)
{
	struct ustat	sb ;
	time_t		ti_ind ;
	int		rs ;
	int		rs1 ;
	int		f = FALSE ;
	char		indfname[MAXPATHLEN + 1] ;

#if	CF_WITHENDIAN
	rs = mkfnamesuf2(indfname,indname,INDSUF,ENDIANSTR) ;
#else
	rs = mkfnamesuf1(indfname,indname,INDSUF) ;
#endif

	if (rs < 0)
	    goto ret0 ;

	rs1 = u_stat(indfname,&sb) ;
	ti_ind = sb.st_mtime ;

	if ((rs1 >= 0) && ((sb.st_size == 0) || (ti_ind == 0)))
	    rs1 = SR_NOTFOUND ;

	if ((rs1 >= 0) && (op->ti_mod > ti_ind))
	    rs1 = SR_NOTFOUND ;

	if ((rs1 >= 0) && ((dt - ti_ind) >= TO_FILEMOD))
	    rs1 = SR_TIMEDOUT ;

	f = (rs1 >= 0) ;

ret0:
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (uunames_indtest) */


/* ARGSUSED */
static int uunames_indmk(UU *op,cchar *dname,time_t dt)
{
	int		rs ;
	int		c = 0 ;
	char		indname[MAXPATHLEN + 1] ;

/* check the given directory for writability */

	if ((rs = checkdname(dname)) == SR_NOENT) {
	    rs = mkdirs(dname,TMPDMODE) ;
	}
	if (rs >= 0) {
	    rs = mkpath2(indname,dname,op->dbname) ;
	}

	return (rs >= 0) ? c : rs ;
}
/* end subroutine (uunames_indmk) */


static int uunames_indclose(UU *op)
{
	int		rs = SR_OK ;
	int		rs1 ;

	if (op->f.varind) {
	    op->f.varind = FALSE ;
	    rs1 = uunames_indmapdestroy(op) ;
	    if (rs >= 0) rs = rs1 ;
	}

	return rs ;
}
/* end subroutine (uunames_indclose) */


/* make the index */
static int uunames_mkuunamesi(op,dname)
UUNAMES		*op ;
cchar	dname[] ;
{
	SPAWNPROC	ps{} ;

	vecstr	envs ;

	pid_t	cpid ;

	int	rs ;
	int	i, cstat, cex ;
	int	prlen = 0 ;

	cchar	*varprmkuu = VARPRMKUU ;
	cchar	*pn = PROG_MKUU ;
	cchar	*av[10] ;
	cchar	**ev ;

	char	progfname[MAXPATHLEN + 1] ;
	char	dbname[MAXPATHLEN + 1] ;
	if (dname == nullptr)
	    return SR_FAULT ;

	if (dname[0] == '\0')
	    return SR_INVALID ;

	rs = mkpath2(dbname,dname,op->dbname) ;
	if (rs < 0)
	    goto ret0 ;

	rs = vecstr_start(&envs,10,VECSTR_OCOMPACT) ;
	if (rs < 0)
	    goto ret0 ;

#if	CF_GETPROGROOT

	rs = getprogroot(op->pr,prnames,&prlen,progfname,pn) ;

	if (rs == 0)
	    rs = mkpath1(progfname,pn) ;

#ifdef	COMMENT
	if ((rs >= 0) && (prlen > 0)) {
	    rs = vecstr_envadd(&envs,varprmkuu,progfname,prlen) ;
	} else if (rs >= 0)
	    rs = vecstr_envadd(&envs,varprmkuu,op->pr,-1) ;
#endif /* COMMENT */

#else /* CF_GETPROGROOT */

	rs = prgetprogpath(op->pr,progfname,pn,-1) ;
	if (rs == 0)
	    rs = mkpath1(progfname,progmkuunamesi) ;

#endif /* CF_GETPROGROOT */

	if (rs < 0)
	    goto ret2 ;

/* setup environment for child process */

	if (rs >= 0)
	    rs = uunames_envpaths(op,&envs) ;

	if (rs >= 0)
	    rs = vecstr_envadd(&envs,varprmkuu,op->pr,-1) ;

	if (rs >= 0)
	    rs = vecstr_envadd(&envs,VARDBNAME,dbname,-1) ;

	if (rs >= 0)
	    rs = vecstr_defenvs(&envs,envsys) ;

	if (rs >= 0)
	    rs = vecstr_defenvs(&envs,prnames) ;

	if (rs >= 0)
	    rs = vecstr_defenvs(&envs,envdefs) ;

	if (rs < 0)
	    goto ret2 ;

/* setup arguments */

	i = 0 ;
	av[i++] = pn ;
	av[i++] = nullptr ;

/* go */

	vecstr_getvec(&envs,(cchar ***) &ev) ;
	ps.opts |= SPAWNPROC_OIGNINTR ;
	ps.opts |= SPAWNPROC_OSETPGRP ;

	for (i = 0 ; i < 3 ; i += 1) {
	    ps.disp[i] = (i != 2) ? SPAWNPROC_DCLOSE : SPAWNPROC_DINHERIT ;
	}

	rs = spawnproc(&ps,progfname,av,ev) ;
	cpid = rs ;

ret2:
	vecstr_finish(&envs) ;

ret1:
	if (rs < 0)
	    goto ret0 ;

	cstat = 0 ;
	rs = u_waitpid(cpid,&cstat,0) ;

	if (rs >= 0) {

	    cex = 0 ;
	    if (WIFSIGNALED(cstat))
	        rs = SR_UNATCH ;	/* protocol not attached */

	    if ((rs >= 0) && WIFEXITED(cstat)) {
	        cex = WEXITSTATUS(cstat) ;
	        if (cex != 0) rs = SR_LIBBAD ;
	    }

	} /* end if (process finished) */

ret0:
	return rs ;
}
/* end subroutine (uunames_mkuunamesi) */


static int uunames_envpaths(op,elp)
UUNAMES		*op ;
vecstr		*elp ;
{
	vecstr	pathcomps ;

	int	rs ;
	int	i ;
	int	opts ;
	int	sz ;
	int	bl, pl ;

	cchar	*subdname ;

	char	pathbuf[MAXPATHLEN + 1] ;
	char	*bp = nullptr ;
	char	*vp ;


	opts = VECSTR_OORDERED | VECSTR_OSTSIZE ;
	rs = vecstr_start(&pathcomps,40,opts) ;
	if (rs < 0)
	    goto ret0 ;

	for (i = 0 ; envpops[i].name != nullptr ; i += 1) {
	    cchar	*enp = envpops[i].name ;

	    subdname = envpops[i].sub1dname ;
	    if ((rs >= 0) && (subdname != nullptr)) {

	        rs = mkpath2(pathbuf,op->pr,subdname) ;
	        pl = rs ;
	        if (rs >= 0)
	            rs = vecstr_add(&pathcomps,pathbuf,pl) ;

	    } /* end if */

	    subdname = envpops[i].sub2dname ;
	    if ((rs >= 0) && (subdname != nullptr)) {

	        rs = mkpath2(pathbuf,op->pr,subdname) ;
	        pl = rs ;
	        if (rs >= 0)
	            rs = vecstr_add(&pathcomps,pathbuf,pl) ;

	    } /* end if */

	    if ((rs >= 0) && ((vp = getenv(enp)) != nullptr)) {
	        rs = vecstr_loadpath(&pathcomps,vp) ;
	    }

	    if (rs >= 0) {
	        sz = vecstr_strsize(&pathcomps) ;
	    }

	    if ((rs >= 0) && ((rs = uc_malloc(sz,&bp)) >= 0)) {
	        if ((rs = mkpathval(&pathcomps,bp,(sz-1))) >= 0) {
	            bl = rs ;
	            rs = vecstr_envadd(elp,enp,bp,bl) ;
		}
	        uc_free(bp) ;
	    } /* end if (memory allocation) */
	    vecstr_delall(&pathcomps) ;
	    if (rs < 0) break ;
	} /* end for */

ret1:
	vecstr_finish(&pathcomps) ;

ret0:
	return rs ;
}
/* end subroutine (uunames_envpaths) */


static int uunames_indlist(op)
UUNAMES		*op ;
{
	LINER	le ;

	uint	lineoff = 0 ;

	int	rs = SR_OK ;
	int	ml ;
	int	len ;
	int	n = 0 ;

	cchar	*mp ;
	cchar	*tp ;
	cchar	*filemagic = UUNAMES_DBMAGICSTR ;


	mp = (cchar *) op->indfmap ;
	ml = op->indfsize ;

	lineoff = 0 ;
	while ((tp = strnchr(mp,ml,'\n')) != nullptr) {

	    len = ((tp + 1) - mp) ;
	    le.lp = mp ;
	    le.ll = (len - 1) ;

	    if (lineoff > 0) {
		if ((le.ll > 0) && (le.lp[0] != '#')) {
		    n += 1 ;
		    rs = vecobj_add(op->nlp,&le) ;
		}
	    } else {
		if ((le.ll == 0) || (strncmp(le.lp,filemagic,le.ll) != 0)) {
		    rs = SR_LIBBAD ;
		}
	    }
	    if (rs < 0) break ;

	    lineoff += len ;
	    mp += len ;
	    ml -= len ;

	} /* end while (processing lines) */

ret0:
	return (rs >= 0) ? n : rs ;
}
/* end subroutine (uunames_indlist) */


/* ARGSUSED */
static int uunames_indcheck(UU *op,time_t dt)
{
	int		rs = SR_OK ;
	int		f = FALSE ;

	if (op->indfmap == nullptr) rs = SR_NOTFOUND ;

	return (rs >= 0) ? f : rs ;
}
/* end subroutine (uunames_indcheck) */


static int checkdname(cchar *dname)
{
	int		rs = SR_OK ;

	if (dname[0] == '/') {
	    USTAT	sb ;
	    if ((rs = u_stat(dname,&sb)) >= 0) {
		if (S_ISDIR(sb.st_mode)) {
	    	    rs = perm(dname,-1,-1,nullptr,W_OK) ;
		} else {
	            rs = SR_NOTDIR ;
		}
	    }
	} else {
	    rs = SR_INVALID ;
	}

	return rs ;
}
/* end subroutine (checkdname) */


#ifdef	COMMENT

static int mkindfname(buf,dname,name,suf,end)
char		buf[] ;
cchar	dname[] ;
cchar	name[] ;
cchar	suf[] ;
cchar	end[] ;
{
	int	rs = SR_OK ;
	int	buflen = MAXPATHLEN ;
	int	dnl = 0 ;
	int	i = 0 ;


	if (rs >= 0) {
	    rs = storebuf_strw(buf,buflen,i,dname,-1) ;
	    i += rs ;
	    dnl = rs ;
	}

	if ((rs >= 0) && (dname[dnl - 1] != '/')) {
	    rs = storebuf_chr(buf,buflen,i,'/') ;
	    i += rs ;
	}

	if (rs >= 0) {
	    rs = storebuf_strw(buf,buflen,i,name,-1) ;
	    i += rs ;
	}

	if (rs >= 0) {
	    rs = storebuf_chr(buf,buflen,i,'.') ;
	    i += rs ;
	}

	if (rs >= 0) {
	    rs = storebuf_strw(buf,buflen,i,suf,-1) ;
	    i += rs ;
	}

	if (rs >= 0) {
	    rs = storebuf_strw(buf,buflen,i,end,-1) ;
	    i += rs ;
	}

	return (rs >= 0) ? i : rs ;
}
/* end subroutine (mkindfname) */

#endif /* COMMENT */


static int vecstr_defenvs(vecstr *elp,cchar **ea)
{
	int		rs = SR_OK ;
	int		i ;
	int		c = 0 ;
	char		*cp ;

	for (i = 0 ; ea[i] != nullptr ; i += 1) {
	    if ((cp = getenv(ea[i])) != nullptr) {
		rs = vecstr_envadd(elp,ea[i],cp,-1) ;
	    }
	    if (rs < 0) break ;
	} /* end for */

	return (rs >= 0) ? c : rs ;
}
/* end subroutine (vecstr_defenvs) */


static int vecstr_loadpath(vecstr *clp,cchar *pp)
{
	int		rs = SR_OK ;
	int		rs1 ;
	int		cl ;
	int		c = 0 ;
	char		tmpfname[MAXPATHLEN + 1] ;
	char		*cp ;

	while ((cp = strpbrk(pp,":;")) != nullptr) {

	    cl = pathclean(tmpfname,pp,(cp - pp)) ;

	    rs1 = vecstr_findn(clp,tmpfname,cl) ;
	    if (rs1 == SR_NOTFOUND) {
	        c += 1 ;
		rs = vecstr_add(clp,tmpfname,cl) ;
	    }

	    if ((rs >= 0) && (cp[0] == ';')) {
		rs = vecstr_adduniq(clp,";",1) ;
	    }

	    pp = (cp + 1) ;
	    if (rs < 0) break ;
	} /* end while */

	if ((rs >= 0) && (pp[0] != '\0')) {

	    cl = pathclean(tmpfname,pp,-1) ;

	    rs1 = vecstr_findn(clp,tmpfname,cl) ;
	    if (rs1 == SR_NOTFOUND) {
	        c += 1 ;
	        rs = vecstr_add(clp,tmpfname,cl) ;
	    }

	} /* end if (trailing one) */

	return (rs >= 0) ? c : rs ;
}
/* end subroutine (vecstr_loadpath) */


static int mkpathval(clp,vbuf,vbuflen)
vecstr		*clp ;
char		vbuf[] ;
int		vbuflen ;
{
	int		rs = SR_OK ;
	int		i ;
	int		sch ;
	int		c = 0 ;
	int		rlen = 0 ;
	int		f_semi = FALSE ;
	cchar	*cp ;

	if (vbuflen >= 0) {
	vbuf[0] = '\0' ;
	for (i = 0 ; vecstr_get(clp,i,&cp) >= 0 ; i += 1) {
	    if (cp != nullptr) {

	        if (cp[0] != ';') {

	            if (c++ > 0) {
	                if (f_semi) {
	                    f_semi = FALSE ;
	                    sch = ';' ;
	                } else {
	                    sch = ':' ;
		        }

	                rs = storebuf_chr(vbuf,vbuflen,rlen,sch) ;
	                rlen += rs ;

	            } /* end if */

	            if (rs >= 0) {
	                rs = storebuf_strw(vbuf,vbuflen,rlen,cp,-1) ;
	                rlen += rs ;
	            }
    
	        } else {
	            f_semi = TRUE ;
	        }

	    }
	    if (rs < 0) break ;
	} /* end for */
	} else {
	    rs = SR_NOANODE ;
	}

	return (rs >= 0) ? rlen : rs ;
}
/* end subroutine (mkpathval) */

static int vesrch(void *v1p,void *v2p) noex {
	LINER		**e1pp = (LINER **) v1p ;
	LINER	**e2pp = (LINER **) v2p ;
	LINER	*l1 ;
	LINER	*l2 ;
	int		rc = 0 ;
	l1 = *e1pp ;
	l2 = *e2pp ;
	if (l1 != nullptr) {
	    if (l2 != nullptr) {
		if ((rc = (l1->lp[0] - l2->lp[0])) == 0) {
	    	    if ((rc = strncmp(l1->lp,l2->lp,l1->ll)) == 0) {
	    		rc = (l1->ll - l2->ll) ;
		    }
		}
	    } else {
	        rc = +1 ;
	    }
	} else {
	    rc = -1 ;
	}
	return rc ;
}
/* end subroutine (vesrch) */


