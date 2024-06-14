/* connection SUPPORT */
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
	int connection_start(CON *cnp,cchar *inetdomain) noex
	int connection_finish(CON *cnp) noex
	int connection_socklocname(CON *cnp,char *dp,int dl,int s) noex
	int connection_sockremname(CON *cnp,char *dp,int dl,int s) noex
	int connection_peername(CON *cnp,SA *sap,int sal,char *dp,int dl) noex
	int connection_mknames(CON *cnp,vecstr *nlp) noex

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
#include	<cstring>		/* |strlen(3c)| */
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
#include	<sncpyx.h>
#include	<strwcpy.h>
#include	<isindomain.h>
#include	<isnot.h>
#include	<localmisc.h>

#include	"connection.h"


/* local defines */

#undef	LOCALHOST
#define	LOCALHOST	"localhost"

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

struct vars {
	int		maxpathlen ;
	int		maxhostlen ;
	int		socknamelen ;
	int		hostentlen ;
} ;


/* forward references */

static int connection_addname(CON *,vecstr *,cc *) noex ;
static int connection_ip4lookup(CON *,char *,int) noex ;
static int connection_ip6lookup(CON *,char *,int) noex ;

static int isNotFound(int rs) noex {
	return isNotPresent(rs) ;
}

static int mkvars() noex ;


/* local variables */

static vars		var ;

constexpr int		in4addrlen = INET4ADDRLEN ;
constexpr int		in6addrlen = INET6ADDRLEN ;


/* exported variables */


/* exported subroutines */

int connection_start(CON *cnp,cchar *inetdomain) noex {
	int		rs = SR_FAULT ;
	if (cnp && inetdomain) {
	    static cint	rsv = mkvars() ;
	    connection_head	*hop = static_cast<connection *>(cnp) ;
	    cint		ssz = sizeof(sockaddress) ;
	    memclear(hop) ;
	    if ((rs = rsv) >= 0) {
	        void	*vp{} ;
	        cnp->inetdomain = inetdomain ;
	        cnp->f.inet = false ;
	        cnp->s = -1 ;
	        if ((rs = uc_libmalloc(ssz,&vp)) >= 0) {
		    cnp->sap = sockaddressp(vp) ;
	        }
	    } /* end if (mkvars) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (connection_start) */

int connection_finish(CON *cnp) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (cnp) {
	    rs = SR_OK ;
	    if (cnp->f.inetdomain && cnp->inetdomain) {
	        cnp->f.inetdomain = false ;
	        rs1 = uc_free(cnp->inetdomain) ;
	        if (rs >= 0) rs = rs1 ;
	        cnp->inetdomain = nullptr ;
	    }
	    if (cnp->peername) {
	        rs1 = uc_free(cnp->peername) ;
	        if (rs >= 0) rs = rs1 ;
	        cnp->peername = nullptr ;
	    }
	    if (cnp->sap) {
		rs1 = uc_libfree(cnp->sap) ;
	        if (rs >= 0) rs = rs1 ;
		cnp->sap = nullptr ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (connection_finish) */

int connection_socklocname(CON *cnp,char *dp,int dl,int s) noex {
	int		rs = SR_FAULT ;
	int		len = 0 ;
	if (cnp && dp) {
	    rs = SR_BADF ;
	    if (s >= 0) {
		USTAT	sb ;
	        if ((rs = uc_fstat(s,&sb)) >= 0) {
		    if (S_ISSOCK(sb.st_mode)) {
	                sockaddress	*sap = cnp->sap ;
	                int		sal = sizeof(sockaddress) ;
	                cnp->s = s ;
	                if ((rs = u_getsockname(s,sap,&sal)) >= 0) {
	                    cnp->f.sa = true ;
	                    rs = connection_peername(cnp,sap,sal,dp,dl) ;
	                    len = rs ;
	                }
	    	    } else if (S_ISFIFO(sb.st_mode)) {
	                cchar	*lh = LOCALHOST ;
	                rs = sncpy1(dp,dl,lh) ;
	                len = rs ;
		    } /* end if */
		} /* end if (uc_fstat) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (connection_socklocname) */

int connection_sockremname(CON *cnp,char *dp,int dl,int s) noex {
	int		rs = SR_FAULT ;
	int		len = 0 ;
	if (cnp && dp) {
	    rs = SR_BADF ;
	    if (s >= 0) {
		USTAT	sb ;
	        if ((rs = uc_fstat(s,&sb)) >= 0) {
		    if (S_ISSOCK(sb.st_mode)) {
	                sockaddress	*sap = cnp->sap ;
	                int		sal = sizeof(sockaddress) ;
	                cnp->s = s ;
	                if ((rs = u_getpeername(s,sap,&sal)) >= 0) {
	                    cnp->f.sa = true ;
	                    rs = connection_peername(cnp,sap,sal,dp,dl) ;
	                    len = rs ;
	                }
	    	    } else if (S_ISFIFO(sb.st_mode)) {
	                cchar	*lh = LOCALHOST ;
	                rs = sncpy1(dp,dl,lh) ;
	                len = rs ;
		    } /* end if */
		} /* end if (uc_fstat) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (connection_sockremname) */

int connection_peername(CON *cnp,SA *sap,int sal,char *dp,int dl) noex {
	int		rs = SR_FAULT ;
	int		len = 0 ;
	if (cnp && sap && dp) {
	    cint	af = sockaddress_getaf(sap) ;
	    rs = SR_OK ;
	    dp[0] = '\0' ;
	    cnp->f.addr = false ;
	    cnp->f.sa = true ;
	    memcpy(cnp->sap,sap) ;
	    cnp->sal = sal ;
	    switch (af) {
	    case AF_UNIX:
	        rs = sockaddress_getaddr(sap,dp,dl) ;
	        len = rs ;
	        break ;
	    case AF_INET4:
	        cnp->f.inet = true ;
	        cnp->f.addr = true ;
	        rs = connection_ip4lookup(cnp,dp,dl) ;
	        len = rs ;
	        break ;
	    case AF_INET6:
	        cnp->f.inet = true ;
	        rs = connection_ip6lookup(cnp,dp,dl) ;
	        len = rs ;
	        break ;
	    default:
	        break ;
	    } /* end switch (got an INET host entry) */
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (connection_peername) */

namespace {
    struct sub_mknames {
	CON	*cnp ;
	vecstr	*nlp ;
	sub_mknames(CON *o,vecstr *n) noex : cnp(o), nlp(n) { } ;
	operator int () noex ;
	int addstuff(int) noex ;
	int addnames(hostinfo *) noex ;
	int addresses(hostinfo *) noex ;
    } ; /* end struct (sub_mknames) */
}

int connection_mknames(CON *cnp,vecstr *nlp) noex {
	int		rs = SR_FAULT ;
	int		n = 0 ;
	if (cnp && nlp) {
	    sub_mknames		mko(cnp,nlp) ;
	    rs = mko ;
	    n = rs ;
	} /* end if (non-null) */
	return (rs >= 0) ? n : rs ;
}
/* end subroutine (connection_mknames) */

sub_mknames::operator int () noex {
	int		rs = SR_DESTADDRREQ ;
	int		n = 0 ;
	if (cnp->f.sa) {
	    if ((rs = sockaddress_getaf(cnp->sap)) >= 0) {
	        cint	af = rs ;
	        switch (af) {
	        case AF_UNIX:
	            rs = connection_addname(cnp,nlp,LOCALHOST) ;
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
	hostinfo	hi ;
	int		rs ;
	int		rs1 ;
	int		n = 0 ;
	if ((rs = hostinfo_start(&hi,af,cnp->peername)) >= 0) {
	    if ((rs = addnames(&hi)) >= 0) {
		n += rs ;
		rs = addresses(&hi) ;
		n += rs ;
	    }
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
	    if ((rs = connection_addname(cnp,nlp,hp)) >= 0) {
	        n += rs ;
	        if ((rs = hostinfo_getcanonical(hip,&hp)) >= 0) {
		    hostinfo_cur	hc ;
	            rs = connection_addname(cnp,nlp,hp) ;
	            n += rs ;
	            if ((rs = hostinfo_curbegin(hip,&hc)) >= 0) {
	                while ((rs = hostinfo_enumname(hip,&hc,&hp)) > 0) {
	                    rs = connection_addname(cnp,nlp,hp) ;
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
	char		*nbuf{} ;
	if ((rs = malloc_hn(&nbuf)) >= 0) {
	    hostinfo_cur	hc ;
	    cint		nlen = rs ;
	    const uchar		*ap ;
	    if ((rs = hostinfo_curbegin(hip,&hc)) >= 0) {
	        while ((rs = hostinfo_enumaddr(hip,&hc,&ap)) > 0) {
		    inetaddr	ia ;
		    cint	al = rs ;
	            if (al != INET4ADDRLEN) continue ;
	            if ((rs = inetaddr_start(&ia,ap)) >= 0) {
	                auto in_ga = inetaddr_getdotaddr ;
	                if ((rs = in_ga(&ia,nbuf,nlen)) >= 0) {
	                    rs = vecstr_adduniq(nlp,nbuf,-1) ;
	                    if (rs < INT_MAX) n += 1 ;
		        }
	                rs1 = inetaddr_finish(&ia) ;
	   	        if (rs >= 0) rs = rs1 ;
	            } /* end if (inet4addr) */
	            if (rs < 0) break ;
	        } /* end while (addresses) */
	        rs1 = hostinfo_curend(hip,&hc) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (hostinfo-cur) */
	    rs1 = uc_free(&nbuf) ;
	    if (rs >= 0) rs = rs1 ;
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
	if ((af == AF_INET4) && cnp->f.inet && cnp->f.addr) {
	    char	*nbuf{} ;
	    if ((rs = malloc_hn(&nbuf)) >= 0) {
	        cint		nlen = rs ;
	        inetaddr	ia ;
	        if ((rs = inetaddr_start(&ia,&cnp->netipaddr)) >= 0) {
	            if ((rs = inetaddr_getdotaddr(&ia,nbuf,nlen)) >= 0) {
	                rs = vecstr_adduniq(nlp,nbuf,rs) ;
	                if (rs < INT_MAX) n += 1 ;
		    }
	            rs1 = inetaddr_finish(&ia) ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (inetaddr) */
	        rs1 = uc_free(&nbuf) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (m-a-f) */
	} /* end if (go) */
	return (rs >= 0) ? n : rs ;
}
/* end subroutine (connection_adddots) */
#endif /* COMMENT */


/* private subroutines */

static int connection_addname(CON *cnp,vecstr *nlp,cc *name) noex {
	int		nl = strlen(name) ;
	int		rs ;
	int		n = 0 ;
	if ((rs = vecstr_adduniq(nlp,name,nl)) >= 0) {
	    cchar	*dn = cnp->inetdomain ;
	    n += (rs < INT_MAX) ;
	    if (dn && isindomain(name,dn)) {
		cnullptr	np{} ;
	        if (cchar *tp ; (tp = strchr(name,'.')) != np) {
		    nl = (tp - name) ;
		}
		rs = vecstr_adduniq(nlp,name,nl) ;
	    	n += (rs < INT_MAX) ;
	    } /* end if (it is in our domain) */
	} /* end if (vecstr_adduniq) */
	return (rs >= 0) ? n : rs ;
}
/* end subroutine (connection_addname) */

/* lookup this IP (INET4) address */
static int connection_ip4lookup(CON *cnp,char *dp,int dl) noex {
	sockaddress	*sap = cnp->sap ;
	cint		alen = INET4ADDRLEN ;
	int		rs ;
	int		rs1 ;
	int		len = 0 ;
	if ((rs = sockaddress_getaddr(sap,&cnp->netipaddr,alen)) >= 0) {
	    char	*hebuf ;
	    if ((rs = malloc_ho(&hebuf)) >= 0) {
	        ucentho		he ;
	        cint		helen = rs ;
	        cint		af = AF_INET4 ;
	        cint		al = INET4ADDRLEN ;
	        cchar		*ap  = charp(&cnp->netipaddr) ;
	        if ((rs = getho_addr(&he,hebuf,helen,af,ap,al)) >= 0) {
	            hostent_cur		hc ;
	            cchar		*sp = nullptr ;
	            if (cnp->inetdomain != nullptr) {
	                if ((rs = hostent_curbegin(&he,&hc)) >= 0) {
	                    while ((rs = hostent_enumname(&he,&hc,&sp)) > 0) {
	                        if (isindomain(sp,cnp->inetdomain)) break ;
	                    } /* end while */
	                    rs1 = hostent_curend(&he,&hc) ;
			    if (rs >= 0) rs = rs1 ;
	                } /* end if (hostent-cur) */
	            } else {
	                rs1 = SR_INVALID ;
		    }
		    if (rs >= 0) {
	               if (sp) {
			    cnullptr	np{} ;
	                    int		nl = -1 ;
	                    if (cchar *tp ; (tp = strchr(sp,'.')) != np) {
			        nl = (tp-sp) ;
			    }
	                    rs = sncpy1w(dp,dl,sp,nl) ;
			    len = rs ;
	               } else {
		           /* it is NOT in our domain */
	                   if ((rs = hostent_getcanonical(&he,&sp)) >= 0) {
	                        rs = sncpy1(dp,dl,sp) ;
				len = rs ;
		           }
	               } /* end if */
		    } /* end if (ok) */
	        } else if (isNotFound(rs)) {
	            inetaddr	ia ;
	            if ((rs = inetaddr_start(&ia,&cnp->netipaddr)) >= 0) {
		        {
	                    rs = inetaddr_getdotaddr(&ia,dp,dl) ;
			    len = rs ;
		        }
	                rs1 = inetaddr_finish(&ia) ;
		        if (rs >= 0) rs = rs1 ;
	            } /* end if (inetadd) */
	        } /* end if */
	        rs1 = uc_free(hebuf) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (m-a-f) */
	} /* end if (sockaddress_getaddr) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (connection_ip4lookup) */

static int connection_ip6lookup(CON *cnp,char *dp,int dl) noex {
	int		rs ;
	int		rs1 ;
	int		len = 0 ;
	char		*hnbuf{} ;
	if ((rs = malloc_hn(&hnbuf)) >= 0) {
	    CSOCKADDR	*ssap = sockaddrp(cnp->sap) ;
	    cint	sal = cnp->sal ;
	    cint	slen = NI_MAXSERV ;
	    cint	hnlen = rs ;
	    cint	fl = NI_NOFQDN ;
	    ushort	in6addr[in6addrlen/2] ;
	    char	sbuf[NI_MAXSERV + 1] ;
	    if ((rs = uc_getnameinfo(ssap,sal,hnbuf,hnlen,sbuf,slen,fl)) >= 0) {
	        rs = sncpy1(dp,dl,hnbuf) ;
	        len = rs ;
	    } else if (isNotPresent(rs)) {
		sockaddress	*sap = cnp->sap ;
		cint		al = in6addrlen ;
	        if ((rs = sockaddress_getaddr(sap,in6addr,al)) >= 0) {
	            if (isin4mapped(in6addr)) {
	                cnp->f.addr = true ;
	                memcpy(&cnp->netipaddr,(in6addr + 6)) ;
	                rs = connection_ip4lookup(cnp,dp,dl) ;
	                len = rs ;
	            } else if (isin6loopback(in6addr)) {
		        cchar	*lh = LOCALHOST ;
	                rs = sncpy1(dp,dl,lh) ;
	                len = rs ;
	            } /* end if */
		} /* end if (sockaddress_getaddr) */
	    } /* end if (getnameinfo) */
	    rs1 = uc_free(hnbuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (connection_ip6lookup) */

static int mkvars() noex {
	int		rs ;
	if ((rs = getbufsize(getbufsize_mp)) >= 0) {
	    var.maxpathlen = rs ;
	    if ((rs = getbufsize(getbufsize_hn)) >= 0) {
	        var.maxhostlen = rs ;
	        if ((rs = getbufsize(getbufsize_ho)) >= 0) {
		    var.hostentlen = rs ;
	            var.socknamelen = max(var.maxpathlen,var.maxhostlen) ;
		}
	    }
	}
	return rs ;
}
/* end subroutine (mkvars) */


