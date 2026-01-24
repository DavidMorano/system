/* sd_uss SUPPORT (Sys-Dialer) */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* SYSDIALER "uss" dialer */
/* version %I% last-modified %G% */

#define	CF_DEBUGS	0		/* non-switchable debug print-outs */

/* revision history:

	= 2003-11-04, David A­D­ Morano
	This was created as one of the first dialer modules for the
	SYSDIALER object.

*/

/* Copyright © 2003 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	uss

	Description:
	This is a SYSDIALER module.

	Synopsis:
	uss <path>

	Arguments:
	<path>		path to socket file

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<strings.h>		/* for |strcasecmp(3c)| */
#include	<clanguage.h>
#include	<usysbase.h>
#include	<keyopt.h>
#include	<ids.h>
#include	<userinfo.h>
#include	<vecstr.h>
#include	<nulstr.h>
#include	<expcook.h>
#include	<hasx.h>
#include	<ischarx.h>
#include	<localmisc.h>

#include	"sysdialer.h"
#include	"sd_uss.h"
#include	"envs.h"
#include	"inetaddrparse.h"


/* local defines */

#define	USS_MNAME	"uss"
#define	USS_VERSION	"0"
#define	USS_INAME	""
#define	USS_MF1		(SYSDIALER_MFULL | SYSDIALER_MHALFOUT)
#define	USS_MF2		(SYSDIALER_MCOR | SYSDIALER_MCO)
#define	USS_MF3		(SYSDIALER_MHALFIN)
#define	USS_MF		(USS_MF1 | USS_MF2|USS_MF3)

#define	USS_SEARCHNAME	"uss"
#define	USS_VARPR	"LOCAL"
#define	USS_PR		"/usr/add-on/local"
#define	USS_LOGDNAME	"log"
#define	USS_LOGFNAME	SYSDIALER_LF

#ifndef	SVCNAMELEN
#define	SVCNAMELEN	32
#endif

#define	ARGBUFLEN	(MAXPATHLEN + 35)

#define	SUBINFO		struct subinfo
#define	SUBINFO_ALLOCS	struct subinfo_allocs
#define	SUBINFO_FL	struct subinfo_flags


/* external subroutines */

#if	CF_DEBUGS
extern int	debugprintf(cchar *,...) ;
extern int	strlinelen(cchar *,int,int) ;
#endif


/* external variables */

extern char	**environ ;


/* local structures */

struct subinfo_flags {
	uint		stores:1 ;
	uint		ids:1 ;
	uint		userinfo:1 ;
	uint		ignore:1 ;
	uint		progdash:1 ;
	uint		log:1 ;
} ; /* end struct */

struct subinfo_allocs {
	cchar	*node ;
	cchar	*svc ;
	cchar	*pr ;
	cchar	*portspec ;
} ; /* end struct */

struct subinfo {
	cchar	**argv ;
	cchar	**envv ;
	cchar	*pr ;
	cchar	*prn ;
	cchar	*searchname ;
	cchar	*afspec ;
	cchar	*hostname ;
	cchar	*portspec ;
	cchar	*svcname ;
	cchar	*pvfname ;
	cchar	*dfname ;
	cchar	*xfname ;
	cchar	*efname ;
	cchar	*architecture ;		/* machine architecture */
	cchar	*umachine ;		/* UNAME machine name */
	cchar	*usysname ;		/* UNAME OS system-name */
	cchar	*urelease ;		/* UNAME OS release */
	cchar	*uversion ;		/* UNAME OS version */
	cchar	*hz ;			/* OS HZ */
	cchar	*nodename ;		/* userinfo */
	cchar	*domainname ;		/* userinfo */
	cchar	*username ;		/* userinfo */
	cchar	*homedname ;		/* userinfo */
	cchar	*shell ;		/* userinfo */
	cchar	*organization ;		/* userinfo */
	cchar	*gecosname ; 		/* userinfo */
	cchar	*realname ;		/* userinfo */
	cchar	*name ;			/* userinfo */
	cchar	*tz ;			/* userinfo */
	cchar	*groupname ;
	cchar	*tmpdname ;
	cchar	*maildname ;
	cchar	*hfname ;
	cchar	*lfname ;
	cchar	*paramfname ;
	cchar	*version ;		/* object version */
	cchar	*logid ;
	cchar	*defprog ;
	USS		*op ;
	SYSDIALER_ARGS	*ap ;
	IDS		id ;
	vecstr		aenvs ;
	vecstr		stores ;
	userinfo	u ;
	SUBINFO_ALLOCS	a ;
	SUBINFO_FL	f, init, open ;
	uid_t		uid ;
	gid_t		gid ;
	int		argc ;
	int		argi ;
	int		ncpu ;
	int		af ;
	int		to ;
} ; /* end struct */

struct intprog {
	char		fname[MAXPATHLEN + 1] ;
	char		arg[MAXPATHLEN + 1] ;
} ; /* end struct */

struct afamily {
	cchar	*name ;
	int		af ;
} ; /* end struct */


/* forward references */

static int	uss_logbegin(USS *op,cchar *,cchar *) ;
static int	uss_logend(USS *) ;
static int	uss_logstuff(USS *,SUBINFO *) ;

static int	subinfo_start(SUBINFO *,USS *,
			SYSDIALER_INFO *,
			SYSDIALER_ARGS *,
			cchar *,cchar *) ;
static int	subinfo_procargs(SUBINFO *) ;
static int	subinfo_procopts(SUBINFO *,KEYOPT *) ;
static int	subinfo_defaults(SUBINFO *) ;
static int	subinfo_userinfo(SUBINFO *) ;
static int	subinfo_logfile(SUBINFO *) ;
static int	subinfo_addrparse(SUBINFO *) ;
static int	subinfo_addrparseunix(SUBINFO *,int) ;
static int	subinfo_addrparseinet(SUBINFO *) ;
static int	subinfo_dirok(SUBINFO *,cchar *,int) ;
static int	subinfo_setentry(SUBINFO *,cchar **,cchar *,int) ;
static int	subinfo_finish(SUBINFO *) ;


/* local variables */

enum argopts {
	argopt_root,
	argopt_rn,
	argopt_sn,
	argopt_af,
	argopt_lf,
	argopt_pvf,
	argopt_pf,
	argopt_df,
	argopt_xf,
	argopt_ef,
	argopt_overlast
} ; /* end enum (argopts) */

constexpr cpcchar	argopts[] = {
	"ROOT",
	"RN",
	"sn",
	"af",
	"lf",
	"pvf",
	"pf",
	"df",
	"xf",
	"ef",
	nullptr
} ; /* end array (argopts) */

enum procopts {
	procopt_log,
	procopt_overlast
} ; /* end enum (procopts) */

constexpr cpcchar	procopts[] = {
	"log",
	nullptr
} ; /* end array (procopts) */


/* external variables (module information) */

SYSDIALER_INFO	uss = {
	USS_MNAME,
	USS_VERSION,
	USS_INAME,
	szof(USS),
	USS_MF
} ;


/* exported subroutines */

/* ARGSUSED */
int uss_open(op,ap,hostname,svcname,av)
USS		*op ;
SYSDIALER_ARGS	*ap ;
cchar	hostname[] ;
cchar	svcname[] ;
cchar	*av[] ;
{
	SUBINFO		si, *sip = &si ;
	int		rs = SR_OK ;
	int		rs1 ;
	int		opts = 0 ;

	if (op == nullptr) return SR_FAULT ;
	if (hostname == nullptr) return SR_FAULT ;

	if (hostname[0] == '\0') return SR_INVALID ;

	memset(op,0,sizeof(USS)) ;

#if	CF_DEBUGS
	{
	    int	i ;
	    debugprintf("uss_open: ent hostname=%s svcname=%s\n",
	        hostname,svcname) ;
	    if (ap->argv != nullptr) {
	        for (i = 0 ; ap->argv[i] != nullptr ; i += 1) {
	            debugprintf("uss_open: a%u=>%s<\n",i,ap->argv[i]) ;
	        }
	    }
	}
#endif /* CF_DEBUGS */

	if ((rs = subinfo_start(sip,op,&uss,ap,hostname,svcname)) >= 0) {

	    rs = subinfo_defaults(sip) ;

#if	CF_DEBUGS
	    debugprintf("uss_open: subinfo_logstuff()\n") ;
	    debugprintf("uss_open: f_log=%u\n",op->open.log) ;
#endif

	    if ((rs >= 0) && sip->fl.log)
	        rs = uss_logstuff(op,sip) ;

/* parse the port-specification if we have one */

	    if (rs >= 0)
	        rs = subinfo_addrparse(sip) ;

#if	CF_DEBUGS
	    debugprintf("uss_open: af=%u\n",sip->af) ;
	    debugprintf("uss_open: hostname=%s\n",hostname) ;
	    debugprintf("uss_open: portspec=%s\n",sip->portspec) ;
	    debugprintf("uss_open: svcname=%s\n",svcname) ;
#endif

/* OK, do the dial */

	    if (rs >= 0) {
	        switch (sip->af) {
	        case AF_UNSPEC:
	        case AF_UNIX:
	            if (sip->portspec != nullptr) {
	                rs = dialuss(sip->portspec,sip->to,opts) ;
	            } else
	                rs = SR_NOENT ;
	            break ;
	        } /* end switch */
	    } /* end if */
	    op->fd = rs ;

	    if (rs >= 0) {
	        op->magic = USS_MAGIC ;
	        uc_closeonexec(op->fd,true) ;
	    }

	    rs1 = subinfo_finish(sip) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if */

	if ((rs < 0) && (op->fd >= 0)) {
	    u_close(op->fd) ;
	    op->fd = -1 ;
	    op->magic = 0 ;
	}

#if	CF_DEBUGS
	debugprintf("uss_open: ret rs=%d fd=%d\n",rs,op->fd) ;
#endif

	return (rs >= 0) ? op->fd : rs ;
}
/* end subroutine (uss_open) */


int uss_reade(op,buf,buflen,to,opts)
USS		*op ;
char		buf[] ;
int		buflen ;
int		to, opts ;
{
	int	rs ;


	if (op == nullptr)
	    return SR_FAULT ;

	if (op->magic != USS_MAGIC)
	    return SR_NOTOPEN ;

	rs = uc_reade(op->fd,buf,buflen,to,opts) ;

	if (rs > 0)
	    op->tlen += rs ;

	return rs ;
}
/* end subroutine (uss_reade) */


int uss_recve(op,buf,buflen,flags,to,opts)
USS		*op ;
char		buf[] ;
int		buflen ;
int		flags ;
int		to, opts ;
{
	int	rs ;


	if (op == nullptr)
	    return SR_FAULT ;

	if (op->magic != USS_MAGIC)
	    return SR_NOTOPEN ;

	rs = uc_recve(op->fd,buf,buflen,flags,to,opts) ;

	if (rs > 0)
	    op->tlen += rs ;

	return rs ;
}
/* end subroutine (uss_recve) */


int uss_recvfrome(op,buf,buflen,flags,sap,salenp,to,opts)
USS		*op ;
char		buf[] ;
int		buflen ;
int		flags ;
void		*sap ;
int		*salenp ;
int		to, opts ;
{
	int	rs ;


	if (op == nullptr)
	    return SR_FAULT ;

	if (op->magic != USS_MAGIC)
	    return SR_NOTOPEN ;

	rs = uc_recvfrome(op->fd,buf,buflen,flags,sap,salenp,to,opts) ;

	if (rs > 0)
	    op->tlen += rs ;

	return rs ;
}
/* end subroutine (uss_recvfrome) */


int uss_recvmsge(op,msgp,flags,to,opts)
USS		*op ;
struct msghdr	*msgp ;
int		flags ;
int		to, opts ;
{
	int	rs ;


	if (op == nullptr)
	    return SR_FAULT ;

	if (op->magic != USS_MAGIC)
	    return SR_NOTOPEN ;

	rs = uc_recvmsge(op->fd,msgp,flags,to,opts) ;

	if (rs > 0)
	    op->tlen += rs ;

	return rs ;
}
/* end subroutine (uss_recvmsge) */


int uss_write(op,buf,buflen)
USS		*op ;
cchar	buf[] ;
int		buflen ;
{
	int	rs ;


	if (op == nullptr)
	    return SR_FAULT ;

	if (op->magic != USS_MAGIC)
	    return SR_NOTOPEN ;

	rs = uc_writen(op->fd,((void *) buf),buflen) ;

	if (rs > 0)
	    op->tlen += rs ;

	return rs ;
}
/* end subroutine (uss_write) */


int uss_send(op,buf,buflen,flags)
USS		*op ;
cchar	buf[] ;
int		buflen ;
int		flags ;
{
	int	rs ;


	if (op == nullptr)
	    return SR_FAULT ;

	if (op->magic != USS_MAGIC)
	    return SR_NOTOPEN ;

	rs = u_send(op->fd,buf,buflen,flags) ;

	if (rs > 0)
	    op->tlen += rs ;

	return rs ;
}
/* end subroutine (uss_send) */


int uss_sendto(op,buf,buflen,flags,sap,salen)
USS		*op ;
cchar	buf[] ;
int		buflen ;
int		flags ;
void		*sap ;
int		salen ;
{
	int	rs ;


	if (op == nullptr)
	    return SR_FAULT ;

	if (op->magic != USS_MAGIC)
	    return SR_NOTOPEN ;

	rs = u_sendto(op->fd,buf,buflen,flags,sap,salen) ;

	if (rs > 0)
	    op->tlen += rs ;

	return rs ;
}
/* end subroutine (uss_sendto) */


int uss_sendmsg(op,msgp,flags)
USS		*op ;
struct msghdr	*msgp ;
int		flags ;
{
	int	rs ;


	if (op == nullptr)
	    return SR_FAULT ;

	if (op->magic != USS_MAGIC)
	    return SR_NOTOPEN ;

	rs = u_sendmsg(op->fd,msgp,flags) ;

	if (rs > 0)
	    op->tlen += rs ;

	return rs ;
}
/* end subroutine (uss_sendmsg) */


/* shutdown */
int uss_shutdown(op,cmd)
USS		*op ;
int		cmd ;
{
	int	rs ;


	if (op == nullptr)
	    return SR_FAULT ;

	if (op->magic != USS_MAGIC)
	    return SR_NOTOPEN ;

	rs = u_shutdown(op->fd,cmd) ;

	if (rs > 0)
	    op->tlen += rs ;

	return rs ;
}
/* end subroutine (uss_shutdown) */


/* close the connection */
int uss_close(op)
USS		*op ;
{
	int	rs = SR_OK ;
	int	rs1 ;


	if (op == nullptr)
	    return SR_FAULT ;

	if (op->magic != USS_MAGIC)
	    return SR_NOTOPEN ;

	rs1 = u_close(op->fd) ;
	if (rs >= 0) rs = rs1 ;

	if (op->open.log) {
	    logfile_printf(&op->lh,"bytes=%u",op->tlen) ;
	    rs1 = uss_logend(op) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if */

	op->magic = 0 ;
	return rs ;
}
/* end subroutine (uss_close) */


/* private subroutines */


static int uss_logbegin(USS *op,cchar *lfname,cchar *logid)
{
	int	rs = SR_OK ;
	int	f = op->open.log ;

	if (! op->open.log) {
	    if ((rs = logfile_open(&op->lh,lfname,0,0666,logid)) >= 0) {
	        op->open.log = true ;
	        f = true ;
	    } else if (isNotPresent(rs))
	        rs = SR_OK ;
	} /* end if (needed opening) */

	return (rs >= 0) ? f : rs ;
}
/* end if (uss_logbegin) */


static int uss_logend(USS *op)
{
	int	rs = SR_OK ;
	int	rs1 ;

	if (op->open.log) {
	    op->open.log = false ;
	    rs1 = logfile_close(&op->lh) ;
	    if (rs >= 0) rs = rs1 ;
	}

	return rs ;
}
/* end if (uss_logend) */


static int uss_logstuff(USS *op,SUBINFO *sip)
{
	int		rs ;
	int		f = false ;

	if ((rs = subinfo_logfile(sip)) > 0) {
	    cchar	*lfname = sip->lfname ;
	    cchar	*logid = sip->logid ;
	    if ((rs = uss_logbegin(op,lfname,logid)) > 0) {
	        userinfo	*uip = &sip->u ;
	        f = true ;

	        logfile_userinfo(&op->lh,uip,0L,
	            sip->searchname,sip->version) ;

	        logfile_printf(&op->lh,"pid=%d",uip->pid) ;

	        logfile_printf(&op->lh,"pr=%s",sip->pr) ;

	        logfile_printf(&op->lh,"host=%s",sip->hostname) ;

	        logfile_printf(&op->lh,"svc=%s",sip->svcname) ;

	    } /* end if (ussmux-logbegin) */
	} /* end if (subinfo-logfile) */

	return (rs >= 0) ? f : rs ;
}
/* end subroutine (uss_logstuff) */


static int subinfo_start(sip,op,dip,ap,hostname,svcname)
SUBINFO		*sip ;
USS		*op ;
SYSDIALER_INFO	*dip ;
SYSDIALER_ARGS	*ap ;
cchar	hostname[] ;
cchar	svcname[] ;
{
	int		rs = SR_OK ;

	memset(sip,0,sizeof(SUBINFO)) ;

	sip->envv = (cchar **) environ ;
	sip->op = op ;
	sip->ap = ap ;
	sip->pr = ap->pr ;
	sip->to = ap->timeout ;
	sip->hostname = hostname ;
	sip->svcname = svcname ;
	sip->version = dip->version ;
	sip->af = -1 ;

	if ((rs = vecstr_start(&sip->stores,3,0)) >= 0) {
	    sip->open.stores = true ;
	    if ((rs = vecstr_start(&sip->aenvs,3,0)) >= 0) {
	        if (ap != nullptr) {
	            rs = subinfo_procargs(sip) ;
	        }
	        if (rs < 0)
	            vecstr_finish(&sip->aenvs) ;
	    } /* end if (vecstr-aenvs) */
	    if (rs < 0)
	        vecstr_finish(&sip->stores) ;
	} /* end if (vecstr-stores) */

#if	CF_DEBUGS
	debugprintf("uss/subinfo_start: ret rs=%d\n",rs) ;
#endif

	return rs ;
}
/* end subroutine (subinfo_start) */


static int subinfo_finish(SUBINFO *sip)
{
	int		rs = SR_OK ;
	int		rs1 ;

	if (sip->open.userinfo) {
	    sip->open.userinfo = false ;
	    rs1 = userinfo_finish(&sip->u) ;
	    if (rs >= 0) rs = rs1 ;
	}

	if (sip->open.ids) {
	    sip->open.ids = false ;
	    rs1 = ids_load(&sip->id) ;
	    if (rs >= 0) rs = rs1 ;
	}

	if (sip->a.node != nullptr) {
	    rs1 = uc_free(sip->a.node) ;
	    if (rs >= 0) rs = rs1 ;
	    sip->a.node = nullptr ;
	}

	if (sip->a.svc != nullptr) {
	    rs1 = uc_free(sip->a.svc) ;
	    if (rs >= 0) rs = rs1 ;
	    sip->a.svc = nullptr ;
	}

	if (sip->a.pr != nullptr) {
	    rs1 = uc_free(sip->a.pr) ;
	    if (rs >= 0) rs = rs1 ;
	    sip->a.pr = nullptr ;
	}

	if (sip->a.portspec != nullptr) {
	    rs1 = uc_free(sip->a.portspec) ;
	    if (rs >= 0) rs = rs1 ;
	}

	rs1 = vecstr_finish(&sip->aenvs) ;
	if (rs >= 0) rs = rs1 ;

	if (sip->open.stores) {
	    sip->open.stores = false ;
	    rs1 = vecstr_finish(&sip->stores) ;
	    if (rs >= 0) rs = rs1 ;
	}

	return rs ;
}
/* end subroutine (subinfo_finish) */


static int subinfo_procargs(SUBINFO *sip)
{
	KEYOPT		akopts ;
	SYSDIALER_ARGS	*ap = sip->ap ;
	int		rs = SR_OK ;
	int		rs1 ;
	int		argc ;
	int		argr, argl, aol, akl, avl, kwi ;
	int		ai, ai_max, ai_pos ;
	int		pan = 0 ;
	int		f_optminus, f_optplus, f_optequal ;
	int		f_dash = false ;
	cchar	**argv ;
	cchar	*argval = nullptr ;
	cchar	*argp, *aop, *akp, *avp ;

#if	CF_DEBUGS
	debugprintf("uss/subinfo_procargs: ent arguments\n") ;
#endif

	argv = (cchar **) ap->argv ;

	for (argc = 0 ; argv[argc] != nullptr ; argc += 1) ;

#if	CF_DEBUGS
	{
	    debugprintf("uss/subinfo_procargs: argc=%u\n",argc) ;
	    for (ai = 0 ; argv[ai] != nullptr ; ai += 1)
	        debugprintf("uss/subinfo_procargs: argv[%u]=%s\n",
	            ai,argv[ai]) ;
	}
#endif /* CF_DEBUGS */

	sip->argc = argc ;
	sip->argv = argv ;
	rs = keyopt_start(&akopts) ;
	if (rs < 0) goto badkopts ;

/* process program arguments */

	ai_max = 0 ;
	ai_pos = 0 ;
	argr = argc ;
	for (ai = 0 ; (ai < argc) && (argv[ai] != nullptr) ; ai += 1) {
	    if (rs < 0) break ;
	    argr -= 1 ;
	    if (ai == 0) continue ;

	    argp = argv[ai] ;
	    argl = strlen(argp) ;

	    f_optminus = (*argp == '-') ;
	    f_optplus = (*argp == '+') ;
	    if ((argl > 1) && (f_optminus || f_optplus)) {
	        const int	ach = MKCHAR(argp[1]) ;

	        if (isdigitlatin(ach)) {

	            argval = (argp + 1) ;

	        } else if ((argl == 2) && (ach == '-')) {

	            f_dash = true ;
	            ai += 1 ;
	            argr -= 1 ;

	        } else {
	            int		v ;

#if	CF_DEBUGS
	            debugprintf("uss/subinfo_procargs: option? ao=>%r<\n",
	                argp,argl) ;
#endif

	            aop = argp + 1 ;
	            akp = aop ;
	            aol = argl - 1 ;
	            f_optequal = false ;
	            if ((avp = strchr(aop,'=')) != nullptr) {
	                f_optequal = true ;
	                akl = avp - aop ;
	                avp += 1 ;
	                avl = aop + argl - 1 - avp ;
	                aol = akl ;
	            } else {
	                avp = nullptr ;
	                avl = 0 ;
	                akl = aol ;
	            }

#if	CF_DEBUGS
	            debugprintf("uss/subinfo_procargs: k=>%r<\n",akp,akl) ;
#endif

	            if ((kwi = matostr(argopts,2,akp,akl)) >= 0) {

	                switch (kwi) {

/* program-root */
	                case argopt_root:
	                    if (f_optequal) {
	                        f_optequal = false ;
	                        if (avl)
	                            sip->pr = avp ;
	                    } else {
	                        if (argr > 0) {
	                            argp = argv[++ai] ;
	                            argr -= 1 ;
	                            argl = strlen(argp) ;
	                            if (argl)
	                                sip->pr = argp ;
	                        } else
	                            rs = SR_INVALID ;
	                    }
	                    break ;

/* program-root-name */
	                case argopt_rn:
	                    if (f_optequal) {
	                        f_optequal = false ;
	                        if (avl)
	                            sip->prn = avp ;
	                    } else {
	                        if (argr > 0) {
	                            argp = argv[++ai] ;
	                            argr -= 1 ;
	                            argl = strlen(argp) ;
	                            if (argl)
	                                sip->prn = argp ;
	                        } else
	                            rs = SR_INVALID ;
	                    }
	                    break ;

/* search-name root */
	                case argopt_sn:
	                    if (f_optequal) {
	                        f_optequal = false ;
	                        if (avl)
	                            sip->searchname = avp ;
	                    } else {
	                        if (argr > 0) {
	                            argp = argv[++ai] ;
	                            argr -= 1 ;
	                            argl = strlen(argp) ;
	                            if (argl)
	                                sip->searchname = argp ;
	                        } else
	                            rs = SR_INVALID ;
	                    }
	                    break ;

/* logfile */
	                case argopt_lf:
	                    sip->fl.log = true ;
	                    if (f_optequal) {
	                        f_optequal = false ;
	                        if (avl)
	                            sip->lfname = avp ;
	                    } else {
	                        if (argr > 0) {
	                            argp = argv[++ai] ;
	                            argr -= 1 ;
	                            argl = strlen(argp) ;
	                            if (argl)
	                                sip->lfname = argp ;
	                        } else
	                            rs = SR_INVALID ;
	                    }
	                    break ;

/* path-vars file */
	                case argopt_pvf:
	                case argopt_pf:
	                    if (f_optequal) {
	                        f_optequal = false ;
	                        if (avl)
	                            sip->pvfname = avp ;
	                    } else {
	                        if (argr > 0) {
	                            argp = argv[++ai] ;
	                            argr -= 1 ;
	                            argl = strlen(argp) ;
	                            if (argl)
	                                sip->pvfname = argp ;
	                        } else
	                            rs = SR_INVALID ;
	                    }
	                    break ;

	                case argopt_df:
	                    if (f_optequal) {
	                        f_optequal = false ;
	                        if (avl)
	                            sip->dfname = avp ;
	                    } else {
	                        if (argr > 0) {
	                            argp = argv[++ai] ;
	                            argr -= 1 ;
	                            argl = strlen(argp) ;
	                            if (argl)
	                                sip->dfname = avp ;
	                        } else
	                            rs = SR_INVALID ;
	                    }
	                    break ;

	                case argopt_xf:
	                    if (f_optequal) {
	                        f_optequal = false ;
	                        if (avl)
	                            sip->xfname = avp ;
	                    } else {
	                        if (argr > 0) {
	                            argp = argv[++ai] ;
	                            argr -= 1 ;
	                            argl = strlen(argp) ;
	                            if (argl)
	                                sip->xfname = argp ;
	                        } else
	                            rs = SR_INVALID ;
	                    }
	                    break ;

	                case argopt_ef:
	                    if (f_optequal) {
	                        f_optequal = false ;
	                        if (avl)
	                            sip->efname = avp ;
	                    } else {
	                        if (argr > 0) {
	                            argp = argv[++ai] ;
	                            argr -= 1 ;
	                            argl = strlen(argp) ;
	                            if (argl)
	                                sip->efname = argp ;
	                        } else
	                            rs = SR_INVALID ;
	                    }
	                    break ;

	                } /* end switch (key words) */

	            } else {

	                while (akl--) {
	                    const int	kc = MKCHAR(*akp) ;

	                    switch (kc) {

/* address-family */
	                    case 'f':
	                        if (argr > 0) {
	                            argp = argv[++ai] ;
	                            argr -= 1 ;
	                            argl = strlen(argp) ;
	                            if (argl)
	                                sip->afspec = argp ;
	                        } else
	                            rs = SR_INVALID ;
	                        break ;

	                    case 'i':
	                        sip->fl.ignore = true ;
	                        break ;

/* options */
	                    case 'o':
	                        if (argr > 0) {
	                            argp = argv[++ai] ;
	                            argr -= 1 ;
	                            argl = strlen(argp) ;
	                            if (argl) {
					KEYOPT	*kop = &akopts ;
	                                rs = keyopt_loads(kop,argp,argl) ;
	                            }
	                        } else
	                            rs = SR_INVALID ;
	                        break ;

/* service */
	                    case 's':
	                        if (argr > 0) {
	                            argp = argv[++ai] ;
	                            argr -= 1 ;
	                            argl = strlen(argp) ;
	                            if (argl)
	                                sip->svcname = argp ;
	                        } else
	                            rs = SR_INVALID ;
	                        break ;
/* timeout */
	                    case 't':
	                        if (argr > 0) {
	                            argp = argv[++ai] ;
	                            argr -= 1 ;
	                            argl = strlen(argp) ;
	                            if (argl) {
	                                rs = cfdecti(argp,argl,&v) ;
	                                sip->to = v ;
	                            }
	                        } else
	                            rs = SR_INVALID ;
	                        break ;

/* eXported environment */
	                    case 'x':
	                        if (argr > 0) {
	                            argp = argv[++ai] ;
	                            argr -= 1 ;
	                            argl = strlen(argp) ;
	                            if (argl) {
	                                vecstr	*vlp = &sip->aenvs ;
	                                rs = vecstr_envadds(vlp,argp,argl) ;
	                            }
	                        } else
	                            rs = SR_INVALID ;
	                        break ;

	                    default:
	                        rs = SR_INVALID ;
	                        break ;

	                    } /* end switch */
	                    akp += 1 ;

	                    if (rs < 0) break ;
	                } /* end while */

	            } /* end if (individual option key letters) */

	        } /* end if (digits as argument or not) */

	    } else {

	        switch (pan) {
	        case 0:
	            sip->portspec = (cchar *) argp ;
	            break ;
	        case 1:
	            sip->svcname = (cchar *) argp ;
	            break ;
	        } /* end switch */
	        pan += 1 ;

	    } /* end if (key letter-word or positional) */

	    ai_pos = ai ;

	} /* end while (all command line argument processing) */

	sip->argi = (argc > 0) ? (ai + 1) : 0 ;

#if	CF_DEBUGS
	debugprintf("uss/subinfo_procargs: portspec=%s\n",sip->portspec) ;
	debugprintf("uss/subinfo_procargs: argi=%u\n",sip->argi) ;
#endif

	if ((rs >= 0) && ((ai_max < 0) || (ai_pos < 0))) rs = SR_BUGCHECK ;

	if ((rs >= 0) && (argval != nullptr)) {
	    rs = 1 ;
	}

	if ((rs >= 0) && f_dash) {
	    rs = 1 ;
	}

	if (rs >= 0) {
	    rs = subinfo_procopts(sip,&akopts) ;
	}

/* process any address-family specification */

#if	CF_DEBUGS
	debugprintf("uss/subinfo_procargs: af=%d afspec=%s\n",
	    sip->af, sip->afspec) ;
#endif

	if ((rs >= 0) && (sip->af < 0) && 
	    (sip->afspec != nullptr) && (sip->afspec[0] != '\0')) {

#if	CF_DEBUGS
	    debugprintf("uss/subinfo_procargs: afspec=%s\n",sip->afspec) ;
#endif

	    rs1 = getaf(sip->afspec,-1) ;
	    if (rs1 >= 0)
	        sip->af = rs1 ;

#if	CF_DEBUGS
	    debugprintf("uss/subinfo_procargs: getaf() rs=%d\n",rs1) ;
#endif

	} /* end if */

/* done */
	keyopt_finish(&akopts) ;

badkopts:

#if	CF_DEBUGS
	debugprintf("uss/subinfo_procargs: ret rs=%d\n",rs) ;
#endif

	return rs ;
}
/* end subroutine (subinfo_procargs) */


static int subinfo_procopts(SUBINFO *sip,KEYOPT *kop)
{
	KEYOPT_CUR	kcur ;
	int		rs = SR_OK ;
	int		c = 0 ;

	if ((rs = keyopt_curbegin(kop,&kcur)) >= 0) {
	    int		oi ;
	    int		kl, vl ;
	    cchar	*kp, *vp ;

	    while ((kl = keyopt_enumkeys(kop,&kcur,&kp)) >= 0) {

	        if ((oi = matostr(procopts,2,kp,kl)) >= 0) {

	            vl = keyopt_fetch(kop,kp,nullptr,&vp) ;

	            switch (oi) {
	            case procopt_log:
	                sip->fl.log = true ;
	                if ((vl > 0) && ((rs = optbool(vp,vl)) >= 0))
	                    sip->fl.log = (rs > 0) ;
	                break ;
	            } /* end switch */
	            c += 1 ;

	        } /* end if (valid option) */

	        if (rs < 0) break ;
	    } /* end while (looping through key options) */

	    keyopt_curend(kop,&kcur) ;
	} /* end if (keyopt-cur) */

	return (rs >= 0) ? c : rs ;
}
/* end subroutine (subinfo_procopts) */


static int subinfo_setentry(SUBINFO *sip,cchar **epp,cchar *sp,int sl)
{
	int		rs = SR_OK ;
	int		len = 0 ;

	if (sip == nullptr) return SR_FAULT ;
	if (epp == nullptr) return SR_FAULT ;

	if (! sip->open.stores) {
	    rs = vecstr_start(&sip->stores,4,0) ;
	    sip->open.stores = (rs >= 0) ;
	}

	if (rs >= 0) {
	    int	oi = -1 ;

	    if (*epp != nullptr) oi = vecstr_findaddr(&sip->stores,*epp) ;

	    if (sp != nullptr) {
	        len = strnlen(sp,sl) ;
	        if ((rs = vecstr_add(&sip->stores,sp,len)) >= 0) {
	            rs = vecstr_get(&sip->stores,rs,epp) ;
	        } /* end if (added new entry) */
	    } else {
	        *epp = nullptr ;
	    }
	    if ((rs >= 0) && (oi >= 0)) {
	        vecstr_del(&sip->stores,oi) ;
	    }

	} /* end if */

	return (rs >= 0) ? len : rs ;
}
/* end subroutine (subinfo_setentry) */


static int subinfo_defaults(SUBINFO *sip)
{
	SYSDIALER_ARGS	*ap = sip->ap ;
	int		rs = SR_OK ;
	int		rs1 ;
	cchar	*vp ;

#if	CF_DEBUGS
	debugprintf("uss/subinfo_defaults: ent af=%d\n",sip->af) ;
#endif

/* program-root */

	if ((sip->pr == nullptr) && (sip->prn != nullptr) && (sip->prn[0] != '\0')) {
	    char	domainname[MAXHOSTNAMELEN + 1] ;
	    char	pr[MAXPATHLEN + 1] ;

	    rs1 = getnodedomain(nullptr,domainname) ;

	    if (rs1 >= 0)
	        rs1 = mkpr(pr,MAXPATHLEN,sip->prn,domainname) ;

	    if (rs1 >= 0) {

	        rs1 = subinfo_dirok(sip,pr,rs1) ;

	        if (rs1 > 0) {
	            rs = uc_mallocstrw(pr,rs1,&sip->a.pr) ;
	            if (rs > 0)
	                sip->pr = sip->a.pr ;
	        }

	    } /* end if */

	} /* end if */

	if ((rs >= 0) && (sip->pr == nullptr)) {

	    if ((vp = getenv(USS_VARPR)) != nullptr) {

	        rs1 = subinfo_dirok(sip,vp,-1) ;
	        if (rs1 > 0)
	            sip->pr = vp ;

	    }

	} /* end if */

	if ((rs >= 0) && (sip->pr == nullptr)) {

	    vp = USS_PR ;
	    rs1 = subinfo_dirok(sip,vp,-1) ;
	    if (rs1 > 0)
	        sip->pr = vp ;

	} /* end if */

	if (sip->pr == nullptr)
	    sip->pr = ap->pr ;

/* search-name */

	if (sip->searchname == nullptr)
	    sip->searchname = USS_SEARCHNAME ;

/* log-file */

	if ((rs >= 0) && (sip->lfname == nullptr))
	    sip->lfname = USS_LOGFNAME ;

#if	CF_DEBUGS
	debugprintf("uss/subinfo_defaults: pr=%s\n",sip->pr) ;
	debugprintf("uss/subinfo_defaults: logfname=%s\n",sip->lfname) ;
#endif

/* address family */

#if	CF_DEBUGS
	debugprintf("uss/subinfo_defaults: af=%d\n",sip->af) ;
#endif

#ifdef	COMMENT
	if (sip->af < 0)
	    sip->af = AF_UNIX ;
#endif

/* out of here */

	return rs ;
}
/* end subroutine (subinfo_defaults) */


static int subinfo_userinfo(SUBINFO *sip)
{
	int		rs = SR_OK ;

	if (! sip->open.userinfo) {
	    userinfo	*uip = &sip->u ;
	    if ((rs = userinfo_start(uip,nullptr)) >= 0) {
	        sip->open.userinfo = true ;
	        sip->umachine = uip->machine ;
	        sip->usysname = uip->sysname ;
	        sip->urelease = uip->release ;
	        sip->uversion = uip->version ;
	        sip->nodename = uip->nodename ;
	        sip->username = uip->username ;
	        sip->homedname = uip->homedname ;
	        sip->shell = uip->shell ;
	        sip->organization = uip->organization ;
	        sip->gecosname = uip->gecosname ;
	        sip->realname = uip->realname ;
	        sip->name = uip->name ;
	        sip->domainname = uip->domainname ;
	        sip->tz = uip->tz ;
	        sip->logid = uip->logid ;
	        sip->uid = uip->uid ;
	        sip->gid = uip->gid ;
	    } /* end if (userinfo-start) */
	} /* end if (needed initialization) */

	return rs ;
}
/* end subroutine (subinfo_userinfo) */


static int subinfo_logfile(SUBINFO *sip)
{
	int		rs = SR_OK ;
	int		f = sip->init.log ;

	if (sip->fl.log && (! sip->init.log)) {
	    cchar	*lfname = sip->lfname ;
	    sip->init.log = true ;
	    f = true ;

	    if (lfname[0] != '/') {
	        cchar	*logdname = USS_LOGDNAME ;
	        char	tbuf[MAXPATHLEN + 1] ;
	        if ((rs = mkpath3(tbuf,sip->pr,logdname,lfname)) >= 0) {
	            cchar	**vpp = &sip->lfname ;
	            rs = subinfo_setentry(sip,vpp,tbuf,rs) ;
	        }
	    }

#if	CF_DEBUGS
	    debugprintf("subinfo/subinfo_logfile: lfname=%s\n",lfname) ;
#endif

	    if (rs >= 0) {
	        rs = subinfo_userinfo(sip) ;
	    } /* end if (ok) */

	} /* end if (needed initialziation) */

	return (rs >= 0) ? f : rs ;
}
/* end subroutine (subinfo_logfile) */


static int subinfo_dirok(SUBINFO *sip,cchar *d,int dlen)
{
	ustat	sb ;
	NULSTR		ss ;
	int		rs = SR_OK ;
	int		rs1 ;
	int		f = false ;
	cchar	*dnp ;

	if (! sip->open.ids) {
	    sip->open.ids = true ;
	    rs = ids_load(&sip->id) ;
	}

	if (rs >= 0) {
	    if ((rs = nulstr_start(&ss,d,dlen,&dnp)) >= 0) {
	        if ((rs1 = u_stat(dnp,&sb)) >= 0) {
	            if (S_ISDIR(sb.st_mode)) {
	                rs1 = permid(&sip->id,&sb,(R_OK | X_OK)) ;
	                f = (rs1 >= 0) ;
	            }
	        }
	        nulstr_finish(&ss) ;
	    } /* end if (nulstr) */
	} /* end if (ok) */

	return (rs >= 0) ? f : rs ;
}
/* end subroutine (subinfo_dirok) */

static int subinfo_addrparse(SUBINFO *sip) noex {
	int		rs = SR_OK ;

	if ((sip->portspec != nullptr) && (sip->portspec[0] != '\0')) {

	    if (hasleadcolon(sip->portspec,-1)) {
	        if (strncasecmp("unix:",sip->portspec,5) == 0) {
	            rs = subinfo_addrparseunix(sip,1) ;
	        } else {
	            rs = subinfo_addrparseinet(sip) ;
	        }
	    } else if (strchr(sip->portspec,'/') != nullptr) {
	        rs = subinfo_addrparseunix(sip,0) ;
	    } else {
	        if ((sip->af == 0) || 
	            ((sip->af >= 0) && (sip->af != AF_UNIX))) {
	            rs = subinfo_addrparseinet(sip) ;
	        } else {
	            rs = subinfo_addrparseunix(sip,0) ;
	        }
	    }

	} /* end if */

	return rs ;
}
/* end subroutine (subinfo_addrparse) */


static int subinfo_addrparseunix(SUBINFO *sip,int f)
{
	int		rs = SR_OK ;
	int		pslen = -1 ;
	cchar	*ps = sip->portspec ;
	char		tmpfname[MAXPATHLEN + 1] ;

	if (f)
	    ps += 5 ;

	if (ps[0] != '/') {
	    f = true ;
	    pslen = mkpath2(tmpfname,sip->pr,ps) ;
	    ps = tmpfname ;
	}

	if (f) {
	    rs = uc_mallocstrw(ps,pslen,&sip->a.portspec) ;
	    if (rs >= 0)
	        sip->portspec = sip->a.portspec ;
	}


	if (rs >= 0)
	    sip->af = AF_UNIX ;

	return rs ;
}
/* end subroutine (subinfo_addrparseunix) */


static int subinfo_addrparseinet(SUBINFO *sip)
{
	INETADDRPARSE	a ;
	int		rs ;
	int		rs1 ;

	if ((rs = inetaddrparse_load(&a,sip->portspec,-1)) >= 0) {

	    if ((rs >= 0) && (a.af.sp != nullptr) && a.af.sl) {
	        cchar	**vpp = &sip->afspec ;

	        if ((rs = subinfo_setentry(sip,vpp,a.af.sp,a.af.sl)) >= 0) {
	            if (strcasecmp(sip->afspec,"inet") == 0) {
	                sip->af = AF_UNSPEC ;
	            } else {
	                rs1 = getaf(sip->afspec,-1) ;
	                if (rs1 >= 0)
	                    sip->af = rs1 ;
	            }
	        }

	    } /* end if */

	    if ((rs >= 0) && (a.host.sp != nullptr) && a.host.sl) {
	        cchar	**vpp = &sip->hostname ;
	        rs = subinfo_setentry(sip,vpp,a.host.sp,a.host.sl) ;
	    }

	    if ((rs >= 0) && (a.port.sp != nullptr) && a.port.sl) {
	        cchar	**vpp = &sip->portspec ;
	        rs = subinfo_setentry(sip,vpp,a.port.sp,a.port.sl) ;
	    }

	    if (sip->af < 0)
	        sip->af = AF_INET4 ;

	} /* end if */

	return rs ;
}
/* end subroutine (subinfo_addrparseinet) */


