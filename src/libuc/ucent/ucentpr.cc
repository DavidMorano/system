/* ucentpr SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* UCENTPR object management */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-23, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Group:
	uc_entpr{x}

	Description:
	I provide the normal (usual) subroutines for managing the
	UCENTPR (UNIX® |PROTOENT| database entries) object.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<unistd.h>
#include	<cerrno>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<storeitem.h>
#include	<sbuf.h>
#include	<vechand.h>
#include	<strn.h>
#include	<sfx.h>
#include	<six.h>
#include	<intceil.h>
#include	<cfdec.h>
#include	<localmisc.h>

#include	"ucentpr.h"
#include	"ucentxx.hh"


/* local defines */

#define	PRE	ucentpr
#define	CPRE	const ucentpr
#define	SI	storeitem


/* imported namespaces */

using ucent::si_loadnames ;		/* local group support subroutine */
using ucent::si_loadname ;		/* local group support subroutine */
using ucent::si_copystr ;		/* local group support subroutine */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

static int ucentpr_parsestrs(PRE *,SI *,cchar *,int) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int ucentpr::parse(char *ebuf,int elen,cchar *sp,int sl) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (this && ebuf && sp) {
	    if (sl < 0) sl = strlen(sp) ;
	    memclear(this) ;		/* potentially dangerous */
	    if (storeitem si ; (rs = si.start(ebuf,elen)) >= 0) {
	        cchar	*cp{} ;
		if (int idx ; (idx = sichr(sp,sl,'#')) >= 0) {
		    sl = idx ;
		}
	        for (int cl, fi = 0 ; (cl = sfnext(sp,sl,&cp)) > 0 ; ) {
	            int		v = -1 ;
	            cchar	**vpp = nullptr ;
	            switch (fi++) {
	            case 0:
	                vpp = ccharpp(&p_name) ;
	                break ;
	            case 1:
	                rs = cfdeci(cp,cl,&v) ;
	                p_proto = v ;
	                break ;
	            case 2:
	                rs = ucentpr_parsestrs(this,&si,cp,cl) ;
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
/* end subroutine (ucentpr::parse) */

int ucentpr::load(char *rbuf,int rlen,const ucentpr *cprp) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (this && rbuf && cprp) {
	    memcpy(this,cprp,sizeof(ucentpr)) ;
	    if (storeitem si ; (rs = si.start(rbuf,rlen)) >= 0) {
	        if (cprp->p_aliases) {
	            int		n ; /* used-afterwards */
	            for (n = 0 ; cprp->p_aliases[n] ; n += 1) ;
	            if (void **tab{} ; (rs = si.ptab(n,&tab)) >= 0) {
		        cchar	**aliases = ccharpp(cprp->p_aliases) ;
		        int	i ; /* used-afterwards */
	                p_aliases = charpp(tab) ;
	                for (i = 0 ; cprp->p_aliases[i] ; i += 1) {
			    cchar	*an = aliases[i] ;
	                    rs = si_copystr(&si,(p_aliases + i),an) ;
	                    if (rs < 0) break ;
	                } /* end for */
	                p_aliases[i] = nullptr ;
	            } /* end if (storeitem-ptab) */
	        } else {
		    p_aliases = nullptr ;
	        } /* end if (aliases) */
		if (rs >= 0) {
		    rs = si_copystr(&si,&p_name,cprp->p_name) ;
		}
	        rs1 = si.finish ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (storeitem) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (ucentpr::load) */

int ucentpr::format(char *rbuf,int rlen) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (this && rbuf) {
	    if (sbuf b ; (rs = b.start(rbuf,rlen)) >= 0) {
	        for (int i = 0 ; i < 3 ; i += 1) {
	            if (i > 0) rs = b.chr(' ') ;
	            if (rs >= 0) {
		        int	v ;
	                switch (i) {
	                case 0:
	                    rs = b.strw(p_name,-1) ;
	                    break ;
	                case 1:
	                    v = p_proto ;
	                    rs = b.dec(v) ;
	                    break ;
	                case 2:
	    		    if (p_aliases) {
				cchar	**sv = (cchar **) p_aliases ;
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
/* end subroutine (ucentpr::format) */

int ucentpr::size() noex {
	int		rs = SR_FAULT ;
	if (this) {
	    int		sz = 1 ;
	    if (p_name) {
	        sz += (strlen(p_name)+1) ;
	    }
	    if (p_aliases) {
	        int	i ; /* used-afterwards */
	        for (i = 0 ; p_aliases[i] ; i += 1) {
	            sz += (strlen(p_aliases[i])+1) ;
	        } /* end for */
	        sz += ((i+1) * szof(cchar *)) ;
	    } /* end if (group members) */
	    rs = iceil(sz,szof(cchar *)) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (ucentpr::size) */

int ucentpr::getent(char *prbuf,int prlen) noex {
	return uc_getprent(this,prbuf,prlen) ;
}

int ucentpr::getnam(char *prbuf,int prlen,cchar *name) noex {
	return uc_getprnam(this,prbuf,prlen,name) ;
}

int ucentpr::getnum(char *prbuf,int prlen,int num) noex {
	return uc_getprnum(this,prbuf,prlen,num) ;
}


/* local subroutines */

static int ucentpr_parsestrs(ucentpr *prp,SI *sip,cchar *sp,int sl) noex {
	int		rs ;
	int		rs1 ;
	if (vechand u ; (rs = u.start(8,0)) >= 0) {
	    if ((rs = si_loadnames(sip,&u,sp,sl)) > 0) {
	        cint	n = rs ;
	        if (void **ptab{} ; (rs = sip->ptab(n,&ptab)) >= 0) {
		    int		i ; /* used-afterwards */
	            void	*vp{} ;
	            prp->p_aliases = charpp(ptab) ;
		    for (i = 0 ; u.get(i,&vp) >= 0 ; i += 1) {
	                prp->p_aliases[i] = charp(vp) ;
	            } /* end for */
	            prp->p_aliases[i] = nullptr ;
	        } /* end if (storeitem-ptab) */
	    } else {
	        prp->p_aliases = nullptr ;
	    }
	    rs1 = u.finish ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (vechand) */
	return rs ;
}
/* end subroutine (ucentpr_parsestrs) */


