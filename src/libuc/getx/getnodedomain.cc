/* getnodedomain SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* get the local node-name and INET domain-name */
/* version %I% last-modified %G% */

#define	CF_DEBUG	0		/* debugging */
#define	CF_GUESS	1		/* try to guess domain names? */

/* revision history:

	= 1998-07-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Names:
	getnodedomain
    	getsysdomain

	Description:
	Get the local host node name and INET domain name.

	Synopsis:
	int getnodedomain	(char *nrbuf,char *drbuf) noex
    	int getsysdomain	(char *rbuf,int rlen) noex

	Arguments:
	nrbuf		result buffer to receive the local node name
	drbuf		result buffer to receive the local INET domain name
	rbuf		result buffer pointer
	rlen		result buffer length

	Returns:
	SR_OK		if OK
	SR_NOTFOUND	if could not get something needed for 
			correct operation

	The algorithm for finding the local nodename is:
	1. use the first component of the environment variable NODE
	2. use the first component of the nodename returned from
	the system

	NOTE: Searching for the "current" domain is not an easy
	task and never has been.  There is no easy way to find out
	the domain part of the hostname for the current machine
	node.

	We use the following algorithm for finding the local domain:

	1. use the environment variable DOMAIN

	2. use the first component of the LOCALDOMAIN environment variable

	3. see if the environment variable NODE has a domain

	4. get the node name from the system and see if it has a domain

	5. lookup the system node name with the system resolver functions
	   and grab the first name for the current node that has a domain
	   attached

	6. use the domainname given in the resolver configuration file
           (keyword 'domain') if it is not corrupted

	7. use the first component of the 'search' keyword from the
	   resolver configuration file (NOT YET IMPLEMENTED!)

	8. we try to guess what the domain name is from the given
	   node name

	9. return that we could not find a domain for the current node!

	Implemention-notes:
	1. If a node-name is **not** requested by the caller, we
	need to allocated a buffer to hold a temporary resuling
	node-name anyway.  This is needed to that when serching for
	a domain-name, if one was also requested, I can use the
	information in the stored node-name information for a
	possible domain-name.  The stored note-name information
	might contain an appended domain-name.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<fcntl.h>		/* POSIX® |O_RDONLY| */
#include	<climits>		/* CSTD |INT_MAX| */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstring>		/* CSTD */
#include	<algorithm>		/* C++STD |min(3c++)| + |max(3c++)| */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<ucinfo.h>		/* LIBUC */
#include	<uclibmem.h>		/* LIBUC */
#include	<ucopen.h>		/* LIBUC */
#include	<ucdesc.h>		/* LIBUC */
#include	<ucgetho.h>		/* LIBUC */
#include	<bufsizevar.hh>		/* LIBUC */
#include	<estrings.h>		/* LIBUC most all string subroutines */
#include	<filer.h>		/* LIBUC */
#include	<strn.h>		/* LIBUC */
#include	<sfx.h>			/* LIBUC */
#include	<sncpyx.h>		/* LIBUC */
#include	<snwcpy.h>		/* LIBUC */
#include	<strdcpyx.h>		/* LIBUC */
#include	<char.h>		/* LIBUC */
#include	<isnot.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */
#include	<deb.hh>		/* LIBU |DEBPRINTF(3u)| */

#include	"getnodedomain.h"

#pragma		GCC dependency		"mod/libutil.ccm"
#pragma		GCC dependency		"mod/uconstants.ccm"
#pragma		GCC dependency		"mod/deb.ccm"

import libutil ;			/* |memclear(3u)| + |lenstr(3u)| */
import uconstants ;			/* |varname(3u)| */
import deb ;				/* debugging */

/* local defines */

#define	RESOLVFNAME	"/etc/resolv.conf"
#define	LOCALHOSTNAME	"localhost"

#define	FILERLEN	1024		/* initial size for FILER */
#define	TO_READ		30		/* time-out for read */

#define	TRY		tryer

#define	ISWHT(ch)	CHAR_ISWHITE(ch)

#ifndef	CF_DEBUG
#define	CF_DEBUG	0		/* debugging */
#endif
#ifndef	CF_GUESS
#define	CF_GUESS	0
#endif


/* imported namespaces */

using std::min ;			/* subroutine-template */
using std::max ;			/* subroutine-template */
using libuc::libmem ;			/* variable */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct tryer_fl {
	uint		initvarnode:1 ;
	uint		inituname:1 ;
	uint		initnode:1 ;
	uint		varnode:1 ;
	uint		uname:1 ;
	uint		node:1 ;
    } ; /* end struct (tryer_flags) */
    struct tryer {
	char		*nrbuf ;	/* passed caller argument (returned) */
	char		*drbuf ;	/* passed caller argument (returned) */
	char		*ntbuf ;	/* allocation tracking */
	cchar		*varnode ;
	cchar		*sysnodename ;
	tryer_fl	fl ;
	int		dlen ;
    } ; /* end struct (try) */
    struct guess {
	cchar		*name ;
	cchar		*domain ;
    } ; /* end struct (guess) */
} /* end namespace */

typedef int (*try_f)(TRY *) noex ;


/* forward references */

local int	getnode		(TRY *,charp) noex ;
local int	getdomain	(TRY *,charp) noex ;

local int	try_start	(TRY *,char *,char *,int) noex ;
local int	try_finish	(TRY *) noex ;
local int	try_resolvefd	(TRY *,char *,int,int) noex ;
local int	try_resolvefile	(TRY *,cchar *) noex ;

local int	try_initvarnode	(TRY *) noex ;
local int	try_inituname	(TRY *) noex ;
local int	try_initnode	(TRY *) noex ;
local int	try_vardomain	(TRY *) noex ;
local int	try_varlocaldom	(TRY *) noex ;
local int	try_varnode	(TRY *) noex ;
local int	try_uname	(TRY *) noex ;
local int	try_gethost	(TRY *) noex ;
local int	try_resolve	(TRY *) noex ;
local int	try_guess	(TRY *) noex ;

local con guess	*guesser	(ccharp) noex ;

local int	rmwhitedot	(char *,int) noex ;


/* local variables */

constexpr try_f		tries[] = {
	try_vardomain,
	try_varlocaldom,
	try_varnode,
	try_uname,
	try_gethost,
	try_resolve,
	try_guess,
	nullptr
} ; /* end array */

constexpr try_f		systries[] = {
	try_uname,
	try_gethost,
	try_resolve,
	try_guess,
	nullptr
} ; /* end array */

constexpr cpcchar	resolvefnames[] = {
	RESOLVFNAME,			/* most operating systems */
	"/var/run/resolv.conf",		/* for example: MacOS */
	nullptr
} ; /* end array */

constexpr guess		ga[] = {
	{ "rc",		"rightcore.com" },
	{ "rca",	"rightcore.com" },
	{ "rcb",	"rightcore.com" },
	{ "rcc",	"rightcore.com" },
	{ "rcd",	"rightcore.com" },
	{ "rce",	"rightcore.com" },
	{ "rcf",	"rightcore.com" },
	{ "rcg",	"rightcore.com" },
	{ "jig",	"rightcore.com" },
	{ nullptr,	nullptr }
} ; /* end array (ga) */

static bufsizevar	maxnodelen	(bufsize_nn) ;
static bufsizevar	maxhostlen	(bufsize_hostname) ;
constexpr bool		f_debug		= CF_DEBUG ;
constexpr bool		f_guess		= CF_GUESS ;


/* exported variables */


/* exported subroutines */

int getnodedomain(char *nrbuf,char *drbuf) noex {
	int		rs ;
	int		rs1 ;
	DEBPRINTF("ent\n") ;
	if ((rs = maxhostlen) >= 0) ylikely {
	    if (TRY ti ; (rs = try_start(&ti,nrbuf,drbuf,rs)) >= 0) ylikely {
		if ((rs = getnode(&ti,nrbuf)) >= 0) {
		    rs = getdomain(&ti,drbuf) ;
		} /* end */
	        rs1 = try_finish(&ti) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (try) */
	} /* end if (maxhostlen) */
	DEBPRINTF("ret rs=%d\n",rs) ;
	return rs ;
} /* end subroutine (getnodedomain) */

int getsysdomain(char *drbuf,int drlen) noex {
    	cnullptr	np{} ;
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		len = 0 ; /* return-value */
	if (drbuf) ylikely {
	    drbuf[0] = eos ;
	    if (TRY ti ; (rs = try_start(&ti,np,drbuf,drlen)) >= 0) ylikely {
		rs = SR_OK ; /* <- preparation for below */
	        for (int i = 0 ; (rs == SR_OK) && systries[i] ; i += 1) {
	            rs = (*systries[i])(&ti) ;
	        } /* end for */
		/* remove any stupid trailing dots if any */
	        if ((rs >= 0) && (drbuf[0] != '\0')) {
	            rs = rmwhitedot(drbuf,rs) ;
		    len = rs ;
	        } /* end */
	        rs1 = try_finish(&ti) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (try) */
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
} /* end subroutine (getsysdomain) */

int getinetdomain(char *drbuf,int drlen) noex {
    	cnullptr	np{} ;
    	int		rs ;
	int		rs1 ;
	int		len = 0 ; /* return-value */
	if (char *tbuf ; (rs = libmem.hostname(&tbuf)) >= 0) {
	    if ((rs = getnodedomain(np,tbuf)) >= 0) {
		rs = sncpy(drbuf,drlen,tbuf,rs) ;
		len = rs ;
	    } /* end if (getnodedomain) */
	    rs1 = libmem.free(tbuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return (rs >= 0) ? len : rs ;
} /* end subroutine (getsysdomain) */


/* local subroutines */

/* do we need a nodename? */
local int getnode(TRY *tip,charp nrbuf) noex {
    	int		rs = SR_OK ;
	if (nrbuf) {
	    DEBPRINTF("req node\n") ;
	    nrbuf[0] = eos ;
	    if (! tip->fl.initnode) {
	        rs = try_initnode(tip) ;
	    } /* end */
	} /* end if (trying to get a nodename) */
	return rs ;
} /* end subroutine (getnode) */

/* do we need a domainname? */
local int getdomain(TRY *tip,charp drbuf) noex {
    	int		rs = SR_OK ;
	if (drbuf) {
	    DEBPRINTF("req domain\n") ;
	    drbuf[0] = eos ;
	    rs = SR_OK ; /* <- need for check below */
	    for (int i = 0 ; (rs == SR_OK) && tries[i] ; i += 1) {
		DEBPRINTF("try i=%d\n",i) ;
	        rs = (*tries[i])(tip) ;
	    } /* end for */
	    /* remove any stupid trailing dots if any */
	    if ((rs >= 0) && (drbuf[0] != '\0')) {
	        rs = rmwhitedot(drbuf,rs) ;
	    } /* end */
	} /* end if (trying to get a domainname) */
	return rs ;
} /* end subroutine (getdomain) */

local int try_start(TRY *tip,char *nb,char *db,int dl) noex {
	int		rs = SR_FAULT ;
	if (tip) ylikely {
	    int sz = 0 ;
	    memclear(tip) ;
	    if ((rs = maxnodelen) >= 0) ylikely {
		cint nl = rs ;
		sz += rs ;
	        if ((rs = maxhostlen) >= 0) ylikely {
		    cint hl = rs ;
	            tip->fl = {} ;
	            tip->nrbuf = nb ;
	            tip->drbuf = db ;
	            tip->varnode = nullptr ;
	            tip->sysnodename = nullptr ;
		    tip->dlen = (dl >= 0) ? dl : hl ;
	            if (nb == nullptr) {
		        int ai = 1 ;
	                if (char *bp ; (rs = lm_mall(sz,&bp)) >= 0) ylikely {
			    *bp = eos ;
		            tip->ntbuf = bp + (--ai * nl) ;
			    tip->nrbuf = tip->ntbuf ;
		        } /* memory-acquire) */
	            } /* end if (memory-acquire) */
	        } /* end if (maxhostlen) */
	    } /* end if (maxnodelen) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (try_start) */

local int try_finish(TRY *tip) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (tip) ylikely {
	    rs = SR_OK ;
	    if (tip->sysnodename) {
		void *vp = voidp(tip->sysnodename) ;
	        rs1 = libmem.free(vp) ;
	        if (rs >= 0) rs = rs1 ;
	        tip->sysnodename = nullptr ;
	    } /* end */
	    if (tip->ntbuf) {
	        rs1 = lm_free(tip->ntbuf) ;
	        if (rs >= 0) rs = rs1 ;
	        tip->ntbuf = nullptr ;
	    } /* end if (memory-release) */
	    tip->varnode = nullptr ;
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (try_finish) */

local int try_initvarnode(TRY *tip) noex {
	int		rs = SR_FAULT ;
	DEBPRINTF("ent\n") ;
	if (tip) ylikely {
	    rs = SR_OK ;
	    if (! tip->fl.initvarnode) {
	        static ccharp v_node = getenver(varname.node) ;
	        tip->fl.initvarnode = true ;
		if (v_node) {
	            tip->fl.varnode = true ;
	            tip->varnode = v_node ;
		    DEBPRINTF("v_node=%s\n",v_node) ;
	        } /* end */
	    } /* end */
	} /* end if (non-null) */
	DEBPRINTF("ret rs=%d\n",rs) ;
	return rs ;
} /* end subroutine (try_initvarnode) */

local int try_inituname(TRY *tip) noex {
	int		rs = SR_FAULT ;
	if (tip) ylikely {
	    rs = SR_OK ;
	    if (! tip->fl.inituname) {
	        tip->fl.inituname = true ;
	        if (ucinfo_names uin ; (rs = ucinfo_name(&uin)) >= 0) ylikely {
	            ccharp	sp = uin.nodename ;
	            int		sl = lenstr(uin.nodename) ;
	            if (cc *cp ; (rs = libmem.strw(sp,sl,&cp)) >= 0) {
	                tip->fl.uname = true ;
	                tip->sysnodename = cp ;
	                rs = 0 ;
	            } /* end if (memory-acquire) */
	        } /* end if (uname) */
	    } /* end if (needed initialization) */
	} /* end if (magic) */
	return rs ;
} /* end subroutine (try_inituname) */

local int try_initnode(TRY *tip) noex {
	int		rs = SR_FAULT ;
	DEBPRINTF("ent\n") ;
	if (tip) ylikely {
	    rs = SR_OK ;
	    if (! tip->fl.initnode) {
	        tip->fl.initnode = true ;
	        if ((rs = maxnodelen) >= 0) ylikely {
		    cint maxnode = rs ;
	            int		sl = -1 ;
	            ccharp	sp = nullptr ;
	            cchar	*cp ;
	            if ((rs >= 0) && (sp == nullptr)) {
	                if (! tip->fl.initvarnode) {
		            rs = try_initvarnode(tip) ;
		        }
	                cp = tip->varnode ;
	                if (tip->fl.varnode && cp && (cp[0] != '\0')) {
		            sp = cp ;
		        }
	            } /* end if (ok) */
		    DEBPRINTF("mid1 rs=%d sp=%s\n",rs,sp) ;
	            if ((rs >= 0) && (sp == nullptr)) {
	                if (! tip->fl.inituname) {
			    rs = try_inituname(tip) ;
		        }
	                if (rs >= 0) {
	                    cp = tip->sysnodename ;
	                    if (tip->fl.inituname && cp && (cp[0] != '\0')) {
			        sp = cp ;
		            }
	                } /* end if (ok) */
	            } /* end if (ok) */
		    DEBPRINTF("mid2 rs=%d sp=%s\n",rs,sp) ;
	            if (rs >= 0) ylikely {
	                if (sp != nullptr) {
			    int cl = -1 ;
		    DEBPRINTF("mid2 go sl=%d sp=%s\n",sl,sp) ;
		    DEBPRINTSTR("mid2 go ss=",sp,sl) ;
	                    if ((cl = sfwhitedot(sp,sl,&cp)) >= 0) {
				sp = cp ;
				sl = cl ;
			    } /* end if (sfwhitedot) */
			    {
			        ccp rp{} ;
		    DEBPRINTSTR("mid2 res ss=",sp,sl) ;
	                        tip->fl.node = true ;
	                        rp = strdcpy1w(tip->nrbuf,maxnode,sp,sl) ;
				rs = intconv(rp - tip->nrbuf) ;
		    DEBPRINTF("mid2 nrbuf=%s\n",tip->nrbuf) ;
	                    } /* end block */
		        } else {
		            rs = SR_IDRM ;
		        } /* end if (error) */
	            } /* end if (ok) */
		    DEBPRINTF("mid3 rs=%d\n",rs) ;
	        } /* end if (maxnodelen) */
	    } /* end if (needed initialization) */
	} /* end if (non-null) */
	DEBPRINTF("ret rs=%d\n",rs) ;
	return (rs >= 0) ? 0 : rs ;
} /* end subroutine (try_initnode) */

local int try_vardomain(TRY *tip) noex {
	static cchar	*vap = getenver(varname.domain) ;
	int		rs = SR_OK ;
	int		len = 0 ;
	DEBPRINTF("ent\n") ;
	if (vap) {
	    cchar	*cp{} ;
	    if (int cl ; (cl = sfshrink(vap,-1,&cp)) > 0) {
		cint	dlen = tip->dlen ;
	        rs = snwcpy(tip->drbuf,dlen,cp,cl) ;
		len = rs ;
	        DEBPRINTF("drbuf=%s\n",tip->drbuf) ;
	    } /* end */
	} /* end if */
	DEBPRINTF("ret rs=%d len=%d\n",rs,len) ;
	return (rs >= 0) ? len : rs ;
} /* end subroutine (try_vardomain) */

local int try_varlocaldom(TRY *tip) noex {
	static cchar	*vap = getenver(varname.localdomain) ;
	int		rs = SR_OK ;
	int		len = 0 ;
	if (vap) {
	    int		cl ;
	    cchar	*sp = vap ;
	    cchar	*cp ;
	    while (ISWHT(*sp)) {
	        sp += 1 ;
	    } /* end while */
	    cp = sp ;
	    while (*sp && (! ISWHT(*sp)) && (*sp != ':')) {
	        sp += 1 ;
	    } /* end while */
	    if ((cl = intconv(sp - cp)) > 0) {
		cint	dlen = tip->dlen ;
	        rs = snwcpy(tip->drbuf,dlen,cp,cl) ;
		len = rs ;
	    } /* end if */
	} /* end if (localdomain) */
	return (rs >= 0) ? len : rs ;
} /* end subroutine (try_varlocaldom) */

local int try_varnode(TRY *tip) noex {
	int		rs = SR_OK ;
	int		len = 0 ;
	if (! tip->fl.initvarnode) {
	    rs = try_initvarnode(tip) ;
	} /* end if */
	if ((rs >= 0) && tip->fl.varnode) ylikely {
	    if (ccharp sp = tip->varnode ; sp[0]) {
		cint	dlen = tip->dlen ;
	        int sl = -1 ;
	        if (cchar *tp = strochr(sp,'.')) {
		    sl = intconv(tp - sp) ;
	        } /* end */
	        rs = snwcpy(tip->drbuf,dlen,sp,sl) ;
		len = rs ;
	    } /* end if */
	} /* end if */
	return (rs >= 0) ? len : rs ;
} /* end subroutine (try_varnode) */

local int try_uname(TRY *tip) noex {
	int		rs = SR_OK ;
	int		len = 0 ;
	DEBPRINTF("ent\n") ;
	if (! tip->fl.inituname) {
	    rs = try_inituname(tip) ;
	} /* end */
	if ((rs >= 0) && tip->fl.uname) ylikely {
	    if (ccharp sp = tip->sysnodename ; sp[0]) {
	        DEBPRINTF("node=»%s«\n",sp) ;
	        if (cchar *tp = strochr(sp,'.')) {
	            if (cchar *cp = (tp + 1) ; cp[0]) {
		        cint	dlen = tip->dlen ;
	                rs = sncpy(tip->drbuf,dlen,cp) ;
		        len = rs ;
		    } /* end if (non-empty) */
	        } /* end if */
	    } /* end if */
	} /* end if */
	DEBPRINTF("ret rs=%d len=%d\n",rs,len) ;
	return (rs >= 0) ? len : rs ;
} /* end subroutine (try_uname) */

local int try_gethost(TRY *tip) noex {
	cnullptr	np{} ;
	int		rs = SR_OK ;
	int		rs1 ;
	int		len = 0 ;
	if (! tip->fl.initnode) {
	    rs = try_initnode(tip) ;
	} /* end */
	if ((rs >= 0) && tip->fl.node) ylikely {
	    if (char *hbuf ; (rs = lm_ho(&hbuf)) >= 0) ylikely {
	        cint		hlen = rs ;
	        cchar		*nn = tip->nrbuf ;
		if (ucentho he ; (rs = he.getnam(hbuf,hlen,nn)) >= 0) {
		    cint	dlen = tip->dlen ;
	            cchar	*tp{} ; /* used-multiple */
		    char	*dbuf = tip->drbuf ;
		    bool	f = true ;
		    rs = 0 ;
	            f = f && (he.h_name != np) ;
		    f = f && ((tp = strochr(he.h_name,'.')) != np) ;
		    if (f) {
	                rs = sncpy(dbuf,dlen,(tp + 1)) ;
			len = rs ;
	            } /* end if (official name) */
	            if ((rs == 0) && he.h_aliases) ylikely {
	                for (int i = 0 ; he.h_aliases[i] ; i += 1) {
			    cchar	*ap = he.h_aliases[i] ;
	                    if ((tp = strochr(ap,'.')) != np) {
	                        rs = sncpy(dbuf,dlen,(tp+1)) ;
				len = rs ;
	                    } /* end if */
			    if (rs != 0) break ;
	                } /* end for */
	            } /* end if (alias names) */
		    rs1 = lm_free(hbuf) ;
		    if (rs >= 0) rs = rs1 ;
		} /* end if (m-a-f) */
	    } else if (isNotPresent(rs)) {
	        rs = SR_OK ;
	    } /* end if */
	} /* end if (have node) */
	return (rs >= 0) ? len : rs ;
} /* end subroutine (try_gethost) */

local int try_resolve(TRY *tip) noex {
	int		rs = SR_OK ;
	for (int i = 0 ; (rs == SR_OK) && resolvefnames[i] ; i += 1) ylikely {
	    cchar	*fn = resolvefnames[i] ;
	    rs = try_resolvefile(tip,fn) ;
	} /* end for */
	return rs ;
} /* end subroutine (try_resolve) */

local int try_resolvefile(TRY *tip,cchar *fname) noex {
	int		rs ;
	int		rs1 ;
	int		len = 0 ;
	if (char *lbuf ; (rs = lm_ml(&lbuf)) >= 0) ylikely {
	    cint	llen = rs ;
	    if ((rs = uc_open(fname,O_RDONLY,0666)) >= 0) ylikely {
		cint	fd = rs ;
		{
		    rs = try_resolvefd(tip,lbuf,llen,fd) ;
		    len = rs ;
		}
	        rs1 = uc_close(fd) ;
		if (rs >= 0) rs = rs1 ;
	    } else if (isNotPresent(rs)) {
	        rs = SR_OK ;
	    } /* end */
	    rs1 = lm_free(lbuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return (rs >= 0) ? len : rs;
} /* end subroutine (try_resolvefile) */

local int tryreader(int,char *,int,cchar **) noex ;

local int try_resolvefd(TRY *tip,char *lbuf,int llen,int fd) noex {
	int		rs ;
	int		len = 0 ; /* return-value */
	if (cchar *dp{} ; (rs = tryreader(fd,lbuf,llen,&dp)) > 0) ylikely {
            if (dp) {
	        cint	dlen = tip->dlen ;
	        charp	dbuf = tip->drbuf ;
		len = rs ;
                rs = snwcpy(dbuf,dlen,dp,rs) ;
	    } /* end */
        } /* end if (tryreader) */
	return (rs >= 0) ? len : rs ;
} /* end subroutine (try_resolvefd) */

local int tryreader(int fd,char *lbuf,int llen,cchar **rpp) noex {
    	cint		bsz = FILERLEN ; /* buffer size */
	int		rs ;
	int		rs1 ;
	int		len = 0 ; /* return-value */
	cchar		key[] = "domain" ;
        if (filer b ; (rs = b.start(fd,0z,bsz,0)) >= 0) ylikely {
	    cint	to = TO_READ ;
            while ((rs = b.readln(lbuf,llen,to)) > 0) {
                if ((len = sfkeyval(lbuf,rs,key,rpp)) > 0) break ;
            } /* end while (reading lines) */
            rs1 = b.finish ;
            if (rs >= 0) rs = rs1 ;
        } /* end if (filer) */
	if (len < 0) len = 0 ;
	return (rs >= 0) ? len : rs ;
} /* end subroutine (tryreader) */

local int try_guess(TRY *tip) noex {
	int		rs = SR_OK ;
	int		len = 0 ;
	if_constexpr (f_guess) {
	    if (! tip->fl.initnode) {
	        rs = try_initnode(tip) ;
	    } /* end */
	    if ((rs >= 0) && tip->fl.node) {
	        cchar	*nn = tip->nrbuf ;
		if (con guess *resp = guesser(nn)) {
		    cint	mhl = maxhostlen ;
	            rs = sncpy(tip->drbuf,mhl,resp->domain) ;
		    len = rs ;
	        } /* end if */
	    } /* end if (have node) */
	} /* end if_constexpr (f_guess) */
	return (rs >= 0) ? len : rs ;
} /* end subroutine (try_guess) */

local con guess *guesser(ccharp nn) noex {
    	con guess	*resp = nullptr ;
	int	si = -1 ;
	int	m_max = 0 ;
	for (int i = 0 ; ga[i].name ; i += 1) {
	    ccharp	gnp = ga[i].name ;
	    cint	gnl = lenstr(ga[i].name) ;
	    if (int m ; (m = comstr(gnp,nn,-1)) >= gnl) {
	        if (m > m_max) {
	            m_max = m ;
	            si = i ;
	        }
	    } /* end if */
	} /* end for */
	if (si >= 0) {
	    resp = (ga + si) ;
	} /* end if */
	return resp ;
} /* end subroutine (guesser) */

/* remove trailing whitespace and dots */
local int rmwhitedot(char *bp,int bl) noex {
	cauto isrm = [] (int ch) -> bool {
	    return (ch == '.') || char_iswhite(ch) ;
	} ;
	while ((bl > 0) && isrm(bp[bl - 1])) {
	    bp[--bl] = eos ;
	} /* end while */
	return (bl > 0) ? bl : SR_NOTFOUND ;
} /* end subroutine (rmwhitedot) */


