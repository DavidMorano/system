/* strfilter SUPPORT */
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
	strfilter

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
#include	<six.h>
#include	<localmisc.h>

#include	"strfilter.h"


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
static int strfilter_ctor(strfilter *op,Args ... args) noex {
    	STRFILTER	*hop = op ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    cnullptr	np{} ;
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
}
/* end subroutine (strfilter_ctor) */

static int strfilter_dtor(strfilter *op) noex {
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
}
/* end subroutine (strfilter_dtor) */


/* local variables */


/* exported variables */


/* exported subroutines */

int strfilter_start(strfilter *op,cchar *ssfname,cchar *sxfname) noex {
	int		rs ;
	if ((rs = strfilter_ctor(op)) >= 0) {
	    cint	vn = DEFENTS ;
	    cint	vo = 0 ;
	    if ((ssfname != nullptr) && (ssfname[0] != '\0')) {
	        op->f.sslist = true ;
	        if ((rs = vecstr_start(op->sslp,vn,vo)) >= 0) {
	            rs = vecstr_loadfile(op->sslp,true,ssfname) ;
	            if (rs < 0) {
	                vecstr_finish(op->sslp) ;
		    }
	        }
	    }
	    if (rs >= 0) {
	        if ((sxfname != nullptr) && (sxfname[0] != '\0')) {
	            op->f.sxlist = true ;
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
		strfilter_dtor(op) ;
	    }
	} /* end if (strfilter_ctor) */
	return rs ;
}
/* end subroutine (strfilter_start) */

int strfilter_finish(strfilter *op) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (op) {
	    rs = SR_OK ;
	    if (op->f.sxlist) {
	        op->f.sxlist = false ;
	        rs1 = vecstr_finish(op->sxlp) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    if (op->f.sslist) {
	        op->f.sslist = false ;
	        rs1 = vecstr_finish(op->sslp) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
		rs = strfilter_dtor(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (strfilter_finish) */

int strfilter_check(strfilter *op,cchar *lbuf,int llen) noex {
	int		rs = SR_FAULT ;
	int		f = true ;
	if (op && lbuf) {
	    int		si ;
	    cchar	*cp ;
	    rs = SR_OK ;
	    if (op->f.sslist) {
		vecstr	*sslp = op->sslp ;
	        f = false ;
	        for (int i = 0 ; vecstr_get(sslp,i,&cp) >= 0 ; i += 1) {
	            if (cp) {
	                if ((si = sisub(lbuf,llen,cp)) >= 0) {
	                    f = true ;
	                    break ;
	                }
		    }
	        }
	    } /* end if */
	    if (f && op->f.sxlist) {
		vecstr	*sxlp = op->sxlp ;
	        for (int i = 0 ; vecstr_get(sxlp,i,&cp) >= 0 ; i += 1) {
	            if (cp) {
	                if ((si = sisub(lbuf,llen,cp)) >= 0) {
	                    f = false ;
	                    break ;
	                }
		    }
	        }
	    } /* end if */
	} /* end if (non-null) */
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (strfilter_check) */


