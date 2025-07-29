/* linefilter SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* filter a string of text against some criteria */
/* version %I% last-modified %G% */


/* revision history:

	= 2009-04-10, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 2009 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	linefilter

	Description:
	This object module filters a line of text against one or
	both of a "select" list and an "exclude" list.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<usystem.h>
#include	<vecstr.h>
#include	<ascii.h>
#include	<sfx.h>
#include	<six.h>			/* |sisub(3uc)| */
#include	<localmisc.h>

#include	"linefilter.h"

#pragma		GCC dependency	"mod/libutil.ccm"

import libutil ;

/* local defines */


/* imported namespaces */

using std::nullptr_t ;			/* type */
using std::min ;			/* subroutine-template */
using std::max ;			/* subroutine-template */
using std::nothrow ;			/* constant */


/* local typedefs */


/* local defines */

#define	DEFENTS		10


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

template<typename ... Args>
static int linefilter_ctor(linefilter *op,Args ... args) noex {
    	LINEFILTER	*hop = op ;
	cnullptr	np{} ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = SR_NOMEM ;
	    memclear(hop) ;
	    if ((op->sslp = new(nothrow) vecstr) != np) {
	        if ((op->sxlp = new(nothrow) vecstr) != np) {
		    rs = SR_OK ;
		} /* end if (new-vector) */
		if (rs < 0) {
		    delete op->sslp ;
		    op->sslp = nullptr ;
		}
	    } /* end if (new-vecstr) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (linefilter_ctor) */

static int linefilter_dtor(linefilter *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	    if (op->sxlp) {
		delete op->sxlp ;
		op->sxlp = nullptr ;
	    }
	    if (op->sslp) {
		delete op->sslp ;
		op->sslp = nullptr ;
	    }
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (linefilter_dtor) */


/* local variables */


/* exported variables */


/* exported subroutines */

int linefilter_start(linefilter *op,cchar *ssfname,cchar *sxfname) noex {
	int		rs ;
	if ((rs = linefilter_ctor(op)) >= 0) {
	    cint	vn = DEFENTS ;
	    cint	vo = 0 ;
	    if ((ssfname != nullptr) && (ssfname[0] != '\0')) {
	        op->fl.sslist = true ;
	        if ((rs = vecstr_start(op->sslp,vn,vo)) >= 0) {
	            rs = vecstr_loadfile(op->sslp,true,ssfname) ;
	            if (rs < 0) {
	                vecstr_finish(op->sslp) ;
		    }
	        }
	    }
	    if (rs >= 0) {
	        if ((sxfname != nullptr) && (sxfname[0] != '\0')) {
	            op->fl.sxlist = true ;
	            if ((rs = vecstr_start(op->sxlp,vn,vo)) >= 0) {
	                rs = vecstr_loadfile(op->sxlp,true,sxfname) ;
	                if (rs < 0) {
	                    vecstr_finish(op->sxlp) ;
		        }
	            }
	        }
	        if (rs < 0) {
	            vecstr_finish(op->sslp) ;
	        }
	    } /* end if (ok) */
	    if (rs < 0) {
		linefilter_dtor(op) ;
	    }
	} /* end if (linefilter_ctor) */
	return rs ;
}
/* end subroutine (linefilter_start) */

int linefilter_finish(linefilter *op) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (op) {
	    rs = SR_OK ;
	    if (op->fl.sxlist) {
	        op->fl.sxlist = false ;
	        rs1 = vecstr_finish(op->sxlp) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    if (op->fl.sslist) {
	        op->fl.sslist = false ;
	        rs1 = vecstr_finish(op->sslp) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
		rs = linefilter_dtor(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (linefilter_finish) */

int linefilter_check(linefilter *op,cchar *lbuf,int llen) noex {
	int		rs = SR_FAULT ;
	int		f = true ; /* return-value */
	if (op && lbuf) {
	    int		si ;	/* used-multiple */
	    cchar	*cp ;	/* used-multiple */
	    rs = SR_OK ;
	    if (op->fl.sslist) {
		vecstr	*sslp = op->sslp ;
	        f = false ;
	        for (int i = 0 ; vecstr_get(sslp,i,&cp) >= 0 ; i += 1) {
	            if (cp) {
	                if ((si = sisub(lbuf,llen,cp)) >= 0) {
	                    f = true ;
	                    break ;
	                }
		    }
	        } /* end for */
	    } /* end if */
	    if (f && op->fl.sxlist) {
		vecstr	*sxlp = op->sxlp ;
	        for (int i = 0 ; vecstr_get(sxlp,i,&cp) >= 0 ; i += 1) {
	            if (cp) {
	                if ((si = sisub(lbuf,llen,cp)) >= 0) {
	                    f = false ;
	                    break ;
	                }
		    }
	        } /* end for */
	    } /* end if */
	} /* end if (non-null) */
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (linefilter_check) */


/* local subroutines */

int linefilter::start(cchar *ssfname,cchar *sxfname) noex {
	return linefilter_start(this,ssfname,sxfname) ;
}

int linefilter::check(cchar *lbuf,int llen) noex {
	return linefilter_check(this,lbuf,llen) ;
}

void linefilter::dtor() noex {
	if (cint rs = finish ; rs < 0) {
	    ulogerror("linefilter",rs,"fini-finish") ;
	}
} /* end method (linefilter_co::dtor) */

linefilter_co::operator int () noex {
	int		rs = SR_BUGCHECK ;
	if (op) {
	    switch (w) {
	    case linefiltermem_finish:
	        rs = linefilter_finish(op) ;
	        break ;
	    } /* end switch */
	} /* end if (non-null) */
	return rs ;
} /* end method (linefilter_co::operator) */


