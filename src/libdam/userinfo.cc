/* userinfo SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* get user information from various databases */
/* version %I% last-modified %G% */

#define	CF_OLDUSERINFO	0		/* compile-in old |userinfo(3dam)| */
#define	CF_UCPWCACHE	0		/* use |ucpwcache(3uc)| */

/* revision history:

	= 1997-07-01, David A­D­ Morano
	This code was originally written.

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

/* Copyright © 1997,2005,2008 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	userinfo

	Description:
	Get user information from PASSWD database.

	Synopsis:
	int userinfo_start(UI *uip,cchar *username) noex

	Arguments:
	uip		address of 'userinfo' structure
	username	optional username

	Returns:
	>=0		success
	<0		failure (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>		/* POSIX */
#include	<sys/param.h>		/* POSIX */
#include	<sys/utsname.h>		/* POSIX */
#include	<unistd.h>		/* POSIX */
#include	<fcntl.h>		/* POSIX */
#include	<netdb.h>		/* POSIX */
#include	<pwd.h>			/* POSIX */
#include	<grp.h>			/* POSIX */
#include	<strings.h>		/* BSD |strcasecmp(3c)| */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<algorithm>		/* C++STD |min(3c++)| + |max(3c++)| */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<uclibmem.h>		/* LIBUC */
#include	<ucgetpid.h>		/* LIBUC */
#include	<ucpwcache.h>		/* LIBUC |ucpwcache_name(3uc)| */
#include	<ucinfo.h>		/* LIBUC */
#include	<getnodedomain.h>	/* LIBUC */
#include	<getnodename.h>		/* LIBUC */
#include	<getusername.h>		/* LIBUC */
#include	<getprojname.h>		/* LIBUC |getprojname(3uc)| */
#include	<getostype.h>		/* LIBUC */
#include	<getax.h>		/* LIBUC */
#include	<getpwx.h>		/* LIBUC */
#include	<bufsizeget.h>		/* LIBUC */
#include	<userattrdb.h>		/* LIBUC */
#include	<gecos.h>		/* LIBUC */
#include	<bits.h>		/* LIBUC */
#include	<strstore.h>		/* LIBUC */
#include	<storeitem.h>		/* LIBUC */
#include	<filereadln.h>		/* LIBUC */
#include	<strn.h>		/* LIBUC */
#include	<sncpyx.h>		/* LIBUC */
#include	<snwcpyx.h>		/* LIBUC */
#include	<mkx.h>			/* LIBUC */
#include	<mklogid.h>		/* LIBUC */
#include	<cfdec.h>		/* LIBUC */
#include	<ctdec.h>		/* LIBUC */
#include	<char.h>		/* LIBUC */
#include	<isnot.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU |DIGBUFLEN| + |REALNAMELEN| */

#include	"userinfo.h"

#pragma		GCC dependency		"mod/libutil.ccm"
#pragma		GCC dependency		"mod/uconstants.ccm"

import libutil ;			/* |memclear(3u)| */
import uconstants ;			/* |sysword(3u)| */

/* local defines */

#define	PCI		procinfo
#define	PCI_FL		procinfo_flags
#define	PCI_TMPS	procinfo_tmps

#define	PRNAME		"LOCAL"		/* program-root for USERATTR */

#ifndef	LOGIDLEN
#define	LOGIDLEN	15		/* log ID length */
#endif

#ifndef	PROJNAMELEN
#define	PROJNAMELEN	100		/* is this enough? */
#endif

#ifndef	WSLEN
#define	WSLEN		15		/* weather-station (abbr) length */
#endif

#define	DEFHOMEDNAME	sysword.w_tmpdir
#define	SBINDNAME	"/usr/sbin"

#ifndef	ORGCNAME
#define	ORGCNAME	"organization"
#endif

#ifndef	VARBIN
#define	VARBIN		"PRINTERBIN"
#endif

#ifndef	VAROFFICE
#define	VAROFFICE	"OFFICE"
#endif

#ifndef	VARWSTATION
#define	VARWSTATION	"WSTATION"
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

#define	UI		userinfo

#define	NDF		"/tmp/userinfo.nd"


/* imported namespaces */

using std::min ;			/* subroutine-template */
using std::max ;			/* subroutine-template */


/* local typedefs */


/* external subroutines */


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
} ; /* end enum (uits) */

struct procinfo_flags {
	uint		altuser:1 ;
	uint		pw:1 ;
	uint		ua:1 ;
	uint		uainit:1 ;
	uint		allocua:1 ;
} ; /* end struct */

struct procinfo_tmps {
	cchar		*gecosname ;
	cchar		*realname ;
} ; /* end struct */

struct procinfo {
	PCI_TMPS	tstrs ;
	PASSWD		pw ;
	bits		have ;
	ucinfo_names	unixinfo ;
	userinfo	*uip ;		/* supplied argument */
	strstore	*stp ;		/* supplied argument */
	int		*sis ;		/* supplied argument */
	cchar		*username ;	/* supplied argument */
	char		*a ;		/* the memory allocation */
	userattrdb	*uap ;
	char		*pwbuf ;	/* specially allocated */
	char		*nodename ;	/* allocated */
	char		*domainname ;	/* allocated */
	char		*pr ;		/* allocated */
	char		*tbuf ;
	PCI_FL	fl ;
	int		pwlen ;
	int		tlen ;
} ; /* end struct */

namespace {
    struct vars {
	int		maxnamelen ;
	int		maxpathlen ;
	int		nodenamelen ;
	int		hostnamelen ;
	int		projnamelen ;
	int		pwlen ;
	int mkvars() noex ;
    } ; /* end struct (vars) */
} /* end namespace */


/* forward references */

template<typename ... Args>
local int userinfo_ctor(userinfo *op,Args ... args) noex {
    	USERINFO	*hop = op ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    rs = memclear(hop) ;
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (userinfo_ctor) */

local int userinfo_dtor(userinfo *op) noex {
	int		rs = SR_FAULT ;
	if (op) ylikely {
	    rs = SR_OK ;
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (userinfo_dtor) */

template<typename ... Args>
local inline int userinfo_magic(userinfo *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    rs = (op->magval == USERINFO_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
} /* end subroutine (userinfo_magic) */

local int	userinfo_process(UI *,strstore *,int *,cchar *) noex ;
local int	userinfo_id(UI *uip) noex ;
local int	userinfo_load(UI *,strstore *,int *) noex ;

#ifdef	COMMENT
local int	userinfo_setnuls(UI *,cchar *) noex ;
#endif

local int	procinfo_start(PCI *,UI *,strstore *,int *) noex ;
local int	procinfo_finish(PCI *) noex ;
local int 	procinfo_find(PCI *,cchar *) noex ;
local int	procinfo_pwentry(PCI *,cchar *) noex ;
local int	procinfo_getpwuser(PCI *,ucentpw *,char *,int,
			cchar *) noex ;
local int	procinfo_store(PCI *,int,cchar *,int,cchar **) noex ;
local int	procinfo_uabegin(PCI *) noex ;
local int	procinfo_uaend(PCI *) noex ;
local int	procinfo_ualookup(PCI *,char *,int,cchar *) noex ;

/* components */
local int	procinfo_homedname(PCI *) noex ;
local int	procinfo_shell(PCI *) noex ;
local int	procinfo_org(PCI *) noex ;
local int	procinfo_bin(PCI *) noex ;
local int	procinfo_office(PCI *) noex ;
local int	procinfo_printer(PCI *) noex ;
local int	procinfo_gecos(PCI *) noex ;
local int	procinfo_gecosname(PCI *) noex ;
local int	procinfo_realname(PCI *) noex ;
local int	procinfo_mailname(PCI *) noex ;
local int	procinfo_name(PCI *) noex ;
local int	procinfo_fullname(PCI *) noex ;
local int	procinfo_uinfo(PCI *) noex ;
local int	procinfo_nodename(PCI *) noex ;
local int	procinfo_domainname(PCI *) noex ;
local int	procinfo_project(PCI *) noex ;
local int	procinfo_tz(PCI *) noex ;
local int	procinfo_md(PCI *) noex ;
local int	procinfo_wstation(PCI *) noex ;
local int	procinfo_logid(PCI *) noex ;


/* local variables */

/* order here is generally (quite) important */
constexpr int		(*components[])(PCI *) = {
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
	procinfo_uinfo,
	procinfo_nodename,
	procinfo_domainname,
	procinfo_project,
	procinfo_tz,
	procinfo_md,
	procinfo_wstation,
	procinfo_logid,
	nullptr
} ; /* end array */

static vars		var ;


/* exported variables */


/* exported subroutines */

int userinfo_start(UI *uip,cchar *un) noex {
	cint		startsize = (USERINFO_LEN/4) ;
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		len = 0 ;
	if ((rs = userinfo_ctor(uip)) >= 0) ylikely {
	    if (static cint rsv = var.mkvars() ; (rs = rsv) >= 0) ylikely {
		static cchar	*nn = getenv(varname.node) ;
	        uip->nodename = nn ;
	        if ((rs = userinfo_id(uip)) >= 0) ylikely {
	            cint	sz = (uit_overlast * szof(cchar *)) ;
	            if (void *vp ; (rs = lm_call(1,sz,&vp)) >= 0) ylikely {
	                strstore	st ;
	                int		*sis = cast_static<int *>(vp) ;
	                if ((rs = strstore_start(&st,10,startsize)) >= 0) {
	                    if ((rs = userinfo_process(uip,&st,sis,un)) >= 0) {
	                        rs = userinfo_load(uip,&st,sis) ;
	                        len = rs ;
	                    } /* end if */
	                    rs1 = strstore_finish(&st) ;
	                    if (rs >= 0) rs = rs1 ;
	                } /* end if (strstore) */
	                rs1 = lm_free(vp) ;
	                if (rs >= 0) rs = rs1 ;
	            } /* end if (memory_allocation) */
	            if (rs >= 0) uip->magval = USERINFO_MAGIC ;
	        } /* end if (userinfo_id) */
	    } /* end if (vars::mkvars) */
	    if (rs < 0) {
		userinfo_dtor(uip) ;
	    } /* end if (error) */
	} /* end if (userinfo_ctor) */
	return (rs >= 0) ? len : rs ;
} /* end subroutine (userinfo_start) */

int userinfo_finish(UI *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = userinfo_magic(op)) >= 0) ylikely {
	    if (op->a) ylikely {
	        rs1 = lm_free(op->a) ;
	        if (rs >= 0) rs = rs1 ;
	        op->a = nullptr ;
	    } /* end if (memory-release) */
	    {
	        rs1 = userinfo_dtor(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    op->magval = 0 ;
	} /* end if (magic) */
	return rs ;
} /* end subroutine (userinfo_finish) */


/* local subroutines */

local int userinfo_process(UI *uip,strstore *stp,int *sis,cchar *un) noex {
	int		rs ;
	int		rs1 ;
	int		rv = 0 ;
	if (PCI pi ; (rs = procinfo_start(&pi,uip,stp,sis)) >= 0) ylikely {
	    {
	        rs = procinfo_find(&pi,un) ;
		rv = rs ;
	    }
	    rs1 = procinfo_finish(&pi) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (procinfo) */
	return (rs >= 0) ? rv : rs ;
} /* end subroutine (userinfo_process) */

local int userinfo_id(UI *uip) noex {
	int		rs ;
	if ((rs = ucpid) >= 0) {
	    uip->pid = rs ;
	    uip->uid = getuid() ;
	    uip->euid = geteuid() ;
	    uip->gid = getgid() ;
	    uip->egid = getegid() ;
	    if ((rs = ostype) >= 0) {
	        uip->ostype = rs ;
	    }
	} /* end if (ucpid) */
	return rs ;
} /* end subroutine (userinfo_id) */

local int userinfo_load(UI *uip,strstore *stp,int *sis) noex {
	int		rs ;
	int		sz = 0 ;
	if ((rs = strstore_strsize(stp)) >= 0) ylikely {
	    sz = rs ;
	    if (char *a ; (rs = lm_mall(sz,&a)) >= 0) ylikely {
	        if ((rs = strstore_strmk(stp,a,sz)) >= 0) ylikely {
		    for (int i = 0 ; i < uit_overlast ; i += 1) {
	                cchar	**vpp = nullptr ;
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
	                    vpp = &uip->projname ;
	                    break ;
	                case uit_tz:
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
	                if ((rs >= 0) && (*vpp == nullptr)) {
			    *vpp = (a + sis[i]) ;
			}
	            } /* end for */
	        } /* end if */
	        if (rs >= 0) {
	            uip->a = a ;
	        } else {
	            lm_free(a) ;
		}
	    } /* end if (memory-acquire) */
	} /* end if */
	return (rs >= 0) ? sz : rs ;
} /* end subroutine (userinfo_load) */

local int procinfo_start(PCI *pip,UI *uip,strstore *stp,int *sis) noex {
	cint		pwlen = var.pwlen ;
	int		rs ;
	int		sz = 0 ;
	{
	    memclear(pip) ;		/* <- noted */
	    pip->uip = uip ;
	    pip->stp = stp ;
	    pip->sis = sis ;
	    pip->tlen = max(pwlen,var.maxpathlen) ;
	} /* end block */
	sz += szof(userattrdb) ;
	sz += var.nodenamelen ;
	sz += var.hostnamelen ;
	sz += var.maxpathlen ;
	sz += pip->tlen ;
	if (char *bp ; (rs = lm_mall(sz,&bp)) >= 0) ylikely {
	    int		bl = 0 ;
	    pip->a = bp ;
	    pip->uap = (userattrdb *) (bp+bl) ;
	    bl += szof(userattrdb) ;
	    pip->nodename = (bp+bl) ;
	    bl += var.nodenamelen ;
	    pip->domainname = (bp+bl) ;
	    bl += var.hostnamelen ;
	    pip->pr = (bp+bl) ;
	    bl += var.maxpathlen ;
	    pip->tbuf = (bp+bl) ;
	    bl += pip->tlen ;
	    if ((rs = bits_start(&pip->have,uit_overlast)) >= 0) ylikely {
	        pip->nodename[0] = '\0' ;
	        pip->domainname[0] = '\0' ;
	        pip->pr[0] = '\0' ;
	        pip->tbuf[0] = '\0' ;
	    } else {
	        lm_free(bp) ;
	        pip->a = nullptr ;
	    }
	} /* end if */
	return rs ;
} /* end subroutine (procinfo_start) */

local int procinfo_finish(PCI *pip) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	{
	    rs1 = procinfo_uaend(pip) ;
	    if (rs >= 0) rs = rs1 ;
	}
	if (pip->pwbuf) ylikely {
	    rs1 = lm_free(pip->pwbuf) ;
	    if (rs >= 0) rs = rs1 ;
	    pip->pwbuf = nullptr ;
	    pip->pwlen = 0 ;
	} /* end if (memory-release) */
	{
	    rs1 = bits_finish(&pip->have) ;
	    if (rs >= 0) rs = rs1 ;
	}
	if (pip->a) {
	    rs1 = lm_free(pip->a) ;
	    if (rs >= 0) rs = rs1 ;
	    pip->a = nullptr ;
	} /* end if (memory-release) */
	return rs ;
} /* end subroutine (procinfo_finish) */

local int procinfo_store(PCI *pip,int uit,cc *vp,int vl,cc **rpp) noex {
	strstore	*stp = pip->stp ;
	int		rs = SR_OK ;
	if (uit >= 0) {
	    if (pip->sis[uit] == 0) {
	        rs = strstore_store(stp,vp,vl,rpp) ;
	        pip->sis[uit] = rs ;
	        if (rs >= 0) {
	            rs = bits_set(&pip->have,uit) ;
		}
	    }
	} else {
	    rs = SR_BUGCHECK ;
	}
	if (rpp && (rs < 0)) {
	    *rpp = nullptr ;
	} /* end if (error) */
	return rs ;
} /* end subroutine (procinfo_store) */

local int procinfo_uabegin(PCI *pip) noex {
	int		rs = SR_OK ;
	if ((! pip->fl.ua) && pip->fl.pw && (! pip->fl.uainit)) {
	    pip->fl.uainit = true ;
	    if (pip->uap == nullptr) {
	        cint	sz = szof(userattrdb) ;
	        if (void *vp ; (rs = lm_mall(sz,&vp)) >= 0) {
	            pip->uap = (userattrdb *) vp ;
	            pip->fl.allocua = true ;
	        }
	    }
	    if (rs >= 0) {
	        cchar	*un = pip->pw.pw_name ;
	        if ((rs = userattrdb_open(pip->uap,un)) >= 0) {
	            pip->fl.ua = true ;
		} else {
		    {
	                lm_free(pip->uap) ;
	                pip->uap = nullptr ;
	                pip->fl.allocua = false ;
		    } /* end if (memory-release) */
		    if (isNotPresent(rs)) rs = SR_OK ;
		}
	    } /* end if (ok) */
	} /* end if (f.ua) */
	return rs ;
} /* end subroutine (procinfo_uabegin) */

local int procinfo_uaend(PCI *pip) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (pip->fl.ua) ylikely {
	    pip->fl.ua = false ;
	    rs1 = userattrdb_close(pip->uap) ;
	    if (rs >= 0) rs = rs1 ;
	}
	if (pip->fl.allocua && (pip->uap != nullptr)) {
	    rs1 = lm_free(pip->uap) ;
	    pip->uap = nullptr ;
	    pip->fl.allocua = false ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (memory-release) */
	return rs ;
} /* end subroutine (procinfo_uaend) */

local int procinfo_ualookup(PCI *pip,char *rbuf,int rlen,cc *kn) noex {
	int		rs = SR_FAULT ;
	if (pip && rbuf && kn) ylikely {
	    rbuf[0] = '\0' ;
	    rs = SR_OK ;
	    if (pip->fl.pw) {
	        if (! pip->fl.ua) {
		    rs = procinfo_uabegin(pip) ;
		}
	        if (rs >= 0) ylikely {
	            rs = userattrdb_lookup(pip->uap,rbuf,rlen,kn) ;
	        }
	    }
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (procinfo_ualookup) */

local int procinfo_find(PCI *pip,cchar *un) noex {
	userinfo	*uip = pip->uip ;
	int		rs ;
	if ((rs = procinfo_pwentry(pip,un)) >= 0) ylikely {
	    cint	dlen = DIGBUFLEN ;
	    int		cl = -1 ;
	    int		uit ;
	    cchar	*cp{} ;
	    char	dbuf[DIGBUFLEN+1] ;
	    if (rs > 0) {
	        cchar	*vp{} ;
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
	        for (int i = 0 ; components[i] ; i += 1) {
	            rs = (*components[i])(pip) ;
	            if (rs < 0) break ;
	        } /* end if */
	    } /* end if */
	} /* end if */
	return rs ;
} /* end subroutine (procinfo_find) */

local int procinfo_pwentry(PCI *pip,cchar *un) noex {
	ucentpw		pw ;
	int		rs ;
	int		pwlen = pip->tlen ;
	int		f = false ;
	char		*pwbuf = pip->tbuf ;
	if ((rs = procinfo_getpwuser(pip,&pw,pwbuf,pwlen,un)) >= 0) {
	    ucentpw	*pwp = cast_static<ucentpw *>(&pw) ;
	    pip->fl.pw = true ;
	    if ((rs = pwp->size()) >= 0) {
	        int	pwsz = rs ;
	        if (char *p ; (rs = lm_mall((pwsz+1),&p)) >= 0) {
	            ucentpw	*tpwp = cast_static<ucentpw *>(&pip->pw) ;
	            if ((rs = tpwp->load(p,pwsz,pwp)) >= 0) {
	                pip->pwbuf = p ;
	                pip->pwlen = pwsz ;
	                f = true ;
	            }
	            if (rs < 0) {
	                lm_free(p) ;
		    } /* end if (error) */
	        } /* end if (memory-acquire) */
	    } /* end if (passwdent_size) */
	} /* end if (procinfo_getpwuser) */
	return (rs >= 0) ? f : rs ;
} /* end subroutine (procinfo_pwentry) */

local int procinfo_getpwuser(PCI *pip,ucentpw *pwp,
		char *pwbuf,int pwlen,cchar *un) noex {
	userinfo	*uip = pip->uip ;
	int		rs ;
	if ((un != nullptr) && (un[0] != '-') && (un[0] != '\0')) {
	    if ((rs = getpwx_name(pwp,pwbuf,pwlen,un)) >= 0) {
	        pip->fl.altuser = (pwp->pw_uid != uip->uid) ;
	        if (pip->fl.altuser) {
	            uip->uid = pwp->pw_uid ;
	            uip->gid = pwp->pw_gid ;
	        }
	    } /* end if (getpwx_name) */
	} else {
	    rs = getpwusername(pwp,pwbuf,pwlen,uip->uid) ;
	}
	return rs ;
} /* end subroutine (procinfo_getpwuser) */

local int procinfo_homedname(PCI *pip) noex {
	userinfo	*uip = pip->uip ;
	int		rs = SR_OK ;
	int		uit = uit_homedname ;
	if ((pip->sis[uit] == 0) && (uip->homedname == nullptr)) {
	    cchar	*vp = nullptr ;
	    if ((vp == nullptr) || (vp[0] == '\0')) {
	        if (! pip->fl.altuser) {
	            vp = getenv(varname.home) ;
	        }
	    }
	    if ((vp == nullptr) || (vp[0] == '\0')) {
	        vp = getenv(varname.tmpdir) ;
	    }
	    if ((vp == nullptr) || (vp[0] == '\0')) {
	        vp = sysword.w_tmpdir ;
	    }
	    uip->homedname = vp ;
	}
	return rs ;
} /* end subroutine (procinfo_homedname) */

local int procinfo_shell(PCI *pip) noex {
	userinfo	*uip = pip->uip ;
	int		rs = SR_OK ;
	int		uit = uit_shell ;
	if ((pip->sis[uit] == 0) && (uip->shell == nullptr)) {
	    cchar	*vp = nullptr ;
	    if ((vp == nullptr) || (vp[0] == '\0')) {
	        if (! pip->fl.altuser) {
	            vp = getenv(varname.shell) ;
	        }
	    }
	    if (vp && (vp[0] != '\0')) {
	        uip->shell = vp ;
	    }
	}
	return rs ;
} /* end subroutine (procinfo_shell) */

local int procinfo_org(PCI *pip) noex {
	userinfo	*uip = pip->uip ;
	cint		orglen = var.maxnamelen ;
	int		rs = SR_OK ;
	int		rs1 ;
	int		uit = uit_organization ;
	if ((pip->sis[uit] == 0) && (uip->organization == nullptr)) {
	    cint	rlen = orglen ;
	    int		vl = -1 ;
	    cchar	*orgcname = ORGCNAME ;
	    cchar	*vp = nullptr ;
	    char	rbuf[orglen+1] ;
	    if (char *orgfname ; (rs = lm_mp(&orgfname)) >= 0) ylikely {
	        if ((vp == nullptr) || (vp[0] == '\0')) {
	            if (! pip->fl.altuser) {
	                vp = getenv(varname.organization) ;
	            }
	        }
	        if ((vp == nullptr) || (vp[0] == '\0')) {
	            char	cname[orglen+1] ;
	            rs = sncpy2(cname,orglen,".",orgcname) ;
	            if (rs >= 0) {
	                cchar	*hd = uip->homedname ;
	                if ((hd == nullptr) && pip->fl.pw) {
	                    hd = pip->pw.pw_dir ;
		        }
	                if (hd == nullptr) hd = DEFHOMEDNAME ;
	                rs = mkpath2(orgfname,hd,cname) ;
	            }
	            if (rs >= 0) {
	                rs = filereadln(orgfname,rbuf,rlen) ;
	                vl = rs ;
	                if (rs > 0) vp = rbuf ;
	                if (isNotPresent(rs)) rs = SR_OK ;
	            }
	        } /* end if */
	        if (rs >= 0) {
	            if ((vp != nullptr) && (vp[0] != '\0')) {
	                rs = procinfo_store(pip,uit,vp,vl,nullptr) ;
		    }
	        } /* end if (ok) */
		rs1 = lm_free(orgfname) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (m-a-f) */
	} /* end if */
	return rs ;
} /* end subroutine (procinfo_org) */

local int procinfo_bin(PCI *pip) noex {
	userinfo	*uip = pip->uip ;
	int		rs = SR_OK ;
	int		uit = uit_bin ;
	if ((pip->sis[uit] == 0) && (uip->bin == nullptr)) {
	    cchar	*vp = nullptr ;
	    if ((vp == nullptr) || (vp[0] == '\0')) {
	        if (! pip->fl.altuser) {
	            vp = getenv(VARBIN) ;
	        }
	    }
	    if ((vp != nullptr) && (vp[0] != '\0')) {
	        uip->bin = vp ;
	    }
	}
	return rs ;
} /* end subroutine (procinfo_bin) */

local int procinfo_office(PCI *pip) noex {
	userinfo	*uip = pip->uip ;
	int		rs = SR_OK ;
	int		uit = uit_office ;
	if ((pip->sis[uit] == 0) && (uip->office == nullptr)) {
	    cchar	*vp = nullptr ;
	    if ((vp == nullptr) || (vp[0] == '\0')) {
	        if (! pip->fl.altuser) {
	            vp = getenv(VAROFFICE) ;
	        }
	    }
	    if ((vp != nullptr) && (vp[0] != '\0')) {
	        uip->office = vp ;
	    }
	}
	return rs ;
} /* end subroutine (procinfo_office) */

local int procinfo_printer(PCI *pip) noex {
	userinfo	*uip = pip->uip ;
	int		rs = SR_OK ;
	int		uit = uit_printer ;
	if ((pip->sis[uit] == 0) && (uip->printer == nullptr)) {
	    cchar	*vp = nullptr ;
	    if ((vp == nullptr) || (vp[0] == '\0')) {
	        if (! pip->fl.altuser) {
	            vp = getenv(varname.printer) ;
	        }
	    }
	    if ((vp != nullptr) && (vp[0] != '\0')) {
	        uip->printer = vp ;
	    }
	}
	return rs ;
} /* end subroutine (procinfo_printer) */

/* parse out the GECOS field */
local int procinfo_gecos(PCI *pip) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	cchar		*gstr = pip->pw.pw_gecos ;
	if (pip->fl.pw && (gstr != nullptr) && (gstr[0] != '\0')) {
	    gecos	g ;
	    if ((rs = gecos_start(&g,gstr,-1)) >= 0) {
	        userinfo	*uip = pip->uip ;
	        bits		*bip = &pip->have ;
	        for (int i = 0 ; i < gecosval_overlast ; i += 1) {
	            cchar	*vp{} ;
	            if (int vl ; (vl = gecos_getval(&g,i,&vp)) >= 0) {
	                int	uit = -1 ;
	                cchar	*up = nullptr ;
	                switch (i) {
	                case gecosval_organization:
	                    uit = uit_organization ;
	                    up = uip->organization ;
	                    if ((bits_test(bip,uit) > 0) || (up != nullptr)) {
	                        vp = nullptr ;
			    }
	                    break ;
	                case gecosval_realname:
	                    uit = uit_gecosname ;
	                    up = uip->gecosname ;
	                    if ((bits_test(bip,uit) == 0) && (up == nullptr)) {
	                        char	*nbuf ;
	                        if ((rs = lm_mall((vl+1),&nbuf)) >= 0) {
	                            if (strnchr(vp,vl,'_') != nullptr) {
	                                rs = snwcpyhyphen(nbuf,-1,vp,vl) ;
	                                vp = nbuf ;
	                            }
	                            if (rs >= 0) {
	                                if (pip->sis[uit] == 0) {
	                                    strstore	*stp = pip->stp ;
	                                    cchar	*cp{} ;
	                                    rs = strstore_store(stp,vp,vl,&cp) ;
	                                    pip->sis[uit] = rs ;
	                                    pip->tstrs.gecosname = cp ;
	                                }
	                                vp = nullptr ;
	                            } /* end if (ok) */
	                            rs1 = lm_free(nbuf) ;
				    if (rs >= 0) rs = rs1 ;
	                        } /* end if (memory-acquire) */
	                    } else {
	                        vp = nullptr ;
			    }
	                    break ;
	                case gecosval_account:
	                    uit = uit_account ;
	                    up = uip->account ;
	                    if ((bits_test(bip,uit) > 0) || (up != nullptr)) {
	                        vp = nullptr ;
			    }
	                    break ;
	                case gecosval_bin:
	                    uit = uit_bin ;
	                    up = uip->bin ;
	                    if ((bits_test(bip,uit) > 0) || (up != nullptr)) {
	                        vp = nullptr ;
			    }
	                    break ;
	                case gecosval_office:
	                    uit = uit_office ;
	                    up = uip->office ;
	                    if ((bits_test(bip,uit) > 0) || (up != nullptr)) {
	                        vp = nullptr ;
			    }
	                    break ;
	                case gecosval_wphone:
	                    uit = uit_wphone ;
	                    up = uip->hphone ;
	                    if ((bits_test(bip,uit) > 0) || (up != nullptr)) {
	                        vp = nullptr ;
			    }
	                    break ;
	                case gecosval_hphone:
	                    uit = uit_hphone ;
	                    up = uip->hphone ;
	                    if ((bits_test(bip,uit) > 0) || (up != nullptr)) {
	                        vp = nullptr ;
			    }
	                    break ;
	                case gecosval_printer:
	                    uit = uit_printer ;
	                    up = uip->printer ;
	                    if ((bits_test(bip,uit) > 0) || (up != nullptr)) {
	                        vp = nullptr ;
			    }
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
} /* end subroutine (procinfo_gecos) */

local int procinfo_gecosname(PCI *pip) noex {
	userinfo	*uip = pip->uip ;
	int		rs = SR_OK ;
	int		uit = uit_gecosname ;
	if ((pip->sis[uit] == 0) && (uip->gecosname == nullptr)) {
	    cchar	*gstr = pip->pw.pw_gecos ;
	    if ((gstr != nullptr) && (gstr[0] != '\0')) {
	        cint	nlen = REALNAMELEN ;
	        int	vl ;
	        cchar	*vp ;
	        char	nbuf[REALNAMELEN+1] ;
	        if ((vl = mkgecosname(nbuf,nlen,gstr)) > 0) {
	            vp = nbuf ;
	            rs = procinfo_store(pip,uit,vp,vl,nullptr) ;
	        }
	    }
	}
	return rs ;
} /* end subroutine (procinfo_gecosname) */

local int procinfo_realname(PCI *pip) noex {
	userinfo	*uip = pip->uip ;
	int		rs = SR_OK ;
	int		uit = uit_realname ;
	if ((pip->sis[uit] == 0) && (uip->realname == nullptr)) {
	    cint	nlen = REALNAMELEN ;
	    cchar	*sp = pip->tstrs.gecosname ;
	    char	nbuf[REALNAMELEN+1] ;
	    if ((sp == nullptr) || (sp[0] == '\0')) {
	        if (! pip->fl.altuser) {
	            sp = getenv(varname.name) ;
	        }
	    }
	    if ((sp != nullptr) && (sp[0] != '\0')) {
	        int	vl ;
	        cchar	*vp = nbuf ;
	        if ((vl = mkrealname(nbuf,nlen,sp,-1)) > 0) {
	            strstore	*stp = pip->stp ;
	            cchar	*cp ;
	            rs = strstore_store(stp,vp,vl,&cp) ;
	            pip->sis[uit] = rs ;
	            pip->tstrs.realname = cp ;
	        }
	    }
	} /* end if (needed) */
	return rs ;
} /* end subroutine (procinfo_realname) */

local int procinfo_mailname(PCI *pip) noex {
	userinfo	*uip = pip->uip ;
	int		rs = SR_OK ;
	int		uit = uit_mailname ;
	if ((pip->sis[uit] == 0) && (uip->mailname == nullptr)) {
	    cint	nlen = REALNAMELEN ;
	    cint	mlen = REALNAMELEN ;
	    int		vl ;
	    int		sl = -1 ;
	    cchar	*vp ;
	    cchar	*sp = nullptr ;
	    char	nbuf[REALNAMELEN+1] ;
	    char	mbuf[REALNAMELEN+1] ;
	    if ((sp == nullptr) || (sp[0] == '\0')) {
	        if (! pip->fl.altuser) {
	            sp = getenv(varname.mailname) ;
	        }
	    }
	    if ((sp == nullptr) || (sp[0] == '\0')) {
	        sp = pip->tstrs.gecosname ;
	    }
	    if ((sp == nullptr) || (sp[0] == '\0')) {
	        if (pip->fl.pw) {
	            cchar	*gn = pip->pw.pw_gecos ;
	            sp = nbuf ;
	            sl = mkgecosname(nbuf,nlen,gn) ;
	        }
	    }
	    if ((sp == nullptr) || (sp[0] == '\0')) {
	        if (! pip->fl.altuser) {
	            sp = getenv(varname.name) ;
		    sl = -1 ;
	        }
	    }
	    if ((sp == nullptr) || (sp[0] == '\0')) {
	        if (pip->fl.pw) {
		    sp = pip->pw.pw_name ;
		    sl = -1 ;
		}
	    }
	    if ((sp != nullptr) && (sp[0] != '\0')) {
	        vp = mbuf ;
	        if ((vl = mkmailname(mbuf,mlen,sp,sl)) > 0) {
	            rs = procinfo_store(pip,uit,vp,vl,nullptr) ;
		}
	    }
	}
	return rs ;
} /* end subroutine (procinfo_mailname) */

local int procinfo_name(PCI *pip) noex {
	userinfo	*uip = pip->uip ;
	int		rs = SR_OK ;
	int		uit = uit_name ;
	if ((pip->sis[uit] == 0) && (uip->name == nullptr)) {
	    cint	glen = REALNAMELEN ;
	    cint	rlen = REALNAMELEN ;
	    int		sl = -1 ;
	    cchar	*sp = nullptr ;
	    char	gbuf[REALNAMELEN+1] ;
	    char	rbuf[REALNAMELEN+1] ;
	    if ((sp == nullptr) || (sp[0] == '\0')) {
	        if (! pip->fl.altuser) {
	            sp = getenv(varname.name) ;
	        }
	    }
	    if ((sp == nullptr) || (sp[0] == '\0')) {
	        sp = pip->tstrs.realname ;
	        if (sp == nullptr) {
	            cchar	*gp = pip->tstrs.gecosname ;
	            int		gl = -1 ;
	            if (gp == nullptr) {
	                cchar	*gn = pip->pw.pw_gecos ;
	                gp = gbuf ;
	                gl = mkgecosname(gbuf,glen,gn) ;
	            }
	            sp = rbuf ;
	            sl = mkrealname(rbuf,rlen,gp,gl) ;
	        }
	    }
	    if ((sp == nullptr) || (sp[0] == '\0')) {
	        if (pip->fl.pw) {
		    sp = pip->pw.pw_name ;
		    sl = -1 ;
		}
	    }
	    if ((sp != nullptr) && (sp[0] != '\0')) {
	        rs = procinfo_store(pip,uit,sp,sl,nullptr) ;
	    }
	}
	return rs ;
} /* end subroutine (procinfo_name) */

local int procinfo_fullname(PCI *pip) noex {
	userinfo	*uip = pip->uip ;
	int		rs = SR_OK ;
	int		uit = uit_fullname ;
	if ((pip->sis[uit] == 0) && (uip->fullname == nullptr)) {
	    cchar	*sp = nullptr ;
	        if (! pip->fl.altuser) {
	            sp = getenv(varname.fullname) ;
	        }
	    if ((sp != nullptr) && (sp[0] != '\0')) {
	        uip->fullname = sp ;
	    }
	}
	return rs ;
} /* end subroutine (procinfo_fullname) */

local int procinfo_nodename(PCI *pip) noex {
	userinfo	*uip = pip->uip ;
	int		rs = SR_OK ;
	int		uit = uit_nodename ;
	if ((pip->sis[uit] == 0) && (uip->nodename == nullptr)) {
	    cint	nlen = var.nodenamelen ;
	    int		nl = -1 ;
	    char	*nbuf = pip->nodename ;
	    if (nbuf[0] == '\0') {
	        rs = getnodename(nbuf,nlen) ;
	        nl = rs ;
	    }
	    if (rs >= 0) {
	        rs = procinfo_store(pip,uit,nbuf,nl,nullptr) ;
	    }
	}
	return rs ;
} /* end subroutine (procinfo_nodename) */

local int procinfo_domainname(PCI *pip) noex {
	userinfo	*uip = pip->uip ;
	int		rs = SR_OK ;
	int		uit = uit_domainname ;
	if ((pip->sis[uit] == 0) && (uip->domainname == nullptr)) {
	    uip->domainname = nullptr ;
	    if (uip->domainname == nullptr) {
	        if (! pip->fl.altuser) {
		    cnullptr	np{} ;
		    cchar	*valp ; 
		    if ((valp = getenv(varname.localdomain)) != np) {
			valp = getenv(varname.domain) ;
		    }
	            uip->domainname = valp ;
	        } /* end if (getting env-domain) */
	    }
	    if (uip->domainname == nullptr) {
	        cint	dlen = var.hostnamelen ;
	        int	dl = -1 ;
	        char	*dbuf = pip->domainname ;
	        if ((rs >= 0) && (dbuf[0] == '\0')) {
	            if (pip->fl.pw) {
	                cchar	*kn = UA_DN ;
	                rs = procinfo_ualookup(pip,dbuf,dlen,kn) ;
	                if (isNotPresent(rs)) {
	                    dbuf[0] = '\0' ;
	                    rs = SR_OK ;
	                }
	            }
	        }
	        if ((rs >= 0) && (dbuf[0] == '\0')) {
	            rs = getinetdomain(dbuf,dlen) ;
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
	return rs ;
} /* end subroutine (procinfo_domainename) */

local int procinfo_project(PCI *pip) noex {
	userinfo	*uip = pip->uip ;
	cint		dlen = var.projnamelen ;
	int		rs = SR_OK ;
	int		uit = uit_project ;
	if ((pip->sis[uit] == 0) && (uip->projname == nullptr)) {
	    int		dl = -1 ;
	    char	dbuf[dlen+1] = { 0 } ;
	    if ((rs >= 0) && (dbuf[0] == '\0')) {
	        if (pip->fl.pw) {
	            cchar	*kn = UA_PROJECT ;
	            rs = procinfo_ualookup(pip,dbuf,dlen,kn) ;
	            dl = rs ;
	            if (isNotPresent(rs)) {
	                dbuf[0] = '\0' ;
	                rs = SR_OK ;
	            }
	        }
	    }
	    if ((rs >= 0) && (dbuf[0] == '\0')) {
	        if (pip->fl.pw) {
	            cchar	*un = pip->pw.pw_name ;
	            rs = getprojname(dbuf,dlen,un) ;
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
	return rs ;
} /* end subroutine (procinfo_project) */

local int procinfo_tz(PCI *pip) noex {
	userinfo	*uip = pip->uip ;
	int		rs = SR_OK ;
	int		uit = uit_tz ;
	if ((pip->sis[uit] == 0) && (uip->tz == nullptr)) {
	    uip->tz = nullptr ;
	    if (uip->tz == nullptr) {
	        if (! pip->fl.altuser) {
	            uip->tz = getenv(varname.tz) ;
	        }
	    }
	    if (uip->tz == nullptr) {
	        cint	dlen = TZABBRLEN ;
	        int	dl = -1 ;
	        char	dbuf[TZABBRLEN+1] = { 0 } ;
	        if ((rs >= 0) && (dbuf[0] == '\0')) {
	            if (pip->fl.pw) {
	                cchar	*kn = UA_TZ ;
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
	} /* end if (tz) */
	return rs ;
} /* end subroutine (procinfo_tz) */

local int procinfo_md(PCI *pip) noex {
	userinfo	*uip = pip->uip ;
	int		rs = SR_OK ;
	int		rs1 ;
	int		uit = uit_md ;
	if ((pip->sis[uit] == 0) && (uip->md == nullptr)) {
	    uip->md = nullptr ;
	    if (uip->md == nullptr) {
	        if (! pip->fl.altuser) {
	            uip->md = getenv(varname.maildir) ;
	        }
	    }
	    if (uip->md == nullptr) {
	        int	dl = -1 ;
	        if (char *dbuf ; (rs = lm_hostname(&dbuf)) >= 0) {
		    cint	dlen = rs ;
	            if ((rs >= 0) && (dbuf[0] == '\0')) {
	                if (pip->fl.pw) {
	                    cchar	*kn = UA_MD ;
	                    rs = procinfo_ualookup(pip,dbuf,dlen,kn) ;
	                    dl = rs ;
	                    if (isNotPresent(rs)) {
	                        dbuf[0] = '\0' ;
	                        rs = SR_OK ;
	                    }
	                }
	            } /* end if */
	            if ((rs >= 0) && (dbuf[0] != '\0')) {
	                rs = procinfo_store(pip,uit,dbuf,dl,nullptr) ;
	            }
		    rs1 = lm_free(dbuf) ;
		    if (rs >= 0) rs = rs1 ;
		} /* end if (m-a-f) */
	    }
	} /* end if (md) */
	return rs ;
} /* end subroutine (procinfo_md) */

local int procinfo_wstation(PCI *pip) noex {
	userinfo	*uip = pip->uip ;
	int		rs = SR_OK ;
	int		uit = uit_wstation ;
	if ((pip->sis[uit] == 0) && (uip->wstation == nullptr)) {
	    uip->wstation = nullptr ;
	    if (uip->wstation == nullptr) {
	        if (! pip->fl.altuser) {
	            uip->wstation = getenv(VARWSTATION) ;
	        }
	    }
	    if (uip->wstation == nullptr) {
	        cint	dlen = WSLEN ;
	        int	dl = -1 ;
	        char	dbuf[WSLEN+1] = { 0 } ;
	        if ((rs >= 0) && (dbuf[0] == '\0')) {
	            if (pip->fl.pw) {
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
	return rs ;
} /* end subroutine (procinfo_wstation) */

local int procinfo_logid(PCI *pip) noex {
	userinfo	*uip = pip->uip ;
	cnullptr	np{} ;
	int		rs = SR_OK ;
	int		uit = uit_logid ;
	if ((pip->sis[uit] == 0) && (uip->logid == nullptr)) {
	    int		v = uip->pid ;
	    cchar	*nn = uip->nodename ;
	    if ((nn == nullptr) || (nn[0] == '\0')) {
		nn = pip->nodename ;
	    }
	    if ((nn != nullptr) && (nn[0] != '\0')) {
	        cint	llen = LOGIDLEN ;
	        char	lbuf[LOGIDLEN+1] ;
	        if ((rs = mklogid(lbuf,llen,nn,-1,v)) >= 0) {
	            rs = procinfo_store(pip,uit,lbuf,rs,np) ;
	        }
	    }
	}
	return rs ;
} /* end subroutine (procinfo_logid) */

local int procinfo_uinfo(PCI *pip) noex {
	int		rs ;
	if ((rs = ucinfo_name(&pip->unixinfo)) >= 0) {
	    userinfo	*uip = pip->uip ;
	    ucinfo_names	*xip = &pip->unixinfo ;
	    cint	uit = uit_nodename ;
	    uip->sysname = xip->sysname ;
	    uip->release = xip->release ;
	    uip->version = xip->version ;
	    uip->machine = xip->machine ;
	    if ((pip->sis[uit] == 0) && (uip->nodename == nullptr)) {
	        uip->nodename = xip->nodename ;
	    }
	}
	return rs ;
} /* end subroutine (procinfo_uinfo) */

void userinfo::dtor() noex {
	if (cint rs = finish ; rs < 0) {
	    ulogerror("userinfo",rs,"fini-finish") ;
	}
} /* end subroutine (useirnfo::dtor) */

userinfo_cos::operator int () noex {
	return userinfo_start(op,nullptr) ;
}

int userinfo_cos::operator () (cchar *un) noex {
	return userinfo_start(op,un) ;
}

userinfo_co::operator int () noex {
	int		rs = SR_BUGCHECK ;
	if (op) {
	    switch (w) {
	    case userinfomem_finish:
	        rs = userinfo_finish(op) ;
	        break ;
	    } /* end switch */
	} /* end if (non-null) */
	return rs ;
} /* end method (userinfo_co::operator) */

#if	CF_OLDUSERINFO
int userinfo_data(UI *oup,char *ubuf,int ulen,cchar *un) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		sz = 0 ;
	if (oup && ubuf) ylikely {
	    rs = SR_TOOBIG ;
	    if ((ulen < 0) && (ulen >= var.nodenamelen)) {
	        memnset(ubuf,0,ulen) ;
	        if (userinfo u ; (rs = userinfo_start(&u,un)) >= 0) ylikely {
	            if (storeitem si ; (rs = si.start(ubuf,ulen)) >= 0) {
	                cchar	*sp ;
	                cchar	**rpp ;
	                oup->ostype = u.ostype ;
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
	                        rpp = &oup->projname ;
	                        sp = u.projname ;
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
			        rs = si.strw(sp,-1,rpp) ;
		            }
	                    if (rs < 0) break ;
	                } /* end for */
	                if (rs >= 0) {
		            oup->magval = USERINFO_MAGIC ;
		        }
	                sz = si.finish ;
	                if (rs >= 0) rs = sz ;
	            } /* end if (storeitem) */
	            rs1 = userinfo_finish(&u) ;
	            if (rs >= 0) rs = rs1 ;
	            if (rs < 0) {
			oup->magval = 0 ;
		    } /* end if (error) */
	        } /* end if (userinfo) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? sz : rs ;
} /* end subroutine (userinfo_data) */
#endif /* CF_OLDUSERINFO */

int vars::mkvars() noex {
	int		rs ;
	if ((rs = bufsizeget(bufsize_mn)) >= 0) {
	    maxnamelen = rs ;
	    if ((rs = bufsizeget(bufsize_mp)) >= 0) {
	        maxpathlen = rs ;
	        if ((rs = bufsizeget(bufsize_nn)) >= 0) {
	            nodenamelen = rs ;
	            if ((rs = bufsizeget(bufsize_hostname)) >= 0) {
	                hostnamelen = rs ;
	                if ((rs = bufsizeget(bufsize_pn)) >= 0) {
	                    projnamelen = rs ;
	                    if ((rs = bufsizeget(bufsize_pw)) >= 0) {
				pwlen = rs ;
			    }
			}
		    }
		}
	    }
	}
	return rs ;
} /* end method (vars::mkvars) */


