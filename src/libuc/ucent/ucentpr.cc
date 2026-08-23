/* ucentpr SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

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
	These subroutines facilitate read-nnly access to the the
	system PROTOCOLS database.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<new>			/* C++STD |nothrow(3c++)| */
#include	<algorithm>		/* C++STD |min(3c++)| + |max(3c++)| */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<intceil.h>		/* LIBU */
#include	<six.h>			/* LIBUC */
#include	<storeitem.h>		/* LIBUC */
#include	<sbuf.h>		/* LIBUC */
#include	<vechand.h>		/* LIBUC */
#include	<strn.h>		/* LIBUC */
#include	<sfx.h>			/* LIBUC */
#include	<cfdec.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */

#include	"ucgetpr.h"		/* |uc_getpr{3uc}(3uc)| */
#include	"ucentpr.h"
#include	"ucentxx.hh"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |memclear(3u)| */

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

local int ucentpr_parsestrs(PRE *,SI *,cchar *,int) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int ucentpr::parse(char *ebuf,int elen,cchar *sp,int sl) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (ebuf && sp) ylikely {
	    PROTOENT *pep = this ;
	    if (sl < 0) sl = lenstr(sp) ;
	    memclear(pep) ;
	    if (storeitem si ; (rs = si.start(ebuf,elen)) >= 0) ylikely {
	        cchar	*cp{} ;
		if (int idx ; (idx = sichr(sp,sl,'#')) >= 0) {
		    sl = idx ;
		} /* end if */
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
	            sl -= intconv((cp + cl) - sp) ;
	            sp = (cp + cl) ;
	            if (rs < 0) break ;
	        } /* end for */
	        rs1 = si.finish ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (storeitem) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (ucentpr::parse) */

int ucentpr::load(char *rbuf,int rlen,const ucentpr *cprp) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (rbuf && cprp) {
	    PROTOENT *pep = this ;
	    *pep = *cprp ; /* shallow copy */
	    if (storeitem si ; (rs = si.start(rbuf,rlen)) >= 0) {
	        if (cprp->p_aliases) {
	            cint	n = lenstrarr(cprp->p_aliases) ;
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
		} /* end if (ok) */
	        rs1 = si.finish ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (storeitem) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (ucentpr::load) */

int ucentpr::format(char *rbuf,int rlen) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (rbuf) ylikely {
	    if (sbuf b ; (rs = b.start(rbuf,rlen)) >= 0) ylikely {
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
} /* end subroutine (ucentpr::format) */

int ucentpr::size() noex {
	int		rs = SR_OK ;
	    int		sz = 1 ;
	    if (p_name) {
	        sz += (lenstr(p_name) + 1) ;
	    }
	    if (p_aliases) {
	        int	i ; /* used-afterwards */
	        for (i = 0 ; p_aliases[i] ; i += 1) {
	            sz += (lenstr(p_aliases[i]) + 1) ;
	        } /* end for */
	        sz += ((i+1) * szof(cchar *)) ;
	    } /* end if (group members) */
	    rs = iceil(sz,szof(cchar *)) ;
	return rs ;
} /* end subroutine (ucentpr::size) */

int ucentpr::getent(char *prbuf,int prlen) noex {
	return uc_getprent(this,prbuf,prlen) ;
} /* end method */

int ucentpr::getnam(char *prbuf,int prlen,cchar *name) noex {
	return uc_getprnam(this,prbuf,prlen,name) ;
} /* end method */

int ucentpr::getnum(char *prbuf,int prlen,int num) noex {
	return uc_getprnum(this,prbuf,prlen,num) ;
} /* end method */


/* local subroutines */

local int ucentpr_parsestrs(ucentpr *prp,SI *sip,cchar *sp,int sl) noex {
	int		rs ;
	int		rs1 ;
	if (vechand u ; (rs = u.start(8,0)) >= 0) ylikely {
	    if ((rs = si_loadnames(sip,&u,sp,sl)) > 0) ylikely {
	        cint	n = rs ;
	        if (void **ptab{} ; (rs = sip->ptab(n,&ptab)) >= 0) ylikely {
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
} /* end subroutine (ucentpr_parsestrs) */


