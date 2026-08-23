/* ucentpj SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* UCENTPJ object management */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-23, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Group:
	uc_entpj{x}

	Description:
	These subroutines facilitate read-nnly access to the the
	system PROJECT database.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<new>			/* C++STD |nothrow(3c++)| */
#include	<algorithm>		/* C++STD |min(3c++)| + |max(3c++)| */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<intceil.h>		/* LIBU */
#include	<storeitem.h>		/* LIBUC */
#include	<sbuf.h>		/* LIBUC */
#include	<vechand.h>		/* LIBUC */
#include	<strn.h>		/* LIBUC */
#include	<sfx.h>			/* LIBUC */
#include	<six.h>			/* LIBUC */
#include	<cfdec.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */

#include	"ucgetpj.h"		/* |uc_getpj{x}(3uc)| */
#include	"ucentpj.h"
#include	"ucentxx.hh"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |memclear(3u)| */

/* local defines */

#define	PJE	ucentpj
#define	CPJE	const ucentpj
#define	SI	storeitem


/* imported namespaces */

using ucent::si_copystr ;		/* local group support subroutine */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

local int si_storestrs(SI *,int,cchar *,int,char ***) noex ;
local int si_loadstrs(SI *,vechand *,int,cc *,int) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int ucentpj::parse(char *pjbuf,int pjlen,cchar *sp,int sl) noex {
    	cnullptr	np{} ;
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (pjbuf && sp) ylikely {
	    PROJECT *pep = this ;
	    if (sl < 0) sl = lenstr(sp) ;
	    memclear(pep) ;
	    if (storeitem si ; (rs = si.start(pjbuf,pjlen)) >= 0) ylikely {
	        int	fi = 0 ;
	        cchar	**vpp ;
	        for (cc *tp ; (tp = strnchr(sp,sl,':')) != np ; ) ylikely {
		    cint	tl = intconv(tp - sp) ;
	            int		v = -1 ;
	            char	**sv{} ;
	            vpp = nullptr ;
	            switch (fi++) {
	            case 0:
	                vpp = ccharpp(&pj_name) ;
	                break ;
	            case 1:
	                rs = cfdeci(sp,tl,&v) ;
	                pj_projid = v ;
	                break ;
	            case 2:
	                vpp = ccharpp(&pj_comment) ;
	                break ;
		    case 3:
	                rs = si_storestrs(&si,',',sp,tl,&sv) ;
		        pj_users = sv ;
	                break ;
	            case 4:
	                rs = si_storestrs(&si,',',sp,tl,&sv) ;
		        pj_groups = sv ;
	                break ;
	            case 5:
	                vpp = ccharpp(&pj_attr) ;
	                break ;
	            } /* end switch */
	            if ((rs >= 0) && vpp) {
	                cchar	*cp{} ;
	                if (int cl ; (cl = sfshrink(sp,tl,&cp)) >= 0) {
	                    rs = si.strw(cp,cl,vpp) ;
	                }
	            } /* end if */
	            sl -= intconv((tp + 1) - sp) ;
	            sp = (tp + 1) ;
	            if (rs < 0) break ;
	        } /* end for */
	        if ((rs >= 0) && (fi == 5) && sl && sp[0]) {
	            cchar	*cp{} ;
		    vpp = ccharpp(&pj_attr) ;
		    fi += 1 ;
	            if (int cl ; (cl = sfshrink(sp,sl,&cp)) >= 0) {
	                rs = si.strw(cp,cl,vpp) ;
	            }
	        } /* end if */
	        rs1 = si.finish ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (storeitem) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (ucentpj::parse) */

int ucentpj::load(char *pjbuf,int pjlen,CPJE *cpjp) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (pjbuf && cpjp) ylikely {
	    PROJECT *pep = this ;
	    *pep = *cpjp ; /* shallow copy */
	    if (storeitem si ; (rs = si.start(pjbuf,pjlen)) >= 0) ylikely {
	        int	n ; /* used-afterwards */
	        void	**ptab{} ; /* used twice below */
	        if (cpjp->pj_users) {
		    n = lenstrarr(cpjp->pj_users) ;
	            if ((rs = si.ptab(n,&ptab)) >= 0) {
	                int	i = 0 ; /* used-afterwards */
	                char	**tab = charpp(ptab) ;
	                pj_users = tab ;
	                while ((rs >= 0) && cpjp->pj_users[i]) {
			    cchar	*un = cpjp->pj_users[i] ;
	                    rs = si_copystr(&si,(tab + i),un) ;
			    i += 1 ;
	                } /* end while */
	                pj_users[i] = nullptr ;
	            } /* end if (storeitem-ptab) */
	        } /* end if (users) */
	        if (cpjp->pj_groups) {
		    n = lenstrarr(cpjp->pj_groups) ;
	            if ((rs = si.ptab(n,&ptab)) >= 0) ylikely {
	                int	i = 0 ; /* used-afterwards */
	                char	**tab = charpp(ptab) ;
	                pj_groups = tab ;
	                while ((rs >= 0) && cpjp->pj_groups[i]) {
			    cchar	*gn = cpjp->pj_groups[i] ;
	                    rs = si_copystr(&si,(tab + i),gn) ;
			    i += 1 ;
	                } /* end for */
	                pj_groups[i] = nullptr ;
	            } /* end if (storeitem-ptab) */
	        } /* end if (groups) */
		if (rs >= 0) ylikely {
	            si_copystr(&si,&pj_name,cpjp->pj_name) ;
	            si_copystr(&si,&pj_comment,cpjp->pj_comment) ;
	            si_copystr(&si,&pj_attr,cpjp->pj_attr) ;
		} /* end if (ok) */
	        rs1 = si.finish ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (storeitem) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (ucentpj::load) */

int ucentpj::format(char *rbuf,int rlen) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (rbuf) ylikely {
	    if (sbuf b ; (rs = b.start(rbuf,rlen)) >= 0) ylikely {
	        for (int i = 0 ; i < 6 ; i += 1) {
	            if (i > 0) rs = b.chr(':') ;
	            if (rs >= 0) {
	                int	v ;
	                switch (i) {
	                case 0:
	                    rs = b.str(pj_name) ;
	                    break ;
	                case 1:
	                    v = pj_projid ;
	                    rs = b.dec(v) ;
	                    break ;
	                case 2:
	                    rs = b.str(pj_comment) ;
	                    break ;
	                case 3:
			    if (pj_users) {
				cchar	**sv = ccharpp(pj_users) ;
			        rs = b.strs(',',sv) ;
			    }
	                    break ;
	                case 4:
			    if (pj_groups) {
				cchar	**sv = ccharpp(pj_groups) ;
			        rs = b.strs(',',sv) ;
			    }
	                    break ;
	                case 5:
	                    rs = b.str(pj_attr) ;
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
} /* end subroutine (ucentpj::format) */

int ucentpj::size() noex {
	int		rs = SR_OK ;
	    int		sz = 1 ;
	    int		i = 0 ;
	    if (pj_name) {
	        sz += (lenstr(pj_name) + 1) ;
	    }
	    if (pj_comment) {
	        sz += (lenstr(pj_comment) + 1) ;
	    }
	    if (pj_attr) {
	        sz += (lenstr(pj_attr) + 1) ;
	    }
	    if (pj_users) {
	        for (i = 0 ; pj_users[i] ; i += 1) {
	            sz += (lenstr(pj_users[i]) + 1) ;
	        } /* end for */
	        sz += ((i+1)*szof(cchar *)) ;
	    } /* end if */
	    if (pj_groups) {
	        for (i = 0 ; pj_groups[i] ; i += 1) {
	            sz += (lenstr(pj_groups[i]) + 1) ;
	        } /* end for */
	        sz += ((i+1)*szof(cchar *)) ;
	    } /* end if */
	    rs = iceil(sz,szof(cchar *)) ;
	return rs ;
} /* end subroutine (ucentpj::size) */

int ucentpj::getent(char *pjbuf,int pjlen) noex {
	return uc_getpjent(this,pjbuf,pjlen) ;
} /* end method */

int ucentpj::getnam(char *pjbuf,int pjlen,cchar *name) noex {
	return uc_getpjnam(this,pjbuf,pjlen,name) ;
} /* end method */

int ucentpj::getpid(char *pjbuf,int pjlen,projid_t pjid) noex {
	return uc_getpjpid(this,pjbuf,pjlen,pjid) ;
} /* end method */

int ucentpj::getdef(char *pjbuf,int pjlen,cchar *name) noex {
	return uc_getpjdef(this,pjbuf,pjlen,name) ;
} /* end method */


/* local subroutines */

local int si_storestrs(SI *sip,int sch,cc *sp,int sl,char ***svp) noex {
	int		rs ;
	int		rs1 ;
	if (vechand u ; (rs = u.start(8,0)) >= 0) ylikely {
	    if ((rs = si_loadstrs(sip,&u,sch,sp,sl)) > 0) ylikely {
	        cint	n = rs ;
	        if (void **ptab{} ; (rs = sip->ptab(n,&ptab)) >= 0) {
		    int		i ; /* used-afterwards */
	            void	*vp{} ;
	            *svp = charpp(ptab) ;
		    for (i = 0 ; u.get(i,&vp) >= 0 ; i += 1) {
	                (*svp)[i] = (char *) vp ;
	            } /* end for */
	            (*svp)[i] = nullptr ;
	        } /* end if (storeitem_ptab) */
	    } else {
	        (*svp) = nullptr ;
	    }
	    rs1 = u.finish ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (vechand) */
	return rs ;
} /* end subroutine (si_storestrs) */

local int si_loadstrs(SI *sip,vechand *ulp,int sch,cc *sp,int sl) noex {
	int		rs = SR_OK ;
	int		c = 0 ;
	for (cchar *tp ; (tp = strnchr(sp,sl,sch)) != nullptr ; ) {
	    if (cint tl = intconv(tp - sp) ; tl > 0) ylikely {
		cchar	*zp{} ;
		if (int zl ; (zl = sfshrink(sp,tl,&zp)) > 0) ylikely {
	            if (cc *cp ; (rs = sip->strw(zp,zl,&cp)) >= 0) {
		        c += 1 ;
		        rs = ulp->add(cp) ;
	            }
		} /* end if (sfshrink) */
	    } /* end if (non-zero) */
	    sl -= intconv((tp + 1) - sp) ;
	    sp = (tp + 1) ;
	    if (rs < 0) break ;
	} /* end for */
	if ((rs >= 0) && sl && sp[0]) {
	    if (cc *cp{} ; (rs = sip->strw(sp,sl,&cp)) >= 0) {
		c += 1 ;
		rs = ulp->add(cp) ;
	    }
	} /* end if */
	return (rs >= 0) ? c : rs ;
} /* end subroutine (si_loadstrs) */


