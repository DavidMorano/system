/* connection SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* try to provide some INET connection information */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-07-01, David A­D­ Morano
	This code (an object) is updated from some previous code
	that performed the approximate same functions.  This old
	code (of which I did not change the API) is a kind of a
	mess in terms of its architected interface.  Maybe 
	someday a better API will come along to perform similar
	functions.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	connection

	Description:
	This subroutine will take an INET socket and a local domain
	name and find the hostname of the remote end of the socket.

	Synopses:
	int connection_start(CON *op,cchar *inetdomain) noex
	int connection_finish(CON *op) noex
	int connection_socklocname(CON *op,char *dp,int dl,int s) noex
	int connection_sockremname(CON *op,char *dp,int dl,int s) noex
	int connection_peername(CON *op,SA *sap,int sal,char *dp,int dl) noex
	int connection_mknames(CON *op,vecstr *nlp) noex

	Returns:
	>=0		OK
	<0		error (system-return)

	Notes:
	Like much of all of my code, this module (object) dates
	from an earlier period (some years ago now, now beyond my
	memory).  How can I tell?  I can kind of tell how old some
	code is, or how old its predecessor was, from the style of
	the code.  The style of this code dates from the late 1980s
	or early 1990s.  I put it into its present form (1998) from
	the prior code, but left many old stylistic features of the
	prior code (because I am kind of lazy).

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<sys/socket.h>
#include	<netinet/in.h>
#include	<arpa/inet.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<climits>		/* |INT_MAX| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>		/* |lenstr(3c)| + |memcpy(3c)| */
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<netdb.h>
#include	<usystem.h>
#include	<uinet.h>
#include	<getbufsize.h>
#include	<mallocxx.h>
#include	<mallocstuff.h>
#include	<getxx.h>
#include	<hostent.h>
#include	<hostinfo.h>
#include	<inetaddr.h>
#include	<vecstr.h>
#include	<sockaddress.h>
#include	<rmx.h>
#include	<sncpyx.h>
#include	<strwcpy.h>
#include	<isindomain.h>
#include	<isnot.h>
#include	<localmisc.h>

#include	"connection.h"

import libutil ;
import uconstants ;

/* local defines */

#define	CON		connection
#define	SA		sockaddress


/* imported namespaces */

using std::nullptr_t ;			/* type */
using std::min ;			/* subroutine-template */
using std::max ;			/* subroutine-template */
using std::nothrow ;			/* constant */


/* local typedefs */

typedef sockaddress *	sockaddressp ;
typedef SOCKADDR *	sockaddrp ;


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct vars {
	int		maxpathlen ;
	int		maxhostlen ;
	int		socknamelen ;
	int		hostentlen ;
	operator int () noex ;
    } ; /* end struct (vars) */
} /* end namespace */


/* forward references */

static int connection_addname(CON *,vecstr *,cc *) noex ;
static int connection_ip4lookup(CON *,char *,int) noex ;
static int connection_ip6lookup(CON *,char *,int) noex ;

static int isNotFound(int rs) noex {
	return isNotPresent(rs) ;
}


/* local variables */

static vars		var ;

constexpr int		in4addrlen = INET4ADDRLEN ;
constexpr int		in6addrlen = INET6ADDRLEN ;

constexpr cchar		*localhost = sysword.w_localhost ;


/* exported variables */


/* exported subroutines */

int connection_start(CON *op,cchar *inetdomain) noex {
    	CONNECTION	*hop = op ;
	int		rs = SR_FAULT ;
	if (op && inetdomain) {
	    static cint		rsv = var ;
	    cint		ssz = szof(sockaddress) ;
	    memclear(hop) ;
	    if ((rs = rsv) >= 0) {
	        op->inetdomain = inetdomain ;
	        op->fl.inet = false ;
	        op->sock = -1 ;
	        if (void *vp ; (rs = lm_mall(ssz,&vp)) >= 0) {
		    op->sap = sockaddressp(vp) ;
	        }
	    } /* end if (vars) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (connection_start) */

int connection_finish(CON *op) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (op) {
	    rs = SR_OK ;
	    if (op->fl.inetdomain && op->inetdomain) {
	        op->fl.inetdomain = false ;
	        rs1 = uc_free(op->inetdomain) ;
	        if (rs >= 0) rs = rs1 ;
	        op->inetdomain = nullptr ;
	    }
	    if (op->name) {
	        rs1 = uc_free(op->name) ;
	        if (rs >= 0) rs = rs1 ;
	        op->name = nullptr ;
	    }
	    if (op->sap) {
		rs1 = lm_free(op->sap) ;
	        if (rs >= 0) rs = rs1 ;
		op->sap = nullptr ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (connection_finish) */

int connection_socklocname(CON *op,char *dp,int dl,int s) noex {
	int		rs = SR_FAULT ;
	int		len = 0 ;
	if (op && dp) {
	    rs = SR_BADF ;
	    if (s >= 0) {
		if (USTAT sb ; (rs = uc_fstat(s,&sb)) >= 0) {
		    if (S_ISSOCK(sb.st_mode)) {
	                sockaddress	*sap = op->sap ;
	                int		sal = szof(sockaddress) ;
	                op->sock = s ;
	                if ((rs = u_sockaddrloc(s,sap,&sal)) >= 0) {
	                    op->fl.sa = true ;
	                    rs = connection_peername(op,sap,sal,dp,dl) ;
	                    len = rs ;
	                }
	    	    } else if (S_ISFIFO(sb.st_mode)) {
	                rs = sncpy(dp,dl,localhost) ;
	                len = rs ;
		    } /* end if */
		} /* end if (uc_fstat) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (connection_socklocname) */

int connection_sockremname(CON *op,char *dp,int dl,int s) noex {
	int		rs = SR_FAULT ;
	int		len = 0 ;
	if (op && dp) {
	    rs = SR_BADF ;
	    if (s >= 0) {
		if (USTAT sb ; (rs = uc_fstat(s,&sb)) >= 0) {
		    if (S_ISSOCK(sb.st_mode)) {
	                sockaddress	*sap = op->sap ;
	                int		sal = szof(sockaddress) ;
	                op->sock = s ;
	                if ((rs = u_sockaddrrem(s,sap,&sal)) >= 0) {
	                    op->fl.sa = true ;
	                    rs = connection_peername(op,sap,sal,dp,dl) ;
	                    len = rs ;
	                }
	    	    } else if (S_ISFIFO(sb.st_mode)) {
	                rs = sncpy(dp,dl,localhost) ;
	                len = rs ;
		    } /* end if */
		} /* end if (uc_fstat) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (connection_sockremname) */

int connection_peername(CON *op,SA *sap,int sal,char *dp,int dl) noex {
	int		rs = SR_FAULT ;
	int		len = 0 ;
	if (op && sap && dp) {
	    dp[0] = '\0' ;
	    if ((rs = sockaddress_getaf(sap)) >= 0) {
	        cint	af = rs ;
	        op->fl.addr = false ;
	        op->fl.sa = true ;
	        memcpy(op->sap,sap) ;
	        op->sal = sal ;
	        switch (af) {
	        case AF_UNIX:
	            rs = sockaddress_getaddr(sap,dp,dl) ;
	            len = rs ;
	            break ;
	        case AF_INET4:
	            op->fl.inet = true ;
	            op->fl.addr = true ;
	            rs = connection_ip4lookup(op,dp,dl) ;
	            len = rs ;
	            break ;
	        case AF_INET6:
	            op->fl.inet = true ;
	            rs = connection_ip6lookup(op,dp,dl) ;
	            len = rs ;
	            break ;
	        } /* end switch (got an INET host entry) */
	    } /* end if (sockaddress_getaf) */
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (connection_peername) */

namespace {
    struct sub_mknames {
	CON	*op ;
	vecstr	*nlp ;
	sub_mknames(CON *o,vecstr *n) noex : op(o), nlp(n) { } ;
	operator int () noex ;
	int addstuff(int) noex ;
	int addnames(hostinfo *) noex ;
	int addresses(hostinfo *) noex ;
    } ; /* end struct (sub_mknames) */
}

int connection_mknames(CON *op,vecstr *nlp) noex {
	int		rs = SR_FAULT ;
	int		n = 0 ;
	if (op && nlp) {
	    sub_mknames		mko(op,nlp) ;
	    rs = mko ;
	    n = rs ;
	} /* end if (non-null) */
	return (rs >= 0) ? n : rs ;
}
/* end subroutine (connection_mknames) */

sub_mknames::operator int () noex {
	int		rs = SR_DESTADDRREQ ;
	int		n = 0 ;
	if (op->fl.sa) {
	    if ((rs = sockaddress_getaf(op->sap)) >= 0) {
	        cint	af = rs ;
	        switch (af) {
	        case AF_UNIX:
	            rs = connection_addname(op,nlp,localhost) ;
		    n += rs ;
		    break ;
		case AF_INET4:
		case AF_INET6:
		    rs = addstuff(af) ;
		    n += rs ;
		    break ;
		} /* end switch */
	    } /* end if (sockaddress_getaf) */
	} /* end if (have address) */
	return (rs >= 0) ? n : rs ;
}
/* end method (sub_mknames::operator) */

int sub_mknames::addstuff(int af) noex {
	int		rs ;
	int		rs1 ;
	int		n = 0 ;
	if (hostinfo hi ; (rs = hostinfo_start(&hi,af,op->name)) >= 0) {
	    if ((rs = addnames(&hi)) >= 0) {
		n += rs ;
		rs = addresses(&hi) ;
		n += rs ;
	    } /* end if (addnames) */
	    rs1 = hostinfo_finish(&hi) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (hostinfo) */
	return (rs >= 0) ? n : rs ;
}
/* end method (sub_mknames::addstuff) */

int sub_mknames::addnames(hostinfo *hip) noex {
	int		rs ;
	int		rs1 ;
	int		n = 0 ;
	cchar		*hp ;
	if ((rs = hostinfo_geteffective(hip,&hp)) >= 0) {
	    if ((rs = connection_addname(op,nlp,hp)) >= 0) {
	        n += rs ;
	        if ((rs = hostinfo_getcanonical(hip,&hp)) >= 0) {
		    hostinfo_cur	hc ;
	            rs = connection_addname(op,nlp,hp) ;
	            n += rs ;
	            if ((rs = hostinfo_curbegin(hip,&hc)) >= 0) {
	                while ((rs = hostinfo_curenumname(hip,&hc,&hp)) > 0) {
	                    rs = connection_addname(op,nlp,hp) ;
	              	    n += rs ;
	              	    if (rs < 0) break ;
	            	} /* end while (names) */
	                rs1 = hostinfo_curend(hip,&hc) ;
			if (rs >= 0) rs = rs1 ;
	            } /* end if (hostinfo-cur) */
	        } /* end if (names) */
	    } /* end if (connection_addname) */
	} /* end if (hostinfo_geteffective) */
	return (rs >= 0) ? n : rs ;
}
/* end method (sub_mknames::addnames) */

int sub_mknames::addresses(hostinfo *hip) noex {
	int		rs ;
	int		rs1 ;
	int		n = 0 ;
	if (char *nbuf{} ; (rs = malloc_hn(&nbuf)) >= 0) {
	    cint		nlen = rs ;
	    if (hostinfo_cur hc ; (rs = hostinfo_curbegin(hip,&hc)) >= 0) {
	        const uchar	*ap{} ;
	        while ((rs = hostinfo_curenumaddr(hip,&hc,&ap)) > 0) {
		    cint	al = rs ;
	            if (al != in4addrlen) continue ;
		    if (inetaddr ia ; (rs = ia.start(ap)) >= 0) {
	                if ((rs = ia.getdotaddr(nbuf,nlen)) >= 0) {
	                    rs = nlp->adduniq(nbuf) ;
	                    if (rs < INT_MAX) n += 1 ;
		        }
	                rs1 = ia.finish ;
	   	        if (rs >= 0) rs = rs1 ;
	            } /* end if (inetaddr) */
	            if (rs < 0) break ;
	        } /* end while (addresses) */
	        rs1 = hostinfo_curend(hip,&hc) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (hostinfo-cur) */
	    rs = rsfree(rs,nbuf) ;
	} /* end if (m-a-f) */
	return (rs >= 0) ? n : rs ;
}
/* end method (sub_mknames::addresses) */

#ifdef	COMMENT
/* store the INET "dot" address also! */
int sub_mknames::adddots(int af) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	int		n = 0 ;
	if ((af == AF_INET4) && op->fl.inet && op->fl.addr) {
	    if (char *nbuf{} ; (rs = malloc_hn(&nbuf)) >= 0) {
	        cint		nlen = rs ;
	        if (inetaddr ia ; (rs = ia.start(&op->netipaddr)) >= 0) {
	            if ((rs = ia.getdotaddr(nbuf,nlen)) >= 0) {
	                rs = nlp->adduniq(nbuf,rs) ;
	                if (rs < INT_MAX) n += 1 ;
		    }
	            rs1 = ia.finish ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (inetaddr) */
	        rs = rsfree(rs,nbuf) ;
	    } /* end if (m-a-f) */
	} /* end if (go) */
	return (rs >= 0) ? n : rs ;
}
/* end subroutine (connection_adddots) */
#endif /* COMMENT */


/* private subroutines */

static int connection_addname(CON *op,vecstr *nlp,cc *name) noex {
	int		sl = lenstr(name) ;
	int		rs ;
	int		n = 0 ;
	if ((rs = nlp->adduniq(name,sl)) >= 0) {
	    cchar	*dn = op->inetdomain ;
	    n += (rs < INT_MAX) ;
	    if (dn && isindomain(name,dn)) {
		sl = rmchr(name,sl,'.') ; /* <- first dot for 'nodename' */
		rs = nlp->adduniq(name,sl) ;
	    	n += (rs < INT_MAX) ;
	    } /* end if (it is in our domain) */
	} /* end if (vecstr_adduniq) */
	return (rs >= 0) ? n : rs ;
}
/* end subroutine (connection_addname) */

/* lookup this IP (INET4) address */
static int connection_ip4lookup(CON *op,char *dp,int dl) noex {
	sockaddress	*sap = op->sap ;
	cint		alen = in4addrlen ;
	int		rs ;
	int		rs1 ;
	int		len = 0 ;
	if ((rs = sockaddress_getaddr(sap,&op->netipaddr,alen)) >= 0) {
	    if (char *hebuf{} ; (rs = malloc_ho(&hebuf)) >= 0) {
	        ucentho		he ;
	        cint		helen = rs ;
	        cint		af = AF_INET4 ;
	        cint		al = in4addrlen ;
	        cchar		*ap  = charp(&op->netipaddr) ;
	        if ((rs = getho_addr(&he,hebuf,helen,af,ap,al)) >= 0) {
	            hostent_cur		hc ;
	            cchar		*sp = nullptr ;
	            if (op->inetdomain) {
	                if ((rs = hostent_curbegin(&he,&hc)) >= 0) {
			    auto	enumname = hostent_curenumname ;
	                    while ((rs = enumname(&he,&hc,&sp)) > 0) {
	                        if (isindomain(sp,op->inetdomain)) break ;
	                    } /* end while */
	                    rs1 = hostent_curend(&he,&hc) ;
			    if (rs >= 0) rs = rs1 ;
	                } /* end if (hostent-cur) */
		    } /* end if (non-null) */
		    if (rs >= 0) {
	               if (sp) {
			   cint		sl = rmdot(sp) ; /* last 'dot' */
	                   rs = sncpyw(dp,dl,sp,sl) ;
			   len = rs ;
	               } else {
		           /* it is NOT in our domain */
	                   if ((rs = hostent_getcanonical(&he,&sp)) >= 0) {
	                        rs = sncpy(dp,dl,sp) ;
				len = rs ;
		           }
	               } /* end if */
		    } /* end if (ok) */
	        } else if (isNotFound(rs)) {
	            if (inetaddr ia ; (rs = ia.start(&op->netipaddr)) >= 0) {
		        {
	                    rs = ia.getdotaddr(dp,dl) ;
			    len = rs ;
		        }
	                rs1 = ia.finish ;
		        if (rs >= 0) rs = rs1 ;
	            } /* end if (inetadd) */
	        } /* end if */
	        rs = rsfree(rs,hebuf) ;
	    } /* end if (m-a-f) */
	} /* end if (sockaddress_getaddr) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (connection_ip4lookup) */

static int connection_ip6lookup(CON *op,char *dp,int dl) noex {
	int		rs ;
	int		len = 0 ;
	if (char *hnbuf{} ; (rs = malloc_hn(&hnbuf)) >= 0) {
	    CSOCKADDR	*ssap = sockaddrp(op->sap) ;
	    cint	sal = op->sal ;
	    cint	slen = NI_MAXSERV ;
	    cint	hnlen = rs ;
	    cint	fl = NI_NOFQDN ;
	    ushort	in6addr[in6addrlen/2] ;
	    char	sbuf[NI_MAXSERV + 1] ;
	    auto gni = uc_getnameinfo ;
	    if ((rs = gni(ssap,sal,hnbuf,hnlen,sbuf,slen,fl)) >= 0) {
	        rs = sncpy(dp,dl,hnbuf) ;
	        len = rs ;
	    } else if (isNotPresent(rs)) {
		sockaddress	*sap = op->sap ;
		cint		al = in6addrlen ;
	        if ((rs = sockaddress_getaddr(sap,in6addr,al)) >= 0) {
	            if (isin4mapped(in6addr)) {
	                op->fl.addr = true ;
	                memcpy(&op->netipaddr,(in6addr + 6)) ;
	                rs = connection_ip4lookup(op,dp,dl) ;
	                len = rs ;
	            } else if (isin6loopback(in6addr)) {
	                rs = sncpy(dp,dl,localhost) ;
	                len = rs ;
	            } /* end if */
		} /* end if (sockaddress_getaddr) */
	    } /* end if (getnameinfo) */
	    rs = rsfree(rs,hnbuf) ;
	} /* end if (m-a-f) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (connection_ip6lookup) */

int connection::start(cchar *dname) noex {
    	return connection_start(this,dname) ;
}

int connection::socklocname(char *dp,int dl,int s) noex {
	return connection_socklocname(this,dp,dl,s) ;
}

int connection::sockremname(char *dp,int dl,int s) noex {
	return connection_sockremname(this,dp,dl,s) ;
}

int connection::peername(sockaddress *asap,int asal,char *dp,int dl) noex {
	return connection_peername(this,asap,asal,dp,dl) ;
}

int connection::mknames(vecstr *nlp) noex {
	return connection_mknames(this,nlp) ;
}

void connection::dtor() noex {
	if (cint rs = finish ; rs < 0) {
	    ulogerror("connection",rs,"fini-finish") ;
	}
}

connection_co::operator int () noex {
	int		rs = SR_BUGCHECK ;
	if (op) {
	    switch (w) {
	    case connectionmem_finish:
	        rs = connection_finish(op) ;
	        break ;
	    } /* end switch */
	} /* end if (non-null) */
	return rs ;
}
/* end method (connection_co::operator) */

vars::operator int () noex {
	int		rs ;
	if ((rs = getbufsize(getbufsize_mp)) >= 0) {
	    maxpathlen = rs ;
	    if ((rs = getbufsize(getbufsize_hn)) >= 0) {
	        maxhostlen = rs ;
	        if ((rs = getbufsize(getbufsize_ho)) >= 0) {
		    hostentlen = rs ;
	            socknamelen = max(maxpathlen,maxhostlen) ;
		}
	    }
	}
	return rs ;
} /* end method (vars::operator) */


