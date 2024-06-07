/* getsysname SUPPORT */
/* lang=C++20 */

/* get the name of the operating system */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-07-01, David AÂ­DÂ­ Morano
	This subroutine was originally written.

*/

/* Copyright © 1998,2018 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	getsysname

	Descriptor:
	This subroutine retrieves the name of the operating system.

	Synopsis:
	int getsysname(char *rbuf,int rlen) noex

	Arguments:
	rbuf		result buffer pointer
	rlen		result buffer length

	Returns:
	>=0		length of returned string
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>		/* |getenv(3c)| */
#include	<cstring>
#include	<usystem.h>
#include	<uvariables.hh>
#include	<uinfo.h>
#include	<sfx.h>
#include	<snwcpy.h>
#include	<sncpyx.h>
#include	<sncpyxw.h>
#include	<localmisc.h>

#include	"getsysname.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct namer ;
    typedef int (namer::*namer_m)() noex ;
    struct namer {
	char	*rbuf ;
	int	rlen ;
	namer(char *b,int l) noex : rbuf(b), rlen(l) { } ;
	operator int () noex ;
	int tryenv() noex ;
	int trysys() noex ;
    } ;
}


/* forward references */


/* local variables */

constexpr namer_m	mems[] = {
	&namer::tryenv,
	&namer::trysys
} ;


/* exported variables */


/* exported subroutines */

int getsysname(char *rbuf,int rlen) noex {
	int		rs = SR_FAULT ;
	if (rbuf) {
	    rs = SR_INVALID ;
	    if (rlen >= 0) {
	        namer	no(rbuf,rlen) ;
	        rs = no ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (getsysname) */


/* local subroutines */

namer::operator int () noex {
	int		rs = SR_OK ;
	for (auto &m : mems) {
	    rs = (this->*m)() ;
	    if (rs != 0) break ;
	} /* end for */
	return rs ;
}
/* end method (namer::operator) */

int namer::tryenv() noex {
	static cchar	*val = getenv(varname.sysname) ;
	int		rs = SR_OK ;
	int		len = 0 ;
	if (val) {
	    cchar	*cp ;
	    if (int cl ; (cl = sfshrink(val,-1,&cp)) > 0) {
		rs = snwcpy(rbuf,rlen,cp,cl) ;
		len = rs ;
	    }
	}
	return (rs >= 0) ? len : rs ;
}
/* end method (namer::tryenv) */

int namer::trysys() noex {
	uinfo_names	names ;
	int		rs ;
	int		len = 0 ;
	if ((rs = uinfo_name(&names)) >= 0) {
	    rs = sncpy(rbuf,rlen,names.sysname) ;
	    len = rs ;
	} /* end if (uinfo) */
	return (rs >= 0) ? len : rs ;
}
/* end method (namer::trysys) */


