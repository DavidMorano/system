/* tardir2 MODULE (module-interface-unit) */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* enumerate filenames */
/* version %I% last-modified %G% */


/* revision history:

	= 2017-03-21, David A-D- Morano
	This subroutine was originally written.

*/

/* Copyright © 2017 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Object:
	vecent

	Description:
	This (all of this) is just a helper object for the main
	TARDIR object.  This object really just stores a composite
	value into a regular "vector" type container (for later
	retrieval).

*******************************************************************************/

module ;

#include	<envstandards.h>	/* ordered first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstdio>
#include	<new>			/* |nothrow(3c++)| */
#include	<stdexcept>
#include	<vector>
#include	<string>
#include	<string_view>
#include	<usyscalls.h>		/* |u_stat(3u)| */
#include	<strnul.hh>
#include	<strn.h>
#include	<sfx.h>
#include	<six.h>
#include	<strwcpy.h>
#include	<localmisc.h>
#include	<dprintf.h>		/* debugging */

#pragma		GCC dependency		"mod/libutil.ccm"

module tardir ;

import libutil ;			/* |lenstr(3u)| */

/* local defines */

#ifndef	CF_DEBUG
#define	CF_DEBUG	0		/* debugging */
#endif


/* imported namespaces */

using std::string ;			/* type */
using std::string_view ;		/* type */


/* local typedefs */

typedef	string_view	strview ;


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */

cbool		f_memclear = false ;


/* exported variables */


/* exported subroutines */


/* local subroutines */

int vecent::istart(mainv arr) noex {
    	int		rs = SR_OK ;
	int		c = 0 ;
    	magval = vecent_magicval ;
	if (arr) {
	    rs = load(arr) ;
	    c = rs ;
	} /* end if (non-null) */
    	return (rs >= 0) ? c : rs ;
} /* end method (vecent::istart) */

int vecent::load(mainv arr) noex {
    	int		rs ;
	int		c = 0 ;
	if ((rs = magic(arr)) >= 0) {
	    rs = SR_OK ;
	    for (int i = 0 ; arr[i] ; i += 1) {
		cchar *dn = arr[i] ;
		rs = SR_INVALID ;
		if (dn[0]) {
		    rs = add(dn) ;
		    c += rs ;
		} /* end if (value) */
		if (rs < 0) break ;
	    } /* end for */
	} /* end if (vecent_magic) */
    	return (rs >= 0) ? c : rs ;
} /* end method (vecent::load) */

int vecent::ifinish() noex {
    	int		rs ;
	if ((rs = magic) >= 0) {
    	    magval = 0 ;
	} /* end if (vecent_magic) */
    	return rs ;
} /* end method (vecent::ifinish) */

int vecent::add(cchar *sp,int µsl) noex {
    	int		rs ;
	int		c = 0 ; /* return-value */
	if ((rs = magic(sp)) >= 0) {
	    rs = SR_INVALID ;
	    if (int sl ; (sl = getlenstr(sp,µsl)) > 0) {
		strnul dn(sp,sl) ;
		if (ustat sb ; (rs = u_stat(dn,&sb)) >= 0) {
		    rs = SR_NOTDIR ;
		    if (S_ISDIR(sb.st_mode)) {
			rs = add(&sb,dn) ;
			c = rs ;
		    } /* ed if (is-dir) */
		} /* end if (u_stat) */
	    } /* end if (getlenstr) */
	} /* end if (vecent_magic) */
    	return (rs >= 0) ? c : rs ;
} /* end method (vecent::add) */

int vecent::add(const ustat *sbp,cchar *dnp,int µdnl) noex {
    	int		rs = SR_FAULT ;
	int		c = 0 ;
	{
	    strnul s(dnp,µdnl) ;
	    DPRINTF("ent s=%s\n",ccp(s)) ;
	}
	if (int dnl ; sbp && ((dnl = getlenstr(dnp,µdnl)) >= 0)) {
	    rs = SR_INVALID ;
	    if (dnl > 0) {
	        if (vecent_ent e ; (rs = e.load(sbp,dnp,dnl)) >= 0) {
		    try {
		        push_back(e) ;
		        c = 1 ;
		    } catch (...) {
		        rs = SR_NOMEM ;
		    }
	        } /* end if (vecent_ent) */
	    } /* end if (valid) */
	} /* end if (getlenstr) */
	DPRINTF("ret rs=%d c=%d\n",rs,c) ;
    	return (rs >= 0) ? c : rs ;
} /* end method (cecent::add) */

int vecent::icount() noex {
    	int		rs ;
	int		c = 0 ;
	if ((rs = magic) >= 0) {
	    csize cnt = size() ;
	    c = intconv(cnt) ;
	} /* end if (vecent_magic) */
    	return (rs >= 0) ? c : rs ;
} /* end method (cecent::icount) */

int vecent::iaudit() noex {
    	int		rs ;
	int		c = 0 ;
	if ((rs = magic) >= 0) {
	    csize cnt = size() ;
	    c = intconv(cnt) ;
	} /* end if (vecent_magic) */
    	return (rs >= 0) ? c : rs ;
} /* end method (cecent::iaudit) */

int vecent::get(int idx,ustat *sbp,cchar **rpp) noex {
    	int		rs = SR_INVALID ;
	DPRINTF("ent idx=%d\n",idx) ;
	if (idx >= 0) {
	    csize cnt = size() ;
	    DPRINTF("cnt=%ld\n",cnt) ;
	    if (cint c = intconv(cnt) ; idx < c) {
		rs = SR_OK ;
		try {
	            const vecent_ent &e = at(idx) ;
	            if (sbp) {
			if_constexpr (f_memclear) {
		            memclear(sbp) ;
			}
		        sbp->st_dev = e.dev ;
		        sbp->st_ino = e.ino ;
	            }
	            if (rpp) {
		        cchar *cp = e.dname.c_str() ;
			DPRINTF("dp=%s\n",cp) ;
		        *rpp = cp ;
	            }
		} catch (const std::out_of_range &ex) {
		    (void) ex ;
		    rs = SR_NOTFOUND ;
		}
	    } else {
		rs = SR_NOTFOUND ;
	    }
	} /* end if (valid) */
	DPRINTF("ret rs=%d\n",rs) ;
    	return rs ;
} /* end method (vecent::get) */

void vecent::dtor() noex {
	if (cint rs = finish ; rs < 0) {
	    ulogerror("vecent",rs,"fini-finish") ;
	}
} /* end method (vecent::dtor) */

vecent::operator int () noex {
    	return icount() ;
} /* end method (vecent::operator) */

int vecent_st::operator () (mainv arr) noex {
    	return op->istart(arr) ;
} /* end method (vecent_st::operator) */

vecent_co::operator int () noex {
	int		rs = SR_BUGCHECK ;
	if (op) ylikely {
	    switch (w) {
	    case vecentmem_count:
	        rs = op->icount() ;
	        break ;
	    case vecentmem_audit:
	        rs = op->iaudit() ;
	        break ;
	    case vecentmem_finish:
	        rs = op->ifinish() ;
	        break ;
	    } /* end switch */
	} /* end if (non-null) */
	return rs ;
} /* end method (vecent::operator) */

int vecent_ent::load(const ustat *sbp,cchar *dnp,int µdnl) noex {
    	int		rs = SR_FAULT ;
	if (int dnl ; (dnl = getlenstr(dnp,µdnl)) >= 0) {
	    rs = SR_INVALID ;
	    if (dnl > 0) {
    	        strview dn(dnp,dnl) ;
	        dev = sbp->st_dev ;
	        ino = sbp->st_ino ;
	        dname = dn ;
		rs = SR_OK ;
	    } /* end if (valid) */
	} /* end if (getlentr) */
	return rs ;
} /* end method (vecent_ent::load) */


