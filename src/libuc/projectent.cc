/* projectent SUPPORT */
/* lang=C++20 */

/* subroutines for simple PROJECT object (from UNIX® library-3c) management */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-06-16, David A­D­ Morano
	This subroutine was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	These subroutines manage some simple tasks for the PROJECT
	object, referenced as 'struct project'. This object is
	defined by UNIX® (really Solaris®) standards.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<cstring>
#include	<usystem.h>
#include	<usupport.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<storeitem.h>
#include	<sbuf.h>
#include	<vechand.h>
#include	<localmisc.h>

#include	"ucproject.h"
#include	"projectent.h"


/* local defines */

#define	PJE	PROJECTENT
#define	CPJE	CPROJECTENT
#define	SI	storeitem

#ifndef	TYPEDEF_CC
#define	TYPEDEF_CC
typedef const char	cc ;
#endif


/* external subroutines */

extern "C" {
    extern int	sfshrink(cchar *,int,cchar **) noex ;
    extern int	cfdeci(cchar *,int,int *) noex ;
    extern int	iceil(int,int) noex ;
    extern char	*strnchr(const char *,int,int) noex ;
}


/* external variables */


/* local structures */


/* forward references */

static int storeitem_storestrs(SI *,int,cchar *,int,char ***) noex ;
static int storeitem_loadstrs(SI *,vechand *,int,cchar *,int) noex ;

static int si_copystr(storeitem *,char **,cchar *) noex ;

static int sbuf_fmtstrs(sbuf *,int,char **) noex ;


/* local variables */


/* exported subroutines */

int projectent_parse(PJE *pjp,char *pjbuf,int pjlen,cchar *sp,int sl) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (pjp && pjbuf && sp) {
	    storeitem	ib, *ibp = &ib ;
	    if (sl < 0) sl = strlen(sp) ;
	    memclear(pjp,sizeof(PJE)) ;
	    if ((rs = storeitem_start(ibp,pjbuf,pjlen)) >= 0) {
	        int		fi = 0 ;
	        cchar		*tp ;
	        cchar		**vpp ;
	        while ((tp = strnchr(sp,sl,':')) != nullptr) {
	            int		v ;
	            char	**sv ;
	            vpp = nullptr ;
	            switch (fi++) {
	            case 0:
	                vpp = (cchar **) &pjp->pj_name ;
	                break ;
	            case 1:
	                rs = cfdeci(sp,(tp-sp),&v) ;
	                pjp->pj_projid = v ;
	                break ;
	            case 2:
	                vpp = (cchar **) &pjp->pj_comment ;
	                break ;
		    case 3:
	                rs = storeitem_storestrs(ibp,',',sp,(tp-sp),&sv) ;
		        pjp->pj_users = sv ;
	                break ;
	            case 4:
	                rs = storeitem_storestrs(ibp,',',sp,(tp-sp),&sv) ;
		        pjp->pj_groups = sv ;
	                break ;
	            case 5:
	                vpp = (cchar **) &pjp->pj_attr ;
	                break ;
	            } /* end switch */
	            if ((rs >= 0) && vpp) {
	                int	cl ;
	                cchar	*cp ;
	                if ((cl = sfshrink(sp,(tp-sp),&cp)) >= 0) {
	                    rs = storeitem_strw(ibp,cp,cl,vpp) ;
	                }
	            } /* end if */
	            sl -= ((tp+1)-sp) ;
	            sp = (tp+1) ;
	            if (rs < 0) break ;
	        } /* end while */
	        if ((rs >= 0) && (fi == 5) && sl && sp[0]) {
	            int		cl ;
	            cchar	*cp ;
		    vpp = (cchar **) &pjp->pj_attr ;
		    fi += 1 ;
	            if ((cl = sfshrink(sp,sl,&cp)) >= 0) {
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
	if (pjp && pjbuf && spjp) {
	    storeitem	ib ;
	    memcpy(pjp,spjp,sizeof(PJE)) ;
	    if ((rs = storeitem_start(&ib,pjbuf,pjlen)) >= 0) {
	        int	n ;
	        void	**ptab ;
	        if (spjp->pj_users) {
	            for (n = 0 ; spjp->pj_users[n] ; n += 1) ;
	            if ((rs = storeitem_ptab(&ib,n,&ptab)) >= 0) {
	                int	i = 0 ;
	                char	**tab = (char **) ptab ;
	                pjp->pj_users = tab ;
	                while ((rs >= 0) && spjp->pj_users[i]) {
	                    rs = si_copystr(&ib,(tab + i),spjp->pj_users[i]) ;
			    i += 1 ;
	                } /* end while */
	                pjp->pj_users[i] = nullptr ;
	            } /* end if (storeitem-ptab) */
	        } /* end if (users) */
	        if (spjp->pj_groups) {
	            for (n = 0 ; spjp->pj_groups[n] ; n += 1) ;
	            if ((rs = storeitem_ptab(&ib,n,&ptab)) >= 0) {
	                int	i = 0 ;
	                char	**tab = (char **) ptab ;
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
	if (pjp && rbuf) {
	    sbuf	b ;
	    if ((rs = sbuf_start(&b,rbuf,rlen)) >= 0) {
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
	if (pjp) {
	    int		size = 1 ;
	    int		i = 0 ;
	    if (pjp->pj_name) {
	        size += (strlen(pjp->pj_name)+1) ;
	    }
	    if (pjp->pj_comment) {
	        size += (strlen(pjp->pj_comment)+1) ;
	    }
	    if (pjp->pj_attr) {
	        size += (strlen(pjp->pj_attr)+1) ;
	    }
	    if (pjp->pj_users) {
	        for (i = 0 ; pjp->pj_users[i] ; i += 1) {
	            size += (strlen(pjp->pj_users[i])+1) ;
	        } /* end for */
	        size += ((i+1)*sizeof(cchar *)) ;
	    } /* end if */
	    if (pjp->pj_groups) {
	        for (i = 0 ; pjp->pj_groups[i] ; i += 1) {
	            size += (strlen(pjp->pj_groups[i])+1) ;
	        } /* end for */
	        size += ((i+1)*sizeof(cchar *)) ;
	    } /* end if */
	    rs = iceil(size,sizeof(cchar *)) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (projectent_size) */


/* local subroutines */

static int storeitem_storestrs(SI *ibp,int sch,cc *sp,int sl,char ***svp) noex {
	vechand		u ;
	int		rs ;
	int		rs1 ;
	if ((rs = vechand_start(&u,8,0)) >= 0) {
	    if ((rs = storeitem_loadstrs(ibp,&u,sch,sp,sl)) > 0) {
	        int	n = rs ;
	        void	**ptab ;
	        if ((rs = storeitem_ptab(ibp,n,&ptab)) >= 0) {
		    int		i ;
	            void	*vp ;
	            *svp = (char **) ptab ;
		    for (i = 0 ; vechand_get(&u,i,&vp) >= 0 ; i += 1) {
	                (*svp)[i] = (char *) vp ;
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
}
/* end subroutine (storeitem_storestrs) */

static int storeitem_loadstrs(SI *ibp,vechand *ulp,int sch,cc *sp,int sl) noex {
	int		rs = SR_OK ;
	int		c = 0 ;
	cchar		*tp ;
	cchar		*cp ;
	while ((tp = strnchr(sp,sl,sch)) != nullptr) {
	    if ((tp-sp) > 0) {
	        if ((rs = storeitem_strw(ibp,sp,(tp-sp),&cp)) >= 0) {
		    c += 1 ;
		    rs = vechand_add(ulp,cp) ;
	        }
	    } /* end if (non-zero) */
	    sl -= ((tp+1)-sp) ;
	    sp = (tp+1) ;
	    if (rs < 0) break ;
	} /* end while */
	if ((rs >= 0) && sl && sp[0]) {
	    if ((rs = storeitem_strw(ibp,sp,sl,&cp)) >= 0) {
		c += 1 ;
		rs = vechand_add(ulp,cp) ;
	    }
	}
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (storeitem_loadstrs) */

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

static int sbuf_fmtstrs(sbuf *bp,int sch,char **sv) noex {
	int		rs = SR_OK ;
	if (sv) {
	    for (int i = 0 ; sv[i] ; i += 1) {
	        cchar	*sp = sv[i] ;
		if (sp[0]) {
	            if (i > 0) rs = sbuf_char(bp,sch) ;
	            if (rs >= 0) rs = sbuf_strw(bp,sp,-1) ;
		}
	        if (rs < 0) break ;
	    } /* end for */
	} /* end if (non-null vector) */
	return rs ;
}
/* end subroutine (sbuf_fmtstrs) */


