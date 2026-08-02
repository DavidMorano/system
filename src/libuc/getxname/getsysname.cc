/* getsysname SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* get the name of the operating system */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-07-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	getsysname

	Description:
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
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<ucinfo.h>		/* LIBUC */
#include	<sfx.h>			/* LIBUC */
#include	<snwcpy.h>		/* LIBUC */
#include	<sncpyx.h>		/* LIBUC */
#include	<sncpyxw.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */

#include	"getsysname.h"

#pragma		GCC dependency		"mod/uconstants.ccm"

import uconstants ;			/* |varname(3u)| */

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
    } ; /* end struct (names) */
} /* end namespace */


/* forward references */


/* local variables */

constexpr namer_m	tries[] = {
	&namer::tryenv,
	&namer::trysys
} ; /* end array (tries) */


/* exported variables */


/* exported subroutines */

int getsysname(char *rbuf,int rlen) noex {
	int		rs = SR_FAULT ;
	if (rbuf) ylikely {
	    rs = SR_INVALID ;
	    if (rlen >= 0) ylikely {
	        namer nao(rbuf,rlen) ;
	        rs = nao ;
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (getsysname) */


/* local subroutines */

namer::operator int () noex {
	int		rs = SR_OK ;
	for (cauto &m : tries) {
	    rs = (this->*m)() ;
	    if (rs != 0) break ;
	} /* end for */
	return rs ;
} /* end method (namer::operator) */

int namer::tryenv() noex {
	static cchar	*valp = getenver(varname.sysname) ;
	int		rs = SR_OK ;
	int		len = 0 ;
	if (valp) ylikely {
	    cchar	*cp ;
	    if (int cl ; (cl = sfshrink(valp,-1,&cp)) > 0) ylikely {
		rs = snwcpy(rbuf,rlen,cp,cl) ;
		len = rs ;
	    }
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
} /* end method (namer::tryenv) */

int namer::trysys() noex {
	int		rs ;
	int		len = 0 ;
	if (ucinfo_names names ; (rs = ucinfo_name(&names)) >= 0) ylikely {
	    rs = sncpy(rbuf,rlen,names.sysname) ;
	    len = rs ;
	} /* end if (ucinfo) */
	return (rs >= 0) ? len : rs ;
} /* end method (namer::trysys) */


