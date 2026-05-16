/* projectent SUPPORT (obsoleted) */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* subroutines for simple PROJECT object (from UNIX® library-3c) management */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-06-16, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Group:
	projectent

	Descruption:
	These subroutines manage some simple tasks for the PROJECT
	object, referenced as 'struct project'.  This object is
	defined by UNIX® (really Solaris®) standards.

	Notes:
	This group is obsoleted and should be replaced by UCENTPJ.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<usysbase.h>
#include	<usupport.h>
#include	<intceil.h>
#include	<sbuf.h>
#include	<storeitem.h>
#include	<vechand.h>
#include	<cfdec.h>
#include	<strn.h>
#include	<sfx.h>
#include	<localmisc.h>

#include	"projectent.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |memclear(3u)| */

/* local defines */

#define	PJE	PROJECTENT
#define	CPJE	CPROJECTENT
#define	SI	storeitem


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

local int storeitem_storestrs(SI *,int,cchar *,int,char ***) noex ;
local int storeitem_loadstrs(SI *,vechand *,int,cchar *,int) noex ;

local int si_copystr(storeitem *,char **,cchar *) noex ;

local int sbuf_fmtstrs(sbuf *,int,char **) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int projectent_parse(PJE *pjp,char *pjbuf,int pjlen,cchar *sp,int sl) noex {
    	cnullptr	np{} ;
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (pjp && pjbuf && sp) ylikely {
	    storeitem	ib, *ibp = &ib ;
	    if (sl < 0) sl = lenstr(sp) ;
	    memclear(pjp,szof(PJE)) ;
	    if ((rs = storeitem_start(ibp,pjbuf,pjlen)) >= 0) ylikely {
	        int		fi = 0 ;
	        cchar		**vpp ;
	        for (cchar *tp ; (tp = strnchr(sp,sl,':')) != np ; ) {
		    cint	tl = intconv(tp - sp) ;
	            int		v ;
	            char	**sv ;
	            vpp = nullptr ;
	            switch (fi++) {
	            case 0:
	                vpp = ccharpp(&pjp->pj_name) ;
	                break ;
	            case 1:
			{
	                    rs = cfdeci(sp,tl,&v) ;
	                    pjp->pj_projid = v ;
			}
	                break ;
	            case 2:
	                vpp = ccharpp(&pjp->pj_comment) ;
	                break ;
		    case 3:
	                rs = storeitem_storestrs(ibp,',',sp,tl,&sv) ;
		        pjp->pj_users = sv ;
	                break ;
	            case 4:
	                rs = storeitem_storestrs(ibp,',',sp,tl,&sv) ;
		        pjp->pj_groups = sv ;
	                break ;
	            case 5:
	                vpp = ccharpp(&pjp->pj_attr) ;
	                break ;
	            } /* end switch */
	            if ((rs >= 0) && vpp) {
	                cchar	*cp ;
	                if (int cl ; (cl = sfshrink(sp,tl,&cp)) >= 0) {
	                    rs = storeitem_strw(ibp,cp,cl,vpp) ;
	                }
	            } /* end if */
	            sl -= intconv((tp + 1) - sp) ;
	            sp = (tp + 1) ;
	            if (rs < 0) break ;
	        } /* end for */
	        if ((rs >= 0) && (fi == 5) && sl && sp[0]) {
	            cchar	*cp ;
		    vpp = ccharpp(&pjp->pj_attr) ;
		    fi += 1 ;
	            if (int cl ; (cl = sfshrink(sp,sl,&cp)) >= 0) {
	                rs = storeitem_strw(ibp,cp,cl,vpp) ;
	            }
	        } /* end if */
	        rs1 = storeitem_finish(ibp) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (storeitem) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (projectent_parse) */

int projectent_load(PJE *pjp,char *pjbuf,int pjlen,CPJE *spjp) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (pjp && pjbuf && spjp) ylikely {
	    storeitem	ib ;
	    memcopy(pjp,spjp,szof(PJE)) ;
	    if ((rs = storeitem_start(&ib,pjbuf,pjlen)) >= 0) ylikely {
	        int	n ; /* used-afterwards (muliple places) */
	        void	**ptab ;
	        if ((rs >= 0) && spjp->pj_users) {
		    n = lenstrarr(spjp->pj_users) ;
	            if ((rs = storeitem_ptab(&ib,n,&ptab)) >= 0) {
	                int	i = 0 ; /* used-afterwards */
	                char	**tab = charpp(ptab) ;
	                pjp->pj_users = tab ;
	                while ((rs >= 0) && spjp->pj_users[i]) {
			    cchar *un = spjp->pj_users[i] ;
	                    rs = si_copystr(&ib,(tab + i),un) ;
			    i += 1 ;
	                } /* end while */
	                pjp->pj_users[i] = nullptr ;
	            } /* end if (storeitem-ptab) */
	        } /* end if (users) */
	        if ((rs >= 0) && spjp->pj_groups) {
	            n = lenstrarr(spjp->pj_groups) ;
	            if ((rs = storeitem_ptab(&ib,n,&ptab)) >= 0) {
	                int	i = 0 ; /* used-afterwards */
	                char	**tab = charpp(ptab) ;
	                pjp->pj_groups = tab ;
	                while ((rs >= 0) && spjp->pj_groups[i]) {
	                    rs = si_copystr(&ib,(tab + i),spjp->pj_groups[i]) ;
			    i += 1 ;
	                } /* end for */
	                pjp->pj_groups[i] = nullptr ;
	            } /* end if (storeitem-ptab) */
	        } /* end if (groups) */
	        si_copystr(&ib,&pjp->pj_name,spjp->pj_name) ;
	        si_copystr(&ib,&pjp->pj_comment,spjp->pj_comment) ;
	        si_copystr(&ib,&pjp->pj_attr,spjp->pj_attr) ;
	        rs1 = storeitem_finish(&ib) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (sbuf) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (projectent_load) */

int projectent_format(PJE *pjp,char *rbuf,int rlen) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (pjp && rbuf) ylikely {
	    if (sbuf b ; (rs = sbuf_start(&b,rbuf,rlen)) >= 0) ylikely {
	        for (int i = 0 ; i < 6 ; i += 1) {
	            int		v ;
	            if (i > 0) rs = sbuf_char(&b,':') ;
	            if (rs >= 0) {
	                switch (i) {
	                case 0:
	                    rs = sbuf_strw(&b,pjp->pj_name,-1) ;
	                    break ;
	                case 1:
	                    v = pjp->pj_projid ;
	                    rs = sbuf_deci(&b,v) ;
	                    break ;
	                case 2:
	                    rs = sbuf_strw(&b,pjp->pj_comment,-1) ;
	                    break ;
	                case 3:
	                    rs = sbuf_fmtstrs(&b,',',pjp->pj_users) ;
	                    break ;
	                case 4:
	                    rs = sbuf_fmtstrs(&b,',',pjp->pj_groups) ;
	                    break ;
	                case 5:
	                    rs = sbuf_strw(&b,pjp->pj_attr,-1) ;
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
/* end subroutine (projectent_format) */

int projectent_size(CPJE *pjp) noex {
	int		rs = SR_FAULT ;
	if (pjp) ylikely {
	    int		sz = 1 ;
	    int		i = 0 ; /* used-multiple */
	    if (pjp->pj_name) ylikely  {
	        sz += (lenstr(pjp->pj_name)+ 1) ;
	    }
	    if (pjp->pj_comment) {
	        sz += (lenstr(pjp->pj_comment)+ 1) ;
	    }
	    if (pjp->pj_attr) {
	        sz += (lenstr(pjp->pj_attr)+ 1) ;
	    }
	    if (pjp->pj_users) {
	        for (i = 0 ; pjp->pj_users[i] ; i += 1) {
	            sz += (lenstr(pjp->pj_users[i])+ 1) ;
	        } /* end for */
	        sz += ((i + 1) * szof(cchar *)) ;
	    } /* end if */
	    if (pjp->pj_groups) {
	        for (i = 0 ; pjp->pj_groups[i] ; i += 1) {
	            sz += (lenstr(pjp->pj_groups[i])+ 1) ;
	        } /* end for */
	        sz += ((i + 1) * szof(cchar *)) ;
	    } /* end if */
	    rs = iceil(sz,szof(cchar *)) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (projectent_size) */


/* local subroutines */

local int storeitem_storestrs(SI *ibp,int sch,cc *sp,int sl,char ***svp) noex {
	int		rs ;
	int		rs1 ;
	if (vechand u ; (rs = vechand_start(&u,8,0)) >= 0) ylikely {
	    if ((rs = storeitem_loadstrs(ibp,&u,sch,sp,sl)) > 0) ylikely {
	        int	n = rs ;
	        void **ptab ; 
		if ((rs = storeitem_ptab(ibp,n,&ptab)) >= 0) ylikely {
		    int		i ; /* used-afterwards */
	            void	*vp ;
	            *svp = charpp(ptab) ;
		    for (i = 0 ; vechand_get(&u,i,&vp) >= 0 ; i += 1) {
	                (*svp)[i] = charp(vp) ;
	            } /* end for */
	            (*svp)[i] = nullptr ;
	        } /* end if (storeitem_ptab) */
	    } else {
	        (*svp) = nullptr ;
	    }
	    rs1 = vechand_finish(&u) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (vechand) */
	return rs ;
} /* end subroutine (storeitem_storestrs) */

local int storeitem_loadstrs(SI *ibp,vechand *ulp,int sch,cc *sp,int sl) noex {
	int		rs = SR_OK ;
	int		c = 0 ;
	cchar		*cp ;
	for (cchar *tp ; (tp = strnchr(sp,sl,sch)) != nullptr ; ) {
	    if (cint tl = intconv(tp - sp) ; tl > 0) {
	        if ((rs = storeitem_strw(ibp,sp,tl,&cp)) >= 0) ylikely {
		    c += 1 ;
		    rs = vechand_add(ulp,cp) ;
	        }
	    } /* end if (non-zero) */
	    sl -= intconv((tp + 1) - sp) ;
	    sp = (tp + 1) ;
	    if (rs < 0) break ;
	} /* end for */
	if ((rs >= 0) && sl && sp[0]) {
	    if ((rs = storeitem_strw(ibp,sp,sl,&cp)) >= 0) ylikely {
		c += 1 ;
		rs = vechand_add(ulp,cp) ;
	    }
	} /* end if */
	return (rs >= 0) ? c : rs ;
} /* end subroutine (storeitem_loadstrs) */

local int si_copystr(SI *ibp,char **pp,cchar *p1) noex {
	int		rs = SR_OK ;
	cchar		**cpp = ccharpp(pp) ;
	*cpp = nullptr ;
	if (p1) {
	    rs = storeitem_strw(ibp,p1,-1,cpp) ;
	}
	return rs ;
} /* end subroutine (si_copystr) */

local int sbuf_fmtstrs(sbuf *bp,int sch,char **sv) noex {
	int		rs = SR_OK ;
	if (sv) ylikely {
	    for (int i = 0 ; sv[i] ; i += 1) {
	        if (cchar *sp = sv[i] ; sp[0]) {
	            if (i > 0) rs = sbuf_char(bp,sch) ;
	            if (rs >= 0) rs = sbuf_strw(bp,sp,-1) ;
		}
	        if (rs < 0) break ;
	    } /* end for */
	} /* end if (non-null vector) */
	return rs ;
} /* end subroutine (sbuf_fmtstrs) */


