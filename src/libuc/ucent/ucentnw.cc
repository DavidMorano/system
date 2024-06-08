/* ucentnw SUPPORT */
/* lang=C++20 */

/* UCENTNW object management */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-23, David A­D­ Morano
	This subroutine was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	I provide the normal (usual) subroutines for managing the
	UCENTNW (UNIX® |NETWENT| database entries) object.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<arpa/inet.h>		/* <- |htonl(3c)| */
#include	<cerrno>
#include	<unistd.h>
#include	<cstdlib>
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


/* local defines */

#define	NWE	ucentnw
#define	CNWE	const ucentnw
#define	SI	storeitem

#ifndef INET4ADDRLEN
#define INET4ADDRLEN            sizeof(in_addr_t)
#endif

#ifndef INET6ADDRLEN
#define INET6ADDRLEN            sizeof(in6_addr_t)
#endif

#ifndef INET4_ADDRSTRLEN
#define INET4_ADDRSTRLEN        16
#endif

#ifndef INET6_ADDRSTRLEN
#define INET6_ADDRSTRLEN        46      /* Solaris® says this is 46! */
#endif

#define ASTRLEN                 MAX(INET4_ADDRSTRLEN,INET6_ADDRSTRLEN)


/* imported namespaces */

using std::min ;			/* subroutine-template */
using std::max ;			/* subroutine-template */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

static int ucentnw_parseaddr(NWE *,cchar *,int) noex ;
static int ucentnw_parsestrs(NWE *,SI *,cchar *,int) noex ;

static int ucentnw_formataddr(NWE *,sbuf *) noex ;

static int si_loadnames(SI *,vechand *,cchar *,int) noex ;
static int si_loadname(SI *,vechand *,cchar *,int) noex ;
static int si_copystr(SI *,char **,cchar *) noex ;


/* local variables */

constexpr int			inet4addrlen = int(INET4ADDRLEN) ;
constexpr int			inet6addrlen = int(INET6ADDRLEN) ;
constexpr int			inetxaddrlen = max(inet4addrlen,inet6addrlen) ;
constexpr int			astrlen = int(ASTRLEN) ;


/* exported variables */


/* exported subroutines */

int ucentnw::parse(char *ebuf,int elen,cchar *sp,int sl) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (this && ebuf && sp) {
	    storeitem	ib, *ibp = &ib ;
	    if (sl < 0) sl = strlen(sp) ;
	    memclear(this) ;		/* potentially dangerous */
	    n_addrtype = AF_INET4 ;		/* <- mandatory */
	    if ((rs = storeitem_start(ibp,ebuf,elen)) >= 0) {
	        int	fi = 0 ;
		int	si ;
		int	cl ;
	        cchar	*cp ;
		if ((si = sichr(sp,sl,'#')) >= 0) sl = si ;
	        while ((cl = sfnext(sp,sl,&cp)) > 0) {
	            cchar	**vpp = nullptr ;
	            switch (fi++) {
	            case 0:
	                vpp = (cchar **) &n_name ;
	                break ;
	            case 1:
	                rs = ucentnw_parseaddr(this,cp,cl) ;
	                break ;
	            case 2:
	                rs = ucentnw_parsestrs(this,ibp,cp,cl) ;
	                break ;
	            } /* end switch */
		    if ((rs >= 0) && vpp) {
	        	rs = storeitem_strw(ibp,cp,cl,vpp) ;
		    }
	            sl -= ((cp+cl)-sp) ;
	            sp = (cp+cl) ;
	            if (rs < 0) break ;
	        } /* end while */
	        rs1 = storeitem_finish(ibp) ;
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
	    storeitem	ib ;
	    memcpy(this,cprp,sizeof(ucentnw)) ;
	    if ((rs = storeitem_start(&ib,rbuf,rlen)) >= 0) {
	        if (cprp->n_aliases) {
	            int		n = 0 ;
	            void	**tab{} ;
	            for (n = 0 ; cprp->n_aliases[n] ; n += 1) ;
	            if ((rs = storeitem_ptab(&ib,n,&tab)) >= 0) {
		        cchar	**aliases = (cchar **) cprp->n_aliases ;
		        int	i = 0 ;
	                n_aliases = (char **) tab ;
	                for (i = 0 ; cprp->n_aliases[i] ; i += 1) {
	                    rs = si_copystr(&ib,(n_aliases + i),aliases[i]) ;
	                    if (rs < 0) break ;
	                } /* end for */
	                n_aliases[i] = nullptr ;
	            } /* end if (storeitem-ptab) */
	        } else {
		    n_aliases = nullptr ;
	        } /* end if (aliases) */
		if (rs >= 0) rs = si_copystr(&ib,&n_name,cprp->n_name) ;
	        rs1 = storeitem_finish(&ib) ;
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
	        sbuf	b ;
	        if ((rs = sbuf_start(&b,rbuf,rlen)) >= 0) {
	            for (int i = 0 ; i < 3 ; i += 1) {
	                if (i > 0) rs = sbuf_chr(&b,' ') ;
	                if (rs >= 0) {
	                    switch (i) {
	                    case 0:
	                        rs = sbuf_strw(&b,n_name,-1) ;
	                        break ;
	                    case 1:
			        rs = ucentnw_formataddr(this,&b) ;
	                        break ;
	                    case 2:
			        if (n_aliases) {
				    cchar	**sv = (cchar **) n_aliases ;
	                            rs = sbuf_strs(&b,' ',sv) ;
			        }
	                        break ;
	                    } /* end switch */
	                } /* end if */
	                if (rs < 0) break ;
	            } /* end for */
	            rs1 = sbuf_finish(&b) ;
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
	    int		size = 1 ;
	    if (n_name) {
	        size += (strlen(n_name)+1) ;
	    }
	    if (n_aliases) {
	        int	i = 0 ;
	        for (i = 0 ; n_aliases[i] ; i += 1) {
	            size += (strlen(n_aliases[i])+1) ;
	        } /* end for */
	        size += ((i+1)*sizeof(cchar *)) ;
	    } /* end if (name-list aliases) */
	    rs = iceil(size,sizeof(cchar *)) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (ucentnw::size) */


/* local subroutines */

static int ucentnw_parseaddr(ucentnw *op,cchar *sp,int sl) noex {
	static constexpr int	af = AF_INET4 ;
	cint	alen = inetxaddrlen ;
	int	rs ;
	char	abuf[inetxaddrlen+1] ;
	if ((rs = inetnetpton(abuf,alen,af,sp,sl)) >= 0) {
	    in4_addr_t	a ;
	    memcpy(&a,abuf,inet4addrlen) ;
	    op->n_net = ntohl(a) ;	/* |n_net| is machine-byte-order */
	}
	return rs ;
}
/* end subroutine (ucentnw_parseaddr) */

static int ucentnw_parsestrs(ucentnw *prp,SI *ibp,cchar *sp,int sl) noex {
	vechand		u ;
	int		rs ;
	int		rs1 ;
	if ((rs = vechand_start(&u,8,0)) >= 0) {
	    if ((rs = si_loadnames(ibp,&u,sp,sl)) > 0) {
	        cint	n = rs ;
	        void	**ptab ;
	        if ((rs = storeitem_ptab(ibp,n,&ptab)) >= 0) {
		    int		i = 0 ;
	            void	*vp{} ;
	            prp->n_aliases = (char **) ptab ;
		    for (i = 0 ; vechand_get(&u,i,&vp) >= 0 ; i += 1) {
	                prp->n_aliases[i] = (char *) vp ;
	            } /* end for */
	            prp->n_aliases[i] = nullptr ;
	        } /* end if (storeitem-ptab) */
	    } else {
	        prp->n_aliases = nullptr ;
	    }
	    rs1 = vechand_finish(&u) ;
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
	    rs = sbuf_strw(sbp,pbuf,rs) ;
	}
	return rs ;
}
/* end subroutine (ucentnw_formataddr) */

static int si_loadnames(SI *ibp,vechand *ulp,cchar *sp,int sl) noex {
	int		rs = SR_OK ;
	int		c = 0 ;
	cchar		*tp ;
	while ((tp = strnpbrk(sp,sl," ,")) != nullptr) {
	    if ((tp-sp) > 0) {
		c += 1 ;
		rs = si_loadname(ibp,ulp,sp,(tp-sp)) ;
	    } /* end if (non-zero) */
	    sl -= ((tp+1)-sp) ;
	    sp = (tp+1) ;
	    if (rs < 0) break ;
	} /* end while */
	if ((rs >= 0) && sl && sp[0]) {
	    c += 1 ;
	    rs = si_loadname(ibp,ulp,sp,sl) ;
	}
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (si_loadnames) */

static int si_loadname(SI *ibp,vechand *ulp,cchar *sp,int sl) noex {
	int		rs ;
	cchar		*cp ;
	if ((rs = storeitem_strw(ibp,sp,sl,&cp)) >= 0) {
	    rs = vechand_add(ulp,cp) ;
	}
	return rs ;
}
/* end subroutine (si_loadname) */

static int si_copystr(SI *ibp,char **pp,cchar *p1) noex {
	int		rs = SR_OK ;
	cchar		**cpp = (cchar **) pp ;
	*cpp = nullptr ;
	if (p1) {
	    rs = storeitem_strw(ibp,p1,-1,cpp) ;
	}
	return rs ;
}
/* end subroutine (si_copystr) */


