/* getnodedomain SUPPORT */
/* lang=C++20 */

/* get the local node-name and INET domain name */
/* version %I% last-modified %G% */

#define	CF_GUESS	0	/* try to guess domain names? */

/* revision history:

	= 1998-07-01, David A­D­ Morano
	This subroutine was originally written.

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
#include	<cstdlib>		/* for |getenv(3c)| */
#include	<algorithm>		/* for |min(3c++)| */
#include	<strings.h>		/* from BSD |strncasecmp(3c)| */
#include	<usystem.h>
#include	<varnames.hh>
#include	<bufsizevar.hh>
#include	<mallocxx.h>
#include	<estrings.h>
#include	<uinfo.h>
#include	<filebuf.h>
#include	<strn.h>
#include	<sfx.h>
#include	<sncpyx.h>
#include	<snwcpy.h>
#include	<strdcpyx.h>
#include	<nleadstr.h>
#include	<char.h>
#include	<isnot.h>
#include	<localmisc.h>

#include	"getnodename.h"


/* local defines */

#define	RESOLVFNAME	"/etc/resolv.conf"
#define	LOCALHOSTNAME	"localhost"

#define	FILEBUFLEN	1024		/* initial size for FILEBUF */
#define	TO_READ		30		/* time-out for read */

#undef	TRY
#define	TRY		struct tryer
#define	TRY_FL		struct tryer_flags

#ifndef	CF_GUESS
#define	CF_GUESS	0
#endif


/* imported namespaces */

using std::nullptr_t ;			/* type */
using std::min ;			/* subroutine-template */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

struct tryer_flags {
	uint		initvarnode:1 ;
	uint		inituname:1 ;
	uint		initnode:1 ;
	uint		varnode:1 ;
	uint		uname:1 ;
	uint		node:1 ;
} ; /* end struct (tryer_flags) */

namespace {
    struct tryer {
	char		*nodename ;	/* passed caller argument (returned) */
	char		*domainname ;	/* passed caller argument (returned) */
	char		*nbuf ;
	cchar		*varnode ;
	cchar		*sysnodename ;
	TRY_FL		f ;
	int		dlen ;
    } ; /* end struct (try) */
}

struct guess {
	cchar		*name ;
	cchar		*domain ;
} ;


/* forward references */

extern "C" {
    int		getnodedomain(char *,char *) noex ;
    int		getsysdomain(char *,int) noex ;
    int		getuserdomain(char *,int) noex ;
}

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

static constexpr int	(*tries[])(TRY *) = {
	try_vardomain,
	try_varlocaldomain,
	try_varnode,
	try_uname,
	try_gethost,
	try_resolve,
	try_guess,
	nullptr
} ;

static constexpr int	(*systries[])(TRY *) = {
	try_uname,
	try_gethost,
	try_resolve,
	try_guess,
	nullptr
} ;

static constexpr cpcchar	resolvefnames[] = {
	RESOLVFNAME,			/* most operating systems */
	"/var/run/resolv.conf",		/* for example: MacOS */
	nullptr
} ;

static constexpr struct guess	ga[] = {
	{ "rc", "rightcore.com" },
	{ "jig", "rightcore.com" },
	{ "gateway", "ece.neu.com" },
	{ "vinson", "ece.neu.com" },
	{ "frodo", "ece.neu.com" },
	{ "olive", "ece.neu.com" },
	{ "gilmore", "ece.neu.com" },
	{ "dr", "dr.lucent.com" },
	{ "ho", "ho.lucent.com" },
	{ "mh", "mh.lucent.com" },
	{ "mt", "mt.lucent.com" },
	{ "cb", "cb.lucent.com" },
	{ nullptr, nullptr }
} ;

static bufsizevar	maxnodelen(getbufsize_nn) ;
static bufsizevar	maxhostlen(getbufsize_hn) ;

constexpr bool		f_guess = CF_GUESS ;


/* exported variables */


/* exported subroutines */

int getnodedomain(char *nbuf,char *dbuf) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = maxhostlen) >= 0) {
	    TRY		ti ;
	    if ((rs = try_start(&ti,nbuf,dbuf,rs)) >= 0) {
/* do we need a nodename? */
	        if (nbuf != nullptr) {
	            nbuf[0] = '\0' ;
	            if (! ti.f.initnode) rs = try_initnode(&ti) ;
	        } /* end if (trying to get a nodename) */
/* do we need a domainname? */
	        if ((rs >= 0) && (dbuf != nullptr)) {
	            dbuf[0] = '\0' ;
		    rs = SR_OK ;
	            for (int i = 0 ; (rs == SR_OK) && tries[i] ; i += 1) {
	                rs = (*tries[i])(&ti) ;
	            } /* end for */
/* remove any stupid trailing dots from the domain name if any */
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
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		len = 0 ;
	if (dbuf) {
	    TRY		ti ;
	    dbuf[0] = '\0' ;
	    if ((rs = try_start(&ti,nullptr,dbuf,dlen)) >= 0) {
/* do we need a domainname? */
		rs = SR_OK ;
	        for (int i = 0 ; (rs == SR_OK) && systries[i] ; i += 1) {
	            rs = (*systries[i])(&ti) ;
	        } /* end for */
/* remove any stupid trailing dots from the domain name if any */
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
	if (dbuf) {
	    char	*dn{} ;
	    if ((rs = malloc_hn(&dn)) >= 0) {
	        if (dlen < 0) dlen = rs ;
	        if ((rs = getnodedomain(nullptr,dn)) >= 0) {
	            rs = sncpy1(dbuf,dlen,dn) ;
		    len = rs ;
	        }
	        rs1 = uc_free(dn) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (m-a-f) */
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (getuserdomain) */


/* local subroutines */

static int try_start(TRY *tip,char *nb,char *db,int dl) noex {
	int		rs = SR_FAULT ;
	if (tip) {
	    memclear(tip) ;
	    if ((rs = maxhostlen) >= 0) {
		cint	hl = rs ;
	        tip->f = {} ;
	        tip->domainname = db ;
	        tip->varnode = nullptr ;
	        tip->sysnodename = nullptr ;
	        tip->nodename = nb ;
		tip->dlen = (dl < 0) ? hl : dl ;
	        if (nb == nullptr) {
	            if (char *cp{} ; (rs = malloc_mn(&cp)) >= 0) {
		       tip->nbuf = cp ;
	    	       tip->nodename = cp ;
		    }
	        } /* end if (memory-allocation) */
	        if (rs >= 0) tip->nodename[0] = '\0' ;
	    } /* end if (maxhostlen) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (try_start) */

static int try_finish(TRY *tip) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (tip->sysnodename) {
	    rs1 = uc_free(tip->sysnodename) ;
	    if (rs >= 0) rs = rs1 ;
	    tip->sysnodename = nullptr ;
	}
	if (tip->nbuf) {
	    rs1 = uc_free(tip->nbuf) ;
	    if (rs >= 0) rs = rs1 ;
	    tip->nbuf = nullptr ;
	}
	tip->varnode = nullptr ;
	return rs ;
}
/* end subroutine (try_finish) */

static int try_initvarnode(TRY *tip) noex {
	int		rs = SR_FAULT ;
	if (tip) {
	    rs = SR_OK ;
	    if (! tip->f.initvarnode) {
	        static cchar	*val = getenv(varname.node) ;
	        tip->f.initvarnode = true ;
		if (val != nullptr) {
	            tip->f.varnode = true ;
	            tip->varnode = val ;
	        }
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (try_initvarnode) */

static int try_inituname(TRY *tip) noex {
	int		rs = SR_OK ;
	if (! tip->f.inituname) {
	    uinfo_names		uin ;
	    tip->f.inituname = true ;
	    if ((rs = uinfo_name(&uin)) >= 0) {
	        cchar	*sp = uin.nodename ;
	        int	sl = strlen(uin.nodename) ;
	        if (cchar *cp{} ; (rs = uc_mallocstrw(sp,sl,&cp)) >= 0) {
	            tip->f.uname = true ;
	            tip->sysnodename = cp ;
	            rs = 0 ;
	        } /* end if (memory-allocation) */
	    } /* end if (uname) */
	} /* end if (needed initialization) */
	return rs ;
}
/* end subroutine (try_inituname) */

static int try_initnode(TRY *tip) noex {
	int		rs = SR_OK ;
	if (! tip->f.initnode) {
	    tip->f.initnode = true ;
	    if ((rs = maxnodelen) >= 0) {
	        int	sl = -1 ;
	        cchar	*sp = nullptr ;
	        cchar	*cp ;
	        if ((rs >= 0) && (sp == nullptr)) {
	            if (! tip->f.initvarnode) {
		        rs = try_initvarnode(tip) ;
		    }
	            cp = tip->varnode ;
	            if (tip->f.varnode && (cp) && (cp[0] != '\0')) {
		        sp = cp ;
		    }
	        } /* end if (ok) */
	        if ((rs >= 0) && (sp == nullptr)) {
	            if (! tip->f.inituname) {
			rs = try_inituname(tip) ;
		    }
	            if (rs >= 0) {
	                cp = tip->sysnodename ;
	                if (tip->f.inituname && cp && (cp[0] != '\0')) {
			    sp = cp ;
		        }
	            }
	        } /* end if (ok) */
	        if (rs >= 0) {
	            if (sp != nullptr) {
	                if (int cl ; (cl = sfwhitedot(sp,sl,&cp)) > 0) {
	                    tip->f.node = true ;
	                    strdcpy1w(tip->nodename,maxnodelen,cp,cl) ;
	                }
		    } else {
		        rs = SR_IDRM ;
		    }
	        } /* end if (ok) */
	    } /* end if (maxnodelen) */
	} /* end if (needed initialization) */
	return (rs >= 0) ? 0 : rs ;
}
/* end subroutine (try_initnode) */

static int try_vardomain(TRY *tip) noex {
	static cchar	*val = getenv(varname.domain) ;
	int		rs = SR_OK ;
	int		len = 0 ;
	if (val != nullptr) {
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
	if (val != nullptr) {
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
	    cl = (sp - cp) ;
	    if (cl > 0) {
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
	if (! tip->f.initvarnode) {
	    rs = try_initvarnode(tip) ;
	}
	if ((rs >= 0) && tip->f.varnode) {
	    cchar	*sp = tip->varnode ;
	    if (cchar *tp ; (tp = strchr(sp,'.')) != nullptr) {
	        cchar	*cp = (tp + 1) ;
	        if (cp[0] != '\0') {
		    cint	dlen = tip->dlen ;
	            rs = sncpy1(tip->domainname,dlen,cp) ;
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
	if (! tip->f.inituname) {
	    rs = try_inituname(tip) ;
	}
	if ((rs >= 0) && tip->f.uname) {
	    cchar	*sp = tip->sysnodename ;
	    if (cchar *tp ; (tp = strchr(sp,'.')) != nullptr) {
	        cchar	*cp = (tp + 1) ;
	        if (cp[0] != '\0') {
		    cint	dlen = tip->dlen ;
	            rs = sncpy1(tip->domainname,dlen,cp) ;
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
	if (! tip->f.initnode) {
	    rs = try_initnode(tip) ;
	}
	if ((rs >= 0) && tip->f.node) {
	    char	*hbuf{} ;
	    if ((rs = malloc_ho(&hbuf)) >= 0) {
		const nullptr_t	np{} ;
	        ucentho		he, *hep = &he ;
	        cint		hlen = rs ;
	        cchar		*nn = tip->nodename ;
	        if ((rs = uc_gethonam(&he,hbuf,hlen,nn)) >= 0) {
		    cint	dlen = tip->dlen ;
	            cchar	*tp{} ;
		    bool	f = true ;
		    rs = 0 ;
	            f = f && (hep->h_name != np) ;
		    f = f && ((tp = strchr(hep->h_name,'.')) != np) ;
		    if (f) {
	                rs = sncpy1(tip->domainname,dlen,(tp + 1)) ;
			len = rs ;
	            } /* end if (official name) */
	            if ((rs == 0) && (hep->h_aliases != nullptr)) {
	                for (int i = 0 ; hep->h_aliases[i] ; i += 1) {
			    cchar	*ap = hep->h_aliases[i] ;
	                    if ((tp = strchr(ap,'.')) != nullptr) {
	                        rs = sncpy1(tip->domainname,dlen,(tp+1)) ;
				len = rs ;
	                    } /* end if */
			    if (rs > 0) break ;
	                } /* end for */
	            } /* end if (alias names) */
		    rs1 = uc_free(hbuf) ;
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
	for (int i = 0 ; (rs == SR_OK) && resolvefnames[i] ; i += 1) {
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
	char		*lbuf{} ;
	if ((rs = malloc_ml(&lbuf)) >= 0) {
	    cint	llen = rs ;
	    if ((rs = uc_open(fname,O_RDONLY,0666)) >= 0) {
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
	    rs1 = uc_free(lbuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return (rs >= 0) ? len : rs;
	return rs ;
}
/* end subroutine (try_resolvefile) */

static int try_resolvefd(TRY *tip,char *lbuf,int llen,int fd) noex {
	filebuf		b ;
	int		rs ;
	int		rs1 ;
	int		len = 0 ;
	cchar		*dp = nullptr ;
        if ((rs = filebuf_start(&b,fd,0L,FILEBUFLEN,0)) >= 0) {
	    cint	to = TO_READ ;
	    cchar	*key = "domain" ;
            while ((rs = filebuf_readln(&b,lbuf,llen,to)) > 0) {
                if ((len = sfkeyval(lbuf,rs,key,&dp)) > 0) break ;
            } /* end while (reading lines) */
            rs1 = filebuf_finish(&b) ;
            if (rs >= 0) rs = rs1 ;
        } /* end if (filebuf) */
        if ((rs >= 0) && (len > 0)) {
	    cint	dlen = tip->dlen ;
            rs = snwcpy(tip->domainname,dlen,dp,len) ;
        }
	if (len < 0) len = 0 ;
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (try_resolvefd) */

static int try_guess(TRY *tip) noex {
	int		rs = SR_OK ;
	int		len = 0 ;
	if constexpr (f_guess) {
	    if (! tip->f.initnode) {
	        rs = try_initnode(tip) ;
	    }
	    if ((rs >= 0) && tip->f.node) {
	        int	si = -1 ;
	        int	m_max = 0 ;
	        cchar	*nn = tip->nodename ;
	        rs = 0 ;
	        for (int i = 0 ; ga[i].name ; i += 1) {
	            cchar	*gnp = ga[i].name ;
		    cint	gnl = strlen(ga[i].name) ;
	            if (int m ; (m = nleadstr(gnp,nn,-1)) >= gnl) {
	                if (m > m_max) {
	                    m_max = m ;
	                    si = i ;
	                }
	            } /* end if */
	        } /* end for */
	        if (si >= 0) {
	            rs = sncpy1(tip->domainname,maxhostlen,ga[si].domain) ;
		    len = rs ;
	        }
	    } /* end if (have node) */
	} /* end if-constexpr (f_guess) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (try_guess) */

/* remove trailing whitespace and dots */
static int rmwhitedot(char *bp,int bl) noex {
	while ((bl > 0) && ((bp[bl-1] == '.') || CHAR_ISWHITE(bp[bl-1]))) {
	    bp[--bl] = '\0' ;
	}
	return (bl > 0) ? bl : SR_NOTFOUND ;
}
/* end subroutine (rmwhitedot) */


