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
#include	<climits>		/* CSTD |UCHAR_MAX| */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstring>		/* CSTD |strchr(3c)| */
#include	<algorithm>		/* C++STD |sort(3c++)| */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU |u_getenviron(3u)| */
#include	<usupport.h>		/* LIBU */
#include	<ulogerror.h>		/* LIBU */
#include	<umem.hh>		/* LIBU */
#include	<strnul.hh>		/* LIBU (currently unused) */
#include	<localmisc.h>		/* LIBU */

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
using libu::strnchr ;			/* subroutine */
using libu::strkeycmp ;			/* subroutine */
using libu::strnkeycmp ;		/* subroutine */
using libu::umem ;			/* variable */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local strctures */

namespace {
    struct namemgr {
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
		bool f = false ;
		{
		    cuchar uch1 = uchar(*s1) ;
		    cuchar uch2 = uchar(*s2) ;
		    if ((f = (uch1 < uch2)) == false) {
			f = ((uch1 == uch2) && (strkeycmp(s1,s2) < 0)) ;
		    }
		} /* end block */
		return f ;
	    } ; /* end lambda */
	    tabload_x() ;
	    sort(tab,(tab+ne),predf) ;
	} ; /* end method (tabinit) */
	namemgr() = default ;
	int start(con mainv,int) noex ;
	int finish() noex ;
	void dtor() noex ;
	int operator () (con mainv,int) noex ;
	int operator () (cchar *,int) const noex ;
	destruct namemgr() {
	    if (tab) dtor() ;
	} ; /* end destruct */
    } ; /* end struct (namemgr) */
} /* end namespace */

namespace {
    struct subiniter {
	mainv envv ;
	operator int () noex ;
    } ; /* end struct (subiniter) */
} /* end namespace */


/* forward references */

local int rmeq(cchar *kp,int kl) noex {
	if (cchar *tp = strnchr(kp,kl,'=') ; tp) {
	    kl = intconv(tp - kp) ;
	}
	return kl ;
} /* end subroutine (rmeq) */


/* local variables */

static subiniter	sub ;
static namemgr		nametab ;

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
		    if (cint ei = nametab(kp,kl) ; ei >= 0) {
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
} /* end subroutine (getenver) */


/* local subroutines */

int namemgr::start(mainv ev,int n) noex {
    	int	rs = SR_OK ;
	cint	sz = ((n + 1) * szof(ushort)) ;
	envv = ev ;
	if (void *vp ; (rs = umem.mall(sz,&vp)) >= 0) {
	    tab = ushortp(vp) ;
	    ne = n ;
	    tabinit() ;
	} /* end if (memory-acquire) */
	return rs ;
} /* end method (namemgr::start) */

int namemgr::finish() noex {
    	int		rs = SR_OK ;
	int		rs1 ;
	if (tab) {
	    rs1 = umem.free(tab) ;
	    if (rs >= 0) rs = rs1 ;
	    tab = nullptr ;
	    ne = 0 ;
	} /* end if (memory-release) */
	return rs ;
} /* end method (namemgr::finish) */

void namemgr::dtor() noex {
	if (cint rs = finish() ; rs < 0) {
	    ulogerror("getenver",rs,"fini-finish") ;
	}
} /* end method (namemgr::dtor) */

int namemgr::operator () (mainv ev,int n) noex {
	return start(ev,n) ;
} /* end method (namemgr::operator) */

int namemgr::operator () (cchar *sp,int sl) const noex {
	int	ei = -1 ; /* return-value (initially "not-found") */
        cauto predf = [this,sp,sl] (ushort c) noex -> bool {
            cchar *an = envv[c] ;
	    bool f = false ;
	    {
		cuchar uch_a = uchar(an[0]) ;
		cuchar uch_s = uchar(sp[0]) ;
		if ((f = (uch_a < uch_s)) == false) {
		    f = ((uch_a == uch_s) && (strnkeycmp(an,sp,sl) < 0)) ;
		}
	    } /* end block */
            return f ;
        } ; /* end lambda (predf) */
        con ushort *itf = (tab + 0) ;
        con ushort *itl = (tab + ne) ;
        if (cauto it = partition_point(itf,itl,predf) ; it != itl) {
            cauto mat = [this,sp,sl] (int c) noex -> bool {
                cchar *an = envv[c] ;
                return (strnkeycmp(an,sp,sl) == 0) ;
            } ; /* end lambda (mat) */
            if (cint ii = *it ; mat(ii)) {
                ei = ii ;
            } /* end if (got a match) */
        } /* end if */
	return ei ;
} /* end method (namemgr::operator) */

subiniter::operator int () noex {
    	int		rs ;
	if ((rs = u_getenviron(&envv)) >= 0) {
	    rs = nametab(envv,rs) ;
	} /* end if (u_getenviron) */
	return rs ;
} /* end method (subiniter::operator) */


