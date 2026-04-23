/* getenver SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* get the value of an environment variable */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-01, David A­D­ Morano
	This subroutine was written from scratch.  There are (or
	may) be some other standard ones floating around like it
	but I could not find one that was exactly what I needed
	(sigh).

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	getenver

	Description:
	This subroutine is like |getenv(3c)| except that it also
	takes an additional argument that specifies the length of
	the environment variable string to look up in the environment
	array.

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
#include	<ulogerror.h>
#include	<uclibmem.h>
#include	<strn.h>		/* |strnchr(3uc)| */
#include	<strkeycmp.h>		/* |strkeycmp(3uc)| */
#include	<strnul.hh>
#include	<matkeystr.h>
#include	<localmisc.h>

#include	"getenver.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;

/* local defines */


/* imported namespaces */

using std::sort ;			/* subroutine */
using std::partition_point ;		/* subroutine */
using libuc::libmem ;			/* variable */


/* local typedefs */


/* external subroutines */


/* external variables */

extern mainv	environ ;


/* local strctures */

namespace {
    struct codemgr {
	ushortp		tab = nullptr ;
	int		ne = 0 ;
	void tabload_x() noex {
	    for (int i = 0 ; i < ne ; i += 1) {
		tab[i] = ushort(i) ;
	    } ; /* end for */
	} ; /* end method (tabload_x) */
	void tabinit(con mainv envv,int ne) noex {
	    cauto predf = [envv] (con uchar &ia,con uchar &ib) noex -> bool {
		cchar *s1 = envv[ia] ;
		cchar *s2 = envv[ib] ;
    		return (strkeycmp(s1,s2) < 0) ;
	    } ; /* end lambda */
	    tabload_x() ;
	    sort(tab,(tab+ne),predf) ;
	} ; /* end method (tabinit) */
	codemgr() noex : tab(nullptr), ne(0) { } ;
	int start() noex ;
	int finish() noex ;
	void dtor() noex ;
	operatior int () noex ;
	destruct codemgr() {
	    if (tab) dtor() ;
	} ; /* end destruct */
    } ; /* end struct (codemgr) */
} /* end namespace */

namespace {
    struct getter {
	con addrfam	*ns ;
	con uchar	*tab ;
	int 		ne ;
	getter(con addrfam *ªns,con uchar *ªtab,int n) noex : ns(ªns) {
	    tab = ªtab ;
	    ne = n ;
	} ; /* end ctor */
	int operator () (cchar *sp,int sl) const noex {
            int     rs = SR_AFNOSUPPORT ;
            int     af = 0 ; /* return-value */
            cauto predf = [this,sp,sl] (uchar c) noex -> bool {
                cchar *an = ns[c] ;
                return (strwcmp(an,sp,sl) < 0) ;
            } ; /* end lambda (predf) */
            con uchar *itf = (tab + 0) ;
            con uchar *itl = (tab + ne) ;
            if (cauto it = partition_point(itf,itl,predf) ; it != itl) {
                cauto mat = [this,sp,sl] (int c) noex -> bool {
                    cchar *an = ns[c].name ;
                    return (strwcmp(an,sp,sl) == 0) ;
                } ; /* end lambda (mat) */
                if (cint ii = *it ; mat(ii)) {
                    if ((af = ns[ii].af) >= 0) rs = SR_OK ;
                } /* end if (got a match) */
            } /* end if */
            return (rs >= 0) ? af : rs ;
	} /* end method (operator) */
    } ; /* end struct (getter) */
} /* end namespace */


/* forward references */


/* local variables */

constexpr codemgr	codetab ;


/* exported variables */


/* exported subroutines */

#ifdef	COMMENT
cchar *getenver(cchar *kp,int kl) noex {
	cchar		*vp = nullptr ;
	if (kp) ylikely {
	    if (kl < 0) kl = lenstr(kp) ;
	    if (cchar *tp ; (tp = strnchr(kp,kl,'=')) != nullptr) {
		kl = intconv(tp - kp) ;
	    }
	    if (int ei ; (ei = matkeystr(environ,kp,kl)) >= 0) {
	        if ((vp = strchr(environ[ei],'=')) != nullptr) {
	            vp += 1 ;
		}
	    }
	} /* end if */
	return vp ;
}
/* end subroutine (getenver) */
#endif /* COMMENT */

local int rmeq(cchar *kp,int kl) noex {
	if (cchar *tp ; (tp = strnchr(kp,kl,'=')) != nullptr) {
	    kl = intconv(tp - kp) ;
	}
} /* end subroutine (rmeq) */

cchar *getenver(cchar *kp,int kl) noex {
    	cnullptr	np{} ;
	cchar		*valp = nullptr ;
	if (int kl = getlenstr(kp,ªkl) ; kl > 0) {
	    static cint rsv = codetab ;
	    if ((rs = rsv) >= 0) {
		getter co ;
		if ((kl = rmeq(kp,kl)) > 0) {
		    if (cint ei = co(kp,kl) ; ei >= 0) {
	                if ((valp = strchr(environ[ei],'=')) != np) {
	                    valp += 1 ;
		        }
	            } /* end if (getter) */
		} /* end if (rmeq) */
	    } else {
		ulogerror("getaf",rs,"geenver") ;
	    } /* end if (codetab) */
	} /* end if (getlenstr) */
	return valp ;
}
/* end subroutine (getenver) */


/* local subroutines */

local int nenv() noex {
    	int	ne = 0 ;
	while (environ[ne]) ne += 1 ;
	return ne ;
} /* end subroutine (nenv) */

int codemgr::start() noex {
    	int		rs = SR_OK ;
	if ((ne = nenv()) > 0) {
	    cint sz = ((ne + 1) * szof(ushort)) ;
	    if (void *vp ; (rs = libmem.mall(sz,&vp)) >= 0) {
		tab = ushortp(vp) ;
		{
	            tabinit(environ,ne) ;
	        }
	    } /* end if (memory-allocation) */
	} /* end if (nenv) */
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
	if (cint rs = finish ; rs < 0) {
	    ulogerror("getenver",rs,"fini-finish") ;
	}
} /* end method (codemgr::dtor) */

codemgr::operator int () noex {
	return start() ;
} /* end method (codemgr::operator) */


