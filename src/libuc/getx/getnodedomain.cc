/* getnodedomain SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* get the local node-name and INET domain name */
/* version %I% last-modified %G% */

#define	CF_GUESS	1	/* try to guess domain names? */

/* revision history:

	= 1998-07-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	getnodedomain

	Description:
	Get the local host node name and INET domain name.

	Synopsis:
	int getnodedomain(char *nodename,char *domainname) noex

	Arguments:
	nodename	buffer to receive the local node name
	domainname	buffer to receive the local INET domain name

	Returns:
	SR_OK		if OK
	SR_NOTFOUND	if could not get something needed for correct operation

	The algorithm for finding the local nodename is:
	1. use the first component of the environment variable NODE
	2. use the first component of the nodename returned from the system

	NOTE: Searching for the "current" domain is not an easy task and never
	has been.  There is no easy way to find out the domain part of the
	hostname for the current machine node.

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

	8. we try to guess what the domain name is from the given node name

	9. return that we couln't find a domain for the current node!

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<fcntl.h>		/* for |O_RDONLY| */
#include	<climits>		/* for |INT_MAX| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>		/* |getenv(3c)| */
#include	<cstring>		/* |strchr(3c)| */
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<usystem.h>
#include	<uinfo.h>
#include	<varnames.hh>
#include	<bufsizevar.hh>
#include	<mallocxx.h>
#include	<estrings.h>		/* most all string subroutines */
#include	<filer.h>
#include	<strn.h>
#include	<sfx.h>
#include	<sncpyx.h>
#include	<snwcpy.h>
#include	<strdcpyx.h>
#include	<nleadstr.h>
#include	<char.h>
#include	<isnot.h>
#include	<localmisc.h>

#include	"getnodedomain.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |memclear(3u)| + |lenstr(3u)| */

/* local defines */

#define	RESOLVFNAME	"/etc/resolv.conf"
#define	LOCALHOSTNAME	"localhost"

#define	FILERLEN	1024		/* initial size for FILER */
#define	TO_READ		30		/* time-out for read */

#undef	TRY
#define	TRY		tryer
#define	TRY_FL		tryer_flags

#ifndef	CF_GUESS
#define	CF_GUESS	0
#endif


/* imported namespaces */

using std::min ;			/* subroutine-template */
using std::max ;			/* subroutine-template */
using libuc::libmem ;			/* variable */
using std::nothrow ;			/* constant */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct tryer_flags {
	uint		initvarnode:1 ;
	uint		inituname:1 ;
	uint		initnode:1 ;
	uint		varnode:1 ;
	uint		uname:1 ;
	uint		node:1 ;
    } ; /* end struct (tryer_flags) */
    struct tryer {
	char		*nodename ;	/* passed caller argument (returned) */
	char		*domainname ;	/* passed caller argument (returned) */
	char		*nbuf ;
	cchar		*varnode ;
	cchar		*sysnodename ;
	TRY_FL		fl ;
	int		dlen ;
    } ; /* end struct (try) */
    struct guess {
	cchar		*name ;
	cchar		*domain ;
    } ; /* end struct (guess) */
} /* end namespace */


/* forward references */

static int	try_start(TRY *,char *,char *,int) noex ;
static int	try_finish(TRY *) noex ;
static int	try_resolvefd(TRY *,char *,int,int) noex ;

static int	try_initvarnode(TRY *) noex ;
static int	try_inituname(TRY *) noex ;
static int	try_initnode(TRY *) noex ;
static int	try_vardomain(TRY *) noex ;
static int	try_varlocaldomain(TRY *) noex ;
static int	try_varnode(TRY *) noex ;
static int	try_uname(TRY *) noex ;
static int	try_gethost(TRY *) noex ;
static int	try_resolve(TRY *) noex ;
static int	try_resolvefile(TRY *,cchar *) noex ;
static int	try_guess(TRY *) noex ;

static int	rmwhitedot(char *,int) noex ;


/* local variables */

constexpr int		(*tries[])(TRY *) = {
	try_vardomain,
	try_varlocaldomain,
	try_varnode,
	try_uname,
	try_gethost,
	try_resolve,
	try_guess,
	nullptr
} ;

constexpr int		(*systries[])(TRY *) = {
	try_uname,
	try_gethost,
	try_resolve,
	try_guess,
	nullptr
} ;

constexpr cpcchar	resolvefnames[] = {
	RESOLVFNAME,			/* most operating systems */
	"/var/run/resolv.conf",		/* for example: MacOS */
	nullptr
} ;

constexpr guess		ga[] = {
	{ "rc", "rightcore.com" },
	{ "rca", "rightcore.com" },
	{ "rcb", "rightcore.com" },
	{ "rcc", "rightcore.com" },
	{ "rcd", "rightcore.com" },
	{ "rce", "rightcore.com" },
	{ "rcf", "rightcore.com" },
	{ "rcg", "rightcore.com" },
	{ "jig", "rightcore.com" },
	{ nullptr, nullptr }
} ; /* end array (ga) */

static bufsizevar	maxnodelen(getbufsize_nn) ;
static bufsizevar	maxhostlen(getbufsize_hn) ;

constexpr bool		f_guess = CF_GUESS ;


/* exported variables */


/* exported subroutines */

int getnodedomain(char *nbuf,char *dbuf) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = maxhostlen) >= 0) ylikely {
	    if (TRY ti ; (rs = try_start(&ti,nbuf,dbuf,rs)) >= 0) ylikely {
		/* do we need a nodename? */
	        if (nbuf != nullptr) {
	            nbuf[0] = '\0' ;
	            if (! ti.fl.initnode) rs = try_initnode(&ti) ;
	        } /* end if (trying to get a nodename) */
		/* do we need a domainname? */
	        if ((rs >= 0) && (dbuf != nullptr)) {
	            dbuf[0] = '\0' ;
		    rs = SR_OK ;
	            for (int i = 0 ; (rs == SR_OK) && tries[i] ; i += 1) {
	                rs = (*tries[i])(&ti) ;
	            } /* end for */
	            /* remove any stupid trailing dots if any */
	            if ((rs >= 0) && (dbuf[0] != '\0')) {
	                rs = rmwhitedot(dbuf,rs) ;
		    }
	        } /* end if (trying to get a domainname) */
	        rs1 = try_finish(&ti) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (try) */
	} /* end if (maxhostlen) */
	return rs ;
}
/* end subroutine (getnodedomain) */

int getsysdomain(char *dbuf,int dlen) noex {
    	cnullptr	np{} ;
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		len = 0 ;
	if (dbuf) ylikely {
	    dbuf[0] = '\0' ;
	    if (TRY ti ; (rs = try_start(&ti,np,dbuf,dlen)) >= 0) ylikely {
		/* do we need a domainname? */
		rs = SR_OK ;
	        for (int i = 0 ; (rs == SR_OK) && systries[i] ; i += 1) {
	            rs = (*systries[i])(&ti) ;
	        } /* end for */
		/* remove any stupid trailing dots if any */
	        if ((rs >= 0) && (dbuf[0] != '\0')) {
	            rs = rmwhitedot(dbuf,rs) ;
		    len = rs ;
	        }
	        rs1 = try_finish(&ti) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (try) */
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (getsysdomain) */

int getuserdomain(char *dbuf,int dlen) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		len = 0 ;
	if (dbuf) ylikely {
	    if (char *dn ; (rs = malloc_hn(&dn)) >= 0) ylikely {
	        if (dlen < 0) dlen = rs ;
	        if ((rs = getnodedomain(nullptr,dn)) >= 0) ylikely {
	            rs = sncpy(dbuf,dlen,dn) ;
		    len = rs ;
	        }
	        rs1 = malloc_free(dn) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (m-a-f) */
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (getuserdomain) */


/* local subroutines */

static int try_start(TRY *tip,char *nb,char *db,int dl) noex {
	int		rs = SR_FAULT ;
	if (tip) ylikely {
	    memclear(tip) ;
	    if ((rs = maxhostlen) >= 0) ylikely {
		cint	hl = rs ;
	        tip->fl = {} ;
	        tip->domainname = db ;
	        tip->varnode = nullptr ;
	        tip->sysnodename = nullptr ;
	        tip->nodename = nb ;
		tip->dlen = (dl < 0) ? hl : dl ;
	        if (nb == nullptr) {
	            if (char *bp ; (rs = malloc_nn(&bp)) >= 0) ylikely {
		       tip->nbuf = bp ;
	    	       tip->nodename = bp ;
		    } /* memory-allocation) */
	        } /* end if (memory-allocation) */
	        if (rs >= 0) tip->nodename[0] = '\0' ;
	    } /* end if (maxhostlen) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (try_start) */

static int try_finish(TRY *tip) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (tip) ylikely {
	    rs = SR_OK ;
	    if (tip->sysnodename) {
		void *vp = voidp(tip->sysnodename) ;
	        rs1 = libmem.free(vp) ;
	        if (rs >= 0) rs = rs1 ;
	        tip->sysnodename = nullptr ;
	    }
	    if (tip->nbuf) {
	        rs1 = malloc_free(tip->nbuf) ;
	        if (rs >= 0) rs = rs1 ;
	        tip->nbuf = nullptr ;
	    }
	    tip->varnode = nullptr ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (try_finish) */

static int try_initvarnode(TRY *tip) noex {
	int		rs = SR_FAULT ;
	if (tip) ylikely {
	    rs = SR_OK ;
	    if (! tip->fl.initvarnode) {
	        static cchar	*val = getenv(varname.node) ;
	        tip->fl.initvarnode = true ;
		if (val) {
	            tip->fl.varnode = true ;
	            tip->varnode = val ;
	        }
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (try_initvarnode) */

static int try_inituname(TRY *tip) noex {
	int		rs = SR_FAULT ;
	if (tip) ylikely {
	    rs = SR_OK ;
	    if (! tip->fl.inituname) {
	        tip->fl.inituname = true ;
	        if (uinfo_names uin ; (rs = uinfo_name(&uin)) >= 0) ylikely {
	            cchar	*sp = uin.nodename ;
	            int		sl = lenstr(uin.nodename) ;
	            if (cc *cp ; (rs = libmem.strw(sp,sl,&cp)) >= 0) {
	                tip->fl.uname = true ;
	                tip->sysnodename = cp ;
	                rs = 0 ;
	            } /* end if (memory-allocation) */
	        } /* end if (uname) */
	    } /* end if (needed initialization) */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (try_inituname) */

static int try_initnode(TRY *tip) noex {
	int		rs = SR_FAULT ;
	if (tip) ylikely {
	    rs = SR_OK ;
	    if (! tip->fl.initnode) {
	        tip->fl.initnode = true ;
	        if ((rs = maxnodelen) >= 0) ylikely {
	            int		sl = -1 ;
	            cchar	*sp = nullptr ;
	            cchar	*cp ;
	            if ((rs >= 0) && (sp == nullptr)) {
	                if (! tip->fl.initvarnode) {
		            rs = try_initvarnode(tip) ;
		        }
	                cp = tip->varnode ;
	                if (tip->fl.varnode && (cp) && (cp[0] != '\0')) {
		            sp = cp ;
		        }
	            } /* end if (ok) */
	            if ((rs >= 0) && (sp == nullptr)) {
	                if (! tip->fl.inituname) {
			    rs = try_inituname(tip) ;
		        }
	                if (rs >= 0) {
	                    cp = tip->sysnodename ;
	                    if (tip->fl.inituname && cp && (cp[0] != '\0')) {
			        sp = cp ;
		            }
	                }
	            } /* end if (ok) */
	            if (rs >= 0) ylikely {
	                if (sp != nullptr) {
	                    if (int cl ; (cl = sfwhitedot(sp,sl,&cp)) > 0) {
	                        tip->fl.node = true ;
	                        strdcpy1w(tip->nodename,maxnodelen,cp,cl) ;
	                    }
		        } else {
		            rs = SR_IDRM ;
		        }
	            } /* end if (ok) */
	        } /* end if (maxnodelen) */
	    } /* end if (needed initialization) */
	} /* end if (non-null) */
	return (rs >= 0) ? 0 : rs ;
}
/* end subroutine (try_initnode) */

static int try_vardomain(TRY *tip) noex {
	static cchar	*val = getenv(varname.domain) ;
	int		rs = SR_OK ;
	int		len = 0 ;
	if (val) {
	    cchar	*cp{} ;
	    if (int cl ; (cl = sfshrink(val,-1,&cp)) > 0) {
		cint	dlen = tip->dlen ;
	        rs = snwcpy(tip->domainname,dlen,cp,cl) ;
		len = rs ;
	    }
	} /* end if */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (try_vardomain) */

static int try_varlocaldomain(TRY *tip) noex {
	static cchar	*val = getenv(varname.localdomain) ;
	int		rs = SR_OK ;
	int		len = 0 ;
	if (val) {
	    int		cl ;
	    cchar	*sp = val ;
	    cchar	*cp ;
	    while (CHAR_ISWHITE(*sp)) {
	        sp += 1 ;
	    }
	    cp = sp ;
	    while (*sp && (! CHAR_ISWHITE(*sp)) && (*sp != ':')) {
	        sp += 1 ;
	    }
	    if ((cl = intconv(sp - cp)) > 0) {
		cint	dlen = tip->dlen ;
	        rs = snwcpy(tip->domainname,dlen,cp,cl) ;
		len = rs ;
	    }
	} /* end if (localdomain) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (try_varlocaldomain) */

static int try_varnode(TRY *tip) noex {
	int		rs = SR_OK ;
	int		len = 0 ;
	if (! tip->fl.initvarnode) {
	    rs = try_initvarnode(tip) ;
	}
	if ((rs >= 0) && tip->fl.varnode) ylikely {
	    cchar	*sp = tip->varnode ;
	    if (cchar *tp ; (tp = strchr(sp,'.')) != nullptr) {
	        cchar	*cp = (tp + 1) ;
	        if (cp[0] != '\0') {
		    cint	dlen = tip->dlen ;
	            rs = sncpy(tip->domainname,dlen,cp) ;
		    len = rs ;
		}
	    }
	}
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (try_varnode) */

static int try_uname(TRY *tip) noex {
	int		rs = SR_OK ;
	int		len = 0 ;
	if (! tip->fl.inituname) {
	    rs = try_inituname(tip) ;
	}
	if ((rs >= 0) && tip->fl.uname) ylikely {
	    cchar	*sp = tip->sysnodename ;
	    if (cchar *tp ; (tp = strchr(sp,'.')) != nullptr) {
	        cchar	*cp = (tp + 1) ;
	        if (cp[0] != '\0') {
		    cint	dlen = tip->dlen ;
	            rs = sncpy(tip->domainname,dlen,cp) ;
		    len = rs ;
		}
	    }
	} /* end if */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (try_uname) */

static int try_gethost(TRY *tip) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	int		len = 0 ;
	if (! tip->fl.initnode) {
	    rs = try_initnode(tip) ;
	}
	if ((rs >= 0) && tip->fl.node) ylikely {
	    if (char *hbuf ; (rs = malloc_ho(&hbuf)) >= 0) ylikely {
		cnullptr	np{} ;
	        ucentho		he, *hep = &he ;
	        cint		hlen = rs ;
	        cchar		*nn = tip->nodename ;
	        if ((rs = uc_gethonam(&he,hbuf,hlen,nn)) >= 0) ylikely {
		    cint	dlen = tip->dlen ;
	            cchar	*tp{} ;
		    char	*dbuf = tip->domainname ;
		    bool	f = true ;
		    rs = 0 ;
	            f = f && (hep->h_name != np) ;
		    f = f && ((tp = strchr(hep->h_name,'.')) != np) ;
		    if (f) {
	                rs = sncpy(dbuf,dlen,(tp + 1)) ;
			len = rs ;
	            } /* end if (official name) */
	            if ((rs == 0) && (hep->h_aliases != nullptr)) ylikely {
	                for (int i = 0 ; hep->h_aliases[i] ; i += 1) {
			    cchar	*ap = hep->h_aliases[i] ;
	                    if ((tp = strchr(ap,'.')) != nullptr) {
	                        rs = sncpy(dbuf,dlen,(tp+1)) ;
				len = rs ;
	                    } /* end if */
			    if (rs != 0) break ;
	                } /* end for */
	            } /* end if (alias names) */
		    rs1 = malloc_free(hbuf) ;
		    if (rs >= 0) rs = rs1 ;
		} /* end if (m-a-f) */
	    } else if (isNotPresent(rs)) {
	        rs = SR_OK ;
	    }
	} /* end if (have node) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (try_gethost) */

static int try_resolve(TRY *tip) noex {
	int		rs = SR_OK ;
	for (int i = 0 ; (rs == SR_OK) && resolvefnames[i] ; i += 1) ylikely {
	    cchar	*fn = resolvefnames[i] ;
	    rs = try_resolvefile(tip,fn) ;
	} /* end for */
	return rs ;
}
/* end subroutine (try_resolve) */

static int try_resolvefile(TRY *tip,cchar *fname) noex {
	int		rs ;
	int		rs1 ;
	int		len = 0 ;
	if (char *lbuf ; (rs = malloc_ml(&lbuf)) >= 0) ylikely {
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
	    }
	    rs1 = malloc_free(lbuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return (rs >= 0) ? len : rs;
}
/* end subroutine (try_resolvefile) */

static int tryreader(int,char *,int,cchar **) noex ;

static int try_resolvefd(TRY *tip,char *lbuf,int llen,int fd) noex {
	int		rs ;
	int		len = 0 ; /* return-value */
	if (cchar *dp{} ; (rs = tryreader(fd,lbuf,llen,&dp)) > 0) ylikely {
            if (dp) {
	        cint	dlen = tip->dlen ;
	        char	*dbuf = tip->domainname ;
		len = rs ;
                rs = snwcpy(dbuf,dlen,dp,rs) ;
	    }
        } /* end if (tryreader) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (try_resolvefd) */

static int tryreader(int fd,char *lbuf,int llen,cchar **rpp) noex {
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

static int try_guess(TRY *tip) noex {
	int		rs = SR_OK ;
	int		len = 0 ;
	if_constexpr (f_guess) {
	    if (! tip->fl.initnode) {
	        rs = try_initnode(tip) ;
	    }
	    if ((rs >= 0) && tip->fl.node) {
	        int	si = -1 ;
	        int	m_max = 0 ;
	        cchar	*nn = tip->nodename ;
	        rs = 0 ;
	        for (int i = 0 ; ga[i].name ; i += 1) {
	            cchar	*gnp = ga[i].name ;
		    cint	gnl = lenstr(ga[i].name) ;
	            if (int m ; (m = nleadstr(gnp,nn,-1)) >= gnl) {
	                if (m > m_max) {
	                    m_max = m ;
	                    si = i ;
	                }
	            } /* end if */
	        } /* end for */
	        if (si >= 0) {
		    cint	mhl = maxhostlen ;
	            rs = sncpy(tip->domainname,mhl,ga[si].domain) ;
		    len = rs ;
	        }
	    } /* end if (have node) */
	} /* end if_constexpr (f_guess) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (try_guess) */

/* remove trailing whitespace and dots */
static int rmwhitedot(char *bp,int bl) noex {
	auto isrm = [] (int ch) -> bool {
	    return (ch == '.') || char_iswhite(ch) ;
	} ;
	while ((bl > 0) && isrm(bp[bl - 1])) {
	    bp[--bl] = '\0' ;
	}
	return (bl > 0) ? bl : SR_NOTFOUND ;
}
/* end subroutine (rmwhitedot) */


