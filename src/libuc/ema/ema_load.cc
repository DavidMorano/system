/* ema_load SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* E-Mail Address */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-01, David A­D­ Morano
	This object module was origpinally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	ema

	Description:
	This is an email address handling module object.  It can
	parse out and store hierarchically organized EMAs.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<climits>		/* |UCHAR_MAX| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<usystem.h>
#include	<vechand.h>
#include	<rmx.h>
#include	<char.h>		/* |CHAR_ISWHITE(3uc)| */
#include	<localmisc.h>

#include	"ema.h"

#pragma		GCC dependency		"mod/libutil.ccm"
#pragma		GCC dependency		"ema_entry.ccm"
#pragma		GCC dependency		"ema_asstr.ccm"
#pragma		GCC dependency		"ema_parts.ccm"

import libutil ;			/* |getlenstr(3u)| */
import ema_entry ;
import ema_asstr ;
import ema_parts ;

/* local defines */


/* imported namespaces */

using libuc::libmem ;			/* variable */


/* local typedefs */

typedef ema_ent		ent ;
typedef ema_ent	*	entp ;


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct loader {
	ema	*op ;
	ema	*nlp ;
	parts	*pbp ;
	cchar	*origp ;
	int	origl ;
	loader(ema *eop,cc *ogp,int ogl,parts *dp,ema *lp) noex {
	    op = eop ;
	    origp = ogp ;
	    origl = ogl ;
	    pbp = dp ;
	    nlp = lp ;
	} ;
	operator int () noex ;
	int loads(ema_ent *) noex ;
	int loadparts(ema_ent *) noex ;
	int loadorig(ema_ent *) noex ;
	int loadgroup(ema_ent *) noex ;
	int loadstore(ema_ent *) noex ;
    } ; /* end struct (loader) */
} /* end namespace */


/* forward references */

local int getlen(cchar *sp,int sl) noex {
    	int		rs = SR_FAULT ;
	if (sp) {
    	    if (sl < 0) sl = lenstr(sp) ;
	}
    	return (rs >= 0) ? sl : rs ;
} /* end subroutine (getlen) */


/* local variables */


/* exported variables */


/* exported subroutines */

namespace ema_ns {
    int ema_load(ema *op,cchar *origp,int origl,parts *pbp,ema *nlp) noex {
	int		rs ;
	if ((rs = getlen(origp,origl)) >= 0) {
	    loader ld(op,origp,rs,pbp,nlp) ;
	    rs = ld ;
	} /* end if (valid) */
	return rs ;
    } /* end subroutine (ema_load) */
} /* end namespace (ema_ns) */

loader::operator int () noex {
        cint        sz = szof(ema_ent) ;
	int		rs ;
        if (void *vp ; (rs = libmem.mall(sz,&vp)) >= 0) {
            ema_ent         *ep = entp(vp) ;
            if ((rs = entry_start(ep)) >= 0) {
                rs = loads(ep) ;
                if (rs < 0) {
                    entry_finish(ep) ;
                }
            } /* end if (entry_start) */
            if (rs < 0) {
                libmem.free(ep) ;
            }
        } /* end if (memory-allocation) */
	return rs ;
} /* end subroutine (ema_load) */

local bool ispcs(int ch) noex {
    	ch &= UCHAR_MAX ;
    	return (ch == '~') || (ch == '_') ;
}

int loader::loads(ema_ent *ep) noex {
    	int		rs ;
	if ((rs = loadparts(ep)) >= 0) {
	    if ((rs = loadorig(ep)) >= 0) {
		if ((rs = loadgroup(ep)) >= 0) {
		    rs = loadstore(ep) ;
		}
	    }
	}
        return rs ;
} /* end method (loader::loads) */

int loader::loadparts(ema_ent *ep) noex {
    	int		rs = SR_OK ;
	cchar		*cp ; /* used-multiple */
        for (int i = 0 ; (rs >= 0) && (i < si_overlast) ; i += 1) {
            if (cc *sp ; (rs = pbp->get(i,&sp)) > 0) {
		int sl = rs ;
                switch (i) {
                case si_address:
                    if ((sl >= 1) && ispcs(*sp)) {
                        sp += 1 ;
                        sl -= 1 ;
                        ep->type = ematype_pcs ;
                    } /* end if (PCS list-type) */
                    if ((rs = libmem.strw(sp,sl,&cp)) >= 0) {
                        ep->ap = cp ;
                        ep->al = sl ;
                    }
                    break ;
                case si_route:
                    if ((rs = libmem.strw(sp,sl,&cp)) >= 0) {
                        ep->rp = cp ;
                        ep->rl = sl ;
                    }
                    break ;
                case si_comment:
                    if ((rs = libmem.strw(sp,sl,&cp)) >= 0) {
                        ep->cp = cp ;
                        ep->cl = sl ;
                    }
                    break ;
                } /* end switch */
            } /* end if (parts_get) */
        } /* end for */
	return rs ;
} /* end method (loader::loadparts) */

int loader::loadorig(ema_ent *ep) noex {
    	int		rs = SR_OK ;
        if ((rs >= 0) && (origl > 0)) {
	    if ((rs = rmwht(origp,origl)) > 0) {
                if (cc *cp ; (rs = libmem.strw(origp,rs,&cp)) >= 0) {
                    ep->op = cp ;
                    ep->ol = origl ;
                } /* end if (memory-allocation) */
	    } /* end if (rmwht) */
        } /* end if */
	return rs ;
} /* end method (loader::loadorig) */

int loader::loadgroup(ema_ent *ep) noex {
    	int		rs = SR_OK ;
        if (nlp) {
            ep->listp = nlp ;
            ep->type = ematype_group ;
        }
	return rs ;
} /* end method (loader::loadgroup) */

int loader::loadstore(ema_ent *ep) noex {
    	int		rs ;
	if ((rs = vechand_add(op->elp,ep)) >= 0) {
	    op->n += 1 ;
	}
        return rs ;
} /* end method (loader::loadstore) */


