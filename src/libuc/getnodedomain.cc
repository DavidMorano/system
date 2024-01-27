/* getnodedomain SUPPORT */
/* lang=C++20 */

/* get the local node name and INET domain name */
/* version %I% last-modified %G% */

#define	CF_GUESS	1	/* try to guess domain names? */

/* revision history:

	= 1998-07-01, David A�D� Morano
	This subroutine was originally written.

*/

/* Copyright � 1998 David A�D� Morano.  All rights reserved. */

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
#include	<sys/param.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<climits>		/* for |INT_MAX| */
#include	<cstdlib>
#include	<algorithm>		/* for |min(3c++)| */
#include	<strings.h>		/* from BSD |strncasecmp(3c)| */
#include	<usystem.h>
#include	<usupport.h>
#include	<varnames.hh>
#include	<bufsizevar.hh>
#include	<mallocxx.h>
#include	<estrings.h>
#include	<filebuf.h>
#include	<strn.h>
#include	<sncpyx.h>
#include	<snwcpy.h>
#include	<strdcpyx.h>
#include	<nleadstr.h>
#include	<char.h>
#include	<isnot.h>
#include	<localmisc.h>


/* local defines */

#undef	HOSTBUFLEN
#define	HOSTBUFLEN	(MAXPATHLEN + (8 * MAXHOSTNAMELEN))

#define	RESOLVFNAME	"/etc/resolv.conf"
#define	LOCALHOSTNAME	"localhost"

#ifndef	VARNODE
#define	VARNODE		"NODE"
#endif

#ifndef	VARDOMAIN
#define	VARDOMAIN	"DOMAIN"
#endif

#ifndef	VARLOCALDOMAIN
#define	VARLOCALDOMAIN	"LOCALDOMAIN"
#endif

#define	FILEBUFLEN	1024
#define	TO_OPEN		10		/* time-out for open */
#define	TO_READ		30		/* time-out for read */

#undef	TRY
#define	TRY		struct tryer
#define	TRY_FL		struct tryer_flags


/* local namespaces */

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
    } ; /* end struct (try) */
}

struct guess {
	cchar		*name ;
	cchar		*domain ;
} ;


/* forward references */

extern "C" {
    int		getnodedomain(char *,char *) noex ;
}

static int	try_start(TRY *,char *,char *) noex ;
static int	try_startvarnode(TRY *) noex ;
static int	try_startuname(TRY *) noex ;
static int	try_startnode(TRY *) noex ;
static int	try_vardomain(TRY *) noex ;
static int	try_varlocaldomain(TRY *) noex ;
static int	try_varnode(TRY *) noex ;
static int	try_uname(TRY *) noex ;
static int	try_gethost(TRY *) noex ;
static int	try_resolve(TRY *) noex ;
static int	try_resolvefile(TRY *,cchar *) noex ;
static int	try_guess(TRY *) noex ;
static int	try_finish(TRY *) noex ;

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

static constexpr cchar	*resolvefnames[] = {
	RESOLVFNAME,
	"/var/run/resolv.conf",
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

static bufsizevar	maxhostlen(getbufsize_hn) ;


/* exported variables */


/* exported subroutines */

int getnodedomain(char *nbuf,char *dbuf) noex {
	TRY		ti ;
	int		rs ;
	int		rs1 ;
	if ((rs = try_start(&ti,nbuf,dbuf)) >= 0) {
/* do we need a nodename? */
	    if (nbuf != nullptr) {
	        nbuf[0] = '\0' ;
	        if (! ti.f.initnode) rs = try_startnode(&ti) ;
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
	return rs ;
}
/* end subroutine (getnodedomain) */

int getsysdomain(char *dbuf,int dlen) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		len = 0 ;
	if (dlen < 0) dlen = INT_MAX ;
	if (dbuf) {
	    TRY		ti ;
	    dbuf[0] = '\0' ;
	    if ((rs = try_start(&ti,nullptr,dbuf)) >= 0) {
/* do we need a domainname? */
		rs = SR_OK ;
	        for (int i = 0 ; (rs == SR_OK) && systries[i] ; i += 1) {
	            rs = (*systries[i])(&ti) ;
	        } /* end for */
/* remove any stupid trailing dots from the domain name if any */
	        if ((rs >= 0) && (dbuf[0] != '\0')) {
	            cint	dl = min(dlen,rs) ;
	            rs = rmwhitedot(dbuf,dl) ;
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

static int try_start(TRY *tip,char *nodename,char *domainname) noex {
	int		rs = SR_FAULT ;
	if (tip) {
	    memclear(tip) ;
	    if ((rs = maxhostlen) >= 0) {
	        tip->f = {} ;
	        tip->domainname = domainname ;
	        tip->varnode = nullptr ;
	        tip->sysnodename = nullptr ;
	        tip->nodename = nodename ;
	        if (nodename == nullptr) {
		    char	*cp{} ;
	            if ((rs = malloc_mn(&cp)) >= 0) {
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

static int try_startvarnode(TRY *tip) noex {
	int		rs = SR_FAULT ;
	if (tip) {
	    rs = SR_OK ;
	    if (! tip->f.initvarnode) {
	        tip->f.initvarnode = true ;
	        if (cchar *cp ; (cp = getenv(VARNODE)) != nullptr) {
	            tip->f.varnode = true ;
	            tip->varnode = cp ;
	        }
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (try_startvarnode) */

static int try_startuname(TRY *tip) noex {
	int		rs = SR_OK ;
	if (! tip->f.inituname) {
	    UTSNAME	un ;
	    tip->f.inituname = true ;
	    if ((rs = u_uname(&un)) >= 0) {
	        cchar	*cp ;
	        cchar	*np = un.nodename ;
	        int	nl = strlen(un.nodename) ;
	        if ((rs = uc_mallocstrw(np,nl,&cp)) >= 0) {
	            tip->f.uname = true ;
	            tip->sysnodename = cp ;
	            rs = 0 ;
	        } /* end if (memory-allocation) */
	    } /* end if (uname) */
	} /* end if (needed initialization) */
	return rs ;
}
/* end subroutine (try_startuname) */

static int try_startnode(TRY *tip) noex {
	int		rs = SR_OK ;
	int		sl = -1 ;
	int		cl ;
	cchar	*sp = nullptr ;
	cchar	*cp ;

	if (! tip->f.initnode) {
	    tip->f.initnode = true ;

	    if ((rs >= 0) && (sp == nullptr)) {
	        if (! tip->f.initvarnode) rs = try_startvarnode(tip) ;
	        cp = tip->varnode ;
	        if (tip->f.varnode && (cp) && (cp[0] != '\0')) sp = cp ;
	    }

	    if ((rs >= 0) && (sp == nullptr)) {
	        if (! tip->f.inituname) rs = try_startuname(tip) ;
	        if (rs >= 0) {
	            cp = tip->sysnodename ;
	            if (tip->f.inituname && cp && (cp[0] != '\0')) sp = cp ;
	        }
	    }

	    if (rs >= 0) {
	        if (sp != nullptr) {
	            if ((cl = sfwhitedot(sp,sl,&cp)) > 0) {
	                tip->f.node = true ;
	                strdcpy1w(tip->nodename,NODENAMELEN,cp,cl) ;
	            }
		} else
		    rs = SR_IDRM ;
	    } /* end if */

	} /* end if (needed initialization) */

	return (rs >= 0) ? 0 : rs ;
}
/* end subroutine (try_startnode) */

static int try_vardomain(TRY *tip) noex {
	int		rs = SR_OK ;
	if (cchar *sp ; (sp = getenv(VARDOMAIN)) != nullptr) {
	    int		cl ;
	    cchar	*cp ;
	    if ((cl = sfshrink(sp,-1,&cp)) > 0) {
	        rs = snwcpy(tip->domainname,maxhostlen,cp,cl) ;
	    }
	} /* end if */
	return rs ;
}
/* end subroutine (try_vardomain) */

static int try_varlocaldomain(TRY *tip) noex {
	int		rs = SR_OK ;
	int		cl ;
	cchar		*sp, *cp ;
	if ((sp = getenv(VARLOCALDOMAIN)) != nullptr) {
	    while (CHAR_ISWHITE(*sp)) {
	        sp += 1 ;
	    }
	    cp = sp ;
	    while (*sp && (! CHAR_ISWHITE(*sp)) && (*sp != ':')) {
	        sp += 1 ;
	    }
	    cl = (sp - cp) ;
	    if (cl > 0) {
	        rs = snwcpy(tip->domainname,maxhostlen,cp,cl) ;
	    }
	} /* end if (localdomain) */
	return rs ;
}
/* end subroutine (try_varlocaldomain) */

static int try_varnode(TRY *tip) noex {
	int		rs = SR_OK ;
	if (! tip->f.initvarnode) {
	    rs = try_startvarnode(tip) ;
	}
	if ((rs >= 0) && tip->f.varnode) {
	    cchar	*tp, *cp ;
	    cchar	*sp = tip->varnode ;
	    if ((tp = strchr(sp,'.')) != nullptr) {
	        cp = (tp + 1) ;
	        if (cp[0] != '\0') {
	            rs = sncpy1(tip->domainname,maxhostlen,cp) ;
		}
	    }
	}
	return rs ;
}
/* end subroutine (try_varnode) */

static int try_uname(TRY *tip) noex {
	int		rs = SR_OK ;
	if (! tip->f.inituname) {
	    rs = try_startuname(tip) ;
	}
	if ((rs >= 0) && tip->f.uname) {
	    cchar	*tp, *sp, *cp ;
	    sp = tip->sysnodename ;
	    if ((tp = strchr(sp,'.')) != nullptr) {
	        cp = (tp + 1) ;
	        if (cp[0] != '\0')
	            rs = sncpy1(tip->domainname,maxhostlen,cp) ;
	    }
	} /* end if */
	return rs ;
}
/* end subroutine (try_uname) */

static int try_gethost(TRY *tip) noex {
	int		rs = SR_OK ;
	if (! tip->f.initnode) {
	    rs = try_startnode(tip) ;
	}
	if ((rs >= 0) && tip->f.node) {
	    HOSTENT	he, *hep = &he ;
	    cint	hlen = HOSTBUFLEN ;
	    cchar	*nn = tip->nodename ;
	    cchar	*tp ;
	    char	hbuf[HOSTBUFLEN + 1] ;
	    if ((rs = uc_gethostbyname(&he,hbuf,hlen,nn)) >= 0) {
		cint	dlen = maxhostlen ;

		rs = 0 ;
	        if (hep->h_name &&
	            ((tp = strchr(hep->h_name,'.')) != nullptr)) {

	            rs = sncpy1(tip->domainname,dlen,(tp + 1)) ;

	        } /* end if (official name) */

	        if ((rs == 0) && (hep->h_aliases != nullptr)) {
	            for (int i = 0 ; hep->h_aliases[i] ; i += 1) {
			cchar	*ap = hep->h_aliases[i] ;
	                if ((tp = strchr(ap,'.')) != nullptr) {
	                    rs = sncpy1(tip->domainname,dlen,(tp+1)) ;
	                } /* end if */
			if (rs > 0) break ;
	            } /* end for */
	        } /* end if (alias names) */

	    } else if (isNotPresent(rs))
	        rs = SR_OK ;
	} /* end if (have node) */

	return rs ;
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
	cint		dlen = maxhostlen ;
	cint		to = TO_READ ;
	int		rs ;
	int		rs1 ;
	int		f_found = false ;
	char		*lbuf{} ;
	if ((rs = malloc_ml(&lbuf)) >= 0) {
	    cint	llen = rs ;
	    if ((rs = u_open(fname,O_RDONLY,0666)) >= 0) {
	        filebuf	b ;
	        cint	fd = rs ;
	        if ((rs = filebuf_start(&b,fd,0L,FILEBUFLEN,0)) >= 0) {
		    int		len ;
		    int		sl, cl ;
		    cchar	*tp, *sp, *cp ;
	            while ((rs = filebuf_readln(&b,lbuf,llen,to)) > 0) {
	                len = rs ;
	                if (lbuf[len - 1] == '\n') len -= 1 ;
	                lbuf[len] = '\0' ;

	                sp = lbuf ;
	                sl = len ;
	                if ((sl == 0) || (sp[0] == '#')) continue ;

	                cl = nextfield(sp,sl,&cp) ;

	                if (cl < 6) continue ;

	                if ((strncasecmp(cp,"domain",6) != 0) ||
	                    (! CHAR_ISWHITE(cp[6])))
	                        continue ;

	                sp += 6 ;
	                sl -= 6 ;
	                cl = nextfield(sp,sl,&cp) ;

	                if ((cp[0] == '#') ||
	                    (strncmp(cp,"..",2) == 0)) continue ;

	                if ((tp = strnchr(cp,cl,'#')) != nullptr) {
	                    cl = (tp - cp) ;
			}
	                if (cl > 0) {
	                    f_found = true ;
	                    break ;
	                }
	            } /* end while (reading lines) */
		    if ((rs >= 0) && f_found) {
	    	        rs = snwcpy(tip->domainname,dlen,cp,cl) ;
		    }
	            rs1 = filebuf_finish(&b) ;
		    if (rs >= 0) rs = rs1 ;
	        } /* end if (filebuf) */
	        rs1 = u_close(fd) ;
		if (rs >= 0) rs = rs1 ;
	    } else if (isNotPresent(rs)) {
	        rs = SR_OK ;
	    }
	    rs1 = uc_free(lbuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return rs ;
}
/* end subroutine (try_resolvefile) */

static int try_guess(TRY *tip) noex {
	int		rs = SR_OK ;

#if	CF_GUESS
	if (! tip->f.initnode) {
	    rs = try_startnode(tip) ;
	}
	if ((rs >= 0) && tip->f.node) {
	    int		si = -1 ;
	    int		m ;
	    int		m_max = 0 ;
	    int		gnl ;
	    cchar	*nn = tip->nodename ;
	    cchar	*gnp ;
	    rs = 0 ;
	    for (int i = 0 ; ga[i].name ; i += 1) {
	        gnp = ga[i].name ;
		gnl = strlen(gnp) ;
	        if ((m = nleadstr(gnp,nn,-1)) >= gnl) {
	            if (m > m_max) {
	                m_max = m ;
	                si = i ;
	            }
	        } /* end if */
	    } /* end for */
	    if (si >= 0) {
	        rs = sncpy1(tip->domainname,maxhostlen,ga[si].domain) ;
	    }
	} /* end if (have node) */
#endif /* CF_GUESS */

	return rs ;
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


