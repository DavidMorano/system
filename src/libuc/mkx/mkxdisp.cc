/* mkxdisp SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* qualifit a display name with its domain-name */
/* version %I% last-modified %G% */


/* revision history:

	= 1996-05-01, David A­D­ Morano 
	This code was originally written.

*/

/* Copyright © 1996 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	mkxdisp

	Description:
	This subroutine is used to qualify an X window DISPLAY
	specification with the local hostname when :

	= it is not given at all!

	= it is an implied local name and the target is
	  in another domain!

	Arguments:
	- display	current display string
	- node		current nodename
	- domain	current domainname
	- disbuf	buffer to hold the resulting display value
	- dislen	length of user supplied display buffer

	Returns:
	>=0	length of the created DISPLAY string specification
	<0	error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>		/* |strchr(3c)| */
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<strwcpy.h>
#include	<localmisc.h>

#include	"mkxdisp.h"

import libutil ;			/* |lenstr(3u)| */

/* local defines */

#define	LENLRFT		intconv(rlen + rbuf - bp) /* historical */


/* imported namespaces */

using std::min ;			/* type */
using std::max ;			/* type */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct suber {
	char	*rbuf ;
	cchar	*disp ;
	cchar	*node ;
	cchar	*domain ;
	int	rlen ;
	suber(char *rb,int rl,cc *ad,cc *an,cc *ao) noex : rbuf(rb), rlen(rl) {
	    disp = ad ;
	    node = an ;
	    domain = ao ;
	} ; /* end ctor */
	operator int () noex ;
	int tryone() noex ;
	int trytwo() noex ;
	int trythree() noex ;
    private:
	int lenr(cc *) noex ;
    } ; /* end struct (suber) */
    typedef int (suber::*suber_m)() noex ;
} /* end namespace */


/* forward references */

static bool havevar(cchar *v) noex {
    	return (v && v[0]) ;
}


/* local variables */

constexpr suber_m	tries[] = {
    	&suber::tryone,
    	&suber::trytwo,
    	&suber::trythree
} ; /* end array (tries) */


/* exported variables */


/* exported subroutines */

int mkxsdisp(char *rbuf,int rlen,cc *disp,cc *node,cc *domain) noex {
    	int		rs = SR_FAULT ;
	int		len = 0 ; /* return-value */
	if (rbuf && disp) ylikely {
	    rs = SR_INVALID ;
	    rbuf[0] = '\0' ;
	    if ((rlen > 0) && disp[0]) ylikely {
		if (suber so(rbuf,rlen,disp,node,domain) ; (rs = so) >= 0) {
		    len = rs ;
		}
	    }
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (mkxdisp) */


/* private subroutines */

suber::operator int () noex {
    	int		rs = SR_OK ;
	for (cauto &m : tries) {
	    rs = (this->*m)() ;
	    if (rs != 0) break ;
	} /* end for */
    	return rs ;
} /* end method (suber::operator) */

int suber::tryone() noex {
    	int		rs = SR_OK ;
	if (disp[0] == ':') {
	    char	*bp = rbuf ;
	    bp = strwcpy(bp,node,lenr(bp)) ;
	    if (havevar(domain)) {
	        if (lenr(bp) > 0) *bp++ = '.' ;
	        bp = strwcpy(bp,domain,lenr(bp)) ;
	    } /* end if */
	    bp = strwcpy(bp,disp,lenr(bp)) ;
	    rs = intconv(bp - rbuf) ;
	} /* end if */
    	return rs ;
} /* end method (suber::tryone) */

int suber::trytwo() noex {
    	int		rs = SR_OK ;
	bool		f_remote = havevar(domain) ;
	cchar		*cp = strchr(disp,':') ;
	cchar		*tp = nullptr ;
	if (f_remote) tp = strchr(disp,'.') ;
	if (cp && ((! f_remote) || (tp && (cp > tp)))) {
	    cint	len = lenstr(disp) ;
	    char	*bp = rbuf ;
	    bp = strwcpy(bp,disp,min(lenr(bp),len)) ;
	    rs = intconv(bp - rbuf) ;
	} /* end if */
    	return rs ;
} /* end method (suber::trytwo) */

int suber::trythree() noex {
    	int		rs = SR_OK ;
	if (havevar(domain)) ylikely {
	    cchar	*cp = strchr(disp,':') ;
	    char	*bp = rbuf ;
	    cint ol = intconv(cp - disp) ;
	    bp = strwcpy(bp,disp,min(ol,lenr(bp))) ;
	    if (lenr(bp) > 0) *bp++ = '.' ;
	    bp = strwcpy(bp,domain,lenr(bp)) ;
	    bp = strwcpy(bp,cp,lenr(bp)) ;
	    rs = intconv(bp - rbuf) ;
	} /* end if */
    	return rs ;
} /* end method (suber::trythree) */

int suber::lenr(cc *bp) noex {
	return intconv(rlen + rbuf - bp) ;
} /* end method (suber::lenr) */


