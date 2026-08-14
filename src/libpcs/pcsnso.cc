/* pcsnso SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* PCS-NAME-SERVER query database manager */
/* version %I% last-modified %G% */

#define	CF_DEBUGS	0		/* compile-time debugging */
#define	CF_PCSNSC	1		/* use PCSNSC facility */

/* revision history:

	- 2008-10-01, David A­D­ Morano
	This object module was originally written.

*/

/* Copyright © 2008 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	pcsnso

	Description:
	This is the main interface to the PCS Name-Server.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* POSIX® */
#include	<sys/param.h>		/* POSIX® */
#include	<sys/stat.h>		/* POSIX® */
#include	<netdb.h>		/* POSIX® */
#include	<pwd.h>			/* POSIX® */
#include	<ctime>			/* CSTD */
#include	<climits>		/* CSTD */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstring>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<ascii.h>		/* LIBU */
#include	<uclibmem.h>		/* LIBUC */
#include	<getpx.h>		/* LIBUC */
#include	<getpwx.h>		/* LIBUC */
#include	<bufsizeget.h>		/* LIBUC */
#include	<getusername.h>		/* LIBUC */
#include	<getax.h>		/* LIBUC */
#include	<estrings.h>		/* LIBUC */
#include	<vecstr.h>		/* LIBUC */
#include	<spawnproc.h>		/* LIBUC */
#include	<expcook.h>		/* LIBUC */
#include	<field.h>		/* LIBUC */
#include	<sbuf.h>		/* LIBUC */
#include	<gecos.h>		/* LIBUC */
#include	<filereadln.h>		/* LIBUC */
#include	<strwcpy.h>		/* LIBUC */
#include	<char.h>		/* LIBUC */
#include	<vardefs.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */
#include	<libdebug.h>		/* LIBDEBUG */

#include	"pcsnso.h"
#include	"pcsnsmgr.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| */

/* local defines */

#ifndef	NSYSPIDS
#define	NSYSPIDS	100
#endif

#ifndef	DEFPROJNAME
#define	DEFPROJNAME	"default"
#endif

#ifndef	NAMEFNAME
#define	NAMEFNAME	".name"
#endif

#ifndef	FULLNAMEFNAME
#define	FULLNAMEFNAME	".fullname"
#endif

#ifndef	PROJECTFNAME
#define	PROJECTFNAME	".project"
#endif

#ifndef	ORGFNAME
#define	ORGFNAME	".organization"
#endif

#ifndef	PCSDPIFNAME /* PCS Default-Project-Info file */
#define	PCSDPIFNAME	"etc/projectinfo"
#endif

#ifndef	PRORGFNAME
#define	PRORGFNAME	"etc/organization"
#endif

#ifndef	TMPDNAME
#define	TMPDNAME	"/tmp"
#endif

#ifndef	TMPVARDNAME
#define	TMPVARDNAME	"/var/tmp"
#endif

#ifndef	VCNAME
#define	VCNAME		"var"
#endif

#define	INDDNAME	"pcsnso"

#define	TO_FILEMOD	(60 * 24 * 3600)
#define	TO_LASTCHECK	(4*60)

#define	SI		subinfo
#define	SI_FL		subinfo_flags


/* external subroutines */


/* exported variables */

const pcsno_obj		pcsnso_mod = {
	"pcsnso",
	szof(pcsnso),
	szof(pcsnos_cur)
} ; /* end initialization */


/* local structures */

struct subinfo_flags {
	uint		setcache:1 ;
} ; /* end struct */

struct subinfo {
	PCSNSO		*op ;
	cchar		*pr ;		/* convenience */
	cchar		*varusername ;
	cchar		*un ;		/* passed argument */
	char		*rbuf ;		/* passed argument */
	SI_FL	init, f ;
	uid_t		uid ;
	int		rlen ;		/* passed argument */
	int		w ;		/* passed argument */
} ; /* end struct (subinfo) */

struct pcsnametype {
	cchar	*var ;
	cchar	*fname ;
} ; /* end struct */


/* forward references */

local int	pcsnso_infoloadbegin(PCSNSO *,cchar *) ;
local int	pcsnso_infoloadend(PCSNSO *) ;
local int	pcsnso_getpw(PCSNSO *,cchar *) ;

local int	pcsnso_getrealname(PCSNSO *,SI *) ;
local int	pcsnso_getpcsname(PCSNSO *,SI *) ;
local int	pcsnso_getfullname(PCSNSO *,SI *) ;
local int	pcsnso_getprojinfo(PCSNSO *,SI *) ;
local int	pcsnso_client(PCSNSO *) ;
local int	pcsnso_clientbegin(PCSNSO *,time_t) ;
local int	pcsnso_clientend(PCSNSO *) ;

local int	subinfo_start(SI *,PCSNSO *,char *,int,cchar *,int) ;
local int	subinfo_finish(SI *) ;
local int	subinfo_prfile(SI *,cchar *) ;

local int	getname(SI *) ;
local int	getname_var(SI *) ;
local int	getname_daemon(SI *) ;
local int	getname_nsmgr(SI *) ;
local int	getname_userhome(SI *) ;
local int	getname_again(SI *) ;
local int	getname_sysdb(SI *) ;
local int	getname_pcsdef(SI *) ;

local int	getprojinfo_sysdb(SI *) ;


/* local variables */

constexpr pcsnametype	pcsnametypes[] = {
	{ VARNAME,	NAMEFNAME },
	{ VARFULLNAME,	FULLNAMEFNAME },
	{ VARPROJINFO,	PROJECTFNAME },
	{ VARORG,	ORGFNAME },
	{ nullptr,	nullptr }
} ; /* end array */

constexpr int		(*getnames[])(SI *) = {
	getname_var,
	getname_nsmgr,
	getname_daemon,
	getname_userhome,
	getname_again,
	getname_sysdb,
	getname_pcsdef,
	nullptr
} ; /* end array */


/* exported variables */


/* exported subroutines */

int pcsnso_open(PCSNSO *op,cchar *pr) noex {
	int		rs ;

	if (op == nullptr) return SR_FAULT ;
	if (pr == nullptr) return SR_FAULT ;

	if (pr[0] == '\0') return SR_INVALID ;

#if	CF_DEBUGS
	DEBUGPRINTF("pcsnso_open: pr=%s\n",pr) ;
#endif

	memclear(op) ;
	if ((rs = pcsnso_infoloadbegin(op,pr)) >= 0) {
	    op->magval = PCSNSO_MAGIC ;
	} /* end if (pcsnso_infoloadbegin) */

#if	CF_DEBUGS
	DEBUGPRINTF("pcsnso_open: ret rs=%d\n",rs) ;
#endif

	return rs ;
} /* end subroutine (pcsnso_open) */

int pcsnso_close(PCSNSO *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;

	if (op == nullptr) return SR_FAULT ;

	if (op->magval != PCSNSO_MAGIC) return SR_NOTOPEN ;

	rs1 = pcsnso_clientend(op) ;
	if (rs >= 0) rs = rs1 ;

	rs1 = pcsnso_infoloadend(op) ;
	if (rs >= 0) rs = rs1 ;

#if	CF_DEBUGS
	DEBUGPRINTF("pcsnso_close: ret rs=%d\n",rs) ;
#endif

	op->magval = 0 ;
	return rs ;
} /* end subroutine (pcsnso_close) */

int pcsnso_setopts(PCSNSO *op,int opts) noex {
	int		rs = SR_OK ;
	if (op == nullptr) return SR_FAULT ;
	if (op->magval != PCSNSO_MAGIC) return SR_NOTOPEN ;
	op->opts = opts ;
	return rs ;
} /* end subroutine (pcsnso_setopts) */

int pcsnso_get(PCSNSO *op,char *rbuf,int rlen,cchar *un,int w) noex {
	SI		si, *sip = &si ;
	int		rs ;
	int		rs1 ;
	int		len = 0 ;

	if (op == nullptr) return SR_FAULT ;

	if (op->magval != PCSNSO_MAGIC) return SR_NOTOPEN ;

	if ((rs = subinfo_start(sip,op,rbuf,rlen,un,w)) >= 0) {
	    switch (w) {
	    case pcsnsreq_realname:
	        rs = pcsnso_getrealname(op,sip) ;
	        len = rs ;
	        break ;
	    case pcsnsreq_pcsname:
	        rs = pcsnso_getpcsname(op,sip) ;
	        len = rs ;
	        break ;
	    case pcsnsreq_fullname:
	        rs = pcsnso_getfullname(op,sip) ;
	        len = rs ;
	        break ;
	    case pcsnsreq_projinfo:
	        rs = pcsnso_getprojinfo(op,sip) ;
	        len = rs ;
	        break ;
	    case pcsnsreq_pcsorg:
	        rs = pcsnso_getpcsname(op,sip) ;
	        len = rs ;
	        break ;
	    default:
	        rs = SR_INVALID ;
	        break ;
	    } /* end switch */
	    rs1 = subinfo_finish(sip) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (subinfo) */

#if	CF_DEBUGS
	DEBUGPRINTF("pcsnso_get: ret rs=%d len=%u\n",rs,len) ;
#endif

	return (rs >= 0) ? len : rs ;
} /* end subroutine (pcsnso_get) */

int pcsnso_audit(PCSNSO *op) noex {
	int		rs = SR_OK ;

	if (op == nullptr) return SR_FAULT ;

	if (op->magval != PCSNSO_MAGIC) return SR_NOTOPEN ;

#if	CF_DEBUGS
	DEBUGPRINTF("pcsnso_audit: txtindex_audit() rs=%d\n",rs) ;
#endif

	return rs ;
} /* end subroutine (pcsnso_audit) */

int pcsnso_curbegin(PCSNSO *op,PCSNSO_CUR *curp) noex {
	int		rs = SR_OK ;

	if (op == nullptr) return SR_FAULT ;
	if (curp == nullptr) return SR_FAULT ;

	if (op->magval != PCSNSO_MAGIC) return SR_NOTOPEN ;

	memclear(curp) ;
	op->ncursors += 1 ;

	return rs ;
} /* end subroutine (pcsnso_curbegin) */

int pcsnso_curend(PCSNSO *op,PCSNSO_CUR *curp) {
	int		rs = SR_OK ;
	int		rs1 ;

	if (op == nullptr) return SR_FAULT ;
	if (curp == nullptr) return SR_FAULT ;

	if (op->magval != PCSNSO_MAGIC) return SR_NOTOPEN ;

	if (curp->verses != nullptr) {
	    rs1 = lm_free(curp->verses) ;
	    if (rs >= 0) rs = rs1 ;
	    curp->verses = nullptr ;
	}

	curp->nverses = 0 ;
	if (op->ncursors > 0) op->ncursors -= 1 ;

	return rs ;
} /* end subroutine (pcsnso_curend) */

int pcsnso_curenum(PCSNSO *op,PCSNSO_CUR *curp,char *vbuf,int vlen,int w) {
	int		rs = SR_OK ;
	int		len = 0 ;

	if (op == nullptr) return SR_FAULT ;
	if (curp == nullptr) return SR_FAULT ;
	if (vbuf == nullptr) return SR_FAULT ;

	if (op->magval != PCSNSO_MAGIC) return SR_NOTOPEN ;

#if	CF_DEBUGS
	DEBUGPRINTF("pcsnso_curenum: ret rs=%d len=%u\n",rs,len) ;
#endif

	return (rs >= 0) ? len : rs ;
} /* end subroutine (pcsnso_curenum) */


/* private subroutines */

local int pcsnso_infoloadbegin(PCSNSO *op,cchar *pr) noex {
	int		rs ;

	if ((rs = bufsizeget(bufsize_pw)) >= 0) {
	    cint	pwlen = rs ;
	    int		size = 0 ;
	    char	*bp ;
	    size += (pwlen+1) ;
	    size += (lenstr(pr)+1) ;
	    if ((rs = lm_mall(size,&bp)) >= 0) {
	        op->a = bp ;
	        pdp->pwbuf = bp ;
	        pdp->pwlen = pwlen ;
	        bp += (pwlen+1) ;
	        op->pr = bp ;
	        strwcpy(bp,pr,-1) ;
	    } /* end if (m-a) */
	} /* end if (bufsizeget) */

	return rs ;
} /* end subroutine (pcsnso_infoloadbegin) */

local int pcsnso_infoloadend(PCSNSO *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (op->fl.id) {
	    op->fl.id = FALSE ;
	    rs1 = ids_release(&op->id) ;
	    if (rs >= 0) rs = rs1 ;
	}
	if (op->a) {
	    rs1 = lm_free(op->a) ;
	    if (rs >= 0) rs = rs1 ;
	    op->a = nullptr ;
	    op->pr = nullptr ;
	    pdp->pwbuf = nullptr ;
	    pdp->pwlen = 0 ;
	}
	return rs ;
} /* end subroutine (pcsnso_infoloadend) */

local int pcsnso_getpw(PCSNSO *op,cchar *un) noex {
	PCSNSO_PWD	*pdp = &op->pwd ;
	int		rs = SR_OK ;
	cchar		*pun ;

	pun = pdp->pw.pw_name ;
	if ((pun == nullptr) || (strcmp(pun,un) != 0)) {
	    struct passwd	*pwp = &pdp->pw ;
	    cint		pwlen = pdp->pwlen ;
	    char		*pwbuf = pdp->pwbuf ;
	    if ((un != nullptr) && (un[0] != '\0') && (un[0] != '-')) {
	        if (hasalldig(un,-1)) {
	            uint	uv ;
	            if ((rs = cfdecui(un,-1,&uv)) >= 0) {
	                const uid_t	uid = uv ;
	                rs = getpwusername(pwp,pwbuf,pwlen,uid) ;
	            }
	        } else {
	            rs = getpwx_name(pwp,pwbuf,pwlen,un) ;
	        }
	    } else {
	        rs = getpwusername(pwp,pwbuf,pwlen,-1) ;
	    }
	} /* end if (was not already initialized) */
	DEBUGPRINTF("ret rs=%d\n",rs) ;
	return rs ;
} /* end subrouine (pcsnso_getpw) */

local int pcsnso_getrealname(PCSNSO *op,SI *sip) noex {
	int		rs ;

	rs = getname(sip) ;

	return rs ;
} /* end subrouine (pcsnso_getrealname) */

local int pcsnso_getpcsname(PCSNSO *op,SI *sip) noex {
	int		rs ;
	{
	rs = getname(sip) ;
	}
	return rs ;
} /* end subrouine (pcsnso_getpcsname) */

local int pcsnso_getfullname(PCSNSO *op,SI *sip) noex {
	int		rs ;
	{
	rs = getname(sip) ;
	}
	return rs ;
} /* end subrouine (pcsnso_getfullname) */

local int pcsnso_getprojinfo(PCSNSO *op,SI *sip) noex {
	int		rs ;

#if	CF_DEBUGS
	DEBUGPRINTF("pcsnso/getprojinfo: ent\n") ;
#endif

	rs = getname(sip) ;

#if	CF_DEBUGS
	DEBUGPRINTF("pcsnso/getprojinfo: ret rs=%d\n",rs) ;
#endif

	return rs ;
} /* end subrouine (pcsnso_getprojinfo) */

local int pcsnso_client(PCSNSO *op) noex {
	int		rs = MKBOOL(op->open.client) ;
	if (! op->fl.client) {
	    op->fl.client = TRUE ;
	    rs = pcsnso_clientbegin(op,0) ;
	} else if (! op->open.client) {
	    const time_t	dt = time(nullptr) ;
	    if ((dt - op->ti_lastcheck) >= TO_LASTCHECK) {
		rs = pcsno_clientbegin(op,dt) ;
	    }
	}
#if	CF_DEBUGS
	DEBUGPRINTF("pcsnso_client: ret rs=%d\n",rs) ;
#endif
	return rs ;
} /* end subroiutine (pcsnso_client) */

local int pcsnso_clientbegin(PCSNSO *op,time_t dt) noex {
	int		rs = SR_OK ;
	int		f = FALSE ;
	if (! op->open.client) {
	    PCSNSC	*pcp = &op->client ;
	    cint	to = PCSNSO_TO ;
	    if (dt == 0) dt = time(nullptr) ;
	    op->ti_lastcheck = dt ;
	    if ((rs = pcsnsc_open(pcp,op->pr,to)) >= 0) {
		op->open.client = TRUE ;
		op->open.server = (rs > 0) ;
		f = TRUE ;
	    } else if (isBadSend(rs)) {
		rs = SR_OK ;
	    }
	} /* end if (client was not open) */
#if	CF_DEBUGS
	DEBUGPRINTF("pcsnso_clientbegin: ret rs=%d f=%u\n",rs,f) ;
#endif
	return (rs >= 0) ? f : rs ;
} /* end subroutine (pcsnso_clientbegin) */

local int pcsnso_clientend(PCSNSO *op) noex {
	int		rs = SR_OK ;
	if (op->open.client) {
	    PCSNSC	*pcp = &op->client ;
	    op->open.client = FALSE ;
	    rs = pcsnsc_close(pcp) ;
	}
	return rs ;
} /* end subroiutine (pcsnso_clientend) */

local int subinfo_start(SI *sip,PCSNSO *op,char *rbuf,int rlen,
			cchar *un,int w) noex {
	int		rs = SR_OK ;
	rbuf[0] = '\0' ;
	memclear(sip) ;
	sip->op = op ;
	sip->pr = op->pr ;
	sip->rbuf = rbuf ;
	sip->rlen = rlen ;
	sip->un = un ;
	sip->w = w ;
	sip->varusername = VARUSERNAME ;
	return rs ;
} /* end subroutine (subinfo_start) */

local int subinfo_finish(SI *sip) noex {
	int		rs = SR_OK ;
	if (sip == nullptr) return SR_FAULT ;
	return rs ;
} /* end subroutine (subinfo_finish) */

local int subinfo_prfile(SI *sip,cchar *fn) noex {
	int		rs ;
	int		len = 0 ;
	char		tbuf[MAXPATHLEN + 1] ;
	if ((rs = mkpath2(tbuf,sip->pr,fn)) >= 0) {
	    cint	rlen = sip->rlen ;
	    char	*rbuf = sip->rbuf ;
	    if ((rs = filereadln(tbuf,rbuf,rlen)) >= 0) {
	        len = rs ;
	    } else if (isNotPresent(rs)) {
	        rs = SR_OK ;
	    }
	}
	return (rs >= 0) ? len : rs ;
} /* end subroutine (subinfo_prfile) */

local int getname(SI *sip) noex {
	cint	w = sip->w ;
	int		rs = SR_OK ;
	int		len = 0 ;
	DEBUGPRINTF("ent u=%s w=%u\n",sip->un,sip->w) ;
	switch (w) {
	case pcsnsreq_realname:
	case pcsnsreq_pcsname:
	case pcsnsreq_fullname:
	case pcsnsreq_projinfo:
	case pcsnsreq_pcsorg:
	    {
	        int		i ;
	        for (i = 0 ; getnames[i] != nullptr ; i += 1) {
	            rs = (*getnames[i])(sip) ;
	            len = rs ;
	            if (rs != 0) break ;
	        } /* end for */
	        if ((rs > 0) && sip->fl.setcache) {
	            rs = pcsnsmgr_set(sip->rbuf,len,sip->un,sip->w,0) ;
#if	CF_DEBUGS
	            DEBUGPRINTF("pcsnso/getname: pcsnsmgr_set() rs=%d\n",rs) ;
#endif
	        }
	    } /* end block */
	    break ;
	} /* end switch */
	DEBUGPRINTF("ret rs=%d len=%u\n",rs,len) ;
	return (rs >= 0) ? len : rs ;
} /* end subroutine (getname) */

local int getname_var(SI *sip) noex {
	cint	w = sip->w ;
	int		rs = SR_OK ;
	int		len = 0 ;
	cchar		*un = sip->un ;

	switch (w) {
	case pcsnsreq_pcsname:
	case pcsnsreq_fullname:
	case pcsnsreq_pcsorg:
	    {
	        int	f = (un[0] == '-') ;
	        if (! f) {
	            cchar	*vun = getenv(VARUSERNAME) ;
	            if ((vun != nullptr) && (vun[0] != '\0')) {
	                f = (strcmp(vun,un) == 0) ;
	            }
	        }
	        if (f) {
	            cchar	*var = pcsnametypes[w].var ;
	            if (var != nullptr) {
	                cchar	*cp = getenv(var) ;
	                if ((cp != nullptr) && (cp[0] != '\0')) {
	                    rs = sncpy1(sip->rbuf,sip->rlen,cp) ;
	                    len = rs ;
	                }
	            } /* end if (anyone but 'realname') */
	        } /* end if */
	    } /* end block */
	    break ;
	} /* end switch */
	return (rs >= 0) ? len : rs ;
} /* end subroutine (getname_var) */

local int getname_daemon(SI *sip) noex {
	PCSNSO		*op = sip->op ;
	int		rs = SR_OK ;
	int		rl = 0 ;
	DEBUGPRINTFent\n") ;
#if	CF_PCSNSC
	if ((op->opts & PCSNSO_ONOSERV) == 0) {
	    DEBUGPRINTF("serv\n") ;
	    if ((rs = pcsnso_client(op)) > 0) {
	        PCSNSC		*pcp = &op->client ;
	        cint	rlen = sip->rlen ;
		cint	w = sip->w ;
		cchar	*un = sip->un ;
		char		*rbuf = sip->rbuf ;
	        if ((rs = pcsnsc_getval(pcp,rbuf,rlen,un,w)) > 0) {
		    rl = rs ;
		    DEBUGPRINTF("pcsnsc_getval() rs=%d\n", rs) ;
		} else if (isBadSend(rs)) {
		    rs = SR_OK ;
		}
		DEBUGPRINTF("-out rs=%d\n", rs) ;
	    } /* end if (pcsnso_client) */
	    DEBUGPRINTF("leaving rs=%d\n",rs) ;
	} /* end if (ok to call server) */
#endif /* CF_PCSNSC */
	DEBUGPRINTF("ret rs=%d rl=%u\n",rs,rl) ;
	return (rs >= 0) ? rl : rs ;
} /* end subroutine (getname_daemon) */

local int getname_nsmgr(SI *sip) noex {
	cint	rsn = SR_NOTFOUND ;
	cint	w = sip->w ;
	int		rs ;
	cchar	*un = sip->un ;

#if	CF_DEBUGS
	DEBUGPRINTF("pcsnso/getname_nsmgr: ent\n") ;
#endif

	if ((rs = pcsnsmgr_get(sip->rbuf,sip->rlen,un,w)) == rsn) {
#if	CF_DEBUGS
	    DEBUGPRINTF("pcsnso/getname_nsmgr: pcsnsmgr_get() rs=%d\n",rs) ;
#endif
	    rs = SR_OK ;
	    sip->fl.setcache = TRUE ;
	} else if (rs == 0) {
#if	CF_DEBUGS
	    DEBUGPRINTF("pcsnso/getname_nsmgr: pcsnsmgr_get() rs=%d\n",rs) ;
#endif
	    sip->fl.setcache = TRUE ;
	}

#if	CF_DEBUGS
	DEBUGPRINTF("pcsnso/getname_nsmgr: ret rs=%d\n",rs) ;
#endif

	return rs ;
} /* end subroutine (getname_nsmgr) */

local int getname_userhome(SI *sip) noex {
	cint	w = sip->w ;
	int		rs = SR_OK ;
	cchar		*un = sip->un ;
	cchar		*fn ;

#if	CF_DEBUGS
	DEBUGPRINTF("pcsgetnames/getname_userhome: ent un=%s w=%u\n",un,w) ;
#endif

	fn = pcsnametypes[w].fname ;
#if	CF_DEBUGS
	DEBUGPRINTF("pcsgetnames/getname_userhome: fn=%s\n",fn) ;
#endif
	if (fn != nullptr) {
	    cint	hlen = MAXPATHLEN ;
	    char	hbuf[MAXPATHLEN + 1] ;
	    if ((rs = getuserhome(hbuf,hlen,un)) >= 0) {
	        char	tbuf[MAXPATHLEN + 1] ;
#if	CF_DEBUGS
	        DEBUGPRINTF("pcsgetnames/getname_userhome: h=%s\n",hbuf) ;
#endif
	        if ((rs = mkpath2(tbuf,hbuf,fn)) >= 0) {
#if	CF_DEBUGS
	            DEBUGPRINTF("pcsgetnames/getname_userhome: tbuf=%s\n",
	                tbuf) ;
#endif
	            rs = filereadln(tbuf,sip->rbuf,sip->rlen) ;
	            if (isNotPresent(rs)) rs = SR_OK ;
	        }
	    } /* end if (getuserhome) */
	} /* end if (non-null) */

#if	CF_DEBUGS
	DEBUGPRINTF("pcsgetnames/getname_userhome: ret rs=%d\n",rs) ;
#endif

	return rs ;
} /* end subroutine (getname_userhome) */

local int getname_again(SI *sip) noex {
	int		rs = SR_OK ;
	if (sip->w == pcsnsreq_fullname) {
	    sip->w = pcsnsreq_pcsname ;
	    rs = getname(sip) ;
	}
	return rs ;
} /* end subroutine (getname_again) */

local int getname_sysdb(SI *sip) noex {
	PCSNSO		*op = sip->op ;
	cint	w = sip->w ;
	int		rs ;
	int		len = 0 ;

	if ((rs = pcsnso_getpw(op,sip->un)) >= 0) {
	    PCSNSO_PWD		*pdp = &op->pwd ;
	    int			nlen ;
	    char		*nbuf ;
	    switch (w) {
	    case pcsnsreq_realname:
	    case pcsnsreq_pcsname:
	    case pcsnsreq_fullname:
	        {
	            cchar	*gecos = pdp->pw.pw_gecos ;
	            nlen = (lenstr(gecos)+10) ;
	            if ((rs = lm_mall((nlen+1),&nbuf)) >= 0) {
	                if ((rs = mkgecosname(nbuf,nlen,gecos)) > 0) {
	                    rs = mkrealname(sip->rbuf,sip->rlen,nbuf,rs) ;
	                    len = rs ;
	                }
	                lm_free(nbuf) ;
	            } /* end if (memory-allocation) */
	        } /* end block */
	        break ;
	    case pcsnsreq_projinfo:
	        rs = getprojinfo_sysdb(sip) ;
	        len = rs ;
	        break ;
	    case pcsnsreq_pcsorg:
	        {
	            GECOS	g ;
	            cchar	*gecos = pdp->pw.pw_gecos ;
	            if ((rs = gecos_start(&g,gecos,-1)) >= 0) {
	                int		vl ;
	                cint	gi = gecosval_organization ;
	                cchar	*vp ;
	                if ((vl = gecos_getval(&g,gi,&vp)) > 0) {
	                    rs = sncpy1w(sip->rbuf,sip->rlen,vp,vl) ;
	                    len = rs ;
	                }
	                gecos_finish(&g) ;
	            } /* end if (GECOS) */
	        } /* end block */
	        break ;
	    } /* end switch */
	} /* end if */
	DEBUGPRINTF("rn=>%r<\n",sip->rbuf,sip->rlen) ;
	DEBUGPRINTF("ret rs=%d len=%u\n",rs,len) ;
	return (rs >= 0) ? len : rs ;
} /* end subroutine (getname_sysdb) */

local int getname_pcsdef(SI *sip) noex {
	PCSNSO		*op = sip->op ;
	cint	w = sip->w ;
	int		rs = SR_OK ;
	int		len = 0 ;
	cchar		*fn = nullptr ;

	switch (w) {
	case pcsnsreq_projinfo:
	    fn = PCSDPIFNAME ;
	    break ;
	case pcsnsreq_pcsorg:
	    fn = PRORGFNAME ;
	    break ;
	} /* end switch */

	if ((rs >= 0) && (fn != nullptr)) {
	    switch (w) {
	    case pcsnsreq_projinfo:
	        {
	            PCSNSO_PWD	*pdp = &op->pwd ;
	            cchar	*un = sip->un ;
	            if ((rs = pcsnso_getpw(op,un)) >= 0) {
	                const uid_t	uid = pdp->pw.pw_uid ;
	                if (uid >= NSYSPIDS) {
	                    rs = subinfo_prfile(sip,fn) ;
	                    len = rs ;
	                } /* end if (system UID) */
	            } /* end if (pcsnso_getpw) */
	        }
	        break ;
	    case pcsnsreq_pcsorg:
	        {
	            rs = subinfo_prfile(sip,fn) ;
	            len = rs ;
	        }
	        break ;
	    } /* end switch */
	} /* end if (appropriate) */

	return (rs >= 0) ? len : rs ;
} /* end subroutine (getname_pcsdef) */

local int getprojinfo_sysdb(SI *sip) noex {
	PCSNSO		*op = sip->op ;
	int		rs ;
	int		rs1 ;
	int		len = 0 ;

#if	CF_DEBUGS
	DEBUGPRINTF("pcsgetnames/getprojinfo_sysdb: un=%d\n",sip->un) ;
#endif

	if ((rs = bufsizeget(bufsize_pj)) >= 0) {
	    PROJECT	pj ;
	    cint	pjlen = rs ;
	    char	*pjbuf ;
	    if ((rs = lm_mall((pjlen+1),&pjbuf)) >= 0) {
	        cchar	*un = sip->un ;
	        if ((rs = uc_getprojdef(&pj,pjbuf,pjlen)) >= 0) {
	            int	f = (strcmp(pj.pj_name,DEFPROJNAME) != 0) ;
	            if (f) {
	                PCSNSO_PWD	*pdp = &op->pwd ;
	                if ((rs = pcsnso_getpw(op,un)) >= 0) {
	                    const uid_t	uid = pdp->pw.pw_uid ;
	                    f = (uid >= NSYSPIDS) ;
	                }
	            }
	            if ((rs >= 0) && f) {
	                cchar	*comment = pj.pj_comment ;
	                rs = sncpy1(sip->rbuf,sip->rlen,comment) ;
	                len = rs ;
	            }
	        } else if (isNotPresent(rs)) {
	            rs = SR_OK ;
	        }
	        rs1 = lm_free(pjbuf) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (memory-allocation) */
	} /* end if (bufsizeget) */

	DEBUGPRINTF("ret rs=%d len=%u\n",rs,len) ;
	return (rs >= 0) ? len : rs ;
} /* end subroutine (getprojinfo_sysdb) */


