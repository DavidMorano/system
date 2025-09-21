/* geteaddrinfo SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* subroutine to get a canonical hostname */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This program was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	geaddrinfo

	Description:
	This subroutine is used to get a canonical INET hostname
	for a supplied name.  Note carefully that the returned
	hostname, if any, may NOT be a name that can be translated
	into a good INET address.  In other words, this subroutine
	uses its own definition of a "canonical" name and that
	definition does NOT necessarily include the fact that the
	resulting name can be translated into a good INET address.
	If you want a name that is guaranteed to be translatable
	into a valid INET address then you would use the 'ehostname'
	name that is supplied as a result of calling this subroutine.

	Having said that the resuling name is not guaranteed to be
	translatable, a good translation facility will generally
	figure out that the given name is something that can be
	translated given the existing host information.

	Synopsis:
	int geteaddrinfo(cc *hn,cc *svc,AI *hintp,char *ehbuf,cc **rpp) noex

	Arguments:
	hn		name of host to lookup
	svc		name of service to lookup
	hintp		pointer to 'addrinfo' structure (hint)
	ehbuf		caller-supplied buffer to receive "effective" name
	rpp		pointer to pointer to 'addrinfo' result data

	Returns:
	>=0		<name> had a valid INET address
	<0		<name> did not have a valid address (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/socket.h>
#include	<netinet/in.h>
#include	<arpa/inet.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>		/* |strcmp(3c)| */
#include	<netdb.h>
#include	<usystem.h>
#include	<bufsizevar.hh>
#include	<mallocxx.h>
#include	<getnodedomain.h>
#include	<snx.h>
#include	<snwcpy.h>
#include	<isnot.h>
#include	<isinetaddr.h>
#include	<isindomain.h>
#include	<localmisc.h>

#include	"geteaddrinfo.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |memclear(3u)| + |lenstr(3u)| */

/* local defines */

#ifndef	LOCALHOST
#define	LOCALHOST	"localhost"
#endif

#ifndef	ANYHOST
#define	ANYHOST		"anyhost"
#endif

#ifndef	LOCALDOMAINNAME
#define	LOCALDOMAINNAME	"local"
#endif

#define	AI		ADDRINFO

#define	ARI		arginfo

#define	SI		subinfo
#define	SI_FL		subinfo_flags


/* imported namespaces */

using libuc::libmem ;			/* variable */
using std::nothrow ;			/* constant */


/* local typedefs */


/* external subroutines */


/* local structures */

struct arginfo {
	cchar		*hostname ;
	cchar		*svcname ;
	ADDRINFO	*hintp ;
	ADDRINFO	**rpp ;
} ;

struct subinfo_flags {
	uint		inetaddr:1 ;
	uint		inetaddr_start:1 ;
} ;

struct subinfo {
	arginfo		*aip ;		/* user argument (hint) */
	char		*ehostname ;	/* user argument (writable) */
	cchar		*domainname ;	/* dynamically determined */
	SI_FL		fl ;
	int		rs_last ;
} ;


/* forward references */

static int	arginfo_load(ARI *,cc *,cc *,AI *,AI **) noex ;

static int	subinfo_start(SI *,char *,ARI *) noex ;
static int	subinfo_domain(SI *) noex ;
static int	subinfo_finish(SI *) noex ;

static int	try_straight(SI *) noex ;
static int	try_add(SI *) noex ;
static int	try_rem(SI *) noex ;
static int	try_remlocal(SI *) noex ;


/* external variables */


/* local variables */

static constexpr int	(*tries[])(SI *) = {
	try_straight,
	try_add,
	try_rem,
	try_remlocal,
	nullptr
} ;

static bufsizevar		maxhostlen(getbufsize_hn) ;


/* exported variables */


/* exported subroutines */

int geteaddrinfo(cc *hn,cc *svc,AI *hintp,char *ehostname,AI **rpp) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		rs_last = SR_NOTFOUND ;
	int		c = 0 ;
	if (hn && svc) {
	    if (ARI argi ; (rs = arginfo_load(&argi,hn,svc,hintp,rpp)) >= 0) {
	        if (SI mi ; (rs = subinfo_start(&mi,ehostname,&argi)) >= 0) {
	            for (int i = 0 ; tries[i] ; i += 1) {
	                rs = (*tries[i])(&mi) ;
		        c = rs ;
	                if (rs != 0) break ;
	            } /* end for */
	            rs_last = mi.rs_last ;
	            rs1 = subinfo_finish(&mi) ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (subinfo) */
	        if ((rs >= 0) && (c == 0)) rs = rs_last ;
	    } /* end if (arginfo_load) */
	} /* end if (non-null) */
	return (rs >= 0) ? c : rs ;
}
/* end if (geteaddrinfo) */


/* local subroutines */

static int subinfo_start(SI *mip,char *ehostname,ARI *aip) noex {
	int		rs = SR_FAULT ;
	if (mip && aip) {
	    rs = memclear(mip) ;
	    mip->domainname = nullptr ;
	    mip->aip = aip ;
	    mip->ehostname = ehostname ;
	    mip->rs_last = SR_NOTFOUND ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (subinfo_start) */

static int subinfo_finish(SI *mip) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (mip->domainname) {
	    void *vp = voidp(mip->domainname) ;
	    rs1 = libmem.free(vp) ;
	    if (rs >= 0) rs = rs1 ;
	    mip->domainname = nullptr ;
	}
	return rs ;
}
/* end subroutine (subinfo_finish) */

static int subinfo_domain(SI *mip) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	int		len = 0 ; /* return-value */
	if (mip->domainname == nullptr) {
	    if (char *dbuf ; (rs = malloc_hn(&dbuf)) >= 0) {
	        if ((rs = getnodedomain(nullptr,dbuf)) >= 0) {
	            len = lenstr(dbuf) ;
	            if (cchar *dp ; (rs = libmem.strw(dbuf,len,&dp)) >= 0) {
	                mip->domainname = dp ;
		    } /* end if (memory-allocation) */
	        } /* end if (getnodedomain) */
		rs1 = malloc_free(dbuf) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (m-a-f) */
	} else {
	    len = lenstr(mip->domainname) ;
	}
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (subinfo_domain) */

static int try_straight(SI *mip) noex {
	arginfo		*aip = mip->aip ;
	int		rs ;
	int		c = 0 ;
	cchar		*hn = aip->hostname ;
	if (hn != nullptr) {
	    bool	f = false ;
	    f = f || ((hn[0] == 'a') && (strcmp(hn,ANYHOST) == 0)) ;
	    f = f || (hn[0] == '*') ;
	    f = f || (hn[0] == '\0') ;
	    if (f) {
	        hn = nullptr ;
	    }
	}
	if ((rs = uc_addrinfoget(hn,aip->svcname,aip->hintp,aip->rpp)) >= 0) {
	    if (mip->ehostname != nullptr) {
	        mip->ehostname[0] = '\0' ;
	        if (aip->hostname != nullptr) {
		    if ((rs = maxhostlen) >= 0) {
			cint	hlen = rs ;
		        c = 1 ;
	                rs = sncpy1(mip->ehostname,hlen,aip->hostname) ;
		    } /* end if (maxhostlen) */
	        }
	    }
	} else if (isNotPresent(rs)) {
	    mip->rs_last = rs ;
	    rs = SR_OK ;
	}
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (try_straight) */

static int try_add(SI *mip) noex {
	arginfo		*aip = mip->aip ;
	int		rs = SR_OK ;
	int		rs1 ;
	int		c = 0 ;
	if (aip->hostname != nullptr) {
	    if (strchr(aip->hostname,'.') == nullptr) {
	        if (! isinetaddr(aip->hostname)) {
	            if ((rs = subinfo_domain(mip)) >= 0) {
			cchar	*dn = mip->domainname ;
			cchar	*hn = aip->hostname ;
	                if (char *hbuf ; (rs = malloc_hn(&hbuf)) >= 0) {
			    cint	hlen = rs ;
	                    char	*bp = hbuf ;
	                    if (mip->ehostname != nullptr) {
			        bp = mip->ehostname ;
			    }
	                    if ((rs = snsds(bp,hlen,hn,dn)) >= 0) {
			        ADDRINFO	*hp = aip->hintp ;
			        ADDRINFO	**rpp = aip->rpp ;
	                        cchar		*sn = aip->svcname ;
				auto		gai = uc_addrinfoget ;
	                        if ((rs = gai(bp,sn,hp,rpp)) >= 0) {
				    c = 1 ;
			        } else if (isNotPresent(rs)) {
	    			    mip->rs_last = rs ;
				    rs = SR_OK ;
			        }
	                    } /* end if (snsds) */
			    rs1 = malloc_free(hbuf) ;
			    if (rs >= 0) rs = rs1 ;
			} /* end if (m-a-f) */
	            } /* end if (subinfo_domain) */
	        } /* end if (isinetaddr) */
	    }
	}
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (try_add) */

static int try_rem(SI *mip) noex {
	arginfo		*aip = mip->aip ;
	int		rs = SR_OK ;
	int		rs1 ;
	int		c = 0 ;
	if (aip->hostname != nullptr) {
	    if (! isinetaddr(aip->hostname)) {
		cnullptr	np{} ;
	        if (cchar *tp ; (tp = strchr(aip->hostname,'.')) != np) {
	            if ((rs = subinfo_domain(mip)) >= 0) {
	                rs = SR_NOTFOUND ;
	                if (isindomain(aip->hostname,mip->domainname)) {
	                    int		hl = intconv(tp - aip->hostname) ;
			    cchar	*hn = aip->hostname ;
			    if (char *hbuf ; (rs = malloc_hn(&hbuf)) >= 0) {
				cint	hlen = rs ;
	    		        char	*bp = hbuf ;
			        if (mip->ehostname != nullptr) {
	                            bp = mip->ehostname ;
	                        }
	    		        if ((rs = snwcpy(bp,hlen,hn,hl)) >= 0) {
				    ADDRINFO	*hp = aip->hintp ;
				    ADDRINFO	**rpp = aip->rpp ;
				    cchar	*sn = aip->svcname ;
				    auto	gai = uc_addrinfoget ;
	        		    if ((rs = gai(bp,sn,hp,rpp)) >= 0) {
				        c = 1 ;
			            } else if (isNotPresent(rs)) {
	    			        mip->rs_last = rs ;
				        rs = SR_OK ;
			            }
			        } /* end if (snwcpy) */
				rs1 = malloc_free(&hbuf) ;
				if (rs >= 0) rs = rs1 ;
			    } /* end if (m-a-f) */
			} /* end if (isindomain) */
		    } /* end if (subinfo_domain) */
	        }
	    }
	}
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (try_rem) */

static int try_remlocal(SI *mip) noex {
	arginfo		*aip = mip->aip ;
	cnullptr	np{} ;
	int		rs = SR_OK ;
	int		rs1 ;
	int		c = 0 ;
	if (aip->hostname != nullptr) {
	    if (! isinetaddr(aip->hostname)) {
	        if (cchar *tp ; (tp = strchr(aip->hostname,'.')) != np) {
	            if (isindomain(aip->hostname,LOCALDOMAINNAME)) {
	                int	hl = intconv(tp - aip->hostname) ;
			cchar	*hn = aip->hostname ;
	                if (char *hbuf ; (rs = malloc_hn(&hbuf)) >= 0) {
			    cint	hlen = rs ;
			    char	*bp = hbuf ;
	    		    if (mip->ehostname != nullptr) {
			        bp = mip->ehostname ;
	    		    }
	    		    if ((rs = snwcpy(bp,hlen,hn,hl)) >= 0) {
			        ADDRINFO	*hp = aip->hintp ;
			        ADDRINFO	**rpp = aip->rpp ;
			        cchar		*sn = aip->svcname ;
			        auto		gai = uc_addrinfoget ;
	                        if ((rs = gai(bp,sn,hp,rpp)) >= 0) {
				    c = 1 ;
			        } else if (isNotPresent(rs)) {
	    			    mip->rs_last = rs ;
				    rs = SR_OK ;
			        }
	                    } /* end if (snwcpy) */
			    rs1 = malloc_free(&hbuf) ;
			    if (rs >= 0) rs = rs1 ;
			} /* end if (m-a-f) */
	            } /* end if (isindomain) */
	        } /* end if (strchr) */
	    }
	}
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (try_remlocal) */

static int arginfo_load(ARI *aip,cc *hn,cc *svc,AI *hintp,AI **rpp) noex {
	int		rs = SR_FAULT ;
	if (aip) {
	    rs = SR_OK ;
	    aip->hostname = hn ;
	    aip->svcname = svc ;
	    aip->hintp = hintp ;
	    aip->rpp = rpp ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (arginfo_load) */


