/* ng SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* search (and other things) a newsgroup list for a newsgroup name */
/* version %I% last-modified %G% */

#define	CF_SAFE		1

/* revision history:

	= 1995-05-01, David A­D­ Morano
	This code module was completely rewritten to replace any
	original garbage that was here before.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	ng

	Description:
	This little object just keeps track of a list of newsgroup names.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<unistd.h>
#include	<strings.h>		/* for |strcasecmp(3c)| */
#include	<climits>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<usystem.h>
#include	<vecitem.h>
#include	<ema.h>
#include	<sfx.h>
#include	<localmisc.h>

#include	"ng.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

extern "C" {
	int	ng_add(NG *,cchar *,int,cchar *) noex ;
}


/* local variables */


/* exported variables */


/* exported subroutines */

int ng_start(NG *ngp) noex {
	int		rs = SR_FAULT ;
	if (ngp) {
	    rs = vecitem_start(ngp,10,0) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (ng_start) */

int ng_finish(NG *ngp) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (ngp) {
	    void *vp{} ;
	    rs = SR_OK ;
	    for (int i = 0 ; vecitem_get(ngp,i,&vp) >= 0 ; i += 1) {
	        ng_ent	*ep = (ng_ent *) vp ;
	        if (vp) {
	            if (ep->name) {
	                rs1 = uc_free(ep->name) ;
		        if (rs >= 0) rs = rs1 ;
		        ep->name = NULL ;
	            }
	            if (ep->dir) {
	                rs1 = uc_free(ep->dir) ;
		        if (rs >= 0) rs = rs1 ;
		        ep->dir = NULL ;
	            }
	        }
	    } /* end for */
	    {
	        rs1 = vecitem_finish(ngp) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (mg_finish) */

int ng_search(NG *ngp,cchar *name,ng_ent **rpp) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		i = 0 ;
	if (rpp) *rpp = nullptr ;
	if (ngp) {
	    void *vp{} ;
	    for (int i = 0 ; (rs1 = vecitem_get(ngp,i,&vp)) >= 0 ; i += 1) {
	        ng_ent	*ep = (ng_ent *) vp ;
	        if (vp) {
	            if (strcasecmp(name,ep->name) == 0) {
			if (rpp) *rpp = ep ;
		        break ;
		    }
	        }
	    } /* end for */
	} /* end if (non-null) */
	return (rs >= 0) ? i : rs ;
}
/* end subroutine (ng_search) */

int ng_add(NG *ngp,cchar *ngbuf,int nglen,cchar *ngdname) noex {
	int		rs ;
	cchar	*cp ;

#if	CF_SAFE
	if (ngp == NULL) return SR_FAULT ;
#endif

	if ((rs = uc_mallocstrw(ngbuf,nglen,&cp)) >= 0) {
	    ng_ent	ne{} ;
	    ne.dir = NULL ;
	    ne.len = (rs-1) ;
	    ne.name = cp ;
	    if (ngdname != NULL) {
		cchar	*dp ;
		if ((rs = uc_mallocstrw(ngdname,-1,&dp)) >= 0) {
		    ne.dir = dp ;
		}
	    } /* end if (had directory) */
	    if (rs >= 0) {
		cint	nsz = szof(ng_ent) ;
	        rs = vecitem_add(ngp,&ne,nsz) ;
	    }
	    if (rs < 0) {
		if (ne.dir != NULL) uc_free(ne.dir) ;
		uc_free(cp) ;
	    }
	} /* end if (m-a) */

	return rs ;
}
/* end subroutine (ng_add) */

int ng_copy(NG *ngp1,NG *ngp2) noex {
	ng_ent		*ep ;
	int		rs = SR_OK ;
	int		i ;
	int		count = 0 ;

#if	CF_SAFE
	if ((ngp1 == NULL) || (ngp2 == NULL)) return SR_FAULT ;
#endif

	for (i = 0 ; vecitem_get(ngp2,i,&ep) >= 0 ; i += 1) {
	    if (ep != NULL) {
	        count += 1 ;
	        rs = ng_add(ngp1,ep->name,ep->len,ep->dir) ;
	    }
	    if (rs < 0) break ;
	} /* end if */

	return (rs >= 0) ? count : rs ;
}
/* end subroutine (ng_copy) */

int ng_count(NG *ngp) noex {
	int		rs ;

#if	CF_SAFE
	if (ngp == NULL) return SR_FAULT ;
#endif

	rs = vecitem_count(ngp) ;

	return rs ;
}
/* end subroutine (ng_count) */

int ng_get(NG *ngp,int i,ng_ent **rpp) noex {
	int		rs ;

#if	CF_SAFE
	if (ngp == NULL) return SR_FAULT ;
#endif

	rs = vecitem_get(ngp,i,rpp) ;

	return rs ;
}
/* end subroutine (ng_get) */

/* extract newsgroup names from the "newsgroups" header string */
int ng_addparse(NG *ngp,cchar *sp,int sl) noex {
	ema		aid ;
	ema_ent		*ep ;
	int		rs ;
	int		rs1 ;
	int		n = 0 ;

#if	CF_SAFE
	if (ngp == NULL) return SR_FAULT ;
#endif

	if (sl < 0) sl = strlen(sp) ;

	if ((rs = ema_start(&aid)) >= 0) {
	    if ((rs = ema_parse(&aid,sp,sl)) > 0) {
	        for (int i = 0 ; ema_get(&aid,i,&ep) >= 0 ; i += 1) {
	            if ((ep != NULL) && (! ep->f.error)) {
	                int	sl = 0 ;
	                cchar	*sp = NULL ;

	            if ((ep->rp != NULL) && (ep->rl > 0)) {
	                sp = ep->rp ;
	                sl = ep->rl ;
	            } else if ((ep->ap != NULL) && (ep->al > 0)) {
	                sp = ep->ap ;
	                sl = ep->al ;
	            }

		    if (sp != NULL) {
			int	cl ;
			cchar	*cp ;
	                if ((cl = sfshrink(sp,sl,&cp)) > 0) {
	                    rs = ng_add(ngp,cp,cl,NULL) ;
			    if (rs < INT_MAX) n += 1 ;
			}
	            } /* end if (had something) */

		    }
	            if (rs < 0) break ;
	        } /* end for */

	    } /* end if (parse) */
	    rs1 = ema_finish(&aid) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (ema) */

	return (rs >= 0) ? n : rs ;
}
/* end subroutine (ng_addparse) */

int ng_parse(NG *ngp,cchar *sp,int sl) noex {
	int		rs = SR_FAULT ;
	if (ngp) {
	    rs = ng_addparse(ngp,sp,sl) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (ng_parse) */


