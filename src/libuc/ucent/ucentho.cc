/* ucentho SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* UCENTHO object management */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-23, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Group:
	uc_entho{x}

	Description:
	I provide the normal (usual) subroutines for managing the
	UCENTHO (UNIX® |HOSTENT| database entries) object.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<unistd.h>
#include	<cerrno>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>		/* |strlen(3c)| */
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
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
#include	<localmisc.h>

#include	"ucentho.h"
#include	"ucentxx.hh"


/* local defines */

#define	NWE	ucentho
#define	CNWE	const ucentho
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


/* local variables */

constexpr int                   inet4addrlen = int(INET4ADDRLEN) ;
constexpr int                   inet6addrlen = int(INET6ADDRLEN) ;
constexpr int                   inetxaddrlen = max(inet4addrlen,inet6addrlen) ;
constexpr int                   astrlen = int(ASTRLEN) ;


/* forward references */

static int ucentho_parseaddr(NWE *,SI *,cchar *,int) noex ;
static int ucentho_parsestrs(NWE *,SI *,cchar *,int) noex ;
static int ucentho_formataddr(NWE *,sbuf *) noex ;
static int ucentho_formatname(NWE *,sbuf *) noex ;

static inline int getaflen(int) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int ucentho::parse(char *ebuf,int elen,cchar *sp,int sl) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (this && ebuf && sp) {
	    if (sl < 0) sl = strlen(sp) ;
	    memclear(this) ;		/* potentially dangerous */
	    h_addrtype = AF_INET4 ;	/* <- mandatory */
	    if (storeitem si ; (rs = si.start(ebuf,elen)) >= 0) {
	        int	fi = 0 ;
		int	cl ;
	        cchar	*cp{} ;
		if (int idx ; (idx = sichr(sp,sl,'#')) >= 0) {
		    sl = idx ;
		}
	        while ((cl = sfnext(sp,sl,&cp)) > 0) {
	            cchar	**vpp = nullptr ;
	            switch (fi++) {
	            case 0:
	                rs = ucentho_parseaddr(this,&si,cp,cl) ;
	                break ;
	            case 1:
	                vpp = ccharpp(&h_name) ;
	                break ;
	            case 2:
	                rs = ucentho_parsestrs(this,&si,cp,cl) ;
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
/* end subroutine (ucentho::parse) */

int ucentho::load(char *rbuf,int rlen,const ucentho *cprp) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (this && rbuf && cprp) {
	    memcpy(this,cprp,sizeof(ucentho)) ;
	    if (storeitem si ; (rs = si.start(rbuf,rlen)) >= 0) {
	        if (cprp->h_aliases) {
	            int		n = 0 ;
	            for (n = 0 ; cprp->h_aliases[n] ; n += 1) ;
	            if (void **tab{} ; (rs = si.ptab(n,&tab)) >= 0) {
		        cchar	**aliases = ccharpp(cprp->h_aliases) ;
		        int	i = 0 ; /* used-afterwards */
	                h_aliases = charpp(tab) ;
	                for (i = 0 ; cprp->h_aliases[i] ; i += 1) {
	                    rs = si_copystr(&si,(h_aliases + i),aliases[i]) ;
	                    if (rs < 0) break ;
	                } /* end for */
	                h_aliases[i] = nullptr ;
	            } /* end if (storeitem-ptab) */
	        } else {
		    h_aliases = nullptr ;
	        } /* end if (aliases) */
		if (rs >= 0) {
		    rs = si_copystr(&si,&h_name,cprp->h_name) ;
		}
	        rs1 = si.finish ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (storeitem) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (ucentho::load) */

int ucentho::format(char *rbuf,int rlen) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (this && rbuf) {
	    if (sbuf b ; (rs = b.start(rbuf,rlen)) >= 0) {
	        for (int i = 0 ; i < 3 ; i += 1) {
	            if (i > 0) rs = b.chr(' ') ;
	            if (rs >= 0) {
	                switch (i) {
	                case 0:
			    rs = ucentho_formataddr(this,&b) ;
	                    break ;
	                case 1:
			    rs = ucentho_formatname(this,&b) ;
	                    break ;
	                case 2:
			    if (h_aliases) {
				cchar	**sv = ccharpp(h_aliases) ;
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
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (ucentho::format) */

int ucentho::size() noex {
	int		rs = SR_FAULT ;
	if (this) {
	    int		sz = 1 ;
	    if (h_name) {
	        sz += (strlen(h_name)+1) ;
	    }
	    if (h_aliases) {
	        int	i = 0 ; /* used-afterwards */
	        for (i = 0 ; h_aliases[i] ; i += 1) {
	            sz += (strlen(h_aliases[i])+1) ;
	        } /* end for */
	        sz += ((i+1)*szof(cchar *)) ;
	    } /* end if (name-alias list) */
	    if (h_addr_list) {
		cint	asize = h_length ;
	        int	i = 0 ; /* used-afterwards */
	        for (i = 0 ; h_addr_list[i] ; i += 1) {
	            sz += (asize+1) ;
	        } /* end for */
	        sz += ((i+1)*szof(cchar *)) ;
	    } /* end if (addr-alias list) */
	    rs = iceil(sz,szof(cchar *)) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (ucentho::size) */


/* local subroutines */

static int ucentho_parseaddr(ucentho *hep,SI *sip,cchar *sp,int sl) noex {
	constexpr int	alen = inetxaddrlen ;
	int		rs ;
	char		abuf[inetxaddrlen+1] ;
	if ((rs = inetpton(abuf,alen,0,sp,sl)) >= 0) {
	    cint	addrl = getaflen(rs) ;
	    hep->h_addrtype = rs ;	/* address-type (v4 or v6) */
	    hep->h_length = addrl ;
	    if (cchar *sap{} ; (rs = sip->buf(abuf,addrl,&sap)) >= 0) {
	        cint	n = 1 ;		/* one address to store */
	        if (void **vpp{} ; (rs = sip->ptab(n,&vpp)) >= 0) {
		    char	**bpp = charpp(vpp) ;
		    hep->h_addr_list = charpp(vpp) ;
		    bpp[0] = cast_const<charp>(sap) ;
		    bpp[1] = nullptr ;
	        } /* end if (storeitem_ptab) */
	    } /* end if (storeitem_buf) */
	} /* end if (inetpton) */
	return rs ;
}
/* end subroutine (ucentho_parseaddr) */

static int ucentho_parsestrs(ucentho *prp,SI *sip,cchar *sp,int sl) noex {
	int		rs ;
	int		rs1 ;
	if (vechand u ; (rs = u.start(8,0)) >= 0) {
	    if ((rs = si_loadnames(sip,&u,sp,sl)) > 0) {
	        cint	n = rs ;
	        if (void **ptab{} ; (rs = sip->ptab(n,&ptab)) >= 0) {
		    int		i ; /* used-afterwards */
	            void	*vp{} ;
	            prp->h_aliases = charpp(ptab) ;
		    for (i = 0 ; u.get(i,&vp) >= 0 ; i += 1) {
	                prp->h_aliases[i] = charp(vp) ;
	            } /* end for */
	            prp->h_aliases[i] = nullptr ;
	        } /* end if (storeitem-ptab) */
	    } else {
	        prp->h_aliases = nullptr ;
	    }
	    rs1 = u.finish ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (vechand) */
	return rs ;
}
/* end subroutine (ucentho_parsestrs) */

static int ucentho_formataddr(ucentho *hep,sbuf *sbp) noex {
	cint		af = hep->h_addrtype ;
	cint		plen = astrlen ;
	int		rs ;
	char		pbuf[astrlen+1] ;
	cvoid		*binaddr = voidp(hep->h_addr_list[0]) ;
	if ((rs = inetntop(pbuf,plen,af,binaddr)) >= 0) {
	    rs = sbp->strw(pbuf,rs) ;
	}
	return rs ;
}
/* end subroutine (ucentho_formataddr) */

static int ucentho_formatname(ucentho *hep,sbuf *sbp) noex {
	cint		clen = 16 ;
	int		rs ;
	if ((rs = sbp->getlen) >= 0) {
	    cint	idx = rs ;
	    cint	nb = (idx >= clen) ? 1 : (clen-idx) ;
	    if ((rs = sbp->blanks(nb)) >= 0) {
		rs = sbp->str(hep->h_name) ;
	    }
	}
	return rs ;
}
/* end subroutine (ucentho_formatname) */

static inline int getaflen(int af) noex {
	int	aflen = 0 ;
	switch (af) {
	case AF_INET4:
	    aflen = inet4addrlen ;
	    break ;
	case AF_INET6:
	    aflen = inet6addrlen ;
	    break ;
	} /* end switch */
	return aflen ;
}
/* end subroutine (getaflen) */


