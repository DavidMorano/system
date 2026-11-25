/* userinfo SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* get user information from various databases */
/* version %I% last-modified %G% */

#define	CF_DEBUG	1		/* debugging */
#define	CF_OLDUSERINFO	0		/* compile-in old |userinfo(3dam)| */

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
	I replaced calls to |getusernam(3secdb)| and |userdomain(3dam)|
	by calls to the single object |userattr(3uc)|.  Since the
	old UDOMAIN database is now (quietly) queried by the
	|userattr(3dam)| object if a lookup on the system user-attribute
	database fails for the 'id' keyword, any code here that
	used to call |udomain(3dam)| was just eliminated (deleted).

*/

/* Copyright © 1997,2005,2008 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Object:
	userinfo

	Description:
	Get user information from PASSWD database.

	Synopsis:
	int userinfo_start	(UI *uip,cchar *username) noex
	int userinfo_finish	(UI *uip) noex

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
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<algorithm>		/* C++STD |min(3c++)| + |max(3c++)| */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<uclibmem.h>		/* LIBUC */
#include	<ucgetpid.h>		/* LIBUC */
#include	<ucinfo.h>		/* LIBUC */
#include	<ucent.h>		/* LIBUC */
#include	<getnodedomain.h>	/* LIBUC */
#include	<getnodename.h>		/* LIBUC */
#include	<getusername.h>		/* LIBUC */
#include	<getprojname.h>		/* LIBUC |getprojname(3uc)| */
#include	<getostype.h>		/* LIBUC */
#include	<getax.h>		/* LIBUC */
#include	<getpwx.h>		/* LIBUC */
#include	<bufsizeget.h>		/* LIBUC */
#include	<userattrdb.h>		/* LIBUC */
#include	<user.h>		/* LIBUC */
#include	<gecos.h>		/* LIBUC */
#include	<bits.h>		/* LIBUC */
#include	<strstore.h>		/* LIBUC */
#include	<storeitem.h>		/* LIBUC */
#include	<strn.h>		/* LIBUC */
#include	<sncpyx.h>		/* LIBUC */
#include	<sncpyx.h>		/* LIBUC */
#include	<snxxx.h>		/* LIBUC |snsd(3uc)| */
#include	<snwcpy.h>		/* LIBUC |snwcpyhyphen(3uc)| */
#include	<mkx.h>			/* LIBUC */
#include	<mkpath.h>		/* LIBUC */
#include	<mklogid.h>		/* LIBUC */
#include	<filereadln.h>		/* LIBUC */
#include	<char.h>		/* LIBUC */
#include	<isnot.h>		/* LIBUC */
#include	<vardefs.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU |DIGBUFLEN| + |REALNAMELEN| */
#include	<deb.hh>		/* LIBU |DEBPRINTF(3u)| */

#include	"userinfo.h"

#pragma		GCC dependency		"mod/libutil.ccm"
#pragma		GCC dependency		"mod/uconstants.ccm"
#pragma		GCC dependency		"mod/deb.ccm"

import libutil ;			/* |memclear(3u)| */
import uconstants ;			/* |sysword(3u)| */
import deb ;				/* debugging */

/* local defines */

#ifndef	CF_DEBUG
#define	CF_DEBUG	0		/* debugging */
#endif

#define	UI		userinfo

#define	SMGR		submgr
#define	SMGR_FL		submgr_flags
#define	SMGR_TMPS	submgr_tmps

#define	UAD		userattrdb

#define	PRNAME		"LOCAL"		/* program-root for USERATTR */

#ifndef	USERINFO_LEN
#define	USERINFO_LEN	((3 * 2048) + MAXHOSTNAMELEN)
#endif

#ifndef	LOGIDLEN
#define	LOGIDLEN	15		/* log ID length */
#endif

#ifndef	WSLEN
#define	WSLEN		15		/* weather-station (abbr) length */
#endif

#define	DEFHOMEDNAME	sysword.w_tmpdir
#define	SBINDNAME	"/usr/sbin"

#ifndef	ORGCNAME
#define	ORGCNAME	"organization"
#endif

#ifndef	VARPRBIN
#define	VARPRBIN	"PRINTERBIN"
#endif
#ifndef	VAROFFICE
#define	VAROFFICE	"OFFICE"
#endif
#ifndef	VARWSTATION
#define	VARWSTATION	"WSTATION"
#endif

#define	HOME_NORMNAME	".name"
#define	HOME_FULLNAME	".fullname"
#define	HOME_ORG	".organization"

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
	uit_prbin,
	uit_office,
	uit_wphone,
	uit_hphone,
	uit_printer,
	uit_realname,
	uit_mailname,
	uit_name,
	uit_fullname,
	uit_groupname,
	uit_projname,
	uit_tz,
	uit_md,
	uit_wstation,
	uit_logid,
	uit_overlast
} ; /* end enum (uits) */

struct submgr_flags {
	uint		altuser:1 ;
	uint		pw:1 ;
	uint		uaopen:1 ;
	uint		uamall:1 ;
} ; /* end struct */

struct submgr_tmps {
	cchar		*gecosname ;
	cchar		*realname ;
} ; /* end struct */

struct submgr {
	SMGR_TMPS	tstrs ;
	PASSWD		pw ;
	bits		have ;
	ucinfo_names	unixinfo ;
	userinfo	*uip ;		/* supplied argument */
	strstore	*stp ;		/* supplied argument */
	int		*sis ;		/* supplied argument */
	cchar		*username ;	/* supplied argument */
	cchar		*userhome ;	/* calculated user home directory */
	char		*a ;		/* the memory allocation */
	userattrdb	*uap ;
	char		*pwbuf ;	/* specially allocated */
	char		*nodename ;	/* allocated */
	char		*domainname ;	/* allocated */
	char		*pr ;		/* allocated */
	char		*tbuf ;
	SMGR_FL		fl ;
	int		pwlen ;
	int		tlen ;
	int getpwu	(ucentpw *pwp,ccp un) noex ;
	int rdhomefile	(charp,int,ccharp) noex ;
} ; /* end struct */

namespace {
    struct vars {
	int		maxnamelen ;
	int		maxpathlen ;
	int		nodenamelen ;
	int		hostnamelen ;
	int		projnamelen ;
	int		tznamelen ;
	int		pwlen ;
	operator int () noex ;
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
	} /* end */
	return rs ;
} /* end subroutine (userinfo_magic) */

local int userinfo_proc		(UI *,strstore *,int *,cchar *) noex ;
local int userinfo_id		(UI *uip) noex ;
local int userinfo_load		(UI *,strstore *,int *) noex ;

local int submgr_start		(SMGR *,UI *,strstore *,int *) noex ;
local int submgr_finish		(SMGR *) noex ;
local int submgr_pwext		(SMGR *,cchar *) noex ;
local int submgr_pwentry	(SMGR *,cchar *) noex ;
local int submgr_comps		(SMGR *) noex ;
local int submgr_store		(SMGR *,int,ccp,int,ccpp = nullptr) noex ;
local int submgr_ualookup	(SMGR *,char *,int,cchar *) noex ;
local int submgr_uaready	(SMGR *) noex ;
local int submgr_uaobjbeg	(SMGR *) noex ;
local int submgr_uaobjend	(SMGR *) noex ;
local int submgr_uabeg		(SMGR *) noex ;
local int submgr_uaend		(SMGR *) noex ;

/* components */
local int submgr_homedname	(SMGR *) noex ;
local int submgr_shell		(SMGR *) noex ;
local int submgr_org		(SMGR *) noex ;
local int submgr_prbin		(SMGR *) noex ;
local int submgr_office		(SMGR *) noex ;
local int submgr_printer	(SMGR *) noex ;
local int submgr_gecos		(SMGR *) noex ;
local int submgr_gecosname	(SMGR *) noex ;
local int submgr_realname	(SMGR *) noex ;
local int submgr_mailname	(SMGR *) noex ;
local int submgr_name		(SMGR *) noex ;
local int submgr_fullname	(SMGR *) noex ;
local int submgr_groupname	(SMGR *) noex ;
local int submgr_projname	(SMGR *) noex ;
local int submgr_unixname	(SMGR *) noex ;
local int submgr_nodename	(SMGR *) noex ;
local int submgr_domain		(SMGR *) noex ;
local int submgr_tz		(SMGR *) noex ;
local int submgr_md		(SMGR *) noex ;
local int submgr_wstation	(SMGR *) noex ;
local int submgr_logid		(SMGR *) noex ;

local bool isNoEnt		(int) noex ;


/* local variables */

/* order here is generally (quite) important */
constexpr int		(*components[])(SMGR *) = {
	submgr_homedname,
	submgr_shell,
	submgr_org,
	submgr_prbin,
	submgr_office,
	submgr_printer,
	submgr_gecos,
	submgr_gecosname,
	submgr_realname,
	submgr_mailname,
	submgr_name,
	submgr_fullname,
	submgr_groupname,
	submgr_projname,
	submgr_unixname,
	submgr_nodename,
	submgr_domain,
	submgr_tz,
	submgr_md,
	submgr_wstation,
	submgr_logid,
	nullptr
} ; /* end array */

static vars		var ;
cbool			f_debug		= CF_DEBUG ;


/* exported variables */


/* exported subroutines */

int userinfo_start(UI *uip,cchar *un) noex {
	cint		startsz = (USERINFO_LEN/4) ;
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		len = 0 ;
	DEBPRINTF("ent\n") ;
	if ((rs = userinfo_ctor(uip)) >= 0) ylikely {
	    if (static cint rsv = var ; (rs = rsv) >= 0) ylikely {
		static cchar	*v_node = getenver(varname.node) ;
	        uip->nodename = v_node ;
	        if ((rs = userinfo_id(uip)) >= 0) ylikely {
	            cint	sz = (uit_overlast * szof(cchar *)) ;
	            if (void *vp ; (rs = lm_call(1,sz,&vp)) >= 0) ylikely {
			cint	sn = 10 ;
	                int	*sis = cast_static<int *>(vp) ;
	                strstore st ;
	                if ((rs = strstore_start(&st,sn,startsz)) >= 0) {
	                    if ((rs = userinfo_proc(uip,&st,sis,un)) >= 0) {
	                        rs = userinfo_load(uip,&st,sis) ;
	                        len = rs ;
	                    } /* end if */
	                    rs1 = strstore_finish(&st) ;
	                    if (rs >= 0) rs = rs1 ;
	                } /* end if (strstore) */
	                rs1 = lm_free(vp) ;
	                if (rs >= 0) rs = rs1 ;
	            } /* end if (memory_allocation) */
	            if (rs >= 0) {
			uip->magval = USERINFO_MAGIC ;
		    } /* end if (ok) */
	        } /* end if (userinfo_id) */
	    } /* end if (vars::operator) */
	    if (rs < 0) {
		userinfo_dtor(uip) ;
	    } /* end if (error) */
	} /* end if (userinfo_ctor) */
	DEBPRINTF("ret rs=%d len=%d\n",rs,len) ;
	return (rs >= 0) ? len : rs ;
} /* end subroutine (userinfo_start) */

int userinfo_finish(UI *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = userinfo_magic(op)) >= 0) ylikely {
	    if (op->allocinfo) ylikely {
	        rs1 = lm_free(op->allocinfo) ;
	        if (rs >= 0) rs = rs1 ;
	        op->allocinfo = nullptr ;
	    } /* end if (memory-release) */
	    {
	        rs1 = userinfo_dtor(op) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end */
	    op->magval = 0 ;
	} /* end if (magic) */
	return rs ;
} /* end subroutine (userinfo_finish) */


/* local subroutines */

local int userinfo_proc(UI *uip,strstore *stp,int *sis,cchar *un) noex {
	int		rs ;
	int		rs1 ;
	int		rv = 0 ;
	DEBPRINTF("ent\n") ;
	if (SMGR pi ; (rs = submgr_start(&pi,uip,stp,sis)) >= 0) ylikely {
	    if ((rs = submgr_pwext(&pi,un)) >= 0) {
		rs = submgr_comps(&pi) ;
		rv = rs ;
		DEBPRINTF("find() rs=%d\n",rs) ;
	    } /* end */
	    rs1 = submgr_finish(&pi) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (submgr) */
	DEBPRINTF("ret rs=%d rv=%d\n",rs,rv) ;
	return (rs >= 0) ? rv : rs ;
} /* end subroutine (userinfo_proc) */

local int userinfo_id(UI *uip) noex {
	int		rs ;
	if ((rs = ucpid) >= 0) {
	    uip->pid	= rs ;
	    uip->ruid	= getruid() ;
	    uip->euid	= geteuid() ;
	    uip->rgid	= getrgid() ;
	    uip->egid	= getegid() ;
	    if ((rs = ostype) >= 0) {
	        uip->ostype = rs ;
	    } /* end if */
	} /* end if (ucpid) */
	return rs ;
} /* end subroutine (userinfo_id) */

local int userinfo_load(UI *uip,strstore *stp,int *sis) noex {
	int		rs ;
	int		sz = 0 ; /* return-value */
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
	                case uit_prbin:
	                    vpp = &uip->prbin ;
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
	                case uit_projname:
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
	                if ((rs >= 0) && vpp && (*vpp == nullptr)) {
			    *vpp = (a + sis[i]) ;
			} /* end */
	            } /* end for */
	        } /* end if */
	        if (rs >= 0) {
	            uip->allocinfo = a ;
	        } else {
	            lm_free(a) ;
		} /* end */
	    } /* end if (memory-acquire) */
	} /* end if */
	return (rs >= 0) ? sz : rs ;
} /* end subroutine (userinfo_load) */

local int submgr_start(SMGR *pip,UI *uip,strstore *stp,int *sis) noex {
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
	    pip->uap = (userattrdb *) (bp + bl) ;
	    bl += szof(userattrdb) ;
	    pip->nodename = (bp + bl) ;
	    bl += var.nodenamelen ;
	    pip->domainname = (bp + bl) ;
	    bl += var.hostnamelen ;
	    pip->pr = (bp + bl) ;
	    bl += var.maxpathlen ;
	    pip->tbuf = (bp + bl) ;
	    bl += pip->tlen ;
	    if ((rs = bits_start(&pip->have,uit_overlast)) >= 0) ylikely {
	        pip->nodename[0]	= eos ;
	        pip->domainname[0]	= eos ;
	        pip->pr[0]		= eos ;
	        pip->tbuf[0]		= eos ;
	    } else {
	        lm_free(bp) ;
	        pip->a = nullptr ;
	    } /* end */
	} /* end if */
	return rs ;
} /* end subroutine (submgr_start) */

local int submgr_finish(SMGR *pip) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	{
	    rs1 = submgr_uaend(pip) ;
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
} /* end subroutine (submgr_finish) */

local int submgr_store(SMGR *pip,int uit,cc *vap,int val,cc **rpp) noex {
	int		rs = SR_OK ;
	int		len = 0 ; /* return-value */
	if (uit >= 0) {
	    if (pip->sis[uit] == 0) {
	        strstore *stp = pip->stp ;
	        if ((rs = strstore_store(stp,vap,val,rpp)) >= 0) {
		    len = rs ;
	            pip->sis[uit] = rs ;
	            rs = bits_set(&pip->have,uit) ;
		} /* end if (strstr_store) */
	    } /* end if (needed) */
	} else {
	    rs = SR_BUGCHECK ;
	} /* end if (error) */
	if (rpp && (rs < 0)) {
	    *rpp = nullptr ;
	} /* end if (error) */
	return (rs >= 0) ? len : rs ;
} /* end subroutine (submgr_store) */

local int submgr_uaready(SMGR *pip) noex {
    	int		rs = SR_OK ;
	if (syshas.userattr) {
	    rs = submgr_uabeg(pip) ;
	} /* end if (syshas.userattr) */
	return rs ;
} /* end subroutine (submgr_uaready) */

local int submgr_uabeg(SMGR *pip) noex {
	int		rs = SR_OK ;
	int		f = false ; /* return-value */
	DEBPRINTF("ent\n") ;
	if ((! pip->fl.uaopen) && pip->fl.pw) {
	    if ((rs = submgr_uaobjbeg(pip)) >= 0) {
	        cchar	*un = pip->pw.pw_name ;
	        if ((rs = userattrdb_open(pip->uap,un)) >= 0) {
	            pip->fl.uaopen = true ;
		    f = true ;
		} /* end if (open) */
		if (rs < 0) {
		    submgr_uaobjend(pip) ;
		} /* end if (error) */
	    } /* end if (submgr_uaobjbeg) */
	} /* end if (fl.uaopen) */
	DEBPRINTF("ret rs=%d\n",rs) ;
	return (rs >= 0) ? f : rs ;
} /* end subroutine (submgr_uabeg) */

local int submgr_uaend(SMGR *pip) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (pip->fl.uaopen) ylikely {
	    pip->fl.uaopen = false ;
	    rs1 = userattrdb_close(pip->uap) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if */
	if (pip->uap) {
	    rs1 = submgr_uaobjend(pip) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if */
	return rs ;
} /* end subroutine (submgr_uaend) */

local int submgr_uaobjbeg(SMGR *pip) noex {
    	int		rs = SR_OK ;
	if ((pip->uap == nullptr) && (! pip->fl.uamall)) {
	    cint	sz = szof(userattrdb) ;
	    if (void *vp ; (rs = lm_mall(sz,&vp)) >= 0) {
	        pip->uap = (userattrdb *) vp ;
		pip->fl.uamall = true ;
	    } /* end if (memory-acquire) */
	} /* end if (needed) */
	return rs ;
} /* end subroutine (submgr_uaobjbeg) */

local int submgr_uaobjend(SMGR *pip) noex {
    	int		rs = SR_OK ;
	int		rs1 ;
	if (pip->uap && pip->fl.uamall) {
	    rs1 = lm_free(pip->uap) ;
	    if (rs >= 0) rs = rs1 ;
	    pip->uap = nullptr ;
	    pip->fl.uamall = false ;
	} /* end if (had) */
	return rs ;
} /* end subroutine (submgr_uaobjend) */

local int submgr_ualookup(SMGR *pip,char *rbuf,int rlen,cc *kn) noex {
	int		rs = SR_FAULT ;
	int		len = 0 ;
	if (pip && rbuf && kn) ylikely {
	    rbuf[0] = eos ;
	    rs = SR_OK ;
	    if (syshas.userattr) {
	        if (pip->fl.pw) {
		    if ((rs = submgr_uaready(pip)) > 0) ylikely {
		        if (UAD *uap = resumelife<UAD>(pip->uap)) {
	                    if ((rs = userattrdb_look(uap,rbuf,rlen,kn)) >= 0) {
			        len = rs ;
			    } else if (isNoEnt(rs)) {
				rs = SR_OK ;
			    } /* end if */
			} /* end if (uap) */
	            } /* end if (uaready) */
	        } /* end if (has-pw) */
	    } /* end if (syshas.userattr) */
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
} /* end subroutine (submgr_ualookup) */

local int submgr_pwext(SMGR *pip,cchar *un) noex {
	userinfo	*uip = pip->uip ;
	int		rs ;
	DEBPRINTF("ent\n") ;
	if ((rs = submgr_pwentry(pip,un)) >= 0) ylikely {
	    cint	dlen = DIGBUFLEN ;
	    int		cl = -1 ;
	    int		uit ;
	    cchar	*cp{} ;
	    char	dbuf[DIGBUFLEN+1] ;
	DEBPRINTF("1 rs=%d\n",rs) ;
	    if (rs > 0) {
	        cchar	*vap{} ;
	        cp = pip->pw.pw_name ;
	        if (rs >= 0) {
	DEBPRINTF("2 rs=%d\n",rs) ;
	            uit = uit_password ;
	            vap = pip->pw.pw_passwd ;
	            rs = submgr_store(pip,uit,vap,-1) ;
	        }
	DEBPRINTF("3 rs=%d\n",rs) ;
	        if (rs >= 0) {
	            uit = uit_gecos ;
	            vap = pip->pw.pw_gecos ;
	            rs = submgr_store(pip,uit,vap,-1) ;
	        }
	DEBPRINTF("4 rs=%d\n",rs) ;
	        if (rs >= 0) {
		    cauto sm_st = submgr_store ;
	            uit = uit_homedname ;
	            vap = pip->pw.pw_dir ;
	            if (ccharp hd ; (rs = sm_st(pip,uit,vap,-1,&hd)) >= 0) {
			pip->userhome = hd ;
		    } /* end */
	        } /* end if (ok) */
	DEBPRINTF("5 rs=%d\n",rs) ;
	    } else {
	        uint	uv = (uint) uip->ruid ;
		rs = snsd(dbuf,dlen,"U",uv) ;
	DEBPRINTF("6 rs=%d\n",rs) ;
	    } /* end if */
	DEBPRINTF("7 rs=%d\n",rs) ;
	    if (rs >= 0) {
	        uit = uit_username ;
	        rs = submgr_store(pip,uit,cp,cl) ;
	    }
	} /* end if (submgr_pwentry) */
	DEBPRINTF("ret rs=%d\n",rs) ;
	return rs ;
} /* end subroutine (submgr_pwext) */

local int submgr_comps(SMGR *pip) noex {
    	int		rs = SR_OK ;
	for (int i = 0 ; components[i] ; i += 1) {
	DEBPRINTF("loop i=%d rs=%d\n",i,rs) ;
	    rs = (*components[i])(pip) ;
	    if (rs < 0) break ;
	} /* end for */
	DEBPRINTF("ret rs=%d\n",rs) ;
	return rs ;
} /* end subroutine (submgr_comps) */

local int submgr_pwentry(SMGR *pip,cchar *un) noex {
	int		rs ;
	int		f = false ;
	if (ucentpw pw ; (rs = pip->getpwu(&pw,un)) >= 0) {
	    if ((rs = pw.size()) >= 0) {
	        cint	pwsz = rs ;
	        if (char *p ; (rs = lm_mall((pwsz+1),&p)) >= 0) {
	            ucentpw *tpwp = resumelife<ucentpw>(&pip->pw) ;
	            if ((rs = tpwp->load(p,pwsz,&pw)) >= 0) {
	                pip->pwbuf = p ;
	                pip->pwlen = pwsz ;
	                f = true ;
	            } /* end */
	            if (rs < 0) {
	                lm_free(p) ;
		    } /* end if (error) */
	        } /* end if (memory-acquire) */
	    } /* end if (passwdent_size) */
	} /* end if (submgr_getpwu) */
	return (rs >= 0) ? f : rs ;
} /* end subroutine (submgr_pwentry) */

local int submgr_homedname(SMGR *pip) noex {
	userinfo	*uip = pip->uip ;
	int		rs = SR_OK ;
	int		uit = uit_homedname ;
	DEBPRINTF("ent\n") ;
	if ((pip->sis[uit] == 0) && (uip->homedname == nullptr)) {
	    cchar	*vap = nullptr ;
	    if ((vap == nullptr) || (vap[0] == '\0')) {
	        if (! pip->fl.altuser) {
		    static ccp v_home = getenver(varname.home) ;
	            vap = v_home ;
	        } /* end if */
	    } /* end */
	    if ((vap == nullptr) || (vap[0] == '\0')) {
	        static ccp v_tmpdir = getenver(varname.tmpdir) ;
	        vap = v_tmpdir ;
	    } /* end */
	    if ((vap == nullptr) || (vap[0] == '\0')) {
	        vap = sysword.w_tmpdir ;
	    } /* end */
	    uip->homedname = vap ;
	} /* end */
	DEBPRINTF("ret rs=%d\n",rs) ;
	return rs ;
} /* end subroutine (submgr_homedname) */

local int submgr_shell(SMGR *pip) noex {
	userinfo	*uip = pip->uip ;
	cnullptr	np{} ;
	int		rs = SR_OK ;
	int		uit = uit_shell ;
	if ((pip->sis[uit] == 0) && (uip->shell == np)) {
	    cchar	*vap = nullptr ;
	    if ((vap == nullptr) || (vap[0] == '\0')) {
	        if (! pip->fl.altuser) {
		    static ccp v_shell = getenver(varname.shell) ;
	            vap = v_shell ;
	        } /* end if */
	    } /* end */
	    if (vap && (vap[0] != '\0')) {
	        uip->shell = vap ;
	    } /* end */
	} /* end if */
	return rs ;
} /* end subroutine (submgr_shell) */

local int submgr_org(SMGR *pip) noex {
	userinfo	*uip = pip->uip ;
	cint		orglen = var.maxnamelen ;
	int		rs = SR_OK ;
	int		rs1 ;
	int		uit = uit_organization ;
	if ((pip->sis[uit] == 0) && (uip->organization == nullptr)) {
	    cint	rlen = orglen ;
	    int		val = -1 ;
	    cchar	*orgcname = ORGCNAME ;
	    cchar	*vap = nullptr ;
	    char	rbuf[orglen+1] ;
	    if (char *orgfname ; (rs = lm_mp(&orgfname)) >= 0) ylikely {
	        if ((vap == nullptr) || (vap[0] == '\0')) {
	            if (! pip->fl.altuser) {
	                static ccp v_org = getenver(varname.organization) ;
	                vap = v_org ;
	            } /* end if */
	        } /* end */
	        if ((vap == nullptr) || (vap[0] == '\0')) {
	            char	cname[orglen+1] ;
	            rs = sncpy2(cname,orglen,".",orgcname) ;
	            if (rs >= 0) ylikely {
	                cchar	*hd = uip->homedname ;
	                if ((hd == nullptr) && pip->fl.pw) {
	                    hd = pip->pw.pw_dir ;
		        }
	                if (hd == nullptr) hd = DEFHOMEDNAME ;
	                rs = mkpath2(orgfname,hd,cname) ;
	            } /* end if (ok) */
	            if (rs >= 0) ylikely {
	                if ((rs = filereadln(orgfname,rbuf,rlen)) > 0) {
	                    val = rs ;
	                    vap = rbuf ;
			} else if (isNotPresent(rs)) {
			    rs = SR_OK ;
			} /* end */
	            } /* end if (ok) */
	        } /* end if */
	        if (rs >= 0) ylikely {
	            if ((vap != nullptr) && (vap[0] != '\0')) {
	                rs = submgr_store(pip,uit,vap,val) ;
		    }
	        } /* end if (ok) */
		rs1 = lm_free(orgfname) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (m-a-f) */
	} /* end if */
	return rs ;
} /* end subroutine (submgr_org) */

local int submgr_prbin(SMGR *pip) noex {
	userinfo	*uip = pip->uip ;
	int		rs = SR_OK ;
	int		uit = uit_prbin ;
	if ((pip->sis[uit] == 0) && (uip->prbin == nullptr)) {
	    cchar	*vap = nullptr ;
	    if ((vap == nullptr) || (vap[0] == '\0')) {
	        if (! pip->fl.altuser) {
		    static ccp v_prbin = getenver(VARPRBIN) ;
	            vap = v_prbin ;
	        } /* end if */
	    } /* end */
	    if ((vap != nullptr) && (vap[0] != '\0')) {
	        uip->prbin = vap ;
	    } /* end */
	} /* end */
	return rs ;
} /* end subroutine (submgr_prbin) */

local int submgr_office(SMGR *pip) noex {
	userinfo	*uip = pip->uip ;
	int		rs = SR_OK ;
	int		uit = uit_office ;
	if ((pip->sis[uit] == 0) && (uip->office == nullptr)) {
	    cchar	*vap = nullptr ;
	    if ((vap == nullptr) || (vap[0] == '\0')) {
	        if (! pip->fl.altuser) {
	            vap = getenver(VAROFFICE) ;
	        }
	    } /* end */
	    if ((vap != nullptr) && (vap[0] != '\0')) {
	        uip->office = vap ;
	    } /* end */
	} /* end */
	return rs ;
} /* end subroutine (submgr_office) */

local int submgr_printer(SMGR *pip) noex {
	userinfo	*uip = pip->uip ;
	int		rs = SR_OK ;
	int		uit = uit_printer ;
	if ((pip->sis[uit] == 0) && (uip->printer == nullptr)) {
	    cchar	*vap = nullptr ;
	    if ((vap == nullptr) || (vap[0] == '\0')) {
	        if (! pip->fl.altuser) {
	            static ccp v_printer = getenver(varname.printer) ;
	            vap = v_printer ;
	        } /* end if */
	    } /* end */
	    if ((vap != nullptr) && (vap[0] != '\0')) {
	        uip->printer = vap ;
	    } /* end */
	} /* end */
	return rs ;
} /* end subroutine (submgr_printer) */

/* parse out the GECOS field */
local int submgr_gecos(SMGR *pip) noex {
    	cnullptr	np{} ;
	int		rs = SR_OK ;
	int		rs1 ;
	cchar		*gstr = pip->pw.pw_gecos ;
	if (pip->fl.pw && (gstr != nullptr) && (gstr[0] != '\0')) {
	    if (gecos g ; (rs = gecos_start(&g,gstr,-1)) >= 0) {
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
	                    if ((bits_test(bip,uit) > 0) || (up != np)) {
	                        vp = nullptr ;
			    }
	                    break ;
	                case gecosval_realname:
	                    uit = uit_gecosname ;
	                    up = uip->gecosname ;
	                    if ((bits_test(bip,uit) == 0) && (up == np)) {
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
	                    if ((bits_test(bip,uit) > 0) || (up != np)) {
	                        vp = nullptr ;
			    }
	                    break ;
	                case gecosval_prbin:
	                    uit = uit_prbin ;
	                    up = uip->prbin ;
	                    if ((bits_test(bip,uit) > 0) || (up != np)) {
	                        vp = nullptr ;
			    }
	                    break ;
	                case gecosval_office:
	                    uit = uit_office ;
	                    up = uip->office ;
	                    if ((bits_test(bip,uit) > 0) || (up != np)) {
	                        vp = nullptr ;
			    }
	                    break ;
	                case gecosval_wphone:
	                    uit = uit_wphone ;
	                    up = uip->hphone ;
	                    if ((bits_test(bip,uit) > 0) || (up != np)) {
	                        vp = nullptr ;
			    }
	                    break ;
	                case gecosval_hphone:
	                    uit = uit_hphone ;
	                    up = uip->hphone ;
	                    if ((bits_test(bip,uit) > 0) || (up != np)) {
	                        vp = nullptr ;
			    }
	                    break ;
	                case gecosval_printer:
	                    uit = uit_printer ;
	                    up = uip->printer ;
	                    if ((bits_test(bip,uit) > 0) || (up != np)) {
	                        vp = nullptr ;
			    }
	                    break ;
	                default:
	                    vp = nullptr ;
	                    break ;
	                } /* end switch */
	                if ((rs >= 0) && (vp != np)) {
	                    rs = submgr_store(pip,uit,vp,vl) ;
	                }
	            } /* end if */
	            if (rs < 0) break ;
	        } /* end for */
	        rs1 = gecos_finish(&g) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (gecos) */
	} /* end if (non-empty) */
	return rs ;
} /* end subroutine (submgr_gecos) */

local int submgr_gecosname(SMGR *pip) noex {
	userinfo	*uip = pip->uip ;
	int		rs = SR_OK ;
	int		uit = uit_gecosname ;
	if ((pip->sis[uit] == 0) && (uip->gecosname == nullptr)) {
	    cchar	*gstr = pip->pw.pw_gecos ;
	    if ((gstr != nullptr) && (gstr[0] != '\0')) {
	        cint	nlen = REALNAMELEN ;
	        char	nbuf[REALNAMELEN+1] ;
	        if ((rs = mkgecosname(nbuf,nlen,gstr)) > 0) {
		    cint val = rs ;
	            ccp vap = nbuf ;
	            rs = submgr_store(pip,uit,vap,val) ;
	        } /* end if (mkgecosname) */
	    } /* end */
	} /* end */
	return rs ;
} /* end subroutine (submgr_gecosname) */

local int submgr_realname(SMGR *pip) noex {
	userinfo	*uip = pip->uip ;
	int		rs = SR_OK ;
	int		uit = uit_realname ;
	DEBPRINTF("ent\n") ;
	if ((pip->sis[uit] == 0) && (uip->realname == nullptr)) {
	    cint	nlen = REALNAMELEN ;
	    cchar	*sp = pip->tstrs.gecosname ;
	    char	nbuf[REALNAMELEN+1] ;
	    if ((sp == nullptr) || (sp[0] == '\0')) {
	        if (! pip->fl.altuser) {
	            static ccp v_name = getenver(varname.name) ;
	            sp = v_name ;
	        } /* end if */
	    } /* end */
	    if ((sp != nullptr) && (sp[0] != '\0')) {
	        if ((rs = mkrealname(nbuf,nlen,sp,-1)) > 0) {
	            strstore	*stp = pip->stp ;
	            cchar	*cp ;
	DEBPRINTF("mkrealname() rs=%d\n",rs) ;
	            rs = strstore_store(stp,nbuf,rs,&cp) ;
	            pip->sis[uit] = rs ;
	            pip->tstrs.realname = cp ;
	        } /* end */
	DEBPRINTF("mkrealname-out rs=%d\n",rs) ;
	    } /* end */
	} /* end if (needed) */
	DEBPRINTF("ret rs=%d\n",rs) ;
	return rs ;
} /* end subroutine (submgr_realname) */

local int submgr_mailname(SMGR *pip) noex {
	userinfo	*uip = pip->uip ;
	int		rs = SR_OK ;
	int		uit = uit_mailname ;
	DEBPRINTF("ent\n") ;
	if ((pip->sis[uit] == 0) && (uip->mailname == nullptr)) {
	    cint	nlen = REALNAMELEN ;
	    cint	mlen = REALNAMELEN ;
	    int		sl = -1 ;
	    cchar	*sp = nullptr ;
	    char	nbuf[REALNAMELEN+1] ;
	    char	mbuf[REALNAMELEN+1] ;
	    if ((sp == nullptr) || (sp[0] == '\0')) {
	        if (! pip->fl.altuser) {
	            static ccp v_mailname = getenver(varname.mailname) ;
	            sp = v_mailname ;
	        } /* end if */
	    } /* end */
	    if ((sp == nullptr) || (sp[0] == '\0')) {
	        sp = pip->tstrs.gecosname ;
	    } /* end */
	    if ((sp == nullptr) || (sp[0] == '\0')) {
	        if (pip->fl.pw) {
	            cchar	*gn = pip->pw.pw_gecos ;
	            sp = nbuf ;
	            rs = mkgecosname(nbuf,nlen,gn) ;
		    sl = rs ;
	DEBPRINTF("mkgecosname() rs=%d\n",rs) ;
	        } /* end if (has-pw) */
	    } /* end */
	DEBPRINTF("mid2() rs=%d\n",rs) ;
	    if (rs >= 0) {
	    if ((sp == nullptr) || (sp[0] == '\0')) {
	        if (! pip->fl.altuser) {
	            static ccp v_name = getenver(varname.name) ;
	            sp = v_name ;
		    sl = -1 ;
	        } /* end if */
	    } /* end */
	    if ((sp == nullptr) || (sp[0] == '\0')) {
	        if (pip->fl.pw) {
		    sp = pip->pw.pw_name ;
		    sl = -1 ;
		} /* end if (has-pw) */
	    } /* end */
	    if ((sp != nullptr) && (sp[0] != '\0')) {
	        if ((rs = mkmailname(mbuf,mlen,sp,sl)) > 0) {
	            rs = submgr_store(pip,uit,mbuf,rs) ;
		} /* end if (mkmailname) */
	DEBPRINTF("mkmailname-out rs=%d\n",rs) ;
	    } /* end */
	    } /* end if (ok) */
	} /* end */
	DEBPRINTF("ret rs=%d\n",rs) ;
	return rs ;
} /* end subroutine (submgr_mailname) */

local int submgr_name(SMGR *pip) noex {
	userinfo	*uip = pip->uip ;
	int		rs = SR_OK ;
	int		uit = uit_name ;
	DEBPRINTF("ent\n") ;
	if ((pip->sis[uit] == 0) && (uip->name == nullptr)) {
	    cint	glen = REALNAMELEN ;
	    cint	rlen = REALNAMELEN ;
	    int		sl = -1 ;
	    cchar	*sp = nullptr ;
	    char	gbuf[REALNAMELEN+1] ;
	    char	rbuf[REALNAMELEN+1] ;
	    if ((sp == nullptr) || (sp[0] == '\0')) {
	        if (! pip->fl.altuser) {
	            static ccp v_name = getenver(varname.name) ;
		    sp = v_name ;
	        } /* end if */
	    } /* end */
	    if ((sp == nullptr) || (sp[0] == '\0')) {
	        sp = pip->tstrs.realname ;
	        if (sp == nullptr) {
	            cchar	*gp = pip->tstrs.gecosname ;
	            int		gl = -1 ;
	            if (gp == nullptr) {
	                cchar	*gn = pip->pw.pw_gecos ;
	DEBPRINTF("1 gn=%s\n",gn) ;
	                gp = gbuf ;
	                rs = mkgecosname(gbuf,glen,gn) ;
			gl = rs ;
	DEBPRINTF("mkgecosname() rs=%d\n",rs) ;
		    } /* end */
		    if (rs >= 0) {
	DEBPRINTF("2 gp=%s\n",gp) ;
	                sp = rbuf ;
	                rs = mkrealname(rbuf,rlen,gp,gl) ;
		        sl = rs ;
	DEBPRINTF("mkrealname() rs=%d\n",rs) ;
		    } /* end */
	        } /* end */
	    } /* end */
	DEBPRINTF("mid3 rs=%d\n",rs) ;
	    if (rs >= 0) {
	        if ((sp == nullptr) || (sp[0] == '\0')) {
	            if (pip->fl.pw) {
		        sp = pip->pw.pw_name ;
		        sl = -1 ;
		    } /* end if (has-pw) */
	        } /* end */
	        if ((sp != nullptr) && (sp[0] != '\0')) {
	            rs = submgr_store(pip,uit,sp,sl) ;
	        } /* end */
	    } /* end if */
	} /* end */
	DEBPRINTF("ret rs=%d\n",rs) ;
	return rs ;
} /* end subroutine (submgr_name) */

local int submgr_fullname(SMGR *pip) noex {
	userinfo	*uip = pip->uip ;
	cnullptr	np{} ;
	int		rs = SR_OK ;
	int		uit = uit_fullname ;
	DEBPRINTF("ent\n") ;
	if ((pip->sis[uit] == 0) && (uip->fullname == nullptr)) {
	    ccp vap = nullptr ;
	    if ((vap == np) || (vap[0] == '\0')) {
	        static ccp v_fullname = getenver(varname.fullname) ;
		vap = v_fullname ;
	    } /* end if */
	    if ((vap == np) || (vap[0] == '\0')) {
	        cint	nlen = REALNAMELEN ;
		char	nbuf[REALNAMELEN + 1] ;
		if ((rs = pip->rdhomefile(nbuf,nlen,HOME_FULLNAME)) > 0) {
	            rs = submgr_store(pip,uit,nbuf,rs) ;
		} /* end if (rdhomefile) */
	DEBPRINTF("rdhomefle-out rs=%d\n",rs) ;
	    } else {
	            uip->fullname = vap ;
	    } /* end if */
	} /* end if (needed) */
	DEBPRINTF("ret rs=%d\n",rs) ;
	return rs ;
} /* end subroutine (submgr_fullname) */

local int submgr_nodename(SMGR *pip) noex {
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
	    } /* end */
	    if (rs >= 0) {
	        rs = submgr_store(pip,uit,nbuf,nl) ;
	    } /* end */
	} /* end */
	return rs ;
} /* end subroutine (submgr_nodename) */

local int submgr_domain(SMGR *pip) noex {
	userinfo	*uip = pip->uip ;
	cnullptr	np{} ;
	int		rs = SR_OK ;
	int		uit = uit_domainname ;
	DEBPRINTF("ent\n") ;
	if ((pip->sis[uit] == 0) && (uip->domainname == np)) {
	    uip->domainname = nullptr ;
	    if (uip->domainname == nullptr) {
	        if (! pip->fl.altuser) {
		    cchar	*valp ; 
		    if ((valp = getenver(varname.localdomain)) != np) {
			valp = getenver(varname.domain) ;
		    } /* end */
	            uip->domainname = valp ;
	        } /* end if (getting env-domain) */
	    } /* end */
	DEBPRINTF("mid1 rs=%d\n",rs) ;
	    if (uip->domainname == nullptr) {
	        cint	dlen = var.hostnamelen ;
	        int	dl = -1 ;
	        char	*dbuf = pip->domainname ;
	        if ((rs >= 0) && (dbuf[0] == '\0')) {
	DEBPRINTF("pw1 rs=%d\n",rs) ;
	            if (pip->fl.pw) {
	                cchar	*kn = UA_DN ;
	DEBPRINTF("pw2 rs=%d\n",rs) ;
	                rs = submgr_ualookup(pip,dbuf,dlen,kn) ;
	DEBPRINTF("pw5 rs=%d\n",rs) ;
	            } /* end if (has-pw) */
	        } /* end */
	DEBPRINTF("mid2 rs=%d\n",rs) ;
	        if ((rs >= 0) && (dbuf[0] == '\0')) {
	            if ((rs = getinetdomain(dbuf,dlen)) >= 0) {
	DEBPRINTF("getinetdm() dbuf=%s\n",dbuf) ;
	                dl = rs ;
		    } else if (isNotPresent(rs)) {
	DEBPRINTF("getinetdm() failed rs=%d\n",rs) ;
	                dbuf[0] = eos ;
	                rs = SR_OK ;
	            } /* end */
	        } /* end */
	DEBPRINTF("mid3 rs=%d\n",rs) ;
	        if ((rs >= 0) && (dbuf[0] != '\0')) {
	            rs = submgr_store(pip,uit,dbuf,dl) ;
	        } /* end */
	    } /* end */
	} /* end if (needed) */
	DEBPRINTF("ret rs=%d\n",rs) ;
	return rs ;
} /* end subroutine (submgr_domainename) */

local int submgr_tz(SMGR *pip) noex {
	userinfo	*uip = pip->uip ;
	cnullptr	np{} ;
	int		rs = SR_OK ;
	int		uit = uit_tz ;
	if ((pip->sis[uit] == 0) && (uip->tz == np)) {
	    uip->tz = nullptr ;
	    if (uip->tz == nullptr) {
	        if (! pip->fl.altuser) {
	            static ccp v_tz = getenver(varname.tz) ;
	            uip->tz = v_tz ;
	        } /* end if */
	    } /* end if */
	    if ((rs >= 0) && (uip->tz == np)) {
	        cint	dlen = var.tznamelen ;
	        int	dl = -1 ;
	        char	dbuf[var.tznamelen +1] ;
	        if (pip->fl.pw) {
	            cchar	*kn = UA_TZ ;
	            if ((rs = submgr_ualookup(pip,dbuf,dlen,kn)) >= 0) {
	                dl = rs ;
		    } else if (isNotPresent(rs)) {
	                dbuf[0] = eos ;
	                rs = SR_OK ;
	            } /* end */
	        } /* end if (has-pw) */
	        if ((rs >= 0) && (dbuf[0] != '\0')) {
	            rs = submgr_store(pip,uit,dbuf,dl) ;
	        } /* end if */
	    } /* end if */
	} /* end if (tz) */
	return rs ;
} /* end subroutine (submgr_tz) */

local int submgr_md(SMGR *pip) noex {
	userinfo	*uip = pip->uip ;
	cnullptr	np{} ;
	int		rs = SR_OK ;
	int		rs1 ;
	int		uit = uit_md ;
	if ((pip->sis[uit] == 0) && (uip->md == np)) {
	    uip->md = nullptr ;
	    if (uip->md == nullptr) {
	        if (! pip->fl.altuser) {
	            static ccp v_maildir = getenver(varname.maildir) ;
	            uip->md = v_maildir ;
	        } /* end if */
	    } /* end */
	    if (uip->md == nullptr) {
	        int	dl = -1 ;
	        if (char *dbuf ; (rs = lm_hostname(&dbuf)) >= 0) {
		    cint	dlen = rs ;
	            if ((rs >= 0) && (dbuf[0] == '\0')) {
	                if (pip->fl.pw) {
	                    cchar	*kn = UA_MD ;
	                    if ((rs = submgr_ualookup(pip,dbuf,dlen,kn)) >= 0) {
	                        dl = rs ;
			    } else if (isNotPresent(rs)) {
	                        dbuf[0] = eos ;
	                        rs = SR_OK ;
	                    } /* end */
	                } /* end if (has-pw) */
	            } /* end if */
	            if ((rs >= 0) && (dbuf[0] != '\0')) {
	                rs = submgr_store(pip,uit,dbuf,dl) ;
	            } /* end */
		    rs1 = lm_free(dbuf) ;
		    if (rs >= 0) rs = rs1 ;
		} /* end if (m-a-f) */
	    } /* end */
	} /* end if (md) */
	return rs ;
} /* end subroutine (submgr_md) */

local int submgr_wstation(SMGR *pip) noex {
	userinfo	*uip = pip->uip ;
	cnullptr	np{} ;
	int		rs = SR_OK ;
	int		uit = uit_wstation ;
	if ((pip->sis[uit] == 0) && (uip->wstation == np)) {
	    uip->wstation = nullptr ;
	    if (uip->wstation == nullptr) {
	        if (! pip->fl.altuser) {
	            static ccp v_wstation = getenver(VARWSTATION) ;
	            uip->wstation = v_wstation ;
	        } /* end if */
	    } /* end */
	    if (uip->wstation == nullptr) {
	        cint	dlen = WSLEN ;
	        int	dl = -1 ;
	        char	dbuf[WSLEN+1] = { 0 } ;
	        if ((rs >= 0) && (dbuf[0] == '\0')) {
	            if (pip->fl.pw) {
	                cchar	*kn = UA_WS ;
	                if ((rs = submgr_ualookup(pip,dbuf,dlen,kn)) >= 0) {
	                    dl = rs ;
			} else if (isNotPresent(rs)) {
	                    dbuf[0] = eos ;
	                    rs = SR_OK ;
	                } /* end */
	            } /* end if (has-pw) */
	        } /* end */
	        if ((rs >= 0) && (dbuf[0] != '\0')) {
	            rs = submgr_store(pip,uit,dbuf,dl) ;
	        } /* end */
	    } /* end */
	} /* end */
	return rs ;
} /* end subroutine (submgr_wstation) */

local int submgr_logid(SMGR *pip) noex {
	userinfo	*uip = pip->uip ;
	cnullptr	np{} ;
	int		rs = SR_OK ;
	int		uit = uit_logid ;
	if ((pip->sis[uit] == 0) && (uip->logid == np)) {
	    int		v = uip->pid ;
	    cchar	*nn = uip->nodename ;
	    if ((nn == nullptr) || (nn[0] == '\0')) {
		nn = pip->nodename ;
	    }
	    if ((nn != nullptr) && (nn[0] != '\0')) {
	        cint	llen = LOGIDLEN ;
	        char	lbuf[LOGIDLEN+1] ;
	        if ((rs = mklogid(lbuf,llen,nn,-1,v)) >= 0) {
	            rs = submgr_store(pip,uit,lbuf,rs) ;
	        }
	    }
	} /* end if (needed) */
	return rs ;
} /* end subroutine (submgr_logid) */

local int submgr_groupname(SMGR *pip) noex {
    	userinfo	*uip = pip->uip ;
	cnullptr	np{} ;
    	int		rs ;
	int		rs1 ;
	int		uit = uit_groupname ;
	if ((pip->sis[uit] == 0) && (uip->groupname == np)) {
	    if (charp grbuf ; (rs = lm_gr(&grbuf)) >= 0) {
	        cint grlen = rs ;
	        con gid_t pwgid = uip->pwgid ;
	        if (ucentgr gr ; (rs = gr.getgid(grbuf,grlen,pwgid)) >= 0) {
	            rs = submgr_store(pip,uit,gr.gr_name,-1) ;
	        } else if (isNoEnt(rs)) {
		    rs = SR_OK ;
	        } /* end */
	        rs1 = lm_free(grbuf) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (m-a-f) */
	} /* end if (needed) */
	return rs ;
} /* end subroutine (submgr_groupname) */

local int submgr_projname(SMGR *pip) noex {
    	cnullptr	np{} ;
	int		rs = SR_OK ;
	DEBPRINTF("ent\n") ;
	if (syshas.project) {
	    userinfo	*uip = pip->uip ;
	    cint	dlen = var.projnamelen ;
	    int		uit = uit_projname ;
	    if ((pip->sis[uit] == 0) && (uip->projname == np)) {
	        int	dl = 0 ;
	        char	dbuf[dlen+1] = { 0 } ;
	        if ((rs >= 0) && (dbuf[0] == '\0')) {
	            if (pip->fl.pw) {
	                cchar	*kn = UA_PROJECT ;
	                rs = submgr_ualookup(pip,dbuf,dlen,kn) ;
	                dl = rs ;
	            } /* end if (fl.pw) */
	        } /* end */
	        if ((rs >= 0) && (dl == 0) && (dbuf[0] == '\0')) {
	            if (pip->fl.pw) {
	                cchar	*un = pip->pw.pw_name ;
	                if ((rs = getprojname(dbuf,dlen,un)) >= 0) {
	                    dl = rs ;
			} else if (isNoEnt(rs)) {
	                    dbuf[0] = eos ;
	                    rs = SR_OK ;
	                } /* end */
	            } /* end */
	        } /* end */
	        if ((rs >= 0) && (dl > 0) && dbuf[0]) {
	            rs = submgr_store(pip,uit,dbuf,dl) ;
	        } /* end */
	    } /* end if (project) */
	} /* end if (syshas.project) */
	DEBPRINTF("ret rs=%d\n",rs) ;
	return rs ;
} /* end subroutine (submgr_projname) */

local int submgr_unixname(SMGR *pip) noex {
	int		rs ;
	if ((rs = ucinfo_name(&pip->unixinfo)) >= 0) {
	    ucinfo_names	*xip = &pip->unixinfo ;
	    userinfo		*uip = pip->uip ;
	    cint		uit = uit_nodename ;
	    uip->sysname = xip->sysname ;
	    uip->release = xip->release ;
	    uip->version = xip->version ;
	    uip->machine = xip->machine ;
	    if ((pip->sis[uit] == 0) && (uip->nodename == nullptr)) {
	        uip->nodename = xip->nodename ;
	    } /* end if */
	} /* end subroutine */
	return rs ;
} /* end subroutine (submgr_unixname) */

int submgr::getpwu(ucentpw *pwp,ccp un) noex {
	int		rs ;
	if ((un != nullptr) && (un[0] != '-') && (un[0] != '\0')) {
	    rs = getpwx_name(pwp,tbuf,tlen,un) ;
	} else {
	    rs = getpwusername(pwp,tbuf,tlen,uip->ruid) ;
	} /* end */
	if (rs >= 0) {
	    fl.pw = true ;
	    fl.altuser = (pwp->pw_uid != uip->ruid) ;
	    uip->pwuid = pwp->pw_uid ;
	    uip->pwgid = pwp->pw_gid ;
	} /* end if (ok) */
	return rs ;
} /* end method (submgr::getpwu) */

int submgr::rdhomefile(charp rbuf,int rlen,ccp fn) noex {
    	int		rs = SR_OK ;
	int		len = 0 ; /* return-value */
	rbuf[0] = eos ;
	DEBPRINTF("ent fn=%s\n",fn) ;
	if (userhome) {
	DEBPRINTF("userhome=%s\n",userhome) ;
	        if ((rs = mkpath(tbuf,userhome,fn)) >= 0) {
	DEBPRINTF("tbuf=%s\n",tbuf) ;
		    if ((rs = filereadln(tbuf,rbuf,rlen)) >= 0) {
		        len = rs ;
		    } else if (isNotPresent(rs)) {
			rs = SR_OK ;
		    } /* end */
		} /* end if (mkpath) */
	} /* end if (have home-dir) */
	DEBPRINTF("ret rs=%d len=%d\n",rs,len) ;
	return (rs >= 0) ? len : rs ;
} /* end method (submgr::rdhomefile) */

void userinfo::dtor() noex {
	if (cint rs = finish ; rs < 0) {
	    ulogerror("userinfo",rs,"fini-finish") ;
	} /* end */
} /* end method (useirnfo::dtor) */

userinfo_cos::operator int () noex {
    	int		rs = SR_BUGCHECK ;
	DEBPRINTF("ent op-int\n") ;
	if (op) ylikely {
	    rs = userinfo_start(op,nullptr) ;
	} /* end if (non-null) */
	return rs ;
} /* end method */

int userinfo_cos::operator () (cchar *un) noex {
    	int		rs = SR_BUGCHECK ;
	DEBPRINTF("ent op-call\n") ;
	if (op) ylikely {
	    rs = userinfo_start(op,un) ;
	} /* end if (non-null) */
	return rs ;
} /* end method */

userinfo_co::operator int () noex {
	int		rs = SR_BUGCHECK ;
	if (op) ylikely {
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
	        memclear(ubuf,ulen) ;
	        if (userinfo u ; (rs = userinfo_start(&u,un)) >= 0) ylikely {
	            if (storeitem si ; (rs = si.start(ubuf,ulen)) >= 0) {
	                cchar	*sp ;
	                cchar	**rpp = nullptr ;
	                oup->ostype = u.ostype ;
	                oup->pid	= u.pid ;
	                oup->ruid	= u.ruid ;
	                oup->euid	= u.euid ;
	                oup->rgid	= u.rgid ;
	                oup->egid	= u.egid ;
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
	                    case uit_prbin:
	                        rpp = &oup->prbin ;
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
	                    case uit_projname:
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
	                    if (sp && rpp) {
			        rs = si.strw(sp,-1,rpp) ;
		            } /* end */
	                    if (rs < 0) break ;
	                } /* end for */
	                if (rs >= 0) {
		            oup->magval = USERINFO_MAGIC ;
		        } /* end */
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

vars::operator int () noex {
    	cnullptr	np{} ;
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
				cint req = _SC_TZNAME_MAX ;
				pwlen = rs ;
				if ((rs = u_sysconfval(req,np)) >= 0) {
				    tznamelen = rs ;
				} /* end if (u_sysconfval) */
			    }
			}
		    }
		}
	    }
	} /* end */
	return rs ;
} /* end method (vars::operator) */

local bool isNoEnt(int rs) noex {
	return isNotPresent(rs) || (rs == SR_NOSYS) ;
} /* end subroutine */


