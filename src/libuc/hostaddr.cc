/* hostaddr SUPPORT */
/* lang=C++20 */

/* manipulate host entry structures */
/* version %I% last-modified %G% */


/* revision history:

	= 2005-02-03, David A­D­ Morano
	This module was originally written.

*/

/* Copyright © 2005 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This object provides the capability to peruse the information
	returned by the subroutines:
		geteaddrinfo(3dam)
	which itself is an enhanved version of the system-provided:
		getaddrinfo(3socket)

	Additional information:

 * addrinfo introduced with IPv6 for Protocol-Independent Hostname
 * and Service Name Translation.

ADDRINFO {
	int ai_flags;		 	AI_PASSIVE, AI_CANONNAME 
	int ai_family;		 	PF_xxx 
	int ai_socktype;	 	SOCK_xxx 
	int ai_protocol;	 	0 or IPPROTO_xxx for IPv4 and IPv6 
	size_t ai_addrlen;		length of ai_addr 
	char *ai_canonname;	 	canonical name for hostname 
	struct sockaddr *ai_addr;	binary address 
	ADDRINFO *ai_next;	next structure in linked list 
};

 addrinfo flags 
#define	AI_PASSIVE	0x0008	 intended for bind() + listen() 
#define	AI_CANONNAME	0x0010	 return canonical version of host 
#define	AI_NUMERICHOST	0x0020	 use numeric node address string 

 getipnodebyname() flags 
#define	AI_V4MAPPED	0x0001	 IPv4 mapped addresses if no IPv6 
#define	AI_ALL		0x0002	 IPv6 and IPv4 mapped addresses 
#define	AI_ADDRCONFIG	0x0004	 AAAA or A records only if IPv6/IPv4 cnfg'd 
#define	AI_DEFAULT	(AI_V4MAPPED | AI_ADDRCONFIG)

 addrinfo errors 
#define	EAI_ADDRFAMILY	1	 address family not supported 
#define	EAI_AGAIN	2	 DNS temporary failure 
#define	EAI_BADFLAGS	3	 invalid ai_flags 
#define	EAI_FAIL	4	 DNS non-recoverable failure 
#define	EAI_FAMILY	5	 ai_family not supported 
#define	EAI_MEMORY	6	 memory allocation failure 
#define	EAI_NODATA	7	 no address 
#define	EAI_NONAME	8	 host/servname not known 
#define	EAI_SERVICE	9	 servname not supported for ai_socktype 
#define	EAI_SOCKTYPE	10	 ai_socktype not supported 
#define	EAI_SYSTEM	11	 system error in ERRNO

 getnameinfo max sizes as defined in spec 

#define	NI_MAXHOST	1025
#define	NI_MAXSERV	32

 getnameinfo flags 

#define	NI_NOFQDN	0x0001
#define	NI_NUMERICHOST	0x0002	 return numeric form of address 
#define	NI_NAMEREQD	0x0004	 request DNS name 
#define	NI_NUMERICSERV	0x0008
#define	NI_DGRAM	0x0010

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/socket.h>
#include	<netinet/in.h>
#include	<arpa/inet.h>
#include	<cstdlib>
#include	<cstring>
#include	<netdb.h>
#include	<usystem.h>
#include	<usupport.h>
#include	<mallocxx.h>
#include	<localmisc.h>

#include	"hostaddr.h"


/* local defines */

#define	AI	ADDRINFO


/* local namespaces */


/* local typedefs */


/* external subroutines */

extern "C" {
    extern int geteaddrinfo(cchar *,cchar *,AI *,char *,AI **) noex ;
}


/* external variables */


/* local structures */


/* forward references */

template<typename ... Args>
static inline int hostaddr_ctor(hostaddr *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = SR_OK ;
	    op->magic = 0 ;
	    op->aip = nullptr ;
	    op->resarr = nullptr ;
	    op->ehostname = nullptr ;
	    op->n = 0 ;
	}
	return rs ;
}

static inline int hostaddr_dtor(hostaddr *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	}
	return rs ;
}

template<typename ... Args>
static inline int hostaddr_magic(hostaddr *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magic == HOSTADDR_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}

static int	hostaddr_resultbegin(HOSTADDR *) noex ;
static int	hostaddr_resultend(HOSTADDR *) noex ;

extern "C" {
    static int	vcmp(cvoid *,cvoid *) noex ;
}


/* local variables */


/* exported variables */


/* exported subroutines */

int hostaddr_start(HOSTADDR *op,cchar *hn,cchar *svc,ADDRINFO *hintp) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = hostaddr_ctor(op,hn,svc)) >= 0) {
	    char	*ehostname{} ;
	    if ((rs = malloc_hn(&ehostname)) >= 0) {
	        ADDRINFO	*aip{} ;
	        if ((rs = geteaddrinfo(hn,svc,hintp,ehostname,&aip)) >= 0) {
	            op->aip = aip ;
	            if ((rs = hostaddr_resultbegin(op)) >= 0) {
	                cchar	*cp{} ;
	                if ((rs = uc_mallocstrw(ehostname,-1,&cp)) >= 0) {
		            op->ehostname = cp ;
		            op->magic = HOSTADDR_MAGIC ;
	                }
		        if (rs < 0) {
		            hostaddr_resultend(op) ;
			}
	            } /* end if (hostaddr_sort) */
	            if (rs < 0) {
	                uc_addrinfofree(op->aip) ;
		        op->aip = nullptr ;
	            }
	        } /* end if (geteaddrinfo) */
		rs1 = uc_free(ehostname) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (m-a-f) */
	    if (rs < 0) {
		hostaddr_dtor(op) ;
	    }
	} /* end if (hostaddr_ctor) */
	return rs ;
}
/* end subroutine (hostaddr_start) */

int hostaddr_finish(HOSTADDR *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = hostaddr_magic(op)) >= 0) {
	    if (op->ehostname) {
	        rs1 = uc_free(op->ehostname) ;
	        if (rs >= 0) rs = rs1 ;
	        op->ehostname = nullptr ;
	   }
	   {
	        rs1 = hostaddr_resultend(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    if (op->aip) {
	        rs1 = uc_addrinfofree(op->aip) ;
	        if (rs >= 0) rs = rs1 ;
	        op->aip = nullptr ;
	    }
	    {
	        rs1 = hostaddr_dtor(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    op->magic = 0 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (hostaddr_finish) */

int hostaddr_getcanonical(HOSTADDR *op,cchar **rpp) noex {
	int		rs ;
	if ((rs = hostaddr_magic(op,rpp)) >= 0) {
	    ADDRINFO	*aip = op->aip ;
	    *rpp = aip->ai_canonname ;
	    rs = strlen(aip->ai_canonname) ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (hostaddr_cannonical) */

int hostaddr_curbegin(HOSTADDR *op,HOSTADDR_CUR *curp) noex {
	int		rs ;
	if ((rs = hostaddr_magic(op,curp)) >= 0) {
	        curp->i = -1 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (hostaddr_curbegin) */

int hostaddr_curend(HOSTADDR *op,HOSTADDR_CUR *curp) noex {
	int		rs ;
	if ((rs = hostaddr_magic(op,curp)) >= 0) {
	    curp->i = -1 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (hostaddr_curend) */

int hostaddr_enum(HOSTADDR *op,HOSTADDR_CUR *curp,ADDRINFO **rpp) noex {
	int		rs ;
	int		i = 0 ;
	if ((rs = hostaddr_magic(op,curp)) >= 0) {
	    i = (curp->i >= 0) ? (curp->i+1) : 0 ;
	    if (i < op->n) {
	        if (rpp) *rpp = op->resarr[i] ;
	        curp->i = i ;
	    } else {
	        if (rpp) *rpp = nullptr ;
	        rs = SR_NOTFOUND ;
	    }
	} /* end if (magic) */
	return (rs >= 0) ? i : rs ;
}
/* end subroutine (hostaddr_enum) */


/* private subroutines */

static int hostaddr_resultbegin(HOSTADDR *op) noex {
	ADDRINFO	*aip = op->aip ;
	ADDRINFO	**resarr{} ;
	cint		esize = sizeof(ADDRINFO) ;
	int		rs ;
	int		n = 0 ;
	int		size ;
	n += 1 ;
	while (aip->ai_next != nullptr) {
	    aip = aip->ai_next ;
	    n += 1 ;
	} /* end while */
	size = ((n+1) * esize) ;
	if ((rs = uc_malloc(size,&resarr)) >= 0) {
	    int		i = 0 ;
	    op->resarr = resarr ;
	    op->n = n ;
	    aip = op->aip ;
	    resarr[i++] = aip ;
	    while (aip->ai_next != nullptr) {
		aip = aip->ai_next ;
		resarr[i++] = aip ;
	    } /* end while */
	    resarr[i] = nullptr ;
	    if (n > 1) {
		cint	ssize = sizeof(void *) ;
	        qsort(resarr,n,ssize,vcmp) ;
	    }
	} /* end if (m-a) */
	return (rs >= 0) ? n : rs ;
}
/* end subroutine (hostaddr_resultbegin) */

static int hostaddr_resultend(HOSTADDR *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (op->resarr) {
	    rs1 = uc_free(op->resarr) ;
	    if (rs >= 0) rs = rs1 ;
	    op->resarr = nullptr ;
	    op->n = 0 ;
	}
	return rs ;
}
/* end subroutine (hostaddr_resultend) */

static int vcmp(cvoid *v1pp,cvoid *v2pp) noex {
	ADDRINFO	**a1pp = (ADDRINFO **) v1pp ;
	ADDRINFO	**a2pp = (ADDRINFO **) v2pp ;
	int		rc = 0 ;
	if (v1pp || v2pp) {
	    rc = 1 ;
	    if (v1pp) {
		rc = -1 ;
	        if (v2pp) {
		    ADDRINFO	*a1p = (ADDRINFO *) *a1pp ;
		    ADDRINFO	*a2p = (ADDRINFO *) *a2pp ;
	            rc = (a1p->ai_family - a2p->ai_family) ;
	        }
	    }
	}
	return rc ;
}
/* end subroutine (vcmp) */


