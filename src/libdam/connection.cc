/* connection SUPPORT */
/* lang=C++20 */

/* manipulate INET connection information */
/* version %I% last-modified %G% */

#define	CF_LOCALFIFO	1		/* identify local FIFOs? */

/* revision history:

	= 1998-07-01, David A­D­ Morano
	This program was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	connection

	Description:
	This subroutine will take an INET socket and a local domain
	name and find the hostname of the remote end of the socket.

	Synopses:
	int connection_start(CON *cnp,cchar *domainname) noex
	int connection_finish(CON *cnp) noex
	int connection_socklocname(CON *cnp,char *dp,int dl,int s) noex
	int connection_sockremname(CON *cnp,char *dp,int dl,int s) noex

	Returns:
	>=0		OK
	<0		error (system-return)

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
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>		/* |strlen(3c)| */
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<netdb.h>
#include	<usystem.h>
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

#ifndef	VARPRLOCAL
#define	VARPRLOCAL	"LOCAL"
#endif

#undef	LOCALHOST
#define	LOCALHOST	"localhost"

#ifndef	INET4ADDRLEN
#define	INET4ADDRLEN	sizeof(struct in_addr)
#endif

#undef	HOSTBUFLEN
#define	HOSTBUFLEN	MAX(MAXHOSTNAMELEN,NI_MAXHOST)

#define	CON		connection
#define	SA		sockaddress


/* imported namespaces */

using std::nullptr_t ;			/* type */
using std::min ;			/* subroutine-template */
using std::max ;			/* subroutine-template */
using std::nothrow ;			/* constant */


/* local typedefs */

typedef sockaddress *	sockaddressp ;


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

static int connection_addname(CON *,vecstr *,cc *,char *,int *) noex ;
static int connection_ip4lookup(CON *,char *,int) noex ;

static int isin4mapped(ushort *) noex ;
static int isin6loopback(ushort *) noex ;

static int isNotFound(int rs) noex {
	return isNotPresent(rs) ;
}

static int mkvars() noex ;


/* local variables */

static vars		var ;

constexpr int		peerlen = CONNECTION_PEERNAMELEN ;


/* exported variables */


/* exported subroutines */

int connection_start(CON *cnp,cchar *domainname) noex {
	int		rs = SR_FAULT ;
	if (cnp && domainname) {
	    static cint	rsv = mkvars() ;
	    cint	ssz = sizeof(sockaddress) ;
	    memclear(cnp) ;
	    if ((rs = rsv) >= 0) {
	        void	*vp{} ;
	        cnp->domainname = charp(domainname) ;
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
	    if (cnp->f.domainname && cnp->domainname) {
	        cnp->f.domainname = false ;
	        rs1 = uc_free(cnp->domainname) ;
	        if (rs >= 0) rs = rs1 ;
	        cnp->domainname = nullptr ;
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
	                rs = sncpy1(peerbuf,peerlen,lh) ;
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
	                rs = sncpy1(peerbuf,peerlen,lh) ;
	                len = rs ;
		    } /* end if */
		} /* end if (uc_fstat) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (connection_sockremname) */

int connection_peername(CON *cnp,SA *sap,int sal,char *dp,int dl) noex {
	int		rs = SR_OK ;
	int		len = 0 ;
	cchar		*lh = LOCALHOST ;
	char		tmpnamebuf[HOSTBUFLEN + 1] ;
	char		namebuf[HOSTBUFLEN + 1] ;
	char		svcname[NI_MAXSERV + 1] ;

	if (cnp == nullptr) return SR_FAULT ;
	if (sap == nullptr) return SR_FAULT ;
	if (peerbuf == nullptr) peerbuf = namebuf ;

	peerbuf[0] = '\0' ;

/* is the peername cached? */

	if (cnp->peername != nullptr) {
	    if (peerbuf != nullptr) {
	        rs = sncpy1(peerbuf,peerlen,cnp->peername) ;
	        len = rs ;
	    } else {
	        rs = strlen(cnp->peername) ;
	        len = rs ;
	    }
	} /* end if (cached peername) */

/* we have to do it the hard way */

	if (rs >= 0) {
	    cint	af = sockaddress_getaf(sap) ;
	    int		alen = 0 ;
	    cnp->f.sa = true ;
	    cnp->f.addr = false ;
	    memcpy(cnp->sap,sap) ;
	    switch (af) {
	    case AF_UNIX:
		alen = MAXPATHLEN ;
	        if ((rs = sockaddress_getaddr(sap,peerbuf,alen)) >= 0) {
	            len = rs ;
	            if (len == 0) {
	                rs = sncpy1(peerbuf,peerlen,lh) ;
	                len = rs ;
		    }
	        }
	        break ;
	    case AF_INET4:
		alen = INET4ADDRLEN ;
	        cnp->f.inet = true ;
	        cnp->f.addr = true ;
	        if ((rs = sockaddress_getaddr(sap,&cnp->netipaddr,alen)) >= 0) {
	            rs = connection_ip4lookup(cnp,peerbuf,peerlen) ;
	            len = rs ;
	        }
	        break ;
	    case AF_INET6:
	        cnp->f.inet = true ;
	        {
	            CSOCKADDR	*ssap = sockaddressp(sap) ;
	            int		flags = NI_NOFQDN ;
	            ushort	in6addr[8] ;
	            rs = uc_getnameinfo(ssap,sal,tmpnamebuf,NI_MAXHOST,
	                svcname,NI_MAXSERV,flags) ;
	            if (rs >= 0) {
	                rs = sncpy1(peerbuf,MAXHOSTNAMELEN,tmpnamebuf) ;
	                len = rs ;
	            } else if (isNotPresent(rs)) {
	                sockaddress_getaddr(sap,in6addr,16) ;
	                if (isin4mapped(in6addr)) {
			    alen = INET6ADDRLEN ;
	                    cnp->f.addr = true ;
	                    memcpy(&cnp->netipaddr,(in6addr + 6),alen) ;
	                    rs = connection_ip4lookup(cnp,peerbuf,peerlen) ;
	                    len = rs ;
	                } else if (isin6loopback(in6addr)) {
	                    rs = sncpy1(peerbuf,peerlen,lh) ;
	                    len = rs ;
	                }
	            } /* end if (getnameinfo) */
	        } /* end if (AF_INET6) */
	        break ;
	    default:
	        break ;
	    } /* end switch (got an INET host entry) */

	    if (rs >= 0) {
	        if (peerbuf[0] != '\0') {
	            if (cnp->peername != nullptr) {
	                uc_free(cnp->peername) ;
	                cnp->peername = nullptr ;
	            }
	            cnp->peername = mallocstrw(peerbuf,len) ;
	            if (cnp->peername == nullptr) rs = SR_NOMEM ;
	        } else if (af != AF_UNIX) {
	            rs = SR_NOTFOUND ;
		}
	    } /* end if */

	} /* end if (ok) */

	return (rs >= 0) ? len : rs ;
}
/* end subroutine (connection_peername) */

int connection_mknames(CON *cnp,vecstr *nlp) noex {
	hostinfo	hi ;
	hostinfo_cur	hicur ;
	inetaddr	ia ;
	uint		af ;
	int		rs = SR_OK ;
	int		rs1 ;
	int		al ;
	int		n = 0 ;
	const uchar	*ap ;
	cchar		*hp ;
	char		namebuf[MAXHOSTNAMELEN + 1] ;

	if (cnp == nullptr) return SR_FAULT ;
	if (nlp == nullptr) return SR_FAULT ;

	if ((cnp->peername != nullptr) && (cnp->peername[0] != '\0')) {
	    rs = connection_addname(cnp,nlp,cnp->peername,namebuf,nullptr) ;
	    n += rs ;
	}

	if (rs < 0)
	    goto ret0 ;

	if (! cnp->f.sa) {
	    rs = SR_DESTADDRREQ ;
	    goto ret0 ;
	}

	rs = sockaddress_getaf(cnp->sap) ;
	af = rs ;
	if (rs < 0)
	    goto ret0 ;

/* UNIX family */

	if (af == AF_UNIX) {
	    n = 1 ;
	    rs = connection_addname(cnp,nlp,LOCALHOST,namebuf,nullptr) ;
	    goto ret0 ;
	}

/* prepare for INET hostname lookups */

	if (cnp->peername == nullptr)
	    goto ret0 ;

	if (rs == 0)
	    goto ret0 ;

/* INET families? */

	if ((rs = hostinfo_start(&hi,af,cnp->peername)) >= 0) {

	    if (rs >= 0) {
	        rs = hostinfo_geteffective(&hi,&hp) ;
	        if (rs >= 0) {
	            rs = connection_addname(cnp,nlp,hp,namebuf,nullptr) ;
	            n += rs ;
	        }
	    }

	    if (rs >= 0) {
	        if ((rs = hostinfo_getcanonical(&hi,&hp)) >= 0) {
	            rs = connection_addname(cnp,nlp,hp,namebuf,nullptr) ;
	            n += rs ;
	        }
	    }

/* all other names */

	    if (rs >= 0) {
	        if ((rs = hostinfo_curbegin(&hi,&hicur)) >= 0) {
	            while ((rs = hostinfo_enumname(&hi,&hicur,&hp)) > 0) {
	                rs = connection_addname(cnp,nlp,hp,namebuf,nullptr) ;
	                n += rs ;
	                if (rs < 0) break ;
	            } /* end while (names) */
	            hostinfo_curend(&hi,&hicur) ;
	        } /* end if (hostinfo-cur) */
	    } /* end if (names) */

/* addresses as names */

	    if (rs >= 0) {
		cint	nlen = MAXHOSTNAMELEN ;
	        if ((rs = hostinfo_curbegin(&hi,&hicur)) >= 0) {

	        while ((rs = hostinfo_enumaddr(&hi,&hicur,&ap)) > 0) {
		    int	al = rs ;
	            if (al != INET4ADDRLEN) continue ;

	            if ((rs = inetaddr_start(&ia,ap)) >= 0) {

	                inetaddr_getdotaddr(&ia,namebuf,nlen) ;

	                rs = vecstr_adduniq(nlp,namebuf,-1) ;
	                if (rs < INT_MAX) n += 1 ;

	                inetaddr_finish(&ia) ;
	            } /* end if (inet4addr) */

	            if (rs < 0) break ;
	        } /* end while (addresses) */

	        hostinfo_curend(&hi,&hicur) ;
		} /* end if (hostinfo-cur) */
	    } /* end if (addresses) */

	    rs1 = hostinfo_finish(&hi) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (hostinfo) */

/* store the INET "dot" address also! */

	if ((rs >= 0) && (af == AF_INET4) && cnp->f.inet && cnp->f.addr) {
	    if ((rs = inetaddr_start(&ia,&cnp->netipaddr)) >= 0) {
	        inetaddr_getdotaddr(&ia,namebuf,MAXHOSTNAMELEN) ;
	        rs = vecstr_adduniq(nlp,namebuf,-1) ;
	        if (rs < INT_MAX) n += 1 ;
	        inetaddr_finish(&ia) ;
	    } /* end if (inetaddr) */
	} /* end if (go) */

ret0:

	return (rs >= 0) ? n : rs ;
}
/* end subroutine (connection_mknames) */


/* private subroutines */

static int connection_addname(CON *cnp,vecstr *nlp,cc *name,
		char *nbuf,int *idp) noex {
	cint		rsn = SR_NOTFOUND ;
	int		rs ;
	int		n = 0 ;
	if ((rs = vecstr_find(nlp,name)) == rsn) {
	    n += 1 ;
	    rs = vecstr_add(nlp,name,-1) ;
	}
	if ((rs >= 0) && nbuf) {
	    cchar	*dn = cnp->domainname ;
	    if (dn && isindomain(name,dn)) {
	        int	nl = var.maxhostlen ;
	        if (idp) *idp = true ;
	        if (cchar *tp ; (tp = strchr(name,'.')) != nullptr) {
		    nl = (tp - name) ;
		}
	        nl = strwcpy(nbuf,name,nl) - nbuf ;
	        if ((rs = vecstr_findn(nlp,nbuf,nl)) == rsn) {
	            n += 1 ;
	            rs = vecstr_add(nlp,nbuf,nl) ;
	        }
	    } /* end if (it is in our domain) */
	} /* end if (non-null) */
	return (rs >= 0) ? n : rs ;
}
/* end subroutine (connection_addname) */

/* lookup this IP (INET4) address */
static int connection_ip4lookup(CON *cnp,char *peerbuf,int peerlen) noex {
	int		rs ;
	int		rs1 ;
	int		len = 0 ;
	char		*hebuf ;
	if ((rs = malloc_ho(&hebuf)) >= 0) {
	    HOSTENT	he ;
	    cint	helen = rs ;
	    cint	af = AF_INET4 ;
	    cint	al = INET4ADDRLEN ;
	    cchar	*ap  = charp(&cnp->netipaddr) ;
	    if ((rs = getho_addr(&he,hebuf,helen,af,ap,al)) >= 0) {
	        hostent_cur	hc ;
	        cchar		*sp = nullptr ;
	        if (cnp->domainname != nullptr) {
	            if ((rs = hostent_curbegin(&he,&hc)) >= 0) {
	                while ((rs = hostent_enumname(&he,&hc,&sp)) > 0) {
	                    if (isindomain(sp,cnp->domainname)) break ;
	                } /* end while */
	                hostent_curend(&he,&hc) ;
	            } /* end if (hostent-cur) */
	        } else {
	            rs1 = SR_INVALID ;
		}
		if (rs >= 0) {
	           if (sp) {
			cnullptr	np{} ;
	                int	nl = -1 ;
	                if (cchar *tp ; (tp = strchr(sp,'.')) != np) {
			    nl = (tp-sp) ;
			}
	                rs = sncpy1w(peerbuf,peerlen,sp,nl) ;
	           } else {
		       /* it is NOT in our domain */
	               if ((rs = hostent_getcanonical(&he,&sp)) >= 0) {
	                   rs = sncpy1(peerbuf,peerlen,sp) ;
		       }
	           } /* end if */
		} /* end if (ok) */

	    } else if (isNotFound(rs)) {
	        inetaddr	ia ;
	        if ((rs = inetaddr_start(&ia,&cnp->netipaddr)) >= 0) {
		    {
	                rs = inetaddr_getdotaddr(&ia,peerbuf,peerlen) ;
			len = rs ;
		    }
	            rs1 = inetaddr_finish(&ia) ;
		    if (rs >= 0) rs = rs1 ;
	        } /* end if (inetadd) */
	    } /* end if */
	    rs1 = uc_free(hebuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (connection_ip4lookup) */

static int isin4mapped(ushort *in6addr) noex {
	int		f = false ;
	int		i ; /* used afterwards */
	for (i = 0 ; i < 5 ; i += 1) {
	    if (in6addr[i] != 0) break ;
	}
	if (i == 5) {
	    const ushort	v = in6addr[5] ;
	    f = ((v == 0) || (v == 0xFFFF)) ;
	}
	return f ;
}
/* end subroutine (isin4mapped) */

static int isin6loopback(ushort *in6addr) noex {
	int		f = false ;
	int		i ; /* used-afterwards */
	for (i = 0 ; i < 7 ; i += 1) {
	    if (in6addr[i] != 0) break ;
	}
	if (i == 7) {
	    const ushort	v = ntohs(in6addr[7]) ;
	    f = (v == 0x0001) ;
	}
	return f ;
}
/* end subroutine (isin6loopback) */

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


