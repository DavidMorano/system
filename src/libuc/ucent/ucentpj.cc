/* ucentpj SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

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
	These subroutines manage some simple tasks for the PROJECT
	object, referenced as 'struct project'. This object is
	defined by UNIX® (really Solaris®) standards.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>		/* |strlen(3cu)| */
#include	<new>			/* |nothrow(3c++)| */
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
#include	<localmisc.h>

#include	"ucentpj.h"
#include	"ucentxx.hh"

import libutil ;

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

static int si_storestrs(SI *,int,cchar *,int,char ***) noex ;
static int si_loadstrs(SI *,vechand *,int,cc *,int) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int ucentpj::parse(char *pjbuf,int pjlen,cchar *sp,int sl) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (this && pjbuf && sp) {
	    PROJECT *pep = this ;
	    if (sl < 0) sl = xstrlen(sp) ;
	    memclear(pep) ;
	    if (storeitem si ; (rs = si.start(pjbuf,pjlen)) >= 0) {
	        int	fi = 0 ;
	        cchar	**vpp ;
	        for (cc *tp ; (tp = strnchr(sp,sl,':')) != nullptr ; ) {
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
	        } /* end while */
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
}
/* end subroutine (ucentpj::parse) */

int ucentpj::load(char *pjbuf,int pjlen,CPJE *cpjp) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (this && pjbuf && cpjp) {
	    PROJECT *pep = this ;
	    *pep = *cpjp ; /* shallow copy */
	    if (storeitem si ; (rs = si.start(pjbuf,pjlen)) >= 0) {
	        int	n ; /* used-afterwards */
	        void	**ptab{} ; /* used twice below */
	        if (cpjp->pj_users) {
	            for (n = 0 ; cpjp->pj_users[n] ; n += 1) ;
	            if ((rs = si.ptab(n,&ptab)) >= 0) {
	                int	i = 0 ;
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
	            for (n = 0 ; cpjp->pj_groups[n] ; n += 1) ;
	            if ((rs = si.ptab(n,&ptab)) >= 0) {
	                int	i = 0 ;
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
		{
	            si_copystr(&si,&pj_name,cpjp->pj_name) ;
	            si_copystr(&si,&pj_comment,cpjp->pj_comment) ;
	            si_copystr(&si,&pj_attr,cpjp->pj_attr) ;
		}
	        rs1 = si.finish ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (storeitem) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (ucentpj::load) */

int ucentpj::format(char *rbuf,int rlen) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (this && rbuf) {
	    if (sbuf b ; (rs = b.start(rbuf,rlen)) >= 0) {
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
}
/* end subroutine (ucentpj::format) */

int ucentpj::size() noex {
	int		rs = SR_FAULT ;
	if (this) {
	    int		sz = 1 ;
	    int		i = 0 ;
	    if (pj_name) {
	        sz += (xstrlen(pj_name) + 1) ;
	    }
	    if (pj_comment) {
	        sz += (xstrlen(pj_comment) + 1) ;
	    }
	    if (pj_attr) {
	        sz += (xstrlen(pj_attr) + 1) ;
	    }
	    if (pj_users) {
	        for (i = 0 ; pj_users[i] ; i += 1) {
	            sz += (xstrlen(pj_users[i]) + 1) ;
	        } /* end for */
	        sz += ((i+1)*szof(cchar *)) ;
	    } /* end if */
	    if (pj_groups) {
	        for (i = 0 ; pj_groups[i] ; i += 1) {
	            sz += (xstrlen(pj_groups[i]) + 1) ;
	        } /* end for */
	        sz += ((i+1)*szof(cchar *)) ;
	    } /* end if */
	    rs = iceil(sz,szof(cchar *)) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (ucentpj::size) */

int ucentpj::getent(char *pjbuf,int pjlen) noex {
	return uc_getpjent(this,pjbuf,pjlen) ;
}

int ucentpj::getnam(char *pjbuf,int pjlen,cchar *name) noex {
	return uc_getpjnam(this,pjbuf,pjlen,name) ;
}

int ucentpj::getpid(char *pjbuf,int pjlen,projid_t pjid) noex {
	return uc_getpjpid(this,pjbuf,pjlen,pjid) ;
}

int ucentpj::getdef(char *pjbuf,int pjlen,cchar *name) noex {
	return uc_getpjdef(this,pjbuf,pjlen,name) ;
}


/* local subroutines */

static int si_storestrs(SI *sip,int sch,cc *sp,int sl,char ***svp) noex {
	int		rs ;
	int		rs1 ;
	if (vechand u ; (rs = u.start(8,0)) >= 0) {
	    if ((rs = si_loadstrs(sip,&u,sch,sp,sl)) > 0) {
	        cint	n = rs ;
	        if (void **ptab{} ; (rs = sip->ptab(n,&ptab)) >= 0) {
		    int		i ; /* used-afterwards */
	            void	*vp{} ;
	            *svp = (char **) ptab ;
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
}
/* end subroutine (si_storestrs) */

static int si_loadstrs(SI *sip,vechand *ulp,int sch,cc *sp,int sl) noex {
	int		rs = SR_OK ;
	int		c = 0 ;
	for (cchar *tp ; (tp = strnchr(sp,sl,sch)) != nullptr ; ) {
	    if (cint tl = intconv(tp - sp) ; tl > 0) {
		cchar	*zp{} ;
		if (int zl ; (zl = sfshrink(sp,tl,&zp)) > 0) {
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
	}
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (si_loadstrs) */


