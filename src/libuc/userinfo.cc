/* userinfo SUPPORT */
/* lang=C++20 */

/* get user information from various databases */
/* version %I% last-modified %G% */

#define	CF_DEBUGS	0		/* compile-time debug print-outs */
#define	CF_UINFO	1		/* include 'uinfo(3uc)' */
#define	CF_UNAME	0		/* include 'u_uname(3u)' */
#define	CF_OLDUSERINFO	1		/* compile-in old 'userinfo(3dam)' */
#define	CF_UGETPW	1		/* use |ugetpw(3uc)| */

/* revision history:

	= 1999-07-01, David A­D­ Morano
	This subroutine was originally written.

	= 2005-03-30, David A­D­ Morano
	I added code to look at the LOCALDOMAIN environment variable
	in this module.  Yes, yes, it would have been grabbed from
	the |getnodedomain()| subroutine, but the order was not
	what I wanted.  I want LOCALDOMAIN to have precedence over
	the USERATTR domian (if there is one).  Users already could
	have changed their local domain using the DOMAIN environment
	variable, but many older applications (very old now) might
	have still used the LOCALDOMAIN environment variable only
	(instead of DOMAIN).

	= 2008-08-12, David A­D­ Morano
	I replaced calls to |getusernam(3secdb)| and |udomain(3dam)|
	by calls to the single object 'userattr(3dam)'.  Since the
	old UDOMAIN database is now (quietly) queried by the
	|userattr(3dam)| object if a lookup on the system user-attribute
	database fails for the 'id' keyword, any code here that
	used to call |udomain(3dam)| was just eliminated (deleted).

*/

/* Copyright © 1999,2005,2008 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	userinfo

	Description:
	Get user information from PASSWD database.

	Synopsis:
	int userinfo_start(USERINFO *uip,cchar *username) noex

	Arguments:
	uip		address of 'userinfo' structure
	username	optional username

	Returns:
	>=0		success
	<0		failure (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/utsname.h>
#include	<unistd.h>
#include	<stdlib.h>
#include	<strings.h>		/* for |strcasecmp(3c)| */
#include	<pwd.h>
#include	<grp.h>
#include	<netdb.h>
#include	<user_attr.h>
#include	<usystem.h>
#include	<ugetpid.h>
#include	<getbufsize.h>
#include	<char.h>
#include	<bits.h>
#include	<strstore.h>
#include	<getax.h>
#include	<ugetpw.h>
#include	<getxusername.h>
#include	<gecos.h>
#include	<userattr.h>
#include	<uinfo.h>
#include	<storeitem.h>
#include	<passwdent.h>
#include	<localmisc.h>

#include	"userinfo.h"


/* local defines */

#undef	USERINFO_SYSV
#define	USERINFO_SYSV defined(SYSV)||(defined(OSTYPE_SYSV)&&(OSTYPE_SYSV>0))\
	    ||defined(OSNAME_SunOS)&&(OSNAME_SunOS==1)

#if	CF_UGETPW
#define	GETPW_NAME	ugetpw_name
#else
#define	GETPW_NAME	getpw_name
#endif /* CF_UGETPW */

#define	PROCINFO	struct procinfo
#define	PROCINFO_FL	struct procinfo_flags
#define	PROCINFO_TMPS	struct procinfo_tmps

#define	PRNAME		"LOCAL"		/* program-root for USERATTR */
#define	PRBUFLEN	MAXPATHLEN

#ifndef	LOGIDLEN
#define	LOGIDLEN	15		/* log ID length */
#endif

#ifndef	PROJNAMELEN
#define	PROJNAMELEN	32		/* is this enough? */
#endif

#ifndef	REALNAMELEN
#define	REALNAMELEN	100		/* real-name length */
#endif

#ifndef	WSLEN
#define	WSLEN		15		/* weather-station (abbr) length */
#endif

#ifndef	DIGBUFLEN
#define	DIGBUFLEN	40
#endif

#define	DEFHOMEDNAME	"/tmp"
#define	SBINDNAME	"/usr/sbin"

#ifndef	ETCDNAME
#define	ETCDNAME	"/etc"
#endif

#ifndef	TMPDNAME
#define	TMPDNAME	"/tmp"
#endif

#ifndef	ORGCNAME
#define	ORGCNAME	"organization"
#endif

#ifndef	VARNODE
#define	VARNODE		"NODE"
#endif

#ifndef	VARDOMAIN
#define	VARDOMAIN	"DOMAIN"
#endif

#ifndef	VARLOCALDOMAIN
#define	VARLOCALDOMAIN	"LOCALDOMAIN"
#endif

#ifndef	VARHOME
#define	VARHOME		"HOME"
#endif

#ifndef	VARSHELL
#define	VARSHELL	"SHELL"
#endif

#ifndef	VARORGANIZATION
#define	VARORGANIZATION	"ORGANIZATION"
#endif

#ifndef	VARBIN
#define	VARBIN		"PRINTERBIN"
#endif

#ifndef	VAROFFICE
#define	VAROFFICE	"OFFICE"
#endif

#ifndef	VARPRINTER
#define	VARPRINTER	"PRINTER"
#endif

#ifndef	VARNAME
#define	VARNAME		"NAME"
#endif

#ifndef	VARMAILNAME
#define	VARMAILNAME	"MAILNAME"
#endif

#ifndef	VARFULLNAME
#define	VARFULLNAME	"FULLNAME"
#endif

#ifndef	VARTZ
#define	VARTZ		"TZ"
#endif

#ifndef	VARMD
#define	VARMD		"MAILDIR"
#endif

#ifndef	VARWSTATION
#define	VARWSTATION	"WSTATION"
#endif

#ifndef	VARTMPDNAME
#define	VARTMPDNAME	"TMPDIR"
#endif

#ifndef	VAROSTYPE
#define	VAROSTYPE	"OSTYPE"
#endif

#ifndef	VAROSNAME
#define	VAROSNAME	"OSNAME"
#endif

#ifndef	VAROSNUM
#define	VAROSNUM	"OSNUM"
#endif

#ifndef	UA_DN
#define	UA_DN		"dn"		/* INET domain-name */
#endif

#ifndef	UA_PROJECT
#define	UA_PROJECT	"project"	/* UNIX® project */
#endif

#ifndef	UA_TZ
#define	UA_TZ		"tz"		/* time-zone specification */
#endif

#ifndef	UA_MD
#define	UA_MD		"md"		/* mail-spool directory */
#endif

#ifndef	UA_WS
#define	UA_WS		"ws"		/* weather station */
#endif

#define	NDF		"/tmp/userinfo.nd"


/* external subroutines */

extern int	sncpylc(char *,int,cchar *) ;
extern int	sncpyuc(char *,int,cchar *) ;
extern int	sncpy1w(char *,int,cchar *,int) ;
extern int	sncpy1(char *,int,cchar *) ;
extern int	sncpy2(char *,int,cchar *,cchar *) ;
extern int	snwcpyhyphen(char *,int,cchar *,int) ;
extern int	mkpath2(char *,cchar *,cchar *) ;
extern int	mkgecosname(char *,int,cchar *) ;
extern int	mkrealname(char *,int,cchar *,int) ;
extern int	mkmailname(char *,int,cchar *,int) ;
extern int	mklogid(char *,int,cchar *,int,int) ;
extern int	cfdeci(cchar *,int,int *) ;
extern int	ctdecui(char *,int,uint) ;
extern int	readfileline(char *,int,cchar *) ;
extern int	getprojname(char *,int,cchar *) ;
extern int	getnodename(char *,int) ;
extern int	getdomainname(char *,int,cchar *) ;
extern int	getnodedomain(char *,char *) ;
extern int	mkpr(char *,int,cchar *,cchar *) ;
extern int	isNotPresent(const int) ;

#if	CF_DEBUGS
extern int	debugprintf(cchar *,...) ;
extern int	strlinelen(cchar *,int,int) ;
#endif

extern char	*strwcpy(char *,cchar *,int) ;
extern char	*strnchr(cchar *,int,int) ;
extern char	*strdcpy1(char *,int,cchar *) ;


/* external variables */


/* local structures */

enum uits {
	uit_sysname,
	uit_release,
	uit_version,
	uit_machine,
	uit_nodename,
	uit_domainname,
	uit_username,
	uit_password,
	uit_gecos,
	uit_homedname,
	uit_shell,
	uit_organization,
	uit_gecosname,
	uit_account,
	uit_bin,
	uit_office,
	uit_wphone,
	uit_hphone,
	uit_printer,
	uit_realname,
	uit_mailname,
	uit_name,
	uit_fullname,
	uit_groupname,
	uit_project,
	uit_tz,
	uit_md,
	uit_wstation,
	uit_logid,
	uit_overlast
} ;

struct procinfo_flags {
	uint		altuser:1 ;
	uint		pw:1 ;
	uint		ua:1 ;
	uint		uainit:1 ;
	uint		allocua:1 ;
} ;

struct procinfo_tmps {
	cchar	*gecosname ;
	cchar	*realname ;
} ;

struct procinfo {
	PROCINFO_FL	f ;
	PROCINFO_TMPS	tstrs ;
	PASSWD	pw ;
	BITS		have ;
	UINFO_NAME	unixinfo ;
	USERINFO	*uip ;		/* supplied argument */
	STRSTORE	*stp ;		/* supplied argument */
	int		*sis ;		/* supplied argument */
	cchar	*username ;	/* supplied argument */
	cchar	*a ;		/* the memory allocation */
	USERATTR	*uap ;
	cchar	*pwbuf ;	/* specially allocated */
	char		*nodename ;	/* allocated */
	char		*domainname ;	/* allocated */
	char		*pr ;		/* allocated */
	char		*tbuf ;
	int		pwlen ;
	int		tlen ;
} ;


/* forward references */

static int	userinfo_process(USERINFO *,STRSTORE *,int *,cchar *) ;
static int	userinfo_id(USERINFO *uip) ;
static int	userinfo_load(USERINFO *,STRSTORE *,int *) ;

#ifdef	COMMENT
static int	userinfo_setnuls(USERINFO *,cchar *) ;
#endif

static int	procinfo_start(PROCINFO *,USERINFO *,STRSTORE *,int *) ;
static int	procinfo_finish(PROCINFO *) ;
static int 	procinfo_find(PROCINFO *,cchar *) ;
static int	procinfo_pwentry(PROCINFO *,cchar *) ;
static int	procinfo_getpwuser(PROCINFO *,PASSWD *,char *,int,
			cchar *) ;
static int	procinfo_store(PROCINFO *,int,cchar *,int,cchar **) ;
static int	procinfo_uabegin(PROCINFO *) ;
static int	procinfo_uaend(PROCINFO *) ;
static int	procinfo_ualookup(PROCINFO *,char *,int,cchar *) ;

/* components */
static int	procinfo_homedname(PROCINFO *) ;
static int	procinfo_shell(PROCINFO *) ;
static int	procinfo_org(PROCINFO *) ;
static int	procinfo_bin(PROCINFO *) ;
static int	procinfo_office(PROCINFO *) ;
static int	procinfo_printer(PROCINFO *) ;
static int	procinfo_gecos(PROCINFO *) ;
static int	procinfo_gecosname(PROCINFO *) ;
static int	procinfo_realname(PROCINFO *) ;
static int	procinfo_mailname(PROCINFO *) ;
static int	procinfo_name(PROCINFO *) ;
static int	procinfo_fullname(PROCINFO *) ;
#if	CF_UINFO
static int	procinfo_uinfo(PROCINFO *) ;
#endif
#if	CF_UNAME
static int	procinfo_uname(PROCINFO *) ;
#endif
static int	procinfo_nodename(PROCINFO *) ;
static int	procinfo_domainname(PROCINFO *) ;
static int	procinfo_project(PROCINFO *) ;
static int	procinfo_tz(PROCINFO *) ;
static int	procinfo_md(PROCINFO *) ;
static int	procinfo_wstation(PROCINFO *) ;
static int	procinfo_logid(PROCINFO *) ;

#ifdef	COMMENT
static int	checknul(cchar *,cchar **) ;
static int	empty(cchar *) ;
#endif

#if	USERINFO_SYSV
#else
static int	getostype() ;
#endif


/* local variables */

/* order here is generally (quite) important */
static int	(*components[])(PROCINFO *) = {
	procinfo_homedname,
	procinfo_shell,
	procinfo_org,
	procinfo_bin,
	procinfo_office,
	procinfo_printer,
	procinfo_gecos,
	procinfo_gecosname,
	procinfo_realname,
	procinfo_mailname,
	procinfo_name,
	procinfo_fullname,
#if	CF_UINFO
	procinfo_uinfo,
#endif
#if	CF_UNAME
	procinfo_uname,
#endif
	procinfo_nodename,
	procinfo_domainname,
	procinfo_project,
	procinfo_tz,
	procinfo_md,
	procinfo_wstation,
	procinfo_logid,
	nullptr
} ;


/* exported subroutines */


int userinfo_start(USERINFO *uip,cchar *username)
{
	const int	startsize = (USERINFO_LEN/4) ;
	int		rs ;
	int		rs1 ;
	int		len = 0 ;

	if (uip == nullptr) return SR_FAULT ;

	memset(uip,0,sizeof(USERINFO)) ;
	uip->nodename = getenv(VARNODE) ;

	if ((rs = userinfo_id(uip)) >= 0) {
	    const int	size = (uit_overlast * sizeof(cchar *)) ;
	    void	*p ;
	    if ((rs = uc_malloc(size,&p)) >= 0) {
	        STRSTORE	st ;
	        int		*sis = (int *) p ;

	        memset(p,0,size) ;
	        if ((rs = strstore_start(&st,10,startsize)) >= 0) {

	            if ((rs = userinfo_process(uip,&st,sis,username)) >= 0) {
	                rs = userinfo_load(uip,&st,sis) ;
	                len = rs ;
	            } /* end if */

	            rs1 = strstore_finish(&st) ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (strstore) */

	        rs1 = uc_free(p) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (memory_allocation) */
	    if (rs >= 0) uip->magic = USERINFO_MAGIC ;
	} /* end if (userinfo_id) */

#if	CF_DEBUGS
	debugprintf("userinfo_start: ret rs=%d len=%u\n",rs,len) ;
#endif

	return (rs >= 0) ? len : rs ;
}
/* end subroutine (userinfo_start) */


int userinfo_finish(USERINFO *uip)
{
	int		rs = SR_OK ;
	int		rs1 ;

#if	CF_DEBUGS
	debugprintf("userinfo_finish: ent\n") ;
#endif

	if (uip == nullptr) return SR_FAULT ;

	if (uip->magic != USERINFO_MAGIC) return SR_NOTOPEN ;

	if (uip->a != nullptr) {
	    rs1 = uc_free(uip->a) ;
	    uip->a = nullptr ;
	    if (rs >= 0) rs = rs1 ;
	}

#if	CF_DEBUGS
	debugprintf("userinfo_finish: ret rs=%d\n",rs) ;
#endif

	uip->magic = 0 ;
	return rs ;
}
/* end subroutine (userinfo_finish) */


/* local subroutines */


static int userinfo_process(USERINFO *uip,STRSTORE *stp,int *sis,cchar *un)
{
	PROCINFO	pi ;
	int		rs ;
	int		rs1 ;

	if ((rs = procinfo_start(&pi,uip,stp,sis)) >= 0) {
	    {
	        rs = procinfo_find(&pi,un) ;
	    }
	    rs1 = procinfo_finish(&pi) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (procinfo) */

#if	CF_DEBUGS
	debugprintf("userinfo_process: ret rs=%d\n",rs) ;
#endif

	return rs ;
}
/* end subroutine (userinfo_process) */


static int userinfo_id(USERINFO *uip)
{

	uip->pid = ugetpid() ;

#if	USERINFO_SYSV
	uip->f.sysv_ct = TRUE ;
	uip->f.sysv_rt = TRUE ;
#else
	uip->f.sysv_ct = FALSE ;
	uip->f.sysv_rt = getostype() ;
#endif

	uip->uid = getuid() ;
	uip->euid = geteuid() ;

	uip->gid = getgid() ;
	uip->egid = getegid() ;

	return SR_OK ;
}
/* end subroutine (userinfo_id) */


static int userinfo_load(USERINFO *uip,STRSTORE *stp,int *sis)
{
	int		rs ;
	int		size = 0 ;

#if	CF_DEBUGS
	debugprintf("userinfo_load: ent\n") ;
#endif

	if ((rs = strstore_strsize(stp)) >= 0) {
	    char	*a ;
	    size = rs ;
#if	CF_DEBUGS
	    debugprintf("userinfo_load: size=%u\n",size) ;
#endif
	    if ((rs = uc_malloc(size,&a)) >= 0) {
	        if ((rs = strstore_strmk(stp,a,size)) >= 0) {
	            int		i ;
	            cchar	**vpp ;

#if	CF_DEBUGS
	            debugprintf("userinfo_load: strmk() rs=%d\n",rs) ;
#endif
	            for (i = 0 ; i < uit_overlast ; i += 1) {
	                switch (i) {
	                case uit_machine:
	                    vpp = &uip->machine ;
	                    break ;
	                case uit_sysname:
	                    vpp = &uip->sysname ;
	                    break ;
	                case uit_release:
	                    vpp = &uip->release ;
	                    break ;
	                case uit_version:
	                    vpp = &uip->version ;
	                    break ;
	                case uit_nodename:
	                    vpp = &uip->nodename ;
	                    break ;
	                case uit_domainname:
	                    vpp = &uip->domainname ;
	                    break ;
	                case uit_username:
	                    vpp = &uip->username ;
	                    break ;
	                case uit_password:
	                    vpp = &uip->password ;
	                    break ;
	                case uit_gecos:
	                    vpp = &uip->gecos ;
	                    break ;
	                case uit_homedname:
	                    vpp = &uip->homedname ;
	                    break ;
	                case uit_shell:
	                    vpp = &uip->shell ;
	                    break ;
	                case uit_organization:
	                    vpp = &uip->organization ;
	                    break ;
	                case uit_gecosname:
	                    vpp = &uip->gecosname ;
	                    break ;
	                case uit_account:
	                    vpp = &uip->account ;
	                    break ;
	                case uit_bin:
	                    vpp = &uip->bin ;
	                    break ;
	                case uit_office:
	                    vpp = &uip->office ;
	                    break ;
	                case uit_wphone:
	                    vpp = &uip->wphone ;
	                    break ;
	                case uit_hphone:
	                    vpp = &uip->hphone ;
	                    break ;
	                case uit_printer:
	                    vpp = &uip->printer ;
	                    break ;
	                case uit_realname:
	                    vpp = &uip->realname ;
	                    break ;
	                case uit_mailname:
	                    vpp = &uip->mailname ;
	                    break ;
	                case uit_name:
	                    vpp = &uip->name ;
	                    break ;
	                case uit_fullname:
	                    vpp = &uip->fullname ;
	                    break ;
	                case uit_groupname:
	                    vpp = &uip->groupname ;
	                    break ;
	                case uit_project:
	                    vpp = &uip->project ;
	                    break ;
	                case uit_tz:
#if	CF_DEBUGS
	                    debugprintf("userinfo_load: uip->tz=%s\n",uip->tz) ;
#endif
	                    vpp = &uip->tz ;
	                    break ;
	                case uit_md:
	                    vpp = &uip->md ;
	                    break ;
	                case uit_wstation:
	                    vpp = &uip->wstation ;
	                    break ;
	                case uit_logid:
	                    vpp = &uip->logid ;
	                    break ;
	                default:
	                    rs = SR_BADFMT ;
	                    break ;
	                } /* end switch */
	                if ((rs >= 0) && (*vpp == nullptr)) *vpp = (a + sis[i]) ;
	            } /* end for */

	        } /* end if */
	        if (rs >= 0) {
	            uip->a = a ;
	        } else 
	            uc_free(a) ;
	    } /* end if (memory-allocation) */
	} /* end if */

#if	CF_DEBUGS
	debugprintf("userinfo_load: ret rs=%d size=%u\n",rs,size) ;
#endif

	return (rs >= 0) ? size : rs ;
}
/* end subroutine (userinfo_load) */


#ifdef	COMMENT
static int userinfo_setnuls(uep,emptyp)
USERINFO	*uep ;
cchar	*emptyp ;
{

	checknul(emptyp,&uep->sysname) ;
	checknul(emptyp,&uep->release) ;
	checknul(emptyp,&uep->version) ;
	checknul(emptyp,&uep->machine) ;
	checknul(emptyp,&uep->nodename) ;
	checknul(emptyp,&uep->domainname) ;
	checknul(emptyp,&uep->username) ;
	checknul(emptyp,&uep->password) ;
	checknul(emptyp,&uep->gecos) ;
	checknul(emptyp,&uep->homedname) ;
	checknul(emptyp,&uep->shell) ;
	checknul(emptyp,&uep->organization) ;
	checknul(emptyp,&uep->gecosname) ;
	checknul(emptyp,&uep->account) ;
	checknul(emptyp,&uep->bin) ;
	checknul(emptyp,&uep->office) ;
	checknul(emptyp,&uep->wphone) ;
	checknul(emptyp,&uep->hphone) ;
	checknul(emptyp,&uep->printer) ;
	checknul(emptyp,&uep->realname) ;
	checknul(emptyp,&uep->mailname) ;
	checknul(emptyp,&uep->fullname) ;
	checknul(emptyp,&uep->name) ;
	checknul(emptyp,&uep->groupname) ;
	checknul(emptyp,&uep->logid) ;
	checknul(emptyp,&uep->project) ;
	checknul(emptyp,&uep->tz) ;
	checknul(emptyp,&uep->wstation) ;

	return 0 ;
}
/* end subroutine (userinfo_setnuls) */
#endif /* COMMENT */


static int procinfo_start(PROCINFO *pip,USERINFO *uip,STRSTORE *stp,int *sis)
{
	const int	pwlen = getbufsize(getbufsize_pw) ;
	int		rs ;
	int		size ;
	char		*bp ;

	memset(pip,0,sizeof(PROCINFO)) ;
	pip->uip = uip ;
	pip->stp = stp ;
	pip->sis = sis ;
	pip->tlen = MAX(pwlen,MAXPATHLEN) ;

	size = 0 ;
	size += sizeof(USERATTR) ;
	size += NODENAMELEN ;
	size += MAXHOSTNAMELEN ;
	size += MAXPATHLEN ;
	size += pip->tlen ;
	if ((rs = uc_malloc(size,&bp)) >= 0) {
	    int	bl = 0 ;
	    pip->a = bp ;
	    pip->uap = (USERATTR *) (bp+bl) ;
	    bl += sizeof(USERATTR) ;
	    pip->nodename = (bp+bl) ;
	    bl += NODENAMELEN ;
	    pip->domainname = (bp+bl) ;
	    bl += MAXHOSTNAMELEN ;
	    pip->pr = (bp+bl) ;
	    bl += MAXPATHLEN ;
	    pip->tbuf = (bp+bl) ;
	    bl += pip->tlen ;
	    if ((rs = bits_start(&pip->have,uit_overlast)) >= 0) {
	        pip->nodename[0] = '\0' ;
	        pip->domainname[0] = '\0' ;
	        pip->pr[0] = '\0' ;
	        pip->tbuf[0] = '\0' ;
	    } else {
	        uc_free(bp) ;
	        pip->a = nullptr ;
	    }
	} /* end if */

	return rs ;
}
/* end subroutine (procinfo_start) */


static int procinfo_finish(PROCINFO *pip)
{
	int		rs = SR_OK ;
	int		rs1 ;

	rs1 = procinfo_uaend(pip) ;
	if (rs >= 0) rs = rs1 ;

	if (pip->pwbuf != nullptr) {
	    rs1 = uc_free(pip->pwbuf) ;
	    if (rs >= 0) rs = rs1 ;
	    pip->pwbuf = nullptr ;
	    pip->pwlen = 0 ;
	}

	rs1 = bits_finish(&pip->have) ;
	if (rs >= 0) rs = rs1 ;

	if (pip->a != nullptr) {
	    rs1 = uc_free(pip->a) ;
	    if (rs >= 0) rs = rs1 ;
	    pip->a = nullptr ;
	}

	return rs ;
}
/* end subroutine (procinfo_finish) */


static int procinfo_store(PROCINFO *pip,int uit,cchar *vp,int vl,cchar **rpp)
{
	STRSTORE	*stp = pip->stp ;
	int		rs = SR_OK ;

	if (uit >= 0) {
	    if (pip->sis[uit] == 0) {
	        rs = strstore_store(stp,vp,vl,rpp) ;
	        pip->sis[uit] = rs ;
	        if (rs >= 0) {
	            rs = bits_set(&pip->have,uit) ;
		}
	    }
	} else
	    rs = SR_BUGCHECK ;

	if ((rpp != nullptr) && (rs < 0)) *rpp = nullptr ;
	return rs ;
}
/* end subroutine (procinfo_store) */


static int procinfo_uabegin(PROCINFO *pip)
{
	int		rs = SR_OK ;

#if	CF_DEBUGS
	debugprintf("userinfo/procinfo_uabegin: f_pw=%u\n",
	    pip->f.pw) ;
	debugprintf("userinfo/procinfo_uabegin: f_ua=%u f_uainit=%u\n",
	    pip->f.ua,pip->f.uainit) ;
#endif

	if ((! pip->f.ua) && pip->f.pw && (! pip->f.uainit)) {
	    pip->f.uainit = TRUE ;

	    if (pip->uap == nullptr) {
	        const int	size = sizeof(USERATTR) ;
	        char		*p ;
	        if ((rs = uc_malloc(size,&p)) >= 0) {
	            pip->uap = (USERATTR *) p ;
	            pip->f.allocua = TRUE ;
	        }
	    }

	    if (rs >= 0) {
	        cchar	*un = pip->pw.pw_name ;
#if	CF_DEBUGS
	        debugprintf("userinfo/procinfo_uabegin: dn=%s\n",dn) ;
#endif
	        if ((rs = userattr_open(pip->uap,un)) >= 0) {
	            pip->f.ua = TRUE ;
		} else {
		    {
	                uc_free(pip->uap) ;
	                pip->uap = nullptr ;
	                pip->f.allocua = FALSE ;
		    }
		    if (isNotPresent(rs)) rs = SR_OK ;
		}
	    } /* end if (ok) */

	} /* end if (f.ua) */

#if	CF_DEBUGS
	debugprintf("userinfo/procinfo_uabegin: ret rs=%d\n",rs) ;
	debugprintf("userinfo/procinfo_uabegin: f_ua=%u f_uainit=%u\n",
	    pip->f.ua,pip->f.uainit) ;
#endif

	return rs ;
}
/* end subroutine (procinfo_uabegin) */


static int procinfo_uaend(PROCINFO *pip)
{
	int		rs = SR_OK ;
	int		rs1 ;

	if (pip->f.ua) {
	    pip->f.ua = FALSE ;
	    rs1 = userattr_close(pip->uap) ;
	    if (rs >= 0) rs = rs1 ;
	}

	if (pip->f.allocua && (pip->uap != nullptr)) {
	    rs1 = uc_free(pip->uap) ;
	    pip->uap = nullptr ;
	    pip->f.allocua = FALSE ;
	    if (rs >= 0) rs = rs1 ;
	}

	return rs ;
}
/* end subroutine (procinfo_uaend) */


static int procinfo_ualookup(PROCINFO *pip,char *rbuf,int rlen,cchar *kn)
{
	int		rs = SR_OK ;

	if (rbuf == nullptr) return SR_FAULT ;
	if (kn == nullptr) return SR_FAULT ;

	rbuf[0] = '\0' ;
	if (pip->f.pw) {
	    if (! pip->f.ua) rs = procinfo_uabegin(pip) ;
	    if (rs >= 0) {
	        rs = userattr_lookup(pip->uap,rbuf,rlen,kn) ;
	    }
	}

#if	CF_DEBUGS
	debugprintf("userinfo/procinfo_ualookup: ret rs=%d\n",rs) ;
#endif

	return rs ;
}
/* end subroutine (procinfo_ualookup) */


static int procinfo_find(PROCINFO *pip,cchar *un)
{
	USERINFO	*uip = pip->uip ;
	int		rs ;

	if ((rs = procinfo_pwentry(pip,un)) >= 0) {
	    const int	dlen = DIGBUFLEN ;
	    int		cl = -1 ;
	    int		uit ;
	    cchar	*cp ;
	    char	dbuf[DIGBUFLEN+1] ;

	    if (rs > 0) {
	        cchar	*vp ;
	        uip->gid = pip->pw.pw_gid ;
	        cp = pip->pw.pw_name ;
	        if (rs >= 0) {
	            uit = uit_password ;
	            vp = pip->pw.pw_passwd ;
	            rs = procinfo_store(pip,uit,vp,-1,nullptr) ;
	        }
	        if (rs >= 0) {
	            uit = uit_gecos ;
	            vp = pip->pw.pw_gecos ;
	            rs = procinfo_store(pip,uit,vp,-1,nullptr) ;
	        }
	        if (rs >= 0) {
	            uit = uit_homedname ;
	            vp = pip->pw.pw_dir ;
	            rs = procinfo_store(pip,uit,vp,-1,nullptr) ;
	        }
	    } else {
	        uint	uv = (uint) uip->uid ;
	        cp = dbuf ;
	        dbuf[0] = 'U' ;
	        cl = ctdecui((dbuf+1),(dlen-1),uv) + 1 ;
	    } /* end if */

	    if (rs >= 0) {
	        uit = uit_username ;
	        rs = procinfo_store(pip,uit,cp,cl,nullptr) ;
	    }

	    if (rs >= 0) {
	        int	i ;
	        for (i = 0 ; components[i] != nullptr ; i += 1) {
	            rs = (*components[i])(pip) ;
	            if (rs < 0) break ;
	        } /* end if */
	    } /* end if */

	} /* end if */

#if	CF_DEBUGS
	debugprintf("procinfo_find: ret rs=%d\n",rs) ;
#endif

	return rs ;
}
/* end subroutine (procinfo_find) */


static int procinfo_pwentry(PROCINFO *pip,cchar *un)
{
	PASSWD	pw ;
	int		rs ;
	int		pwlen = pip->tlen ;
	int		f = FALSE ;
	char		*pwbuf = pip->tbuf ;

	if ((rs = procinfo_getpwuser(pip,&pw,pwbuf,pwlen,un)) >= 0) {
	    pip->f.pw = TRUE ;

#if	CF_DEBUGS
	    debugprintf("procinfo_pwentry: getpwusername() rs=%d\n",rs) ;
#endif

	    if ((rs = passwdent_size(&pw)) >= 0) {
	        int	size = rs ;
	        char	*p ;
#if	CF_DEBUGS
	        debugprintf("procinfo_pwentry: size=%d\n",size) ;
#endif
	        if ((rs = uc_malloc((size+1),&p)) >= 0) {
	            PASSWD	*pwp = &pip->pw ;
	            if ((rs = passwdent_load(pwp,p,size,&pw)) >= 0) {
	                pip->pwbuf = p ;
	                pip->pwlen = size ;
	                f = TRUE ;
	            }
	            if (rs < 0)
	                uc_free(p) ;
	        } /* end if (memory-allocation) */
	    } /* end if (passwdent_size) */

	} /* end if (procinfo_getpwuser) */

	return (rs >= 0) ? f : rs ;
}
/* end subroutine (procinfo_pwentry) */


static int procinfo_getpwuser(PROCINFO *pip,PASSWD *pwp,
		char pwbuf[],int pwlen,cchar *un)
{
	USERINFO	*uip = pip->uip ;
	int		rs ;

	if ((un != nullptr) && (un[0] != '-') && (un[0] != '\0')) {
	    if ((rs = GETPW_NAME(pwp,pwbuf,pwlen,un)) >= 0) {
	        pip->f.altuser = (pwp->pw_uid != uip->uid) ;
	        if (pip->f.altuser) {
	            uip->uid = pwp->pw_uid ;
	            uip->gid = pwp->pw_gid ;
	        }
	    }
	} else {
	    rs = getpwusername(pwp,pwbuf,pwlen,uip->uid) ;
	}

	return rs ;
}
/* end subroutine (procinfo_getpwuser) */


static int procinfo_homedname(PROCINFO *pip)
{
	USERINFO	*uip = pip->uip ;
	int		rs = SR_OK ;
	int		uit = uit_homedname ;

	if ((pip->sis[uit] == 0) && (uip->homedname == nullptr)) {
	    cchar	*vp = nullptr ;
	    if ((vp == nullptr) || (vp[0] == '\0')) {
	        if (! pip->f.altuser) {
	            vp = getenv(VARHOME) ;
	        }
	    }
	    if ((vp == nullptr) || (vp[0] == '\0')) {
	        vp = getenv(VARTMPDNAME) ;
	    }
	    if ((vp == nullptr) || (vp[0] == '\0')) {
	        vp = TMPDNAME ;
	    }
	    uip->homedname = vp ;
	}

	return rs ;
}
/* end subroutine (procinfo_homedname) */


static int procinfo_shell(PROCINFO *pip)
{
	USERINFO	*uip = pip->uip ;
	int		rs = SR_OK ;
	int		uit = uit_shell ;

	if ((pip->sis[uit] == 0) && (uip->shell == nullptr)) {
	    cchar	*vp = nullptr ;
	    if ((vp == nullptr) || (vp[0] == '\0')) {
	        if (! pip->f.altuser) {
	            vp = getenv(VARSHELL) ;
	        }
	    }
	    if ((vp != nullptr) && (vp[0] != '\0')) {
	        uip->shell = vp ;
	    }
	}

	return rs ;
}
/* end subroutine (procinfo_shell) */


static int procinfo_org(PROCINFO *pip)
{
	USERINFO	*uip = pip->uip ;
	int		rs = SR_OK ;
	int		uit = uit_organization ;

	if ((pip->sis[uit] == 0) && (uip->organization == nullptr)) {
	    const int	rlen = MAXNAMELEN ;
	    int		vl = -1 ;
	    cchar	*orgcname = ORGCNAME ;
	    cchar	*vp = nullptr ;
	    char	rbuf[MAXNAMELEN+1] ;
	    char	orgfname[MAXPATHLEN+1] ;
	    if ((vp == nullptr) || (vp[0] == '\0')) {
	        if (! pip->f.altuser) {
	            vp = getenv(VARORGANIZATION) ;
	        }
	    }
	    if ((vp == nullptr) || (vp[0] == '\0')) {
	        char	cname[MAXNAMELEN+1] ;
	        rs = sncpy2(cname,MAXNAMELEN,".",orgcname) ;
	        if (rs >= 0) {
	            cchar	*hd = uip->homedname ;
	            if ((hd == nullptr) && pip->f.pw) {
	                hd = pip->pw.pw_dir ;
		    }
	            if (hd == nullptr) hd = DEFHOMEDNAME ;
	            rs = mkpath2(orgfname,hd,cname) ;
	        }
	        if (rs >= 0) {
	            rs = readfileline(rbuf,rlen,orgfname) ;
	            vl = rs ;
	            if (rs > 0) vp = rbuf ;
	            if (isNotPresent(rs)) rs = SR_OK ;
	        }
	    }
	    if (rs >= 0) {
	        if ((vp != nullptr) && (vp[0] != '\0'))
	            rs = procinfo_store(pip,uit,vp,vl,nullptr) ;
	    }
	} /* end if */

	return rs ;
}
/* end subroutine (procinfo_org) */


static int procinfo_bin(PROCINFO *pip)
{
	USERINFO	*uip = pip->uip ;
	int		rs = SR_OK ;
	int		uit = uit_bin ;

	if ((pip->sis[uit] == 0) && (uip->bin == nullptr)) {
	    cchar	*vp = nullptr ;
	    if ((vp == nullptr) || (vp[0] == '\0')) {
	        if (! pip->f.altuser) {
	            vp = getenv(VARBIN) ;
	        }
	    }
	    if ((vp != nullptr) && (vp[0] != '\0')) {
	        uip->bin = vp ;
	    }
	}

	return rs ;
}
/* end subroutine (procinfo_bin) */


static int procinfo_office(PROCINFO *pip)
{
	USERINFO	*uip = pip->uip ;
	int		rs = SR_OK ;
	int		uit = uit_office ;

	if ((pip->sis[uit] == 0) && (uip->office == nullptr)) {
	    cchar	*vp = nullptr ;
	    if ((vp == nullptr) || (vp[0] == '\0')) {
	        if (! pip->f.altuser) {
	            vp = getenv(VAROFFICE) ;
	        }
	    }
	    if ((vp != nullptr) && (vp[0] != '\0')) {
	        uip->office = vp ;
	    }
	}

	return rs ;
}
/* end subroutine (procinfo_office) */


static int procinfo_printer(PROCINFO *pip)
{
	USERINFO	*uip = pip->uip ;
	int		rs = SR_OK ;
	int		uit = uit_printer ;

	if ((pip->sis[uit] == 0) && (uip->printer == nullptr)) {
	    cchar	*vp = nullptr ;
	    if ((vp == nullptr) || (vp[0] == '\0')) {
	        if (! pip->f.altuser) {
	            vp = getenv(VARPRINTER) ;
	        }
	    }
	    if ((vp != nullptr) && (vp[0] != '\0')) {
	        uip->printer = vp ;
	    }
	}

	return rs ;
}
/* end subroutine (procinfo_printer) */


/* parse out the GECOS field */
static int procinfo_gecos(PROCINFO *pip)
{
	int		rs = SR_OK ;
	int		rs1 ;
	cchar	*gstr = pip->pw.pw_gecos ;

	if (pip->f.pw && (gstr != nullptr) && (gstr[0] != '\0')) {
	    GECOS	g ;
	    if ((rs = gecos_start(&g,gstr,-1)) >= 0) {
	        USERINFO	*uip = pip->uip ;
	        BITS		*bip = &pip->have ;
	        int		vl ;
	        int		i ;
	        int		uit ;
	        cchar	*vp ;
	        cchar	*up ;

	        for (i = 0 ; i < gecosval_overlast ; i += 1) {

	            if ((vl = gecos_getval(&g,i,&vp)) >= 0) {

	                uit = -1 ;
	                up = nullptr ;
	                switch (i) {

	                case gecosval_organization:
	                    uit = uit_organization ;
	                    up = uip->organization ;
	                    if ((bits_test(bip,uit) > 0) || (up != nullptr))
	                        vp = nullptr ;
	                    break ;

	                case gecosval_realname:
	                    uit = uit_gecosname ;
	                    up = uip->gecosname ;
	                    if ((bits_test(bip,uit) == 0) && (up == nullptr)) {
	                        void	*p ;
	                        if ((rs = uc_malloc((vl+1),&p)) >= 0) {
	                            char	*nbuf = p ;
	                            if (strnchr(vp,vl,'_') != nullptr) {
	                                rs = snwcpyhyphen(nbuf,-1,vp,vl) ;
	                                vp = nbuf ;
	                            }
	                            if (rs >= 0) {
	                                if (pip->sis[uit] == 0) {
	                                    STRSTORE	*stp = pip->stp ;
	                                    cchar	*cp ;
	                                    rs = strstore_store(stp,vp,vl,&cp) ;
	                                    pip->sis[uit] = rs ;
	                                    pip->tstrs.gecosname = cp ;
	                                }
	                                vp = nullptr ;
	                            }
	                            uc_free(p) ;
	                        } /* end if (memory-allocation) */
	                    } else
	                        vp = nullptr ;
	                    break ;

	                case gecosval_account:
	                    uit = uit_account ;
	                    up = uip->account ;
	                    if ((bits_test(bip,uit) > 0) || (up != nullptr))
	                        vp = nullptr ;
	                    break ;

	                case gecosval_bin:
	                    uit = uit_bin ;
	                    up = uip->bin ;
	                    if ((bits_test(bip,uit) > 0) || (up != nullptr))
	                        vp = nullptr ;
	                    break ;

	                case gecosval_office:
	                    uit = uit_office ;
	                    up = uip->office ;
	                    if ((bits_test(bip,uit) > 0) || (up != nullptr))
	                        vp = nullptr ;
	                    break ;

	                case gecosval_wphone:
	                    uit = uit_wphone ;
	                    up = uip->hphone ;
	                    if ((bits_test(bip,uit) > 0) || (up != nullptr))
	                        vp = nullptr ;
	                    break ;

	                case gecosval_hphone:
	                    uit = uit_hphone ;
	                    up = uip->hphone ;
	                    if ((bits_test(bip,uit) > 0) || (up != nullptr))
	                        vp = nullptr ;
	                    break ;

	                case gecosval_printer:
	                    uit = uit_printer ;
	                    up = uip->printer ;
	                    if ((bits_test(bip,uit) > 0) || (up != nullptr))
	                        vp = nullptr ;
	                    break ;

	                default:
	                    vp = nullptr ;
	                    break ;

	                } /* end switch */

	                if ((rs >= 0) && (vp != nullptr)) {
	                    rs = procinfo_store(pip,uit,vp,vl,nullptr) ;
	                }

	            } /* end if */

	            if (rs < 0) break ;
	        } /* end for */

	        rs1 = gecos_finish(&g) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (gecos) */
	} /* end if (non-empty) */

	return rs ;
}
/* end subroutine (procinfo_gecos) */


static int procinfo_gecosname(PROCINFO *pip)
{
	USERINFO	*uip = pip->uip ;
	int		rs = SR_OK ;
	int		uit = uit_gecosname ;

	if ((pip->sis[uit] == 0) && (uip->gecosname == nullptr)) {
	    cchar	*gstr = pip->pw.pw_gecos ;
	    if ((gstr != nullptr) && (gstr[0] != '\0')) {
	        const int	nlen = REALNAMELEN ;
	        int		vl ;
	        cchar	*vp ;
	        char		nbuf[REALNAMELEN+1] ;
	        if ((vl = mkgecosname(nbuf,nlen,gstr)) > 0) {
	            vp = nbuf ;
	            rs = procinfo_store(pip,uit,vp,vl,nullptr) ;
	        }
	    }
	}

	return rs ;
}
/* end subroutine (procinfo_gecosname) */


static int procinfo_realname(PROCINFO *pip)
{
	USERINFO	*uip = pip->uip ;
	int		rs = SR_OK ;
	int		uit = uit_realname ;

#if	CF_DEBUGS
	debugprintf("userinfo/procinfo_realname: sis[%u]=%u\n",
	    uit,pip->sis[uit]) ;
	debugprintf("userinfo/procinfo_realname: realname=%s\n",
	    uip->realname) ;
#endif

	if ((pip->sis[uit] == 0) && (uip->realname == nullptr)) {
	    const int	nlen = REALNAMELEN ;
	    cchar	*np = pip->tstrs.gecosname ;
	    char	nbuf[REALNAMELEN+1] ;
	    if ((np == nullptr) || (np[0] == '\0')) {
	        if (! pip->f.altuser) {
	            np = getenv(VARNAME) ;
	        }
	    }
#if	CF_DEBUGS
	    debugprintf("userinfo/procinfo_realname: n=%s\n",np) ;
#endif
	    if ((np != nullptr) && (np[0] != '\0')) {
	        int		vl ;
	        cchar	*vp = nbuf ;
	        if ((vl = mkrealname(nbuf,nlen,np,-1)) > 0) {
	            STRSTORE	*stp = pip->stp ;
	            cchar	*cp ;
#if	CF_DEBUGS
	            debugprintf("userinfo/procinfo_realname: realname=%t\n",
	                vp,vl) ;
#endif
	            rs = strstore_store(stp,vp,vl,&cp) ;
	            pip->sis[uit] = rs ;
	            pip->tstrs.realname = cp ;
#if	CF_DEBUGS 
	            {
	                debugprintf("userinfo/procinfo_realname: "
	                    "strstore_store() rs=%d\n",rs) ;
	                debugprintf("userinfo/procinfo_realname: "
	                    "tstrs.realname=%s\n",
	                    cp) ;
	            }
#endif
	        }
	    }
	} /* end if (needed) */

	return rs ;
}
/* end subroutine (procinfo_realname) */


static int procinfo_mailname(PROCINFO *pip)
{
	USERINFO	*uip = pip->uip ;
	int		rs = SR_OK ;
	int		uit = uit_mailname ;

#if	CF_DEBUGS
	debugprintf("procinfo_mailname: tstrs.realname¬\n") ;
	debugprintf("procinfo_mailname: tstrs.realname=%s\n",
	    pip->tstrs.realname) ;
#endif

	if ((pip->sis[uit] == 0) && (uip->mailname == nullptr)) {
	    const int	nlen = REALNAMELEN ;
	    const int	mlen = REALNAMELEN ;
	    int		vl ;
	    int		nl = -1 ;
	    cchar	*vp ;
	    cchar	*np = nullptr ;
	    char	nbuf[REALNAMELEN+1] ;
	    char	mbuf[REALNAMELEN+1] ;
	    if ((np == nullptr) || (np[0] == '\0')) {
	        if (! pip->f.altuser) {
	            np = getenv(VARMAILNAME) ;
	        }
	    }
#if	CF_DEBUGS
	    debugprintf("procinfo_mailname: np=%s\n",np) ;
#endif
	    if ((np == nullptr) || (np[0] == '\0')) {
	        np = pip->tstrs.gecosname ;
	    }
	    if ((np == nullptr) || (np[0] == '\0')) {
	        if (pip->f.pw) {
	            cchar	*gn = pip->pw.pw_gecos ;
	            np = nbuf ;
	            nl = mkgecosname(nbuf,nlen,gn) ;
	        }
	    }
#if	CF_DEBUGS
	    debugprintf("procinfo_mailname: np=%s\n",np) ;
#endif
	    if ((np == nullptr) || (np[0] == '\0')) {
	        if (! pip->f.altuser) {
	            np = getenv(VARNAME) ;
	        }
	    }
#if	CF_DEBUGS
	    debugprintf("procinfo_mailname: np=%s\n",np) ;
#endif
	    if ((np == nullptr) || (np[0] == '\0')) {
	        if (pip->f.pw) np = pip->pw.pw_name ;
	    }
#if	CF_DEBUGS
	    debugprintf("procinfo_mailname: np=%s\n",np) ;
#endif
	    if ((np != nullptr) && (np[0] != '\0')) {
	        vp = mbuf ;
	        if ((vl = mkmailname(mbuf,mlen,np,nl)) > 0)
	            rs = procinfo_store(pip,uit,vp,vl,nullptr) ;
#if	CF_DEBUGS
	        debugprintf("procinfo_mailname: rs=%d vl=%u mn=%s\n",
	            rs,vl,mbuf) ;
#endif
	    }
	}

#if	CF_DEBUGS
	debugprintf("procinfo_mailname: ret rs=%d\n",rs) ;
#endif

	return rs ;
}
/* end subroutine (procinfo_mailname) */


static int procinfo_name(PROCINFO *pip)
{
	USERINFO	*uip = pip->uip ;
	int		rs = SR_OK ;
	int		uit = uit_name ;

#if	CF_DEBUGS
	debugprintf("procinfo_name: ent\n") ;
#endif

	if ((pip->sis[uit] == 0) && (uip->name == nullptr)) {
	    const int	glen = REALNAMELEN ;
	    const int	rlen = REALNAMELEN ;
	    int		nl = -1 ;
	    cchar	*np = nullptr ;
	    char	gbuf[REALNAMELEN+1] ;
	    char	rbuf[REALNAMELEN+1] ;
	    if ((np == nullptr) || (np[0] == '\0')) {
	        if (! pip->f.altuser) {
	            np = getenv(VARNAME) ;
	        }
	    }
	    if ((np == nullptr) || (np[0] == '\0')) {
	        np = pip->tstrs.realname ;
	        if (np == nullptr) {
	            cchar	*gp = pip->tstrs.gecosname ;
	            int		gl = -1 ;
	            if (gp == nullptr) {
	                cchar	*gn = pip->pw.pw_gecos ;
	                gp = gbuf ;
	                gl = mkgecosname(gbuf,glen,gn) ;
	            }
	            np = rbuf ;
	            nl = mkrealname(rbuf,rlen,gp,gl) ;
	        }
	    }
	    if ((np == nullptr) || (np[0] == '\0')) {
	        if (pip->f.pw) np = pip->pw.pw_name ;
	    }
	    if ((np != nullptr) && (np[0] != '\0')) {
	        rs = procinfo_store(pip,uit,np,nl,nullptr) ;
	    }
	}

#if	CF_DEBUGS
	debugprintf("procinfo_name: ret rs=%d\n",rs) ;
#endif

	return rs ;
}
/* end subroutine (procinfo_name) */


static int procinfo_fullname(PROCINFO *pip)
{
	USERINFO	*uip = pip->uip ;
	int		rs = SR_OK ;
	int		uit = uit_fullname ;

#if	CF_DEBUGS
	debugprintf("procinfo_fullname: ent\n") ;
#endif

	if ((pip->sis[uit] == 0) && (uip->fullname == nullptr)) {
	    cchar	*np = nullptr ;
	    if ((np == nullptr) || (np[0] == '\0')) {
	        if (! pip->f.altuser) {
	            np = getenv(VARFULLNAME) ;
	        }
	    }
	    if ((np != nullptr) && (np[0] != '\0')) {
	        uip->fullname = np ;
	    }
	}

#if	CF_DEBUGS
	debugprintf("procinfo_fullname: ret rs=%d\n",rs) ;
#endif

	return rs ;
}
/* end subroutine (procinfo_fullname) */


static int procinfo_nodename(PROCINFO *pip)
{
	USERINFO	*uip = pip->uip ;
	int		rs = SR_OK ;
	int		uit = uit_nodename ;

#if	CF_DEBUGS
	debugprintf("procinfo_nodename: ent\n") ;
	debugprintf("procinfo_nodename: uip->nodename=%s\n",
	    uip->nodename) ;
#endif

	if ((pip->sis[uit] == 0) && (uip->nodename == nullptr)) {
	    const int	nlen = NODENAMELEN ;
	    int		nl = -1 ;
	    char	*nbuf = pip->nodename ;
	    if (nbuf[0] == '\0') {
	        rs = getnodename(nbuf,nlen) ;
	        nl = rs ;
	    }
	    if (rs >= 0) {
	        rs = procinfo_store(pip,uit,nbuf,nl,nullptr) ;
#if	CF_DEBUGS
	        debugprintf("procinfo_nodename: nbuf=%s\n",nbuf) ;
#endif
	    }
	}

#if	CF_DEBUGS
	debugprintf("procinfo_nodename: pip->nodename=%s\n",
	    pip->nodename) ;
	debugprintf("procinfo_nodename: ret rs=%d\n",rs) ;
#endif

	return rs ;
}
/* end subroutine (procinfo_nodename) */


static int procinfo_domainname(PROCINFO *pip)
{
	USERINFO	*uip = pip->uip ;
	int		rs = SR_OK ;
	int		uit = uit_domainname ;

#if	CF_DEBUGS
	debugprintf("procinfo_domainname: ent sis=%u dn=%s\n",
	    pip->sis[uit], uip->domainname) ;
#endif

	if ((pip->sis[uit] == 0) && (uip->domainname == nullptr)) {
	    uip->domainname = nullptr ;
	    if (uip->domainname == nullptr) {
	        if (! pip->f.altuser) {
	            uip->domainname = getenv(VARDOMAIN) ;
	        }
	    }
	    if (uip->domainname == nullptr) {
	        const int	dlen = MAXHOSTNAMELEN ;
	        int		dl = -1 ;
	        cchar	*nn = uip->nodename ;
	        char		*dbuf = pip->domainname ;
	        if ((rs >= 0) && (dbuf[0] == '\0')) {
	            if (pip->f.pw) {
	                cchar	*kn = UA_DN ;
	                rs = procinfo_ualookup(pip,dbuf,dlen,kn) ;
	                if (isNotPresent(rs)) {
	                    dbuf[0] = '\0' ;
	                    rs = SR_OK ;
	                }
	            }
	        }
	        if ((rs >= 0) && (dbuf[0] == '\0')) {
	            if (nn == nullptr) nn = pip->nodename ;
	            rs = getdomainname(dbuf,dlen,nn) ;
	            dl = rs ;
	            if (isNotPresent(rs)) {
	                dbuf[0] = '\0' ;
	                rs = SR_OK ;
	            }
	        }
	        if ((rs >= 0) && (dbuf[0] != '\0')) {
	            rs = procinfo_store(pip,uit,dbuf,dl,nullptr) ;
	        }
	    }
	}

#if	CF_DEBUGS
	debugprintf("procinfo_domainname: ret rs=%d\n",rs) ;
#endif

	return rs ;
}
/* end subroutine (procinfo_domainename) */


static int procinfo_project(PROCINFO *pip)
{
	USERINFO	*uip = pip->uip ;
	int		rs = SR_OK ;
	int		uit = uit_project ;

#if	CF_DEBUGS
	debugprintf("procinfo_project: ent projnamelen=%u\n",
	    PROJNAMELEN) ;
#endif

	if ((pip->sis[uit] == 0) && (uip->project == nullptr)) {
	    const int	dlen = PROJNAMELEN ;
	    int		dl = -1 ;
	    char	dbuf[PROJNAMELEN+1] = { 0 } ;
	    if ((rs >= 0) && (dbuf[0] == '\0')) {
	        if (pip->f.pw) {
	            cchar	*kn = UA_PROJECT ;
	            rs = procinfo_ualookup(pip,dbuf,dlen,kn) ;
#if	CF_DEBUGS
	            debugprintf("procinfo_project: mid1 rs=%d\n",rs) ;
#endif
	            dl = rs ;
	            if (isNotPresent(rs)) {
	                dbuf[0] = '\0' ;
	                rs = SR_OK ;
	            }
	        }
	    }
	    if ((rs >= 0) && (dbuf[0] == '\0')) {
	        if (pip->f.pw) {
	            cchar	*un = pip->pw.pw_name ;
	            rs = getprojname(dbuf,dlen,un) ;
#if	CF_DEBUGS
	            debugprintf("procinfo_project: mid2 rs=%d\n",rs) ;
#endif
	            dl = rs ;
	            if (isNotPresent(rs) || (rs == SR_NOSYS)) {
	                dbuf[0] = '\0' ;
	                rs = SR_OK ;
	            }
	        }
	    }
	    if ((rs >= 0) && (dbuf[0] != '\0')) {
	        rs = procinfo_store(pip,uit,dbuf,dl,nullptr) ;
	    }
	} /* end if (project) */

#if	CF_DEBUGS
	debugprintf("procinfo_project: ret rs=%d\n",rs) ;
#endif

	return rs ;
}
/* end subroutine (procinfo_project) */


static int procinfo_tz(PROCINFO *pip)
{
	USERINFO	*uip = pip->uip ;
	int		rs = SR_OK ;
	int		uit = uit_tz ;

#if	CF_DEBUGS
	debugprintf("procinfo_tz: ent\n") ;
	debugprintf("procinfo_tz: pip->sis[uit]=%u\n",pip->sis[uit]) ;
	debugprintf("procinfo_tz: uip->tz=%s\n",uip->tz) ;
#endif

	if ((pip->sis[uit] == 0) && (uip->tz == nullptr)) {
	    uip->tz = nullptr ;
	    if (uip->tz == nullptr) {
	        if (! pip->f.altuser) {
	            uip->tz = getenv(VARTZ) ;
	        }
	    }
#if	CF_DEBUGS
	    debugprintf("procinfo_tz: 2 uip->tz=%s\n",uip->tz) ;
#endif
	    if (uip->tz == nullptr) {
	        const int	dlen = TZLEN ;
	        int		dl = -1 ;
	        char		dbuf[TZLEN+1] = { 0 } ;
	        if ((rs >= 0) && (dbuf[0] == '\0')) {
#if	CF_DEBUGS
	            debugprintf("procinfo_tz: f_pw=%u f_ua=%u\n",
	                pip->f.pw,pip->f.ua) ;
#endif
	            if (pip->f.pw) {
	                cchar	*kn = UA_TZ ;
	                rs = procinfo_ualookup(pip,dbuf,dlen,kn) ;
	                dl = rs ;
#if	CF_DEBUGS
	                debugprintf("procinfo_tz: procinfo_ualookup() rs=%d\n",
	                    rs) ;
#endif
	                if (isNotPresent(rs)) {
	                    dbuf[0] = '\0' ;
	                    rs = SR_OK ;
	                }
	            }
	        }
	        if ((rs >= 0) && (dbuf[0] != '\0')) {
	            rs = procinfo_store(pip,uit,dbuf,dl,nullptr) ;
	        }
	    }
	} /* end if (tz) */

#if	CF_DEBUGS
	debugprintf("procinfo_tz: ret rs=%d\n",rs) ;
#endif

	return rs ;
}
/* end subroutine (procinfo_tz) */


static int procinfo_md(PROCINFO *pip)
{
	USERINFO	*uip = pip->uip ;
	int		rs = SR_OK ;
	int		uit = uit_md ;

#if	CF_DEBUGS
	debugprintf("procinfo_md: ent\n") ;
#endif

	if ((pip->sis[uit] == 0) && (uip->md == nullptr)) {
	    uip->md = nullptr ;
	    if (uip->md == nullptr) {
	        if (! pip->f.altuser) {
	            uip->md = getenv(VARMD) ;
	        }
	    }
	    if (uip->md == nullptr) {
	        const int	dlen = MAXHOSTNAMELEN ;
	        int		dl = -1 ;
	        char		dbuf[MAXHOSTNAMELEN+1] = { 0 } ;
	        if ((rs >= 0) && (dbuf[0] == '\0')) {
	            if (pip->f.pw) {
	                cchar	*kn = UA_MD ;
	                rs = procinfo_ualookup(pip,dbuf,dlen,kn) ;
	                dl = rs ;
	                if (isNotPresent(rs)) {
	                    dbuf[0] = '\0' ;
	                    rs = SR_OK ;
	                }
	            }
	        }
	        if ((rs >= 0) && (dbuf[0] != '\0')) {
	            rs = procinfo_store(pip,uit,dbuf,dl,nullptr) ;
	        }
	    }
	} /* end if (md) */

#if	CF_DEBUGS
	debugprintf("procinfo_md: ret rs=%d\n",rs) ;
#endif

	return rs ;
}
/* end subroutine (procinfo_md) */


static int procinfo_wstation(PROCINFO *pip)
{
	USERINFO	*uip = pip->uip ;
	int		rs = SR_OK ;
	int		uit = uit_wstation ;

#if	CF_DEBUGS
	debugprintf("procinfo_wstation: ent\n") ;
#endif

	if ((pip->sis[uit] == 0) && (uip->wstation == nullptr)) {
	    uip->wstation = nullptr ;
	    if (uip->wstation == nullptr) {
	        if (! pip->f.altuser) {
	            uip->wstation = getenv(VARWSTATION) ;
	        }
	    }
	    if (uip->wstation == nullptr) {
	        const int	dlen = WSLEN ;
	        int		dl = -1 ;
	        char		dbuf[WSLEN+1] = { 0 } ;
	        if ((rs >= 0) && (dbuf[0] == '\0')) {
	            if (pip->f.pw) {
	                cchar	*kn = UA_WS ;
	                rs = procinfo_ualookup(pip,dbuf,dlen,kn) ;
	                dl = rs ;
	                if (isNotPresent(rs)) {
	                    dbuf[0] = '\0' ;
	                    rs = SR_OK ;
	                }
	            }
	        }
	        if ((rs >= 0) && (dbuf[0] != '\0')) {
	            rs = procinfo_store(pip,uit,dbuf,dl,nullptr) ;
	        }
	    }
	}

#if	CF_DEBUGS
	debugprintf("procinfo_wstation: ret rs=%d\n",rs) ;
#endif

	return rs ;
}
/* end subroutine (procinfo_wstation) */


static int procinfo_logid(PROCINFO *pip)
{
	USERINFO	*uip = pip->uip ;
	int		rs = SR_OK ;
	int		uit = uit_logid ;

#if	CF_DEBUGS
	debugprintf("procinfo_logid: ent sis[%u]=%u\n",
	    uit,pip->sis[uit]) ;
	debugprintf("procinfo_logid: pip->nodename=%s\n",
	    pip->nodename) ;
#endif

	if ((pip->sis[uit] == 0) && (uip->logid == nullptr)) {
	    int		v = uip->pid ;
	    cchar	*nn = uip->nodename ;
#if	CF_DEBUGS
	    debugprintf("procinfo_logid: unn=%s\n",nn) ;
#endif
	    if ((nn == nullptr) || (nn[0] == '\0')) nn = pip->nodename ;
#if	CF_DEBUGS
	    debugprintf("procinfo_logid: snn=%s\n",nn) ;
#endif
	    if ((nn != nullptr) && (nn[0] != '\0')) {
	        const int	llen = LOGIDLEN ;
	        int		vl ;
	        cchar	*vp ;
	        char		lbuf[LOGIDLEN+1] ;
	        vp = lbuf ;
	        if ((vl = mklogid(lbuf,llen,nn,-1,v)) >= 0) {
	            rs = procinfo_store(pip,uit,vp,vl,nullptr) ;
	        }
	    }
	}

#if	CF_DEBUGS
	debugprintf("procinfo_logid: ret rs=%d\n",rs) ;
#endif

	return rs ;
}
/* end subroutine (procinfo_logid) */


#if	CF_UINFO
static int procinfo_uinfo(PROCINFO *pip)
{
	int		rs ;

	if ((rs = uinfo_name(&pip->unixinfo)) >= 0) {
	    USERINFO	*uip = pip->uip ;
	    UINFO_NAME	*xip = &pip->unixinfo ;
	    const int	uit = uit_nodename ;
	    uip->sysname = xip->sysname ;
	    uip->release = xip->release ;
	    uip->version = xip->version ;
	    uip->machine = xip->machine ;
	    if ((pip->sis[uit] == 0) && (uip->nodename == nullptr)) {
	        uip->nodename = xip->nodename ;
	    }
	}

	return rs ;
}
/* end subroutine (procinfo_uinfo) */
#endif /* CF_UINFO */


#if	CF_UNAME
static int procinfo_uname(PROCINFO *pip)
{
	struct utsname	un ;
	int		rs ;

#if	CF_DEBUGS
	debugprintf("procinfo_uname: ent\n") ;
#endif

	if ((rs = u_uname(&un)) >= 0) {
	    USERINFO	*uip = pip->uip ;
	    int		i ;
	    int		uit ;
	    cchar	*vp ;
	    for (i = 0 ; i < 5 ; i += 1) {
	        uit = -1 ;
	        vp = nullptr ;
	        switch (i) {
	        case 0:
	            uit = uit_sysname ;
	            vp = un.sysname ;
	            break ;
	        case 1:
	            uit = uit_nodename ;
	            if (uip->nodename == nullptr) {
	                vp = un.nodename ;
	                if (pip->nodename[0] == '\0') {
	                    const int	nlen = NODENAMELEN ;
	                    char	*nbuf = pip->nodename ;
	                    strwcpy(nbuf,vp,nlen) ;
	                }
	            }
	            break ;
	        case 2:
	            uit = uit_release ;
	            vp = un.release ;
	            break ;
	        case 3:
	            uit = uit_version ;
	            vp = un.version ;
	            break ;
	        case 4:
	            uit = uit_machine ;
	            vp = un.machine ;
	            break ;
	        } /* end switch */
	        if (vp != nullptr) {
	            rs = procinfo_store(pip,uit,vp,-1,nullptr) ;
	        }
	        if (rs < 0) break ;
	    } /* end for */
	} /* end if (uname) */

#if	CF_DEBUGS
	debugprintf("procinfo_uname: ret rs=%d\n",rs) ;
#endif

	return rs ;
}
/* end subroutine (procinfo_uname) */
#endif /* CF_UNAME */


#ifdef	COMMENT
static int checknul(emptyp,epp)
cchar	*emptyp ;
cchar	**epp ;
{

	if (*epp == nullptr)
	    *epp = emptyp ;

	return 0 ;
}
/* end subroutine (checknul) */


static int empty(cp)
cchar	*cp ;
{

	return ((cp == nullptr) || (cp[0] == '\0')) ;
}
/* end subroutine (empty) */
#endif /* COMMENT */


#if	USERINFO_SYSV
#else /* USERINFO_SYSV */
static int getostype()
{
	int		rc = -1 ;
	cchar	*cp ;

/* first try straight-out (fastest) */

	if ((cp = getenv(VAROSTYPE)) != nullptr) {
	    rc = (strcasecmp(cp,"SYSV") == 0) ;
	}

/* next try checking if the OS name and number mean something (pretty fast) */

	if ((rc < 0) && ((cp = getenv(VAROSNAME)) != nullptr)) {
	    int	v ;

	    if ((strcmp(cp,"SunOS") == 0) &&
	        ((cp = getenv(VAROSNUM)) != nullptr)) {

	        if (cfdeci(cp,-1,&v) >= 0) {
	            rc = (v >= 5) ;
		}

	    } /* end if */

	} /* end if (check for "SunOS") */

/* finally (if necessary) 'stat(2)' the i-node (slow disk access) */

	if (rc < 0)
	    rc = (u_access(SBINDNAME,X_OK) >= 0) ;

	return (rc >= 0) ? rc : FALSE ;
}
/* end subroutine (getostype) */
#endif /* USERINFO_SYSV */


#if	CF_OLDUSERINFO
int userinfo_data(USERINFO *oup,char *ubuf,int ulen,cchar *un) noex {
	USERINFO	u ;
	int		rs ;
	int		rs1 ;
	int		size = 0 ;

	if (oup == nullptr) return SR_FAULT ;
	if (ubuf == nullptr) return SR_FAULT ;
	if (ulen < NODENAMELEN) return SR_TOOBIG ;

	memset(ubuf,0,ulen) ;

	if ((rs = userinfo_start(&u,un)) >= 0) {
	    STOREITEM	si ;
	    if ((rs = storeitem_start(&si,ubuf,ulen)) >= 0) {
	        cchar	*sp ;
	        cchar	**rpp ;

	        oup->f = u.f ;
	        oup->pid = u.pid ;
	        oup->uid = u.uid ;
	        oup->euid = u.euid ;
	        oup->gid = u.gid ;
	        oup->egid = u.egid ;

	        for (int i = 0 ; i < uit_overlast ; i += 1) {
	            sp = nullptr ;
	            switch (i) {
	            case uit_sysname:
	                rpp = &oup->sysname ;
	                sp = u.sysname ;
	                break ;
	            case uit_release:
	                rpp = &oup->release ;
	                sp = u.release ;
	                break ;
	            case uit_version:
	                rpp = &oup->version ;
	                sp = u.version ;
	                break ;
	            case uit_machine:
	                rpp = &oup->machine ;
	                sp = u.machine ;
	                break ;
	            case uit_nodename:
	                rpp = &oup->nodename ;
	                sp = u.nodename ;
	                break ;
	            case uit_domainname:
	                rpp = &oup->domainname ;
	                sp = u.domainname ;
	                break ;
	            case uit_username:
	                rpp = &oup->username ;
	                sp = u.username ;
	                break ;
	            case uit_password:
	                rpp = &oup->password ;
	                sp = u.password ;
	                break ;
	            case uit_gecos:
	                rpp = &oup->gecos ;
	                sp = u.gecos ;
	                break ;
	            case uit_homedname:
	                rpp = &oup->homedname ;
	                sp = u.homedname ;
	                break ;
	            case uit_shell:
	                rpp = &oup->shell ;
	                sp = u.shell ;
	                break ;
	            case uit_organization:
	                rpp = &oup->organization ;
	                sp = u.organization ;
	                break ;
	            case uit_gecosname:
	                rpp = &oup->gecosname ;
	                sp = u.gecosname ;
	                break ;
	            case uit_account:
	                rpp = &oup->account ;
	                sp = u.account ;
	                break ;
	            case uit_bin:
	                rpp = &oup->bin ;
	                sp = u.bin ;
	                break ;
	            case uit_office:
	                rpp = &oup->office ;
	                sp = u.office ;
	                break ;
	            case uit_wphone:
	                rpp = &oup->wphone ;
	                sp = u.wphone ;
	                break ;
	            case uit_hphone:
	                rpp = &oup->hphone ;
	                sp = u.hphone ;
	                break ;
	            case uit_printer:
	                rpp = &oup->printer ;
	                sp = u.printer ;
	                break ;
	            case uit_realname:
	                rpp = &oup->realname ;
	                sp = u.realname ;
	                break ;
	            case uit_mailname:
	                rpp = &oup->mailname ;
	                sp = u.mailname ;
	                break ;
	            case uit_fullname:
	                rpp = &oup->fullname ;
	                sp = u.fullname ;
	                break ;
	            case uit_name:
	                rpp = &oup->name ;
	                sp = u.name ;
	                break ;
	            case uit_groupname:
	                rpp = &oup->groupname ;
	                sp = u.groupname ;
	                break ;
	            case uit_project:
	                rpp = &oup->project ;
	                sp = u.project ;
	                break ;
	            case uit_tz:
	                rpp = &oup->tz ;
	                sp = u.tz ;
	                break ;
	            case uit_md:
	                rpp = &oup->md ;
	                sp = u.md ;
	                break ;
	            case uit_wstation:
	                rpp = &oup->wstation ;
	                sp = u.wstation ;
	                break ;
	            case uit_logid:
	                rpp = &oup->logid ;
	                sp = u.logid ;
	                break ;
	            } /* end switch */
	            if (sp) {
			rs = storeitem_strw(&si,sp,-1,rpp) ;
		    }
	            if (rs < 0) break ;
	        } /* end for */
	        if (rs >= 0) {
		    oup->magic = USERINFO_MAGIC ;
		}
	        size = storeitem_finish(&si) ;
	        if (rs >= 0) rs = size ;
	    } /* end if (storeitem) */
	    rs1 = userinfo_finish(&u) ;
	    if (rs >= 0) rs = rs1 ;
	    if (rs < 0) oup->magic = 0 ;
	} /* end if (userinfo) */

	return (rs >= 0) ? size : rs ;
}
#endif /* CF_OLDUSERINFO */


