/* setstr SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* set of strings */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-12-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	setstr

	Description:
	This object provides am ordered set of integers. No two
	strings can be the same (desired for these purposes).

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<climits>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<string>
#include	<usystem.h>
#include	<localmisc.h>

#include	"setstr.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |getlenstr(3u)| */

/* local defines */


/* imported namespaces */

using libuc::libmem ;			/* variable */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

static int	setstr_finents(setstr *) noex ;


/* local variables */

const int	rsn = SR_NOTFOUND ;


/* exported variables */


/* exported subroutines */

int setstr_start(setstr *op,int n) noex {
	cint		at = true ;
	return hdb_start(op,n,at,nullptr,nullptr) ;
}
/* end subroutine (setstr_start) */

int setstr_finish(setstr *op) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		c = 0 ;
	if (op) {
	    rs = SR_OK ;
	    {
		rs1 = setstr_finents(op) ;
	        if (rs >= 0) rs = rs1 ;
		c = rs ;
	    }
	    {
	        rs1 = hdb_finish(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	} /* end if (non-null) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (setstr_finish) */

int setstr_already(setstr *op,cchar *sp,int µsl) noex {
    	cnullptr	np{} ;
	int		rs = SR_FAULT ;
	int		f = true ;
	if (op && sp) {
	    if (int sl ; (sl = getlenstr(sp,µsl)) >= 0) {
	       hdb_dat	key ;
	       hdb_dat	val ;
	       key.buf = sp ;
	       key.len = sl ;
	       val.buf = sp ;
	       val.len = sl ;
	       if ((rs = hdb_fetch(op,key,np,&val)) == rsn) {
	           f = false ;
	           rs = SR_OK ;
	       }
	    } /* end if (getlenstr) */
	} /* end if (non-null) */
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (setstr_already) */

int setstr_add(setstr *op,cchar *sp,int µsl) noex {
    	cnullptr	np{} ;
	int		rs = SR_FAULT ;
	int		f = false ;
	if (op && sp) {
	    if (int sl ; (sl = getlenstr(sp,µsl)) >= 0) {
	        hdb_dat	key ;
	        hdb_dat	val ;
	        key.buf = sp ;
	        key.len = sl ;
	        val.buf = sp ;
	        val.len = sl ;
	        if ((rs = hdb_fetch(op,key,np,&val)) == rsn) {
	            if (cchar *asp ; (rs = libmem.strw(sp,sl,&asp)) >= 0) {
		        key.buf = asp ;
		        val.buf = asp ;
	                f = true ;
	                rs = hdb_store(op,key,val) ;
		        if (rs < 0) {
			    void *vp = voidp(asp) ;
		            libmem.free(vp) ;
		        }
	            } /* end if (m-a) */
	        } /* end if (not already present) */
	    } /* end if (getlenstr) */
	} /* end if (non-null) */
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (setstr_add) */

int setstr_del(setstr *op,cchar *sp,int µsl) noex {
    	cnullptr	np{} ;
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		f = false ;
	if (op && sp) {
	    if (int sl ; (sl = getlenstr(sp,µsl)) >= 0) {
	        hdb_dat	key ;
	        hdb_dat	val{} ;
	        key.buf = sp ;
	        key.len = sl ;
	        if ((rs = hdb_fetch(op,key,np,&val)) >= 0) {
	            cchar	*asp = ccharp(val.buf) ;
		    {
	                rs1 = hdb_delkey(op,key) ;
	                if (rs >= 0) rs = rs1 ;
		    }
		    if (asp) {
			void *vp = voidp(asp) ;
	                rs1 = libmem.free(vp) ;
	                if (rs >= 0) rs = rs1 ;
		    }
	            f = true ;
	        } else if (rs == rsn) {
	            rs = SR_OK ;
	        }
	    } /* end if (getlenstr) */
	} /* end if (non-null) */
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (setstr_del) */

int setstr_delall(setstr *op) noex {
    	return hdb_delall(op) ;
}

int setstr_count(setstr *op) noex {
	return hdb_count(op) ;
}
/* end subroutine (setstr_count) */

int setstr_curbegin(setstr *op,setstr_cur *curp) noex {
	return hdb_curbegin(op,curp) ;
}
/* end subroutine (setstr_curbegin) */

int setstr_curend(setstr *op,setstr_cur *curp) noex {
	return hdb_curend(op,curp) ;
}
/* end subroutine (setstr_curend) */

int setstr_curenum(setstr *op,setstr_cur *curp,cchar **rpp) noex {
    	cnullptr	np{} ;
	int		rs = SR_FAULT ;
	int		rl = 0 ;
	if (op && curp && rpp) {
	    hdb_dat	val{} ;
	    if ((rs = hdb_curenum(op,curp,np,&val)) >= 0) {
	        rl = val.len ;
	        if (rpp) {
	            *rpp = charp(val.buf) ;
	        }
	    } else {
	        if (rpp) *rpp = nullptr ;
	    }
	} /* end if (non-null) */
	return (rs >= 0) ? rl : rs ;
}
/* end subroutine (setstr_curenum) */

int setstr::already(cchar *sp,int sl) noex {
	return setstr_already(this,sp,sl) ;
}

int setstr::add(cchar *sp,int sl) noex {
	return setstr_add(this,sp,sl) ;
}

int setstr::del(cchar *sp,int sl) noex {
	return setstr_del(this,sp,sl) ;
}

int setstr::curbegin(setstr_cur *curp) noex {
	return setstr_curbegin(this,curp) ;
}

int setstr::curend(setstr_cur *curp) noex {
	return setstr_curend(this,curp) ;
}

int setstr::curenum(setstr_cur *curp,cchar **rpp) noex {
	return setstr_curenum(this,curp,rpp) ;
}

void setstr::dtor() noex {
	if (cint rs = finish ; rs < 0) {
	    ulogerror("setstr",rs,"fini-finish") ;
	}
} /* end method (setstr::dtor) */

setstr::operator int () noex {
	return hdb_count(this) ;
} /* end method (setstr::dtor) */

int setstr_co::operator () (int a) noex {
	int		rs = SR_BUGCHECK ;
	if (op) {
	    switch (w) {
	    case setstrmem_start:
	        if ((rs = setstr_start(op,a)) >= 0) {
		    op->fopen = true ;
		}
	        break ;
	    case setstrmem_delall:
	        rs = setstr_delall(op) ;
	        break ;
	    case setstrmem_count:
	        rs = setstr_count(op) ;
	        break ;
	    case setstrmem_finish:
	        rs = setstr_finish(op) ;
		op->fopen = false ;
	        break ;
	    } /* end switch */
	} /* end if (non-null) */
	return rs ;
}
/* end method (setstr_co::operator) */


/* local subroutines */

static int setstr_finents(setstr *op) noex {
    	int		rs = SR_OK ;
	int		rs1 ;
	int		rs2 ;
	int		c = 0 ; /* return-value */
	if (hdb_cur cur ; (rs1 = hdb_curbegin(op,&cur)) >= 0) {
	    hdb_dat	key{} ;
	    hdb_dat	val{} ;
	    while ((rs2 = hdb_curenum(op,&cur,&key,&val)) >= 0) {
	        if (key.buf) {
		    c += 1 ;
		    void *vp = voidp(key.buf) ;
	            rs1 = uc_free(vp) ;
	            if (rs >= 0) rs = rs1 ;
	        }
	    } /* end while (enum) */
	    if ((rs >= 0) && (rs2 != rsn)) rs = rs2 ;
	    rs2 = hdb_curend(op,&cur) ;
	    if (rs >= 0) rs = rs2 ;
	} /* end if (cursor) */
	if ((rs >= 0) && (rs1 != rsn)) rs = rs1 ;
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (setstr_finents) */


