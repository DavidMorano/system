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
	ucenumho

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
#include	<cstring>
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
#include	<localmisc.h>

#include	"ucentho.h"


/* local defines */

#define	NWE	ucentho
#define	CNWE	const ucentho
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

using std::max ;			/* type */


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

static int si_loadnames(SI *,vechand *,cchar *,int) noex ;
static int si_loadname(SI *,vechand *,cchar *,int) noex ;
static int si_copystr(SI *,char **,cchar *) noex ;

static inline int getaflen(int) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int ucentho::parse(char *ebuf,int elen,cchar *sp,int sl) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (this && ebuf && sp) {
	    storeitem	ib, *ibp = &ib ;
	    if (sl < 0) sl = strlen(sp) ;
	    memclear(this) ;		/* potentially dangerous */
	    h_addrtype = AF_INET4 ;		/* <- mandatory */
	    if ((rs = storeitem_start(ibp,ebuf,elen)) >= 0) {
	        int	fi = 0 ;
		int	si ;
		int	cl ;
	        cchar	*cp{} ;
		if ((si = sichr(sp,sl,'#')) >= 0) sl = si ;
	        while ((cl = sfnext(sp,sl,&cp)) > 0) {
	            cchar	**vpp = nullptr ;
	            switch (fi++) {
	            case 0:
	                rs = ucentho_parseaddr(this,ibp,cp,cl) ;
	                break ;
	            case 1:
	                vpp = (cchar **) &h_name ;
	                break ;
	            case 2:
	                rs = ucentho_parsestrs(this,ibp,cp,cl) ;
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
/* end subroutine (ucentho::parse) */

int ucentho::load(char *rbuf,int rlen,const ucentho *cprp) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (this && rbuf && cprp) {
	    storeitem	ib ;
	    memcpy(this,cprp,sizeof(ucentho)) ;
	    if ((rs = storeitem_start(&ib,rbuf,rlen)) >= 0) {
	        if (cprp->h_aliases) {
	            int		n = 0 ;
	            void	**tab{} ;
	            for (n = 0 ; cprp->h_aliases[n] ; n += 1) ;
	            if ((rs = storeitem_ptab(&ib,n,&tab)) >= 0) {
		        cchar	**aliases = (cchar **) cprp->h_aliases ;
		        int	i = 0 ;
	                h_aliases = (char **) tab ;
	                for (i = 0 ; cprp->h_aliases[i] ; i += 1) {
	                    rs = si_copystr(&ib,(h_aliases + i),aliases[i]) ;
	                    if (rs < 0) break ;
	                } /* end for */
	                h_aliases[i] = nullptr ;
	            } /* end if (storeitem-ptab) */
	        } else {
		    h_aliases = nullptr ;
	        } /* end if (aliases) */
		if (rs >= 0) rs = si_copystr(&ib,&h_name,cprp->h_name) ;
	        rs1 = storeitem_finish(&ib) ;
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
	    sbuf	b ;
	    if ((rs = sbuf_start(&b,rbuf,rlen)) >= 0) {
	        for (int i = 0 ; i < 3 ; i += 1) {
	            if (i > 0) rs = sbuf_chr(&b,' ') ;
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
				cchar	**sv = (cchar **) h_aliases ;
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
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (ucentho::format) */

int ucentho::size() noex {
	int		rs = SR_FAULT ;
	if (this) {
	    int		size = 1 ;
	    if (h_name) {
	        size += (strlen(h_name)+1) ;
	    }
	    if (h_aliases) {
	        int	i = 0 ;
	        for (i = 0 ; h_aliases[i] ; i += 1) {
	            size += (strlen(h_aliases[i])+1) ;
	        } /* end for */
	        size += ((i+1)*sizeof(cchar *)) ;
	    } /* end if (name-alias list) */
	    if (h_addr_list) {
		cint	asize = h_length ;
	        int	i = 0 ;
	        for (i = 0 ; h_addr_list[i] ; i += 1) {
	            size += (asize+1) ;
	        } /* end for */
	        size += ((i+1)*sizeof(cchar *)) ;
	    } /* end if (addr-alias list) */
	    rs = iceil(size,sizeof(cchar *)) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (ucentho::size) */


/* local subroutines */

static int ucentho_parseaddr(ucentho *hep,SI *ibp,cchar *sp,int sl) noex {
	constexpr int	alen = inetxaddrlen ;
	int		rs ;
	char		abuf[inetxaddrlen+1] ;
	if ((rs = inetpton(abuf,alen,0,sp,sl)) >= 0) {
	    cint	addrl = getaflen(rs) ;
	    cchar	*sap = nullptr ;
	    hep->h_addrtype = rs ;	/* address-type (v4 or v6) */
	    hep->h_length = addrl ;
	    if ((rs = storeitem_buf(ibp,abuf,addrl,&sap)) >= 0) {
	        cint	n = 1 ;		/* one address to store */
	        void	**vpp = nullptr ;
	        if ((rs = storeitem_ptab(ibp,n,&vpp)) >= 0) {
		    char	**bpp = (char **) vpp ;
		    hep->h_addr_list = (char **) vpp ;
		    bpp[0] = (char *) sap ;
		    bpp[1] = nullptr ;
	        } /* end if (storeitem_ptab) */
	    } /* end if (storeitem_buf) */
	} /* end if (inetpton) */
	return rs ;
}
/* end subroutine (ucentho_parseaddr) */

static int ucentho_parsestrs(ucentho *prp,SI *ibp,cchar *sp,int sl) noex {
	vechand		u ;
	int		rs ;
	int		rs1 ;
	if ((rs = vechand_start(&u,8,0)) >= 0) {
	    if ((rs = si_loadnames(ibp,&u,sp,sl)) > 0) {
	        cint	n = rs ;
	        void	**ptab{} ;
	        if ((rs = storeitem_ptab(ibp,n,&ptab)) >= 0) {
		    int		i = 0 ;
	            void	*vp{} ;
	            prp->h_aliases = (char **) ptab ;
		    for (i = 0 ; vechand_get(&u,i,&vp) >= 0 ; i += 1) {
	                prp->h_aliases[i] = (char *) vp ;
	            } /* end for */
	            prp->h_aliases[i] = nullptr ;
	        } /* end if (storeitem-ptab) */
	    } else {
	        prp->h_aliases = nullptr ;
	    }
	    rs1 = vechand_finish(&u) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (vechand) */
	return rs ;
}
/* end subroutine (ucentho_parsestrs) */

static int ucentho_formataddr(ucentho *hep,sbuf *sbp) noex {
	cint		af = hep->h_addrtype ;
	cint		plen = ASTRLEN ;
	int		rs ;
	char		pbuf[ASTRLEN+1] ;
	cvoid		*binaddr = (cvoid *) hep->h_addr_list[0] ;
	if ((rs = inetntop(pbuf,plen,af,binaddr)) >= 0) {
	    rs = sbuf_strw(sbp,pbuf,rs) ;
	}
	return rs ;
}
/* end subroutine (ucentho_formataddr) */

static int ucentho_formatname(ucentho *hep,sbuf *sbp) noex {
	cint		clen = 16 ;
	int		rs ;
	if ((rs = sbuf_getlen(sbp)) >= 0) {
	    cint	idx = rs ;
	    cint	nb = (idx >= clen) ? 1 : (clen-idx) ;
	    if ((rs = sbuf_blanks(sbp,nb)) >= 0) {
		rs = sbuf_strw(sbp,hep->h_name,-1) ;
	    }
	}
	return rs ;
}
/* end subroutine (ucentho_formatname) */

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
	cchar		*cp{} ;
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


