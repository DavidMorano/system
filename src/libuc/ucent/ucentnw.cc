/* ucentnw SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* UCENTNW object management */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-23, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Group:
	uc_entnw{x}

	Description:
	I provide the normal (usual) subroutines for managing the
	UCENTNW (UNIX® |NETWENT| database entries) object.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<arpa/inet.h>		/* <- |htonl(3c)| */
#include	<cerrno>
#include	<unistd.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstdint>
#include	<cstring>		/* <- |memcpy(3c)| */
#include	<algorithm>
#include	<usystem.h>
#include	<storeitem.h>
#include	<sbuf.h>
#include	<vechand.h>
#include	<strn.h>
#include	<sfx.h>
#include	<six.h>
#include	<intceil.h>
#include	<cfdec.h>
#include	<inetconv.h>
#include	<nulstr.h>
#include	<localmisc.h>

#include	"ucentnw.h"
#include	"ucentxx.hh"


/* local defines */

#define	NWE	ucentnw
#define	CNWE	const ucentnw
#define	SI	storeitem

#ifndef INET4ADDRLEN
#define INET4ADDRLEN            szof(in_addr_t)
#endif

#ifndef INET6ADDRLEN
#define INET6ADDRLEN            szof(in6_addr_t)
#endif

#ifndef INET4_ADDRSTRLEN
#define INET4_ADDRSTRLEN        16
#endif

#ifndef INET6_ADDRSTRLEN
#define INET6_ADDRSTRLEN        46      /* Solaris® says this is 46! */
#endif

#define ASTRLEN                 MAX(INET4_ADDRSTRLEN,INET6_ADDRSTRLEN)


/* imported namespaces */

using std::nullptr_t ;			/* type */
using std::min ;			/* subroutine-template */
using std::max ;			/* subroutine-template */
using ucent::si_loadnames ;		/* local group support subroutine */
using ucent::si_loadname ;		/* local group support subroutine */
using ucent::si_copystr ;		/* local group support subroutine */
using std::nothrow ;			/* constant */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

static int ucentnw_parseaddr(NWE *,cchar *,int) noex ;
static int ucentnw_parsestrs(NWE *,SI *,cchar *,int) noex ;

static int ucentnw_formataddr(NWE *,sbuf *) noex ;


/* local variables */

constexpr int		inet4addrlen = int(INET4ADDRLEN) ;
constexpr int		inet6addrlen = int(INET6ADDRLEN) ;
constexpr int		inetxaddrlen = max(inet4addrlen,inet6addrlen) ;
constexpr int		astrlen = int(ASTRLEN) ;


/* exported variables */


/* exported subroutines */

int ucentnw::parse(char *ebuf,int elen,cchar *sp,int sl) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (this && ebuf && sp) {
	    if (sl < 0) sl = strlen(sp) ;
	    memclear(this) ;		/* potentially dangerous */
	    n_addrtype = AF_INET4 ;		/* <- mandatory */
	    if (storeitem si ; (rs = si.start(ebuf,elen)) >= 0) {
	        cchar	*cp{} ;
		if (int idx ; (idx = sichr(sp,sl,'#')) >= 0) {
		    sl = idx ;
		}
	        for (int cl, fi = 0 ; (cl = sfnext(sp,sl,&cp)) > 0 ; ) {
	            cchar	**vpp = nullptr ;
	            switch (fi++) {
	            case 0:
	                vpp = ccharpp(&n_name) ;
	                break ;
	            case 1:
	                rs = ucentnw_parseaddr(this,cp,cl) ;
	                break ;
	            case 2:
	                rs = ucentnw_parsestrs(this,&si,cp,cl) ;
	                break ;
	            } /* end switch */
		    if ((rs >= 0) && vpp) {
	        	rs = si.strw(cp,cl,vpp) ;
		    }
	            sl -= ((cp+cl)-sp) ;
	            sp = (cp+cl) ;
	            if (rs < 0) break ;
	        } /* end while */
	        rs1 = si.finish ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (storeitem) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (ucentnw::parse) */

int ucentnw::load(char *rbuf,int rlen,const ucentnw *cprp) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (this && rbuf && cprp) {
	    memcpy(this,cprp) ;
	    if (storeitem si ; (rs = si.start(rbuf,rlen)) >= 0) {
	        if (cprp->n_aliases) {
	            int		n ; /* used-afterwards */
	            for (n = 0 ; cprp->n_aliases[n] ; n += 1) ;
	            if (void **tab{} ; (rs = si.ptab(n,&tab)) >= 0) {
		        cchar	**aliases = ccharpp(cprp->n_aliases) ;
		        int	i ; /* used-afterwards */
	                n_aliases = charpp(tab) ;
	                for (i = 0 ; cprp->n_aliases[i] ; i += 1) {
			    cchar	*an = aliases[i] ;
	                    rs = si_copystr(&si,(n_aliases + i),an) ;
	                    if (rs < 0) break ;
	                } /* end for */
	                n_aliases[i] = nullptr ;
	            } /* end if (storeitem-ptab) */
	        } else {
		    n_aliases = nullptr ;
	        } /* end if (aliases) */
		if (rs >= 0) {
		    rs = si_copystr(&si,&n_name,cprp->n_name) ;
		}
	        rs1 = si.finish ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (storeitem) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (ucentnw::load) */

int ucentnw::format(char *rbuf,int rlen) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (this && rbuf) {
	    rs = SR_AFNOSUPPORT ;
	    if (n_addrtype == AF_INET4) {
	        if (sbuf b ; (rs = b.start(rbuf,rlen)) >= 0) {
	            for (int i = 0 ; i < 3 ; i += 1) {
	                if (i > 0) rs = b.chr(' ') ;
	                if (rs >= 0) {
	                    switch (i) {
	                    case 0:
	                        rs = b.str(n_name) ;
	                        break ;
	                    case 1:
			        rs = ucentnw_formataddr(this,&b) ;
	                        break ;
	                    case 2:
			        if (n_aliases) {
				    cchar	**sv = ccharpp(n_aliases) ;
	                            rs = b.strs(' ',sv) ;
			        }
	                        break ;
	                    } /* end switch */
	                } /* end if */
	                if (rs < 0) break ;
	            } /* end for */
	            rs1 = b.finish ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (sbuf) */
	    } /* end if (supported AF) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (ucentnw::format) */

int ucentnw::size() noex {
	int		rs = SR_FAULT ;
	if (this) {
	    int		sz = 1 ;
	    if (n_name) {
	        sz += (strlen(n_name)+1) ;
	    }
	    if (n_aliases) {
	        int	i ; /* used-afterwards */
	        for (i = 0 ; n_aliases[i] ; i += 1) {
	            sz += (strlen(n_aliases[i])+1) ;
	        } /* end for */
	        sz += ((i+1)*szof(cchar *)) ;
	    } /* end if (name-list aliases) */
	    rs = iceil(sz,szof(cchar *)) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (ucentnw::size) */

int ucentnw::getent(char *nwbuf,int nwlen) noex {
	return uc_getnwent(this,nwbuf,nwlen) ;
}

int ucentnw::getnam(char *nwbuf,int nwlen,cchar *name) noex {
	return uc_getnwnam(this,nwbuf,nwlen,name) ;
}

int ucentnw::getnum(char *nwbuf,int nwlen,int af,uint32_t num) noex {
	return uc_getnwnum(this,nwbuf,nwlen,af,num) ;
}


/* local subroutines */

static int ucentnw_parseaddr(ucentnw *op,cchar *sp,int sl) noex {
	static constexpr int	af = AF_INET4 ;
	cint	alen = inetxaddrlen ;
	int	rs ;
	char	abuf[inetxaddrlen+1] ;
	if ((rs = inetnetpton(abuf,alen,af,sp,sl)) >= 0) {
	    in4_addr_t	a ;
	    memcpy(&a,abuf) ;
	    op->n_net = ntohl(a) ;	/* |n_net| is machine-byte-order */
	}
	return rs ;
}
/* end subroutine (ucentnw_parseaddr) */

static int ucentnw_parsestrs(ucentnw *prp,SI *sip,cchar *sp,int sl) noex {
	int		rs ;
	int		rs1 ;
	if (vechand u ; (rs = u.start(8,0)) >= 0) {
	    if ((rs = si_loadnames(sip,&u,sp,sl)) > 0) {
	        cint	n = rs ;
	        if (void **ptab{} ; (rs = sip->ptab(n,&ptab)) >= 0) {
		    int		i ; /* used-afterwards */
	            void	*vp{} ;
	            prp->n_aliases = charpp(ptab) ;
		    for (i = 0 ; u.get(i,&vp) >= 0 ; i += 1) {
	                prp->n_aliases[i] = charp(vp) ;
	            } /* end for */
	            prp->n_aliases[i] = nullptr ;
	        } /* end if (storeitem-ptab) */
	    } else {
	        prp->n_aliases = nullptr ;
	    }
	    rs1 = u.finish ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (vechand) */
	return rs ;
}
/* end subroutine (ucentnw_parsestrs) */

static int ucentnw_formataddr(ucentnw *hep,sbuf *sbp) noex {
	cint		af = hep->n_addrtype ;
	cint		plen = astrlen ;
	int		rs ;
	char		pbuf[astrlen +1] ;
	in4_addr_t	a = htonl(hep->n_net) ;
	if ((rs = inetntop(pbuf,plen,af,&a)) >= 0) {
	    rs = sbp->strw(pbuf,rs) ;
	}
	return rs ;
}
/* end subroutine (ucentnw_formataddr) */


