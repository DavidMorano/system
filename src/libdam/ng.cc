/* ng SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* search (and other things) a newsgroup list for a newsgroup name */
/* version %I% last-modified %G% */


/* revision history:

	= 1995-05-01, David A­D­ Morano
	This code module was completely rewritten to replace any
	original garbage that was here before.

*/

/* Copyright © 1995 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	ng

	Description:
	This little object just keeps track of a list of newsgroup names.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<strings.h>		/* for |strcasecmp(3c)| */
#include	<climits>		/* |INT_MAX| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<usystem.h>
#include	<vecitem.h>
#include	<ema.h>
#include	<sfx.h>
#include	<localmisc.h>

#include	"ng.h"

import libutil ;

/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


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
		        ep->name = nullptr ;
	            }
	            if (ep->dir) {
	                rs1 = uc_free(ep->dir) ;
		        if (rs >= 0) rs = rs1 ;
		        ep->dir = nullptr ;
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
	int		i = 0 ; /* return-value */
	if (rpp) *rpp = nullptr ;
	if (ngp) {
	    void *vp{} ;
	    rs = SR_OK ;
	    for (i = 0 ; (rs = vecitem_get(ngp,i,&vp)) >= 0 ; i += 1) {
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
	int		rs = SR_FAULT ;
	if (ngp && ngbuf) {
	    if (cchar *cp ; (rs = uc_mallocstrw(ngbuf,nglen,&cp)) >= 0) {
	        ng_ent	ne{} ;
	        ne.dir = nullptr ;
	        ne.len = rs ;
	        ne.name = cp ;
	        if (ngdname != nullptr) {
		    if (cchar *dp ; (rs = uc_mallocstrw(ngdname,-1,&dp)) >= 0) {
		        ne.dir = dp ;
		    }
	        } /* end if (had directory) */
	        if (rs >= 0) {
		    cint	nsz = szof(ng_ent) ;
	            rs = vecitem_add(ngp,&ne,nsz) ;
	        }
	        if (rs < 0) {
		    if (ne.dir != nullptr) {
			uc_free(ne.dir) ;
		    }
		    uc_free(cp) ;
	        } /* end if (error handling) */
	    } /* end if (m-a) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (ng_add) */

int ng_copy(NG *ngp1,NG *ngp2) noex {
	int		rs = SR_FAULT ;
	int		count = 0 ;
	if (ngp1 && ngp2) {
	    void	*vp{} ;
	    rs = SR_OK ;
	    for (int i = 0 ; vecitem_get(ngp2,i,&vp) >= 0 ; i += 1) {
	        ng_ent	*ep = (ng_ent *) vp ;
	        if (vp) {
	            count += 1 ;
	            rs = ng_add(ngp1,ep->name,ep->len,ep->dir) ;
	        }
	        if (rs < 0) break ;
	    } /* end if */
	} /* end if (non-null) */
	return (rs >= 0) ? count : rs ;
}
/* end subroutine (ng_copy) */

int ng_count(NG *ngp) noex {
    	int		rs = SR_FAULT ;
	if (ngp) {
	    rs = vecitem_count(ngp) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (ng_count) */

int ng_get(NG *ngp,int i,ng_ent **rpp) noex {
	int		rs = SR_FAULT ;
	if (ngp) {
	    rs = vecitem_get(ngp,i,rpp) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (ng_get) */

/* extract newsgroup names from the "newsgroups" header string */
int ng_addparse(NG *ngp,cchar *sp,int sl) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		n = 0 ; /* return-value */
	if (ngp && sp) {
	    if (sl < 0) sl = lenstr(sp) ;
	    if (ema aid ; (rs = ema_start(&aid)) >= 0) {
	        if ((rs = ema_parse(&aid,sp,sl)) > 0) {
		    ema_ent	*ep ;
	            for (int i = 0 ; ema_get(&aid,i,&ep) >= 0 ; i += 1) {
	                if ((ep != nullptr) && (! ep->fl.error)) {
			    cchar	*rp{} ;
	                    int		rl = 0 ;
	                    if ((ep->rp != nullptr) && (ep->rl > 0)) {
	                        rp = ep->rp ;
	                        rl = ep->rl ;
	                    } else if ((ep->ap != nullptr) && (ep->al > 0)) {
	                        rp = ep->ap ;
	                        rl = ep->al ;
	                    }
		            if (rp) {
				cchar	*cp ;
	                        if (int cl ; (cl = sfshrink(rp,rl,&cp)) > 0) {
	                            rs = ng_add(ngp,cp,cl,nullptr) ;
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
	} /* end if (non-null) */
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


