/* hostinfo SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* manipulate host entry structures */
/* version %I% last-modified %G% */

#define	CF_HOSTBYNAME	1		/* use |gethostbyname(3nsl)| */
#define	CF_FASTADDR	1		/* use fast-addr */

/* revision history:

	= 2005-02-03, David A­D­ Morano
	This module was originally written.

*/

/* Copyright © 2005 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	hostinfo

	Description:
	This object provides the functionality that was previosuly
	supplied by the subroutine |gethostbyname(3snl)|.  That
	subroutine only worked for INET4 class addresses.  This
	object returns host-entry information for both INET4 and
	INET6 class addresses.

	Notes:

	Q. What is this crazy order of evalations being done?
	A. Is is supposed to try to reduce the amount of work needed
	   to find any single result for a method call.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/socket.h>
#include	<netinet/in.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>		/* <- for |strlen(4c)| */
#include	<new>
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<netdb.h>
#include	<usystem.h>
#include	<getbufsize.h>
#include	<bufsizevar.hh>
#include	<mallocxx.h>
#include	<getxx.h>
#include	<hostent.h>
#include	<vecobj.h>
#include	<strwcpy.h>
#include	<strn.h>
#include	<snxxx.h>
#include	<sncpyx.h>
#include	<snwcpy.h>
#include	<nleadstr.h>
#include	<inetconv.h>
#include	<isinetaddr.h>
#include	<getnodename.h>
#include	<getaf.h>
#include	<isindomain.h>
#include	<isnot.h>
#include	<localmisc.h>

#include	"hostinfo.h"


/* local defines */

#define	HOSTINFO_N	struct hostinfo_n
#define	HOSTINFO_A	struct hostinfo_a

#ifndef	LOCALDOMAINNAME
#define	LOCALDOMAINNAME		"local"
#endif

#ifndef	CF_HOSTBYNAME
#define	CF_HOSTBYNAME	0		/* backup safety definition */
#endif

#ifndef	CF_FASTADDR
#define	CF_FASTADDR	0		/* backup safety definition */
#endif


/* imported namespaces */

using std::nullptr_t ;			/* type */
using std::min ;			/* subroutine-template */
using std::max ;			/* subroutine-template */
using std::nothrow ;			/* constant */


/* local typedefs */

extern "C" {
    typedef int (*vog_f)(vecobj *,int,void **) noex ;
}


/* external subroutines */


/* external variables */


/* local structures */

struct hostinfo_n {
	cchar		*name ;
	int		namelen ;
	int		af ;
} ;

struct hostinfo_a {
	int		af ;
	int		addrlen ;
	uchar		addr[INETXADDRLEN + 1] ;
} ;

struct known {
	cchar		*name ;
	uint		a ;
} ;


/* forward references */

template<typename ... Args>
static inline int hostinfo_ctor(hostinfo *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = SR_NOMEM ;
	    op->magic = 0 ;
	    op->init = {} ;
	    op->fl = {} ;
	    op->arg = {} ;
	    op->addr = {} ;
	    op->domainname = nullptr ;
	    op->a = nullptr ;
	    if ((op->nlp = new(nothrow) vecobj) != nullptr) {
	        if ((op->alp = new(nothrow) vecobj) != nullptr) {
		    rs = SR_OK ;
		} /* end if (new-vecobj) */
		if (rs < 0) {
		    delete op->nlp ;
		    op->nlp = nullptr ;
		}
	    } /* end if (new-vecobj) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (hostinfo_dtor) */

static inline int hostinfo_dtor(hostinfo *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	    if (op->alp) {
		delete op->alp ;
		op->alp = nullptr ;
	    }
	    if (op->nlp) {
		delete op->nlp ;
		op->nlp = nullptr ;
	    }
	} /* end if (non-null) */
	return rs ;
}

template<typename ... Args>
static int hostinfo_magic(hostinfo *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magic == HOSTINFO_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}

static int	hostinfo_bufbegin(hostinfo *) noex ;
static int	hostinfo_bufend(hostinfo *) noex ;
static int	hostinfo_argsbegin(hostinfo *,uint,cchar *) noex ;
static int	hostinfo_argsend(hostinfo *) noex ;
static int	hostinfo_domain(hostinfo *) noex ;
static int	hostinfo_findcanonical(hostinfo *) noex ;
static int	hostinfo_getname(hostinfo *,int,cchar *) noex ;
static int	hostinfo_getaddr(hostinfo *,int) noex ;
static int	hostinfo_loadaddrs(hostinfo *,int,HOSTENT *) noex ;
static int	hostinfo_loadnames(hostinfo *,int,HOSTENT *) noex ;
static int	hostinfo_addname(hostinfo *,cchar *,int,int) noex ;
static int	hostinfo_finishnames(hostinfo *) noex ;
static int	hostinfo_addrbegin(hostinfo *,int) noex ;
static int	hostinfo_addrend(hostinfo *) noex ;
static int	hostinfo_loadknownaddr(hostinfo *,int,uint) noex ;

static int	getinet(hostinfo *,int) noex ;
static int	getinet_straight(hostinfo *,int) noex ;
static int	getinet_add(hostinfo *,int) noex ;
static int	getinet_rem(hostinfo *,int) noex ;
static int	getinet_remlocal(hostinfo *,int) noex ;
static int	getinet_known(hostinfo *,int) noex ;
static int	getinet_knowner(hostinfo *,int) noex ;

static int	matknown(cchar *,int) noex ;

extern "C" {
    static int	vmatname(cvoid **,cvoid **) noex ;
    static int	vmataddr(cvoid **,cvoid **) noex ;
}


/* local variables */

constexpr int		(*getinets[])(hostinfo *,int) = {
	getinet_rem,
	getinet_remlocal,
	getinet_straight,
	getinet_add,
	getinet_known,
	nullptr
} ;

constexpr known		knowns[] = {
	{ "localhost", 0x7F000001 },
	{ "anyhost",   0x00000000 },
	{ "allhost",   0xFFFFFFFF },
	{ "broadcast", 0xFFFFFFFF },
	{ "testhost",  0x7F0000FF },
	{ "local",     0x7F000001 },
	{ nullptr,     0x00000000 }
} ;

static bufsizevar	maxhostlen(getbufsize_hn) ;

constexpr int		af0 = AF_UNSPEC ;
constexpr int		af4 = AF_INET4 ;
constexpr int		af6 = AF_INET6 ;

constexpr bool		f_hostbyname = CF_HOSTBYNAME ;
constexpr bool		f_fastaddr = CF_FASTADDR ;

static inline bool isaf4(int a) noex {
	return ((a == af0) || (a == af4)) ;
}

static inline bool isaf6(int a) noex {
	return ((a == af0) || (a == af6)) ;
}


/* exported variables */


/* exported subroutines */

int hostinfo_start(hostinfo *op,int af,cchar *hn) noex {
	int		rs ;
	if ((rs = hostinfo_ctor(op,hn)) >= 0) {
	    rs = SR_INVALID ;
	    if (hn[0] && (af >= 0)) {
		if ((rs = hostinfo_bufbegin(op)) >= 0) {
	            if ((rs = hostinfo_argsbegin(op,af,hn)) >= 0) {
		        vecobj	*nlp = op->nlp ;
			cint	vn = 10 ;
	                cint	vo = VECOBJ_OCOMPACT ;
	                int	osz = sizeof(HOSTINFO_N) ;
	                if ((rs = vecobj_start(nlp,osz,vn,vo)) >= 0) {
			    vecobj	*alp = op->alp ;
	                    osz = sizeof(HOSTINFO_A) ;
	                    if ((rs = vecobj_start(alp,osz,vn,vo)) >= 0) {
	                        rs = 0 ;
	                        if (isaf4(af)) {
	                            rs = getinet(op,af4) ;
	                        }
	                        if (rs == 0) {
			            if (isaf6(af)) {
	                                rs = getinet(op,af6) ;
			            }
	                        }
	                        if (rs >= 0) {
	                            op->magic = HOSTINFO_MAGIC ;
	                        } else { /* error */
	                            if (op->domainname) {
	                                uc_free(op->domainname) ;
	                                op->domainname = nullptr ;
	                            }
	                            hostinfo_addrend(op) ;
	                            vecobj_finish(op->alp) ;
	                        }
	                    } /* end if (vecobj-addrs) */
	                    if (rs < 0) {
	                        hostinfo_finishnames(op) ;
	                        vecobj_finish(op->nlp) ;
	                    }
	                } /* end if (vecobj-names) */
	                if (rs < 0) {
	                    hostinfo_argsend(op) ;
	                }
	            } /* end if (hostinfo_argsbegin) */
		    if (rs < 0) {
	                hostinfo_bufend(op) ;
		    }
	        } /* end if (hostinfo_bufbegin) */
	    } /* end if (valid) */
	    if (rs < 0) {
		hostinfo_dtor(op) ;
	    }
	} /* end if (hostinfo_ctor) */
	return rs ;
}
/* end subroutine (hostinfo_start) */

int hostinfo_finish(hostinfo *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = hostinfo_magic(op)) >= 0) {
            {
                rs1 = hostinfo_addrend(op) ;
                if (rs >= 0) rs = rs1 ;
            }
            if (op->domainname) {
                rs1 = uc_free(op->domainname) ;
                if (rs >= 0) rs = rs1 ;
                op->domainname = nullptr ;
            }
            {
                rs1 = hostinfo_finishnames(op) ;
                if (rs >= 0) rs = rs1 ;
            }
            {
                rs1 = vecobj_finish(op->alp) ;
                if (rs >= 0) rs = rs1 ;
            }
            {
                rs1 = vecobj_finish(op->nlp) ;
                if (rs >= 0) rs = rs1 ;
            }
            {
                rs1 = hostinfo_argsend(op) ;
                if (rs >= 0) rs = rs1 ;
            }
            {
                rs1 = hostinfo_bufend(op) ;
                if (rs >= 0) rs = rs1 ;
            }
	    {
		rs1 = hostinfo_dtor(op) ;
		if (rs >= 0) rs = rs1 ;
	    }
	    op->magic = 0 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (hostinfo_finish) */

int hostinfo_getoffical(hostinfo *op,cchar **rpp) noex {
	int		rs ;
	int		nlen = 0 ;
	if ((rs = hostinfo_magic(op)) >= 0) {
	        if (op->chostname[0] == '\0') {
	            rs = hostinfo_findcanonical(op) ;
	        }
	        if ((rs >= 0) && (op->chostname[0] != '\0')) {
	            nlen = strlen(op->chostname) ;
	            if (rpp != nullptr) *rpp = op->chostname ;
	        } /* end if */
	        if ((rs < 0) && (rpp != nullptr)) {
	            *rpp = nullptr ;
	        }
	} /* end if (magic) */
	return (rs >= 0) ? nlen : rs ;
}
/* end subroutine (hostinfo_getofficial) */

int hostinfo_geteffective(hostinfo *op,cchar **rpp) noex {
	int		rs ;
	int		nlen = 0 ;
	if ((rs = hostinfo_magic(op)) >= 0) {
	        if (op->ehostname[0]) {
	            rs = SR_OK ;
	            nlen = strlen(op->ehostname) ;
	            if (rpp) *rpp = op->ehostname ;
	        } /* end if */
	        if ((rs < 0) && rpp) {
	            *rpp = nullptr ;
	        }
	} /* end if (magic) */
	return (rs >= 0) ? nlen : rs ;
}
/* end subroutine (hostinfo_geteffective) */

int hostinfo_getcanonical(hostinfo *op,cchar **rpp) noex {
	int		rs ;
	int		nlen = 0 ;
	if ((rs = hostinfo_magic(op)) >= 0) {
	        if (op->chostname[0] == '\0') {
	            rs = hostinfo_findcanonical(op) ;
	        }
	        if (rs >= 0) {
	            if (op->chostname[0] != '\0') {
	                nlen = strlen(op->chostname) ;
	                if (rpp) *rpp = op->chostname ;
	            } else {
	                rs = SR_NOTFOUND ;
	            }
	        } /* end if */
	        if ((rs < 0) && rpp) {
	            *rpp = nullptr ;
	        }
	} /* end if (magic) */
	return (rs >= 0) ? nlen : rs ;
}
/* end subroutine (hostinfo_getcanonical) */

int hostinfo_curbegin(hostinfo *op,hostinfo_cur *curp) noex {
	int		rs ;
	if ((rs = hostinfo_magic(op,curp)) >= 0) {
	        curp->i = -1 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (hostinfo_curbegin) */

int hostinfo_curend(hostinfo *op,hostinfo_cur *curp) noex {
	int		rs ;
	if ((rs = hostinfo_magic(op,curp)) >= 0) {
	        curp->i = -1 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (hostinfo_curend) */

/* enumerate the next hostname */
int hostinfo_curenumname(hostinfo *op,hostinfo_cur *curp,cchar **rpp) noex {
	int		rs ;
	int		nlen = 0 ;
	if ((rs = hostinfo_magic(op,curp)) >= 0) {
	        hostinfo_cur	dcur ;
	        bool		f_cur = false ;
	        if (rpp) *rpp = nullptr ;
	        if (curp == nullptr) {
	            curp = &dcur ;
	            f_cur = true ;
	            rs = hostinfo_curbegin(op,&dcur) ;
	        } /* end if (user supplied no cursor) */
	        if (rs >= 0) {
	            vecobj	*nlp = op->nlp ;
	            HOSTINFO_N	*nep = nullptr ;
	            cint	rsn = SR_NOTFOUND ;
	            int		ci = (curp->i >= 0) ? (curp->i + 1) : 0 ;
	            bool	f_exit = false ;
	            vog_f	vg = vecobj_get ;
	            repeat {
	                int	i ; /* used-afterwards */
		        void	*vp{} ;
	                for (i = ci ; (rs = vg(nlp,i,&vp)) >= 0 ; i += 1) {
		            nep = (HOSTINFO_N *) vp ;
	                    if (nep != nullptr) break ;
	                } /* end for */
	                ci = i ;
	                if (rs == rsn) {
	                    rs = SR_OK ;
	                    if ((! op->fl.inet4) || (! op->fl.inet6)) {
	                        if ((rs == 0) && (! op->fl.inet4)) {
	                            rs = getinet(op,af4) ;
	                        }
	                        if ((rs == 0) && (! op->fl.inet6)) {
	                            rs = getinet(op,af6) ;
	                        }
	                        f_exit = (rs == 0) ;
	                    } else {
	                        f_exit = true ;
	                    }
	                } else if (rs == 0) {
	                    rs = 1 ;
	                } /* end if */
	            } until ((rs != 0) || f_exit) ;
	            if (rs > 0) { /* found */
	                curp->i = ci ;
	                if (nep != nullptr) {
	                    nlen = nep->namelen ;
	                    if (rpp != nullptr) *rpp = nep->name ;
	                }
	            } /* end if */
	            if (f_cur) {
	                rs = hostinfo_curend(op,&dcur) ;
	            }
	        } /* end if (ok) */
	} /* end if (magic) */
	return (rs >= 0) ? nlen : rs ;
}
/* end subroutine (hostinfo_curenumname) */

/* enumerate the next host address */
int hostinfo_curenumaddr(hostinfo *op,hostinfo_cur *curp,cuchar **rpp) noex {
	int		rs ;
	int		alen = 0 ;
	if ((rs = hostinfo_magic(op,curp)) >= 0) {
	        hostinfo_cur	dcur ;
	        bool		f_cur = false ;
	        if (rpp) *rpp = nullptr ;
	        if (curp == nullptr) {
	            curp = &dcur ;
	            f_cur = true ;
	            rs = hostinfo_curbegin(op,&dcur) ;
	        } /* end if (user supplied no cursor) */
	        if (rs >= 0) {
	            vecobj	*alp = op->alp ;
	            HOSTINFO_A	*aep = nullptr ;
	            cint	rsn = SR_NOTFOUND ;
	            int		ci = (curp->i >= 0) ? (curp->i + 1) : 0 ;
	            vog_f	vg = vecobj_get ;
	            bool	f_exit = false ;
	            repeat {
	                int	i = 0 ; /* used afterwards */
		        void	*vp{} ;
	                for (i = ci ; (rs = vg(alp,i,&vp)) >= 0 ; i += 1) {
		            aep = (HOSTINFO_A *) vp ;
	                    if (aep != nullptr) break ;
	                } /* end for */
	                ci = i ;
	                if (rs == rsn) {
	                    rs = SR_OK ;
	                    if ((op->fl.inet4) || (! op->fl.inet6)) {
	                        if ((rs == 0) && (! op->fl.inet4)) {
	                            rs = getinet(op,af4) ;
	                        }
	                        if ((rs == 0) && (! op->fl.inet6)) {
	                            rs = getinet(op,af6) ;
	                        }
	                        f_exit = (rs == 0) ;
	                    } else {
	                        f_exit = true ;
	                    }
	                } else if (rs == 0) {
	                    rs = 1 ;
	                } /* end if */
	            } until ((rs != 0) || f_exit) ;
	            if (rs > 0) { /* found */
	                curp->i = ci ;
	                if (aep != nullptr) {
	                    alen = aep->addrlen ;
	                    if (rpp) *rpp = aep->addr ;
	                }
	            } /* end if */
	            if (f_cur) {
	                rs = hostinfo_curend(op,&dcur) ;
	            }
	        } /* end if (ok) */
	} /* end if (magic) */
	return (rs >= 0) ? alen : rs ;
}
/* end subroutine (hostinfo_curenumaddr) */


/* private subroutines */

static int hostinfo_bufbegin(hostinfo *op) noex {
	int		rs ;
	if ((rs = maxhostlen) >= 0) {
	    cint	hlen = rs ;
	    cint	sz = ((rs + 1) * 2) ;
	    caddr_t	*ap{} ;
	    if ((rs = uc_malloc(sz,&ap)) >= 0) {
		op->a = ap ;
		op->ehostname = charp(ap) ;
		op->chostname = charp(ap + (hlen + 1)) ;
		op->ehostname[0] = '\0' ;
		op->chostname[0] = '\0' ;
	    }
	} /* end if (maxhostlen) */
	return rs ;
}
/* end subroutine (hostinfo_bufbegin) */

static int hostinfo_bufend(hostinfo *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (op->a) {
	    rs1 = uc_free(op->a) ;
	    if (rs >= 0) rs = rs1 ;
	    op->a = nullptr ;
	    op->ehostname = nullptr ;
	    op->chostname = nullptr ;
	}
	return rs ;
}
/* end subroutine (hostinfo_bufend) */

static int hostinfo_argsbegin(hostinfo *op,uint af,cchar *name) noex {
	int		rs = SR_FAULT ;
	int		f = false ;
	if (name) {
	    rs = SR_INVALID ;
	    if (name[0]) {
	        int	sl = strlen(name) ;
		rs = SR_OK ;
	        op->arg.af = af ;
	        op->arg.hostname = name ;
	        while ((sl > 0) && (name[sl - 1] == '.')) {
	            f = true ;
	            sl -= 1 ;
	        }
	        if (f) {
	            if (cchar *sp{} ; (rs = uc_mallocstrw(name,sl,&sp)) >= 0) {
	                op->arg.f_alloc = true ;
	                op->arg.hostnamelen = sl ;
	                op->arg.hostname = sp ;
	            }
	        }
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? f : rs ;
}
/* end if (hostinfo_argsbegin) */

static int hostinfo_argsend(hostinfo *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (op->arg.f_alloc && op->arg.hostname) {
	    rs1 = uc_free(op->arg.hostname) ;
	    if (rs >= 0) rs = rs1 ;
	}
	op->arg.f_alloc = false ;
	op->arg.hostname = nullptr ;
	return rs ;
}
/* end if (hostinfo_argsend) */

static int hostinfo_domain(hostinfo *op) noex {
	int		rs ;
	int		rs1 ;
	int		len = 0 ;
	char		*domainname ;
	if ((rs = malloc_hn(&domainname)) >= 0) {
	    if (op->domainname == nullptr) {
	        if ((rs = getnodedomain(nullptr,domainname)) >= 0) {
	            cchar	*dp{} ;
	            len = strlen(domainname) ;
	            rs = uc_mallocstrw(domainname,len,&dp) ;
	            if (rs >= 0) op->domainname = dp ;
	        }
	    } else {
	        len = strlen(op->domainname) ;
	    }
	    rs1 = uc_free(domainname) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (hostinfo_domain) */

static int hostinfo_findcanonical(hostinfo *op) noex {
	int		rs = SR_OK ;
	if (op->chostname[0] == '\0') {
	    vecobj	*nlp = op->nlp ;
	    HOSTINFO_N	*nep = nullptr ;
	    cint	hlen = maxhostlen ;
	    cint	rsn = SR_NOTFOUND ;
	    int		si = 0 ;
	    bool	f_continue = true ;
	    while ((rs >= 0) && f_continue) {
	        int	i{} ;
		void	*vp{} ;
	        for (i = si ; (rs = vecobj_get(nlp,i,&vp)) >= 0 ; i += 1) {
	            if (vp) {
			nep = (HOSTINFO_N *) vp ;
	                if (strchr(nep->name,'.') != nullptr) {
	                    rs = sncpy1(op->chostname,hlen,nep->name) ;
	                    break ;
	                }
	            }
	        } /* end for */
	        si = i ;
	        if (rs == rsn) {
	            rs = SR_OK ;
	            if ((! op->fl.inet4) || (! op->fl.inet6)) {
	                if ((rs == 0) && (! op->fl.inet4)) {
	                    rs = getinet(op,af4) ;
	                }
	                if ((rs == 0) && (! op->fl.inet6)) {
	                    rs = getinet(op,af6) ;
	                }
	                f_continue = (rs > 0) ;
	            } else {
	                f_continue = false ;
	            }
	        } else if (rs == 0) {
	            f_continue = false ;
	            rs = 1 ;
	        } else if (rs > 0) {
	            f_continue = false ;
	        } /* end if */
	    } /* end while */
	    if ((rs == 0) && (matknown(op->ehostname,-1) >= 0)) {
	        rs = vecobj_count(op->alp) ;
	    }
	    if (rs > 0) { /* found */
	        rs = 0 ;
	        if (strchr(op->ehostname,'.') == nullptr) {
	            if ((rs = hostinfo_domain(op)) >= 0) {
	                cchar	*eh = op->ehostname ;
	                cchar	*dn = op->domainname ;
	                rs = snsds(op->chostname,hlen,eh,dn) ;
	            }
	        }
	        if (rs == 0) {
	            rs = sncpy1(op->chostname,hlen,op->ehostname) ;
	        }
	    } /* end if (desperate) */
	} else {
	    rs = strlen(op->chostname) ;
	} /* end if (needed) */
	return rs ;
}
/* end subroutine (hostinfo_findcanonical) */

static int hostinfo_getname(hostinfo *op,int af,cchar *name) noex {
	int		rs ;
	int		rs1 ;
	int		c = 0 ;
	char		*hebuf{} ;
	bool		f_inet4 = false ;
	if_constexpr (f_hostbyname) {
	    f_inet4 = (af == af4) ;
	}
        if ((rs = malloc_ho(&hebuf)) >= 0) {
            HOSTENT         *hep{} ;
            ucentho         he ;
            cint            helen = rs ;
            if (f_inet4) {
                hep = &he ;
                rs = getho_name(&he,hebuf,helen,name) ;
            } else {
                cint	flags = 0 ;
                rs = uc_getipnodebyname(&hep,name,af,flags) ;
            } /* end if */
            if (rs >= 0) {
                if ((rs = hostinfo_loadnames(op,af,hep)) >= 0) {
                    c = rs ;
                    rs = hostinfo_loadaddrs(op,af,hep) ;
                }
                if ((rs >= 0) && (op->ehostname[0] == '\0')) {
                    int     sl ;
                    cchar   *sp ;
                    if ((rs = hostent_getofficial(hep,&sp)) >= 0) {
                        cint        hlen = maxhostlen ;
                        sl = rs ;
                        rs = snwcpy(op->ehostname,hlen,sp,sl) ;
                    }
                }
                if ((! f_inet4) && hep) {
                    uc_hostentfree(hep) ;
                }
            } else if (isNotPresent(rs)) {
                rs = SR_OK ;
            } /* end if (got host-entry) */
            rs1 = uc_free(hebuf) ;
            if (rs >= 0) rs = rs1 ;
        } /* end if (m-a-f) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (hostinfo_getname) */

static int hostinfo_getaddr(hostinfo *op,int af) noex {
	int		rs = SR_NOTFOUND ;
	int		rs1 ;
	int		c = 0 ;
	bool		f_inet4 = false ;
	if_constexpr (f_hostbyname) {
	    f_inet4 = (af == af4) ;
	}
	if (op->fl.addr && (af == op->addr.af)) {
	    char	*hebuf{} ;
	    if ((rs = malloc_ho(&hebuf)) >= 0) {
	        HOSTENT		*hep{} ;
	        ucentho		he ;
	        cint		helen = rs ;
	        cchar		*name = op->arg.hostname ;
	        if (f_inet4) {
	            cint	alen = op->addr.addrlen ;
	            cint	at = op->addr.af ;
	            cchar	*abuf = op->addr.addr ;
	            hep = &he ;
	            rs = getho_addr(&he,hebuf,helen,at,abuf,alen) ;
	        } else {
	            cint	flags = 0 ;
	            rs = uc_getipnodebyname(&hep,name,af,flags) ;
	        } /* end if */
	        if (rs >= 0) {
	            if ((rs = hostinfo_loadnames(op,af,hep)) >= 0) {
	                c = rs ;
	                rs = hostinfo_loadaddrs(op,af,hep) ;
	            }
	            if ((rs >= 0) && (op->ehostname[0] == '\0')) {
	                int	sl ;
	                cchar	*sp ;
	                if ((rs = hostent_getofficial(hep,&sp)) >= 0) {
			    cint	hlen = maxhostlen ;
	                    sl = rs ;
	                    rs = snwcpy(op->ehostname,hlen,sp,sl) ;
	                }
	            }
	            if ((! f_inet4) && hep) {
	                uc_hostentfree(hep) ;
	            }
	        } /* end if (got host-entry) */
	        rs1 = uc_free(hebuf) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (m-a-f) */
	} /* end if (enabled) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (hostinfo_getaddr) */

static int hostinfo_loadaddrs(hostinfo *op,int af,HOSTENT *hep) noex {
	int		rs ;
	int		rs1 ;
	int		c = 0 ;
	if ((rs = hostent_getalen(hep)) >= 0) {
	    HOSTENT_CUR	hc ;
	    HOSTINFO_A	a{} ;
	    cint	nrs = SR_NOTFOUND ;
	    cint	alen = rs ;
	    a.af = af ;
	    a.addrlen = alen ;
	    if ((rs = hostent_curbegin(hep,&hc)) >= 0) {
	        vecobj		*alp = op->alp ;
	        const uchar	*ap ;
		cnullptr	np{} ;
	        while ((rs = hostent_curenumaddr(hep,&hc,&ap)) > 0) {
	            a.addrlen = rs ;
	            memcpy(&a.addr,ap,rs) ;
	            if ((rs = vecobj_search(alp,&a,vmataddr,np)) == nrs) {
	                c += 1 ;
	                a.af = af ;
	                rs = vecobj_add(alp,&a) ;
	            } /* end if (entry not found) */
	            if (rs < 0) break ;
	        } /* end while */
	        rs1 = hostent_curend(hep,&hc) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (hostent) */
	} /* end if */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (hostinfo_loadaddrs) */

static int hostinfo_loadnames(hostinfo *op,int af,HOSTENT *hep) noex {
	HOSTENT_CUR	hc ;
	int		rs = SR_OK ;
	int		rs1 ;
	int		sl ;
	int		c = 0 ;
	cchar		*sp{} ;
/* get the "official" name */
	if (rs >= 0) {
	    if ((rs1 = hostent_getofficial(hep,&sp)) >= 0) {
	        sl = rs1 ;
	        rs = hostinfo_addname(op,sp,sl,af) ;
	        c += rs ;
	    } /* end if */
	} /* end if */
/* get the "canonical" name */
	if (rs >= 0) {
	    if ((rs1 = hostent_getcanonical(hep,&sp)) >= 0) {
	        sl = rs1 ;
	        rs = hostinfo_addname(op,sp,sl,af) ;
	        c += rs ;
	    } /* end if */
	} /* end if */
/* get all of the "alias" name(s) */
	if (rs >= 0) {
	    if ((rs = hostent_curbegin(hep,&hc)) >= 0) {
	        while ((rs = hostent_curenumname(hep,&hc,&sp)) > 0) {
	            rs = hostinfo_addname(op,sp,rs,af) ;
	            c += rs ;
	            if (rs < 0) break ;
	        } /* end while */
	        rs1 = hostent_curend(hep,&hc) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (hostent) */
	} /* end if (ok) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (hostinfo_loadnames) */

static int hostinfo_addname(hostinfo *op,cchar *sp,int sl,int af) noex {
	int		rs = SR_FAULT ;
	int		c = 0 ;
	if (sp) {
	    rs = SR_OK ;
	    if (sl != 0) {
	        HOSTINFO_N	ne{} ;
	        vecobj		*nlp = op->nlp ;
	        vecobj_vcf	vc = vmatname ;
	        cint		nrs = SR_NOTFOUND ;
	        ne.af = af ;
	        ne.namelen = sl ;
	        ne.name = sp ;
	        if ((rs = vecobj_search(nlp,&ne,vc,nullptr)) == nrs) {
	            cchar	*cp ;
	            if ((rs = uc_mallocstrw(sp,sl,&cp)) >= 0) {
	                ne.name = cp ;
	                c += 1 ;
	                rs = vecobj_add(op->nlp,&ne) ;
	            } /* end if (allocated) */
	        } /* end if (entry not found) */
	    } /* end if (non-zero) */
	} /* end if (non-null) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (hostinfo_addname) */

static int hostinfo_finishnames(hostinfo *op) noex {
	vecobj		*vlp = op->nlp ;
	int		rs = SR_OK ;
	int		rs1 ;
	void		*vp{} ;
	for (int i = 0 ; vecobj_get(vlp,i,&vp) >= 0 ; i += 1) {
	    if (vp) {
		HOSTINFO_N	*nep = (HOSTINFO_N *) vp ;
	        if (nep->name != nullptr) {
	            rs1 = uc_free(nep->name) ;
	            if (rs >= 0) rs = rs1 ;
	            nep->name = nullptr ;
	        }
	    }
	} /* end for */
	return rs ;
}
/* end subroutine (hostinfo_finishnames) */

static int hostinfo_addrbegin(hostinfo *op,int af) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (! op->init.addr) {
	    cint	ilen = INETXADDRLEN ;
	    int		inetaddrlen ;
	    char	inetaddr[INETXADDRLEN + 1] ;
	    op->init.addr = true ;
	    rs1 = inetpton(inetaddr,ilen,af,op->arg.hostname,-1) ;
	    if (rs1 >= 0) {
	        cvoid	*vp ;
	        inetaddrlen = getaflen(rs1) ;
	        if ((rs = uc_mallocbuf(inetaddr,inetaddrlen,&vp)) >= 0) {
	            op->addr.addr = (cchar *) vp ;
	            op->fl.addr = true ;
	            op->addr.af = rs1 ;
	            op->addr.addrlen = inetaddrlen ;
	        } else {
	            op->addr.addr = nullptr ;
	        }
	    } /* end if (allocating space for ADDR) */
	} /* end if */
	return (rs >= 0) ? op->fl.addr : rs ;
}
/* end subroutine (hostinfo_addrbegin) */

static int hostinfo_addrend(hostinfo *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (op->addr.addr != nullptr) {
	    rs1 = uc_free(op->addr.addr) ;
	    if (rs >= 0) rs = rs1 ;
	    op->addr.addr = nullptr ;
	}
	return rs ;
}
/* end subroutine (hostinfo_addrend) */

static int hostinfo_loadknownaddr(hostinfo *op,int af,uint ka) noex {
	HOSTINFO_A	a{} ;
	uint		na = htonl(ka) ;
	cint		nrs = SR_NOTFOUND ;
	cint		addrlen = getaflen(af) ;
	int		rs ;
	int		c = 0 ;
	a.af = af ;
	a.addrlen = addrlen ;
	memcpy(&a.addr,&na,addrlen) ;
	if ((rs = vecobj_search(op->alp,&a,vmataddr,nullptr)) == nrs) {
	    c += 1 ;
	    rs = vecobj_add(op->alp,&a) ;
	} /* end if (entry not found) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (hostinfo_loadknownaddr) */

static int getinet(hostinfo *op,int af) noex {
	int		rs = SR_OK ;
	int		c = 0 ; /* count-of-names */
	if (af > 0) {
	    if ((op->arg.af == af0) || (op->arg.af == af)) {
	        switch (af) {
	        case AF_INET4:
	            op->fl.inet4 = true ; /* mark as tried */
	            break ;
	        case AF_INET6:
	            op->fl.inet6 = true ; /* mark as tried */
	            break ;
	        } /* end switch */
	        for (int i = 0 ; getinets[i] != nullptr ; i += 1) {
	            rs = (*getinets[i])(op,af) ;
	            c = rs ;
	            if (rs != 0) break ;
	        } /* end for */
	    } /* end if */
	} /* end if (have AF) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (getinet) */

static int getinet_straight(hostinfo *op,int af) noex {
	int		rs = SR_OK ;
	int		c = 0 ;
	if (! op->init.addr) {
	    rs = hostinfo_addrbegin(op,af) ;
	}
	if (rs >= 0) {
	    if (op->fl.addr && (af == op->addr.af)) {
	        rs = hostinfo_getaddr(op,af) ;
	    } else {
	        rs = hostinfo_getname(op,af,op->arg.hostname) ;
	    }
	    if ((rs > 0) && (op->ehostname[0] == '\0')) {
	        cint	hlen = maxhostlen ;
	        c = 1 ;
	        rs = sncpy1(op->ehostname,hlen,op->arg.hostname) ;
	    }
	} /* end if (ok) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (getinet_straight) */

/* try adding our own domain on the end if it does not already have one */
static int getinet_add(hostinfo *op,int af) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	int		c = 0 ;
	if (strchr(op->arg.hostname,'.') == nullptr) { /* nodename-only */
	    bool	f_continue = false ;
	    if_constexpr (f_fastaddr) {
	        if ((rs = hostinfo_addrbegin(op,af)) >= 0) {
	            f_continue = (! op->fl.addr) ;
	        }
	    } else {
	        f_continue = (! isinetaddr(op->arg.hostname)) ;
	    } /* end if_constexpr (f_fastaddr) */
	    if ((rs >= 0) && f_continue) {
	        if ((rs = hostinfo_domain(op)) >= 0) {
	            cchar	*hn = op->arg.hostname ;
	            cchar	*dn = op->domainname ;
	    	    char	*hbuf{} ;
		    if ((rs = malloc_hn(&hbuf)) >= 0) {
	    		cint	hlen = rs ;
	                if ((rs = snsds(hbuf,hlen,hn,dn)) >= 0) {
	                    if ((rs = hostinfo_getname(op,af,hbuf)) > 0) {
	                        if (op->ehostname[0] == '\0') {
	                            c = 1 ;
	                            rs = sncpy1(op->ehostname,hlen,hbuf) ;
	                        }
	                    }
	                } /* end if (snsds) */
			rs1 = uc_free(hbuf) ;
			if (rs >= 0) rs = rs1 ;
		    } /* end if (m-a-f) */
	        } /* end if (hostinfo_domain) */
	    } /* end if (continue) */
	} /* end if (possible address) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (getinet_add) */

/* try removing our own domain from the end if it is the same as we */
static int getinet_rem(hostinfo *op,int af) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	int		c = 0 ;
	bool		f_continue = false ;
	if_constexpr (f_fastaddr) {
	    if ((rs = hostinfo_addrbegin(op,af)) >= 0) {
	        f_continue = (! op->fl.addr) ;
	    }
	} else {
	    f_continue = (! isinetaddr(op->arg.hostname)) ;
	} /* end if_constexpr (f_fastaddr) */
	if ((rs >= 0) && f_continue) {
	    cnullptr	np{} ;
	    if (cchar *tp ; (tp = strchr(op->arg.hostname,'.')) != np) {
	        if ((rs = hostinfo_domain(op)) >= 0) {
	            if (isindomain(op->arg.hostname,op->domainname)) {
	                cint	cl = (tp - op->arg.hostname) ;
	                cchar	*cp = op->arg.hostname ;
	                char	*hbuf{} ;
			if ((rs = malloc_hn(&hbuf)) >= 0) {
			    cint	hlen = rs ;
	                    if ((rs = snwcpy(hbuf,hlen,cp,cl)) >= 0) {
	                        if ((rs = hostinfo_getname(op,af,hbuf)) > 0) {
	                            if (op->ehostname[0] == '\0') {
				        c = 1 ;
	                                rs = sncpy1(op->ehostname,hlen,hbuf) ;
	                            }
	                        }
	                    } /* end if (snwcpy) */
			    rs1 = uc_free(hbuf) ;
			    if (rs >= 0) rs = rs1 ;
			} /* end if (m-a-f) */
	            } /* end if (the requested hostname is in our domain) */
	        } /* end if (hostinfo_domain) */
	    } /* end if (possible something) */
	} /* end if (continue) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (getinet_rem) */

/* try removing a "LOCAL" domain from the end */
static int getinet_remlocal(hostinfo *op,int af) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	int		c = 0 ;
	bool		f_continue = false ;
	if_constexpr (f_fastaddr) {
	    if ((rs = hostinfo_addrbegin(op,af)) >= 0) {
	        f_continue = (! op->fl.addr) ;
	    }
	} else {
	    f_continue = (! isinetaddr(op->arg.hostname)) ;
	} /* end if_constexpr (f_fastaddr) */
	if ((rs >= 0) && f_continue) {
	    cnullptr	np{} ;
	    if (cchar *tp ; (tp = strchr(op->arg.hostname,'.')) != np) {
	        if (isindomain(op->arg.hostname,LOCALDOMAINNAME)) {
	            int		hl = (tp - op->arg.hostname) ;
		    cchar	*hn = op->arg.hostname ;
	            char	*hbuf{} ;
		    if ((rs = malloc_hn(&hbuf)) >= 0) {
			cint	hlen = rs ;
	                if ((rs = snwcpy(hbuf,hlen,hn,hl)) >= 0) {
	                    if ((rs = hostinfo_getname(op,af,hbuf)) > 0) {
	                        if (op->ehostname[0] == '\0') {
	                            c = 1 ;
	                            rs = sncpy1(op->ehostname,hlen,hbuf) ;
	                        }
	                    }
	                } /* end if (snwcpy) */
			rs1 = uc_free(hbuf) ;
			if (rs >= 0) rs = rs1 ;
		    } /* end if (m-a-f) */
	        } /* end if (the requested hostname is in our domain) */
	    } /* end if */
	} /* end if (continue) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (getinet_remlocal) */

static int getinet_known(hostinfo *op,int af) noex {
	int		rs = SR_OK ;
	int		c = 0 ;
	bool		f_continue = false ;
	if_constexpr (f_fastaddr) {
	    if ((rs = hostinfo_addrbegin(op,af)) >= 0) {
	        f_continue = (! op->fl.addr) ;
	    }
	} else {
	    f_continue = (! isinetaddr(op->arg.hostname)) ;
	} /* end if_constexpr (f_fastaddr) */
	if ((rs >= 0) && f_continue) {
	    if ((af == af0) || (af == af4)) {
	        if ((rs = hostinfo_domain(op)) >= 0) {
		    rs = getinet_knowner(op,af) ;
		    c = rs ;
	        } /* end if (hostinfo_domain) */
	    } /* end if (address space ours) */
	} /* end if (continue) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (getinet_known) */

static int getinet_knowner(hostinfo *op,int af) noex {
	int		rs ;
	int		rs1 ;
	int		c = 0 ;
	int		i = -1 ;
	cchar		*local = LOCALDOMAINNAME ;
	cchar		*sp = op->arg.hostname ;
	char		*hbuf{} ;
	if ((rs = malloc_hn(&hbuf)) >= 0) {
	    cnullptr	np{} ;
            cint    	hlen = rs ;
            int     	sl = strlen(sp) ;
            while ((sl > 0) && (sp[sl - 1] == '.')) {
                sl -= 1 ;
            }
            if (cchar *tp ; (tp = strnchr(sp,sl,'.')) != np) {
		cint	mhlen = maxhostlen ;
                cint	cl = ((sp + sl) - (tp + 1)) ;
                cchar	*cp = (tp+1) ;
                bool	f = false ;
                strwcpy(hbuf,sp,min(sl,mhlen)) ;
                f = f || isindomain(hbuf,op->domainname) ;
                f = f || (strncmp(local,cp,cl) == 0) ;
                if (f) {
                    sl = (tp - sp) ;
                    if ((i = matknown(sp,sl)) >= 0) {
                        char        *chn = op->chostname ;
                        char        *ehn = op->ehostname ;
                        if (op->ehostname[0] == '\0') {
                            c = 1 ;
                            rs = snwcpy(ehn,hlen,sp,sl) ;
                        }
                        if ((rs >= 0) && (chn[0] == '\0')) {
                            rs = sncpy1(chn,hlen,hbuf) ;
                        }
                    } /* end if */
                } /* end if (hit) */
            } else {
                if ((i = matknown(sp,sl)) >= 0) {
                    if (op->ehostname[0] == '\0') {
                        c = 1 ;
                        rs = sncpy1(op->ehostname,hlen,sp) ;
                    }
                    if ((rs >= 0) && (op->chostname[0] == '\0')) {
                        cchar       *dn = op->domainname ;
                        rs = snsds(op->chostname,hlen,sp,dn) ;
                    }
                } /* end if */
            } /* end if */
            rs1 = uc_free(hbuf) ;
            if (rs >= 0) rs = rs1 ;
        } /* end if (m-a-f) */
        if ((rs >= 0) && (i >= 0)) {
            rs = hostinfo_loadknownaddr(op,af,knowns[i].a) ;
        } /* end if (loading known address) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (getinet_knowner) */

static int matknown(cchar *name,int nl) noex {
	int		i = 0 ;
	if (nl < 0) nl = strlen(name) ;
	for (i = 0 ; knowns[i].name != nullptr ; i += 1) {
	    cint	m = nleadstr(knowns[i].name,name,nl) ;
	    if ((m > 0) && (knowns[i].name[m] == '\0') && (m == nl)) break ;
	} /* end for */
	return (knowns[i].name != nullptr) ? i : -1 ;
}
/* end subroutine (matknown) */

static int vmatname(cvoid **v1pp,cvoid **v2pp) noex {
	HOSTINFO_N	*ne1p = (HOSTINFO_N *) *v1pp ;
	HOSTINFO_N	*ne2p = (HOSTINFO_N *) *v2pp ;
	bool		f = true ;
	f = f && (ne1p->name[0] == ne2p->name[0]) ;
	f = f && (ne1p->namelen == ne2p->namelen) ;
	f = f && (strcmp(ne1p->name,ne2p->name) == 0) ;
	return (f) ? 0 : 1 ;	/* <- reversed sense */
}
/* end subroutine (vmatname) */

static int vmataddr(cvoid **v1pp,cvoid **v2pp) noex {
	HOSTINFO_A	*ae1p = (HOSTINFO_A *) *v1pp ;
	HOSTINFO_A	*ae2p = (HOSTINFO_A *) *v2pp ;
	bool		f = true ;
	f = f && (ae1p->addrlen == ae2p->addrlen) ;
	if (f) {
	    cint	af1 = ae1p->af ;
	    cint	af2 = ae2p->af ;
	    f = ((af1 > 0) && (af2 > 0) && (ae1p->af == ae2p->af)) ;
	}
	f = f && (memcmp(ae1p->addr,ae2p->addr,ae1p->addrlen) == 0) ;
	return (f) ? 0 : 1 ;	/* <- reversed sense */
}
/* end subroutine (vmataddr) */

#if	defined(COMMENT) && defined(CF_DEBUGS) && (CF_DEBUGS > 0)
static int debugprintaliases(cchar *s,HOSTENT *hep) noex {
	int		i = 0 ;
	debugprintf("%s: aliases>\n",s) ;
	if (hep->h_aliases != nullptr) {
	    for (i = 0 ; hep->h_aliases[i] != nullptr ; i += 1)
	        debugprintf("%s: alias[%u]=>%s<\n",
	            s,i,hep->h_aliases[i]) ;
	}
	return i ;
}
static int debugprintinetaddr(cchar *s,int af,cvoid *binaddr) noex {
	cint		slen = INETX_ADDRSTRLEN ;
	int		rs1 ;
	char		sbuf[INETX_ADDRSTRLEN + 1] ;
	rs1 = inetntop(sbuf,slen,af,binaddr) ;
	if (rs1 < 0) strcpy(sbuf,"BAD") ;
	debugprintf("%s af=%d addr=%s\n",s,af,sbuf) ;
	return rs1 ;
}
#endif /* CF_DEBUGS */


