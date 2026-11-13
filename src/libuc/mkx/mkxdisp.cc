/* mkxdisp SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* qualify an X display name with its domain-name */
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
	specification with the local hostname when 
	= it is not given at all!
	= it is an implied local name and the target is
	  in another domain!

	Synopsis:
	int mkxsdisp(char *rbuf,int rlen,cc *disp,cc *node,cc *dom) noex

	Arguments:
	rbuf		result buffer pointer
	rlenf		result buffer length
	disp		current display string
	node		current nodename
	dom		current domainname

	Returns:
	>=0	length of the created DISPLAY string specification
	<0	error (system-return)

	Notes:
	1. A valid X-Window display looks like:
		[hostname]:[:]num[:screen]
	where:
		hostname	- a hostname
		num		- a number
		screen		- a screen number

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<algorithm>		/* C++STD |min(3c++)| + |max(3c++)| */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<strwcpy.h>		/* LIBUC */
#include	<isch.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */

#include	"mkxdisp.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| */

/* local defines */


/* imported namespaces */

using std::min ;			/* type */
using std::max ;			/* type */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct suber {
	char		*rbuf ;
	cchar		*disp ;
	cchar		*node ;
	cchar		*dom ;
	int		rlen ;
	suber(char *rb,int rl,cc *ad,cc *an,cc *ao) noex : rbuf(rb), rlen(rl) {
	    disp	= ad ;
	    node	= an ;
	    dom		= ao ;
	} ; /* end ctor */
	operator int	() noex ;
	int tryone	() noex ;
	int trytwo	() noex ;
	int trythree	() noex ;
    private:
	int lenr	(cc *) noex ;
    } ; /* end struct (suber) */
    typedef int (suber::*suber_m)() noex ;
} /* end namespace */


/* forward references */

local bool isvalid(cchar *) noex ;
local bool havevar(cchar *v) noex {
    	return (v && v[0]) ;
} /* end subroutine */


/* local variables */

constexpr suber_m	tries[] = {
    	&suber::tryone,
    	&suber::trytwo,
    	&suber::trythree
} ; /* end array (tries) */


/* exported variables */


/* exported subroutines */

int mkxsdisp(char *rbuf,int rlen,cc *disp,cc *node,cc *dom) noex {
    	int		rs = SR_FAULT ;
	int		len = 0 ; /* return-value */
	if (rbuf && disp) ylikely {
	    rs = SR_INVALID ;
	    rbuf[0] = eos ;
	    if ((rlen > 0) && isvalid(disp)) ylikely {
		if (suber so(rbuf,rlen,disp,node,dom) ; (rs = so) >= 0) {
		    len = rs ;
		}
	    } /* end */
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
} /* end subroutine (mkxdisp) */


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
	    if (havevar(dom)) {
	        if (lenr(bp) > 0) *bp++ = '.' ;
	        bp = strwcpy(bp,dom,lenr(bp)) ;
	    } /* end if */
	    bp = strwcpy(bp,disp,lenr(bp)) ;
	    rs = intconv(bp - rbuf) ;
	} /* end if */
    	return rs ;
} /* end method (suber::tryone) */

int suber::trytwo() noex {
    	int		rs = SR_OK ;
	bool		f_remote = havevar(dom) ;
	cchar		*cp = strochr(disp,':') ;
	cchar		*tp = nullptr ;
	if (f_remote) tp = strochr(disp,'.') ;
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
	if (havevar(dom)) ylikely {
	    cchar	*cp = strochr(disp,':') ;
	    char	*bp = rbuf ;
	    cint ol = intconv(cp - disp) ;
	    bp = strwcpy(bp,disp,min(ol,lenr(bp))) ;
	    if (lenr(bp) > 0) *bp++ = '.' ;
	    bp = strwcpy(bp,dom,lenr(bp)) ;
	    bp = strwcpy(bp,cp,lenr(bp)) ;
	    rs = intconv(bp - rbuf) ;
	} /* end if */
    	return rs ;
} /* end method (suber::trythree) */

int suber::lenr(cc *bp) noex {
	return intconv(rlen + rbuf - bp) ;
} /* end method (suber::lenr) */

local bool isvalid(cchar *disp) noex {
    	cnullptr np{} ;
    	bool f = false ;
	if (cc *tp = strochr(disp,':')) {
	    tp += 1 ;
	    if (*tp == ':') {
		tp += 1 ;
		f = ischdig(tp[0]) ;
	    } else {
		f = ischdig(tp[0]) ;
	    }
	} /* end if */
	return f ;
} /* end subroutine (isvalid) */


