/* uux SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* SYSDIALER "uux" dialer */
/* version %I% last-modified %G% */

#define	CF_SOCKET	0		/* use socket or pipe */

/* revision history:

	= 2003-11-04, David A­D­ Morano
	This subroutine was adopted for use from the DWD program.

*/

/* Copyright © 2003 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	uux

	Description:
	This is a SYSDIALER module.

	Synopsis:
	uux [-R <pr>] [<node>!<svc>] [<arg(s)>]

	Arguments:
	-R <pr>		program-root
	<node>		UU nodename
	<svc>		service
	<arg(s)>	dialer arguments

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<netdb.h>
#include	<usystem.h>
#include	<baops.h>
#include	<vecstr.h>
#include	<vechand.h>
#include	<ids.h>
#include	<keyopt.h>
#include	<logfile.h>
#include	<getusername.h>
#include	<userinfo.h>
#include	<permx.h>
#include	<localmisc.h>

#include	"uux.h"
#include	"sysdialer.h"
#include	"nulstr.h"


/* local defines */

#define	UUX_MNAME	"uux"
#define	UUX_VERSION	"0"
#define	UUX_INAME	""
#define	UUX_MF1		(SYSDIALER_MHALFOUT)
#define	UUX_MF2		(SYSDIALER_MCOR | SYSDIALER_MCO)
#define	UUX_MF3		(SYSDIALER_MARGS)
#define	UUX_MF		(UUX_MF1 | UUX_MF2 | UUX_MF3)

#define	UUX_SEARCHNAME	"uux"
#define	UUX_VARPR	"NCMP"
#define	UUX_PR		"/usr/add-on/ncmp"
#define	UUX_LOGDNAME	"log"
#define	UUX_LOGFNAME	"sysdialer"

#ifndef	NOFILE
#define	NOFILE		20
#endif

#define	ARGBUFLEN	(MAXPATHLEN + 35)
#define	BUFLEN		((2 * MAXPATHLEN) + 3)
#define	PATHBUFLEN	((4 * MAXPATHLEN) + 3)

#define	MAXARGINDEX	100
#define	NARGPRESENT	(MAXARGINDEX/8 + 1)

#ifndef	VARPRAST
#define	VARPRAST	"AST"
#endif

#ifndef	VARPATH
#define	VARPATH		"PATH"
#endif

#define	SI		struct subifo


/* external subroutines */

extern "C" {
    extern int dialuux(cc *,cc *,cc *,cc **,cc *,cc *,int) noex ;
}


/* external variables */


/* local structures */

struct subinfo_flags {
	uint		ids:1 ;
	uint		noreport:1 ;
	uint		queueonly:1 ;
	uint		log:1 ;
} ;

struct subinfo_allocs {
	cchar	*node ;
	cchar	*svc ;
	cchar	*pr ;
} ;

struct subinfo {
	char		**args ;
	cchar	*pr ;
	cchar	*prn ;
	cchar	*searchname ;
	cchar	*hostname ;
	cchar	*svcname ;
	cchar	*node ;
	cchar	*logfname ;
	cchar	*username ;
	cchar	*grade ;
	cchar	**av ;
	cchar	**dav ;		/* allocated */
	UUX		*op ;
	SYSDIALER_ARGS	*ap ;
	struct subinfo_allocs	a ;
	struct subinfo_flags	f ;
	ids		id ;
} ;


/* forward references */

static int	subinfo_start(SI *,UUX *,SYSDIALER_ARGS *,
			cchar *,cchar *,cchar **) ;
static int	subinfo_procargs(SI *) ;
static int	subinfo_procspec(SI *,cchar *) ;
static int	subinfo_procopts(SI *,keyopt *) ;
static int	subinfo_defaults(SI *) ;
static int	subinfo_logfile(SI *) ;
static int	subinfo_mkargs(SI *,cchar **,cchar ***) ;
static int	subinfo_finish(SI *) ;
static int	subinfo_dirok(SI *,cchar *,int) ;


/* external variables (module information) */

SYSDIALER_INFO	uux_mod = {
	UUX_MNAME,
	UUX_VERSION,
	UUX_INAME,
	szof(UUX),
	UUX_MF
} ;


/* local variables */

enum argopts {
	argopt_root,
	argopt_rn,
	argopt_sn,
	argopt_lf,
	argopt_overlast
} ;

constexpr cpcchar	argopts[] = {
	"ROOT",
	"RN",
	"sn",
	"lf",
	nullptr
} ;

enum procopts {
	procopt_log,
	procopt_overlast
} ;

constexpr cpcchar	procopts[] = {
	"log",
	nullptr
} ;


/* exported variables */


/* exported subroutines */

int uux_open(UUX *op,SD_ARGS *ap,cc *hostname,cc *svcname,mainv av) noex {
	subinfo	si, *sip = &si ;
	int	rs ;
	int	opts = 0 ;

	if (op == nullptr) return SR_FAULT ;
	if (ap == nullptr) return SR_FAULT ;
	if (hostname == nullptr) return SR_FAULT ;

	memclear(op) ;

	if ((rs = subinfo_start(sip,op,ap,hostname,svcname,av)) >= 0) {

	if (ap != nullptr)
	    rs = subinfo_procargs(sip) ;

	if (rs >= 0)
	    rs = subinfo_defaults(sip) ;

	if (rs >= 0)
	    rs = subinfo_logfile(sip) ;

	if (rs >= 0) {
	    cchar	**dav = nullptr ;
	    if ((rs = subinfo_mkargs(sip,av,&dav)) >= 0) {
	        cchar	*pr = sip->pr ;
	        cchar	*nn = sip->node ;
	        cchar	*svc = sip->svcname ;
		cchar	*un = sip->username ;
		cchar	*grade = sip->grade ;

	    rs = dialuux(pr,nn,svc,dav,un,grade,opts) ;
	    op->fd = rs ;

	if (rs >= 0)
	    op->magic = UUX_MAGIC ;

	} /* end if (mkargs) */
	} /* end if (ok) */

	if ((rs < 0) && op->fl.log) {
	    op->fl.log = false ;
	    logfile_printf(&op->lh,"failed (%d)",rs) ;
	    logfile_close(&op->lh) ;
	} /* end if */

	subinfo_finish(sip) ;
	} /* end if (subinfo) */

	return (rs >= 0) ? op->fd : rs ;
}
/* end subroutine (uux_open) */


/* close the connection */
int uux_close(op)
UUX		*op ;
{
	int	rs = SR_OK ;
	int	rs1 ;


	if (op == nullptr)
	    return SR_FAULT ;

	if (op->magic != UUX_MAGIC)
	    return SR_NOTOPEN ;

	rs1 = u_close(op->fd) ;
	if (rs >= 0) rs = rs1 ;

	if (op->fl.log) {
	    op->fl.log = false ;
	    logfile_printf(&op->lh,"bytes=%u",op->tlen) ;
	    rs1 = logfile_close(&op->lh) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if */

	op->magic = 0 ;
	return rs ;
}
/* end subroutine (uux_close) */


int uux_reade(op,buf,buflen,to,opts)
UUX		*op ;
char		buf[] ;
int		buflen ;
int		to, opts ;
{
	int	rs ;


	if (op == nullptr)
	    return SR_FAULT ;

	if (op->magic != UUX_MAGIC)
	    return SR_NOTOPEN ;

#ifdef	COMMENT
	rs = uc_reade(op->fd,buf,buflen,to,opts) ;
#else
	rs = SR_BADF ;
#endif

	return rs ;
}
/* end subroutine (uux_reade) */


int uux_recve(op,buf,buflen,flags,to,opts)
UUX		*op ;
char		buf[] ;
int		buflen ;
int		flags ;
int		to, opts ;
{
	int	rs ;


	if (op == nullptr)
	    return SR_FAULT ;

	if (op->magic != UUX_MAGIC)
	    return SR_NOTOPEN ;

#ifdef	COMMENT
	rs = uc_recve(op->fd,buf,buflen,flags,to,opts) ;
#else
	rs = SR_BADF ;
#endif

	return rs ;
}
/* end subroutine (uux_recve) */


int uux_recvfrome(op,buf,buflen,flags,sap,salenp,to,opts)
UUX		*op ;
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

	if (op->magic != UUX_MAGIC)
	    return SR_NOTOPEN ;

#ifdef	COMMENT
	rs = uc_recvfrome(op->fd,buf,buflen,flags,sap,salenp,to,opts) ;
#else
	rs = SR_BADF ;
#endif

	return rs ;
}
/* end subroutine (uux_recvfrome) */


int uux_recvmsge(op,msgp,flags,to,opts)
UUX		*op ;
struct msghdr	*msgp ;
int		flags ;
int		to, opts ;
{
	int	rs ;


	if (op == nullptr)
	    return SR_FAULT ;

	if (op->magic != UUX_MAGIC)
	    return SR_NOTOPEN ;

#ifdef	COMMENT
	rs = uc_recvmsge(op->fd,msgp,flags,to,opts) ;
#else
	rs = SR_BADF ;
#endif

	return rs ;
}
/* end subroutine (uux_recvmsge) */


int uux_write(op,buf,buflen)
UUX		*op ;
cchar	buf[] ;
int		buflen ;
{
	int	rs ;


	if (op == nullptr)
	    return SR_FAULT ;

	if (op->magic != UUX_MAGIC)
	    return SR_NOTOPEN ;

	rs = uc_writen(op->fd,buf,buflen) ;

	if (rs >= 0)
	    op->tlen += rs ;

	return rs ;
}
/* end subroutine (uux_write) */


int uux_send(op,buf,buflen,flags)
UUX		*op ;
cchar	buf[] ;
int		buflen ;
int		flags ;
{
	int	rs ;


	if (op == nullptr)
	    return SR_FAULT ;

	if (op->magic != UUX_MAGIC)
	    return SR_NOTOPEN ;

	rs = u_send(op->fd,buf,buflen,flags) ;

	if (rs >= 0)
	    op->tlen += rs ;

	return rs ;
}
/* end subroutine (uux_send) */


int uux_sendto(op,buf,buflen,flags,sap,salen)
UUX		*op ;
cchar	buf[] ;
int		buflen ;
int		flags ;
void		*sap ;
int		salen ;
{
	int	rs ;


	if (op == nullptr)
	    return SR_FAULT ;

	if (op->magic != UUX_MAGIC)
	    return SR_NOTOPEN ;

	rs = u_sendto(op->fd,buf,buflen,flags,sap,salen) ;

	if (rs >= 0)
	    op->tlen += rs ;

	return rs ;
}
/* end subroutine (uux_sendto) */


int uux_sendmsg(op,msgp,flags)
UUX		*op ;
struct msghdr	*msgp ;
int		flags ;
{
	int	rs ;


	if (op == nullptr)
	    return SR_FAULT ;

	if (op->magic != UUX_MAGIC)
	    return SR_NOTOPEN ;

	rs = u_sendmsg(op->fd,msgp,flags) ;

	if (rs >= 0)
	    op->tlen += rs ;

	return rs ;
}
/* end subroutine (uux_sendmsg) */


int uux_shutdown(op,cmd)
UUX		*op ;
int		cmd ;
{
	int	rs ;


	if (op == nullptr)
	    return SR_FAULT ;

	if (op->magic != UUX_MAGIC)
	    return SR_NOTOPEN ;

	rs = u_shutdown(op->fd,cmd) ;

	return rs ;
}
/* end subroutine (uux_shutdown) */


/* private subroutines */


static int subinfo_start(sip,op,ap,hostname,svcname,av)
struct subinfo	*sip ;
UUX		*op ;
SYSDIALER_ARGS	*ap ;
cchar	hostname[] ;
cchar	svcname[] ;
cchar	*av[] ;
{
	int	rs = SR_FAULT ;
	if (sip) {
	    rs = memclear(sip) ;
	    sip->op = op ;
	    sip->ap = ap ;
	    sip->node = hostname ;
	    sip->hostname = hostname ;
	    sip->svcname = svcname ;
	    sip->av = av ;
	}
	return rs ;
}
/* end subroutine (subinfo_start) */


static int subinfo_finish(sip)
struct subinfo	*sip ;
{
	int	rs = SR_OK ;
	int	rs1 ;


	if (sip->dav != nullptr) {
	    rs1 = uc_free(sip->dav) ;
	    if (rs >= 0) rs = rs1 ;
	    sip->dav = nullptr ;
	}

	if (sip->fl.ids) {
	    sip->fl.ids = false ;
	    rs1 = ids_release(&sip->id) ;
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

	sip->op = nullptr ;
	sip->ap = nullptr ;
	return rs ;
}
/* end subroutine (subinfo_finish) */


static int subinfo_procargs(sip)
struct subinfo	*sip ;
{
	keyopt		akopts ;

	SYSDIALER_ARGS	*ap ;

	int	argr, argl, aol, akl, avl, kwi ;
	int	ai, ai_max, ai_pos ;
	int	argvalue = -1 ;
	int	argc ;
	int	pan ;
	int	rs ;
	int	f_optminus, f_optplus, f_optequal ;
	int	f ;

	char	**argv, *argp, *aop, *akp, *avp ;
	char	argpresent[NARGPRESENT] ;
	char	*cp ;


	ap = sip->ap ;
	argv = (char **) ap->argv ;

	for (argc = 0 ; argv[argc] != nullptr ; argc += 1) ;

	rs = keyopt_start(&akopts) ;
	if (rs < 0)
	    goto ret0 ;

/* process program arguments */

	for (ai = 0 ; ai < NARGPRESENT ; ai += 1)
	    argpresent[ai] = 0 ;

	ai = 0 ;
	ai_max = 0 ;
	ai_pos = 0 ;
	argr = argc - 1 ;
	while ((rs >= 0) && (argr > 0)) {

	    argp = argv[++ai] ;
	    argr -= 1 ;
	    argl = lenstr(argp) ;

	    f_optminus = (*argp == '-') ;
	    f_optplus = (*argp == '+') ;
	    if ((argl > 1) && (f_optminus || f_optplus)) {
		const int	ach = MKCHAR(argp[1]) ;

	        if (isdigitlatin(ach)) {

	            rs = cfdeci((argp + 1),(argl - 1),&argvalue) ;

	        } else if (ach == '-') {

	            ai_pos = ai ;
	            break ;

	        } else {

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

/* keyword match or only key letters? */

	            if ((kwi = matostr(argopts,2,akp,akl)) >= 0) {

	                switch (kwi) {

/* program-root */
	                case argopt_root:
	                    if (f_optequal) {

	                        f_optequal = false ;
	                        if (avl)
	                            sip->pr = avp ;

	                    } else {

	                        if (argr <= 0) {
	                            rs = SR_INVALID ;
	                            break ;
	                        }

	                        argp = argv[++ai] ;
	                        argr -= 1 ;
	                        argl = lenstr(argp) ;

	                        if (argl)
	                            sip->pr = argp ;

	                    }

	                    break ;

/* program-root-name */
	                case argopt_rn:
	                    if (f_optequal) {

	                        f_optequal = false ;
	                        if (avl)
	                            sip->prn = avp ;

	                    } else {

	                        if (argr <= 0) {
	                            rs = SR_INVALID ;
	                            break ;
	                        }

	                        argp = argv[++ai] ;
	                        argr -= 1 ;
	                        argl = lenstr(argp) ;

	                        if (argl)
	                            sip->prn = argp ;

	                    }

	                    break ;

/* search-name root */
	                case argopt_sn:
	                    if (f_optequal) {

	                        f_optequal = false ;
	                        if (avl)
	                            sip->searchname = avp ;

	                    } else {

	                        if (argv[ai + 1] == nullptr) {
	                            rs = SR_INVALID ;
	                            break ;
	                        }

	                        argp = argv[++ai] ;
	                        argr -= 1 ;
	                        argl = lenstr(argp) ;

	                        if (argl)
	                            sip->searchname = argp ;

	                    }

	                    break ;

/* logfile */
	                case argopt_lf:
			    sip->fl.log = true ;
	                    if (f_optequal) {

	                        f_optequal = false ;
	                        if (avl)
	                            sip->logfname = avp ;

	                    } else {

	                        if (argr <= 0) {
	                            rs = SR_INVALID ;
	                            break ;
	                        }

	                        argp = argv[++ai] ;
	                        argr -= 1 ;
	                        argl = lenstr(argp) ;

	                        if (argl)
	                            sip->logfname = argp ;

	                    }

	                    break ;

/* handle all keyword defaults */
	                default:
	                    rs = SR_INVALID ;

	                } /* end switch */

	            } else {

	                while (akl--) {

	                    switch ((int) *akp) {

/* program-root */
	                    case 'R':
	                        if (argr <= 0) {
	                            rs = SR_INVALID ;
	                            break ;
	                        }

	                        argp = argv[++ai] ;
	                        argr -= 1 ;
	                        argl = lenstr(argp) ;

	                        if (argl)
	                            sip->pr = argp ;

	                        break ;

/* options */
	                    case 'o':
	                        if (argr <= 0) {
	                            rs = SR_INVALID ;
	                            break ;
	                        }

	                        argp = argv[++ai] ;
	                        argr -= 1 ;
	                        argl = lenstr(argp) ;

	                        if (argl)
	                            rs = keyopt_loads(&akopts,argp,argl) ;

	                        break ;

/* username */
	                    case 'u':
	                        if (argr <= 0) {
	                            rs = SR_INVALID ;
	                            break ;
	                        }

	                        argp = argv[++ai] ;
	                        argr -= 1 ;
	                        argl = lenstr(argp) ;

	                        if (argl)
	                            sip->username = argp ;

	                        break ;

/* grade */
	                    case 'g':
	                        if (argr <= 0) {
	                            rs = SR_INVALID ;
	                            break ;
	                        }

	                        argp = argv[++ai] ;
	                        argr -= 1 ;
	                        argl = lenstr(argp) ;

	                        if (argl)
	                            sip->username = argp ;

	                        break ;

	                    case 'r':
	                        sip->fl.queueonly = true ;
	                        break ;

	                    case 'n':
	                        sip->fl.noreport = true ;
	                        break ;

	                    default:
	                        rs = SR_INVALID ;

	                    } /* end switch */

	                    akp += 1 ;
	                    if (rs < 0)
	                        break ;

	                } /* end while */

	            } /* end if (individual option key letters) */

	        } /* end if (digits as argument or not) */

	    } else {

	        if (ai >= MAXARGINDEX)
	            break ;

	        BASET(argpresent,ai) ;
	        ai_max = ai ;

	    } /* end if (key letter/word or positional) */

	    ai_pos = ai ;

	} /* end while (all command line argument processing) */

	if (rs < 0)
	    goto badarg ;

	pan = 0 ;

	for (ai = 1 ; ai < argc ; ai += 1) {

	    f = (ai <= ai_max) && BATST(argpresent,ai) ;
	    f = f || (ai > ai_pos) ;
	    if (! f) continue ;

	    cp = argv[ai] ;
	    switch (pan++) {
	    case 0:
	        rs = subinfo_procspec(sip,cp) ;
	        break ;
	    case 1:
	        sip->args = (argv + ai) ;
	        break ;
	    default:
	        break ;
	    } /* end switch */

	    if (rs < 0) break ;
	} /* end for */

	if (rs >= 0)
	    rs = subinfo_procopts(sip,&akopts) ;

ret1:
	keyopt_finish(&akopts) ;

ret0:
	return rs ;

/* bad stuff */
badarg:
	goto ret1 ;
}
/* end subroutine (subinfo_procargs) */

static int subinfo_procopts(SI *sip,keyopt *kop) noex {
	keyopt_cur	kcur ;
	int		rs ;
	int		c = 0 ;
	if ((rs = keyopt_curbegin(kop,&kcur)) >= 0) {
	    int		oi ;
	    int		kl, vl ;
	    cchar	*kp, *vp ;

	while ((kl = keyopt_enumkeys(kop,&kcur,&kp)) >= 0) {

	    vl = keyopt_fetch(kop,kp,nullptr,&vp) ;

	    if ((oi = matostr(procopts,2,kp,kl)) >= 0) {

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


static int subinfo_procspec(sip,spec)
struct subinfo	*sip ;
cchar	spec[] ;
{
	int	rs = SR_OK ;
	int	cl ;

	cchar	*tp ;
	cchar	*cp ;


	if (spec[0] == '\0')
	    goto ret0 ;

	if ((tp = strchr(spec,'!')) != nullptr) {

	    cp = spec ;
	    cl = (tp - spec) ;
	    if (cl > 0) {

	        rs = uc_mallocstrw(cp,cl,&sip->a.node) ;

	        if (rs >= 0)
	            sip->node = sip->a.node ;

	    } /* end if */

	    cp = (tp + 1) ;
	    if (cp[0] != '\0')
		sip->svcname = cp ;

	} else
	    sip->svcname = spec ;

ret0:
	return rs ;
}
/* end subroutine (subinfo_procspec) */


static int subinfo_defaults(sip)
struct subinfo	*sip ;
{
	SYSDIALER_ARGS	*ap ;

	int	rs = SR_OK ;
	int	rs1 ;

	cchar	*vp ;


	ap = sip->ap ;

/* program-root */

	if ((sip->pr == nullptr) && (sip->prn != nullptr) && (sip->prn[0] != '\0')) {
	    char	domainname[MAXHOSTNAMELEN + 1] ;
	    char	pr[MAXPATHLEN + 1] ;

	    rs1 = getnodedomain(nullptr,domainname) ;

	    if (rs1 >= 0)
	        rs1 = mkpr(pr,MAXPATHLEN,sip->prn,domainname) ;

	    if (rs1 >= 0) {

	        rs = subinfo_dirok(sip,pr,rs1) ;

	        if (rs > 0)
	            rs = uc_mallocstrw(pr,rs1,&sip->a.pr) ;

	        if (rs > 0)
	            sip->pr = sip->a.pr ;

	    } /* end if */

	} /* end if */

	if ((rs >= 0) && (sip->pr == nullptr)) {

	    if ((vp = getenv(UUX_VARPR)) != nullptr) {

	        rs = subinfo_dirok(sip,vp,-1) ;
	        if (rs > 0)
	            sip->pr = vp ;

	    }

	} /* end if */

	if ((rs >= 0) && (sip->pr == nullptr)) {

	    vp = UUX_PR ;
	    rs = subinfo_dirok(sip,vp,-1) ;
	    if (rs > 0)
	        sip->pr = vp ;

	} /* end if */

	if (sip->pr == nullptr)
	    sip->pr = ap->pr ;

/* search-name */

	if (sip->searchname == nullptr)
	    sip->searchname = UUX_SEARCHNAME ;

/* log-file */

	if ((rs >= 0) && (sip->logfname == nullptr))
		sip->logfname = UUX_LOGFNAME ;

/* out of here */

	return rs ;
}
/* end subroutine (subinfo_defaults) */


static int subinfo_logfile(sip)
struct subinfo	*sip ;
{
	UUX	*op = sip->op ;

	int	rs = SR_OK ;
	int	rs1 ;

	cchar	*lnp ;
	cchar	*lidp = nullptr ;

	char	logfname[MAXPATHLEN + 1] ;


	if (! sip->fl.log)
	    goto ret0 ;

	sip->fl.log = true ;
	lnp = sip->logfname ;
	if (lnp[0] != '/') {
	    rs = mkpath3(logfname,sip->pr,UUX_LOGDNAME,lnp) ;
	    lnp = logfname ;
	}

	if (rs < 0)
	    goto ret0 ;

	    if ((rs1 = logfile_open(&op->lh,lnp,0,0666,lidp)) >= 0) {
	        USERINFO	u, *uip = &u ;
	        char	userbuf[USERINFO_LEN + 1] ;
	        op->fl.log = true ;

	        if ((rs1 = userinfo(uip,userbuf,USERINFO_LEN,nullptr)) >= 0) {

	            logfile_userinfo(&op->lh,uip,0L,
	                UUX_MNAME,UUX_VERSION) ;

	            logfile_printf(&op->lh,"pid=%d",uip->pid) ;

	        } else {

	            if ((rs1 = getusername(logfname,MAXPATHLEN,-1)) >= 0) {
	        	pid_t	pid = getpid() ;

	                logfile_printf(&op->lh,"username=%s",
	                    logfname) ;

	                logfile_printf(&op->lh,"pid=%d",pid) ;

	            }

	        } /* end if (userinfo) */

	        logfile_printf(&op->lh,"pr=%s",
	            sip->pr) ;

	        logfile_printf(&op->lh,"host=%s",
	            sip->hostname) ;

	        logfile_printf(&op->lh,"node=%s",
	            sip->node) ;

	        logfile_printf(&op->lh,"svc=%s",
	            sip->svcname) ;

	    } /* end if (opened logfile) */

ret0:
	return rs ;
}
/* end subroutine (subinfo_logfile) */


static int subinfo_mkargs(sip,av,davp)
struct subinfo	*sip ;
cchar	*av[] ;
cchar	***davp ;
{
	int	rs = SR_OK ;
	int	i ;
	int	size ;
	int	n = 0 ;
	cchar	**dav ;


	*davp = nullptr ;
	if (sip->args != nullptr) {
	    for (i = 0 ; sip->args[i] != nullptr ; i += 1)
	        n += 1 ;
	}

	if (av != nullptr) {
	    for (i = 0 ; av[i] != nullptr ; i += 1)
	        n += 1 ;
	}

	size = (n + 1) * szof(cchar *) ;
	if ((rs = uc_malloc(size,&dav)) >= 0) {
	    int	j = 0 ;
	    sip->dav = dav ;
	    *davp = dav ;

	    if (sip->args != nullptr) {
	        for (i = 0 ; sip->args[i] != nullptr ; i += 1)
	            dav[j++] = sip->args[i] ;
	    }

	    if (av != nullptr) {
	        for (i = 0 ; av[i] != nullptr ; i += 1)
	            dav[j++] = (char *) av[i] ;
	    }

	    dav[j] = nullptr ;

	} /* end if */

	return (rs >= 0) ? n : rs ;
}
/* end subroutine (subinfo_mkargs) */


static int subinfo_dirok(sip,d,dlen)
struct subinfo	*sip ;
cchar	d[] ;
int		dlen ;
{
	USTAT	sb ;

	NULSTR	ss ;

	int	rs = SR_OK ;
	int	rs1 ;
	int	f = false ;

	cchar	*dnp ;


	if (! sip->fl.ids) {
	    sip->fl.ids = true ;
	    rs = ids_load(&sip->id) ;
	}

	if (rs >= 0)
	    rs = nulstr_start(&ss,d,dlen,&dnp) ;

	if (rs < 0)
	    goto ret0 ;

	rs1 = u_stat(dnp,&sb) ;

	if ((rs1 >= 0) && S_ISDIR(sb.st_mode)) {
	    rs1 = permid(&sip->id,&sb,(R_OK | X_OK)) ;
	    f = (rs1 >= 0) ;
	} /* end if */

	nulstr_finish(&ss) ;

ret0:
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (subinfo_dirok) */



