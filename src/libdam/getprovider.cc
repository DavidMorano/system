/* getprovider SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* Get Provider (manufacturer) */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	getproviderid

	Description:
	This is a small hack to get a number associated with a
	manufacturing provider.  Prividers are identified by a
	c-string (itself provided by the kernel).  This subroutine
	looks this string up in a database (of providers) and 
	returns the corresponding Provider-ID number.

	Synopsis:
	int getproviderid(char *sp,int sl) noex

	Arguments:
	sp		provider string pointer
	sl		provider string length

	Returns:
	>0		provider ID was found and this is it
	0		provider ID was not found
	<0		error (system-return)


  	Name:
	getprovider
	getvendor

	Description:
	Retrieve and return the machine provider c-string (or
	the "realname" of the provider when using |getvendor|).

	Synopsis:
	int getprovider(char *rbuf,int rlen) noex
	int getvendor(char *rbuf,int rlen) noex

	Arguments:
	rbuf		result buffer pointer
	rlen		result buffer length

	Returns:
	>=0		number of characters returned
	<0		error (system-return)


	Variable:
	providerid

	Description:
	This is a global variable that contains a numeric (type |int|)
	value that is the Provider-ID of the current system.

	Synopsis:
	int providerid

	Arguments:
	*none*

	Returns:
	>=0		the Provider-ID of the current system
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>		/* |getenv(3c)| */
#include	<cstring>		/* |strlen(3c)| */
#include	<usystem.h>
#include	<uinfo.h>
#include	<mallocxx.h>
#include	<estrings.h>		/* |sf{x}(3uc)| + |snwcpy(3uc)| */
#include	<nleadstr.h>
#include	<localmisc.h>

#include	"getprovider.h"

import uconstants ;

/* local defines */


/* external subroutines */


/* external variables */


/* local structures */

struct provider {
	uint		id ;
	cchar		*codename ;
	cchar		*realname ;
} ;

namespace {
    struct trier ;
    typedef int (trier::*trier_m)() noex ;
    struct trier {
	char		*rbuf ;
	int		rlen ;
	trier(char *rb,int rl) noex : rbuf(rb), rlen(rl) { 
	    rbuf[0] = '\0' ;
	} ; /* end ctor */
	operator int () noex ;
	int tryenv() noex ;
	int tryinfo() noex ;
	int trydef() noex ;
    } ; /* end struct (trier) */
}


/* forward references */


/* local variables */

constexpr provider	providers[] = {
	{ 0, "unknown", 			"Unknown" }, 
	{ 1, "Sun_Microsystems", 		"Sun Microsystems" },
	{ 2, "Compaq Computer Corporation", 	nullptr },
	{ 3, "sgi", 				"Silicon Graphics" },
	{ 4, nullptr, 				nullptr }
} ;

constexpr trier_m	tries[] = {
    	&trier::tryenv,
	&trier::tryinfo,
	&trier::trydef
} ;


/* exported variables */

libdam::provider		providerid ;


/* exported subroutines */

int getproviderid(cchar *sp,int sl) noex {
    	int		rs = SR_FAULT ;
	int		id = 0 ;
	if (sp) {
	    if (sl < 0) sl = strlen(sp) ;
	    for (int m, i = 0 ; providers[i].codename ; i += 1) {
	        cchar	*bs = providers[i].codename ;
	        m = nleadstr(bs,sp,sl) ;
	        if ((m == sl) && (bs[m] == '\0')) {
	            id = providers[i].id ;
		    break ;
		}
	    } /* end for */
	} /* end if (non-null) */
	return (rs >= 0) ? id : rs ;
}
/* end subroutine (getproviderid) */

int getprovider(char *rbuf,int rlen) noex {
	int		rs = SR_FAULT ;
	if (rbuf) {
	    rs = SR_INVALID ;
	    if (rlen >= 0) {
	        trier	to(rbuf,rlen) ;
	        rs = to ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (getprovider) */

int getvendor(char *rbuf,int rlen) noex {
	int		rs = SR_FAULT ;
	int		rl = 0 ;
	if (rbuf) {
	    rbuf[0] = '\0' ;
	    rs = SR_INVALID ;
	    if (rlen >= 0) {
	        if (char *pbuf{} ; (rs = malloc_mn(&pbuf)) >= 0) {
	            if ((rs = getprovider(pbuf,rs)) >= 0) {
	                cint	pl = rs ;
	                int	i ; /* used-afterwards */
	                int	m ;
	                bool	f = false ;
	                rs = SR_NOTFOUND ;
	                for (i = 0 ; providers[i].codename ; i += 1) {
	                    cchar	*pn = providers[i].codename ;
	                    m = nleadstr(pn,pbuf,pl) ;
	                    f = ((m == pl) && (pn[m] == '\0')) ;
	                    if (f) break ;
	                } /* end for */
	                if (f) {
			    cchar *rn = providers[i].realname ;
			    if (rn == nullptr) {
			       rn = providers[i].codename ;
			    }
		            rs = sncpy(rbuf,rlen,rn) ;
			    rl = rs ;
	                }
	            } /* end if (getprovider) */
		    rs = rsfree(rs,pbuf) ;
	        } /* end if (m-a-f) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? rl : rs ;
}
/* end subroutine (getvendor) */


/* local subroutines */

trier::operator int () noex {
    	int		rs = SR_OK ;
	for (cauto &m : tries) {
	    rs = (this->*m)() ;
	    if (rs != 0) break ;
	} /* end for */
	return rs ;
}
/* end method (trier::operator) */

int trier::tryenv() noex {
	static cchar	*valp = getenv(varname.provider) ;
	int		rs = SR_OK ;
	int		rl = 0 ;
	if (valp) {
	    cchar	*cp ;
	    if (int cl ; (cl = sfshrink(valp,-1,&cp)) > 0) {
	        rs = snwcpy(rbuf,rlen,cp,cl) ;
		rl = rs ;
	    }
	}
	return (rs >= 0) ? rl : rs ;
}
/* end method (trier::tryenv) */

int trier::tryinfo() noex {
    	int		rs ;
	int		rl = 0 ;
	if (uinfo_infoaux aux ; (rs = uinfo_aux(&aux)) >= 0) {
	    if (aux.hwprovider) {
		rs = sncpy(rbuf,rlen,aux.hwprovider) ;
		rl = rs ;
	    }
	} /* end if (uinfo) */
	return (rs >= 0) ? rl : rs ;
}
/* end method (trier::tryinfo) */

int trier::trydef() noex {
	return sncpy(rbuf,rlen,sysword.w_defprovider) ;
}
/* end method (trier::trydef) */

namespace libdam {
    provider::operator int () noex {
	int		rs ;
	int		id = 0 ;
	if (char *pbuf{} ; (rs = malloc_mn(&pbuf)) >= 0) {
	    if ((rs = getprovider(pbuf,rs)) >= 0) {
		rs = getproviderid(pbuf,rs) ;
		id = rs ;
	    }
	    rs = rsfree(rs,pbuf) ;
	} /* end if (m-a-f) */
	return (rs >= 0) ? id : rs ;
    }
}


