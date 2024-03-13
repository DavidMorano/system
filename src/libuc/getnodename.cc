/* getnodename SUPPORT */
/* lang=C++20 */

/* get the node-name of this node */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-07-01, David A­D­ Morano
	This subroutine is an extraction of the corresponding code
	in the |getnodedomain(3dam)| subroutine.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	getnodename

	Description:
	This subroutine retrieves the node-name of the current node.

	Synopsis:
	int getnodename(char *rbuf,int rlen) noex

	Arguments:
	rbuf		result buffer pointer
	rlen		result buffer length (should be NODENAMELEN)

	Returns:
	>=0		length of retrieved nodename
	<0		error (system-return)

	Notes:
	The compiler person can customize how we find the current
	system nodename. Check out the compile-time defines at the
	top of this file. If you have it, generally (we think) using
	UINFO is probably best, because the result is process-wide
	cached. Nevertheless, we always (as it is now) use SI_HOSTNAME
	right now as a first resort (if the system supports that).

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstdlib>		/* <- |getenv(3c)| */
#include	<usystem.h>
#include	<varnames.hh>
#include	<uinfo.h>
#include	<sncpyx.h>
#include	<snwcpy.h>
#include	<rmx.h>			/* <- for |rmchr(3uc)| */
#include	<localmisc.h>


/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct nodeinfo ;
    typedef int (nodeinfo::*nodeinfo_f)() noex ;
    struct nodeinfo {
	char		*nbuf ;		/* user argument */
	int		nlen ;		/* user argument */
	nodeinfo(char *b,int l) noex : nbuf(b), nlen(l) { } ;
	operator int () noex ;
	int env() noex ;
	int uinfo() noex ;
    } ; /* end struct (nodeinfo) */
}


/* forward references */


/* local variables */

static constexpr nodeinfo_f	nodes[] = {
	&nodeinfo::env,
	&nodeinfo::uinfo,
	nullptr
} ;


/* exported variables */


/* exported subroutines */

int getnodename(char *nbuf,int nlen) noex {
	int		rs = SR_FAULT ;
	if (nbuf) {
	    nodeinfo	ni(nbuf,nlen) ;
	    rs = ni ;
	}
	return rs ;
}
/* end subroutine (getnodename) */


/* local subroutines */

nodeinfo::operator int () noex {
	int		rs = SR_OK ;
	for (int i = 0 ; (rs == SR_OK) && nodes[i] ; i += 1) {
	    nodeinfo_f	m = nodes[i] ;
	    rs = (this->*m)() ;
	} /* end for */
	return rs ;
}
/* end method (nodeinfo::operator) */

int nodeinfo::env() noex {
	int		rs = SR_OK ;
	cchar		*vn = varname.node ;
	if (vn) {
	    if (vn[0]) {
		static cchar	*vp = getenv(vn) ;
	 	if (vp) {
		    if (vp[0]) {
			rs = sncpy1(nbuf,nlen,vp) ;
		    } /* end if (non-empty) */
		} /* env if (have) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end method (nodeinfo::env) */

int nodeinfo::uinfo() noex {
	uinfo_names	uin ;
	int		rs ;
	if ((rs = uinfo_name(&uin)) >= 0) {
	    cint	nl = rmchr(uin.nodename,-1,'.') ;
	    rs = snwcpy(nbuf,nlen,uin.nodename,nl) ;
	} /* end if (uinfo_name) */
	return rs ;
}
/* end methode (nodeinfo::uinfo) */


