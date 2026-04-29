/* getenver SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* get the value of an environment variable */
/* version %I% last-modified %G% */

#define	CF_DEBUG	0		/* debugging */

/* revision history:

	= 1998-02-01, David A­D­ Morano
	This subroutine was written from scratch.  There are (or
	may be) some other ones floating around like it but I could
	not find one that was exactly what I needed (sigh).

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	getenver

	Description:
	This subroutine is like |getenv(3c)| except that it also
	takes an additional argument that specifies the length of
	the environment variable key string to look up in the
	environment array.

	Synposis:
	cchar		*getenver(cchar *np,int nl) noex

	Arguments:
	np		pointer to environment name (key) to lookup
	nl		length of environment name (key) to lookup

	Returns:
	-		pointer to value associated with given name
	nullptr		name not found

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>		/* |strchr(3c)| */
#include	<algorithm>		/* |sort(3c++)| */
#include	<clanguage.h>
#include	<usysbase.h>
#include	<usyscalls.h>		/* |u_getenviron(3u)| */
#include	<ulogerror.h>
#include	<uclibmem.h>
#include	<strn.h>		/* |strnchr(3uc)| */
#include	<strkeycmp.h>		/* |strkeycmp(3uc)| */
#include	<strnul.hh>
#include	<matkeystr.h>
#include	<localmisc.h>

#include	"getenver.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| */

/* local defines */

#ifndef	CF_DEBUG
#define	CF_DEBUG	0		/* debugging */
#endif


/* imported namespaces */

using std::sort ;			/* subroutine-template */
using std::partition_point ;		/* subroutine-template */
using libuc::libmem ;			/* variable */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local strctures */

namespace {
    struct codemgr {
	mainv		envv ;
	ushortp		tab = nullptr ;
	int		ne = 0 ;
	void tabload_x() noex {
	    for (int i = 0 ; i < ne ; i += 1) {
		tab[i] = ushort(i) ;
	    } /* end for */
	} ; /* end method (tabload_x) */
	void tabinit() noex {
	    cauto predf = [this] (con ushort &ia,con ushort &ib) noex -> bool {
		cchar *s1 = envv[ia] ;
		cchar *s2 = envv[ib] ;
    		return (strkeycmp(s1,s2) < 0) ;
	    } ; /* end lambda */
	    tabload_x() ;
	    sort(tab,(tab+ne),predf) ;
	} ; /* end method (tabinit) */
	codemgr() = default ;
	int start(con mainv,int) noex ;
	int finish() noex ;
	void dtor() noex ;
	int operator () (con mainv,int) noex ;
	int operator () (cchar *,int) const noex ;
	destruct codemgr() {
	    if (tab) dtor() ;
	} ; /* end destruct */
    } ; /* end struct (codemgr) */
} /* end namespace */

namespace {
    struct subiniter {
	mainv envv ;
	operator int () noex ;
    } ; /* end struct (subiniter) */
} /* end namespace */


/* forward references */

local int rmeq(cchar *kp,int kl) noex {
	if (cchar *tp ; (tp = strnchr(kp,kl,'=')) != nullptr) {
	    kl = intconv(tp - kp) ;
	}
	return kl ;
} /* end subroutine (rmeq) */


/* local variables */

static subiniter	sub ;
static codemgr		codetab ;

cbool			f_debug = CF_DEBUG ;


/* exported variables */


/* exported subroutines */

cchar *getenver(cchar *kp,int ªkl) noex {
    	cnullptr	np{} ;
	cchar		*valp = nullptr ; /* return-value */
	if (int rs, kl = getlenstr(kp,ªkl) ; kl > 0) {
	    static cint rsv = sub ;
	    if ((rs = rsv) >= 0) {
		if ((kl = rmeq(kp,kl)) > 0) {
		    if (cint ei = codetab(kp,kl) ; ei >= 0) {
	                if ((valp = strchr(sub.envv[ei],'=')) != np) {
	                    valp += 1 ;
		        }
	            } /* end if (getter) */
		} /* end if (rmeq) */
	    } else {
		ulogerror("getenver",rs,"initialization") ;
	    } /* end if (sub) */
	} /* end if (getlenstr) */
	return valp ;
}
/* end subroutine (getenver) */


/* local subroutines */

int codemgr::start(mainv ev,int n) noex {
    	int	rs = SR_OK ;
	cint	sz = ((n + 1) * szof(ushort)) ;
	envv = ev ;
	if (void *vp ; (rs = libmem.mall(sz,&vp)) >= 0) {
	    tab = ushortp(vp) ;
	    ne = n ;
	    tabinit() ;
	} /* end if (memory-allocation) */
	return rs ;
} /* end method (codemgr::start) */

int codemgr::finish() noex {
    	int		rs = SR_OK ;
	int		rs1 ;
	if (tab) {
	    rs1 = libmem.free(tab) ;
	    if (rs >= 0) rs = rs1 ;
	    tab = nullptr ;
	    ne = 0 ;
	}
	return rs ;
} /* end method (codemgr::finish) */

void codemgr::dtor() noex {
	if (cint rs = finish() ; rs < 0) {
	    ulogerror("getenver",rs,"fini-finish") ;
	}
} /* end method (codemgr::dtor) */

int codemgr::operator () (mainv ev,int n) noex {
	return start(ev,n) ;
} /* end method (codemgr::operator) */

int codemgr::operator () (cchar *sp,int sl) const noex {
    int         ei = -1 ; /* return-value (initially indicating "not-found") */
    if (strnul ss(sp,sl) ; ss.fok) {
        cauto predf = [this,&ss] (ushort c) noex -> bool {
            cchar *an = envv[c] ;
            return (strkeycmp(an,ss) < 0) ;
        } ; /* end lambda (predf) */
        con ushort *itf = (tab + 0) ;
        con ushort *itl = (tab + ne) ;
        if (cauto it = partition_point(itf,itl,predf) ; it != itl) {
            cauto mat = [this,&ss] (int c) noex -> bool {
                cchar *an = envv[c] ;
                return (strkeycmp(an,ss) == 0) ;
            } ; /* end lambda (mat) */
            if (cint ii = *it ; mat(ii)) {
                ei = ii ;
            } /* end if (got a match) */
        } /* end if */
    } /* end if (non-null) */
    return ei ;
} /* end method (codemgr::operator) */

subiniter::operator int () noex {
    	int		rs ;
	if ((rs = u_getenviron(&envv)) >= 0) {
	    rs = codetab(envv,rs) ;
	} /* end if (u_getenviron) */
	return rs ;
} /* end method (subiniter::operator) */


