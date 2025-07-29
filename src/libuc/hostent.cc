/* hostent SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* manipulate host entry structures */
/* version %I% last-modified %G% */


/* revision history:

	= 1999-02-03, David A­D­ Morano
	This module was originally written.

*/

/* Copyright © 1999 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Object:
	hostent

	Description:
	These routines provide means to peruse the elements of a
	HOSTENT structure as returned by the system subroutines:

		gethostbyname
		gethostbyaddr

	and their kin.

	All, INET addresses (when in any sort of binary form) are
	in network byte order!  This is true of the above subroutines
	as well.

	Below is the structure that represents the object itself:

	struct	hostent {
		char	*h_name;
		char	**h_aliases;
		int	h_addrtype;
		int	h_length;
		char	**h_addr_list;
	#define	h_addr	h_addr_list[0]	
	} ;

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/param.h>
#include	<sys/socket.h>
#include	<netinet/in.h>
#include	<arpa/inet.h>
#include	<netdb.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<usystem.h>
#include	<usupport.h>
#include	<storeitem.h>
#include	<intceil.h>
#include	<localmisc.h>

#include	"hostent.h"

import libutil ;

/* local defines */

#define	HE		hostent

#define	CHE		chostent

#define	SI		storeitem


/* local namespaces */


/* local typedefs */

typedef const hostent	chostent ;


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

static int	si_copyaliases(SI *,HE *,CHE *) noex ;
static int	si_copyaddrs(SI *,HE *,CHE *) noex ;
static int	si_copystr(SI *,char **,cchar *) noex ;
static int	si_copybuf(SI *,char **,cchar *,int) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int hostent_getofficial(HE *hep,cchar **rpp) noex {
	int		rs = SR_FAULT ;
	int		nlen = 0 ;
	if (hep) ylikely {
	    rs = SR_NOTFOUND ;
	    if (hep->h_name) ylikely {
	        rs = SR_OK ;
	        nlen = int(lenstr(hep->h_name)) ;
	        if (rpp) {
	            *rpp = hep->h_name ;
	        }
	    } /* end if */
	    if ((rs < 0) && rpp) {
	        *rpp = nullptr ;
	    }
	} /* end if (non-null) */
	return (rs >= 0) ? nlen : rs ;
}
/* end subroutine (hostent_getofficial) */

/* get address family type (assume it is in host byte order) */
int hostent_getaf(HE *hep) noex {
	int		rs = SR_FAULT ;
	if (hep) ylikely {
	    rs = hep->h_addrtype ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (hostent_getaf) */

int hostent_getalen(HE *hep) noex {
	int		rs = SR_FAULT ;
	if (hep) {
	    rs = hep->h_length ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (hostent_getalen) */

int hostent_curbegin(HE *hep,hostent_cur *curp) noex {
	int		rs = SR_FAULT ;
	if (hep && curp) ylikely {
	    curp->i = -1 ;
	    rs = SR_OK ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (hostent_curbegin) */

int hostent_curend(HE *hep,hostent_cur *curp) noex {
	int		rs = SR_FAULT ;
	if (hep && curp) {
	    curp->i = -1 ;
	    rs = SR_OK ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (hostent_curend) */

/* enumerate the next hostname */
int hostent_curenumname(HE *hep,hostent_cur *curp,cchar **rpp) noex {
	int		rs = SR_FAULT ;
	int		nlen = 0 ;
	if (hep && curp) ylikely {
	    rs = SR_OK ;
	    if (curp == nullptr) {
	        if (hep->h_name) {
	            nlen = int(lenstr(hep->h_name)) ;
	            if (rpp) {
	                *rpp = hep->h_name ;
		    }
	        }
	    } else {
	        if (curp->i < 0) {
	            if (hep->h_name) {
	                nlen = int(lenstr(hep->h_name)) ;
	                if (rpp) {
	                    *rpp = hep->h_name ;
		        }
	                curp->i = 0 ;
	            }
	        } else {
	            if (hep->h_aliases && hep->h_aliases[curp->i]) {
	                nlen = int(lenstr(hep->h_aliases[curp->i])) ;
	                if (rpp) {
	                    *rpp = hep->h_aliases[curp->i] ;
		        }
	                curp->i += 1 ;
	            }
	        } /* end if */
	    } /* end if */
	    if ((rs < 0) && rpp) {
	        *rpp = nullptr ;
	    }
	} /* end if (non-null) */
	return (rs >= 0) ? nlen : rs ;
}
/* end subroutine (hostent_curenumname) */

/* enumerate the next host address */
int hostent_curenumaddr(HE *hep,hostent_cur *curp,cuchar **rpp) noex {
	int		rs = SR_FAULT ;
	int		alen = 0 ;
	if (hep && curp) ylikely {
	    rs = SR_OK ;
	    alen = hep->h_length ;
	    if (hep->h_addr_list != nullptr) {
	        if (curp == nullptr) {
	            if (hep->h_addr_list[0] != nullptr) {
	                if (rpp) {
	                    *rpp = (uchar *) hep->h_addr_list[0] ;
		        }
	            }
	        } else {
	            cint	ci = (curp->i >= 0) ? (curp->i + 1) : 0 ;
		    if (hep->h_addr_list[ci] != nullptr) {
	                if (rpp) {
	                    *rpp = (uchar *) hep->h_addr_list[ci] ;
		        }
	            }
	            if (rs >= 0) {
		        curp->i = ci ;
		    }
	        } /* end if */
	    } /* end if */
	    if ((rs < 0) && rpp) {
	        *rpp = nullptr ;
	    }
	} /* end if (non-null) */
	return (rs >= 0) ? alen : rs ;
}
/* end subroutine (hostent_curenumaddr) */

int hostent_getcanonical(HE *hep,cchar **rpp) noex {
	int		rs = SR_FAULT ;
	int		nlen = 0 ;
	if (hep && rpp) ylikely {
	    rs = SR_NOTFOUND ;
	    if (hep->h_name && (strchr(hep->h_name,'.') != nullptr)) {
	        rs = SR_OK ;
	        nlen = int(lenstr(hep->h_name)) ;
	        *rpp = hep->h_name ;
	    } /* end if */
	    if ((rs == SR_NOTFOUND) && (hep->h_aliases != nullptr)) {
	        for (int i = 0 ; hep->h_aliases[i] != nullptr ; i += 1) {
	            if (strchr(hep->h_aliases[i],'.') != nullptr) {
		        rs = SR_OK ;
	    	        nlen = int(lenstr(hep->h_aliases[i])) ;
	                *rpp = hep->h_aliases[i] ;
	                break ;
	            } /* end if */
	         } /* end for */
	    } /* end if */
	    if ((rs == SR_NOTFOUND) && (hep->h_name != nullptr)) {
	        rs = SR_OK ;
	        nlen = int(lenstr(hep->h_name)) ;
	        *rpp = hep->h_name ;
	    } /* end if */
	} /* end if (non-null) */
	return (rs >= 0) ? nlen : rs ;
}
/* end subroutine (hostent_getcanonical) */

/* get a fully qualified domain name */
int hostent_getfqdn(HE *hep,cchar **rpp) noex {
	return hostent_getcanonical(hep,rpp) ;
}
/* end subroutine (hostent_getfqdn) */

int hostent_size(HE *hep) noex {
	int		rs = SR_FAULT ;
	int		sz = 1 ;
	if (hep) ylikely {
	    int		i{} ; /* used-afterwards */
	    rs = SR_OK ;
	    if (hep->h_name) {
	        sz += int(lenstr(hep->h_name) + 1) ;
	    }
	    if (hep->h_aliases) {
	        for (i = 0 ; hep->h_aliases[i] != nullptr ; i += 1) {
		    sz += int(lenstr(hep->h_aliases[i]) + 1) ;
	        }
	        sz += (i * szof(cchar *)) ;
	    }
	    if (hep->h_addr_list) {
	        for (i = 0 ; hep->h_addr_list[i] != nullptr ; i += 1) {
		    sz += (hep->h_length+1) ;
	        }
	    }
	    sz = iceil(sz,szof(cchar *)) ;
	} /* end if (non-null) */
	return (rs >= 0) ? sz : rs ;
}
/* end subroutine (hostent_size) */

int hostent_load(HE *hep,char *hebuf,int helen,HOSTENT *lp) noex {
	int		rs = SR_FAULT ;
	int		len = 0 ;
	if (hep && hebuf && lp) ylikely {
	    memcpy(hep,lp) ;
	    if (storeitem ib ; (rs = ib.start(hebuf,helen)) >= 0) ylikely {
	        if (rs >= 0) rs = si_copyaliases(&ib,hep,lp) ;
	        if (rs >= 0) rs = si_copyaddrs(&ib,hep,lp) ;
	        if (rs >= 0) rs = si_copystr(&ib,&hep->h_name,lp->h_name) ;
	        len = ib.finish ;
	        if (rs >= 0) rs = len ;
	    } /* end if (storeitem) */
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (hostent_load) */


/* private subroutines */

static int si_copyaliases(SI *ibp,HE *hep,CHE *lp) noex {
	int		rs = SR_OK ;
	if (lp->h_aliases != nullptr) ylikely {
	    int		n{} ;
	    for (n = 0 ; lp->h_aliases[n] != nullptr ; n += 1) ;
	    if (void **vpp{} ; (rs = ibp->ptab(n,&vpp)) >= 0) {
	        int	i{} ; /* used-afterwards */
		hep->h_aliases = charpp(vpp) ;
		for (i = 0 ; lp->h_aliases[i] ; i += 1) {
		    cchar	*ap = lp->h_aliases[i] ;
		    rs = si_copystr(ibp,(hep->h_aliases + i),ap) ;
		    if (rs < 0) break ;
		} /* end for */
		hep->h_aliases[i] = nullptr ;
	    } /* end if (storeitem_ptab) */
	} else {
	    hep->h_aliases = nullptr ;
	}
	return rs ;
}
/* end subroutine (si_copyaliases) */

static int si_copyaddrs(SI *ibp,HE *hep,CHE *lp) noex {
	int		rs = SR_OK ;
	if (lp->h_addr_list != nullptr) ylikely {
	    int		n{} ; /* used-afterwards */
	    for (n = 0 ; lp->h_addr_list[n] != nullptr ; n += 1) ;
	    if (void **vpp{} ; (rs = ibp->ptab(n,&vpp)) >= 0) {
	        int	i{} ; /* used-afterwards */
	        hep->h_addr_list = charpp(vpp) ;
	        for (i = 0 ; lp->h_addr_list[i] ; i += 1) {
		    cchar	*ap = lp->h_addr_list[i] ;
		    cint	al = lp->h_length ;
		    rs = si_copybuf(ibp,(hep->h_addr_list + i),ap,al) ;
		    if (rs < 0) break ;
	        } /* end for */
		hep->h_addr_list[i] = nullptr ;
	    } /* end if */
	} else {
	    hep->h_addr_list = nullptr ;
	}
	return rs ;
}
/* end subroutine (si_copyaddrs) */

static int si_copystr(SI *ibp,char **pp,cchar *s1) noex {
	int		rs = SR_OK ;
	cchar		**cpp = ccharpp(pp) ;
	*cpp = nullptr ;
	if (s1) ylikely {
	    rs = ibp->strw(s1,-1,cpp) ;
	}
	return rs ;
}
/* end subroutine (si_copystr) */

static int si_copybuf(SI *ibp,char **pp,cchar *bp,int bl) noex {
	int		rs = SR_OK ;
	cchar		**cpp = ccharpp(pp) ;
	*cpp = nullptr ;
	if (bp) ylikely {
	    rs = ibp->buf(bp,bl,cpp) ;
	}
	return rs ;
}
/* end subroutine (si_copybuf) */

#ifdef	COMMENT
using typename storeitem {
    	int	copybuf(char *pp,cchar *bp,int bl) noex ;
} ;
#endif /* COMMENT */


