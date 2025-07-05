/* vecstrx_env SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++23 */

/* environment-type string handling */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-01, David A­D­ Morano
	This module was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	vecstrx_envadd
	vecstrx_envadds
	vecstrx_envset
	vecstrx_envget

	Description:
	This subroutine extends the VECSTR object to add a key-value
	pair to the vector list. Key-value pairs are two strings
	(the key and the value) joined with an intervening equal
	sign character ('='). Duplicate entries are checked for
	(after all, this was originally intended for environment
	variables) and are not added if already present.

	Synopsis:
	int vecstrx_envadd(vecstrx *op,cchar *kp,cchar *vp,int vl) noex
	int vecstrx::envadds(cchar *sp,int sl) noex
	int vecstrx::envset(cchar *kp,cchar *vp,int vl) noex
	int vecstrx::envget(cchar *kp,cchar **rpp) noex

	Arguments:
	op		vecstrx string to add to
	kp		key string pointer
	vp		value string pointer
	bl		value string length

	Returns:
	>=0		OK
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<climits>		/* |INT_MAX| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<usystem.h>
#include	<strn.h>
#include	<strwcpy.h>
#include	<char.h>
#include	<vstrkeycmpx.h>
#include	<localmisc.h>

#include	"vecstrx.hh"

import libutil ;

/* local defines */

#ifndef	KEYBUFLEN
#define	KEYBUFLEN	120
#endif


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

static int	vecstrx_addwithin(vecstrx *,cchar *,int) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int vecstrx::envadd(cchar *kp,cchar *vp,int vl) noex {
    	cnullptr	np{} ;
	cint		rsn = SR_NOTFOUND ;
	int		rs = SR_FAULT ;
	int		i = INT_MAX ;
	if (kp) {
	    auto	vcmp = vstrkeycmp ;
	    if ((rs = finder(kp,vcmp,np)) == rsn) {
	        rs = addkeyval(kp,-1,vp,vl) ;
	        i = rs ;
	    } /* end if (was not found) */
	} /* end if (non-null) */
	return (rs >= 0) ? i : rs ;
}
/* end subroutine (vecstrx_envadd) */

int vecstrx::envadds(cchar *sp,int sl) noex {
	int		rs = SR_FAULT ;
	int		c = 0 ;
	if (sp) {
	    rs = SR_OK ;
	    if (sl < 0) sl = lenstr(sp) ;
	    for (cc *tp ; (tp = strnbrk(sp,sl," \t\r\n,")) != nullptr ; ) {
	        if (cint tl = intconv(tp - sp) ; tl > 0) {
	            rs = vecstrx_addwithin(this,sp,tl) ;
	            if (rs > 0) c += 1 ;
	        } /* end if */
	        sl -= intconv((tp + 1) - sp) ;
	        sp = (tp + 1) ;
	        if (rs < 0) break ;
	    } /* end for */
	    if ((rs >= 0) && (sl > 0)) {
	        rs = vecstrx_addwithin(this,sp,sl) ;
	        if (rs > 0) c += 1 ;
	    } /* end if */
	} /* end if (non-null) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (vecstrx_envadds) */

int vecstrx::envset(cchar *kp,cchar *vp,int vl) noex {
    	cnullptr	np{} ;
	int		rs = SR_FAULT ;
	if (kp) {
	    auto	vcmp = vstrkeycmp ;
	    if ((rs = finder(kp,vcmp,np)) >= 0) {
	        del(rs) ;
	    } else if (rs == SR_NOTFOUND) {
	        rs = SR_OK ;
	    }
	    if (rs >= 0) {
		rs = addkeyval(kp,-1,vp,vl) ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (vecstrx_envset) */

int vecstrx::envget(cchar *kp,cchar **rpp) noex {
	int		rs = SR_FAULT ;
	int		rl = 0 ;
	if (kp) {
	    cchar	*rp = nullptr ;
	    if (rpp) *rpp = nullptr ;
	    if (cchar *ep ; (rs = finder(kp,vstrkeycmp,&ep)) >= 0) {
	        if (cchar *tp ; (tp = strchr(ep,'=')) != nullptr) {
	            rp = (tp + 1) ;
	            rl = lenstr(tp + 1) ;
	        } else {
	            rp = (ep + lenstr(ep)) ; /* <- NUL character */
		}
	    } /* end if (finder) */
	    if (rpp) *rpp = rp ;
	} /* end if (non-null) */
	return (rs >= 0) ? rl : rs ;
}
/* end subroutine (vecstrx_envget) */


/* private subroutines */

static int vecstrx_addwithin(vecstrx *op,cchar *sp,int sl) noex {
    	cnullptr	np{} ;
	int		rs = SR_OK ;
	int		f_added = false ;
	if (sl < 0) sl = lenstr(sp) ;
	while ((sl > 0) && CHAR_ISWHITE(*sp)) {
	    sp += 1 ;
	    sl -= 1 ;
	}
	while ((sl > 0) && CHAR_ISWHITE(sp[sl - 1])) {
	    sl -= 1 ;
	}
	if (sl > 0) {
	    int		kl = sl ;
	    int		vl = 0 ;
	    bool	f_separated = false ;
	    cchar	*kp = sp ;
	    cchar	*vp = nullptr ;
	    if (cchar *tp ; (tp = strnchr(sp,sl,'=')) != nullptr) {
	        vp = tp + 1 ;
	        vl = intconv((kp + kl) - vp) ;
	        kl = intconv(tp - kp) ;
	        while ((kl > 0) && CHAR_ISWHITE(kp[kl - 1])) {
	            f_separated = true ;
	            kl -= 1 ;
	        }
	        while ((vl > 0) && CHAR_ISWHITE(*vp)) {
	            f_separated = true ;
	            vp += 1 ;
	            vl -= 1 ;
	        }
	        if (kl > 0) {
	            bool	f = false ;
	            char	keybuf[KEYBUFLEN + 1] ;
	            f = f || (! f_separated) ;
		    f = f || (kp[kl] == '\0') ;
		    f = f || (kp[kl] == '=') ;
	            if (! f) {
	                if (kl <= KEYBUFLEN) {
	                    strwcpy(keybuf,kp,kl) ;
	                    kp = keybuf ;
	                } else {
	                    rs = SR_TOOBIG ;
			}
	            } /* end if */
	            if (rs >= 0) {
		        cint	rsn = SR_NOTFOUND ;
			auto	vs = vstrkeycmp ;
	                if ((rs = op->finder(kp,vs,np)) == rsn) {
	                    f_added = true ;
	                    if (f_separated) {
	                        rs = op->addkeyval(kp,kl,vp,vl) ;
	                    } else {
	                        rs = op->add(sp,sl) ;
			    }
	                } /* end if (was not found) */
	            } /* end if (ok) */
	        } /* end if (non-zero key) */
	    } /* end if (had a value even if NUL) */
	} /* end if (non-zero string) */
	return (rs >= 0) ? f_added : rs ;
}
/* end subroutine (vecstrx_addwithin) */


